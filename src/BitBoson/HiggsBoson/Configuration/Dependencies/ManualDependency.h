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

#ifndef HIGGS_BOSON_MANUAL_DEPENDENCY_H
#define HIGGS_BOSON_MANUAL_DEPENDENCY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/Dependency.h>

namespace BitBoson
{

    class ManualDependency : public Dependency
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
            ManualDependency(const std::string& dir, const std::string& name,
                    const std::vector<std::string>& targets);

            /**
             * Function used to set the build-steps for the given target
             *
             * @param target String representing the target to set the build steps for
             * @param buildSteps Vector of Strings representing the build-steps for
             *                   the target
             * @return Boolean indicating whether the operation was successful
             */
            bool setBuildSteps(const std::string& target,
                    const std::vector<std::string>& buildSteps);

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
             * Function used to cache/save atrifacts (libraries/headers) after a build
             *
             * @param target String representing the target to cache for
             * @param libPaths Vector of Strings representing the output libraries
             * @param headerDirs Vector of Strings representing the header directories
             * @param fullPathsGiven Boolean indicating whether all paths are full (absolute)
             * @return Boolean indicating whether the artifact cache was successful
             */
            bool postBuildArtifactCache(const std::string& target,
                    const std::vector<std::string>& libPaths,
                    const std::vector<std::string>& headerDirs,
                    bool fullPathsGiven=false);

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
            virtual ~ManualDependency() = default;
    };
}

#endif //HIGGS_BOSON_MANUAL_DEPENDENCY_H
