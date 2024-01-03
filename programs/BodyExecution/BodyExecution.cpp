// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "BodyExecution.hpp"

#include <cmath> // std::abs

#include <algorithm> // std::max_element, std::transform
#include <vector>

#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>

using namespace roboticslab;

constexpr auto DEFAULT_ROBOT = "/teo"; // teo or teoSim
constexpr auto DEFAULT_PREFIX = "/bodyExecution";
constexpr auto DEFAULT_REF_SPEED = 25.0; // [m/s]
constexpr auto DEFAULT_REF_ACCELERATION = 25.0; // [m/s^2]

constexpr BodyExecution::setpoints_head_t headZeros { 0.0, 0.0 };
constexpr BodyExecution::setpoints_arm_t armZeros { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

bool BodyExecution::configure(yarp::os::ResourceFinder & rf)
{
    auto robot = rf.check("robot", yarp::os::Value(DEFAULT_ROBOT), "remote robot port prefix").asString();

    if (rf.check("help"))
    {
        yInfo("BodyExecution options:");
        yInfo("\t--help (this help)\t--from [file.ini]\t--context [path]");
        yInfo("\t--robot: %s [%s]", robot.c_str(), DEFAULT_ROBOT);
        return false;
    }

    yarp::os::Property robotOptions {
        {"device", yarp::os::Value("remotecontrolboardremapper")},
        {"localPortPrefix", yarp::os::Value(DEFAULT_PREFIX)}
    };

    yarp::os::Bottle remotePorts {
        yarp::os::Value(robot + "/head"),
        yarp::os::Value(robot + "/leftArm"),
        yarp::os::Value(robot + "/rightArm")
    };

    robotOptions.put("remoteControlBoards", yarp::os::Value::makeList(remotePorts.toString().c_str()));

    yarp::os::Bottle axesNames {
        yarp::os::Value("AxialNeck"), yarp::os::Value("FrontalNeck"),
        yarp::os::Value("FrontalLeftShoulder"), yarp::os::Value("SagittalLeftShoulder"), yarp::os::Value("AxialLeftShoulder"),
        yarp::os::Value("FrontalLeftElbow"), yarp::os::Value("AxialLeftWrist"), yarp::os::Value("FrontalLeftWrist"),
        yarp::os::Value("FrontalRightShoulder"), yarp::os::Value("SagittalRightShoulder"), yarp::os::Value("AxialRightShoulder"),
        yarp::os::Value("FrontalRightElbow"), yarp::os::Value("AxialRightWrist"), yarp::os::Value("FrontalRightWrist")
    };

    robotOptions.put("axesNames", yarp::os::Value::makeList(axesNames.toString().c_str()));

    if (!robotDevice.open(robotOptions))
    {
        yError() << "Failed to open robot device";
        return false;
    }

    if (!robotDevice.view(iControlMode) || !robotDevice.view(iEncoders) || !robotDevice.view(iPositionControl))
    {
        yError() << "Failed to view robot interfaces";
        return false;
    }

    if (!iControlMode->setControlModes(std::vector(axesNames.size(), VOCAB_CM_POSITION).data()))
    {
        yError() << "Failed to set position control mode";
        return false;
    }

    if (!iPositionControl->setRefSpeeds(std::vector(axesNames.size(), DEFAULT_REF_SPEED).data()))
    {
        yError() << "Failed to set reference speeds";
        return false;
    }

    if (!iPositionControl->setRefAccelerations(std::vector(axesNames.size(), DEFAULT_REF_ACCELERATION).data()))
    {
        // might not be available in certain implementations, e.g. OpenRAVE
        yWarning() << "Failed to set reference accelerations";
    }

    if (!serverPort.open(DEFAULT_PREFIX + std::string("/rpc:s")))
    {
        yError() << "Unable to open RPC port";
        return false;
    }

    return yarp::os::Wire::yarp().attachAsServer(serverPort);
}

bool BodyExecution::close()
{
    serverPort.close();
    robotDevice.close();
    return true;
}

bool BodyExecution::interruptModule()
{
    serverPort.interrupt();

    if (int numAxes; !iEncoders->getAxes(&numAxes) || !iPositionControl->setRefSpeeds(std::vector(numAxes, DEFAULT_REF_SPEED).data()))
    {
        yWarning() << "Failed to restore reference speeds";
    }

    return stop();
}

double BodyExecution::getPeriod()
{
    return 0.1; // [s]
}

bool BodyExecution::updateModule()
{
    bool isMotionDone = true;

    if (!iPositionControl->checkMotionDone(&isMotionDone))
    {
        yWarning() << "Unable to check motion state";
    }

    std::unique_lock lock(actionMutex);

    if (isProcessingSetpoints && isMotionDone && currentSetpoints.empty())
    {
        currentAction = noAction; // motion done and no more points to send
        isProcessingSetpoints = false;
    }

    yDebugThrottle(1.0) << "Current action:" << currentAction;

    if (isProcessingSetpoints && isMotionDone && !currentSetpoints.empty())
    {
        yDebug() << "Sending new setpoints:" << currentSetpoints.front();

        auto [head, leftArm, rightArm] = currentSetpoints.front();
        currentSetpoints.pop_front();
        lock.unlock();

        std::vector<double> values;
        values.insert(values.end(), head.cbegin(), head.cend());
        values.insert(values.end(), leftArm.cbegin(), leftArm.cend());
        values.insert(values.end(), rightArm.begin(), rightArm.end());

        if (!sendMotionCommand(values))
        {
            yWarning() << "Failed to send new setpoints";
        }
    }

    return true;
}

bool BodyExecution::sendMotionCommand(const std::vector<double> & targets)
{
    std::vector<double> q(targets.size());

    if (!iEncoders->getEncoders(q.data()))
    {
        yWarning() << "Failed to get current encoder values";
        return false;
    }

    std::vector<double> deltas(targets.size());

    std::transform(targets.cbegin(), targets.cend(), q.cbegin(), deltas.begin(), [](auto target, auto current) {
        return std::abs(target - current);
    });

    double maxDelta = *std::max_element(deltas.cbegin(), deltas.cend());

    std::vector<double> refSpeeds(targets.size());

    std::transform(deltas.cbegin(), deltas.cend(), refSpeeds.begin(), [maxDelta](auto delta) {
        return DEFAULT_REF_SPEED * delta / maxDelta; // isochronous
    });

    if (!iPositionControl->setRefSpeeds(refSpeeds.data()))
    {
        yWarning() << "Failed to set reference speeds";
        return false;
    }

    if (!iPositionControl->positionMove(targets.data()))
    {
        yWarning() << "Failed to send motion command";
        return false;
    }

    return true;
}

void BodyExecution::doGreet()
{
    registerSetpoints("greet", {
        {headZeros, armZeros, {-45.0, 0.0, -20.0, -80.0, 0.0, 0.0}},
        {headZeros, armZeros, {-45.0, 0.0, 20.0, -80.0, 0.0, 0.0}},
        {headZeros, armZeros, {-45.0, 0.0, -20.0, -80.0, 0.0, 0.0}},
    });
}

void BodyExecution::doHoming()
{
    registerSetpoints("homing", {
        {headZeros, armZeros, armZeros}
    });
}

void BodyExecution::doExplanation1()
{
    registerSetpoints("explanation 1", {
        {headZeros, armZeros, {17.03, -22.65, -1.49, -88.75, 2.36, -53.78}},
        {{-60.0, 0.0}, armZeros, {-65.00, -79.42, -6.24, -88.66, 31.44, 31.18}},
        {headZeros, {-68.75, 10.53, -16.59, -96.57, -34.89, -9.02}, {17.03, -22.65, -1.49, -88.75, 2.36, -53.78}},
    });
}

void BodyExecution::doExplanation2()
{
    registerSetpoints("explanation 2", {
        {headZeros, {-39.46, -0.50, 5.09, -58.08, -29.75, -52.80}, {-39.12, -3.60, -5.90, -64.95, 29.75, -56.96}},
        {headZeros, {-63.12, -3.60, -5.90, -80.95, 10.25, -19.75}, {-63.80, -3.85, -3.60, -65.20, -12.65, -38.58}},
    });
}

void BodyExecution::doExplanation3()
{
    registerSetpoints("explanation 3", {
        {headZeros, {-63.12, -3.60, -5.90, -80.95, 10.25, -19.75}, {-63.80, -3.85, -3.60, -65.20, -12.65, -38.58}},
        {headZeros, {-63.12, -3.60, -5.90, -80.95, 10.25, -39.75}, {-63.80, -3.85, -3.60, -65.20, -12.65, -58.58}},
        {headZeros, {-63.12, -3.60, -5.90, -80.95, 10.25, -19.75}, {-63.80, -3.85, -3.60, -65.20, -12.65, -38.58}},
    });
}

void BodyExecution::doExplanation4()
{
    registerSetpoints("explanation 4", {
        {headZeros, {-39.46, -0.50, 5.09, -58.08, -29.75, -52.80}, {-39.12, -3.60, -5.90, -64.95, 29.75, -56.96}},
        {headZeros, {-63.12, -3.60, -5.90, -80.95, 10.25, -19.75}, {-63.80, -3.85, -3.60, -65.20, -12.65, -38.58}},
        {{0.0, 10.0}, {-63.12, -3.60, -5.90, -80.95, 10.25, -19.75}, {-63.80, -3.85, -3.60, -65.20, -12.65, -38.58}},
        {{0.0, -10.0}, {-63.12, -3.60, -5.90, -80.95, 10.25, -19.75}, {-63.80, -3.85, -3.60, -65.20, -12.65, -38.58}},
    });
}

void BodyExecution::doExplanationHead()
{
    registerSetpoints("explanation head", {
        {{20.0, 10.0}, {-70.90, -1.30, -48.51, -93.04, -8.96, -52.80}, armZeros}
    });
}

void BodyExecution::doExplanationRightPC()
{
    registerSetpoints("explanation right PC", {
        {{-20.0, 10.0}, armZeros, {-63.53, -8.15, 34.52, -72.23, 32.32, -85.17}}
    });
}

void BodyExecution::doExplanationLeftPC()
{
    registerSetpoints("explanation left PC", {
        {{20.0, 10.0}, armZeros, {-64.93, 8.96, 54.29, -72.32, 40.40, -50.00}}
    });
}

void BodyExecution::doExplanationInsidePC()
{
    // left arm without plate (wrist = -79.961016)
    registerSetpoints("explanation inside PC", {
        {{20.0, 10.0}, {-37.96, 15.48, -29.75, -94.64, -20.31, -52.80}, armZeros}
    });
}

void BodyExecution::doExplanationHDD()
{
    registerSetpoints("explanation HDD", {
        {{0.0, 10.0}, {-36.80, 14.85, -37.96, -85.83, -60.72, -52.80}, {-42.71, -8.42, 30.83, -81.37, 68.79, -93.67}}
    });
}

void BodyExecution::doExplanationSensors()
{
    registerSetpoints("explanation sensors", {
        {{15.0, 10.0}, {-55.87, -11.32, -27.07, -87.33, -38.84, -43.04}, {-42.00, 5.98, 38.03, -76.98, 48.93, -36.20}},
        {{15.0, 10.0}, {-44.80, 14.32, -9.31, -79.95, -44.64, -52.80}, {-42.00, 5.98, 38.03, -76.98, 48.93, -36.20}},
        {{15.0, 10.0}, {-44.80, 14.32, -9.31, -79.95, -44.64, -52.80}, {-64.32, 13.71, 38.12, -76.98, 48.84, -43.15}},
        {{-15.0, 10.0}, {-62.11, -3.50, -28.91, -66.68, -55.27, -52.80}, {-65.47, 14.85, 36.45, -76.98, 21.33, -56.15}},
    });
}

bool BodyExecution::checkMotionDone()
{
    std::lock_guard lock(actionMutex);
    return !isProcessingSetpoints;
}

bool BodyExecution::stop()
{
    yInfo() << "Commanding stop";

    {
        std::lock_guard lock(actionMutex);
        currentAction = noAction;
        currentSetpoints.clear();
        isProcessingSetpoints = false;
    }

    if (!iPositionControl->stop())
    {
        yWarning() << "Failed to stop";
        return false;
    }

    return true;
}

void BodyExecution::registerSetpoints(const std::string & action, std::initializer_list<setpoints_t> setpoints)
{
    yInfo() << "Registered new action:" << action;

    std::lock_guard lock(actionMutex);
    currentAction = action;
    currentSetpoints.clear();
    currentSetpoints.insert(currentSetpoints.end(), setpoints);
    isProcessingSetpoints = true;
}
