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

#ifndef HIGGS_BOSON_DEPENDENCY_H
#define HIGGS_BOSON_DEPENDENCY_H

#include <string>
#include <vector>

namespace BitBoson
{

    class Dependency
    {

        // Private member variables
        private:
            std::string _dir;
            std::string _name;
            std::vector<std::string> _targets;
            std::vector<std::string> _libPaths;
            std::vector<std::string> _headerDirs;

        // Public member functions
        public:

            /**
             * Constructor used to setup the dependency with the given name and targets
             *
             * @param dir String representing the path to the directory for the project
             * @param name String representing the unique name of the dependency
             * @param targets Vector of Strings representing the available targets
             */
            Dependency(const std::string& dir, const std::string& name,
                    const std::vector<std::string>& targets);

            /**
             * Function used to get the name of the dependency
             *
             * @return String representing the dependency's name
             */
            std::string getName();

            /**
             * Function used to get the library directory for the target
             *
             * @param target String representing the target to get the directory for
             * @return String representing the library directory for the given target
             */
            std::string getLibraryDir(const std::string& target);

            /**
             * Function used to get the header directory for the target
             *
             * @param target String representing the target to get the directory for
             * @return String representing the header directory for the given target
             */
            std::string getHeaderDir(const std::string& target);

            /**
             * Virtual function used to compile the given target using the configured build
             *
             * @param target String representing the target to attempt to build
             * @param libPaths Vector of Strings representing the output libraries
             * @param headerDirs Vector of Strings representing the header directories
             * @return Boolean indicating whether the compilation was successful
             */
            virtual bool compileTarget(const std::string& target,
                    const std::vector<std::string>& libPaths={},
                    const std::vector<std::string>& headerDirs={}) = 0;

            /**
             * Virtual function used to get the paths for the output libraries for the
             * provided target
             *
             * @param target String representing the target to get the libraries for
             * @return Vector of Strings representing the library paths
             */
            virtual std::vector<std::string> getLibraries(const std::string& target) = 0;

            /**
             * Destructor used to cleanup the instance
             */
            virtual ~Dependency() = default;

        // Protected member functions
        protected:

            /**
             * Internal function used to get the directory of the dependency
             *
             * @return String representing the dependency's directory
             */
            std::string getDir();

            /**
             * Internal function used to get the targets for the dependency
             *
             * @return Vector of Strings representing the dependency's targets
             */
            std::vector<std::string> getTargets();
    };
}

#endif //HIGGS_BOSON_DEPENDENCY_H
