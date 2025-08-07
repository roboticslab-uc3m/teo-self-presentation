// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "DialogueManager.hpp"

#include <array>
#include <exception>

#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>
#include <yarp/os/SystemClock.h>

using namespace roboticslab;

namespace
{
    class ThreadTerminator : public std::exception {};

    const std::array sentenceLabels = {
        "presentation_01",
        "presentation_02",
        "composition_01",
        "composition_02",
        "composition_03",
        "composition_04",
        "composition_05_01",
        "composition_05_02",
        "composition_05_03",
        "composition_06",
        "composition_07",
        "purpose_01",
        "purpose_02",
        "ending_01",
    };
}

constexpr auto DEFAULT_PREFIX = "/dialogueManager";
constexpr auto DEFAULT_TTS_PORT = "/tts";
constexpr auto DEFAULT_LANGUAGE = "spanish";
constexpr auto DEFAULT_BACKEND = "espeak";

bool DialogueManager::configure(yarp::os::ResourceFinder & rf)
{
    auto language = rf.check("language", yarp::os::Value(DEFAULT_LANGUAGE), "language to be used").asString();
    auto backend = rf.check("backend", yarp::os::Value(DEFAULT_BACKEND), "TTS backend").asString();

    if (rf.check("help"))
    {
        yInfo("DialogueManager options:");
        yInfo("\t--help (this help)\t--from [file.ini]\t--context [path]");
        yInfo("\t--language: %s [%s]", language.c_str(), DEFAULT_LANGUAGE);
        yInfo("\t--backend: %s [%s]", backend.c_str(), DEFAULT_BACKEND);
        yInfo("\t--model: (specific for the chosen language and backend)");
        return false;
    }

    yarp::os::Property config;
    auto path = rf.findFileByName(language + ".ini");

    if (!config.fromConfigFile(path)) {
        yError() << "Unable to open configuration file for language" << language;
        return false;
    }

    const auto & group = config.findGroup(backend);

    if (group.isNull())
    {
        yError() << "Backend" << backend << "for language" << language << "not found";
        return false;
    }

    if (!group.check("model"))
    {
        yError() << "Backend" << backend << "for language" << language << "does not have a voice model";
        return false;
    }

    model = rf.check("model", group.find("model"), "voice model").asString();

    for (const auto & label : sentenceLabels)
    {
        const auto & value = group.find(label);

        if (!value.isString())
        {
            yError() << "Backend" << backend << "for language" << language << "does not have sentence" << label;
            return false;
        }

        sentences[label] = value.asString();
    }

    yarp::os::Property ttsDeviceOptions {
        {"device", yarp::os::Value("speechSynthesizer_nwc_yarp")},
        {"local", yarp::os::Value(DEFAULT_PREFIX + std::string("/tts"))},
        {"remote", rf.check("remote", yarp::os::Value(DEFAULT_TTS_PORT), "TTS remote port")},
    };

    if (!ttsDevice.open(ttsDeviceOptions))
    {
        yError() << "Unable to open TTS client device";
        return false;
    }

    if (!ttsDevice.view(tts))
    {
        yError() << "Unable to view TTS interface";
        return false;
    }

    if (!motionPort.open(std::string(DEFAULT_PREFIX) + "/motion/rpc:c"))
    {
        yError() << "Unable to open RPC motion port" << motionPort.getName();
        return false;
    }

    motion.yarp().attachAsClient(motionPort);
    return true;
}

double DialogueManager::getPeriod()
{
    return 0.1; // [s]
}

bool DialogueManager::updateModule()
{
    static const auto throttle = 1.0; // [s]

    if (motionPort.getOutputCount() == 0)
    {
        if (yarp::os::Thread::isRunning())
        {
            yInfo() << "Motion port disconnected, forcing presentation stop";

            if (!yarp::os::Thread::stop())
            {
                yError() << "Unable to stop presentation thread";
                return false;
            }
        }
        else
        {
            yDebugThrottle(throttle) << "Waiting for" << motionPort.getName() << "to be connected to motion server";
            demoCompleted = false;
        }
    }
    else
    {
        if (yarp::os::Thread::isRunning())
        {
            yDebugThrottle(throttle) << "Presentation is running";
        }
        else if (!demoCompleted)
        {
            yInfo() << "Starting presentation thread";

            if (!yarp::os::Thread::start())
            {
                yError() << "Unable to start presentation thread";
                return false;
            }
        }
        else
        {
            yDebugThrottle(throttle) << "Presentation has ended, reconnect motion port" << motionPort.getName() << "to start again";
        }
    }

    return true;
}

bool DialogueManager::interruptModule()
{
    return yarp::os::Thread::stop();
}

bool DialogueManager::close()
{
    ttsDevice.close();
    tts = nullptr;
    motionPort.close();
    return true;
}

bool DialogueManager::threadInit()
{
    if (!tts->setLanguage(model))
    {
        yError() << "Unable to set model to" << model;
        return false;
    }

    return true;
}

void DialogueManager::threadRelease()
{
    if (!motion.stop())
    {
        yWarning() << "Unable to stop motion";
    }
}

void DialogueManager::run()
{
    yInfo() << "Presentation start";

    try
    {
        {
            const auto expiry = speak("presentation_01");
            motion.doGreet();
            awaitMotionCompletion();
            motion.doHoming();
            awaitSpeechAndMotionCompletion(expiry);
        }

        yarp::os::SystemClock::delaySystem(0.5);

        {
            const auto expiry = speak("presentation_02");
            motion.doExplanation2();
            awaitSpeechAndMotionCompletion(expiry);
        }

        yarp::os::SystemClock::delaySystem(1.0);

        {
            const auto expiry = speak("composition_01");
            motion.doExplanation1();
            awaitSpeechCompletion(expiry);
        }

        yarp::os::SystemClock::delaySystem(2.0);

        {
            const auto expiry = speak("composition_02");
            awaitMotionCompletion();
            yarp::os::SystemClock::delaySystem(1.0);
            motion.doExplanation3();
            awaitSpeechAndMotionCompletion(expiry);
        }

        {
            const auto expiry = speak("composition_03");
            motion.doExplanationHead();
            awaitSpeechAndMotionCompletion(expiry);
        }

        {
            const auto expiry = speak("composition_04");
            motion.doExplanationRightPC();
            awaitSpeechCompletion(expiry);
        }

        {
            const auto expiry = speak("composition_05_01");
            awaitSpeechAndMotionCompletion(expiry);
        }

        {
            const auto expiry = speak("composition_05_02");
            motion.doExplanationLeftPC();
            awaitSpeechAndMotionCompletion(expiry);
        }

        {
            const auto expiry = speak("composition_05_03");
            motion.doExplanationInsidePC();
            awaitSpeechAndMotionCompletion(expiry);
        }

        {
            const auto expiry = speak("composition_06");
            motion.doExplanation2();
            awaitSpeechAndMotionCompletion(expiry);
        }

        {
            const auto expiry = speak("composition_07");
            motion.doExplanationSensors();
            awaitSpeechAndMotionCompletion(expiry);
        }

        yarp::os::SystemClock::delaySystem(1.0);

        {
            const auto expiry = speak("purpose_01");
            motion.doExplanation1();
            awaitSpeechAndMotionCompletion(expiry);
        }

        yarp::os::SystemClock::delaySystem(1.0);

        {
            const auto expiry = speak("purpose_02");
            motion.doExplanation4();
            awaitSpeechAndMotionCompletion(expiry);
        }

        yarp::os::SystemClock::delaySystem(2.0);

        {
            const auto expiry = speak("ending_01");
            motion.doHoming();
            awaitSpeechAndMotionCompletion(expiry);
        }
    }
    catch (const ThreadTerminator & terminator)
    {
        yInfo() << "Prematurely terminating presentation thread";
        return;
    }

    yInfo() << "Presentation end";
    demoCompleted = true;
}

double DialogueManager::speak(const std::string & sentenceId)
{
    yInfo() << sentenceId << "->" << sentences[sentenceId];

    if (yarp::sig::Sound sound; !tts->synthesize(sentences[sentenceId], sound))
    {
        yWarning() << "Unable to say" << sentenceId;
        return yarp::os::SystemClock::nowSystem();
    }
    else
    {
        return yarp::os::SystemClock::nowSystem() + sound.getDuration();
    }
}

void DialogueManager::awaitSpeechCompletion(double expiry)
{
    const auto delay = expiry - yarp::os::SystemClock::nowSystem();

    if (delay > 0.0)
    {
        yarp::os::SystemClock::delaySystem(delay);
    }
}

void DialogueManager::awaitMotionCompletion()
{
    do
    {
        if (yarp::os::Thread::isStopping())
        {
            throw ThreadTerminator();
        }

        yarp::os::SystemClock::delaySystem(0.1);
    }
    while (motionPort.getOutputCount() > 0 && !motion.checkMotionDone());
}

void DialogueManager::awaitSpeechAndMotionCompletion(double speechExpiry)
{
    awaitSpeechCompletion(speechExpiry);
    awaitMotionCompletion();
}
