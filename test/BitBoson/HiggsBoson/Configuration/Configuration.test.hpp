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
#include <BitBoson/HiggsBoson/Utils/FileWriter.h>
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
    ExecShell::exec("git clone  https://github.com/bitboson-deps/Catch2.git " + projectPath + "/.higgs-boson/external/raw/catch2higgsboson");

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

    // Open the configuration file
    bool configWritten = false;
    std::string confPath = projectPath + "/" + configFile;
    auto higgsConfFile = FileWriter(confPath);
    if (higgsConfFile.isOpen())
    {

        // Write-in the standard Higgs-Boson header for the configuration file
        higgsConfFile.writeLine("project:");
        higgsConfFile.writeLine("  type: exe");
        higgsConfFile.writeLine("  name: higgs-boson");
        higgsConfFile.writeLine("  version: 1.0.0");
        higgsConfFile.writeLine("  source: src");
        higgsConfFile.writeLine("  test: test");
        higgsConfFile.writeLine("  main: src/TestProj/main.cpp");
        higgsConfFile.writeLine("  targets:");
        higgsConfFile.writeLine("    - default");
        higgsConfFile.writeLine("commands:");
        higgsConfFile.writeLine("  test:");
        higgsConfFile.writeLine("    pre:");
        higgsConfFile.writeLine("      - touch /tmp/higgs-boson/cmd1");
        higgsConfFile.writeLine("      - touch /tmp/higgs-boson/cmd2");
        higgsConfFile.writeLine("    post:");
        higgsConfFile.writeLine("      - touch /tmp/higgs-boson/cmd3");
        higgsConfFile.writeLine("  build:");
        higgsConfFile.writeLine("    pre:");
        higgsConfFile.writeLine("      - touch /tmp/higgs-boson/cmd4");
        higgsConfFile.writeLine("    post:");
        higgsConfFile.writeLine("      - touch /tmp/higgs-boson/cmd5");
        higgsConfFile.writeLine("dependencies:");
        higgsConfFile.writeLine("  - name: restbed");
        higgsConfFile.writeLine("    source: git");
        higgsConfFile.writeLine("    url:  https://github.com/bitboson-deps/restbed.git");
        higgsConfFile.writeLine("    rev: 4.6");
        higgsConfFile.writeLine("    type: manual");
        higgsConfFile.writeLine("    target default:");
        higgsConfFile.writeLine("      build:");
        higgsConfFile.writeLine("        - mkdir -p build");
        higgsConfFile.writeLine("        - cd build");
        higgsConfFile.writeLine("        - cmake -DBUILD_SSL=NO");
        higgsConfFile.writeLine("                -DBUILD_TESTS=NO");
        higgsConfFile.writeLine("                -DBUILD_STATIC=NO");
        higgsConfFile.writeLine("                -DBUILD_SHARED=ON ..");
        higgsConfFile.writeLine("        - make");
        higgsConfFile.writeLine("      libs:");
        higgsConfFile.writeLine("        - build/librestbed.so.4");
        higgsConfFile.writeLine("  - name: restbed2");
        higgsConfFile.writeLine("    source: curl");
        higgsConfFile.writeLine("    url: https://raw.githubusercontent.com/bitboson-deps/restbed/4.6/source/corvusoft/restbed/http.hpp");
        higgsConfFile.writeLine("    type: manual");

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
