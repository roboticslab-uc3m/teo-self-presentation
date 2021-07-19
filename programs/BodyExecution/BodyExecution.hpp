// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __FOLLOW_ME_ARM_SWING_HPP__
#define __FOLLOW_ME_ARM_SWING_HPP__

#include <yarp/conf/version.h>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>

constexpr auto DEFAULT_ROBOT = "/teo"; // teo or teoSim

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

namespace teo
{

/**
 * @ingroup teo-self-presentation_programs
 *
 * @brief Body Execution Core.
 *
 */
class BodyExecution : public yarp::os::RFModule, public yarp::os::PortReader, public yarp::os::Thread
{
public:
     virtual bool configure(yarp::os::ResourceFinder &rf);
private:
     /** RFModule interruptModule. */
     virtual bool interruptModule();
     /** RFModule getPeriod. */
     virtual double getPeriod();
     /** RFModule updateModule. */
     virtual bool updateModule();

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
     bool jointsMoveAndWait(std::vector<double>& leftArm, std::vector<double>& rightArm, std::vector<double> &head);

     /** State */
     int state;

     /** movement finished */
     bool done;

     /** Input port from dialogue manager */
     yarp::os::RpcServer inDialogPort;

     /** Treats data received from input port from speech recognition */
     virtual bool read(yarp::os::ConnectionReader& connection);

     /** Thread run */
     virtual void run();

};

}  // namespace teo

#endif  // __FOLLOW_ME_ARM_SWING_HPP__
