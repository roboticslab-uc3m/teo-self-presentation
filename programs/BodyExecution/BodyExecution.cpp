// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "BodyExecution.hpp"

#include <yarp/conf/version.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>
#include <yarp/os/SystemClock.h>

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

    if (!inDialogPort.open(DEFAULT_PREFIX + std::string("/rpc:s")))
    {
        yError() << "Unable to open RPC port";
        return false;
    }

    inDialogPort.setReader(*this);
    return true;
}

bool BodyExecution::close()
{
    inDialogPort.close();
    headDevice.close();
    leftArmDevice.close();
    rightArmDevice.close();
    return true;
}

bool BodyExecution::interruptModule()
{
    inDialogPort.interrupt();
    return true;
}

double BodyExecution::getPeriod()
{
    return 1.0; // [s]
}

bool BodyExecution::updateModule()
{
    static const std::vector<double> headZeros(2, 0.0);
    static const std::vector<double> armZeros(6, 0.0);

    switch (state)
    {
    case VOCAB_STATE_SALUTE:
        yInfo() << "Salute";
        moveAndWait(armZeros, {-45.0, 0.0, -20.0, -80.0, 0.0, 0.0}, headZeros);
        moveAndWait(armZeros, {-45.0, 0.0, 20.0, -80.0, 0.0, 0.0}, headZeros);
        moveAndWait(armZeros, {-45.0, 0.0, -20.0, -80.0, 0.0, 0.0}, headZeros);
        // force homing on next iteration, don't reset done flag nor state yet
        state = VOCAB_STATE_HOME;
        return true;

    case VOCAB_STATE_HOME:
        yInfo() << "Home";
        moveAndWait(armZeros, armZeros, headZeros);
        break;

    case VOCAB_STATE_EXPLANATION_1:
        yInfo() << "Explanation 1";
        moveAndWait(armZeros, {17.03, -22.65, -1.49, -88.75, 2.36, -53.78}, headZeros);
        moveAndWait(armZeros, {-65.00, -79.42, -6.24, -88.66, 31.44, 31.18}, {-60.0, 0.0});
        moveAndWait({-68.75, 10.53, -16.59, -96.57, -34.89, -9.02}, {17.03, -22.65, -1.49, -88.75, 2.36, -53.78}, headZeros);
        break;

    case VOCAB_STATE_EXPLANATION_2:
        yInfo() << "Explanation 2";
        moveAndWait({-39.46, -0.50, 5.09, -58.08, -29.75, -52.80}, {-39.12, -3.60, -5.90, -64.95, 29.75, -56.96}, headZeros);
        moveAndWait({-63.12, -3.60, -5.90, -80.95, 10.25, -19.75}, {-63.80, -3.85, -3.60, -65.20, -12.65, -38.58}, headZeros);
        break;

    case VOCAB_STATE_EXPLANATION_3:
        yInfo() << "Explanation 3";
        moveAndWait({-39.46, -0.50, 5.09, -58.08, -29.75, -52.80}, {-39.12, -3.60, -5.90, -64.95, 29.75, -56.96}, headZeros);
        moveAndWait({-63.12, -3.60, -5.90, -80.95, 10.25, -19.75}, {-63.80, -3.85, -3.60, -65.20, -12.65, -38.58}, headZeros);
        moveAndWait({-63.12, -3.60, -5.90, -80.95, 10.25, -19.75}, {-63.80, -3.85, -3.60, -65.20, -12.65, -38.58}, {0.0, 10.0});
        moveAndWait({-63.12, -3.60, -5.90, -80.95, 10.25, -19.75}, {-63.80, -3.85, -3.60, -65.20, -12.65, -38.58}, {0.0, -10.0});
        break;

    case VOCAB_STATE_EXPLANATION_HEAD:
        yInfo() << "Explanation Head";
        moveAndWait({-70.90, -1.30, -48.51, -93.04, -8.96, -52.80}, armZeros, {20.0, 10.0});
        break;

    case VOCAB_STATE_EXPLANATION_PC_RIGHT:
        yInfo() << "Explanation PC 1";
        moveAndWait(armZeros, {-63.53, -8.15, 34.52, -72.23, 32.32, -85.17}, {-20.0, 10.0});
        break;

    case VOCAB_STATE_EXPLANATION_PC_LEFT:
        yInfo() << "Explanation PC 1";
        moveAndWait(armZeros, {-64.93, 8.96, 54.29, -72.32, 40.40, -50.00}, {20.0, 10.0});
        break;

    case VOCAB_STATE_EXPLANATION_PC_INSIDE:
        yInfo() << "Explanation PC 1";
        // left arm without plate (wrist = -79.961016)
        moveAndWait({-37.96, 15.48, -29.75, -94.64, -20.31, -52.80}, armZeros, {20.0, 10.0});
        break;

    case VOCAB_STATE_EXPLANATION_HDD:
        yInfo() << "Explanation HDD";
        moveAndWait({-36.80, 14.85, -37.96, -85.83, -60.72, -52.80}, {-42.71, -8.42, 30.83, -81.37, 68.79, -93.67}, {0.0, 10.0});
        break;

    case VOCAB_STATE_EXPLANATION_SENSOR:
        yInfo() << "Explanation Sensors";
        moveAndWait({-55.87, -11.32, -27.07, -87.33, -38.84, -43.04}, {-42.00, 5.98, 38.03, -76.98, 48.93, -36.20}, {15.0, 10.0});
        moveAndWait({-44.80, 14.32, -9.31, -79.95, -44.64, -52.80}, {-42.00, 5.98, 38.03, -76.98, 48.93, -36.20}, {15.0, 10.0});
        moveAndWait({-44.80, 14.32, -9.31, -79.95, -44.64, -52.80}, {-64.32, 13.71, 38.12, -76.98, 48.84, -43.15}, {15.0, 10.0});
        moveAndWait({-62.11, -3.50, -28.91, -66.68, -55.27, -52.80}, {-65.47, 14.85, 36.45, -76.98, 21.33, -56.15}, {-15.0, 10.0});
        break;

    default:
        yInfo() << "Doing nothing....";
        break;
    }

    done = true;
    state = 0;

    return true;
}

void BodyExecution::moveAndWait(const std::vector<double> & leftArm, const std::vector<double> & rightArm, const std::vector<double> & head)
{
    done = false;

    if (!headIPositionControl->positionMove(head.data()))
    {
        yWarning() << "Unable to set reference point for head axes";
    }

    if (!leftArmIPositionControl->positionMove(leftArm.data()))
    {
        yWarning() << "Unable to set reference point for left arm axes";
    }

    if (!rightArmIPositionControl->positionMove(rightArm.data()))
    {
        yWarning() << "Unable to set reference point for right arm axes";
    }

    bool leftArmMotionDone = false;

    while (!leftArmMotionDone)
    {
        yarp::os::SystemClock::delaySystem(0.1);
        leftArmIPositionControl->checkMotionDone(&leftArmMotionDone);
    }

    bool rightArmMotionDone = false;

    while (!rightArmMotionDone)
    {
        yarp::os::SystemClock::delaySystem(0.1);
        rightArmIPositionControl->checkMotionDone(&rightArmMotionDone);
    }

    bool headMotionDone = false;

    while (!headMotionDone)
    {
        yarp::os::SystemClock::delaySystem(0.1);
        headIPositionControl->checkMotionDone(&headMotionDone);
    }
}

bool BodyExecution::read(yarp::os::ConnectionReader & connection)
{
    yarp::os::Bottle in; // in: the VOCAB_STATE, out: boolean to check if the movement has finished

    if (!in.read(connection))
    {
        return false;
    }

#if YARP_VERSION_MINOR >= 5
    state = in.get(0).asVocab32();
#else
    state = in.get(0).asVocab();
#endif
    yDebug() << "State received:" << in.toString();

    if (state == VOCAB_RETURN_MOVEMENT_STATE)
    {
        auto * returnToSender = connection.getWriter();

        if (returnToSender)
        {
            yarp::os::Bottle out {yarp::os::Value(done ? 1 : 0)};
            out.write(*returnToSender);
        }
    }

    return true;
}
