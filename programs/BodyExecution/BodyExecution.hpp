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

#include "SelfPresentationCommands.h"

namespace roboticslab
{

/**
 * @ingroup teo-self-presentation_programs
 * @brief Body Execution core.
 */
class BodyExecution : public yarp::os::RFModule,
                      public SelfPresentationCommands
{
public:
    using setpoints_head_t = std::array<double, 2>;
    using setpoints_arm_t = std::array<double, 6>;
    using setpoints_t = std::tuple<setpoints_head_t, setpoints_arm_t, setpoints_arm_t>;

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
    void doExplanation4() override;
    void doExplanationHead() override;
    void doExplanationRightPC() override;
    void doExplanationLeftPC() override;
    void doExplanationInsidePC() override;
    void doExplanationHDD() override;
    void doExplanationSensors() override;
    bool checkMotionDone() override;
    bool stop() override;

private:
    void registerSetpoints(const std::string & action, std::initializer_list<setpoints_t> setpoints);

    const std::string noAction { "none" };

    std::string currentAction { noAction };
    std::deque<setpoints_t> currentSetpoints;
    std::mutex actionMutex;
    bool isProcessingSetpoints { false };

    yarp::dev::PolyDriver robotDevice;
    yarp::dev::IControlMode * iControlMode;
    yarp::dev::IPositionControl * iPositionControl;

    yarp::os::RpcServer serverPort;
};

} // namespace roboticslab

#endif // __BODY_EXECUTION_HPP__
