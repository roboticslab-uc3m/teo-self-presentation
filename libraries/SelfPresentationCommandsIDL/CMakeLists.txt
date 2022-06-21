option(ENABLE_SelfPresentationCommandsIDL "Enable/disable SelfPresentationCommandsIDL library" ON)

if(ENABLE_SelfPresentationCommandsIDL)

    set(ALLOW_IDL_GENERATION ON CACHE BOOL "Detect changes and rebuild IDL files")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS SelfPresentationCommands.thrift)

    yarp_idl_to_dir(INPUT_FILES SelfPresentationCommands.thrift
                    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
                    SOURCES_VAR idl_sources
                    HEADERS_VAR idl_headers
                    INCLUDE_DIRS_VAR idl_include_dirs
                    PLACEMENT MERGED
                    THRIFT_NO_NAMESPACE_PREFIX)

    add_library(SelfPresentationCommandsIDL SHARED ${idl_sources} ${idl_headers})

    set_target_properties(SelfPresentationCommandsIDL PROPERTIES PUBLIC_HEADER "${idl_headers}")

    target_link_libraries(SelfPresentationCommandsIDL PUBLIC YARP::YARP_os)

    target_include_directories(SelfPresentationCommandsIDL PUBLIC ${idl_include_dirs})

    install(TARGETS SelfPresentationCommandsIDL)

    add_library(ROBOTICSLAB::SelfPresentationCommandsIDL ALIAS SelfPresentationCommandsIDL)

endif()
