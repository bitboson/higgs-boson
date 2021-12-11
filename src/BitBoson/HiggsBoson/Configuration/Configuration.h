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

#ifndef HIGGS_BOSON_CONFIGURATION_H
#define HIGGS_BOSON_CONFIGURATION_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <yaml/Yaml.hpp>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/Dependency.h>
#include <BitBoson/HiggsBoson/Configuration/Settings/PeruSettings.h>
#include <BitBoson/HiggsBoson/Configuration/Settings/CMakeSettings.h>
#include <BitBoson/HiggsBoson/Configuration/Settings/ProjectSettings.h>

namespace BitBoson
{

    class Configuration
    {

        // Private member variables
        private:
            std::vector<std::string> _configuredTargets;
            std::shared_ptr<ProjectSettings> _projectSettings;
            std::shared_ptr<CMakeSettings> _cMakeSettings;
            std::shared_ptr<PeruSettings> _peruSettings;
            std::vector<std::shared_ptr<Dependency>> _dependencies;
            std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> _outputLibsMap;
            std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> _outputHeadersMap;

        // Public member functions
        public:

            /**
             * Constructor used to setup the configuration object with the specified file
             *
             * @param projectDir String representing the project directory for finding files
             * @param filePath String representing the path to the YAML configuration file
             * @param tmpDir String representing the temp/cache file-path for managing files
             */
            Configuration(const std::string& projectDir, const std::string& filePath,
                    const std::string& tmpDir);

            /**
             * Function used to get the project-wide configured targets
             *
             * @return Vector of Strings representing the configured targets
             */
            std::vector<std::string> getConfiguredTargets();

            /**
             * Function used to get the project settings for the configuration
             *
             * @return ProjectSettings for the configuration
             */
            std::shared_ptr<ProjectSettings> getProjectSettings();

            /**
             * Function used to get the Peru settings for the configuration
             *
             * @return PeruSettings for the configuration
             */
            std::shared_ptr<PeruSettings> getPeruSettings();

            /**
             * Function used to get the CMake settings for the configuration
             *
             * @return CMakeSettings for the configuration
             */
            std::shared_ptr<CMakeSettings> getCMakeSettings();

            /**
             * Function used to get all of the dependencies for the configuration
             *
             * @return Vector of Dependencies for the configuration
             */
            std::vector<std::shared_ptr<Dependency>> getDependencies();

            /**
             * Function used to get the specified output libraries for the provided dependency
             *
             * @param dependency Dependencies representing the dependency to use
             * @param target String representing the target to get the information for
             * @return Vector of Strings representing the output libraries for the target
             */
            std::vector<std::string> getLibrariesOutputForDependency(
                    std::shared_ptr<Dependency> dependency, const std::string& target);

            /**
             * Function used to get the specified output headers for the provided dependency
             *
             * @param dependency Dependencies representing the dependency to use
             * @param target String representing the target to get the information for
             * @return Vector of Strings representing the output headers for the target
             */
            std::vector<std::string> getHeadersOutputForDependency(
                    std::shared_ptr<Dependency> dependency, const std::string& target);

            /**
             * Function used to get the library extension for the given target
             *
             * @param target String representing the target to get the extension for
             * @return String representing the lirbary extension for the target
             */
            std::string getLibExtensionForTarget(const std::string& target);

            /**
             * Destructor used to cleanup the instance
             */
            virtual ~Configuration() = default;

        // Private member functions
        private:

            /**
             * Internal function used to replace any build-variables for YAML file
             *
             * @param target String representing the target to reference in the replacement
             * @param textToUse String representing the text to do the replacement in
             * @return String representing the input text with the replaced build-variables
             */
            std::string substituteBuildVariables(const std::string& target,
                    const std::string& textToUse);

            /**
             * Internal function used to get the configuration for a target
             * NOTE: This will attempt to adapt target-triples to OS if needed
             *
             * @param yamlConfig YAML Node representing the configuration to source from
             * @param target String representing the target to get the configuration for
             * @return YAML Node representing the desired target-specific configuration
             */
            Yaml::Node getConfigurationForTarget(Yaml::Node& yamlConfig, const std::string& target);
    };
}

#endif //HIGGS_BOSON_CONFIGURATION_H
