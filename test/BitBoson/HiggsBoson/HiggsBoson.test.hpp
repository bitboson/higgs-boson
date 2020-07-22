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

#ifndef HIGGS_BOSON_HIGGS_BOSON_TEST_HPP
#define HIGGS_BOSON_HIGGS_BOSON_TEST_HPP

#include <catch.hpp>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <BitBoson/HiggsBoson/HiggsBoson.h>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>

using namespace BitBoson;

/**
 * Function used to create/write in dummy (test) C++ project files to test
 *
 * @param projectPath String representing the project directory path
 * @param isLibrary Boolean indicating whether the configuration is for
 *                  a library (true) or an executable (false)
 * @return Boolean indicating whether the files were written successfully
 */
bool writeHiggsProjectFiles(const std::string& projectPath, bool isLibrary)
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p " + projectPath + "/src/TestProj").c_str()) == 0);
    REQUIRE (system(std::string("mkdir -p " + projectPath + "/test/TestProj").c_str()) == 0);

    // Ensure the leveldb directory exists
    REQUIRE (system(std::string("mkdir -p /tmp/testdb").c_str()) == 0);

    // Clone the Catch2 Repository
    ExecShell::exec("git clone  git://github.com/bitboson-deps/Catch2.git " + projectPath + "/config/external/catch2");

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
        headerSourceFile << "#include <leveldb/db.h>" << std::endl;
        headerSourceFile << "std::string getMessage() {" << std::endl;
        headerSourceFile << "    leveldb::DB* db;" << std::endl;
        headerSourceFile << "    leveldb::Options options;" << std::endl;
        headerSourceFile << "    options.create_if_missing = true;" << std::endl;
        headerSourceFile << "    leveldb::Status status = leveldb::DB::Open(options, \"/tmp/testdb\", &db);" << std::endl;
        headerSourceFile << "    assert(status.ok());" << std::endl;
        headerSourceFile << "    return \"Hello World!\";" << std::endl;
        headerSourceFile << "};" << std::endl;

        // Close the C++ header source file
        headerSourceFile.close();

        // If we get here, mark the operation as successful
        headerSourceFileWritten = true;
    }

    // Only write the main.cpp file if this is not a library
    bool cppFileWritten = isLibrary;
    if (!isLibrary)
    {

        // Open the CPP file
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

/**
 * Function used to create/write the Higgs-Boson Configuration file
 *
 * @param confPath String representing the configuration file path
 * @param isLibrary Boolean indicating whether the configuration is for
 *                  a library (true) or an executable (false)
 * @return Boolean indicating whether the file was written successfully
 */
bool writeHiggsConfig(const std::string& confPath, bool isLibrary)
{

    // Create a return flag
    bool retFlag = false;

    // Open the configuration file
    std::ofstream higgsConfFile;
    higgsConfFile.open(confPath);
    if (higgsConfFile.is_open())
    {

        // Write-in the standard Higgs-Boson header for the configuration file
        higgsConfFile << "project:" << std::endl;
        if (isLibrary)
            higgsConfFile << "  type: lib" << std::endl;
        else
            higgsConfFile << "  type: exe" << std::endl;
        higgsConfFile << "  name: TestProj" << std::endl;
        higgsConfFile << "  version: 1.0.0" << std::endl;
        higgsConfFile << "  source: src" << std::endl;
        higgsConfFile << "  test: test" << std::endl;
        if (!isLibrary)
            higgsConfFile << "  main: src/TestProj/main.cpp" << std::endl;
        higgsConfFile << "  targets:" << std::endl;
        higgsConfFile << "    - linux-x86" << std::endl;
        higgsConfFile << "dependencies:" << std::endl;
        higgsConfFile << "  - name: leveldb" << std::endl;
        higgsConfFile << "    source: git" << std::endl;
        higgsConfFile << "    url:  git://github.com/bitboson-deps/leveldb.git" << std::endl;
        higgsConfFile << "    rev: 1.22" << std::endl;
        higgsConfFile << "    type: manual" << std::endl;
        higgsConfFile << "    target linux-x86:" << std::endl;
        higgsConfFile << "      build:" << std::endl;
        higgsConfFile << "        - mkdir -p build" << std::endl;
        higgsConfFile << "        - cd build" << std::endl;
        higgsConfFile << "        - cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=1 .." << std::endl;
        higgsConfFile << "        - make leveldb" << std::endl;
        higgsConfFile << "      libs:" << std::endl;
        higgsConfFile << "        - build/libleveldb.so.1" << std::endl;
        higgsConfFile << "      include:" << std::endl;
        higgsConfFile << "        - include/leveldb" << std::endl;
        higgsConfFile << "    target any:" << std::endl;
        higgsConfFile << "      build:" << std::endl;
        higgsConfFile << "        - mkdir -p build" << std::endl;
        higgsConfFile << "        - cd build" << std::endl;
        higgsConfFile << "        - cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=1 .." << std::endl;
        higgsConfFile << "        - make leveldb" << std::endl;
        higgsConfFile << "      libs:" << std::endl;
        higgsConfFile << "        - build/libleveldb.${LIB_EXT}.1" << std::endl;
        higgsConfFile << "      include:" << std::endl;
        higgsConfFile << "        - include/leveldb" << std::endl;
        higgsConfFile << "  - name: testdep" << std::endl;
        higgsConfFile << "    source: git" << std::endl;
        higgsConfFile << "    url:  git://github.com/bitboson-deps/TestDependency.git" << std::endl;
        higgsConfFile << "    rev: 6203c62577ae90f09fdaaaa6a953823822b3996d" << std::endl;
        higgsConfFile << "    type: higgs-boson" << std::endl;

        // Close the configuration file
        higgsConfFile.close();

        // If we get here, mark the operation as successful
        retFlag = true;
    }

    // Return the return flag
    return retFlag;
}

TEST_CASE ("General Executable Higgs-Boson Test", "[HiggsBosonTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/config").c_str()) == 0);

    // Setup the configuration and project files
    REQUIRE (writeHiggsProjectFiles("/tmp/higgs-boson", false));
    REQUIRE (writeHiggsConfig("/tmp/higgs-boson/higgs-boson.test.yaml", false));

    // Setup a Higgs-Boson instance based on the reference files
    auto higgs = HiggsBoson("/tmp/higgs-boson", "/tmp/higgs-boson/higgs-boson.test.yaml",
            "/tmp/higgs-boson/config");

    // Download the external dependencies and validate the output
    REQUIRE (higgs.download());
    std::string headerPath = "/tmp/higgs-boson/config/external/raw/leveldb/include/leveldb/db.h";
    std::string headerHash = "d296b938aa523462ae1aeb297fa6976d569009482b7b2c675b2fea8762aaed6d";
    REQUIRE (ExecShell::exec("sha256sum " + headerPath) == (headerHash + "  " + headerPath + "\n"));

    // Re-create the Configuration (Higgs-Boson) instance after downloading dependencies
    // This is terribly in-elegant, but, the Higgs-Boson YAML file was not available at
    // the time of initial instantiation, and so, we were unable to deduce build information
    // In the real-world, the download and build-deps operations are two distinct CLI invocations
    // so this is a totally acceptable (though a bit dirty) way to implement this limitation
    higgs = HiggsBoson("/tmp/higgs-boson", "/tmp/higgs-boson/higgs-boson.test.yaml",
            "/tmp/higgs-boson/config");

    // Build the external dependencies and validate the output
    REQUIRE (higgs.buildDependencies("default"));
    std::string libraryPath = "/tmp/higgs-boson/config/output/default/leveldb/libleveldb.so.1";
    std::string libraryHash = "93aebe4ea4888735cc993ca2c921fa6a65a9178cf2e2cba3760f7d7f9c9644e5";
    REQUIRE (ExecShell::exec("sha256sum " + libraryPath) == (libraryHash + "  " + libraryPath + "\n"));

    // Build the main project and validate the output
    REQUIRE (higgs.buildProject("default"));
    std::string exeOutputPath = "/tmp/higgs-boson/output/default/bin/TestProj";
    std::string exeOutputHash = "8ffb83934adee9488e915dcc00c5a43e19ee980380d0e0add46ffd1bf706e575";
    REQUIRE (ExecShell::exec("sha256sum " + exeOutputPath) == (exeOutputHash + "  " + exeOutputPath + "\n"));
    std::string libraryPathOutput = "/tmp/higgs-boson/output/default/deps/libleveldb.so.1";
    REQUIRE (ExecShell::exec("sha256sum " + libraryPathOutput) == (libraryHash + "  " + libraryPathOutput + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/config").c_str()) == 0);
}

TEST_CASE ("General Library Higgs-Boson Test", "[HiggsBosonTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/config").c_str()) == 0);

    // Setup the configuration and project files
    REQUIRE (writeHiggsProjectFiles("/tmp/higgs-boson", true));
    REQUIRE (writeHiggsConfig("/tmp/higgs-boson/higgs-boson.test.yaml", true));

    // Setup a Higgs-Boson instance based on the reference files
    auto higgs = HiggsBoson("/tmp/higgs-boson", "/tmp/higgs-boson/higgs-boson.test.yaml",
            "/tmp/higgs-boson/config");

    // Download the external dependencies and validate the output
    REQUIRE (higgs.download());
    std::string headerPath = "/tmp/higgs-boson/config/external/raw/leveldb/include/leveldb/db.h";
    std::string headerHash = "d296b938aa523462ae1aeb297fa6976d569009482b7b2c675b2fea8762aaed6d";
    REQUIRE (ExecShell::exec("sha256sum " + headerPath) == (headerHash + "  " + headerPath + "\n"));

    // Re-create the Configuration (Higgs-Boson) instance after downloading dependencies
    // This is terribly in-elegant, but, the Higgs-Boson YAML file was not available at
    // the time of initial instantiation, and so, we were unable to deduce build information
    // In the real-world, the download and build-deps operations are two distinct CLI invocations
    // so this is a totally acceptable (though a bit dirty) way to implement this limitation
    higgs = HiggsBoson("/tmp/higgs-boson", "/tmp/higgs-boson/higgs-boson.test.yaml",
            "/tmp/higgs-boson/config");

    // Build the external dependencies and validate the output
    REQUIRE (higgs.buildDependencies("default"));
    std::string libraryPath = "/tmp/higgs-boson/config/output/default/leveldb/libleveldb.so.1";
    std::string libraryHash = "93aebe4ea4888735cc993ca2c921fa6a65a9178cf2e2cba3760f7d7f9c9644e5";
    REQUIRE (ExecShell::exec("sha256sum " + libraryPath) == (libraryHash + "  " + libraryPath + "\n"));

    // Build the main project and validate the output
    REQUIRE (higgs.buildProject("default"));
    std::string libOutputPath = "/tmp/higgs-boson/output/default/lib/libTestProj.so";
    std::string libOutputHash = "1d60766f6195baadaccdd4acef566ff40532a8a97a2d966bf51fc78bf036e8c6";
    REQUIRE (ExecShell::exec("sha256sum " + libOutputPath) == (libOutputHash + "  " + libOutputPath + "\n"));
    std::string libraryPathOutput = "/tmp/higgs-boson/output/default/deps/libleveldb.so.1";
    REQUIRE (ExecShell::exec("sha256sum " + libraryPathOutput) == (libraryHash + "  " + libraryPathOutput + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/config").c_str()) == 0);
}

TEST_CASE ("Higgs-Boson Dependency Artifacts Test", "[HiggsBosonTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/config").c_str()) == 0);

    // Setup the configuration and project files
    REQUIRE (writeHiggsProjectFiles("/tmp/higgs-boson", false));
    REQUIRE (writeHiggsConfig("/tmp/higgs-boson/higgs-boson.test.yaml", false));

    // Setup a Higgs-Boson instance based on the reference files
    auto higgs = HiggsBoson("/tmp/higgs-boson", "/tmp/higgs-boson/higgs-boson.test.yaml",
            "/tmp/higgs-boson/config");

    // Download the external dependencies and validate the output
    REQUIRE (higgs.download());

    // Re-create the Configuration (Higgs-Boson) instance after downloading dependencies
    // This is terribly in-elegant, but, the Higgs-Boson YAML file was not available at
    // the time of initial instantiation, and so, we were unable to deduce build information
    // In the real-world, the download and build-deps operations are two distinct CLI invocations
    // so this is a totally acceptable (though a bit dirty) way to implement this limitation
    higgs = HiggsBoson("/tmp/higgs-boson", "/tmp/higgs-boson/higgs-boson.test.yaml",
            "/tmp/higgs-boson/config");

    // Build the external dependencies and validate the output
    REQUIRE (higgs.buildDependencies("default"));
    std::string headerPath = "/tmp/higgs-boson/config/external/raw/testdep/higgs-boson_default_headers/TestProj/helper.h";
    std::string headerHash = "a83e1b70aaeeb884b6bf41b106892f31f610677f8e8bf166961c26bce8d61e35";
    REQUIRE (ExecShell::exec("sha256sum " + headerPath) == (headerHash + "  " + headerPath + "\n"));
    std::string libraryPath = "/tmp/higgs-boson/config/external/raw/testdep/output/default/deps/libleveldb.so.1";
    std::string libraryHash = "93aebe4ea4888735cc993ca2c921fa6a65a9178cf2e2cba3760f7d7f9c9644e5";
    REQUIRE (ExecShell::exec("sha256sum " + libraryPath) == (libraryHash + "  " + libraryPath + "\n"));
    libraryPath = "/tmp/higgs-boson/config/external/raw/testdep/output/default/lib/libTestProj.so";
    libraryHash = "a3acf40dbeb1cfd445d9b009e6ec5f43ee55defcf1c759cdddb46eb98df80fc8";
    REQUIRE (ExecShell::exec("sha256sum " + libraryPath) == (libraryHash + "  " + libraryPath + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/config").c_str()) == 0);
}

TEST_CASE ("General Test Higgs-Boson Test", "[HiggsBosonTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/config").c_str()) == 0);

    // Setup the configuration and project files
    REQUIRE (writeHiggsProjectFiles("/tmp/higgs-boson", false));
    REQUIRE (writeHiggsConfig("/tmp/higgs-boson/higgs-boson.test.yaml", false));

    // Test setting the run-type to "sh" manually
    HiggsBoson::RunTypeSingleton::setRunTypeCommand("sh");

    // Setup a Higgs-Boson instance based on the reference files
    auto higgs = HiggsBoson("/tmp/higgs-boson", "/tmp/higgs-boson/higgs-boson.test.yaml",
            "/tmp/higgs-boson/config");

    // Download the external dependencies and validate the output
    REQUIRE (higgs.download());
    std::string headerPath = "/tmp/higgs-boson/config/external/raw/leveldb/include/leveldb/db.h";
    std::string headerHash = "d296b938aa523462ae1aeb297fa6976d569009482b7b2c675b2fea8762aaed6d";
    REQUIRE (ExecShell::exec("sha256sum " + headerPath) == (headerHash + "  " + headerPath + "\n"));

    // Re-create the Configuration (Higgs-Boson) instance after downloading dependencies
    // This is terribly in-elegant, but, the Higgs-Boson YAML file was not available at
    // the time of initial instantiation, and so, we were unable to deduce build information
    // In the real-world, the download and build-deps operations are two distinct CLI invocations
    // so this is a totally acceptable (though a bit dirty) way to implement this limitation
    higgs = HiggsBoson("/tmp/higgs-boson", "/tmp/higgs-boson/higgs-boson.test.yaml",
            "/tmp/higgs-boson/config");

    // Build the external dependencies and validate the output
    REQUIRE (higgs.buildDependencies("default"));
    std::string libraryPath = "/tmp/higgs-boson/config/output/default/leveldb/libleveldb.so.1";
    std::string libraryHash = "93aebe4ea4888735cc993ca2c921fa6a65a9178cf2e2cba3760f7d7f9c9644e5";
    REQUIRE (ExecShell::exec("sha256sum " + libraryPath) == (libraryHash + "  " + libraryPath + "\n"));

    // Build the main project and validate the output
    REQUIRE (higgs.testProject(CMakeSettings::TestType::TEST));
    std::string testExePath = "/tmp/higgs-boson/config/builds/test/bin/TestProj_test";
    std::string testExeHash = "b95086cff4de93ba72ad9f30e7a6d22d64607ddc0f71c9c97836502e8fa05109";
    REQUIRE (ExecShell::exec("sha256sum " + testExePath) == (testExeHash + "  " + testExePath + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/config").c_str()) == 0);
}

#endif //HIGGS_BOSON_HIGGS_BOSON_TEST_HPP
