# Copyright: Universidad Carlos III de Madrid (C) 2021
# Authors: Raul
# CopyPolicy: Released under the terms of the LGPLv2.1

cmake_minimum_required(VERSION 3.12)

# Start a project. Set the LANGUAGES option according to your
# needs, e.g. 'C', 'CXX', 'NONE'.
project(TEO-SELF-PRESENTATION VERSION 0.1.0 LANGUAGES CXX)

# Set module path.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake
                              ${CMAKE_SOURCE_DIR}/cmake/modules
                              ${CMAKE_SOURCE_DIR}/cmake/find-modules)

# Hard dependencies.
find_package(YCM 0.11 REQUIRED)
find_package(YARP 3.4 REQUIRED COMPONENTS os dev)

# Soft dependencies.
find_package(Doxygen QUIET)
find_package(GTestSources 1.6.0 QUIET)

# Add WITH_PLUGINS if this projects exports YARP devices.
yarp_configure_external_installation(teo-self-presentation)

# Retrieve current TEO-SELF-PRESENTATION version.
include(TEO-SELF-PRESENTATIONVersion)

# Define standard paths, check system configuration.
include(TEO-SELF-PRESENTATIONSystem)

# Setup build options.
include(TEO-SELF-PRESENTATIONOptions)

# Define and enter subdirectories.
add_subdirectory(cmake)
add_subdirectory(programs)
add_subdirectory(share)
add_subdirectory(doc)

# Output configuration files.
include(TEO-SELF-PRESENTATIONDescribe)

# Invoke CPack from here.
include(TEO-SELF-PRESENTATIONPackage)

# Generate a report of enabled/disabled features.
feature_summary(WHAT ALL)
