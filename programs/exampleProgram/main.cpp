// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * @ingroup teo-self-presentation_programs
 * \defgroup exampleProgram
 *
 * @brief Creates an instance of roboticslab::ExampleProgram
 *
 * @section exampleProgram_legal Legal
 *
 * Copyright: 2021 (C) Universidad Carlos III de Madrid
 *
 * Author: Raul
 *
 * CopyPolicy: This program is free software; you can redistribute it and/or modify
 * it under the terms of the LGPLv2.1
 *
 * <hr>
 *
 * This file can be edited at exampleProgram
 */

#include <cstdio>

#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>

#include "ExampleProgram.hpp"

int main(int argc, char ** argv)
{
    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultContext("exampleProgram");
    rf.setDefaultConfigFile("exampleProgram.ini");
    rf.configure(argc, argv);

    roboticslab::ExampleProgram mod;

    if (rf.check("help"))
    {
        return mod.runModule(rf);
    }

    std::printf("Run \"%s --help\" for options.\n", argv[0]);
    std::printf("%s checking for yarp network... ", argv[0]);
    std::fflush(stdout);

    yarp::os::Network yarp;

    if (!yarp.checkNetwork())
    {
        std::fprintf(stderr, "[fail]\n%s found no yarp network (try running \"yarpserver &\"), bye!\n", argv[0]);
        return 1;
    } else std::printf("[ok]\n");

    return mod.runModule(rf);
}
