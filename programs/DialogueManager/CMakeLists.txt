if(NOT TARGET ROBOTICSLAB::SpeechIDL AND (NOT DEFINED ENABLE_dialogueManager OR ENABLE_dialogueManager))
    message(WARNING "ROBOTICSLAB::SpeechIDL target not found, disabling dialogueManager")
endif()

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

    install(TARGETS dialogueManager)

else()

    set(ENABLE_dialogueManager OFF CACHE BOOL "Enable/disable dialogueManager program" FORCE)

endif()
