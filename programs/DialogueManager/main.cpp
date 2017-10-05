// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * 
 * @ingroup teo-self-presentation_programs
 * \defgroup dialogueManager dialogueManager
 *
 * @brief Creates an instance of teo::DialogueManager.
 *
 * @section dialogueManager_legal Legal
 *
 * Copyright: 2017 (C) Universidad Carlos III de Madrid
 *
 * Author: <a href="http://roboticslab.uc3m.es/roboticslab/people/r-de-santos">Raul de Santos Rico</a>
 *
 * CopyPolicy: This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 3.0 or later
 *
 * <hr>
 *
 * This file can be edited at DialogueManager
 *
 */

#include "DialogueManager.hpp"


int main(int argc, char **argv) {

    yarp::os::ResourceFinder rf;
    bool exit;
    rf.setVerbose(true);
    rf.setDefaultContext("dialogueManager");
    rf.setDefaultConfigFile("dialogueManager.ini");
    rf.configure(argc, argv);

    teo::DialogueManager mod;
    if(rf.check("help")) {
        return mod.runModule(rf);
    }

    printf("Run \"%s --help\" for options.\n",argv[0]);
    printf("%s checking for yarp network... ",argv[0]);
    fflush(stdout);
    yarp::os::Network yarp;
    if (!yarp.checkNetwork()) {
        fprintf(stderr,"[fail]\n%s found no yarp network (try running \"yarpserver &\"), bye!\n",argv[0]);
        return -1;
    } else printf("[ok]\n");

    return mod.runModule(rf);

}

