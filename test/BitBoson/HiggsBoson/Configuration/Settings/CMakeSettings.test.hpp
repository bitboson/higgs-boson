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

#ifndef HIGGS_BOSON_CMAKE_SETTINGS_TEST_HPP
#define HIGGS_BOSON_CMAKE_SETTINGS_TEST_HPP

#include <catch.hpp>
#include <string>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
#include <BitBoson/HiggsBoson/Configuration/Settings/CMakeSettings.h>

using namespace BitBoson;

/**
 * Function used to create/write in dummy (test) C++ project files to test
 *
 * @param projectPath String representing the project directory path
 * @return Boolean indicating whether the files were written successfully
 */
bool writeProjectFiles(const std::string& projectPath)
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p " + projectPath + "/src/TestProj").c_str()) == 0);
    REQUIRE (system(std::string("mkdir -p " + projectPath + "/test/TestProj").c_str()) == 0);
    REQUIRE (system(std::string("mkdir -p " + projectPath + "/.higgs-boson/external/raw").c_str()) == 0);

    // Clone the Catch2 Repository
    ExecShell::exec("git clone  git://github.com/bitboson-deps/Catch2.git " + projectPath + "/.higgs-boson/external/raw/catch2higgsboson");

    // Open the Header file
    bool headerFileWritten = false;
    std::ofstream headerFile;
    headerFile.open(projectPath + "/src/TestProj/helper.h");
    if (headerFile.is_open())
    {

        // Write-in the C++ header file information
        headerFile << "#include <string>" << std::endl;
        headerFile << "#ifndef HIGGS_BOSON_HELPER_H" << std::endl;
        headerFile << "#define HIGGS_BOSON_HELPER_H" << std::endl;
        headerFile << "std::string getMessage();" << std::endl;
        headerFile << "#endif // HIGGS_BOSON_HELPER_H" << std::endl;

        // Close the C++ header file
        headerFile.close();

        // If we get here, mark the operation as successful
        headerFileWritten = true;
    }

    // Open the Header Source file
    bool headerSourceFileWritten = false;
    std::ofstream headerSourceFile;
    headerSourceFile.open(projectPath + "/src/TestProj/helper.cpp");
    if (headerSourceFile.is_open())
    {

        // Write-in the C++ header source file information
        headerSourceFile << "#include <TestProj/helper.h>" << std::endl;
        headerSourceFile << "std::string getMessage() { return \"Hello World!\"; };" << std::endl;

        // Close the C++ header source file
        headerSourceFile.close();

        // If we get here, mark the operation as successful
        headerSourceFileWritten = true;
    }

    // Open the CPP file
    bool cppFileWritten = false;
    std::ofstream cppFile;
    cppFile.open(projectPath + "/src/TestProj/main.cpp");
    if (cppFile.is_open())
    {

        // Write-in the C++ source file information
        cppFile << "#include <iostream>" << std::endl;
        cppFile << "#include <TestProj/helper.h>" << std::endl;
        cppFile << "int main() { std::cout << getMessage(); return 0; };" << std::endl;

        // Close the C++ file
        cppFile.close();

        // If we get here, mark the operation as successful
        cppFileWritten = true;
    }

    // Open the testing file
    bool teestingFileWritten = false;
    std::ofstream testFile;
    testFile.open(projectPath + "/test/TestProj/helper.test.hpp");
    if (testFile.is_open())
    {

        // Write-in the C++ testing file information
        testFile << "#ifndef HIGGS_BOSON_HELPER_TEST_HPP" << std::endl;
        testFile << "#define HIGGS_BOSON_HELPER_TEST_HPP" << std::endl;
        testFile << "#include <string>" << std::endl;
        testFile << "#include <TestProj/helper.h>" << std::endl;
        testFile << "TEST_CASE (\"Test1\", \"[TestSect1]\") { REQUIRE (getMessage() == \"Hello World!\"); }" << std::endl;
        testFile << "#endif // HIGGS_BOSON_HELPER_TEST_HPP" << std::endl;

        // Close the C++ testing file
        testFile.close();

        // If we get here, mark the operation as successful
        teestingFileWritten = true;
    }

    // Return the results
    return (headerFileWritten && headerSourceFileWritten && cppFileWritten && teestingFileWritten);
}

TEST_CASE ("External Libraries and Includes CMake Settings Test", "[CMakeSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/test-proj").c_str()) == 0);

    // Setup the project testing files
    REQUIRE (writeProjectFiles("/tmp/higgs-boson/test-proj"));

    // Setup the CMake Settings object
    auto cMakeSettings = CMakeSettings("test-proj", "1.0.0",
            "/tmp/higgs-boson/test-proj", "/tmp/higgs-boson/test-proj/.higgs-boson");

    // Setup the CMake project files (source, header, and testing)
    cMakeSettings.setMainSource("/tmp/higgs-boson/test-proj/src/TestProj/main.cpp");
    REQUIRE (cMakeSettings.addHeaderFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.h"));
    REQUIRE (cMakeSettings.addSourceFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.cpp"));
    REQUIRE (cMakeSettings.addTestingFile("/tmp/higgs-boson/test-proj/test/TestProj/helper.test.hpp"));

    // Add-in the external libraries and includes to the settings
    cMakeSettings.addLibrary("/tmp/higgs-boson/test-proj/test/TestProj/libs/test.so");
    cMakeSettings.addLibrary("/tmp/higgs-boson/test-proj/test/TestProj/libs/test.so.4");
    cMakeSettings.addLibrary("/tmp/higgs-boson/test-proj/test/TestProj/libs/library.so.1");
    cMakeSettings.addIncludeDir("/tmp/higgs-boson/test-proj/test/TestProj/includes/test");
    cMakeSettings.addIncludeDir("/tmp/higgs-boson/test-proj/test/TestProj/includes1/blah/test");
    cMakeSettings.addIncludeDir("/tmp/higgs-boson/test-proj/test/TestProj/includes2/blah/blah");

    // Build the C++ project and validate its outputs
    // NOTE: This will fail since the libraries are dummy libraries
    REQUIRE (!cMakeSettings.buildCMakeProject("default"));

    // Verify the contents of the CMakeLists.txt file
    std::string cMakeHash = "46b576bf2aeef96429cb5263021f34b98059d5b090d98607bafbd2c80ba60469";
    std::string cMakeFile = "/tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt";
    REQUIRE (ExecShell::exec("sha256sum " + cMakeFile) == (cMakeHash + "  " + cMakeFile + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

TEST_CASE ("Generic Executable CMake Settings Test", "[CMakeSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/test-proj").c_str()) == 0);

    // Setup the project testing files
    REQUIRE (writeProjectFiles("/tmp/higgs-boson/test-proj"));

    // Setup the CMake Settings object
    auto cMakeSettings = CMakeSettings("test-proj", "1.0.0",
            "/tmp/higgs-boson/test-proj", "/tmp/higgs-boson/test-proj/.higgs-boson");

    // Setup the CMake project files (source, header, and testing)
    cMakeSettings.setMainSource("/tmp/higgs-boson/test-proj/src/TestProj/main.cpp");
    REQUIRE (cMakeSettings.addHeaderFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.h"));
    REQUIRE (cMakeSettings.addSourceFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.cpp"));
    REQUIRE (cMakeSettings.addTestingFile("/tmp/higgs-boson/test-proj/test/TestProj/helper.test.hpp"));

    // Build the C++ project and validate its outputs
    REQUIRE (cMakeSettings.buildCMakeProject("default"));

    // Verify the contents of the CMakeLists.txt file
    std::string cMakeHash = "51c34ae63e162f543003afc0fe73f854dd54faade9c1a92a4c10e941a5881252";
    std::string cMakeFile = "/tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt";
    REQUIRE (ExecShell::exec("sha256sum " + cMakeFile) == (cMakeHash + "  " + cMakeFile + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

TEST_CASE ("Generic Library CMake Settings Test", "[CMakeSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/test-proj").c_str()) == 0);

    // Setup the project testing files
    REQUIRE (writeProjectFiles("/tmp/higgs-boson/test-proj"));

    // Setup the CMake Settings object
    auto cMakeSettings = CMakeSettings("test-proj", "1.0.0",
            "/tmp/higgs-boson/test-proj", "/tmp/higgs-boson/test-proj/.higgs-boson");

    // Setup the CMake project files (source, header, and testing)
    REQUIRE (cMakeSettings.addHeaderFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.h"));
    REQUIRE (cMakeSettings.addSourceFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.cpp"));
    REQUIRE (cMakeSettings.addTestingFile("/tmp/higgs-boson/test-proj/test/TestProj/helper.test.hpp"));

    // Build the C++ project and validate its outputs
    REQUIRE (cMakeSettings.buildCMakeProject("default"));

    // Verify the contents of the CMakeLists.txt file
    std::string cMakeHash = "e08fc9bb23364512972a16c6078175c188c865b54ceea65a600c3590b862fcb4";
    std::string cMakeFile = "/tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt";
    REQUIRE (ExecShell::exec("sha256sum " + cMakeFile) == (cMakeHash + "  " + cMakeFile + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

TEST_CASE ("Pre-Build and Post-Build Commands CMake Settings Test", "[CMakeSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/test-proj").c_str()) == 0);

    // Setup the project testing files
    REQUIRE (writeProjectFiles("/tmp/higgs-boson/test-proj"));

    // Setup the CMake Settings object
    auto cMakeSettings = CMakeSettings("test-proj", "1.0.0",
            "/tmp/higgs-boson/test-proj", "/tmp/higgs-boson/test-proj/.higgs-boson");

    // Setup the CMake project files (source, header, and testing)
    cMakeSettings.setMainSource("/tmp/higgs-boson/test-proj/src/TestProj/main.cpp");
    REQUIRE (cMakeSettings.addHeaderFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.h"));
    REQUIRE (cMakeSettings.addSourceFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.cpp"));
    REQUIRE (cMakeSettings.addTestingFile("/tmp/higgs-boson/test-proj/test/TestProj/helper.test.hpp"));

    // Add-in some pre-build commands
    cMakeSettings.addPreBuildCommand("mv /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt.tmp");
    cMakeSettings.addPreBuildCommand("mv /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt.tmp /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt");

    // Add-in some post-build commands
    cMakeSettings.addPostBuildCommand("rm -rf /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt");

    // Build the C++ project and validate its outputs
    REQUIRE (cMakeSettings.buildCMakeProject("default"));

    // Verify the CMakeList.txt file was removed by the post-build commands
    REQUIRE (system(std::string("ls -ltr /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt").c_str()) != 0);

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

TEST_CASE ("Run Test CMake Settings Test", "[CMakeSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/test-proj").c_str()) == 0);

    // Setup the project testing files
    REQUIRE (writeProjectFiles("/tmp/higgs-boson/test-proj"));

    // Setup the CMake Settings object
    auto cMakeSettings = CMakeSettings("test-proj", "1.0.0",
            "/tmp/higgs-boson/test-proj", "/tmp/higgs-boson/test-proj/.higgs-boson");

    // Setup the CMake project files (source, header, and testing)
    cMakeSettings.setMainSource("/tmp/higgs-boson/test-proj/src/TestProj/main.cpp");
    REQUIRE (cMakeSettings.addHeaderFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.h"));
    REQUIRE (cMakeSettings.addSourceFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.cpp"));
    REQUIRE (cMakeSettings.addTestingFile("/tmp/higgs-boson/test-proj/test/TestProj/helper.test.hpp"));

    // Build the C++ project and validate its outputs
    REQUIRE (cMakeSettings.testCMakeProject(CMakeSettings::TestType::TEST));

    // Verify the contents of the CMakeLists.txt file
    std::string cMakeHash = "e08fc9bb23364512972a16c6078175c188c865b54ceea65a600c3590b862fcb4";
    std::string cMakeFile = "/tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt";
    REQUIRE (ExecShell::exec("sha256sum " + cMakeFile) == (cMakeHash + "  " + cMakeFile + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

TEST_CASE ("Run Test with Commands CMake Settings Test", "[CMakeSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/test-proj").c_str()) == 0);

    // Setup the project testing files
    REQUIRE (writeProjectFiles("/tmp/higgs-boson/test-proj"));

    // Setup the CMake Settings object
    auto cMakeSettings = CMakeSettings("test-proj", "1.0.0",
            "/tmp/higgs-boson/test-proj", "/tmp/higgs-boson/test-proj/.higgs-boson");

    // Setup the CMake project files (source, header, and testing)
    cMakeSettings.setMainSource("/tmp/higgs-boson/test-proj/src/TestProj/main.cpp");
    REQUIRE (cMakeSettings.addHeaderFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.h"));
    REQUIRE (cMakeSettings.addSourceFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.cpp"));
    REQUIRE (cMakeSettings.addTestingFile("/tmp/higgs-boson/test-proj/test/TestProj/helper.test.hpp"));

    // Add-in some pre-test commands
    cMakeSettings.addPreTestCommand("mv /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt.tmp");
    cMakeSettings.addPreTestCommand("mv /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt.tmp /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt");

    // Add-in some post-test commands
    cMakeSettings.addPostTestCommand("rm -rf /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt");

    // Build the C++ project and validate its outputs
    REQUIRE (cMakeSettings.testCMakeProject(CMakeSettings::TestType::TEST));

    // Verify the CMakeList.txt file was removed by the post-build commands
    REQUIRE (system(std::string("ls -ltr /tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt").c_str()) != 0);

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

TEST_CASE ("Run Coverage CMake Settings Test", "[CMakeSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/test-proj").c_str()) == 0);

    // Setup the project testing files
    REQUIRE (writeProjectFiles("/tmp/higgs-boson/test-proj"));

    // Setup the CMake Settings object
    auto cMakeSettings = CMakeSettings("test-proj", "1.0.0",
            "/tmp/higgs-boson/test-proj", "/tmp/higgs-boson/test-proj/.higgs-boson");

    // Setup the CMake project files (source, header, and testing)
    cMakeSettings.setMainSource("/tmp/higgs-boson/test-proj/src/TestProj/main.cpp");
    REQUIRE (cMakeSettings.addHeaderFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.h"));
    REQUIRE (cMakeSettings.addSourceFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.cpp"));
    REQUIRE (cMakeSettings.addTestingFile("/tmp/higgs-boson/test-proj/test/TestProj/helper.test.hpp"));

    // Build the C++ project and validate its outputs
    REQUIRE (cMakeSettings.testCMakeProject(CMakeSettings::TestType::COVERAGE));

    // Verify the contents of the CMakeLists.txt file
    std::string cMakeHash = "e08fc9bb23364512972a16c6078175c188c865b54ceea65a600c3590b862fcb4";
    std::string cMakeFile = "/tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt";
    REQUIRE (ExecShell::exec("sha256sum " + cMakeFile) == (cMakeHash + "  " + cMakeFile + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

TEST_CASE ("Run Sanitize-Address CMake Settings Test", "[CMakeSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/test-proj").c_str()) == 0);

    // Setup the project testing files
    REQUIRE (writeProjectFiles("/tmp/higgs-boson/test-proj"));

    // Setup the CMake Settings object
    auto cMakeSettings = CMakeSettings("test-proj", "1.0.0",
            "/tmp/higgs-boson/test-proj", "/tmp/higgs-boson/test-proj/.higgs-boson");

    // Setup the CMake project files (source, header, and testing)
    cMakeSettings.setMainSource("/tmp/higgs-boson/test-proj/src/TestProj/main.cpp");
    REQUIRE (cMakeSettings.addHeaderFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.h"));
    REQUIRE (cMakeSettings.addSourceFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.cpp"));
    REQUIRE (cMakeSettings.addTestingFile("/tmp/higgs-boson/test-proj/test/TestProj/helper.test.hpp"));

    // Build the C++ project and validate its outputs
    REQUIRE (cMakeSettings.testCMakeProject(CMakeSettings::TestType::SANITIZE_ADDRESS));

    // Verify the contents of the CMakeLists.txt file
    std::string cMakeHash = "e08fc9bb23364512972a16c6078175c188c865b54ceea65a600c3590b862fcb4";
    std::string cMakeFile = "/tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt";
    REQUIRE (ExecShell::exec("sha256sum " + cMakeFile) == (cMakeHash + "  " + cMakeFile + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

TEST_CASE ("Run Sanitize-Behavior CMake Settings Test", "[CMakeSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/test-proj").c_str()) == 0);

    // Setup the project testing files
    REQUIRE (writeProjectFiles("/tmp/higgs-boson/test-proj"));

    // Setup the CMake Settings object
    auto cMakeSettings = CMakeSettings("test-proj", "1.0.0",
            "/tmp/higgs-boson/test-proj", "/tmp/higgs-boson/test-proj/.higgs-boson");

    // Setup the CMake project files (source, header, and testing)
    cMakeSettings.setMainSource("/tmp/higgs-boson/test-proj/src/TestProj/main.cpp");
    REQUIRE (cMakeSettings.addHeaderFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.h"));
    REQUIRE (cMakeSettings.addSourceFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.cpp"));
    REQUIRE (cMakeSettings.addTestingFile("/tmp/higgs-boson/test-proj/test/TestProj/helper.test.hpp"));

    // Build the C++ project and validate its outputs
    REQUIRE (cMakeSettings.testCMakeProject(CMakeSettings::TestType::SANITIZE_BEHAVIOR));

    // Verify the contents of the CMakeLists.txt file
    std::string cMakeHash = "e08fc9bb23364512972a16c6078175c188c865b54ceea65a600c3590b862fcb4";
    std::string cMakeFile = "/tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt";
    REQUIRE (ExecShell::exec("sha256sum " + cMakeFile) == (cMakeHash + "  " + cMakeFile + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

TEST_CASE ("Run Sanitize-Thread CMake Settings Test", "[CMakeSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/test-proj").c_str()) == 0);

    // Setup the project testing files
    REQUIRE (writeProjectFiles("/tmp/higgs-boson/test-proj"));

    // Setup the CMake Settings object
    auto cMakeSettings = CMakeSettings("test-proj", "1.0.0",
            "/tmp/higgs-boson/test-proj", "/tmp/higgs-boson/test-proj/.higgs-boson");

    // Setup the CMake project files (source, header, and testing)
    cMakeSettings.setMainSource("/tmp/higgs-boson/test-proj/src/TestProj/main.cpp");
    REQUIRE (cMakeSettings.addHeaderFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.h"));
    REQUIRE (cMakeSettings.addSourceFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.cpp"));
    REQUIRE (cMakeSettings.addTestingFile("/tmp/higgs-boson/test-proj/test/TestProj/helper.test.hpp"));

    // Build the C++ project and validate its outputs
    REQUIRE (cMakeSettings.testCMakeProject(CMakeSettings::TestType::SANITIZE_THREAD));

    // Verify the contents of the CMakeLists.txt file
    std::string cMakeHash = "e08fc9bb23364512972a16c6078175c188c865b54ceea65a600c3590b862fcb4";
    std::string cMakeFile = "/tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt";
    REQUIRE (ExecShell::exec("sha256sum " + cMakeFile) == (cMakeHash + "  " + cMakeFile + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

TEST_CASE ("Run Sanitize-Leak CMake Settings Test", "[CMakeSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/test-proj").c_str()) == 0);

    // Setup the project testing files
    REQUIRE (writeProjectFiles("/tmp/higgs-boson/test-proj"));

    // Setup the CMake Settings object
    auto cMakeSettings = CMakeSettings("test-proj", "1.0.0",
            "/tmp/higgs-boson/test-proj", "/tmp/higgs-boson/test-proj/.higgs-boson");

    // Setup the CMake project files (source, header, and testing)
    cMakeSettings.setMainSource("/tmp/higgs-boson/test-proj/src/TestProj/main.cpp");
    REQUIRE (cMakeSettings.addHeaderFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.h"));
    REQUIRE (cMakeSettings.addSourceFile("/tmp/higgs-boson/test-proj/src/TestProj/helper.cpp"));
    REQUIRE (cMakeSettings.addTestingFile("/tmp/higgs-boson/test-proj/test/TestProj/helper.test.hpp"));

    // Build the C++ project and validate its outputs
    REQUIRE (cMakeSettings.testCMakeProject(CMakeSettings::TestType::SANITIZE_LEAK));

    // Verify the contents of the CMakeLists.txt file
    std::string cMakeHash = "e08fc9bb23364512972a16c6078175c188c865b54ceea65a600c3590b862fcb4";
    std::string cMakeFile = "/tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt";
    REQUIRE (ExecShell::exec("sha256sum " + cMakeFile) == (cMakeHash + "  " + cMakeFile + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

#endif //HIGGS_BOSON_CMAKE_SETTINGS_TEST_HPP
