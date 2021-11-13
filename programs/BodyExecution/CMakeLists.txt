cmake_dependent_option(ENABLE_bodyExecution "Choose if you want to compile bodyExecution" ON
                       ENABLE_SelfPresentationCommandsIDL OFF)

IF(ENABLE_bodyExecution)

    add_executable(bodyExecution main.cpp
                                 BodyExecution.hpp
                                 BodyExecution.cpp)

    target_link_libraries(bodyExecution YARP::YARP_os
                                        YARP::YARP_init
                                        YARP::YARP_dev
                                        ROBOTICSLAB::SelfPresentationCommandsIDL)

    install(TARGETS bodyExecution
            DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
