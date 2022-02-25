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

#include <stdlib.h>
#include <BitBoson/HiggsBoson/HiggsBoson.h>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
#include <BitBoson/HiggsBoson/Utils/FileWriter.h>
#include <BitBoson/HiggsBoson/Configuration/Settings/CMakeSettings.h>

using namespace BitBoson;

/**
 * Constructor used to setup the CMake settings object instance
 *
 * @param projectName String representing the project's name
 * @param projectVersion String representing the project's version
 * @param cMakeBuildDir String representing the path to perform
 *                      CMake sync operations in/to
 * @param cMakeCacheDir String representing the path to perform
 *                      CMake cache operations in/to
 */
CMakeSettings::CMakeSettings(const std::string& projectName,
        const std::string& projectVersion, const std::string& cMakeBuildDir,
        const std::string& cMakeCacheDir)
{

    // Setup the memver variables
    _projectName = projectName;
    _projectVersion = projectVersion;
    _cMakeBuildDir = cMakeBuildDir;
    _cMakeCacheDir = cMakeCacheDir;
    _cMakeFile = _cMakeCacheDir + "/CMakeLists.txt";

    // Ensure the build and chache directories exists
    HiggsBoson::RunTypeSingleton::executeInContainer("mkdir -p " + _cMakeBuildDir);
    HiggsBoson::RunTypeSingleton::executeInContainer("mkdir -p " + _cMakeCacheDir);
}

/**
 * Function used to set a main source-file to the CMake configuration
 *
 * @param mainSource String representing the path to the main source file
 */
void CMakeSettings::setMainSource(const std::string& mainSource)
{

    // Setup the main source file value
    _mainFile = mainSource;
}

/**
 * Function used to add a source-file to the CMake configuration
 *
 * @param sourceFile String representing the path to the source file
 * @return Boolean indicating whether the operation was successful
 */
bool CMakeSettings::addSourceFile(const std::string& sourceFile)
{

    // Create a return flag
    bool retFlag = false;

    // Only continue if the provided file does not exist already
    if (_sourceFiles.find(sourceFile) == _sourceFiles.end())
    {

        // Add-in the file to the map
        _sourceFiles[sourceFile] = true;

        // Indicate that the operation was successful
        retFlag = true;
    }

    // Return the return flag
    return retFlag;
}

/**
 * Function used to add a header-file to the CMake configuration
 *
 * @param headerFile String representing the path to the header file
 * @return Boolean indicating whether the operation was successful
 */
bool CMakeSettings::addHeaderFile(const std::string& headerFile)
{

    // Create a return flag
    bool retFlag = false;

    // Only continue if the provided file does not exist already
    if (_headerFiles.find(headerFile) == _headerFiles.end())
    {

        // Add-in the file to the map
        _headerFiles[headerFile] = true;

        // Indicate that the operation was successful
        retFlag = true;
    }

    // Return the return flag
    return retFlag;
}

/**
 * Function used to add a testing-file to the CMake configuration
 *
 * @param testingFile String representing the path to the testing file
 * @return Boolean indicating whether the operation was successful
 */
bool CMakeSettings::addTestingFile(const std::string& testingFile)
{

    // Create a return flag
    bool retFlag = false;

    // Only continue if the provided file does not exist already
    if (_testFiles.find(testingFile) == _testFiles.end())
    {

        // Add-in the file to the map
        _testFiles[testingFile] = true;

        // Indicate that the operation was successful
        retFlag = true;
    }

    // Return the return flag
    return retFlag;
}

/**
 * Function used to add a pre-build command to the configuration
 *
 * @param preBuildCommand String representing the pre-build command
 */
void CMakeSettings::addPreBuildCommand(const std::string& preBuildCommand)
{

    // Simply add the command to the list of commands
    _preBuildCommands.push_back(preBuildCommand);
}

/**
 * Function used to add a post-build command to the configuration
 *
 * @param postBuildCommand String representing the post-build command
 */
void CMakeSettings::addPostBuildCommand(const std::string& postBuildCommand)
{

    // Simply add the command to the list of commands
    _postBuildCommands.push_back(postBuildCommand);
}

/**
 * Function used to add a pre-test command to the configuration
 *
 * @param preTestCommand String representing the pre-test command
 */
void CMakeSettings::addPreTestCommand(const std::string& preTestCommand)
{

    // Simply add the command to the list of commands
    _preTestCommands.push_back(preTestCommand);
}

/**
 * Function used to add a post-test command to the configuration
 *
 * @param postTestCommand String representing the post-test command
 */
void CMakeSettings::addPostTestCommand(const std::string& postTestCommand)
{

    // Simply add the command to the list of commands
    _postTestCommands.push_back(postTestCommand);
}

/**
 * Function used to add a shared library to the configuration
 *
 * @param libraryPath String representing the library path
 */
void CMakeSettings::addLibrary(const std::string& libraryPath)
{

    // Simply add the path to the list of paths
    _externalLibraries.push_back(libraryPath);
}

/**
 * Function used to add an includes path to the configuration
 *
 * @param includesPath String representing the includes path
 */
void CMakeSettings::addIncludeDir(const std::string& includesPath)
{

    // Simply add the path to the list of paths
    _externalIncludes.push_back(includesPath);
}

/**
 * Function used to compile/build the CMake project for the target
 *
 * @param target String representing the target to compile for
 * @return Boolean indicating whether the operation was successful
 */
bool CMakeSettings::buildCMakeProject(const std::string& target)
{

    // Create a return flag
    bool retFlag = false;

    // Write-out the CMakeLists.txt file
    auto wroteFile = writeCMakeFile(false);

    // Force a re-build by deleting the build directory itself
    // NOTE: This is a current workaround and should be removed
    HiggsBoson::RunTypeSingleton::executeInContainer("rm -rf " + _cMakeCacheDir + "/builds/compile/" + target);

    // Create the build directory for CMake to actually use
    if (wroteFile && HiggsBoson::RunTypeSingleton::executeInContainer(
        "mkdir -p " + _cMakeCacheDir + "/builds/compile/" + target))
    {

        // Write the build workflow for the specified target
        bool wroteBuildFile = false;
        auto buildFile = FileWriter(_cMakeCacheDir + "/builds/compile-" + target + ".sh");
        if (buildFile.isOpen())
        {

            // Write-in the standard Higgs-Boson header for the build file
            buildFile.writeLine("# THIS IS AN AUTOGENERATED FILE USING HIGGS");
            buildFile.writeLine("# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)");
            buildFile.writeLine("set -e");
            buildFile.writeLine("");

            // Write-in the standard build file information for the target
            buildFile.writeLine("# Build Steps for the Compile operation for target " + target);
            buildFile.writeLine("mkdir -p " + _cMakeCacheDir + "/builds/compile/" + target);
            buildFile.writeLine("cd " + _cMakeCacheDir + "/builds/compile/" + target);
            buildFile.write("cmake");
            if (target == "default")
                buildFile.write(" -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++");
            buildFile.writeLine(" -DCMAKE_BUILD_TYPE=Release " + _cMakeCacheDir);
            buildFile.writeLine("");

            // Close the build file
            buildFile.close();

            // If we get here, mark the operation as successful
            wroteBuildFile = true;
        }

        // Write-out the make command file
        bool wroteMake = false;
        auto makeShellFile = FileWriter(_cMakeCacheDir + "/builds/compile-" + target + ".make.sh");
        if (makeShellFile.isOpen())
        {

            // Write-in the standard Higgs-Boson header for the Make Shell file
            makeShellFile.writeLine("# THIS IS AN AUTOGENERATED FILE USING HIGGS");
            makeShellFile.writeLine("# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)");
            makeShellFile.writeLine("");

            // Write-in the pre-build commands
            makeShellFile.writeLine("# Pre-Build commands for the Process");
            for (const auto& preBuildCmd : _preBuildCommands)
                makeShellFile.writeLine(preBuildCmd);
            makeShellFile.writeLine("");

            // Write-in the actual Make command
            makeShellFile.writeLine("# Run the Make Operation: Compile Target " + target);
            makeShellFile.writeLine("cd " + _cMakeCacheDir + "/builds/compile/" + target + " && make " + _projectName);
            makeShellFile.writeLine("");

            // Write-in the post-build commands
            makeShellFile.writeLine("# Post-Build commands for the Process");
            for (const auto& postBuildCmd : _postBuildCommands)
                makeShellFile.writeLine(postBuildCmd);
            makeShellFile.writeLine("");

            // Close the shell-file
            makeShellFile.close();

            // Indicate that the Make Shell file was written properly
            wroteMake = true;
        }

        // Only continue if the build file was written successfully
        if (wroteBuildFile && wroteMake)
        {

            // Run the build workflow and keep track of the results
            retFlag = (HiggsBoson::RunTypeSingleton::executeInContainer(
                    "Setting-Up Build for " + _projectName + " Version " + _projectVersion,
                    "bash " + _cMakeCacheDir + "/builds/compile-" + target + ".sh"));
            std::cout << "Building " + _projectName + " Version " + _projectVersion << std::endl;
            retFlag = (retFlag && HiggsBoson::RunTypeSingleton::executeInContainer(
                    "bash " + _cMakeCacheDir + "/builds/compile-" + target + ".make.sh"));
        }
    }

    // Return the return flag
    return retFlag;
}

/**
 * Function used to compile/build the CMake project for the target
 *
 * @param testType TestType representing CMake test to be run
 * @param testFilter String representing the test filter to apply
 * @return Boolean indicating whether the operation was successful
 */
bool CMakeSettings::testCMakeProject(TestType testType, const std::string& testFilter)
{

    // Create a return flag
    bool retFlag = false;

    // Write-out the CMakeLists.txt file
    auto wroteFile = writeCMakeFile(true);

    // Determine the test-type string based on the provided enum
    std::string testTypeString;
    std::string testCMakeVarString;
    switch (testType)
    {

        // Handle the "COVERAGE" enumeration case
        case COVERAGE:
            testTypeString = "coverage";
            testCMakeVarString = "-DCODE_COVERAGE=1";
            break;

        // Handle the "SANITIZE_ADDRESS" enumeration case
        case SANITIZE_ADDRESS:
            testTypeString = "address";
            testCMakeVarString = "-DSANITIZE_ADDRESS=1";
            break;

        // Handle the "SANITIZE_BEHAVIOR" enumeration case
        case SANITIZE_BEHAVIOR:
            testTypeString = "behavior";
            testCMakeVarString = "-DSANITIZE_BEHAVIOR=1";
            break;

        // Handle the "SANITIZE_THREAD" enumeration case
        case SANITIZE_THREAD:
            testTypeString = "thread";
            testCMakeVarString = "-DSANITIZE_THREAD=1";
            break;

        // Handle the "SANITIZE_LEAK" enumeration case
        case SANITIZE_LEAK:
            testTypeString = "leak";
            testCMakeVarString = "-DSANITIZE_LEAK=1";
            break;

        // Handle the "PROFILE" enumeration case
        case PROFILE:
            testTypeString = "profile";
            break;

        // Handle the default case
        default:
            testTypeString = "test";
    }

    // Create the build directory for CMake to actually use
    if (wroteFile && HiggsBoson::RunTypeSingleton::executeInContainer(
        "mkdir -p " + _cMakeCacheDir + "/builds/" + testTypeString))
    {

        // Write the build workflow for the specified target
        bool wroteBuildFile = false;
        auto buildFile = FileWriter(_cMakeCacheDir + "/builds/" + testTypeString + ".sh");
        if (buildFile.isOpen())
        {

            // Write-in the standard Higgs-Boson header for the build file
            buildFile.writeLine("# THIS IS AN AUTOGENERATED FILE USING HIGGS");
            buildFile.writeLine("# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)");
            buildFile.writeLine("set -e");
            buildFile.writeLine("");

            // Write-in the standard build file information for the target
            buildFile.writeLine("# Build Steps for the Test operation " + testTypeString);
            buildFile.writeLine("mkdir -p " + _cMakeCacheDir + "/builds/" + testTypeString);
            buildFile.writeLine("cd " + _cMakeCacheDir + "/builds/" + testTypeString);
            buildFile.write("cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++");
            if (testType == TestType::COVERAGE)
                buildFile.write(" -DCODE_COVERAGE=ON ");
            buildFile.writeLine(" -DCMAKE_BUILD_TYPE=Debug " + _cMakeCacheDir + " " + testCMakeVarString);
            buildFile.writeLine("");

            // Close the build file
            buildFile.close();

            // If we get here, mark the operation as successful
            wroteBuildFile = true;
        }

        // Only continue if the build file was written successfully
        if (wroteBuildFile)
        {

            // Setup the LD_LIBRARY_PATH variable for running the tests
            std::string libraryLdPathString = "LD_LIBRARY_PATH=\"" + _cMakeBuildDir + "/output/default/deps\"";

            // Setup the make command for running the test
            std::string makeCommand = "cd " + _cMakeCacheDir + "/builds/" + testTypeString;
            if (testType == TestType::COVERAGE)
                makeCommand += " && " + libraryLdPathString + " make " + _projectName + "_test_coverage";
            else
                makeCommand += " && " + libraryLdPathString + " make " + _projectName + "_test";

            // Write-out the make command file
            bool wroteMake = false;
            auto makeShellFile = FileWriter(_cMakeCacheDir + "/builds/" + testTypeString + ".make.sh");
            if (makeShellFile.isOpen())
            {

                // Write-in the standard Higgs-Boson header for the Make Shell file
                makeShellFile.writeLine("# THIS IS AN AUTOGENERATED FILE USING HIGGS");
                makeShellFile.writeLine("# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)");
                makeShellFile.writeLine("");

                // Write-in the pre-test commands
                makeShellFile.writeLine("# Pre-Test commands for the Test");
                for (const auto& preTestCmd : _preTestCommands)
                    makeShellFile.writeLine(preTestCmd);
                makeShellFile.writeLine("");

                // Write-in the actual Make command
                makeShellFile.writeLine("# Run the Make Operation: " + testTypeString);
                makeShellFile.writeLine(makeCommand);
                if ((testType != TestType::COVERAGE) && (testType != TestType::DEBUG) && (testType != TestType::PROFILE))
                    makeShellFile.writeLine(libraryLdPathString + " " + _cMakeCacheDir + "/builds/" + testTypeString + "/bin/" + _projectName + "_test " + testFilter);
                else if (testType == TestType::DEBUG)
                    makeShellFile.writeLine(libraryLdPathString + " gdb " + _cMakeCacheDir + "/builds/" + testTypeString + "/bin/" + _projectName + "_test");
                else if (testType == TestType::PROFILE)
                    makeShellFile.writeLine(libraryLdPathString + " valgrind --tool=callgrind --separate-threads=yes " + _cMakeCacheDir + "/builds/" + testTypeString + "/bin/" + _projectName + "_test " + testFilter);
                makeShellFile.writeLine("");

                // Write-in the post-test commands
                makeShellFile.writeLine("# Post-Test commands for the Test");
                for (const auto& postTestCmd : _postTestCommands)
                    makeShellFile.writeLine(postTestCmd);
                makeShellFile.writeLine("");

                // Close the shell-file
                makeShellFile.close();

                // Indicate that the Make Shell file was written properly
                wroteMake = true;
            }

            // Only continue if the make file was written properly
            if (wroteMake)
            {

                // Run the build workflow and keep track of the results
                retFlag = (HiggsBoson::RunTypeSingleton::executeInContainer(
                        "Setting-Up Test " + testTypeString + " for " + _projectName + " Version " + _projectVersion,
                        "bash " + _cMakeCacheDir + "/builds/" + testTypeString + ".sh"));
                std::cout << "Running " + _projectName + " Version " + _projectVersion;
                std::cout << " for Test " + testTypeString << std::endl;
                retFlag = (retFlag && HiggsBoson::RunTypeSingleton::executeInContainer(
                        "bash " + _cMakeCacheDir + "/builds/" + testTypeString + ".make.sh"));
            }
        }
    }

    // Return the return flag
    return retFlag;
}

/**
 * Internal function used to write the CMake file to the pre-defined location
 *
 * @param isTesting Boolean indicating whether to write-out a test or not
 * @return Boolean indicating if the operation was successful or not
 */
bool CMakeSettings::writeCMakeFile(bool isTesting)
{

    // Create a return flag
    bool retFlag = false;

    // Setup the main source file based on if this is a test or not
    std::string mainSourceFile;
    if (!isTesting)
        mainSourceFile = _mainFile;

    // Open the sanitize-blacklist file
    bool wroteSanitize = false;
    auto sanitizeFile = FileWriter(_cMakeCacheDir + "/sanitize-blacklist.txt");
    if (sanitizeFile.isOpen())
    {

        // Write-in the standard Higgs-Boson header for the sanitize-blacklist file
        sanitizeFile.writeLine("# THIS IS AN AUTOGENERATED FILE USING HIGGS");
        sanitizeFile.writeLine("# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)");
        sanitizeFile.writeLine("");

        // Write-in the standard sanitize-blacklist file information
        sanitizeFile.writeLine("# Add external paths as black-list");
        sanitizeFile.writeLine("src:*/external/output/*");
        sanitizeFile.writeLine("src:*/external/catch2/*");
        sanitizeFile.writeLine("src:*/lib/*");
        sanitizeFile.writeLine("src:*/lib64/*");
        sanitizeFile.writeLine("src:*/bin/*");

        // Close the sanitize-blacklist file
        sanitizeFile.close();

        // If we get here, mark the operation as successful
        wroteSanitize = true;
    }

    // Open the Catch2 Main Testing file
    bool wroteCatch2 = false;
    auto catch2File = FileWriter(_cMakeCacheDir + "/main.test.cpp");
    if (catch2File.isOpen())
    {

        // Write-in the standard Higgs-Boson header for the Catch2 Main Testing file
        catch2File.writeLine("// THIS IS AN AUTOGENERATED FILE USING HIGGS");
        catch2File.writeLine("// DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)");
        catch2File.writeLine("");

        // Write-in the standard Catch2 Main Testing file information
        catch2File.writeLine("// Boiler-plate setup for catch2");
        catch2File.writeLine("#define CATCH_CONFIG_MAIN");
        catch2File.writeLine("#include <catch.hpp>");
        catch2File.writeLine("");

        // Write-in the testing include files
        for (const auto& item : _testFiles)
            catch2File.writeLine("#include \"" + item.first + "\"");

        // Close the Catch2 Main Testing file
        catch2File.close();

        // If we get here, mark the operation as successful
        wroteCatch2 = true;
    }

    // Only continue if the sanitize and Catch2 operation was successful
    if (wroteSanitize && wroteCatch2)
    {

        // Open the CMake file
        auto cMakeFile = FileWriter(_cMakeFile);
        if (cMakeFile.isOpen())
        {

            // Write-in the standard Higgs-Boson header for the CMake file
            cMakeFile.writeLine("# THIS IS AN AUTOGENERATED FILE USING HIGGS");
            cMakeFile.writeLine("# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)");
            cMakeFile.writeLine("");

            // Write-in the CMake minumum requirements information
            cMakeFile.writeLine("# Setup the CMake minimum requirements");
            cMakeFile.writeLine("cmake_minimum_required(VERSION 3.9.0)");
            cMakeFile.writeLine("");

            // Write-in the CMake C++ standards information
            cMakeFile.writeLine("# Set C++17 standard");
            cMakeFile.writeLine("set(CMAKE_CXX_STANDARD 17)");
            cMakeFile.writeLine("");

            // Write-in the CMake Higgs-Boson variables
            cMakeFile.writeLine("# Higgs Build Variables");
            cMakeFile.writeLine("set(HIGGS_PROJECT_NAME \"" + _projectName + "\")");
            cMakeFile.writeLine("set(HIGGS_PROJECT_SRC \"" + _cMakeBuildDir + "\")");
            cMakeFile.writeLine("set(HIGGS_PROJECT_CACHE \"" + _cMakeCacheDir + "\")");
            cMakeFile.writeLine("set(HIGGS_PROJECT_VERSION \"" + _projectVersion + "\")");
            cMakeFile.writeLine("");

            // Write-in the CMake Project details
            cMakeFile.writeLine("# Project Specifics");
            cMakeFile.writeLine("set(PROJECTNAME \"${HIGGS_PROJECT_NAME}\")");
            cMakeFile.writeLine("project(${PROJECTNAME} CXX)");
            cMakeFile.writeLine("");

            // Write-in the CMake Project targets
            cMakeFile.writeLine("# Project Main Targets");
            cMakeFile.writeLine("set(PROJECT_TARGET_MAIN \"${PROJECTNAME}\")");
            cMakeFile.writeLine("set(PROJECT_TARGET_TEST \"${PROJECTNAME}_test\")");
            cMakeFile.writeLine("");

            // Write-in the CMake Project outputs
            cMakeFile.writeLine("# Project Outputs");
            cMakeFile.writeLine("set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)");
            cMakeFile.writeLine("set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)");
            cMakeFile.writeLine("set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)");
            cMakeFile.writeLine("");

            // Write-in the CMake
            cMakeFile.writeLine("# Setup the project version");
            cMakeFile.writeLine("set(VERSION \"${HIGGS_PROJECT_VERSION}\")");
            cMakeFile.writeLine("");

            // Setup toolchain CMAKE variables based on environment variables (if present)
            cMakeFile.writeLine("# Setup toolchain CMAKE variables based on environment variables (if present)");
            cMakeFile.writeLine("if(DEFINED ENV{CC})");
            cMakeFile.writeLine("    MESSAGE(STATUS \"C Compiler Set To: $ENV{CC}\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("if(DEFINED ENV{CXX})");
            cMakeFile.writeLine("    MESSAGE(STATUS \"C++ Compiler Set To: $ENV{CXX}\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("if(DEFINED ENV{CPP})");
            cMakeFile.writeLine("    MESSAGE(STATUS \"CPP Compiler Set To: $ENV{CPP}\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("if(DEFINED ENV{AS})");
            cMakeFile.writeLine("    MESSAGE(STATUS \"Assembler Set To: $ENV{AS}\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("if(DEFINED ENV{AR})");
            cMakeFile.writeLine("    MESSAGE(STATUS \"Archiver Set To: $ENV{AR}\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("if(DEFINED ENV{LD})");
            cMakeFile.writeLine("    MESSAGE(STATUS \"Linker Set To: $ENV{LD}\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("if(DEFINED ENV{FC})");
            cMakeFile.writeLine("    MESSAGE(STATUS \"Fortran Compiler Set To: $ENV{FC}\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("");

            // Setup CMake cross-platform toolchain settings
            cMakeFile.writeLine("# Setup CMake cross-platform toolchain settings");
            cMakeFile.writeLine("set(CMAKE_CXX_COMPILER \"$ENV{CXX}\")");
            cMakeFile.writeLine("set(CMAKE_C_COMPILER \"$ENV{CC}\")");
            cMakeFile.writeLine("set(CMAKE_ASM_COMPILER \"$ENV{AS}\")");
            cMakeFile.writeLine("set(CMAKE_Fortran_COMPILER $ENV{FC})");
            cMakeFile.writeLine("if(DEFINED ENV{LD})");
            cMakeFile.writeLine("    set(CMAKE_SYSTEM_VERSION 1)");
            cMakeFile.writeLine("    set(CMAKE_SYSROOT \"$ENV{HIGGS_BOSON_SYSROOT}\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("");

            // Setup default environment variables for cross-building
            // TODO - Inject default value based on target OS for higgs-boson project itself
            cMakeFile.writeLine("# Setup default environment variables for cross-building");
            cMakeFile.writeLine("if(DEFINED ENV{HIGGS_BOSON_TARGET_OS})");
            cMakeFile.writeLine("    MESSAGE(STATUS \"Target OS Set To: $ENV{HIGGS_BOSON_TARGET_OS}\")");
            cMakeFile.writeLine("else()");
            cMakeFile.writeLine("    set(ENV{HIGGS_BOSON_TARGET_OS} linux)");
            cMakeFile.writeLine("    MESSAGE(STATUS \"Target OS Set To: $ENV{HIGGS_BOSON_TARGET_OS}\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("if(DEFINED ENV{HIGGS_BOSON_TARGET_PLATFORM})");
            cMakeFile.writeLine("    MESSAGE(STATUS \"Target Platform Set To: $ENV{HIGGS_BOSON_TARGET_PLATFORM}\")");
            cMakeFile.writeLine("else()");
            cMakeFile.writeLine("    set(ENV{HIGGS_BOSON_TARGET_PLATFORM} linux-clang)");
            cMakeFile.writeLine("    MESSAGE(STATUS \"Target Platform Set To: $ENV{HIGGS_BOSON_TARGET_PLATFORM}\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("if(DEFINED ENV{HIGGS_BOSON_TARGET_ARCH})");
            cMakeFile.writeLine("    MESSAGE(STATUS \"Target Architecture Set To: $ENV{HIGGS_BOSON_TARGET_ARCH}\")");
            cMakeFile.writeLine("else()");
            cMakeFile.writeLine("    set(ENV{HIGGS_BOSON_TARGET_ARCH} x86_64)");
            cMakeFile.writeLine("    MESSAGE(STATUS \"Target Architecture Set To: $ENV{HIGGS_BOSON_TARGET_ARCH}\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("");

            // Write-in cross-compilation configuration
            cMakeFile.writeLine("# Set Cross-Compilation Target Information");
            cMakeFile.writeLine("set(CMAKE_HOST_SYSTEM_NAME Linux)");
            cMakeFile.writeLine("set(CMAKE_SYSTEM_NAME $ENV{HIGGS_BOSON_TARGET_OS})");
            cMakeFile.writeLine("set(CMAKE_CROSSCOMPILING ON)");
            cMakeFile.writeLine("");
            cMakeFile.writeLine("");

            // Write in specifics to darwin (macos) related builds
            cMakeFile.writeLine("# Specify specifics to darwin (macos) related builds");
            cMakeFile.writeLine("if (CMAKE_SYSTEM_NAME STREQUAL darwin)");
            cMakeFile.writeLine("    set(CMAKE_MACOSX_RPATH \"OFF\")");
            cMakeFile.writeLine("    set(CMAKE_SHARED_LIBRARY_PREFIX \"lib\")");
            cMakeFile.writeLine("    set(CMAKE_SHARED_LIBRARY_SUFFIX \".dylib\")");
            cMakeFile.writeLine("    set(CMAKE_SHARED_LIBRARY_SONAME_C_FLAG \"-install_name \")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("");
            cMakeFile.writeLine("");

            // Write-in the CMake source directories
            cMakeFile.writeLine("# Setup the hard-coded source directory");
            cMakeFile.writeLine("set(CMAKE_SOURCE_DIR ${HIGGS_PROJECT_SRC})");
            cMakeFile.writeLine("set(CMAKE_CURRENT_SOURCE_DIR ${HIGGS_PROJECT_SRC})");
            cMakeFile.writeLine("");
            cMakeFile.writeLine("");

            // Write-in the CMake external dependencies section
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("# Setup Internal (required) dependencies");
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("");

            // Write-in support for plibsys to handle platform specific things
            cMakeFile.writeLine("# Add plibsys specific build parameters");
            cMakeFile.writeLine("set(CMAKE_BUILD_TYPE, \"Release\")");
            cMakeFile.writeLine("set(PLIBSYS_BUILD_STATIC, \"OFF\")");
            cMakeFile.writeLine("set(CMAKE_C_COMPILER_ID, \"GNU\")");
            cMakeFile.writeLine("set(PLIBSYS_TARGET_OS, \"$ENV{HIGGS_BOSON_TARGET_OS}\")");
            cMakeFile.writeLine("set(PLIBSYS_TARGET_PLATFORM, \"$ENV{HIGGS_BOSON_TARGET_PLATFORM}\")");
            cMakeFile.writeLine("set(CMAKE_SYSTEM_PROCESSOR, \"$ENV{HIGGS_BOSON_TARGET_ARCH}\")");
            //cMakeFile.writeLine("add_subdirectory (${HIGGS_PROJECT_CACHE}/external/raw/plibsyshiggsboson)");
            cMakeFile.writeLine("include (${HIGGS_PROJECT_CACHE}/external/raw/plibsyshiggsboson/platforms/$ENV{HIGGS_BOSON_TARGET_PLATFORM}/platform.cmake)");
            cMakeFile.writeLine("include (${HIGGS_PROJECT_CACHE}/external/raw/plibsyshiggsboson/cmake/PlatformDetect.cmake)");
            cMakeFile.writeLine("plibsys_detect_target_os ($ENV{HIGGS_BOSON_TARGET_PLATFORM})");
            cMakeFile.writeLine("MESSAGE(STATUS \"Platform-Specific Compiler Flags: ${PLIBSYS_PLATFORM_LINK_LIBRARIES}\")");
            cMakeFile.writeLine("");
            cMakeFile.writeLine("");

            // Write-in the CMake external dependencies section
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("# Setup External dependencies");
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("");

            // Write-in the CMake External includes files
            cMakeFile.writeLine("# Add the include directories");
            cMakeFile.writeLine("set(HIGGS_EXTERNAL_INCLUDES");
            cMakeFile.writeLine("    ${CMAKE_BINARY_DIR}");
            cMakeFile.writeLine("    \"${HIGGS_PROJECT_CACHE}/external/raw/plibsyshiggsboson/src\"");
            cMakeFile.writeLine("    \"${HIGGS_PROJECT_CACHE}/external/raw/catch2higgsboson/single_include/catch2\"");
            for (const auto& item : _externalIncludes)
                cMakeFile.writeLine("    \"" + item + "\"");
            cMakeFile.writeLine(")");
            cMakeFile.writeLine("");

            // Write-in the CMake external libraries files
            cMakeFile.writeLine("# Setup the library and linker information");
            cMakeFile.writeLine("set(HIGGS_EXTERNAL_LIBS");
            for (auto libIndex = _externalLibraries.size(); libIndex > 0; libIndex--)
                cMakeFile.writeLine("    \"" + _externalLibraries[libIndex - 1] + "\"");
            cMakeFile.writeLine(")");
            cMakeFile.writeLine("");

            // Write-in the CMake LLVM coverage information
            cMakeFile.writeLine("# Add/Setup llvm coverage if desired/available");
            cMakeFile.writeLine("SET(LLVM_COV_PATH \"/usr/bin/llvm-cov\")");
            cMakeFile.writeLine("if(CMAKE_BUILD_TYPE STREQUAL \"coverage\" OR CODE_COVERAGE)");
            cMakeFile.writeLine("    if(\"${CMAKE_C_COMPILER_ID}\" MATCHES \"(Apple)?[Cc]lang\" OR \"${CMAKE_CXX_COMPILER_ID}\" MATCHES \"(Apple)?[Cc]lang\")");
            cMakeFile.writeLine("        message(\"Building with llvm Code Coverage Tools\")");
            cMakeFile.writeLine("        # Warning/Error messages");
            cMakeFile.writeLine("        if(NOT LLVM_COV_PATH)");
            cMakeFile.writeLine("            message(FATAL_ERROR \"llvm-cov not found! Aborting.\")");
            cMakeFile.writeLine("        endif()");
            cMakeFile.writeLine("        # set Flags");
            cMakeFile.writeLine("        set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping\")");
            cMakeFile.writeLine("        set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping\")");
            cMakeFile.writeLine("    elseif(CMAKE_COMPILER_IS_GNUCXX)");
            cMakeFile.writeLine("        message(\"Building with lcov Code Coverage Tools\")");
            cMakeFile.writeLine("        # Warning/Error messages");
            cMakeFile.writeLine("        if(NOT (CMAKE_BUILD_TYPE STREQUAL \"Debug\"))");
            cMakeFile.writeLine("            message(WARNING \"Code coverage results with an optimized (non-Debug) build may be misleading\")");
            cMakeFile.writeLine("        endif()");
            cMakeFile.writeLine("        if(NOT LCOV_PATH)");
            cMakeFile.writeLine("            message(FATAL_ERROR \"lcov not found! Aborting...\")");
            cMakeFile.writeLine("        endif()");
            cMakeFile.writeLine("        if(NOT GENHTML_PATH)");
            cMakeFile.writeLine("            message(FATAL_ERROR \"genhtml not found! Aborting...\")");
            cMakeFile.writeLine("        endif()");
            cMakeFile.writeLine("        set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage\")");
            cMakeFile.writeLine("        set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage\")");
            cMakeFile.writeLine("    else()");
            cMakeFile.writeLine("        message(FATAL_ERROR \"Code coverage requires Clang or GCC. Aborting.\")");
            cMakeFile.writeLine("    endif()");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("");

            // Write-in the CMake coverages flags
            cMakeFile.writeLine("if(NOT CODE_COVERAGE)");
            cMakeFile.writeLine("    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -Wall -Werror -Wno-deprecated-declarations\")");
            cMakeFile.writeLine("    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS}  -Wall -Werror -Wno-deprecated-declarations\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("");

            // Write-in the CMake memory sanitizer flags
            cMakeFile.writeLine("if(SANITIZE_MEMORY)");
            cMakeFile.writeLine("    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -g -fsanitize=memory -fsanitize-memory-track-origins -O1 -fno-optimize-sibling-calls -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")");
            cMakeFile.writeLine("    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g -fsanitize=memory -fsanitize-memory-track-origins -O1 -fno-optimize-sibling-calls -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("");

            // Write-in the CMake address sanitizer flags
            cMakeFile.writeLine("if(SANITIZE_ADDRESS)");
            cMakeFile.writeLine("    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -g -fsanitize=address -fno-omit-frame-pointer -O1 -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")");
            cMakeFile.writeLine("    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g -fsanitize=address -fno-omit-frame-pointer -O1 -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("");

            // Write-in the CMake leak sanitizer flags
            cMakeFile.writeLine("if(SANITIZE_LEAK)");
            cMakeFile.writeLine("    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -g -fsanitize=leak -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")");
            cMakeFile.writeLine("    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g -fsanitize=leak -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("");

            // Write-in the CMake thread sanitizer flags
            cMakeFile.writeLine("if(SANITIZE_THREAD)");
            cMakeFile.writeLine("    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -g -fsanitize=thread -O1 -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")");
            cMakeFile.writeLine("    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g -fsanitize=thread -O1 -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("");

            // Write-in the CMake behavior sanitizer flags
            cMakeFile.writeLine("if(SANITIZE_BEHAVIOR)");
            cMakeFile.writeLine("    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -g -fsanitize=undefined -fsanitize-minimal-runtime -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")");
            cMakeFile.writeLine("    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g -fsanitize=undefined -fsanitize-minimal-runtime -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")");
            cMakeFile.writeLine("endif()");
            cMakeFile.writeLine("");

            // Write-in the CMake R-path information
            // TODO - Needs to be target-specific
            //cMakeFile.writeLine("# Add in the higgs library linker location to the R-path");
            //cMakeFile.writeLine("set(CMAKE_BUILD_RPATH \"/usr/lib/higgs-boson\")");
            //cMakeFile.writeLine("set(CMAKE_INSTALL_RPATH  \"/usr/lib/higgs-boson\")");
            //cMakeFile.writeLine("");

            // Write-in the CMake main project target section
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("# Setup Target: Main Project");
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("");

            // Write-in the CMake external includes details
            cMakeFile.writeLine("# Setup the include directories");
            cMakeFile.writeLine("include_directories(${PROJECT_TARGET_MAIN} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)");
            cMakeFile.writeLine("");

            // Write-in the CMake include files
            cMakeFile.writeLine("# Setup Headers");
            cMakeFile.writeLine("set(${PROJECT_TARGET_MAIN}_headers");
            for (const auto& item : _headerFiles)
                cMakeFile.writeLine("        \"" + item.first + "\"");
            cMakeFile.writeLine(")");
            cMakeFile.writeLine("");

            // Write-in the CMake source files
            cMakeFile.writeLine("# C++ Source Files");
            cMakeFile.writeLine("set(${PROJECT_TARGET_MAIN}_sources");
            for (const auto& item : _sourceFiles)
                cMakeFile.writeLine("        \"" + item.first + "\"");
            cMakeFile.writeLine(")");
            cMakeFile.writeLine("");

            // Write-in the CMake main project target information
            cMakeFile.writeLine("# Create the actual library for main project");
            if (mainSourceFile.empty())
                cMakeFile.writeLine("add_library(${PROJECT_TARGET_MAIN} SHARED ");
            else
                cMakeFile.writeLine("add_executable(${PROJECT_TARGET_MAIN} " + mainSourceFile);
            cMakeFile.writeLine("        ${${PROJECT_TARGET_MAIN}_sources} ${${PROJECT_TARGET_MAIN}_headers})");
            //cMakeFile.writeLine("add_dependencies(${PROJECT_TARGET_MAIN} plibsys)");
            //cMakeFile.writeLine("target_link_libraries(${PROJECT_TARGET_MAIN} plibsys)");
            cMakeFile.writeLine("target_link_libraries(${PROJECT_TARGET_MAIN} ${HIGGS_EXTERNAL_LIBS})");
            cMakeFile.writeLine("target_link_libraries(${PROJECT_TARGET_MAIN} ${PLIBSYS_PLATFORM_LINK_LIBRARIES})");
            cMakeFile.writeLine("");

            // Write-in the CMake target includes details
            cMakeFile.writeLine("# Setup include directories for the main project");
            cMakeFile.writeLine("include_directories(${PROJECT_TARGET_MAIN} \"${HIGGS_EXTERNAL_INCLUDES}\")");
            cMakeFile.writeLine("");
            cMakeFile.writeLine("");

            // Write-in the CMake test target section
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("# Setup Target: Test Project");
            cMakeFile.writeLine("#");
            cMakeFile.writeLine("");

            // Write-in the CMake Catch-testing setup information
            cMakeFile.writeLine("# Prepare \"Catch\" library for other executables");
            cMakeFile.writeLine("set(TEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)");
            cMakeFile.writeLine("set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.higgs/external/raw/catch2higgsboson/single_include/catch2)");
            cMakeFile.writeLine("add_library(Catch INTERFACE)");
            cMakeFile.writeLine("target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR} ${TEST_INCLUDE_DIR})");
            cMakeFile.writeLine("");

            // Write-in the CMake include directories for testing
            cMakeFile.writeLine("# Setup the include directories for the test target");
            cMakeFile.writeLine("include_directories(${PROJECT_TARGET_TEST} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)");
            cMakeFile.writeLine("");

            // Write-in the CMake test source files
            cMakeFile.writeLine("# Setup test sources");
            cMakeFile.writeLine("set(TEST_SOURCES");
            for (const auto& item : _testFiles)
                cMakeFile.writeLine("        \"" + item.first + "\"");
            cMakeFile.writeLine(")");
            cMakeFile.writeLine("");

            // Write-in the CMake testing target
            cMakeFile.writeLine("# Make the test executable");
            cMakeFile.writeLine("add_executable(${PROJECT_TARGET_TEST} ${HIGGS_PROJECT_CACHE}/main.test.cpp ${TEST_SOURCES}");
            cMakeFile.writeLine("        ${${PROJECT_TARGET_MAIN}_sources} ${${PROJECT_TARGET_MAIN}_headers})");
            cMakeFile.writeLine("");

            // Write-in the CMake include source/headers for testing targets
            cMakeFile.writeLine("# Setup include directories for the test project");
            cMakeFile.writeLine("include_directories(${PROJECT_TARGET_TEST} \"${CMAKE_SOURCE_DIR}/src\")");
            cMakeFile.writeLine("include_directories(${PROJECT_TARGET_TEST} \"${CMAKE_SOURCE_DIR}/test\")");
            cMakeFile.writeLine("include_directories(${PROJECT_TARGET_TEST} \"${HIGGS_EXTERNAL_INCLUDES}\")");
            cMakeFile.writeLine("");

            // Write-in the CMake testing target libraries information
            cMakeFile.writeLine("# Setup the test target");
            //cMakeFile.writeLine("add_dependencies(${PROJECT_TARGET_TEST} plibsys)");
            //cMakeFile.writeLine("target_link_libraries(${PROJECT_TARGET_TEST} plibsys)");
            cMakeFile.writeLine("target_link_libraries(${PROJECT_TARGET_TEST} ${HIGGS_EXTERNAL_LIBS})");
            cMakeFile.writeLine("target_link_libraries(${PROJECT_TARGET_TEST} ${PLIBSYS_PLATFORM_LINK_LIBRARIES})");
            cMakeFile.writeLine("target_link_libraries(${PROJECT_TARGET_TEST} Catch)");
            cMakeFile.writeLine("");

            // Write-in the CMake testing compile definitions
            cMakeFile.writeLine("# Setup Testing definitions");
            cMakeFile.writeLine("target_compile_definitions(${PROJECT_TARGET_TEST} PRIVATE CATCH_TESTING=1)");
            cMakeFile.writeLine("");

            // Write-in the CMake testing LLVM coverage information
            cMakeFile.writeLine("# Setup the LLVM Coverage Target");
            cMakeFile.writeLine("add_custom_target(${PROJECT_TARGET_TEST}_coverage");
            cMakeFile.writeLine("        COMMAND LLVM_PROFILE_FILE=${PROJECT_TARGET_TEST}.profraw $<TARGET_FILE:${PROJECT_TARGET_TEST}>");
            cMakeFile.writeLine("        COMMAND llvm-profdata merge -sparse ${PROJECT_TARGET_TEST}.profraw -o ${PROJECT_TARGET_TEST}.profdata");
            cMakeFile.writeLine("        COMMAND llvm-cov report $<TARGET_FILE:${PROJECT_TARGET_TEST}> -instr-profile=${PROJECT_TARGET_TEST}.profdata ${CMAKE_SOURCE_DIR}/src");
            cMakeFile.writeLine("        COMMAND llvm-cov show $<TARGET_FILE:${PROJECT_TARGET_TEST}> -instr-profile=${PROJECT_TARGET_TEST}.profdata -show-line-counts-or-regions -output-dir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/llvm-coverage -format=\"html\" ${CMAKE_SOURCE_DIR}/src");
            cMakeFile.writeLine("        COMMAND echo \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/llvm-coverage/index.html in your browser to view the coverage report.\"");
            cMakeFile.writeLine(")");

            // Close the CMake file
            cMakeFile.close();

            // If we get here, mark the operation as successful
            retFlag = true;
        }
    }

    // Return the return flag
    return retFlag;
}
