// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __STATE_MACHINE__
#define __STATE_MACHINE__

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Port.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/Time.h>

#include <yarp/dev/ControlBoardInterfaces.h>

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
 * @ingroup dialogueManager
 *
 * @brief implements a specific state machine for dialogueManager.
 */
class StateMachine : public yarp::os::Thread {
protected:

    yarp::os::BufferedPort<yarp::os::Bottle> *inSrPort;
    yarp::os::RpcClient *outCmdMovementsPort;
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
    std::string composition_05_01;
    std::string composition_05_02;
    std::string composition_05_03;
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

    /** Register an output Port for [BODY MOVEMENTS] commands. */
    void setOutCmdMovementsPort(yarp::os::RpcClient* outCmdMovePort);

};

}  // namespace teo

#endif

