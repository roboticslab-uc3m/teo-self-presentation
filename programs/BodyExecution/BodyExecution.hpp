// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __BODY_EXECUTION_HPP__
#define __BODY_EXECUTION_HPP__

#include <array>
#include <deque>
#include <initializer_list>
#include <mutex>
#include <tuple>

#include <yarp/os/RFModule.h>
#include <yarp/os/RpcServer.h>

#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/PolyDriver.h>

#include "SelfPresentationCommandsIDL.h"

namespace roboticslab
{

/**
 * @ingroup teo-self-presentation_programs
 * @brief Body Execution core.
 */
class BodyExecution : public yarp::os::RFModule,
                      public SelfPresentationCommandsIDL
{
public:
    ~BodyExecution()
    { close(); }

    bool configure(yarp::os::ResourceFinder & rf) override;
    bool close() override;
    bool interruptModule() override;
    double getPeriod() override;
    bool updateModule() override;

    void doGreet() override;
    void doHoming() override;
    void doExplanation1() override;
    void doExplanation2() override;
    void doExplanation3() override;
    void doExplanationHead() override;
    void doExplanationRightPC() override;
    void doExplanationLeftPC() override;
    void doExplanationInsidePC() override;
    void doExplanationHDD() override;
    void doExplanationSensors() override;
    bool checkMotionDone() override;
    bool stop() override;

private:
    using setpoints_head_t = std::array<double, 2>;
    using setpoints_arm_t = std::array<double, 6>;
    using setpoints_t = std::tuple<setpoints_head_t, setpoints_arm_t, setpoints_arm_t>;

    void registerSetpoints(const std::string & action, std::initializer_list<setpoints_t> setpoints);

    const std::string noAction { "none" };
    const setpoints_head_t headZeros { 0.0, 0.0 };
    const setpoints_arm_t armZeros { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

    std::string currentAction { noAction };
    std::deque<setpoints_t> currentSetpoints;
    std::mutex actionMutex;
    bool hasNewSetpoints { false };

    yarp::dev::PolyDriver leftArmDevice;
    yarp::dev::IControlMode * leftArmIControlMode;
    yarp::dev::IPositionControl * leftArmIPositionControl;

    yarp::dev::PolyDriver rightArmDevice;
    yarp::dev::IControlMode * rightArmIControlMode;
    yarp::dev::IPositionControl * rightArmIPositionControl;

    yarp::dev::PolyDriver headDevice;
    yarp::dev::IControlMode * headIControlMode;
    yarp::dev::IPositionControl * headIPositionControl;

    yarp::os::RpcServer serverPort;
};

} // namespace roboticslab

#endif // __BODY_EXECUTION_HPP__
