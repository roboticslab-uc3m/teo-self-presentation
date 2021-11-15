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
        {"presentation_01", "Hi. My name is TEO. I am, an humanoid robot, designed by engineers, of the university Carlos tercero, of Madrid. I am 5 years old. My size, is 1 62 meters, and my weight, is 70 kilograms."},
        {"presentation_02", "My purpose, is to help to researchs, and to get new hits, and discoveries, within the robotics area."},
        {"composition_01", "I am mainly built, of aluminum, and carbon fiber."},
        {"composition_02", "I have 28 degrees, of freedom, that allow me to move freely, being able to do, such human tasks as walking, running, manipulating objects, doing household chores, ironing, serving as a waiter, etc."},
        {"composition_03", "In my head, I have implemented two cameras, with which, I can detect objects, and human faces. Also, I can detect the distance, and the depth, in which they are."},
        {"composition_04", "As you can see, I have two computers, in the chest, and a third, hidden underneath."},
        {"composition_05_01", "The computer on my right, serves me, to process all the tasks, related to object manipulation, as well, as my own, balance."},
        {"composition_05_02", "While, the one on my left, is dedicated, to the tasks of, locomotion."},
        {"composition_05_03", "The computer that is located, just below this, is the most powerful of all, and is dedicated, to the processing of vision. In this way, each computer, is dedicated to processing a part of the task, that I will do."},
        {"composition_06", "Below, are located the hard disks, S, S, D, which are the main memory, where I store, all the tasks and programs, that allow me to perform, all my functions."},
        {"composition_07", "Both of them, the manipulation computer, and the locomotion computer, are connected to a communication network called, CAN, BUS, which sends all movement signals, to each of my motors."},
        {"composition_08", "I also have movement, inertial, and force sensors, that allow me to detect the weight, and pressure exerted, on my joints. These sensors allow, for example, to keep me in balance, while walking or standing up."},
        {"purpose_01", "All the components that make up everything I am, as well as the programs, that allow me to perform all these tasks, have been designed by technical engineers, students and doctors. The main mission, is to discover and advance together, in the field, of robotics."},
        {"purpose_02", "For now, I am only a prototype, dedicated to research, so, I can not be sold, in any store. Also, my price would be too expensive, to be bought."},
        {"ending_01", "These, is the end, of my presentation. I hope you liked it, and if you want, you can take a picture with me. Nice to meet you."},
    };

    const std::unordered_map<std::string, std::string> spanishSentences = {
        {"presentation_01", "Hola. Me iamo TEO y soy un drobot humanoide diseniado por ingenieros de la Universidad Carlos Tercero de Madrid. Tengo 5 anios. Mido 1 62. y peso 70 kilos."},
        {"presentation_02", "He sido diseniado con el proposito de ayudar a la investigacion y a conseguir nuevos logros y descubrimientos dentro del area de la drobotica."},
        {"composition_01", "Mis piezas estan construidas principalmente de aluminio y fibra de carbono."},
        {"composition_02", "Dispongo de 28 grados de libertad que me permiten moverme con soltura, siendo capaz de hacer tareas tan humanas como andar, cogrer, manipular objetos, realizar tareas del hogar, planchar, servir de camarero, etcetera."},
        {"composition_03", "En mi cabeza tengo implementadas 2 camaras con las cuales puedo detectar objetos y caras humanas, asi como detectar la distancia y profundidad a la que se encuentran."},
        {"composition_04", "Como podeis ver, dispongo en el pecho de 2 ordenadores y un tercero escondido debajo."},
        {"composition_05_01", "El ordenador de mi derecha me sirve para procesar todas las tareas grelacionadas con manipulacion de objetos, asi como mi propio equilibrio."},
        {"composition_05_02", "Mientras que el de mi izquierda esta dedicado a las tareas de locomocion."},
        {"composition_05_03", "El ordenador que esta situado justo debajo de este, es el mas potente de todos y se encuentra dedicado al procesamiento de vision. De esta forma, cada ordenador se dedica a procesar una parte de la tarea que voy a grealizar."},
        {"composition_06", "Mas abajo, se encuentran situados los discos duros ese ese de, que son la memoria principal donde almaceno todas las tareas y programas que me permiten realizar todas mis funciones."},
        {"composition_07", "Tanto el ordenador de manipulacion como el de locomocion, estan conectados a una gred de comunicacion iamada can bus, que envia todas las seniales de movimiento, a cada uno de mis motores."},
        {"composition_08", "Tambien poseo sensores de movimiento, inerciales y de fuerza par que me permiten detectar  el peso y la presion ejercida en mis articulaciones. Estos sensores permiten por ejemplo que pueda mantenerme en equilibrio mientras ando o estoy de pie."},
        {"purpose_01", "Todos los componentes que conforman todo lo que soy, asi como los programas que me permiten grealizar todas estas tareas, han sido diseniadas por tecnicos ingenieros, estudiantes y doctores. La mision principal es poder descubrir y avanzar juntos en el campo de la grobotica."},
        {"purpose_02", "Por ahora, tan solo soy un prototipo dedicado a la investigacion, por lo que no puedo ser vendido en ninguna tienda. Ademas, mi precio seria demasiado caro para ser comprado."},
        {"ending_01", "Aqui termina mi presentacion. Espero que os haya gustado y si quereis, podeis haceros una foto conmigo."},
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
    }

    if (!motionPort.open(std::string(DEFAULT_PREFIX) + "/motion/rpc:c"))
    {
        yError() << "Unable to open RPC motion port" << motionPort.getName();
    }

    speech.yarp().attachAsClient(speechPort);
    motion.yarp().attachAsClient(motionPort);

    if (language == "english")
    {
        voice = "mb-en1";
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
    constexpr auto throttle = 1.0; // [s]

    if (speechPort.getOutputCount() == 0)
    {
        if (yarp::os::Thread::isRunning())
        {
            yInfo() << "Port disconnected, forcing presentation stop";

            if (!yarp::os::Thread::stop())
            {
                yWarning() << "Unable to stop presentation thread";
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
                yWarning() << "Unable to start presentation thread";
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
    if (!speech.setLanguage(voice))
    {
        yError() << "Unable to set voice to" << voice;
        return false;
    }

    return true;
}

void DialogueManager::threadRelease()
{
    if (!speech.stop())
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

        yarp::os::SystemClock::delaySystem(1.0);

        speak("presentation_02");
        motion.doExplanation2();
        awaitSpeechAndMotionCompletion();

        yarp::os::SystemClock::delaySystem(1.0);

        speak("composition_01");
        motion.doExplanation1();
        awaitSpeechCompletion();
        yarp::os::SystemClock::delaySystem(2.0);
        speak("composition_02");
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
        motion.doExplanationHead();
        awaitSpeechAndMotionCompletion();

        speak("composition_07");
        motion.doExplanation2();
        awaitSpeechAndMotionCompletion();

        speak("composition_08");
        motion.doExplanationSensors();
        awaitSpeechAndMotionCompletion();

        yarp::os::SystemClock::delaySystem(1.0);

        speak("purpose_01");
        motion.doExplanation1();
        awaitSpeechAndMotionCompletion();

        yarp::os::SystemClock::delaySystem(1.0);

        speak("purpose_02");
        motion.doExplanation3();
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

    if (!speech.say(sentences[sentenceId]))
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
    while (speechPort.getOutputCount() > 0 && !speech.checkSayDone());
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
