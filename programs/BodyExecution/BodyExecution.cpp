// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "BodyExecution.hpp"

#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>

using namespace roboticslab;

constexpr auto DEFAULT_ROBOT = "/teo"; // teo or teoSim
constexpr auto DEFAULT_PREFIX = "/bodyExecution";
constexpr auto DEFAULT_REF_SPEED = 25.0; // [m/s]
constexpr auto DEFAULT_REF_ACCELERATION = 25.0; // [m/s^2]

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

    // ------ HEAD -------

    yarp::os::Property headOptions {
        {"device", yarp::os::Value("remote_controlboard")},
        {"remote", yarp::os::Value(robot + "/head")},
        {"local", yarp::os::Value(DEFAULT_PREFIX + robot + "/head")}
    };

    if (!headDevice.open(headOptions))
    {
        yError() << "Robot head device not available";
        return false;
    }

    if (!headDevice.view(headIControlMode) || !headDevice.view(headIPositionControl))
    {
        yError() << "Unable to acquire head interfaces";
        return false;
    }

    // ------ LEFT ARM -------

    yarp::os::Property leftArmOptions {
        {"device", yarp::os::Value("remote_controlboard")},
        {"remote", yarp::os::Value(robot + "/leftArm")},
        {"local", yarp::os::Value(DEFAULT_PREFIX + robot + "/leftArm")}
    };

    if (!leftArmDevice.open(leftArmOptions)) {
        yError() << "Robot left arm device not available";
        return false;
    }

    if (!leftArmDevice.view(leftArmIControlMode) || !leftArmDevice.view(leftArmIPositionControl))
    {
        yError() << "Unable to acquire left arm interfaces";
        return false;
    }

    // ------ RIGHT ARM -------

    yarp::os::Property rightArmOptions {
        {"device", yarp::os::Value("remote_controlboard")},
        {"remote", yarp::os::Value(robot + "/rightArm")},
        {"local", yarp::os::Value(DEFAULT_PREFIX + robot + "/rightArm")}
    };

    if (!rightArmDevice.open(rightArmOptions))
    {
        yError() << "Robot right arm device not available";
        return false;
    }

    if (!rightArmDevice.view(rightArmIControlMode) || !rightArmDevice.view(rightArmIPositionControl))
    {
        yError() << "Unable to acquire right arm interfaces";
        return false;
    }

    //-- Set control modes

    int headAxes;
    headIPositionControl->getAxes(&headAxes);

    if (!headIControlMode->setControlModes(std::vector<int>(headAxes, VOCAB_CM_POSITION).data()))
    {
        yError() << "Unable to set position control mode on head";
        return false;
    }

    int leftArmAxes;
    leftArmIPositionControl->getAxes(&leftArmAxes);

    if (!leftArmIControlMode->setControlModes(std::vector<int>(leftArmAxes, VOCAB_CM_POSITION).data()))
    {
        yError() << "Unable to set position control mode on left arm";
        return false;
    }

    int rightArmAxes;
    rightArmIPositionControl->getAxes(&rightArmAxes);

    if (!rightArmIControlMode->setControlModes(std::vector<int>(rightArmAxes, VOCAB_CM_POSITION).data()))
    {
        yError() << "Unable to set position control mode on right arm";
        return false;
    }

    // -- Configure speeds and accelerations

    if (!headIPositionControl->setRefSpeeds(std::vector<double>(headAxes, DEFAULT_REF_SPEED).data()))
    {
        yError() << "Unable to set reference speeds on head";
        return false;
    }

    if (!leftArmIPositionControl->setRefSpeeds(std::vector<double>(leftArmAxes, DEFAULT_REF_SPEED).data()))
    {
        yError() << "Unable to set reference speeds on left arm";
        return false;
    }

    if (!rightArmIPositionControl->setRefSpeeds(std::vector<double>(rightArmAxes, DEFAULT_REF_SPEED).data()))
    {
        yError() << "Unable to set reference speeds on right arm";
        return false;
    }

    if (!headIPositionControl->setRefAccelerations(std::vector<double>(headAxes, DEFAULT_REF_ACCELERATION).data()))
    {
        yError() << "Unable to set reference accelerations on head";
        return false;
    }

    if (!leftArmIPositionControl->setRefAccelerations(std::vector<double>(leftArmAxes, DEFAULT_REF_ACCELERATION).data()))
    {
        yError() << "Unable to set reference accelerations on left arm";
        return false;
    }

    if (!rightArmIPositionControl->setRefAccelerations(std::vector<double>(rightArmAxes, DEFAULT_REF_ACCELERATION).data()))
    {
        yError() << "Unable to set reference accelerations on right arm";
        return false;
    }

    // -- Configure port

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
    headDevice.close();
    leftArmDevice.close();
    rightArmDevice.close();
    return true;
}

bool BodyExecution::interruptModule()
{
    serverPort.interrupt();
    return stop();
}

double BodyExecution::getPeriod()
{
    return 0.1; // [s]
}

bool BodyExecution::updateModule()
{
    bool isMotionDone = checkMotionDone();
    actionMutex.lock();

    if (!hasNewSetpoints && isMotionDone && currentSetpoints.empty())
    {
        currentAction = noAction; // motion done and no more points to send
    }

    yDebugThrottle(1.0) << "Current action:" << currentAction;

    if (hasNewSetpoints || (isMotionDone && !currentSetpoints.empty()))
    {
        auto setpoints = currentSetpoints.front();
        currentSetpoints.pop_front();
        hasNewSetpoints = false;
        actionMutex.unlock();

        yInfo() << "Sending new setpoints"; // TODO: print values

        if (!headIPositionControl->positionMove(std::get<0>(setpoints).data()))
        {
            yWarning() << "Unable to configure new setpoints on head";
        }

        if (!leftArmIPositionControl->positionMove(std::get<1>(setpoints).data()))
        {
            yWarning() << "Unable to configure new setpoints on left arm";
        }

        if (!rightArmIPositionControl->positionMove(std::get<2>(setpoints).data()))
        {
            yWarning() << "Unable to configure new setpoints on right arm";
        }
    }

    actionMutex.unlock();
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
    bool headMotionDone = true;

    if (!headIPositionControl->checkMotionDone(&headMotionDone))
    {
        yWarning() << "Unable to check motion state of head";
    }

    bool leftArmMotionDone = true;

    if (!leftArmIPositionControl->checkMotionDone(&leftArmMotionDone))
    {
        yWarning() << "Unable to check motion state of left arm";
    }

    bool rightArmMotionDone = true;

    if (!rightArmIPositionControl->checkMotionDone(&rightArmMotionDone))
    {
        yWarning() << "Unable to check motion state of right arm";
    }

    return headMotionDone && leftArmMotionDone && rightArmMotionDone;
}

bool BodyExecution::stop()
{
    yInfo() << "Commanding stop";

    {
        std::unique_lock<std::mutex> lock(actionMutex);
        currentAction = noAction;
        currentSetpoints.clear();
        hasNewSetpoints = false;
    }

    bool ok = true;

    if (!headIPositionControl->stop())
    {
        yWarning() << "Failed to stop head";
        ok = false;
    }

    if (!leftArmIPositionControl->stop())
    {
        yWarning() << "Failed to stop left arm";
        ok = false;
    }

    if (!rightArmIPositionControl->stop())
    {
        yWarning() << "Failed to stop right arm";
        ok = false;
    }

    return ok;
}

void BodyExecution::registerSetpoints(const std::string & action, std::initializer_list<setpoints_t> setpoints)
{
    yInfo() << "Registered new action:" << action;

    std::lock_guard<std::mutex> lock(actionMutex);
    currentAction = action;
    currentSetpoints.clear();
    currentSetpoints.insert(currentSetpoints.end(), setpoints);
    hasNewSetpoints = true;
}
