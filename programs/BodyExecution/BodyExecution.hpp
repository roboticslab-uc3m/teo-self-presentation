// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __BODY_EXECUTION_HPP__
#define __BODY_EXECUTION_HPP__

#include <atomic>
#include <vector>

#include <yarp/conf/version.h>

#include <yarp/os/PortReader.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/PolyDriver.h>

#if YARP_VERSION_MINOR >= 5
constexpr auto VOCAB_STATE_SALUTE = yarp::os::createVocab32('s','a','l','u');
constexpr auto VOCAB_STATE_HOME = yarp::os::createVocab32('h','o','m','e');
constexpr auto VOCAB_STATE_EXPLANATION_1 = yarp::os::createVocab32('e','x','p','1');
constexpr auto VOCAB_STATE_EXPLANATION_2 = yarp::os::createVocab32('e','x','p','2');
constexpr auto VOCAB_STATE_EXPLANATION_3 = yarp::os::createVocab32('e','x','p','3');
constexpr auto VOCAB_STATE_EXPLANATION_4 = yarp::os::createVocab32('e','x','p','4');
constexpr auto VOCAB_STATE_EXPLANATION_HEAD = yarp::os::createVocab32('e','x','h','e');
constexpr auto VOCAB_STATE_EXPLANATION_PC_RIGHT = yarp::os::createVocab32('e','p','c','r');
constexpr auto VOCAB_STATE_EXPLANATION_PC_LEFT = yarp::os::createVocab32('e','p','c','l');
constexpr auto VOCAB_STATE_EXPLANATION_PC_INSIDE = yarp::os::createVocab32('e','p','c','i');
constexpr auto VOCAB_STATE_EXPLANATION_HDD = yarp::os::createVocab32('e','x','h','d');
constexpr auto VOCAB_STATE_EXPLANATION_SENSOR = yarp::os::createVocab32('e','x','s','e');
constexpr auto VOCAB_RETURN_MOVEMENT_STATE = yarp::os::createVocab32('r','e','t','m');
#else
constexpr auto VOCAB_STATE_SALUTE = yarp::os::createVocab('s','a','l','u');
constexpr auto VOCAB_STATE_HOME = yarp::os::createVocab('h','o','m','e');
constexpr auto VOCAB_STATE_EXPLANATION_1 = yarp::os::createVocab('e','x','p','1');
constexpr auto VOCAB_STATE_EXPLANATION_2 = yarp::os::createVocab('e','x','p','2');
constexpr auto VOCAB_STATE_EXPLANATION_3 = yarp::os::createVocab('e','x','p','3');
constexpr auto VOCAB_STATE_EXPLANATION_4 = yarp::os::createVocab('e','x','p','4');
constexpr auto VOCAB_STATE_EXPLANATION_HEAD = yarp::os::createVocab('e','x','h','e');
constexpr auto VOCAB_STATE_EXPLANATION_PC_RIGHT = yarp::os::createVocab('e','p','c','r');
constexpr auto VOCAB_STATE_EXPLANATION_PC_LEFT = yarp::os::createVocab('e','p','c','l');
constexpr auto VOCAB_STATE_EXPLANATION_PC_INSIDE = yarp::os::createVocab('e','p','c','i');
constexpr auto VOCAB_STATE_EXPLANATION_HDD = yarp::os::createVocab('e','x','h','d');
constexpr auto VOCAB_STATE_EXPLANATION_SENSOR = yarp::os::createVocab('e','x','s','e');
constexpr auto VOCAB_RETURN_MOVEMENT_STATE = yarp::os::createVocab('r','e','t','m');
#endif

namespace roboticslab
{

/**
 * @ingroup teo-self-presentation_programs
 *
 * @brief Body Execution Core.
 */
class BodyExecution : public yarp::os::RFModule,
                      public yarp::os::PortReader
{
public:
    ~BodyExecution()
    { close(); }

    bool configure(yarp::os::ResourceFinder & rf) override;
    bool close() override;
    bool interruptModule() override;
    double getPeriod() override;
    bool updateModule() override;

    /** Treats data received from input port from speech recognition */
    bool read(yarp::os::ConnectionReader & connection) override;

private:
    /** Left Arm Device */
    yarp::dev::PolyDriver leftArmDevice;
    yarp::dev::IControlMode *leftArmIControlMode;
    yarp::dev::IPositionControl *leftArmIPositionControl;

    /** Right Arm Device */
    yarp::dev::PolyDriver rightArmDevice;
    yarp::dev::IControlMode *rightArmIControlMode;
    yarp::dev::IPositionControl *rightArmIPositionControl;

    /** Head Device */
    yarp::dev::PolyDriver headDevice;
    yarp::dev::IControlMode *headIControlMode;
    yarp::dev::IPositionControl *headIPositionControl;

    /** Arm Joints Move And Wait */
    void moveAndWait(const std::vector<double> & leftArm, const std::vector<double> & rightArm, const std::vector<double> & head);

    /** State */
    std::atomic<int> state {0};

    /** movement finished */
    std::atomic_bool done {false};

    /** Input port from dialogue manager */
    yarp::os::RpcServer inDialogPort;
};

} // namespace roboticslab

#endif // __BODY_EXECUTION_HPP__
