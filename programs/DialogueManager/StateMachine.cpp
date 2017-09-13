// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "StateMachine.hpp"

namespace teo
{

/************************************************************************/

bool StateMachine::threadInit() {
    //_machineState = 3;
    //sentence = 'a';
    printf("Starting SELF PRESENTATION...\n");
    yarp::os::Time::delay(1);
    return true;
}

/************************************************************************/

void StateMachine::run() {

    yarp::os::Time::delay(0.5);

    ttsSay( presentation_01 );
    ttsSay( presentation_02 );
    ttsSay( composition_01 );
    ttsSay( composition_02 );
    ttsSay( composition_03 );
    ttsSay( composition_04 );
    ttsSay( composition_05 );
    ttsSay( composition_06 );
    ttsSay( composition_07 );
    ttsSay( composition_08 );
    ttsSay( finality_01 );
    ttsSay( ending_01 );


    //yarp::os::Bottle cmd;
    //cmd.addVocab(VOCAB_STATE_SALUTE);
    //outCmdArmPort->write(cmd);

}

/************************************************************************/

void StateMachine::ttsSay(const yarp::os::ConstString& sayConstString) {
    /*yarp::os::Bottle bOutStop, bResStop;
    bOutStop.addString("stop");
    outTtsPort->write(bOutStop,bResStop);

    yarp::os::Time::delay(0.5);*/

    yarp::os::Bottle bOut, bRes;
    bOut.addString("say");
    bOut.addString(sayConstString);
    outTtsPort->write(bOut,bRes);
    printf("[StateMachine] Said: %s [%s]\n", sayConstString.c_str(), bRes.toString().c_str());
    yarp::os::Time::delay(0.5);
    return;
}

/************************************************************************/

int StateMachine::getMachineState() {
    return _machineState;
}


/************************************************************************/

void StateMachine::setOutTtsPort(yarp::os::RpcClient* outTtsPort) {
    this->outTtsPort = outTtsPort;
}

/************************************************************************/

bool StateMachine::setSpeakLanguage(std::string language) {

    if("english" == language)
    {
        //-- speak sentences


        return true;

    }
    else if("spanish" == language)
    {
        //-- speak sentences
        presentation_01 = std::string("Hola. Me iamo TEO y soy un drobot humanoide diseniado por ingenieros de la Universidad Carlos Tercero de Madrid. Mi edad es de 5 anios. Mido 1 62. y peso 70 kilos. ");
        presentation_02 = std::string("He sido diseniado con el proposito de ayudar a la investigacion y a conseguir nuevos logros y descubrimientos dentro del area de la drobotica. ");
        composition_01 = std::string("Mis piezas estan construidas principalmente de aluminio y fibra de carbono.");
        composition_02 = std::string("Dispongo de 28 grados de libertad que me permiten moverme con soltura, siendo capaz de hacer tareas tan humanas como andar, cogrer, manipular objetos, realizar tareas del hogar, planchar, servir de camarero, etcetera");
        composition_03 = std::string("En mi cabeza tengo implementadas 2 camaras con las cuales puedo detectar objetos y caras humanas, asi como detectar la distancia y profundidad a la que se encuentran.");
        composition_04 = std::string("Como podeis ver, dispongo en el pecho de 2 ordenadores y un tercero escondido debajo.");
        composition_05 = std::string("El ordenador de mi derecha me sirve para procesar todas las tareas grelacionadas con manipulacion de objetos, asi como mi propio equilibrio, mientras que el de mi izquierda esta dedicado a las tareas de locomocion. El ordenador que esta situado justo debajo de este, es el mas potente de todos y se encuentra dedicado al procesamiento de vision. De esta forma, cada ordenador se dedica a procesar una parte de la tarea que voy a grealizar.");
        composition_06 = std::string("Mas abajo, se encuentran situados los discos duros ese ese de, que son la memoria principal donde almaceno todas las tareas y programas que me permiten realizar todas mis funciones.");
        composition_07 = std::string("Tanto el ordenador de manipulacion como el de locomocion, estan conectados a una gred de comunicacion iamada can bus, que envia todas las seniales de movimiento, a cada uno de mis motores");
        composition_08 = std::string("Tambien poseo sensores de movimiento, inerciales y de fuerza par que me permiten detectar  el peso y la presion ejercida en mis articulaciones. Estos sensores permiten por ejemplo que pueda mantenerme en equilibrio mientras ando o estoy de pie.");
        finality_01 = std::string("Todos los componentes que conforman todo lo que soy, asi como los programas que me permiten grealizar todas estas tareas, han sido diseniadas por tecnicos ingenieros, estudiantes y doctores. La mision principal es poder descubrir y avanzar juntos en el campo de la grobotica. Por ahora, tan solo soy un prototipo dedicado a la investigacion, por lo que no puedo ser vendido en ninguna tienda. Ademas, mi precio seria demasiado caro para ser comprado.");
        ending_01 = std::string("Aqui termina mi presentacion. Espero que os haya gustado y si quereis, podeis haceros una foto conmigo.");
    }
    else
    {
        printf("error! %s????\n",language.c_str());
        return false;
    }
}

}  // namespace teo
