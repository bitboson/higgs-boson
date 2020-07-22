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

#ifndef HIGGS_BOSON_CMAKE_SETTINGS_H
#define HIGGS_BOSON_CMAKE_SETTINGS_H

#include <string>
#include <vector>
#include <unordered_map>

namespace BitBoson
{

    class CMakeSettings
    {

        // Public enumerations
        public:
            enum TestType
            {
                TEST,
                COVERAGE,
                SANITIZE_ADDRESS,
                SANITIZE_BEHAVIOR,
                SANITIZE_THREAD,
                SANITIZE_LEAK
            };

        // Private member variables
        private:
            std::string _mainFile;
            std::string _cMakeFile;
            std::string _projectName;
            std::string _projectVersion;
            std::string _cMakeBuildDir;
            std::string _cMakeCacheDir;
            std::vector<std::string> _preBuildCommands;
            std::vector<std::string> _postBuildCommands;
            std::vector<std::string> _preTestCommands;
            std::vector<std::string> _postTestCommands;
            std::vector<std::string> _externalLibraries;
            std::vector<std::string> _externalIncludes;
            std::unordered_map<std::string, bool> _sourceFiles;
            std::unordered_map<std::string, bool> _headerFiles;
            std::unordered_map<std::string, bool> _testFiles;

        // Public member functions
        public:

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
            CMakeSettings(const std::string& projectName, const std::string& projectVersion,
                    const std::string& cMakeBuildDir, const std::string& cMakeCacheDir);

            /**
             * Function used to set a main source-file to the CMake configuration
             *
             * @param mainSource String representing the path to the main source file
             */
            void setMainSource(const std::string& mainSource);

            /**
             * Function used to add a source-file to the CMake configuration
             *
             * @param sourceFile String representing the path to the source file
             * @return Boolean indicating whether the operation was successful
             */
            bool addSourceFile(const std::string& sourceFile);

            /**
             * Function used to add a header-file to the CMake configuration
             *
             * @param headerFile String representing the path to the header file
             * @return Boolean indicating whether the operation was successful
             */
            bool addHeaderFile(const std::string& headerFile);

            /**
             * Function used to add a testing-file to the CMake configuration
             *
             * @param testingFile String representing the path to the testing file
             * @return Boolean indicating whether the operation was successful
             */
            bool addTestingFile(const std::string& testingFile);

            /**
             * Function used to add a pre-build command to the configuration
             *
             * @param preBuildCommand String representing the pre-build command
             */
            void addPreBuildCommand(const std::string& preBuildCommand);

            /**
             * Function used to add a post-build command to the configuration
             *
             * @param postBuildCommand String representing the post-build command
             */
            void addPostBuildCommand(const std::string& postBuildCommand);

            /**
             * Function used to add a pre-test command to the configuration
             *
             * @param preTestCommand String representing the pre-test command
             */
            void addPreTestCommand(const std::string& preTestCommand);

            /**
             * Function used to add a post-test command to the configuration
             *
             * @param postTestCommand String representing the post-test command
             */
            void addPostTestCommand(const std::string& postTestCommand);

            /**
             * Function used to add a shared library to the configuration
             *
             * @param libraryPath String representing the library path
             */
            void addLibrary(const std::string& libraryPath);

            /**
             * Function used to add an includes path to the configuration
             *
             * @param includesPath String representing the includes path
             */
            void addIncludeDir(const std::string& includesPath);

            /**
             * Function used to compile/build the CMake project for the target
             *
             * @param target String representing the target to compile for
             * @return Boolean indicating whether the operation was successful
             */
            bool buildCMakeProject(const std::string& target);

            /**
             * Function used to compile/build the CMake project for the target
             *
             * @param testType TestType representing CMake test to be run
             * @param testFilter String representing the test filter to apply
             * @return Boolean indicating whether the operation was successful
             */
            bool testCMakeProject(TestType testType, const std::string& testFilter="");

            /**
             * Destructor used to cleanup the instance
             */
            virtual ~CMakeSettings() = default;

        // Private member functions
        private:

            /**
             * Internal function used to write the CMake file to the pre-defined location
             *
             * @param isTesting Boolean indicating whether to write-out a test or not
             * @return Boolean indicating if the operation was successful or not
             */
            bool writeCMakeFile(bool isTesting);
    };
}

#endif //HIGGS_BOSON_CMAKE_SETTINGS_H
