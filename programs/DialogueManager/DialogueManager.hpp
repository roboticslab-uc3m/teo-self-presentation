// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __DIALOGUE_MANAGER_HPP__
#define __DIALOGUE_MANAGER_HPP__

#include <atomic>
#include <string>
#include <unordered_map>

#include <yarp/conf/version.h>

#include <yarp/os/RFModule.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>

#include <SpeechIDL.h>

#if YARP_VERSION_MINOR >= 5
constexpr auto VOCAB_STATE_SALUTE = yarp::os::createVocab32('s','a','l','u');
constexpr auto VOCAB_STATE_HOME = yarp::os::createVocab32('h','o','m','e');
constexpr auto VOCAB_STATE_EXPLANATION_1 = yarp::os::createVocab32('e','x','p','1');
constexpr auto VOCAB_STATE_EXPLANATION_2 = yarp::os::createVocab32('e','x','p','2');
constexpr auto VOCAB_STATE_EXPLANATION_3 = yarp::os::createVocab32('e','x','p','3');
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
 * @brief Dialogue Manager.
 */
class DialogueManager : public yarp::os::RFModule,
                        public yarp::os::Thread
{
public:
    ~DialogueManager()
    { close(); }

    bool configure(yarp::os::ResourceFinder & rf) override;
    bool close() override;
    bool interruptModule() override;
    double getPeriod() override;
    bool updateModule() override;

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    void awaitSpeechCompletion();
    void awaitSpeechAndMotionCompletion();

    SpeechIDL speech;

    yarp::os::RpcClient outTtsPort; // tts port
    yarp::os::RpcClient outCmdMovementsPort; // robot movements

    std::string voice;
    std::unordered_map<std::string, std::string> sentences;

    std::atomic<bool> demoCompleted {false};
};

} // namespace roboticslab

#endif // __DIALOGUE_MANAGER_HPP__
