// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __DIALOGUE_MANAGER_HPP__
#define __DIALOGUE_MANAGER_HPP__

#include <atomic>
#include <string>
#include <unordered_map>

#include <yarp/os/RFModule.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/Thread.h>

#include <SpeechIDL.h>

#include "SelfPresentationCommandsIDL.h"

namespace roboticslab
{

/**
 * @ingroup teo-self-presentation_programs
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
    void speak(const std::string & sentenceId);
    void awaitSpeechCompletion();
    void awaitMotionCompletion();
    void awaitSpeechAndMotionCompletion();

    SpeechIDL speech;
    SelfPresentationCommandsIDL motion;

    yarp::os::RpcClient speechPort;
    yarp::os::RpcClient motionPort;

    std::string voice;
    std::unordered_map<std::string, std::string> sentences;

    std::atomic<bool> demoCompleted {false};
};

} // namespace roboticslab

#endif // __DIALOGUE_MANAGER_HPP__
