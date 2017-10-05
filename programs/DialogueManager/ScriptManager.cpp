// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "ScriptManager.hpp"

namespace teo
{

/************************************************************************/

void ScriptManager::start() {

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_SALUTE);
        outCmdMovementsPort->write(cmd);
        ttsSay( presentation_01 );
    }

    if (_language == "english") yarp::os::Time::delay(3); // -- in english, we've to wait for the speech
    else this->waitForMovement();                         // -- in spanish (shorter speech) we've to wait for movements

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_EXPLANATION_2);
        outCmdMovementsPort->write(cmd);
        ttsSay( presentation_02 );
    }

    this->waitForMovement();

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_EXPLANATION_1);
        outCmdMovementsPort->write(cmd);
        ttsSay( composition_01 );
        yarp::os::Time::delay(2);
        ttsSay( composition_02 );
        cmd.addVocab(VOCAB_STOP);
        outCmdMovementsPort->write(cmd);
    }    

    // it doesn't need to wait

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_EXPLANATION_HEAD);
        outCmdMovementsPort->write(cmd);
        ttsSay( composition_03 );
        cmd.addVocab(VOCAB_STOP);
        outCmdMovementsPort->write(cmd);
    }

    // it doesn't need to wait

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_EXPLANATION_PC_RIGHT);
        outCmdMovementsPort->write(cmd);
        ttsSay( composition_04 );
        ttsSay( composition_05_01 );
    }

    // it doesn't need to wait

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_EXPLANATION_PC_LEFT);
        outCmdMovementsPort->write(cmd);
        ttsSay( composition_05_02 );
    }

    // it doesn't need to wait

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_EXPLANATION_PC_INSIDE);
        outCmdMovementsPort->write(cmd);
        ttsSay( composition_05_03 );
        cmd.addVocab(VOCAB_STOP);
        outCmdMovementsPort->write(cmd);
    }

    // it doesn't need to wait

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_EXPLANATION_HDD);
        outCmdMovementsPort->write(cmd);
        ttsSay( composition_06 );
        cmd.addVocab(VOCAB_STOP);
        outCmdMovementsPort->write(cmd);
    }

    // it doesn't need to wait

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_EXPLANATION_2); // mal
        outCmdMovementsPort->write(cmd);
        ttsSay( composition_07 );
        cmd.addVocab(VOCAB_STOP);
        outCmdMovementsPort->write(cmd);
    }

    // it doesn't need to wait

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_EXPLANATION_SENSOR);
        outCmdMovementsPort->write(cmd);
        ttsSay( composition_08 );

    }

    // it doesn't need to wait

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_EXPLANATION_1);
        outCmdMovementsPort->write(cmd);
        ttsSay( finality_01 );
    }

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_EXPLANATION_3);
        outCmdMovementsPort->write(cmd);
        ttsSay( finality_02 );
    }

    yarp::os::Time::delay(2); // wait forced two seconds

    {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_STATE_HOME);
        outCmdMovementsPort->write(cmd);
        ttsSay( ending_01 );
    }

}

/************************************************************************/

void ScriptManager::ttsSay(const yarp::os::ConstString& sayConstString) {

    yarp::os::Bottle bOut, bRes;
    bOut.addString("say");
    bOut.addString(sayConstString);
    outTtsPort->write(bOut,bRes);
    printf("[StateMachine] Said: %s [%s]\n", sayConstString.c_str(), bRes.toString().c_str());
    yarp::os::Time::delay(0.5);
    return;
}

/************************************************************************/

void ScriptManager::waitForMovement(){
    yarp::os::Bottle cmd;
    yarp::os::Bottle done;
    while(!done.get(0).asInt()){
        cmd.addVocab(VOCAB_RETURN_MOVEMENT_STATE);
        outCmdMovementsPort->write(cmd,done);
        yarp::os::Time::delay(0.1);
        printf("doing movement...\n");
    }
    printf("movement done :)\n");
}

/************************************************************************/

void ScriptManager::setOutTtsPort(yarp::os::RpcClient* outTtsPort) {
    this->outTtsPort = outTtsPort;
}

/************************************************************************/

void ScriptManager::setOutCmdMovementsPort(yarp::os::RpcClient* outCmdMovePort) {
    this->outCmdMovementsPort = outCmdMovePort;
}

/************************************************************************/

bool ScriptManager::setSpeakLanguage(std::string language) {

    _language = language;

    if("english" == language)
    {
        //-- speak sentences
        //printf("error! Please, configure the aplication in spanish: dialogueManager --language spanish\n");
        presentation_01 = std::string("Hi. My name is TEO. I am, an humanoid robot, designed by engineers, of the university Carlos tercero, of Madrid. I am 5 years old. My size, is 1 62 meters, and my weight, is 70 kilograms");
        presentation_02 = std::string("my purpose, is to help to researchs, and to get new hits, and discoveries, within the robotics area. ");
        composition_01 = std::string("I am mainly built, of aluminum, and carbon fiber");
        composition_02 = std::string("I have 28 degrees, of freedom, that allow me to move freely, being able to do, such human tasks as walking, running, manipulating objects, doing household chores, ironing, serving as a waiter, etc.");
        composition_03 = std::string("in my head, I have implemented two cameras, with which, I can detect objects, and human faces. Also, I can detect the distance, and the depth, in which they are");
        composition_04 = std::string("As you can see, I have two computers, in the chest, and a third, hidden underneath.");
        composition_05_01 = std::string("The computer on my right, serves me, to process all the tasks, related to object manipulation, as well, as my own, balance");
        composition_05_02 = std::string("while, the one on my left, is dedicated, to the tasks of, locomotion");
        composition_05_03 = std::string("The computer that is located, just below this, is the most powerful of all, and is dedicated, to the processing of vision. In this way, each computer, is dedicated to processing a part of the task, that I will do.");
        composition_06 = std::string("Below, are located the hard disks, S, S, D, which are the main memory, where I store, all the tasks and programs, that allow me to perform, all my functions");
        composition_07 = std::string("Both of them, the manipulation computer, and the locomotion computer, are connected to a communication network called, CAN, BUS, which sends all movement signals, to each of my motors");
        composition_08 = std::string("I also have movement, inertial, and force sensors, that allow me to detect the weight, and pressure exerted, on my joints. These sensors allow, for example, to keep me in balance, while walking or standing up");
        finality_01 = std::string("All the components that make up everything I am, as well as the programs, that allow me to perform all these tasks, have been designed by technical engineers, students and doctors. The main mission, is to discover and advance together, in the field, of robotics.");
        finality_02 = std::string("For now, I am only a prototype, dedicated to research, so, I can not be sold, in any store. Also, my price would be too expensive, to be bought.");
        ending_01 = std::string("these, is the end, of my presentation. I hope you liked it, and if you want, you can take a picture with me. Nice to meet you");

        return false;

    }
    else if("spanish" == language)
    {
        //-- speak sentences
        presentation_01 = std::string("Hola. Me iamo TEO y soy un drobot humanoide diseniado por ingenieros de la Universidad Carlos Tercero de Madrid. Tengo 5 anios. Mido 1 62. y peso 70 kilos. ");
        presentation_02 = std::string("He sido diseniado con el proposito de ayudar a la investigacion y a conseguir nuevos logros y descubrimientos dentro del area de la drobotica. ");
        composition_01 = std::string("Mis piezas estan construidas principalmente de aluminio y fibra de carbono.");
        composition_02 = std::string("Dispongo de 28 grados de libertad que me permiten moverme con soltura, siendo capaz de hacer tareas tan humanas como andar, cogrer, manipular objetos, realizar tareas del hogar, planchar, servir de camarero, etcetera");
        composition_03 = std::string("En mi cabeza tengo implementadas 2 camaras con las cuales puedo detectar objetos y caras humanas, asi como detectar la distancia y profundidad a la que se encuentran.");
        composition_04 = std::string("Como podeis ver, dispongo en el pecho de 2 ordenadores y un tercero escondido debajo.");
        composition_05_01 = std::string("El ordenador de mi derecha me sirve para procesar todas las tareas grelacionadas con manipulacion de objetos, asi como mi propio equilibrio");
        composition_05_02 = std::string("mientras que el de mi izquierda esta dedicado a las tareas de locomocion.");
        composition_05_03 = std::string("El ordenador que esta situado justo debajo de este, es el mas potente de todos y se encuentra dedicado al procesamiento de vision. De esta forma, cada ordenador se dedica a procesar una parte de la tarea que voy a grealizar.");
        composition_06 = std::string("Mas abajo, se encuentran situados los discos duros ese ese de, que son la memoria principal donde almaceno todas las tareas y programas que me permiten realizar todas mis funciones.");
        composition_07 = std::string("Tanto el ordenador de manipulacion como el de locomocion, estan conectados a una gred de comunicacion iamada can bus, que envia todas las seniales de movimiento, a cada uno de mis motores");
        composition_08 = std::string("Tambien poseo sensores de movimiento, inerciales y de fuerza par que me permiten detectar  el peso y la presion ejercida en mis articulaciones. Estos sensores permiten por ejemplo que pueda mantenerme en equilibrio mientras ando o estoy de pie.");
        finality_01 = std::string("Todos los componentes que conforman todo lo que soy, asi como los programas que me permiten grealizar todas estas tareas, han sido diseniadas por tecnicos ingenieros, estudiantes y doctores. La mision principal es poder descubrir y avanzar juntos en el campo de la grobotica. ");
        finality_02 = std::string("Por ahora, tan solo soy un prototipo dedicado a la investigacion, por lo que no puedo ser vendido en ninguna tienda. Ademas, mi precio seria demasiado caro para ser comprado.");
        ending_01 = std::string("Aqui termina mi presentacion. Espero que os haya gustado y si quereis, podeis haceros una foto conmigo.");

        return true;
    }
    else
    {
        printf("error! %s????\n",language.c_str());
        return false;
    }
}

}  // namespace teo
