# This file is part of higgs-boson.
#
# Copyright (c) BitBoson
#
# higgs-boson is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# higgs-boson is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with higgs-boson.  If not, see <https://www.gnu.org/licenses/>.
#
# Written by:
#     - Tyler Parcell <OriginLegend>
#

# Setup the CMake minimum requirements
cmake_minimum_required(VERSION 3.0.0)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)

# Higgs Build Variables
set(HIGGS_PROJECT_NAME "higgs-boson")
set(HIGGS_PROJECT_SRC "${HIGGS_BOSON_MANUAL_DIR}")
set(HIGGS_PROJECT_VERSION "0.1.0")

# Project Specifics
set(PROJECTNAME "${HIGGS_PROJECT_NAME}")
project(${PROJECTNAME} CXX)

# Project Main Targets
set(PROJECT_TARGET_MAIN "${PROJECTNAME}")
set(PROJECT_TARGET_TEST "${PROJECTNAME}_test")

# Project Outputs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Setup the project version
set(VERSION "${HIGGS_PROJECT_VERSION}")

# Setup the hard-coded source directory
set(CMAKE_SOURCE_DIR ${HIGGS_PROJECT_SRC})
set(CMAKE_CURRENT_SOURCE_DIR ${HIGGS_PROJECT_SRC})


#
#
# Setup External dependencies
#

# Add the include directories
set(HIGGS_EXTERNAL_INCLUDES
    "${HIGGS_BOSON_MANUAL_DIR}/.higgs-boson/external/raw/mini-yaml/higgs-boson_default_headers"
)

# Setup the library and linker information
set(HIGGS_EXTERNAL_LIBS
    "${HIGGS_BOSON_MANUAL_DIR}/.higgs-boson/output/manual/miniyaml/libminiyaml.so"
)

# Add threading to the build process
find_package(Threads)
SET(CMAKE_CXX_FLAGS "-pthread -std=c++17")

#
#
# Setup Target: Main Project
#

# Setup the include directories
include_directories(${PROJECT_TARGET_MAIN} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

# Setup Headers
set(${PROJECT_TARGET_MAIN}_headers
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Utils/Utils.h"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/HiggsBoson.h"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Settings/PeruSettings.h"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Utils/ExecShell.h"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Settings/CMakeSettings.h"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Dependencies/ManualDependency.h"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Settings/ProjectSettings.h"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Dependencies/HiggsBosonDependency.h"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Dependencies/Dependency.h"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Configuration.h"
)

# C++ Source Files
set(${PROJECT_TARGET_MAIN}_sources
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/HiggsBoson.cpp"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Utils/ExecShell.cpp"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Settings/ProjectSettings.cpp"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Settings/CMakeSettings.cpp"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Dependencies/ManualDependency.cpp"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Dependencies/HiggsBosonDependency.cpp"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Utils/Utils.cpp"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Dependencies/Dependency.cpp"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Settings/PeruSettings.cpp"
        "${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/Configuration/Configuration.cpp"
)

# Create the actual library for main project
add_executable(${PROJECT_TARGET_MAIN} ${HIGGS_BOSON_MANUAL_DIR}/src/BitBoson/HiggsBoson/main.cpp
        ${${PROJECT_TARGET_MAIN}_sources} ${${PROJECT_TARGET_MAIN}_headers})
target_link_libraries(${PROJECT_TARGET_MAIN} ${HIGGS_EXTERNAL_LIBS})

# Setup include directories for the main project
include_directories(${PROJECT_TARGET_MAIN} "${HIGGS_EXTERNAL_INCLUDES}")