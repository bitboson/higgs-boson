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

#ifndef HIGGS_BOSON_HIGGS_BOSON_H
#define HIGGS_BOSON_HIGGS_BOSON_H

#include <string>
#include <memory>
#include <BitBoson/HiggsBoson/Configuration/Configuration.h>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/Dependency.h>

namespace BitBoson
{

    class HiggsBoson
    {

        // Public internal classes
        public:
            class RunTypeSingleton
            {

                // Private member variables
                private:
                    std::string _runCommand;

                // Public member functions
                public:

                    /**
                     * Destructor used to cleanup the Singleton Class
                     */
                    virtual ~RunTypeSingleton() = default;

                    /**
                     * Static function used to set the run-type command
                     * for the Singleton instance
                     *
                     * @param command String representing the command
                     */
                    static void setRunTypeCommand(const std::string& command)
                    {

                        // Simply set the run-type command accordingly
                        getInstance()._runCommand = command;
                    }

                    /**
                     * Static function used to get the run-type command
                     * for the Singleton instance
                     *
                     * @return String representing the command
                     */
                    static std::string getRunTypeCommand()
                    {

                        // Simply get and return the run-type command
                        return getInstance()._runCommand;
                    }

                // Private member functions
                private:

                    /**
                     * Internal constructor used to setup the
                     * Singleton Class instance
                     */
                    //___attribute__((no_sanitize("memory")))
                    RunTypeSingleton()
                    {

                        // Setup the default run command
                        _runCommand = "sh";
                    }

                    /**
                     * Internal static get-instance function to get the
                     * instance of the Singleton Class
                     *
                     * @return Singleton Class instance
                     */
                    //___attribute__((no_sanitize("memory")))
                    static RunTypeSingleton& getInstance()
                    {

                        // Setup the Singleton Instance
                        static RunTypeSingleton instance;

                        // Return the Singleton instance
                        return instance;
                    }
            };

        // Private member variables
        private:
            std::string _cacheDir;
            std::string _projectDir;
            std::shared_ptr<Configuration> _configuration;

        // Public member functions
        public:

            /**
             * Constructor used to setup the higgs-boson object with the specified file
             *
             * @param projectDir String representing the project directory for finding files
             * @param filePath String representing the path to the YAML configuration file
             * @param tmpDir String representing the temp/cache file-path for managing files
             */
            HiggsBoson(const std::string& projectDir, const std::string& filePath,
                    const std::string& tmpDir);

            /**
             * Function used to download the external dependencies for the project
             *
             * @return Boolean indicating whether the download was successful
             */
            bool download();

            /**
             * Function used to build the external dependencies for the project
             *
             * @param target String representing the target to build for
             * @return Boolean indicating whether the build was successful
             */
            bool buildDependencies(const std::string& target);

            /**
             * Function used to build the project itself for the given target
             *
             * @param target String representing the target to build for
             * @return Boolean indicating whether the build was successful
             */
            bool buildProject(const std::string& target);

            /**
             * Function used to test the project itself for the given test
             *
             * @param testType TestType representing the test to run
             * @param testFilter String representing the test filter to apply
             * @return Boolean indicating whether the test was successful
             */
            bool testProject(CMakeSettings::TestType testType, const std::string& testFilter="");

            /**
             * Destructor used to cleanup the instance
             */
            virtual ~HiggsBoson() = default;
    };
}

#endif //HIGGS_BOSON_HIGGS_BOSON_H
