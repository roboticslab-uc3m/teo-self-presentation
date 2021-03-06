# Copyright: Universidad Carlos III de Madrid (C) 2021
# Authors: Raul de Santos Rico
# CopyPolicy: Released under the terms of the LGPLv2.1

IF (ENABLE_bodyExecution)

set(KEYWORD "bodyExecution")

# Retrieve a list of targets defined elsewhere.
get_property(_exported_targets GLOBAL PROPERTY TEO-SELF-PRESENTATION_TARGETS)

# Configure executable build target.
add_executable(${KEYWORD}
    main.cpp
    BodyExecution.cpp
    BodyExecution.hpp)

# Link external libraries.
target_link_libraries(${KEYWORD} YARP::YARP_os
                                 YARP::YARP_dev
                                 YARP::YARP_init
                                 ${_exported_targets})

# Create installation rule.
install(TARGETS ${KEYWORD}
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT example_program)

endif()
