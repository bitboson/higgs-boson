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
#include <thread>
#include <BitBoson/HiggsBoson/Utils/Utils.h>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
#include <BitBoson/HiggsBoson/Configuration/Configuration.h>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/Dependency.h>
#include <BitBoson/HiggsBoson/Configuration/Settings/DockerSyncSettings.h>

using namespace std::chrono_literals;

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
                    bool _isContainer;
                    std::string _initCmd;
                    std::string _runCommand;
                    std::string _containerName;
                    std::shared_ptr<DockerSyncSettings> _dockerSyncSettings;

                // Public member functions
                public:

                    /**
                     * Static function used to set the run-type command
                     * for the Singleton instance
                     *
                     * @param command String representing the command
                     */
                    static void setDockerRunCommand(const std::string& command, const std::string& containerName="")
                    {

                        // Simply set the run-type command accordingly
                        getInstance()._isContainer = false;
                        getInstance()._runCommand = command;
                        getInstance()._containerName = containerName;

                        // Determine if this command is for a container or not
                        if (!getInstance()._runCommand.empty() && (getInstance()._runCommand != "bash")
                                && (getInstance()._runCommand != "sh"))
                            getInstance()._isContainer = true;
                    }

                    /**
                     * Static function used to set the run-type init command
                     * for the Singleton instance
                     *
                     * @param initCommand String representing the initialization command
                     */
                    static void setDockerRunInitCommand(const std::string& initCommand)
                    {

                        // Simply set the run-type init command accordingly
                        getInstance()._initCmd = initCommand;
                    }

                    /**
                     * Function used to get whether we are running commands in a container
                     *
                     * @return Boolean indicating whether we are running commands in a container
                     */
                    static bool isRunningCommandsInContainer()
                    {

                        // Simply return if we are running commands in a container
                        return getInstance()._isContainer;
                    }

                    /**
                     * Function used to run the internally defined idle container
                     * NOTE: This makes use of the docker-run command configured
                     */
                    static void runIdleContainer()
                    {

                        // Only handle if the command is actually for a container
                        if (getInstance()._isContainer)
                        {

                            // Check if the container is already running and only
                            // start the container if it is not already running
                            bool containerIsRunning = false;
                            std::string dockerPsCmd = "docker ps --format \"{{.Names}}\" | grep higgsboson | grep "
                                    + getInstance()._containerName;
                            for (std::string containerName : Utils::splitStringByDelimiter(ExecShell::exec(dockerPsCmd), '\n'))
                                if (containerName == getInstance()._containerName)
                                    containerIsRunning = true;
                            if (!containerIsRunning)
                            {

                                // Start by executing the container run process
                                ExecShell::exec(getInstance()._runCommand, true);

                                // Wait until the container has actually started
                                std::this_thread::sleep_for(500ms);
                                for (int ii = 0; ii < 6; ii++)
                                    if (ExecShell::exec("docker exec -it " + getInstance()._containerName + " ls")
                                            .find("Error") == std::string::npos)
                                        break;
                                    else
                                        std::this_thread::sleep_for(10000ms);
                            }
                        }
                    }

                    /**
                     * Function used to stop the internally defined idle container
                     * NOTE: This makes use of the docker-run command configured
                     */
                    static void stopIdleContainer()
                    {

                        // Only handle if the command is actually for a container
                        if (getInstance()._isContainer)
                        {

                            // Simply execute the container stop process
                            ExecShell::exec("docker stop " + getInstance()._containerName);
                        }
                    }

                    /**
                     * Function used to wait for the creation/existence of the given path
                     *
                     * @param path String representing the path to wait for existence of
                     */
                    static void waitForFileOrDirectoryExistence(const std::string& path)
                    {

                        // Only handle if the command is actually for a container
                        if (getInstance()._isContainer)
                        {

                            // Attempt to wait until the given path exists
                            for (int ii = 0; ii < 20; ii++)
                                if (ExecShell::exec("docker exec -it " + getInstance()._containerName + " ls " + path)
                                        .find("No such file or directory") == std::string::npos)
                                    break;
                                else
                                    std::this_thread::sleep_for(100ms);
                        }
                    }

                    /**
                     * Static function used to execute the provided command in the
                     * Higgs-Boson builder container with a response
                     *
                     * @param command String representing the command to run
                     * @return String representing the response of the command
                     */
                    static std::string executeInContainerWithResponse(const std::string& command)
                    {

                        // Setup the command prefix differently if this is a container
                        std::string containerCmd = "";
                        if (getInstance()._isContainer)
                            containerCmd += "docker exec -it " + getInstance()._containerName + " ";

                        // Add in the init command if applicable
                        if (!containerCmd.empty())
                            containerCmd += (getInstance()._initCmd + " ");

                        // Simply run the provided command in the Higgs-Boson builder container
                        ExecShell::exec(containerCmd + "container-watch-dog -b");
                        return ExecShell::exec(containerCmd + command);
                    }

                    /**
                     * Static function used to execute the provided command in the
                     * Higgs-Boson builder container
                     *
                     * @param message String representing the message to print
                     * @param command String representing the command to run
                     * @return Boolean indicating whether the command was successful
                     */
                    static bool executeInContainer(const std::string& message, const std::string& command)
                    {

                        // Setup the command prefix differently if this is a container
                        std::string containerCmd = "";
                        if (getInstance()._isContainer)
                            containerCmd += "docker exec -it " + getInstance()._containerName + " ";

                        // Add in the init command if applicable
                        if (!containerCmd.empty())
                            containerCmd += (getInstance()._initCmd + " ");

                        // Simply run the provided command in the Higgs-Boson builder container
                        ExecShell::exec(containerCmd + "container-watch-dog -b");
                        return ExecShell::execWithResponse(message, containerCmd + command);
                    }

                    /**
                     * Static function used to execute the provided command in the
                     * Higgs-Boson builder container
                     *
                     * @param command String representing the command to run
                     * @return Boolean indicating whether the command was successful
                     */
                    static bool executeInContainer(const std::string& command)
                    {

                        // Setup the command prefix differently if this is a container
                        std::string containerCmd = "";
                        if (getInstance()._isContainer)
                            containerCmd += "docker exec -it " + getInstance()._containerName + " ";

                        // Add in the init command if applicable
                        if (!containerCmd.empty())
                            containerCmd += (getInstance()._initCmd + " ");

                        // Simply run the provided command in the Higgs-Boson builder container
                        ExecShell::exec(containerCmd + "container-watch-dog -b");
                        return ExecShell::execLive(containerCmd + command);
                    }

                    /**
                     * Constructor used to setup the docker-sync settings object instance
                     *
                     * @param projectDir String representing the project directory
                     * @param projectCacheDir String representing the project cache directory
                     * @param projectDirHash String representing the project directory hash
                     * @param globalCacheDir String representing the global cache directory
                     */
                    static std::shared_ptr<DockerSyncSettings> getDockerSync(const std::string& projectDir="",
                            const std::string& projectCacheDir="", const std::string& projectDirHash="",
                            const std::string& globalCacheDir="")
                    {

                        // Create the docker-sync settings if it does not already exist
                        // and if the provided parameters are not null
                        if ((getInstance()._dockerSyncSettings == nullptr) && !projectDir.empty()
                                && !projectCacheDir.empty() && !projectDirHash.empty())
                            getInstance()._dockerSyncSettings = std::make_shared<DockerSyncSettings>(
                                    projectDir, projectCacheDir, projectDirHash, globalCacheDir);

                        // Return the configured docker-sync settings
                        return getInstance()._dockerSyncSettings;
                    }

                    /**
                     * Destructor used to cleanup the Singleton Class
                     */
                    virtual ~RunTypeSingleton() = default;

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

                        // Setup the default docker-sync settings (none)
                        _dockerSyncSettings = nullptr;
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
