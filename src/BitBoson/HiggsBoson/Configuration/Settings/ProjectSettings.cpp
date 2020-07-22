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

#include <BitBoson/HiggsBoson/Configuration/Settings/ProjectSettings.h>

using namespace BitBoson;

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
ProjectSettings::ProjectSettings(const std::string& name,
        ProjectType type, const std::string& version,
        const std::string& source, const std::string& test,
        const std::string& main)
{

    // Setup the local member variable values
    _name = name;
    _type = type;
    _version = version;
    _source = source;
    _test = test;
    _main = main;
}

/**
 * Function used to get the project name
 *
 * @return String representing the project name
 */
std::string ProjectSettings::getProjectName()
{

    // Simply return the corresponding member variable
    return _name;
}

/**
 * Function used to get the project-type
 *
 * @return ProjectType representing the project type
 */
ProjectSettings::ProjectType ProjectSettings::getProjectType()
{

    // Simply return the corresponding member variable
    return _type;
}

/**
 * Function used to get the project version
 *
 * @return String representing the project version
 */
std::string ProjectSettings::getProjectVersion()
{

    // Simply return the corresponding member variable
    return _version;
}

/**
 * Function used to get the project source
 *
 * @return String representing the project source
 */
std::string ProjectSettings::getProjectSource()
{

    // Simply return the corresponding member variable
    return _source;
}

/**
 * Function used to get the project test
 *
 * @return String representing the project test
 */
std::string ProjectSettings::getProjectTest()
{

    // Simply return the corresponding member variable
    return _test;
}

/**
 * Function used to get the project main
 *
 * @return String representing the project main
 */
std::string ProjectSettings::getProjectMain()
{

    // Simply return the corresponding member variable
    return _main;
}
