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
#include <BitBoson/HiggsBoson/HiggsBoson.h>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
#include <BitBoson/HiggsBoson/Utils/FileWriter.h>

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
    ExecShell::exec("git clone  https://github.com/bitboson-deps/Catch2.git " + projectPath + "/config/external/catch2");

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
        headerSourceFile.writeLine("#include <leveldb/db.h>");
        headerSourceFile.writeLine("std::string getMessage() {");
        headerSourceFile.writeLine("    leveldb::DB* db;");
        headerSourceFile.writeLine("    leveldb::Options options;");
        headerSourceFile.writeLine("    options.create_if_missing = true;");
        headerSourceFile.writeLine("    leveldb::Status status = leveldb::DB::Open(options, \"/tmp/testdb\", &db);");
        headerSourceFile.writeLine("    assert(status.ok());");
        headerSourceFile.writeLine("    return \"Hello World!\";");
        headerSourceFile.writeLine("};");

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
    auto higgsConfFile = FileWriter(confPath);
    if (higgsConfFile.isOpen())
    {

        // Write-in the standard Higgs-Boson header for the configuration file
        higgsConfFile.writeLine("project:");
        if (isLibrary)
            higgsConfFile.writeLine("  type: lib");
        else
            higgsConfFile.writeLine("  type: exe");
        higgsConfFile.writeLine("  name: TestProj");
        higgsConfFile.writeLine("  version: 1.0.0");
        higgsConfFile.writeLine("  source: src");
        higgsConfFile.writeLine("  test: test");
        if (!isLibrary)
            higgsConfFile.writeLine("  main: src/TestProj/main.cpp");
        higgsConfFile.writeLine("  targets:");
        higgsConfFile.writeLine("    - linux-x86");
        higgsConfFile.writeLine("dependencies:");
        higgsConfFile.writeLine("  - name: leveldb");
        higgsConfFile.writeLine("    source: git");
        higgsConfFile.writeLine("    url:  https://github.com/bitboson-deps/leveldb.git");
        higgsConfFile.writeLine("    rev: 1.22");
        higgsConfFile.writeLine("    type: manual");
        higgsConfFile.writeLine("    target linux-x86:");
        higgsConfFile.writeLine("      build:");
        higgsConfFile.writeLine("        - mkdir -p build");
        higgsConfFile.writeLine("        - cd build");
        higgsConfFile.writeLine("        - cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=1 ..");
        higgsConfFile.writeLine("        - make leveldb");
        higgsConfFile.writeLine("      libs:");
        higgsConfFile.writeLine("        - build/libleveldb.so.1");
        higgsConfFile.writeLine("      include:");
        higgsConfFile.writeLine("        - include/leveldb");
        higgsConfFile.writeLine("    target any:");
        higgsConfFile.writeLine("      build:");
        higgsConfFile.writeLine("        - mkdir -p build");
        higgsConfFile.writeLine("        - cd build");
        higgsConfFile.writeLine("        - cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=1 ..");
        higgsConfFile.writeLine("        - make leveldb");
        higgsConfFile.writeLine("      libs:");
        higgsConfFile.writeLine("        - build/libleveldb.${LIB_EXT}.1");
        higgsConfFile.writeLine("      include:");
        higgsConfFile.writeLine("        - include/leveldb");
        higgsConfFile.writeLine("  - name: testdep");
        higgsConfFile.writeLine("    source: git");
        higgsConfFile.writeLine("    url:  https://github.com/bitboson-deps/TestDependency.git");
        higgsConfFile.writeLine("    rev: 6203c62577ae90f09fdaaaa6a953823822b3996d");
        higgsConfFile.writeLine("    type: higgs-boson");

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
    std::string libraryHash = "0a72e8e68e1923b7c817c56f9b3cfef6b42732f768874ab6f45a2077a0b2b32f";
    REQUIRE (ExecShell::exec("sha256sum " + libraryPath) == (libraryHash + "  " + libraryPath + "\n"));

    // Build the main project and validate the output
    REQUIRE (higgs.buildProject("default"));
    std::string exeOutputPath = "/tmp/higgs-boson/output/default/bin/TestProj";
    std::string exeOutputHash = "62452abb6aae4799da27c9e330af658d3ba4dc5e127ad048d780c5e7dc9671b8";
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
    std::string libraryHash = "0a72e8e68e1923b7c817c56f9b3cfef6b42732f768874ab6f45a2077a0b2b32f";
    REQUIRE (ExecShell::exec("sha256sum " + libraryPath) == (libraryHash + "  " + libraryPath + "\n"));

    // Build the main project and validate the output
    REQUIRE (higgs.buildProject("default"));
    std::string libOutputPath = "/tmp/higgs-boson/output/default/lib/libTestProj.so";
    std::string libOutputHash = "e7b2b619aea3c03f2cbf9357d88c431f7213bcb42680f16d7b28d726477296b1";
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
    std::string libraryHash = "0a72e8e68e1923b7c817c56f9b3cfef6b42732f768874ab6f45a2077a0b2b32f";
    REQUIRE (ExecShell::exec("sha256sum " + libraryPath) == (libraryHash + "  " + libraryPath + "\n"));
    libraryPath = "/tmp/higgs-boson/config/external/raw/testdep/output/default/lib/libTestProj.so";
    libraryHash = "8dca77b067ef73ea30d4904b4602b3a22f893da3d76c1e5672160a6744ebb73b";
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
    HiggsBoson::RunTypeSingleton::setDockerRunCommand("sh");

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
    std::string libraryHash = "0a72e8e68e1923b7c817c56f9b3cfef6b42732f768874ab6f45a2077a0b2b32f";
    REQUIRE (ExecShell::exec("sha256sum " + libraryPath) == (libraryHash + "  " + libraryPath + "\n"));

    // Build the main project and validate the output
    REQUIRE (higgs.testProject(CMakeSettings::TestType::TEST));
    std::string testExePath = "/tmp/higgs-boson/config/builds/test/bin/TestProj_test";
    std::string testExeHash = "dc9cc9fa6ef275ef72acf7e20ba7bed6bc8e5fceb45410474ce364f2e6ac87b8";
    REQUIRE (ExecShell::exec("sha256sum " + testExePath) == (testExeHash + "  " + testExePath + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/config").c_str()) == 0);
}

#endif //HIGGS_BOSON_HIGGS_BOSON_TEST_HPP
