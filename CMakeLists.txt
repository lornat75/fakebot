# Copyright: (C) iCub Facility 2013 Istituto Italiano di Tenologia
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.11)

project (FakeBot)

find_package(YARP REQUIRED)

include_directories(${YARP_INCLUDE_DIRS})

add_subdirectory(src)

#include(AddUninstallTarget)


