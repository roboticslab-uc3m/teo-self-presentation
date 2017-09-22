// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __FOLLOW_ME_ARM_SWING_HPP__
#define __FOLLOW_ME_ARM_SWING_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#define DEFAULT_ROBOT "/teoSim" // teo or teoSim

#define VOCAB_STATE_SALUTE VOCAB4('s','a','l','u')
#define VOCAB_STATE_HOME VOCAB4('h','o','m','e')
#define VOCAB_STATE_EXPLANATION_1 VOCAB4('e','x','p','1')
#define VOCAB_STATE_EXPLANATION_2 VOCAB4('e','x','p','2')
#define VOCAB_STATE_EXPLANATION_3 VOCAB4('e','x','p','3')
#define VOCAB_STATE_EXPLANATION_4 VOCAB4('e','x','p','4')
#define VOCAB_STATE_EXPLANATION_HEAD VOCAB4('e','x','h','e')
#define VOCAB_STATE_EXPLANATION_PC_RIGHT VOCAB4('e','p','c','r')
#define VOCAB_STATE_EXPLANATION_PC_LEFT VOCAB4('e','p','c','l')
#define VOCAB_STATE_EXPLANATION_PC_INSIDE VOCAB4('e','p','c','i')
#define VOCAB_STATE_EXPLANATION_HDD VOCAB4('e','x','h','d')
#define VOCAB_STATE_EXPLANATION_SENSOR VOCAB4('e','x','s','e')




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
     /** Left Arm Position Interface */
     yarp::dev::IPositionControl *leftArmIPositionControl;

     /** Right Arm Device */
     yarp::dev::PolyDriver rightArmDevice;
     /** Right Arm Position Interface */
     yarp::dev::IPositionControl *rightArmIPositionControl;

     /** Head Device */
     yarp::dev::PolyDriver headDevice;
     /** Head Position Interface */
     yarp::dev::IPositionControl *headIPositionControl;

     /** Trunk Device */
     yarp::dev::PolyDriver trunkDevice;
     /** Trunk Position Interface */
     yarp::dev::IPositionControl *trunkIPositionControl;

     /** Left Leg Device */
     yarp::dev::PolyDriver leftLegDevice;
     /** Left Leg Position Interface */
     yarp::dev::IPositionControl *leftLegIPositionControl;

     /** Right Leg Device */
     yarp::dev::PolyDriver rightLegDevice;
     /** Right Leg Position Interface */
     yarp::dev::IPositionControl *rightLegIPositionControl;


     /** Arm Joints Move And Wait */
     bool jointsMoveAndWait(std::vector<double>& leftArm, std::vector<double>& rightArm, std::vector<double> &head);

     /** State */
     int state;

     /** Input port from dialogue manager */
     yarp::os::RpcServer inDialogPort;

     /** Treats data received from input port from speech recognition */
     virtual bool read(yarp::os::ConnectionReader& connection);

     /** Thread run */
     virtual void run();

};

}  // namespace teo

#endif  // __FOLLOW_ME_ARM_SWING_HPP__
