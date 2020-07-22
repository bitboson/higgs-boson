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

#ifndef HIGGS_BOSON_DEPENDENCY_TEST_HPP
#define HIGGS_BOSON_DEPENDENCY_TEST_HPP

#include <catch.hpp>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory>
#include <string>
#include <fstream>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/Dependency.h>

using namespace BitBoson;

class DummyDependencyWrapper : public Dependency
{

    // Public member functions
    public:

        /**
         * Constructor used to setup the dependency with the given name and targets
         *
         * @param dir String representing the path to the directory for the project
         * @param name String representing the unique name of the dependency
         * @param targets Vector of Strings representing the available targets
         */
        DummyDependencyWrapper(const std::string& dir, const std::string& name,
            const std::vector<std::string>& targets) : Dependency(dir, name, targets)
        {

            // Unused...
        }

        /**
         * Overridden function used to compile the given target using the configured build
         *
         * @param target String representing the target to attempt to build
         * @param libPaths Vector of Strings representing the output libraries
         * @param headerDirs Vector of Strings representing the header directories
         * @return Boolean indicating whether the compilation was successful
         */
        bool compileTarget(const std::string& target,
                const std::vector<std::string>& libPaths={},
                const std::vector<std::string>& headerDirs={}) override
        {

            // Simply return true
            return true;
        }

        /**
         * Overridden function used to get the paths for the output libraries for the
         * provided target
         *
         * @param target String representing the target to get the libraries for
         * @return Vector of Strings representing the library paths
         */
        std::vector<std::string> getLibraries(const std::string& target) override
        {

            // Return an empty list
            return {};
        }

        /**
         * Wrapper function used to get the name of the dependency
         *
         * @return String representing the dependency's name
         */
        std::string getNameWrapper()
        {

            // Simply call the correspnding internal function
            return getName();
        }

        /**
         * Wrapper function used to get the directory of the dependency
         *
         * @return String representing the dependency's directory
         */
        std::string getDirWrapper()
        {

            // Simply call the correspnding internal function
            return getDir();
        }

        /**
         * Wrapper function used to get the library directory for the target
         *
         * @param target String representing the target to get the directory for
         * @return String representing the library directory for the given target
         */
        std::string getLibraryDirWrapper(const std::string& target)
        {

            // Simply call the correspnding internal function
            return getLibraryDir(target);
        }

        /**
         * Wrapper function used to get the header directory for the target
         *
         * @param target String representing the target to get the directory for
         * @return String representing the header directory for the given target
         */
        std::string getHeaderDirWrapper(const std::string& target)
        {

            // Simply call the correspnding internal function
            return getHeaderDir(target);
        }

        /**
         * Wrapper function used to get the targets for the dependency
         *
         * @return Vector of Strings representing the dependency's targets
         */
        std::vector<std::string> getTargetsWrapper()
        {

            // Simply call the correspnding internal function
            return getTargets();
        }

        /**
         * Destructor used to cleanup the instance
         */
        virtual ~DummyDependencyWrapper() = default;
};

TEST_CASE ("Member Variables Dependency Test", "[DependencyTest]")
{

    // Create a dependency for testing
    auto dep = DummyDependencyWrapper("/tmp/higgs-boson/dep1", "test-dep1", {"linux", "windows"});

    // Validate that the member variables were set accordingly
    REQUIRE (dep.getNameWrapper() == "test-dep1");
    REQUIRE (dep.getDirWrapper() == "/tmp/higgs-boson/dep1");
    auto targetsVector = dep.getTargetsWrapper();
    REQUIRE (targetsVector.size() == 2);
    REQUIRE (targetsVector[0] == "linux");
    REQUIRE (targetsVector[1] == "windows");

    // Verify that the compile target function returns true
    REQUIRE (dep.compileTarget("linux"));
    REQUIRE (dep.getLibraries("linux").empty());

    // Verify that the library-directory output works
    REQUIRE (dep.getLibraryDirWrapper("linux") == "/tmp/higgs-boson/dep1/higgs-boson_linux_libraries");
    REQUIRE (dep.getLibraryDirWrapper("windows") == "/tmp/higgs-boson/dep1/higgs-boson_windows_libraries");

    // Verify that the header-directory output works
    REQUIRE (dep.getHeaderDirWrapper("linux") == "/tmp/higgs-boson/dep1/higgs-boson_linux_headers");
    REQUIRE (dep.getHeaderDirWrapper("windows") == "/tmp/higgs-boson/dep1/higgs-boson_windows_headers");
}

#endif //HIGGS_BOSON_DEPENDENCY_TEST_HPP
