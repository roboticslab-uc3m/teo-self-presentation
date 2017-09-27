# Copyright: UC3M 2017
# Author: Raul de Santos Rico
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
# 

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpInstallationHelpers)
yarp_configure_external_installation(teo-self-presentation)

### Go through single applications

#add_subdirectory(prog)

