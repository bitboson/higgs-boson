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

#include <algorithm>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/Dependency.h>

using namespace BitBoson;

/**
 * Constructor used to setup the dependency with the given name and targets
 *
 * @param dir String representing the path to the directory for the project
 * @param name String representing the unique name of the dependency
 * @param targets Vector of Strings representing the available targets
 */
Dependency::Dependency(const std::string& dir, const std::string& name,
        const std::vector<std::string>& targets)
{

    // Setup the member variables accordingly
    _dir = dir;
    _name = name;
    for (const auto& target : targets)
        _targets.push_back(target);
}

/**
 * Function used to get the name of the dependency
 *
 * @return String representing the dependency's name
 */
std::string Dependency::getName()
{

    // Simply return the corresponding member variable
    return _name;
}

/**
 * Function used to get the library directory for the target
 *
 * @param target String representing the target to get the directory for
 * @return String representing the library directory for the given target
 */
std::string Dependency::getLibraryDir(const std::string& target)
{

    // Deduce the target-based library-directory for the current dependency
    return getDir() + "/higgs-boson_" + target + "_libraries";
}

/**
 * Function used to get the header directory for the target
 *
 * @param target String representing the target to get the directory for
 * @return String representing the header directory for the given target
 */
std::string Dependency::getHeaderDir(const std::string& target)
{

    // Deduce the target-based header-directory for the current dependency
    return getDir() + "/higgs-boson_" + target + "_headers";
}

/**
 * Internal function used to get the directory of the dependency
 *
 * @return String representing the dependency's directory
 */
std::string Dependency::getDir()
{

    // Simply return the corresponding member variable
    return _dir;
}

/**
 * Internal function used to get the targets for the dependency
 *
 * @return Vector of Strings representing the dependency's targets
 */
std::vector<std::string> Dependency::getTargets()
{

    // Simply return the corresponding member variable
    return _targets;
}
