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

#ifndef HIGGS_BOSON_HIGGS_DEPENDENCY_H
#define HIGGS_BOSON_HIGGS_DEPENDENCY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/Dependency.h>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/ManualDependency.h>

namespace BitBoson
{

    class HiggsBosonDependency : public Dependency
    {

        // Private member variables
        private:
            std::string _confFile;
            std::string _projectOutput;
            std::string _projectSource;
            std::shared_ptr<ManualDependency> _internalDep;

        // Public member functions
        public:

            /**
             * Constructor used to setup the dependency with the given name and targets
             *
             * @param dir String representing the path to the directory for the project
             * @param name String representing the unique name of the dependency
             * @param name String representing the unique name of the dependency
             * @param higgsConfig String representing the Higgs-Boson YAML configuration
             */
            HiggsBosonDependency(const std::string& dir, const std::string& name,
                    const std::string& higgsConfig);

            /**
             * Function used to get the output libraries path for the dependency
             *
             * @return String representing the output path for libraries
             */
            std::string getOutputLibsPath();

            /**
             * Function used to get the output headers path for the dependency
             *
             * @return String representing the output path for headers
             */
            std::string getOutputHeadersPath();

            /**
             * Function used to get the available targets for the dependency
             *
             * @return Vector of Strings representing the available targets
             */
            std::vector<std::string> getAvailableTargets();

            /**
             * Overridden function used to compile the given target using the
             * configured build
             *
             * @param target String representing the target to attempt to build
             * @param libPaths Vector of Strings representing the output libraries
             * @param headerDirs Vector of Strings representing the header directories
             * @return Boolean indicating whether the compilation was successful
             */
            bool compileTarget(const std::string& target,
                const std::vector<std::string>& libPaths={},
                const std::vector<std::string>& headerDirs={}) override;

            /**
             * Overridden function used to get the paths for the output libraries for the
             * provided target
             *
             * @param target String representing the target to get the libraries for
             * @return Vector of Strings representing the library paths
             */
            std::vector<std::string> getLibraries(const std::string& target) override;

            /**
             * Destructor used to cleanup the instance
             */
            virtual ~HiggsBosonDependency() = default;

        // Private member functions
        private:

            /**
             * Internal function used to setup the underlying Higgs-Boson dependency from
             * the corresponding Higgs-Boson configuration (YAML) file
             *
             * @param dir String representing the path to the directory for the project
             * @param higgsConfig String representing the Higgs-Boson YAML configuration
             * @return Vector of Strings representing the available targets for the configuration
             */
            bool setupFromConfig(const std::string& dir, const std::string& higgsConfig);

            /**
             * Internal static function used to get the targets for the YAML configuration file
             *
             * @return Vector of Strings representing the available targets for the configuration
             */
            static std::vector<std::string> getTargetsFromConfig(const std::string& higgsConfig);
    };
}

#endif //HIGGS_BOSON_HIGGS_DEPENDENCY_H
