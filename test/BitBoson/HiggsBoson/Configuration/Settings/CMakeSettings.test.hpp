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
#include <BitBoson/HiggsBoson/Utils/FileWriter.h>
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
    ExecShell::exec("git clone https://github.com/bitboson-deps/Catch2.git " + projectPath + "/.higgs-boson/external/raw/catch2higgsboson");

    // Clone the plibsys Repository
    ExecShell::exec("git clone https://github.com/saprykin/plibsys.git " + projectPath + "/.higgs-boson/external/raw/plibsyshiggsboson");

    // Open the Header file
    bool headerFileWritten = false;
    auto headerFile = FileWriter(projectPath + "/src/TestProj/helper.h");
    if (headerFile.isOpen())
    {

        // Write-in the C++ header file information
        headerFile.writeLine("#include <string>");
        headerFile.writeLine("#ifndef HIGGS_BOSON_HELPER_H");
        headerFile.writeLine("#define HIGGS_BOSON_HELPER_H");
        headerFile.writeLine("std::string getMessage();");
        headerFile.writeLine("#endif // HIGGS_BOSON_HELPER_H");

        // Close the C++ header file
        headerFile.close();

        // If we get here, mark the operation as successful
        headerFileWritten = true;
    }

    // Open the Header Source file
    bool headerSourceFileWritten = false;
    auto headerSourceFile = FileWriter(projectPath + "/src/TestProj/helper.cpp");
    if (headerSourceFile.isOpen())
    {

        // Write-in the C++ header source file information
        headerSourceFile.writeLine("#include <TestProj/helper.h>");
        headerSourceFile.writeLine("std::string getMessage() { return \"Hello World!\"; };");

        // Close the C++ header source file
        headerSourceFile.close();

        // If we get here, mark the operation as successful
        headerSourceFileWritten = true;
    }

    // Open the CPP file
    bool cppFileWritten = false;
    auto cppFile = FileWriter(projectPath + "/src/TestProj/main.cpp");
    if (cppFile.isOpen())
    {

        // Write-in the C++ source file information
        cppFile.writeLine("#include <iostream>");
        cppFile.writeLine("#include <TestProj/helper.h>");
        cppFile.writeLine("int main() { std::cout << getMessage(); return 0; };");

        // Close the C++ file
        cppFile.close();

        // If we get here, mark the operation as successful
        cppFileWritten = true;
    }

    // Open the testing file
    bool teestingFileWritten = false;
    auto testFile = FileWriter(projectPath + "/test/TestProj/helper.test.hpp");
    if (testFile.isOpen())
    {

        // Write-in the C++ testing file information
        testFile.writeLine("#ifndef HIGGS_BOSON_HELPER_TEST_HPP");
        testFile.writeLine("#define HIGGS_BOSON_HELPER_TEST_HPP");
        testFile.writeLine("#include <string>");
        testFile.writeLine("#include <TestProj/helper.h>");
        testFile.writeLine("TEST_CASE (\"Test1\", \"[TestSect1]\") { REQUIRE (getMessage() == \"Hello World!\"); }");
        testFile.writeLine("#endif // HIGGS_BOSON_HELPER_TEST_HPP");

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
    std::string cMakeHash = "07f35d601aecffbc18bd3b8ec751df3e28be2c97b4523ee171a30c980e2ad7fd";
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
    std::string cMakeHash = "91e42b56cd068e1d703d6965c1b9673e25421635ff45fb0254aec4bcfa7e9fb0";
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
    std::string cMakeHash = "bf3e399c351c5836ca9b0a9e2b30ded29600f0a8bc9df5c5df1de5fdc82287d9";
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
    std::string cMakeHash = "bf3e399c351c5836ca9b0a9e2b30ded29600f0a8bc9df5c5df1de5fdc82287d9";
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
    std::string cMakeHash = "bf3e399c351c5836ca9b0a9e2b30ded29600f0a8bc9df5c5df1de5fdc82287d9";
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
    std::string cMakeHash = "bf3e399c351c5836ca9b0a9e2b30ded29600f0a8bc9df5c5df1de5fdc82287d9";
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
    std::string cMakeHash = "bf3e399c351c5836ca9b0a9e2b30ded29600f0a8bc9df5c5df1de5fdc82287d9";
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
    std::string cMakeHash = "bf3e399c351c5836ca9b0a9e2b30ded29600f0a8bc9df5c5df1de5fdc82287d9";
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
    std::string cMakeHash = "bf3e399c351c5836ca9b0a9e2b30ded29600f0a8bc9df5c5df1de5fdc82287d9";
    std::string cMakeFile = "/tmp/higgs-boson/test-proj/.higgs-boson/CMakeLists.txt";
    REQUIRE (ExecShell::exec("sha256sum " + cMakeFile) == (cMakeHash + "  " + cMakeFile + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/test-proj").c_str()) == 0);
}

#endif //HIGGS_BOSON_CMAKE_SETTINGS_TEST_HPP
