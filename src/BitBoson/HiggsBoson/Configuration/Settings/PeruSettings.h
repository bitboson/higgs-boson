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

#ifndef HIGGS_BOSON_PERU_SETTINGS_H
#define HIGGS_BOSON_PERU_SETTINGS_H

#include <string>
#include <unordered_map>

namespace BitBoson
{

    class PeruSettings
    {

        // Public enumerations
        public:
            enum DependencyType
            {
                TYPE_GIT,
                TYPE_CURL
            };

        // Private member variables
        private:
            std::string _peruFile;
            std::string _peruSyncDir;
            std::unordered_map<std::string,
                    std::unordered_map<std::string, std::string>> _dependencies;

        // Public member functions
        public:

            /**
             * Constructor used to setup the Peru settings object instance
             *
             * @param peruFile String representing the path to the Peru file
             *                 the instance is going to manage/overwrite
             * @param peruSyncDir String representing the path to perform Peru
             *                    sync operations in/to
             */
            PeruSettings(const std::string& peruFile, const std::string& peruSyncDir);

            /**
             * Function used to add a dependency to the Peru settings object
             *
             * @param name String representing the name of the dependency
             * @param type DependencyType representing the type of the dependency
             * @return Boolean indicating whether the dependency was added or not
             */
            bool addDependency(const std::string& name, DependencyType type);

            /**
             * Function used to add a property to a dependency in the Peru
             * settings object
             *
             * @param name String representing the name of the dependency
             * @param property String representing the new property to add
             * @param value String representing the value of the new property
             * @return Boolean indicating whether the property was added or not
             */
            bool addDependencyProperty(const std::string& name,
                    const std::string& property, const std::string& value);

            /**
             * Function used to perform a Peru-sync operation on the configured
             * location/directory based on the supplied peru sync directory
             *
             * @return Boolean indcating whether the Peru sync was performed
             */
            bool peruSync();

            /**
             * Destructor used to cleanup the instance
             */
            virtual ~PeruSettings() = default;

        // Private member functions
        private:

            /**
             * Internal function used to write the internal state to the configured
             * Peru file on disk
             *
             * @return Boolean indcating whether the Peru file was written
             */
            bool writePeruFile();
    };
}

#endif //HIGGS_BOSON_PERU_SETTINGS_H
