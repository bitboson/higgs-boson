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

#ifndef HIGGS_BOSON_PROJECT_SETTINGS_H
#define HIGGS_BOSON_PROJECT_SETTINGS_H

#include <string>

namespace BitBoson
{

    class ProjectSettings
    {

        // Public enumerations
        public:
            enum ProjectType
            {
                TYPE_EXE,
                TYPE_LIB
            };

        // Private member variables
        private:
            std::string _name;
            ProjectType _type;
            std::string _version;
            std::string _source;
            std::string _test;
            std::string _main;

        // Public member functions
        public:

            /**
             * Constructor used to setup the project settings object instance
             *
             * @param name String representing the project name
             * @param type ProjectType representing the project type
             * @param version String representing the project version
             * @param source String representing the project source
             * @param test String representing the project test
             * @param main String representing the project main
             */
            ProjectSettings(const std::string& name, ProjectType type,
                    const std::string& version, const std::string& source,
                    const std::string& test, const std::string& main);

            /**
             * Function used to get the project name
             *
             * @return String representing the project name
             */
            std::string getProjectName();

            /**
             * Function used to get the project-type
             *
             * @return ProjectType representing the project type
             */
            ProjectType getProjectType();

            /**
             * Function used to get the project version
             *
             * @return String representing the project version
             */
            std::string getProjectVersion();

            /**
             * Function used to get the project source
             *
             * @return String representing the project source
             */
            std::string getProjectSource();

            /**
             * Function used to get the project test
             *
             * @return String representing the project test
             */
            std::string getProjectTest();

            /**
             * Function used to get the project main
             *
             * @return String representing the project main
             */
            std::string getProjectMain();

            /**
             * Destructor used to cleanup the instance
             */
            virtual ~ProjectSettings() = default;
    };
}

#endif //HIGGS_BOSON_PROJECT_SETTINGS_H
