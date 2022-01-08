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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <yaml/Yaml.hpp>
#include <BitBoson/HiggsBoson/HiggsBoson.h>
#include <BitBoson/HiggsBoson/Utils/Utils.h>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/HiggsBosonDependency.h>

using namespace BitBoson;

/**
 * Constructor used to setup the dependency with the given name and targets
 *
 * @param dir String representing the path to the directory for the project
 * @param name String representing the unique name of the dependency
 * @param higgsConfig String representing the Higgs-Boson YAML configuration
 */
HiggsBosonDependency::HiggsBosonDependency(const std::string& dir, const std::string& name,
        const std::string& higgsConfig) : Dependency(dir, name, getTargetsFromConfig(higgsConfig))
{

    // Initialize the internal ManualDependency object/reference
    _confFile = higgsConfig;
    _internalDep = std::make_shared<ManualDependency>(dir, name, getTargetsFromConfig(_confFile));

    // Setup all remaining configuration from the provided YAML file
    setupFromConfig(dir, higgsConfig);
}

/**
 * Function used to get the available targets for the dependency
 *
 * @return Vector of Strings representing the available targets
 */
std::vector<std::string> HiggsBosonDependency::getAvailableTargets()
{

    // Deduce and return the available targets from the configuration
    return getTargetsFromConfig(_confFile);
}

/**
 * Overridden function used to compile the given target using the
 * configured build
 *
 * @param target String representing the target to attempt to build
 * @param libPaths Vector of Strings representing the output libraries
 * @param headerDirs Vector of Strings representing the header directories
 * @return Boolean indicating whether the compilation was successful
 */
bool HiggsBosonDependency::compileTarget(const std::string& target,
        const std::vector<std::string>& libPaths,
        const std::vector<std::string>& headerDirs)
{

    // Create a return flag
    bool retFlag = false;

    // Simply run the internal ManualDependency's compile operation
    // NOTE: We also have to ensure this runs locally, not in the container
    // TODO - Determine if this is actually needed or not
    //auto currRunTypeCmd = HiggsBoson::RunTypeSingleton::getRunTypeCommand();
    //HiggsBoson::RunTypeSingleton::setRunTypeCommand("sh");
    retFlag = _internalDep->compileTarget(target);
    //HiggsBoson::RunTypeSingleton::setRunTypeCommand(currRunTypeCmd);

    // Only continue if the build was successful
    if (retFlag)
    {

        // Determine the output libraries to use for archiving/caching
        std::vector<std::string> cacheLibsVect;
        for (const auto& libPath : Utils::listFilesInDirectory(_projectOutput + "/" + target + "/deps"))
            cacheLibsVect.push_back(libPath);
        for (const auto& libPath : Utils::listFilesInDirectory(_projectOutput + "/" + target + "/lib"))
            cacheLibsVect.push_back(libPath);

        // Forcibly archive/cache the corresponding atrifacts
        retFlag = _internalDep->postBuildArtifactCache(target, cacheLibsVect,
                    {_projectSource + "/", _headersOutput + "/" + target + "/"}, true);
    }

    // Return the return flag
    return retFlag;
}

/**
 * Overridden function used to get the paths for the output libraries for the
 * provided target
 *
 * @param target String representing the target to get the libraries for
 * @return Vector of Strings representing the library paths
 */
std::vector<std::string> HiggsBosonDependency::getLibraries(const std::string& target)
{

    // Simply run the internal ManualDependency's getLibraries operation
    // and return the corresponding results
    return _internalDep->getLibraries(target);
}

/**
 * Internal function used to setup the underlying Higgs-Boson dependency from
 * the corresponding Higgs-Boson configuration (YAML) file
 *
 * @param dir String representing the path to the directory for the project
 * @param higgsConfig String representing the Higgs-Boson YAML configuration
 * @return Boolean indicating whether the setup process was successful
 */
bool HiggsBosonDependency::setupFromConfig(const std::string& dir, const std::string& higgsConfig)
{

    // Create a return flag
    bool retFlag = false;

    // Surround operation with a try-catch
    try
    {

        // Read-in the YAML file based on the file-path
        Yaml::Node root;
        Yaml::Parse(root, higgsConfig.c_str());

        // Read-in the YAML configuration for the project source for header
        // copies later in the output for the dependency
        // TODO - Eventually add configurable output directory
        _projectOutput = dir + "/output";
        _headersOutput = dir + "/.higgs-boson/includes";
        _projectSource = dir + "/" + root["project"]["source"].As<std::string>();

        // Setup the internal ManualDependency with the required build-steps
        // for each of the configured targets (including dependency downloads)
        for (std::string target : getTargetsFromConfig(higgsConfig))
            _internalDep->setBuildSteps(target,
                    {
                        "higgs-boson-internal download internal",
                        "higgs-boson-internal build-deps internal " + target,
                        "higgs-boson-internal build internal " + target,
                    });
    }

    // Catch and ignore all exceptions
    catch (...)
    {

        // Not Used...
    }

    // Return the return flag
    return retFlag;
}

/**
 * Internal static function used to get the targets for the YAML configuration file
 *
 * @return Vector of Strings representing the available targets for the configuration
 */
std::vector<std::string> HiggsBosonDependency::getTargetsFromConfig(const std::string& higgsConfig)
{

    // Create a return vector
    std::vector<std::string> returnVect;

    // Surround operation with a try-catch
    try
    {

        // Read-in the YAML file based on the file-path
        Yaml::Node root;
        Yaml::Parse(root, higgsConfig.c_str());

        // Read-in the various targets provided in the YAML configuration
        auto configuredTargetsYaml = root["project"]["targets"];
        if (configuredTargetsYaml.Size() > 0)
            for(auto configuredTargetsIter = configuredTargetsYaml.Begin();
                    configuredTargetsIter != configuredTargetsYaml.End(); configuredTargetsIter++)
                returnVect.push_back((*configuredTargetsIter).second.As<std::string>());
    }

    // Catch and ignore all exceptions
    catch (...)
    {

        // Not Used...
    }

    // Return the return vector
    return returnVect;
}
