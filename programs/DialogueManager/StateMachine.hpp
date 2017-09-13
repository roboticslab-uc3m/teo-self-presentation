// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __STATE_MACHINE__
#define __STATE_MACHINE__

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Port.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/Time.h>

//#include <yarp/dev/PolyDriver.h>
//#include <yarp/dev/CartesianControl.h>
//#include <yarp/dev/ControlBoardInterfaces.h>


namespace teo
{

/**
 * @ingroup followMeDialogueManager
 *
 * @brief implements a specific state machine for dialogueManager.
 */
class StateMachine : public yarp::os::Thread {
protected:

    yarp::os::BufferedPort<yarp::os::Bottle> *inSrPort;
    yarp::os::RpcClient *outCmdBodyPort;
    yarp::os::RpcClient *outTtsPort;

    std::string _language;
    int _machineState;


    // sentence variables
    std::string presentation_01;
    std::string presentation_02;
    std::string composition_01;
    std::string composition_02;
    std::string composition_03;
    std::string composition_04;
    std::string composition_05;
    std::string composition_06;
    std::string composition_07;
    std::string composition_08;
    std::string finality_01;
    std::string ending_01;

    void ttsSay(const yarp::os::ConstString& sayConstString);

public:

    /**
     * Initialization method. The thread executes this function
     * when it starts and before "run". This is a good place to 
     * perform initialization tasks that need to be done by the 
     * thread itself (device drivers initialization, memory 
     * allocation etc). If the function returns false the thread 
     * quits and never calls "run". The return value of threadInit()
     * is notified to the class and passed as a parameter 
     * to afterStart(). Note that afterStart() is called by the 
     * same thread that is executing the "start" method.
     */
    bool threadInit();

    /**
     * Loop function. This is the thread itself.
     */
    void run();

    /**
     * Get its state.
     */
    int getMachineState();

    /** set language for speaking */
    bool setSpeakLanguage(std::string language);

    /** Register an output Port for tts. */
    void setOutTtsPort(yarp::os::RpcClient* outTtsPort);

};

}  // namespace teo

#endif

