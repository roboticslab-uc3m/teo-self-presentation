// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __STATE_MACHINE__
#define __STATE_MACHINE__

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Port.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/Time.h>

#include <yarp/dev/ControlBoardInterfaces.h>

#define VOCAB_STATE_SALUTE yarp::os::createVocab('s','a','l','u')
#define VOCAB_STATE_HOME yarp::os::createVocab('h','o','m','e')
#define VOCAB_STATE_EXPLANATION_1 yarp::os::createVocab('e','x','p','1')
#define VOCAB_STATE_EXPLANATION_2 yarp::os::createVocab('e','x','p','2')
#define VOCAB_STATE_EXPLANATION_3 yarp::os::createVocab('e','x','p','3')
#define VOCAB_STATE_EXPLANATION_HEAD yarp::os::createVocab('e','x','h','e')
#define VOCAB_STATE_EXPLANATION_PC_RIGHT yarp::os::createVocab('e','p','c','r')
#define VOCAB_STATE_EXPLANATION_PC_LEFT yarp::os::createVocab('e','p','c','l')
#define VOCAB_STATE_EXPLANATION_PC_INSIDE yarp::os::createVocab('e','p','c','i')
#define VOCAB_STATE_EXPLANATION_HDD yarp::os::createVocab('e','x','h','d')
#define VOCAB_STATE_EXPLANATION_SENSOR yarp::os::createVocab('e','x','s','e')
#define VOCAB_RETURN_MOVEMENT_STATE yarp::os::createVocab('r','e','t','m')



namespace teo
{

/**
 * @ingroup scriptManager
 *
 * @brief implements a specific state machine for scriptManager.
 */
class ScriptManager : public yarp::os::Thread {
protected:

    yarp::os::BufferedPort<yarp::os::Bottle> *inSrPort;
    yarp::os::RpcClient *outCmdMovementsPort;
    yarp::os::RpcClient *outTtsPort;

    std::string _language; // language configurated

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
    std::string finality_02;
    std::string ending_01;

    // function for speaking
    void ttsSay(std::string& sayConstString);

    // function for wait the movement
    void waitForMovement();

public:

    /**
     * Main script function. This start of all robot actions.
     */
    /** Thread run */
    virtual void run();

    /** set language for speaking */
    bool setSpeakLanguage(std::string language);

    /** Register an output Port for tts. */
    void setOutTtsPort(yarp::os::RpcClient* outTtsPort);

    /** Register an output Port for [BODY MOVEMENTS] commands. */
    void setOutCmdMovementsPort(yarp::os::RpcClient* outCmdMovePort);

};

}  // namespace teo

#endif

