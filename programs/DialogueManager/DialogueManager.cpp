// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "DialogueManager.hpp"

#include <exception>

#include <yarp/os/LogStream.h>
#include <yarp/os/SystemClock.h>

using namespace roboticslab;

namespace
{
    class ThreadTerminator : public std::exception {};

    const std::unordered_map<std::string, std::string> englishSentences = {
        {"presentation_01", "Hi. My name is TEO. I am a humanoid robot designed in the University Carlos Tercero of Madrid. I am 10 years old. My size is 1 70 meters and my weight is 60 kilograms."},
        {"presentation_02", "My purpose, is to enable our researchers, to reach new achievements and discoveries, within the robotics area."},
        {"composition_01", "I am mainly built, of aluminum, plastic and carbon fiber."},
        {"composition_02", "I have 28 degrees, of freedom, that allow me to move freely, being able to do, such human tasks as walking, identifying and manipulating objects, doing household chores, ironing, serving as a waiter, etc."},
        {"composition_03", "In my head, I have implemented two cameras, with which, I can detect objects, and human faces. Also, I can detect their distance and depth."},
        {"composition_04", "As you can see, I have three computers, in my chest, each dedicated to a different task."},
        {"composition_05_01", "The computer on my right, serves me, to capture and process the data collected from my sensors."},
        {"composition_05_02", "While, the computer on my left, is dedicated, to the tasks of, manipulation and locomotion. It is the computer that plans and gives life to each of my movements"},
        {"composition_05_03", "The computer that is located, just at the top, is dedicated to the processing of vision. In this way, each computer, is dedicated to processing a part of the task that I will perform."},
        {"composition_06", "Both of them, the manipulation computer, and the locomotion computer, are connected to a communication network called, can bus, which sends all the motion commands to each of my motors."},
        {"composition_07", "I also have movement, inertial, and force sensors, that allow me to detect the weight, and pressure exerted on my joints. These sensors allow, for example, to stay in balance, while walking or standing up."},
        {"purpose_01", "All the components that make up everything I am, as well as the programs that allow me to perform all these tasks, have been developed by teachers and students from this university. The main mission is to discover and advance together in the field of robotics."},
        {"purpose_02", "For now, I am only a prototype, dedicated to research, so, I can not be sold in any store. Also, my price would be too expensive to be bought."},
        {"ending_01", "These, is the end of my presentation. I hope you liked it, and if you want, you can take a picture with me. Nice to meet you."},
    };

    const std::unordered_map<std::string, std::string> spanishSentences = {
        {"presentation_01", "Hola. Mi nombre es TEO y soy un drobot humanoide diseñado en la Universidad Carlos Tercero de Madrid. Tengo unos 10 años desde que me crearon. Mido 1 70 y peso 60 kilos."},
        {"presentation_02", "He sido diseñado con el propósito de ayudar a la investigación y a conseguir nuevos logros y descubrimientos dentro del área de la drobótica."},
        {"composition_01", "Mis piezas están construidas principalmente de aluminio, plástico y fibra de carbono."},
        {"composition_02", "Dispongo de 28 grados de libertad que me permiten moverme con soltura, siendo capaz de hacer tareas tan humanas como andar, identificar y manipular objetos, realizar tareas del hogar, planchar, servir de camarero, etcétera."},
        {"composition_03", "En mi cabeza tengo implementadas dos cámaras con las cuales puedo detectar objetos y caras humanas, así como determinar la distancia y profundidad a la que se encuentran."},
        {"composition_04", "Como podéis ver, dispongo en el interior de mi pecho de tres ordenadores, cada uno dedicado a una tarea distinta."},
        {"composition_05_01", "El ordenador de mi derecha me sirve para captar y procesar los datos que grecogen mis sensores."},
        {"composition_05_02", "Mientras que el de mi izquierda está dedicado a las tareas de manipulación y locomoción. Es el ordenador que planifica y da vida a cada uno de mis movimientos."},
        {"composition_05_03", "El ordenador que está situado justo en la parte superior se emplea para el procesamiento de visión. De esta forma, cada ordenador se dedica a procesar una parte de la tarea que voy a grealizar."},
        {"composition_06", "Tanto el ordenador de manipulación como el de locomoción están conectados a una gred de comunicación yamada can bus, que envía todas las señales de movimiento a cada uno de mis motores."},
        {"composition_07", "También poseo sensores de movimiento, inerciales y de fuerza par que me permiten estimar el peso y la presión ejercida en mis articulaciones. Estos sensores permiten por ejemplo que pueda mantenerme en equilibrio mientras ando o estoy de pie."},
        {"purpose_01", "Todos los componentes que conforman todo lo que soy, así como los programas que me permiten grealizar todas estas tareas, han sido desagroyados por profesores y estudiantes de esta universidad. La misión principal es poder descubrir y avanzar juntos en el campo de la grobótica."},
        {"purpose_02", "Por ahora, tan solo soy un prototipo dedicado a la investigación, por lo que no puedo ser vendido en ninguna tienda. Además, mi precio sería demasiado caro para ser comprado."},
        {"ending_01", "Aquí termina mi presentación. Espero que os haya gustado y si queréis, podéis haceros una foto conmigo."},
    };
}

constexpr auto DEFAULT_PREFIX = "/dialogueManager";
constexpr auto DEFAULT_LANGUAGE = "spanish";

bool DialogueManager::configure(yarp::os::ResourceFinder & rf)
{
    auto language = rf.check("language", yarp::os::Value(DEFAULT_LANGUAGE), "language to be used").asString();

    if (rf.check("help"))
    {
        yInfo("DialogueManager options:");
        yInfo("\t--help (this help)\t--from [file.ini]\t--context [path]");
        yInfo("\t--language: %s [%s]", language.c_str(), DEFAULT_LANGUAGE);
        return false;
    }

    if (!speechPort.open(std::string(DEFAULT_PREFIX) + "/tts/rpc:c"))
    {
        yError() << "Unable to open RPC TTS port" << speechPort.getName();
        return false;
    }

    if (!motionPort.open(std::string(DEFAULT_PREFIX) + "/motion/rpc:c"))
    {
        yError() << "Unable to open RPC motion port" << motionPort.getName();
        return false;
    }

    tts.yarp().attachAsClient(speechPort);
    motion.yarp().attachAsClient(motionPort);

    if (language == "english")
    {
        voice = "en_US/m-ailabs_low";
        sentences = englishSentences;
    }
    else if (language == "spanish")
    {
        voice = "mb-es1";
        sentences = spanishSentences;
    }
    else
    {
        yError() << "Language not found, please use '--language english' or '--language spanish'";
        return false;
    }

    return true;
}

double DialogueManager::getPeriod()
{
    return 0.1; // [s]
}

bool DialogueManager::updateModule()
{
    static const auto throttle = 1.0; // [s]

    if (speechPort.getOutputCount() == 0)
    {
        if (yarp::os::Thread::isRunning())
        {
            yInfo() << "Port disconnected, forcing presentation stop";

            if (!yarp::os::Thread::stop())
            {
                yError() << "Unable to stop presentation thread";
                return false;
            }
        }
        else
        {
            yDebugThrottle(throttle) << "Waiting for" << speechPort.getName() << "to be connected to TTS server";
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
            yDebugThrottle(throttle) << "Presentation has ended, reconnect TTS port" << speechPort.getName() << "to start again";
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
    speechPort.close();
    motionPort.close();
    return true;
}

bool DialogueManager::threadInit()
{
    if (!tts.setLanguage(voice))
    {
        yError() << "Unable to set voice to" << voice;
        return false;
    }

    return true;
}

void DialogueManager::threadRelease()
{
    if (!tts.stop())
    {
        yWarning() << "Unable to stop speech";
    }

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
        speak("presentation_01");
        motion.doGreet();
        awaitMotionCompletion();
        motion.doHoming();
        awaitSpeechAndMotionCompletion();

        yarp::os::SystemClock::delaySystem(0.5);

        speak("presentation_02");
        motion.doExplanation2();
        awaitSpeechAndMotionCompletion();

        yarp::os::SystemClock::delaySystem(1.0);

        speak("composition_01");
        motion.doExplanation1();
        awaitSpeechCompletion();
        yarp::os::SystemClock::delaySystem(2.0);
        speak("composition_02");
        awaitMotionCompletion();
        yarp::os::SystemClock::delaySystem(1.0);
        motion.doExplanation3();
        awaitSpeechAndMotionCompletion();

        speak("composition_03");
        motion.doExplanationHead();
        awaitSpeechAndMotionCompletion();

        speak("composition_04");
        motion.doExplanationRightPC();
        awaitSpeechCompletion();
        speak("composition_05_01");
        awaitSpeechAndMotionCompletion();

        speak("composition_05_02");
        motion.doExplanationLeftPC();
        awaitSpeechAndMotionCompletion();

        speak("composition_05_03");
        motion.doExplanationInsidePC();
        awaitSpeechAndMotionCompletion();

        speak("composition_06");
        motion.doExplanation2();
        awaitSpeechAndMotionCompletion();

        speak("composition_07");
        motion.doExplanationSensors();
        awaitSpeechAndMotionCompletion();

        yarp::os::SystemClock::delaySystem(1.0);

        speak("purpose_01");
        motion.doExplanation1();
        awaitSpeechAndMotionCompletion();

        yarp::os::SystemClock::delaySystem(1.0);

        speak("purpose_02");
        motion.doExplanation4();
        awaitSpeechAndMotionCompletion();

        yarp::os::SystemClock::delaySystem(2.0);

        speak("ending_01");
        motion.doHoming();
        awaitSpeechAndMotionCompletion();
    }
    catch (const ThreadTerminator & terminator)
    {
        yInfo() << "Prematurely terminating presentation thread";
        return;
    }

    yInfo() << "Presentation end";
    demoCompleted = true;
}

void DialogueManager::speak(const std::string & sentenceId)
{
    yInfo() << sentenceId << "->" << sentences[sentenceId];

    if (!tts.say(sentences[sentenceId]))
    {
        yWarning() << "Unable to say" << sentenceId;
    }
}

void DialogueManager::awaitSpeechCompletion()
{
    do
    {
        if (yarp::os::Thread::isStopping())
        {
            throw ThreadTerminator();
        }

        yarp::os::SystemClock::delaySystem(0.1);
    }
    while (speechPort.getOutputCount() > 0 && !tts.checkSayDone());
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

void DialogueManager::awaitSpeechAndMotionCompletion()
{
    awaitSpeechCompletion();
    awaitMotionCompletion();
}
