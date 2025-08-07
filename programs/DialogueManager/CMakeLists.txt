cmake_dependent_option(ENABLE_dialogueManager "Choose if you want to compile dialogueManager" ON
                       "ENABLE_SelfPresentationCommandsIDL" OFF)

IF(ENABLE_dialogueManager)

    add_executable(dialogueManager main.cpp
                                   DialogueManager.hpp
                                   DialogueManager.cpp)

    target_link_libraries(dialogueManager YARP::YARP_os
                                          YARP::YARP_init
                                          YARP::YARP_dev
                                          ROBOTICSLAB::SelfPresentationCommandsIDL)

    install(TARGETS dialogueManager)

else()

    set(ENABLE_dialogueManager OFF CACHE BOOL "Enable/disable dialogueManager program" FORCE)

endif()
