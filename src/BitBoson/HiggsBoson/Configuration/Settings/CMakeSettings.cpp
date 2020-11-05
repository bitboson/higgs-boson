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

#include <fstream>
#include <sstream>
#include <iostream>
#include <BitBoson/HiggsBoson/HiggsBoson.h>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
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
    system(std::string("mkdir -p " + _cMakeBuildDir).c_str());
    system(std::string("mkdir -p " + _cMakeCacheDir).c_str());
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

    // Create the build directory for CMake to actually use
    if (wroteFile && (system(std::string("mkdir -p " + _cMakeCacheDir + "/builds/compile-" + target).c_str()) == 0))
    {

        // Write the build workflow for the specified target
        bool wroteBuildFile = false;
        std::ofstream buildFile;
        buildFile.open(_cMakeCacheDir + "/builds/compile-" + target + ".sh");
        if (buildFile.is_open())
        {

            // Write-in the standard Higgs-Boson header for the build file
            buildFile << "# THIS IS AN AUTOGENERATED FILE USING HIGGS" << std::endl;
            buildFile << "# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)" << std::endl;
            buildFile << "set -e" << std::endl;
            buildFile << std::endl;

            // Write-in the standard build file information for the target
            buildFile << "# Build Steps for the Compile operation for target " + target << std::endl;
            buildFile << "mkdir -p " << _cMakeCacheDir << "/builds/compile/" << target << std::endl;
            buildFile << "cd " << _cMakeCacheDir << "/builds/compile/" << target << std::endl;
            buildFile << "cmake";
            if (target == "default")
                buildFile << " -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++-6.0";
            buildFile << " -DCMAKE_BUILD_TYPE=Release " << _cMakeCacheDir << std::endl;
            buildFile << std::endl;

            // Close the build file
            buildFile.close();

            // If we get here, mark the operation as successful
            wroteBuildFile = true;
        }

        // Write-out the make command file
        bool wroteMake = false;
        std::ofstream makeShellFile;
        makeShellFile.open(_cMakeCacheDir + "/builds/compile-" + target + ".make.sh");
        if (makeShellFile.is_open())
        {

            // Write-in the standard Higgs-Boson header for the Make Shell file
            makeShellFile << "# THIS IS AN AUTOGENERATED FILE USING HIGGS" << std::endl;
            makeShellFile << "# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)" << std::endl;
            makeShellFile << std::endl;

            // Write-in the pre-build commands
            makeShellFile << "# Pre-Build commands for the Process" << std::endl;
            for (const auto& preBuildCmd : _preBuildCommands)
                makeShellFile << preBuildCmd << std::endl;
            makeShellFile << std::endl;

            // Write-in the actual Make command
            makeShellFile << "# Run the Make Operation: Compile Target " << target << std::endl;
            makeShellFile << "cd " + _cMakeCacheDir + "/builds/compile/" + target + " && make " + _projectName << std::endl;
            makeShellFile << std::endl;

            // Write-in the post-build commands
            makeShellFile << "# Post-Build commands for the Process" << std::endl;
            for (const auto& postBuildCmd : _postBuildCommands)
                makeShellFile << postBuildCmd << std::endl;
            makeShellFile << std::endl;

            // Indicate that the Make Shell file was written properly
            wroteMake = true;
        }

        // Only continue if the build file was written successfully
        if (wroteBuildFile && wroteMake)
        {

            // Run the build workflow and keep track of the results
            retFlag = (ExecShell::execWithResponse("Setting-Up Build for " + _projectName + " Version " + _projectVersion,
                    HiggsBoson::RunTypeSingleton::getRunTypeCommand() + " " + _cMakeCacheDir + "/builds/compile-" + target + ".sh"));
            std::cout << "Building " + _projectName + " Version " + _projectVersion << std::endl;
            retFlag = (retFlag && ExecShell::execLive(
                    HiggsBoson::RunTypeSingleton::getRunTypeCommand() + " " + _cMakeCacheDir + "/builds/compile-" + target + ".make.sh"));
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

        // Handle the default case
        default:
            testTypeString = "test";
    }

    // Create the build directory for CMake to actually use
    if (wroteFile && (system(std::string("mkdir -p " + _cMakeCacheDir + "/builds/" + testTypeString).c_str()) == 0))
    {

        // Write the build workflow for the specified target
        bool wroteBuildFile = false;
        std::ofstream buildFile;
        buildFile.open(_cMakeCacheDir + "/builds/" + testTypeString + ".sh");
        if (buildFile.is_open())
        {

            // Write-in the standard Higgs-Boson header for the build file
            buildFile << "# THIS IS AN AUTOGENERATED FILE USING HIGGS" << std::endl;
            buildFile << "# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)" << std::endl;
            buildFile << "set -e" << std::endl;
            buildFile << std::endl;

            // Write-in the standard build file information for the target
            buildFile << "# Build Steps for the Test operation " + testTypeString << std::endl;
            buildFile << "mkdir -p " << _cMakeCacheDir << "/builds/" << testTypeString << std::endl;
            buildFile << "cd " << _cMakeCacheDir << "/builds/" << testTypeString << std::endl;
            buildFile << "cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++-6.0";
            if (testType == TestType::COVERAGE)
                buildFile << " -DCODE_COVERAGE=ON ";
            buildFile << " -DCMAKE_BUILD_TYPE=Debug " << _cMakeCacheDir << " " << testCMakeVarString << std::endl;
            buildFile << std::endl;

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
            std::ofstream makeShellFile;
            makeShellFile.open(_cMakeCacheDir + "/builds/" + testTypeString + ".make.sh");
            if (makeShellFile.is_open())
            {

                // Write-in the standard Higgs-Boson header for the Make Shell file
                makeShellFile << "# THIS IS AN AUTOGENERATED FILE USING HIGGS" << std::endl;
                makeShellFile << "# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)" << std::endl;
                makeShellFile << std::endl;

                // Write-in the pre-test commands
                makeShellFile << "# Pre-Test commands for the Test" << std::endl;
                for (const auto& preTestCmd : _preTestCommands)
                    makeShellFile << preTestCmd << std::endl;
                makeShellFile << std::endl;

                // Write-in the actual Make command
                makeShellFile << "# Run the Make Operation: " << testTypeString << std::endl;
                makeShellFile << makeCommand << std::endl;
                if ((testType != TestType::COVERAGE) && (testType != TestType::DEBUG))
                    makeShellFile << libraryLdPathString + " " + _cMakeCacheDir + "/builds/" + testTypeString + "/bin/" + _projectName + "_test " << testFilter << std::endl;
                if (testType == TestType::DEBUG)
                    makeShellFile << libraryLdPathString + " gdb " + _cMakeCacheDir + "/builds/" + testTypeString + "/bin/" + _projectName + "_test" << std::endl;
                makeShellFile << std::endl;

                // Write-in the post-test commands
                makeShellFile << "# Post-Test commands for the Test" << std::endl;
                for (const auto& postTestCmd : _postTestCommands)
                    makeShellFile << postTestCmd << std::endl;
                makeShellFile << std::endl;

                // Indicate that the Make Shell file was written properly
                wroteMake = true;
            }

            // Only continue if the make file was written properly
            if (wroteMake)
            {

                // Run the build workflow and keep track of the results
                retFlag = (ExecShell::execWithResponse("Setting-Up Test " + testTypeString + " for " + _projectName
                        + " Version " + _projectVersion,
                        HiggsBoson::RunTypeSingleton::getRunTypeCommand() + " " + _cMakeCacheDir + "/builds/" + testTypeString + ".sh"));
                std::cout << "Running " + _projectName + " Version " + _projectVersion;
                std::cout << " for Test " + testTypeString << std::endl;
                retFlag = (retFlag && ExecShell::execLive(
                        HiggsBoson::RunTypeSingleton::getRunTypeCommand() + " " + _cMakeCacheDir + "/builds/" + testTypeString + ".make.sh"));
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
    std::ofstream sanitizeFile;
    sanitizeFile.open(_cMakeCacheDir + "/sanitize-blacklist.txt");
    if (sanitizeFile.is_open())
    {

        // Write-in the standard Higgs-Boson header for the sanitize-blacklist file
        sanitizeFile << "# THIS IS AN AUTOGENERATED FILE USING HIGGS" << std::endl;
        sanitizeFile << "# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)" << std::endl;
        sanitizeFile << std::endl;

        // Write-in the standard sanitize-blacklist file information
        sanitizeFile << "# Add external paths as black-list" << std::endl;
        sanitizeFile << "src:*/external/output/*" << std::endl;
        sanitizeFile << "src:*/external/catch2/*" << std::endl;
        sanitizeFile << "src:*/lib/*" << std::endl;
        sanitizeFile << "src:*/lib64/*" << std::endl;
        sanitizeFile << "src:*/bin/*" << std::endl;

        // Close the sanitize-blacklist file
        sanitizeFile.close();

        // If we get here, mark the operation as successful
        wroteSanitize = true;
    }

    // Open the Catch2 Main Testing file
    bool wroteCatch2 = false;
    std::ofstream catch2File;
    catch2File.open(_cMakeCacheDir + "/main.test.cpp");
    if (catch2File.is_open())
    {

        // Write-in the standard Higgs-Boson header for the Catch2 Main Testing file
        catch2File << "// THIS IS AN AUTOGENERATED FILE USING HIGGS" << std::endl;
        catch2File << "// DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)" << std::endl;
        catch2File << std::endl;

        // Write-in the standard Catch2 Main Testing file information
        catch2File << "// Boiler-plate setup for catch2" << std::endl;
        catch2File << "#define CATCH_CONFIG_MAIN" << std::endl;
        catch2File << "#include <catch.hpp>" << std::endl;
        catch2File << std::endl;

        // Write-in the testing include files
        for (const auto& item : _testFiles)
            catch2File << "#include \"" << item.first << "\"" << std::endl;

        // Close the Catch2 Main Testing file
        catch2File.close();

        // If we get here, mark the operation as successful
        wroteCatch2 = true;
    }

    // Only continue if the sanitize and Catch2 operation was successful
    if (wroteSanitize && wroteCatch2)
    {

        // Open the CMake file
        std::ofstream cMakeFile;
        cMakeFile.open(_cMakeFile);
        if (cMakeFile.is_open())
        {

            // Write-in the standard Higgs-Boson header for the CMake file
            cMakeFile << "# THIS IS AN AUTOGENERATED FILE USING HIGGS" << std::endl;
            cMakeFile << "# DO NOT EDIT (UNLESS YOU KNOW WHAT'S UP)" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake minumum requirements information
            cMakeFile << "# Setup the CMake minimum requirements" << std::endl;
            cMakeFile << "cmake_minimum_required(VERSION 3.0.0)" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake C++ standards information
            cMakeFile << "# Set C++17 standard" << std::endl;
            cMakeFile << "set(CMAKE_CXX_STANDARD 17)" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake Higgs-Boson variables
            cMakeFile << "# Higgs Build Variables" << std::endl;
            cMakeFile << "set(HIGGS_PROJECT_NAME \"" << _projectName << "\")" << std::endl;
            cMakeFile << "set(HIGGS_PROJECT_SRC \"" << _cMakeBuildDir << "\")" << std::endl;
            cMakeFile << "set(HIGGS_PROJECT_CACHE \"" << _cMakeCacheDir << "\")" << std::endl;
            cMakeFile << "set(HIGGS_PROJECT_VERSION \"" << _projectVersion << "\")" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake Project details
            cMakeFile << "# Project Specifics" << std::endl;
            cMakeFile << "set(PROJECTNAME \"${HIGGS_PROJECT_NAME}\")" << std::endl;
            cMakeFile << "project(${PROJECTNAME} CXX)" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake Project targets
            cMakeFile << "# Project Main Targets" << std::endl;
            cMakeFile << "set(PROJECT_TARGET_MAIN \"${PROJECTNAME}\")" << std::endl;
            cMakeFile << "set(PROJECT_TARGET_TEST \"${PROJECTNAME}_test\")" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake Project outputs
            cMakeFile << "# Project Outputs" << std::endl;
            cMakeFile << "set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)" << std::endl;
            cMakeFile << "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)" << std::endl;
            cMakeFile << "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake
            cMakeFile << "# Setup the project version" << std::endl;
            cMakeFile << "set(VERSION \"${HIGGS_PROJECT_VERSION}\")" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake source directories
            cMakeFile << "# Setup the hard-coded source directory" << std::endl;
            cMakeFile << "set(CMAKE_SOURCE_DIR ${HIGGS_PROJECT_SRC})" << std::endl;
            cMakeFile << "set(CMAKE_CURRENT_SOURCE_DIR ${HIGGS_PROJECT_SRC})" << std::endl;
            cMakeFile << std::endl << std::endl;

            // Write-in the CMake external dependencies section
            cMakeFile << "#" << std::endl;
            cMakeFile << "#" << std::endl;
            cMakeFile << "# Setup External dependencies" << std::endl;
            cMakeFile << "#" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake External includes files
            cMakeFile << "# Add the include directories" << std::endl;
            cMakeFile << "set(HIGGS_EXTERNAL_INCLUDES" << std::endl;
            cMakeFile << "    \"${HIGGS_PROJECT_CACHE}/external/raw/catch2higgsboson/single_include/catch2\"" << std::endl;
            for (const auto& item : _externalIncludes)
                cMakeFile << "    \"" << item << "\"" << std::endl;
            cMakeFile << ")" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake external libraries files
            cMakeFile << "# Setup the library and linker information" << std::endl;
            cMakeFile << "set(HIGGS_EXTERNAL_LIBS" << std::endl;
            for (auto libIndex = _externalLibraries.size(); libIndex > 0; libIndex--)
                cMakeFile << "    \"" << _externalLibraries[libIndex - 1] << "\"" << std::endl;
            cMakeFile << ")" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake threading support
            cMakeFile << "# Add threading to the build process" << std::endl;
            cMakeFile << "find_package(Threads)" << std::endl;
            cMakeFile << "SET(CMAKE_CXX_FLAGS \"-pthread\")" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake LLVM coverage information
            cMakeFile << "# Add/Setup llvm coverage if desired/available" << std::endl;
            cMakeFile << "SET(LLVM_COV_PATH \"/usr/bin/llvm-cov\")" << std::endl;
            cMakeFile << "if(CMAKE_BUILD_TYPE STREQUAL \"coverage\" OR CODE_COVERAGE)" << std::endl;
            cMakeFile << "    if(\"${CMAKE_C_COMPILER_ID}\" MATCHES \"(Apple)?[Cc]lang\" OR \"${CMAKE_CXX_COMPILER_ID}\" MATCHES \"(Apple)?[Cc]lang\")" << std::endl;
            cMakeFile << "        message(\"Building with llvm Code Coverage Tools\")" << std::endl;
            cMakeFile << "        # Warning/Error messages" << std::endl;
            cMakeFile << "        if(NOT LLVM_COV_PATH)" << std::endl;
            cMakeFile << "            message(FATAL_ERROR \"llvm-cov not found! Aborting.\")" << std::endl;
            cMakeFile << "        endif()" << std::endl;
            cMakeFile << "        # set Flags" << std::endl;
            cMakeFile << "        set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping\")" << std::endl;
            cMakeFile << "        set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping\")" << std::endl;
            cMakeFile << "    elseif(CMAKE_COMPILER_IS_GNUCXX)" << std::endl;
            cMakeFile << "        message(\"Building with lcov Code Coverage Tools\")" << std::endl;
            cMakeFile << "        # Warning/Error messages" << std::endl;
            cMakeFile << "        if(NOT (CMAKE_BUILD_TYPE STREQUAL \"Debug\"))" << std::endl;
            cMakeFile << "            message(WARNING \"Code coverage results with an optimized (non-Debug) build may be misleading\")" << std::endl;
            cMakeFile << "        endif()" << std::endl;
            cMakeFile << "        if(NOT LCOV_PATH)" << std::endl;
            cMakeFile << "            message(FATAL_ERROR \"lcov not found! Aborting...\")" << std::endl;
            cMakeFile << "        endif()" << std::endl;
            cMakeFile << "        if(NOT GENHTML_PATH)" << std::endl;
            cMakeFile << "            message(FATAL_ERROR \"genhtml not found! Aborting...\")" << std::endl;
            cMakeFile << "        endif()" << std::endl;
            cMakeFile << "        set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage\")" << std::endl;
            cMakeFile << "        set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage\")" << std::endl;
            cMakeFile << "    else()" << std::endl;
            cMakeFile << "        message(FATAL_ERROR \"Code coverage requires Clang or GCC. Aborting.\")" << std::endl;
            cMakeFile << "    endif()" << std::endl;
            cMakeFile << "endif()" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake coverages flags
            cMakeFile << "if(NOT CODE_COVERAGE)" << std::endl;
            cMakeFile << "    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -Wall -Werror\")" << std::endl;
            cMakeFile << "    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS}  -Wall -Werror\")" << std::endl;
            cMakeFile << "endif()" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake memory sanitizer flags
            cMakeFile << "if(SANITIZE_MEMORY)" << std::endl;
            cMakeFile << "    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -g -fsanitize=memory -fsanitize-memory-track-origins -O1 -fno-optimize-sibling-calls -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")" << std::endl;
            cMakeFile << "    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g -fsanitize=memory -fsanitize-memory-track-origins -O1 -fno-optimize-sibling-calls -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")" << std::endl;
            cMakeFile << "endif()" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake address sanitizer flags
            cMakeFile << "if(SANITIZE_ADDRESS)" << std::endl;
            cMakeFile << "    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -g -fsanitize=address -fno-omit-frame-pointer -O1 -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")" << std::endl;
            cMakeFile << "    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g -fsanitize=address -fno-omit-frame-pointer -O1 -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")" << std::endl;
            cMakeFile << "endif()" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake leak sanitizer flags
            cMakeFile << "if(SANITIZE_LEAK)" << std::endl;
            cMakeFile << "    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -g -fsanitize=leak -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")" << std::endl;
            cMakeFile << "    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g -fsanitize=leak -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")" << std::endl;
            cMakeFile << "endif()" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake thread sanitizer flags
            cMakeFile << "if(SANITIZE_THREAD)" << std::endl;
            cMakeFile << "    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -g -fsanitize=thread -O1 -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")" << std::endl;
            cMakeFile << "    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g -fsanitize=thread -O1 -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")" << std::endl;
            cMakeFile << "endif()" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake behavior sanitizer flags
            cMakeFile << "if(SANITIZE_BEHAVIOR)" << std::endl;
            cMakeFile << "    set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -g -fsanitize=undefined -fsanitize-minimal-runtime -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")" << std::endl;
            cMakeFile << "    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g -fsanitize=undefined -fsanitize-minimal-runtime -fsanitize-blacklist=${HIGGS_PROJECT_CACHE}/sanitize-blacklist.txt\")" << std::endl;
            cMakeFile << "endif()" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake R-path information
            // TODO - Needs to be target-specific
            //cMakeFile << "# Add in the higgs library linker location to the R-path" << std::endl;
            //cMakeFile << "set(CMAKE_BUILD_RPATH \"/usr/lib/higgs-boson\")" << std::endl;
            //cMakeFile << "set(CMAKE_INSTALL_RPATH  \"/usr/lib/higgs-boson\")" << std::endl;
            //cMakeFile << std::endl << std::endl;

            // Write-in the CMake main project target section
            cMakeFile << "#" << std::endl;
            cMakeFile << "#" << std::endl;
            cMakeFile << "# Setup Target: Main Project" << std::endl;
            cMakeFile << "#" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake external includes details
            cMakeFile << "# Setup the include directories" << std::endl;
            cMakeFile << "include_directories(${PROJECT_TARGET_MAIN} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake include files
            cMakeFile << "# Setup Headers" << std::endl;
            cMakeFile << "set(${PROJECT_TARGET_MAIN}_headers" << std::endl;
            for (const auto& item : _headerFiles)
                cMakeFile << "        \"" << item.first << "\"" << std::endl;
            cMakeFile << ")" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake source files
            cMakeFile << "# C++ Source Files" << std::endl;
            cMakeFile << "set(${PROJECT_TARGET_MAIN}_sources" << std::endl;
            for (const auto& item : _sourceFiles)
                cMakeFile << "        \"" << item.first << "\"" << std::endl;
            cMakeFile << ")" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake main project target information
            cMakeFile << "# Create the actual library for main project" << std::endl;
            if (mainSourceFile.empty())
                cMakeFile << "add_library(${PROJECT_TARGET_MAIN} SHARED " << std::endl;
            else
                cMakeFile << "add_executable(${PROJECT_TARGET_MAIN} " << mainSourceFile << std::endl;
            cMakeFile << "        ${${PROJECT_TARGET_MAIN}_sources} ${${PROJECT_TARGET_MAIN}_headers})" << std::endl;
            cMakeFile << "target_link_libraries(${PROJECT_TARGET_MAIN} ${HIGGS_EXTERNAL_LIBS})" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake target includes details
            cMakeFile << "# Setup include directories for the main project" << std::endl;
            cMakeFile << "include_directories(${PROJECT_TARGET_MAIN} \"${HIGGS_EXTERNAL_INCLUDES}\")" << std::endl;
            cMakeFile << std::endl << std::endl;

            // Write-in the CMake test target section
            cMakeFile << "#" << std::endl;
            cMakeFile << "#" << std::endl;
            cMakeFile << "# Setup Target: Test Project" << std::endl;
            cMakeFile << "#" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake Catch-testing setup information
            cMakeFile << "# Prepare \"Catch\" library for otherexecutables" << std::endl;
            cMakeFile << "set(TEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)" << std::endl;
            cMakeFile << "set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.higgs/external/raw/catch2higgsboson/single_include/catch2)" << std::endl;
            cMakeFile << "add_library(Catch INTERFACE)" << std::endl;
            cMakeFile << "target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR} ${TEST_INCLUDE_DIR})" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake include directories for testing
            cMakeFile << "# Setup the include directories for the test target" << std::endl;
            cMakeFile << "include_directories(${PROJECT_TARGET_TEST} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake test source files
            cMakeFile << "# Setup test sources" << std::endl;
            cMakeFile << "set(TEST_SOURCES" << std::endl;
            for (const auto& item : _testFiles)
                cMakeFile << "        \"" << item.first << "\"" << std::endl;
            cMakeFile << ")" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake testing target
            cMakeFile << "# Make the test executable" << std::endl;
            cMakeFile << "add_executable(${PROJECT_TARGET_TEST} ${HIGGS_PROJECT_CACHE}/main.test.cpp ${TEST_SOURCES}" << std::endl;
            cMakeFile << "        ${${PROJECT_TARGET_MAIN}_sources} ${${PROJECT_TARGET_MAIN}_headers})" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake include source/headers for testing targets
            cMakeFile << "# Setup include directories for the test project" << std::endl;
            cMakeFile << "include_directories(${PROJECT_TARGET_TEST} \"${CMAKE_SOURCE_DIR}/src\")" << std::endl;
            cMakeFile << "include_directories(${PROJECT_TARGET_TEST} \"${CMAKE_SOURCE_DIR}/test\")" << std::endl;
            cMakeFile << "include_directories(${PROJECT_TARGET_TEST} \"${HIGGS_EXTERNAL_INCLUDES}\")" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake testing target libraries information
            cMakeFile << "# Setup the test target" << std::endl;
            cMakeFile << "target_link_libraries(${PROJECT_TARGET_TEST} ${HIGGS_EXTERNAL_LIBS})" << std::endl;
            cMakeFile << "target_link_libraries(${PROJECT_TARGET_TEST} Catch)" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake testing compile definitions
            cMakeFile << "# Setup Testing definitions" << std::endl;
            cMakeFile << "target_compile_definitions(${PROJECT_TARGET_TEST} PRIVATE CATCH_TESTING=1)" << std::endl;
            cMakeFile << std::endl;

            // Write-in the CMake testing LLVM coverage information
            cMakeFile << "# Setup the LLVM Coverage Target" << std::endl;
            cMakeFile << "add_custom_target(${PROJECT_TARGET_TEST}_coverage" << std::endl;
            cMakeFile << "        COMMAND LLVM_PROFILE_FILE=${PROJECT_TARGET_TEST}.profraw $<TARGET_FILE:${PROJECT_TARGET_TEST}>" << std::endl;
            cMakeFile << "        COMMAND llvm-profdata merge -sparse ${PROJECT_TARGET_TEST}.profraw -o ${PROJECT_TARGET_TEST}.profdata" << std::endl;
            cMakeFile << "        COMMAND llvm-cov report $<TARGET_FILE:${PROJECT_TARGET_TEST}> -instr-profile=${PROJECT_TARGET_TEST}.profdata ${CMAKE_SOURCE_DIR}/src" << std::endl;
            cMakeFile << "        COMMAND llvm-cov show $<TARGET_FILE:${PROJECT_TARGET_TEST}> -instr-profile=${PROJECT_TARGET_TEST}.profdata -show-line-counts-or-regions -output-dir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/llvm-coverage -format=\"html\" ${CMAKE_SOURCE_DIR}/src" << std::endl;
            cMakeFile << "        COMMAND echo \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/llvm-coverage/index.html in your browser to view the coverage report.\"" << std::endl;
            cMakeFile << ")" << std::endl;

            // Close the CMake file
            cMakeFile.close();

            // If we get here, mark the operation as successful
            retFlag = true;
        }
    }

    // Return the return flag
    return retFlag;
}
