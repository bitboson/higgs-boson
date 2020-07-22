/* This file is part of higgs-boson.
 *
 * Copyright (c) BitBoson
 *
 * higgs-boson is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * higgs-boson is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with higgs-boson.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Written by:
 *     - Tyler Parcell <OriginLegend>
 */

#ifndef HIGGS_BOSON_MANUAL_DEPENDENCY_TEST_HPP
#define HIGGS_BOSON_MANUAL_DEPENDENCY_TEST_HPP

#include <catch.hpp>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory>
#include <string>
#include <fstream>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/ManualDependency.h>

using namespace BitBoson;

/**
 * Test helper function used to compare two vector's contents
 *
 * @param vector1 Vector of Strings representing one of the two vectors to compare
 * @param vector2 Vector of Strings representing one of the two vectors to compare
 * @return Boolean indicating whether the two vectors are the same or not
 */
bool compareVectors(const std::vector<std::string>& vector1, const std::vector<std::string>& vector2)
{

    // Create a retrun flag
    bool retFlag = false;

    // Only continue if the vectors are the same size
    if (vector1.size() == vector2.size())
    {

        // Determine if the contents match
        bool contentsMatch = true;
        for (long ii = 0; ii < vector1.size(); ii++)
            if (vector1[ii] != vector2[ii])
                contentsMatch = false;

        // Setup the return value based on the matched contents
        retFlag = contentsMatch;
    }

    // Return the return flag
    return retFlag;
}

/**
 * Test helper function used to read a file into a vector of strings
 *
 * @param file String representing the file path to read the lines from
 * @return Vector of Strings representing the lines of the provided file
 */
std::vector<std::string> readFileIntoVector(const std::string& file)
{

    // Create the return vector
    std::vector<std::string> retVect;

    // Attempt to open up the file and only continue if this is successful
    std::string fileLine;
    std::ifstream fileToRead(file);
    if (fileToRead.is_open())
    {

        // Read-in the file-contents into the return vector
        while (std::getline(fileToRead, fileLine))
            retVect.push_back(fileLine);

        // Close the file when finished reading from it
        fileToRead.close();
    }

    // Return the return vector
    return retVect;
}

TEST_CASE ("Build Files Manual Dependency Test", "[ManualDependencyTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/dep1").c_str()) == 0);
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/dep2").c_str()) == 0);

    // Create a few manual dependencies in a random temporary directory
    auto dep1 = ManualDependency("/tmp/higgs-boson/dep1", "test-dep1", {"linux", "windows"});
    auto dep2 = ManualDependency("/tmp/higgs-boson/dep2", "test-dep2", {"linux", "darwin"});

    // Write the various build steps for the different dependencies and targets
    REQUIRE (dep1.setBuildSteps("linux", {"step 1", "step 2", "step 3"}));
    REQUIRE (dep1.setBuildSteps("windows", {"step A", "step B"}));
    REQUIRE (dep2.setBuildSteps("linux", {"step 1", "step 2", "step 3", "step 4"}));
    REQUIRE (dep2.setBuildSteps("darwin", {"step a", "step b", "step c"}));

    // Verify that the build-steps were written properly for each target
    REQUIRE (compareVectors(
            readFileIntoVector("/tmp/higgs-boson/dep1/higgs-build_linux.sh"),
            {"cd /tmp/higgs-boson/dep1",
             "HIGGS_TARGET=linux",
             "HIGGS_HEADER_DIR=/tmp/higgs-boson/dep1/higgs-boson_linux_headers",
             "HIGGS_LIBRARY_DIR=/tmp/higgs-boson/dep1/higgs-boson_linux_libraries",
             "mkdir -p /tmp/higgs-boson/dep1/higgs-boson_linux_headers",
             "mkdir -p /tmp/higgs-boson/dep1/higgs-boson_linux_libraries",
             "step 1", "step 2", "step 3"}));
    REQUIRE (compareVectors(
            readFileIntoVector("/tmp/higgs-boson/dep1/higgs-build_windows.sh"),
            {"cd /tmp/higgs-boson/dep1",
             "HIGGS_TARGET=windows",
             "HIGGS_HEADER_DIR=/tmp/higgs-boson/dep1/higgs-boson_windows_headers",
             "HIGGS_LIBRARY_DIR=/tmp/higgs-boson/dep1/higgs-boson_windows_libraries",
             "mkdir -p /tmp/higgs-boson/dep1/higgs-boson_windows_headers",
             "mkdir -p /tmp/higgs-boson/dep1/higgs-boson_windows_libraries",
             "step A", "step B"}));
    REQUIRE (compareVectors(
            readFileIntoVector("/tmp/higgs-boson/dep2/higgs-build_linux.sh"),
            {"cd /tmp/higgs-boson/dep2",
             "HIGGS_TARGET=linux",
             "HIGGS_HEADER_DIR=/tmp/higgs-boson/dep2/higgs-boson_linux_headers",
             "HIGGS_LIBRARY_DIR=/tmp/higgs-boson/dep2/higgs-boson_linux_libraries",
             "mkdir -p /tmp/higgs-boson/dep2/higgs-boson_linux_headers",
             "mkdir -p /tmp/higgs-boson/dep2/higgs-boson_linux_libraries",
             "step 1", "step 2", "step 3", "step 4"}));
    REQUIRE (compareVectors(
            readFileIntoVector("/tmp/higgs-boson/dep2/higgs-build_darwin.sh"),
            {"cd /tmp/higgs-boson/dep2",
             "HIGGS_TARGET=darwin",
             "HIGGS_HEADER_DIR=/tmp/higgs-boson/dep2/higgs-boson_darwin_headers",
             "HIGGS_LIBRARY_DIR=/tmp/higgs-boson/dep2/higgs-boson_darwin_libraries",
             "mkdir -p /tmp/higgs-boson/dep2/higgs-boson_darwin_headers",
             "mkdir -p /tmp/higgs-boson/dep2/higgs-boson_darwin_libraries",
             "step a", "step b", "step c"}));

    // Cleanup the temporary files
    REQUIRE (remove(std::string("/tmp/higgs-boson/dep1/higgs-build_linux.sh").c_str()) == 0);
    REQUIRE (remove(std::string("/tmp/higgs-boson/dep1/higgs-build_windows.sh").c_str()) == 0);
    REQUIRE (remove(std::string("/tmp/higgs-boson/dep2/higgs-build_linux.sh").c_str()) == 0);
    REQUIRE (remove(std::string("/tmp/higgs-boson/dep2/higgs-build_darwin.sh").c_str()) == 0);
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/dep1").c_str()) == 0);
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/dep2").c_str()) == 0);
}

TEST_CASE ("Bad Target Build Files Manual Dependency Test", "[ManualDependencyTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/dep1").c_str()) == 0);

    // Create a manual dependency in a random temporary directory
    auto dep1 = ManualDependency("/tmp/higgs-boson/dep1", "test-dep1", {"linux", "windows"});

    // Attemp to write the build-steps for a bad target
    REQUIRE (!dep1.setBuildSteps("darwin", {"step a", "step b", "step c"}));

    // Verify that the build-steps were not written to the corresponding target
    REQUIRE (compareVectors(
            readFileIntoVector("/tmp/higgs-boson/dep1/higgs-build_linux.sh"), {}));
    REQUIRE (compareVectors(
            readFileIntoVector("/tmp/higgs-boson/dep1/higgs-build_windows.sh"), {}));
    REQUIRE (compareVectors(
            readFileIntoVector("/tmp/higgs-boson/dep1/higgs-build_darwin.sh"), {}));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/dep1").c_str()) == 0);
}

TEST_CASE ("Good Compile Target Manual Dependency Test", "[ManualDependencyTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/dep1").c_str()) == 0);

    // Create a manual dependency in a random temporary directory
    auto dep1 = ManualDependency("/tmp/higgs-boson/dep1", "test-dep1", {"linux", "windows"});

    // Attemp to write the build-steps for a valid target
    REQUIRE (dep1.setBuildSteps("linux", {"echo Hello World", "echo Howdy Yall"}));

    // Verify that the build-steps were written to the corresponding target
    REQUIRE (compareVectors(
            readFileIntoVector("/tmp/higgs-boson/dep1/higgs-build_linux.sh"),
            {"cd /tmp/higgs-boson/dep1",
             "HIGGS_TARGET=linux",
             "HIGGS_HEADER_DIR=/tmp/higgs-boson/dep1/higgs-boson_linux_headers",
             "HIGGS_LIBRARY_DIR=/tmp/higgs-boson/dep1/higgs-boson_linux_libraries",
             "mkdir -p /tmp/higgs-boson/dep1/higgs-boson_linux_headers",
             "mkdir -p /tmp/higgs-boson/dep1/higgs-boson_linux_libraries",
             "echo Hello World", "echo Howdy Yall"}));

    // Run the compilation command and ensure it runs correctly
    REQUIRE (dep1.compileTarget("linux"));

    // Cleanup the temporary files
    REQUIRE (remove(std::string("/tmp/higgs-boson/dep1/higgs-build_linux.sh").c_str()) == 0);
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/dep1").c_str()) == 0);
}

TEST_CASE ("Bad Compile Target Manual Dependency Test", "[ManualDependencyTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/dep1").c_str()) == 0);

    // Create a manual dependency in a random temporary directory
    auto dep1 = ManualDependency("/tmp/higgs-boson/dep1", "test-dep1", {"linux", "windows"});

    // Attemp to write the build-steps for a valid target
    REQUIRE (dep1.setBuildSteps("linux", {"bad-command-execution"}));

    // Verify that the build-steps were written to the corresponding target
    REQUIRE (compareVectors(
            readFileIntoVector("/tmp/higgs-boson/dep1/higgs-build_linux.sh"),
            {"cd /tmp/higgs-boson/dep1",
             "HIGGS_TARGET=linux",
             "HIGGS_HEADER_DIR=/tmp/higgs-boson/dep1/higgs-boson_linux_headers",
             "HIGGS_LIBRARY_DIR=/tmp/higgs-boson/dep1/higgs-boson_linux_libraries",
             "mkdir -p /tmp/higgs-boson/dep1/higgs-boson_linux_headers",
             "mkdir -p /tmp/higgs-boson/dep1/higgs-boson_linux_libraries",
             "bad-command-execution"}));

    // Run the compilation command and ensure it runs with a failure
    REQUIRE (!dep1.compileTarget("darwin"));
    REQUIRE (!dep1.compileTarget("linux"));

    // Cleanup the temporary files
    REQUIRE (remove(std::string("/tmp/higgs-boson/dep1/higgs-build_linux.sh").c_str()) == 0);
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/dep1").c_str()) == 0);
}

TEST_CASE ("Get Libraries For Target Manual Dependency Test", "[ManualDependencyTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/dep1").c_str()) == 0);

    // Create a manual dependency in a random temporary directory
    auto dep1 = ManualDependency("/tmp/higgs-boson/dep1", "test-dep1", {"linux", "windows"});

    // Write files into the directory for the linux target
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/dep1/higgs-boson_linux_libraries").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/dep1/higgs-boson_linux_libraries/library.so").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/dep1/higgs-boson_linux_libraries/library1.so").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/dep1/higgs-boson_linux_libraries/library2.so").c_str()) == 0);

    // Write files into the directory for the windows target
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/dep1/higgs-boson_windows_libraries").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/dep1/higgs-boson_windows_libraries/lib1.dll").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/dep1/higgs-boson_windows_libraries/lib2.dll").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/dep1/higgs-boson_windows_libraries/lib3.dll").c_str()) == 0);

    // Get the libraries for the linux target from the dependency object
    REQUIRE (compareVectors(dep1.getLibraries("linux"),
            {"/tmp/higgs-boson/dep1/higgs-boson_linux_libraries/library.so",
             "/tmp/higgs-boson/dep1/higgs-boson_linux_libraries/library1.so",
             "/tmp/higgs-boson/dep1/higgs-boson_linux_libraries/library2.so"}));

    // Get the libraries for the windows target from the dependency object
    REQUIRE (compareVectors(dep1.getLibraries("windows"),
            {"/tmp/higgs-boson/dep1/higgs-boson_windows_libraries/lib1.dll",
             "/tmp/higgs-boson/dep1/higgs-boson_windows_libraries/lib2.dll",
             "/tmp/higgs-boson/dep1/higgs-boson_windows_libraries/lib3.dll"}));

    // Get the libraries for the darwin target from the dependency object
    REQUIRE (compareVectors(dep1.getLibraries("darwin"), {}));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/dep1").c_str()) == 0);
}

TEST_CASE ("Copy Libraries Compile Target Manual Dependency Test", "[ManualDependencyTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/dep1").c_str()) == 0);

    // Create a manual dependency in a random temporary directory
    auto dep1 = ManualDependency("/tmp/higgs-boson/dep1", "test-dep1", {"linux", "windows"});

    // Attemp to write the build-steps for a valid target
    REQUIRE (dep1.setBuildSteps("linux",
            {"touch /tmp/higgs-boson/dep1/library.so",
             "touch /tmp/higgs-boson/dep1/library1.so",
             "touch /tmp/higgs-boson/dep1/library2.so"}));

    // Run the compilation command and ensure it runs correctly
    REQUIRE (dep1.compileTarget("linux",
            {"library.so",
             "library1.so",
             "library2.so"}));

    // Get the libraries for the linux target from the dependency object
    REQUIRE (compareVectors(dep1.getLibraries("linux"),
            {"/tmp/higgs-boson/dep1/higgs-boson_linux_libraries/library.so",
             "/tmp/higgs-boson/dep1/higgs-boson_linux_libraries/library1.so",
             "/tmp/higgs-boson/dep1/higgs-boson_linux_libraries/library2.so"}));

    // Cleanup the temporary files
    REQUIRE (remove(std::string("/tmp/higgs-boson/dep1/higgs-build_linux.sh").c_str()) == 0);
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/dep1").c_str()) == 0);
}

TEST_CASE ("Copy Headers Compile Target Manual Dependency Test", "[ManualDependencyTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/dep1").c_str()) == 0);

    // Create a manual dependency in a random temporary directory
    auto dep1 = ManualDependency("/tmp/higgs-boson/dep1", "test-dep1", {"linux", "windows"});

    // Attemp to write the build-steps for a valid target
    REQUIRE (dep1.setBuildSteps("linux",
            {"mkdir -p /tmp/higgs-boson/dep1/headers",
             "mkdir -p /tmp/higgs-boson/dep1/headers/dir",
             "mkdir -p /tmp/higgs-boson/dep1/headers/test",
             "touch /tmp/higgs-boson/dep1/headers/header1.h",
             "touch /tmp/higgs-boson/dep1/headers/header2.hpp",
             "touch /tmp/higgs-boson/dep1/headers/dir/header1.h",
             "touch /tmp/higgs-boson/dep1/headers/dir/header2.h",
             "touch /tmp/higgs-boson/dep1/headers/dir/header3.h",
             "touch /tmp/higgs-boson/dep1/headers/test/blah.h",
             "touch /tmp/higgs-boson/dep1/headers/test/blah2.h"}));

    // Run the compilation command and ensure it runs correctly
    REQUIRE (dep1.compileTarget("linux", {},
            {"headers"}));

    // Cleanup the temporary files
    REQUIRE (remove(std::string("/tmp/higgs-boson/dep1/higgs-build_linux.sh").c_str()) == 0);
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/dep1").c_str()) == 0);
}

#endif //HIGGS_BOSON_MANUAL_DEPENDENCY_TEST_HPP
