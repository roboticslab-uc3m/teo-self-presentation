// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "DialogueManager.hpp"

namespace teo
{

/************************************************************************/

bool DialogueManager::configure(yarp::os::ResourceFinder &rf) {

    std::string language = rf.check("language",yarp::os::Value(DEFAULT_LANGUAGE),"language to be used").asString();
    
    printf("--------------------------------------------------------------\n");
    if (rf.check("help")) {
        printf("DialogueManager options:\n");
        printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        printf("\t--language (default: \"%s\")\n",language.c_str());
    }    

    printf("--------------------------------------------------------------\n");
    if(rf.check("help")) {
        ::exit(1);
    }

    //-----------------OPEN LOCAL PORTS------------//
    outTtsPort.open("/dialogueManager/tts/rpc:c");
    outCmdMovementsPort.open("/dialogueManager/movements/rpc:c");

    scriptManager.setOutTtsPort(&outTtsPort);
    scriptManager.setOutCmdMovementsPort(&outCmdMovementsPort);

    while(1){
        if(outTtsPort.getOutputCount() > 0) break;
        printf("Waiting for \"/dialogueManager/tts/rpc:c\" to be connected to something...\n");
        yarp::os::Time::delay(0.5);
    }    

    yarp::os::Bottle bConf;
    bConf.clear();

    //-- speech configuration
    if( language == "english" )
    {

        bConf.addString("setLanguage");
        bConf.addString("mb-us2");
        outTtsPort.write(bConf);
        printf("[info] Configured bottle: %s\n", bConf.toString().c_str());
        bConf.clear();

        bConf.addString("setSpeed");
        bConf.addInt(150);
        outTtsPort.write(bConf);
        printf("[info] Configured bottle: %s\n", bConf.toString().c_str());
        bConf.clear();
    }
    else if ( language == "spanish" )
    {
        bConf.addString("setLanguage");
        bConf.addString("mb-es1");
        outTtsPort.write(bConf);
        printf("[info] Configured bottle: %s\n", bConf.toString().c_str());
        bConf.clear();

        bConf.addString("setSpeed");
        bConf.addInt(170);
        outTtsPort.write(bConf);
        printf("[info] Configured bottle: %s\n", bConf.toString().c_str());
        bConf.clear();
    }
    else
    {
        printf("Language not found. Please use '--language english' or '--language spanish'");
        return false;
    }

    scriptManager.setSpeakLanguage(language);
    scriptManager.start();  //-- Start the thread (calls run).
}

/************************************************************************/
double DialogueManager::getPeriod() {
    return 2.0;  // Fixed, in seconds, the slow thread that calls updateModule below
}

/************************************************************************/
bool DialogueManager::updateModule() {
    //printf("StateMachine in state [%d]. DialogueManager alive...\n", scriptManager.getMachineState());
    return true;
}

/************************************************************************/

bool DialogueManager::interruptModule() {
    printf("DialogueManager closing...\n");
    outTtsPort.interrupt();
    outCmdMovementsPort.interrupt();
    scriptManager.stop();
    outTtsPort.close();
    outCmdMovementsPort.close();
    return true;
}

/************************************************************************/

bool DialogueManager::close() {
    printf("DialogueManager closing...\n");
    outTtsPort.interrupt();
    outCmdMovementsPort.interrupt();
    scriptManager.stop();
    outTtsPort.close();
    outCmdMovementsPort.close();
    return true;
}

/************************************************************************/

}  // namespace teo
