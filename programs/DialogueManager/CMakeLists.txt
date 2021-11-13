cmake_dependent_option(ENABLE_dialogueManager "Choose if you want to compile dialogueManager" ON
                       "ENABLE_SelfPresentationCommandsIDL;TARGET ROBOTICSLAB::SpeechIDL" OFF)

IF(ENABLE_dialogueManager)

    add_executable(dialogueManager main.cpp
                                   DialogueManager.hpp
                                   DialogueManager.cpp)

    target_link_libraries(dialogueManager YARP::YARP_os
                                          YARP::YARP_init
                                          ROBOTICSLAB::SpeechIDL
                                          ROBOTICSLAB::SelfPresentationCommandsIDL)

    install(TARGETS dialogueManager
            DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
