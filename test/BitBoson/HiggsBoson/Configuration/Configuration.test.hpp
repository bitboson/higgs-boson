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

#ifndef HIGGS_BOSON_CONFIGURATION_TEST_HPP
#define HIGGS_BOSON_CONFIGURATION_TEST_HPP

#include <catch.hpp>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
#include <BitBoson/HiggsBoson/Configuration/Configuration.h>

using namespace BitBoson;

/**
 * Function used to create/write the Higgs-Boson Configuration file
 *
 * @param projectPath String representing the project path
 * @param configFile String representing the configuration file na
 * @return Boolean indicating whether the file was written successfully
 */
bool writeConfig(const std::string& projectPath, const std::string& configFile)
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

    // Open the configuration file
    bool configWritten = false;
    std::ofstream higgsConfFile;
    std::string confPath = projectPath + "/" + configFile;
    higgsConfFile.open(confPath);
    if (higgsConfFile.is_open())
    {

        // Write-in the standard Higgs-Boson header for the configuration file
        higgsConfFile << "project:" << std::endl;
        higgsConfFile << "  type: exe" << std::endl;
        higgsConfFile << "  name: higgs-boson" << std::endl;
        higgsConfFile << "  version: 1.0.0" << std::endl;
        higgsConfFile << "  source: src" << std::endl;
        higgsConfFile << "  test: test" << std::endl;
        higgsConfFile << "  main: src/TestProj/main.cpp" << std::endl;
        higgsConfFile << "  targets:" << std::endl;
        higgsConfFile << "    - default" << std::endl;
        higgsConfFile << "commands:" << std::endl;
        higgsConfFile << "  test:" << std::endl;
        higgsConfFile << "    pre:" << std::endl;
        higgsConfFile << "      - touch /tmp/higgs-boson/cmd1" << std::endl;
        higgsConfFile << "      - touch /tmp/higgs-boson/cmd2" << std::endl;
        higgsConfFile << "    post:" << std::endl;
        higgsConfFile << "      - touch /tmp/higgs-boson/cmd3" << std::endl;
        higgsConfFile << "  build:" << std::endl;
        higgsConfFile << "    pre:" << std::endl;
        higgsConfFile << "      - touch /tmp/higgs-boson/cmd4" << std::endl;
        higgsConfFile << "    post:" << std::endl;
        higgsConfFile << "      - touch /tmp/higgs-boson/cmd5" << std::endl;
        higgsConfFile << "dependencies:" << std::endl;
        higgsConfFile << "  - name: restbed" << std::endl;
        higgsConfFile << "    source: git" << std::endl;
        higgsConfFile << "    url:  git://github.com/bitboson-deps/restbed.git" << std::endl;
        higgsConfFile << "    rev: 4.6" << std::endl;
        higgsConfFile << "    type: manual" << std::endl;
        higgsConfFile << "    target default:" << std::endl;
        higgsConfFile << "      build:" << std::endl;
        higgsConfFile << "        - mkdir -p build" << std::endl;
        higgsConfFile << "        - cd build" << std::endl;
        higgsConfFile << "        - cmake -DBUILD_SSL=NO" << std::endl;
        higgsConfFile << "                -DBUILD_TESTS=NO" << std::endl;
        higgsConfFile << "                -DBUILD_STATIC=NO" << std::endl;
        higgsConfFile << "                -DBUILD_SHARED=ON .." << std::endl;
        higgsConfFile << "        - make" << std::endl;
        higgsConfFile << "      libs:" << std::endl;
        higgsConfFile << "        - build/librestbed.so.4" << std::endl;
        higgsConfFile << "  - name: restbed2" << std::endl;
        higgsConfFile << "    source: curl" << std::endl;
        higgsConfFile << "    url: https://raw.githubusercontent.com/bitboson-deps/restbed/4.6/source/corvusoft/restbed/http.hpp" << std::endl;
        higgsConfFile << "    type: manual" << std::endl;

        // Close the configuration file
        higgsConfFile.close();

        // If we get here, mark the operation as successful
        configWritten = true;
    }

    // Return the return flag
    return (configWritten && headerFileWritten && headerSourceFileWritten && cppFileWritten && teestingFileWritten);
}

TEST_CASE ("General Configuration Test", "[ConfigurationTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/config").c_str()) == 0);

    // Setup the configuration file
    REQUIRE (writeConfig("/tmp/higgs-boson", "higgs-boson.test.yaml"));

    // Setup a configuration based on the reference file
    auto config = Configuration("/tmp/higgs-boson", "/tmp/higgs-boson/higgs-boson.test.yaml",
            "/tmp/higgs-boson/config");

    // Verify the target-specific extensions are correct
    REQUIRE (config.getLibExtensionForTarget("android-arm") == "so");
    REQUIRE (config.getLibExtensionForTarget("android-arm64") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-arm64") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-armv5-musl") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-armv5") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-armv6") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-armv7") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-armv7a") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-mips") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-mipsel") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-ppc64le") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-s390x") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-x64") == "so");
    REQUIRE (config.getLibExtensionForTarget("linux-x86") == "so");
    REQUIRE (config.getLibExtensionForTarget("manylinux-common") == "so");
    REQUIRE (config.getLibExtensionForTarget("manylinux1-x64") == "so");
    REQUIRE (config.getLibExtensionForTarget("manylinux1-x86") == "so");
    REQUIRE (config.getLibExtensionForTarget("manylinux2010-x64") == "so");
    REQUIRE (config.getLibExtensionForTarget("manylinux2010-x86") == "so");
    REQUIRE (config.getLibExtensionForTarget("manylinux2014-aarch64") == "so");
    REQUIRE (config.getLibExtensionForTarget("manylinux2014-x64") == "so");
    REQUIRE (config.getLibExtensionForTarget("manylinux2014-x86") == "so");
    REQUIRE (config.getLibExtensionForTarget("web-wasm") == "so");
    REQUIRE (config.getLibExtensionForTarget("windows-shared-x64-posix") == "dll");
    REQUIRE (config.getLibExtensionForTarget("windows-shared-x64") == "dll");
    REQUIRE (config.getLibExtensionForTarget("windows-shared-x86") == "dll");
    REQUIRE (config.getLibExtensionForTarget("windows-static-x64-posix") == "dll");
    REQUIRE (config.getLibExtensionForTarget("windows-static-x64") == "dll");
    REQUIRE (config.getLibExtensionForTarget("windows-static-x86") == "dll");
    REQUIRE (config.getLibExtensionForTarget("x86_64-apple-darwin") == "dylib");
    REQUIRE (config.getLibExtensionForTarget("i386-apple-darwin") == "dylib");

    // Verify that the project settings are correct
    auto projectSettings = config.getProjectSettings();
    REQUIRE (projectSettings->getProjectName() == "higgs-boson");
    REQUIRE (projectSettings->getProjectType() == ProjectSettings::ProjectType::TYPE_EXE);
    REQUIRE (projectSettings->getProjectVersion() == "1.0.0");
    REQUIRE (projectSettings->getProjectSource() == "src");
    REQUIRE (projectSettings->getProjectTest() == "test");
    REQUIRE (projectSettings->getProjectMain() == "src/TestProj/main.cpp");

    // Verify that the configured targets is also correct
    auto configuredTargets = config.getConfiguredTargets();
    REQUIRE (configuredTargets.size() == 1);
    REQUIRE (configuredTargets[0] == "default");

    // Verify that the Peru sync operation is successful
    auto peruSettings = config.getPeruSettings();
    REQUIRE (peruSettings->peruSync());
    std::string peruSyncDir = "/tmp/higgs-boson/config/external/raw";
    std::string headerPath = peruSyncDir + "/restbed/source/corvusoft/restbed/http.hpp";
    std::string headerHash = "c7476e24d26a7b0330099018f6f069914c68a51c951b7f3424d533653fed6934";
    REQUIRE (ExecShell::exec("sha256sum " + headerPath) == (headerHash + "  " + headerPath + "\n"));
    headerPath = peruSyncDir + "/restbed2/http.hpp";
    REQUIRE (ExecShell::exec("sha256sum " + headerPath) == (headerHash + "  " + headerPath + "\n"));

    // Verify that the dependency checks out
    auto deps = config.getDependencies();
    REQUIRE (deps.size() == 2);
    REQUIRE (deps[0]->compileTarget("default",
            config.getLibrariesOutputForDependency(deps[0], "default"),
            config.getHeadersOutputForDependency(deps[0], "default")));

    // Verify that the dependency produced a library
    auto libs = deps[0]->getLibraries("default");
    REQUIRE (libs.size() == 1);
    REQUIRE (libs[0] == "/tmp/higgs-boson/config/external/raw/restbed/higgs-boson_default_libraries/librestbed.so.4");

    // Verify that the build command worked successfully
    REQUIRE (config.getCMakeSettings()->buildCMakeProject("default"));

    // Verify the pre/post build commands occurred
    REQUIRE (system(std::string("cat /tmp/higgs-boson/cmd4").c_str()) == 0);
    REQUIRE (system(std::string("cat /tmp/higgs-boson/cmd5").c_str()) == 0);

    // Verify that the test command worked successfully
    REQUIRE (config.getCMakeSettings()->testCMakeProject(CMakeSettings::TestType::TEST));

    // Verify the pre/post test commands occurred
    REQUIRE (system(std::string("cat /tmp/higgs-boson/cmd1").c_str()) == 0);
    REQUIRE (system(std::string("cat /tmp/higgs-boson/cmd2").c_str()) == 0);
    REQUIRE (system(std::string("cat /tmp/higgs-boson/cmd3").c_str()) == 0);

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/config").c_str()) == 0);
}

#endif //HIGGS_BOSON_CONFIGURATION_TEST_HPP
