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

#include <map>
#include <string>
#include <BitBoson/HiggsBoson/Utils/Utils.h>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
#include <BitBoson/HiggsBoson/Utils/Constants.h>
#include <BitBoson/HiggsBoson/Configuration/Configuration.h>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/ManualDependency.h>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/HiggsBosonDependency.h>

using namespace BitBoson;

/**
 * Constructor used to setup the configuration object with the specified file
 *
 * @param projectDir String representing the project directory for finding files
 * @param filePath String representing the path to the YAML configuration file
 * @param tmpDir String representing the temp/cache file-path for managing files
 */
Configuration::Configuration(const std::string& projectDir, const std::string& filePath,
        const std::string& tmpDir)
{

    // Read-in the YAML file based on the file-path
    Yaml::Node root;
    Yaml::Parse(root, filePath.c_str());

    // Ensure the temporary directory exists
    ExecShell::exec("mkdir -p " + tmpDir);

    // Read-in the YAML configuration for the Project Settings
    auto projectName = root["project"]["name"].As<std::string>();
    auto projectType = root["project"]["type"].As<std::string>();
    auto projectVersion = root["project"]["version"].As<std::string>();
    auto projectSource = root["project"]["source"].As<std::string>();
    auto projectTest = root["project"]["test"].As<std::string>();
    auto projectMain = root["project"]["main"].As<std::string>();

    // Read-in the various targets provided in the Project Settings
    auto configuredTargetsYaml = root["project"]["targets"];
    if (configuredTargetsYaml.Size() > 0)
        for(auto configuredTargetsIter = configuredTargetsYaml.Begin();
                configuredTargetsIter != configuredTargetsYaml.End(); configuredTargetsIter++)
            _configuredTargets.push_back((*configuredTargetsIter).second.As<std::string>());
    if (std::find(_configuredTargets.begin(), _configuredTargets.end(), "default") == _configuredTargets.end())
        _configuredTargets.push_back("default");

    // Create and setup the project settings object
    ProjectSettings::ProjectType projectTypeEnum = ProjectSettings::ProjectType::TYPE_LIB;
    if (projectType == "exe")
        projectTypeEnum = ProjectSettings::ProjectType::TYPE_EXE;
    _projectSettings = std::make_shared<ProjectSettings>(projectName, projectTypeEnum,
            projectVersion, projectSource, projectTest, projectMain);

    // Initialize the Peru settings object
    std::string peruFile = tmpDir + "/peru.yaml";
    std::string peruDir = tmpDir + "/external/raw/";
    ExecShell::exec("mkdir -p " + peruDir);
    _peruSettings = std::make_shared<PeruSettings>(peruFile, peruDir);

    // Read-in the YAML configuration for the Dependencies
    auto depsListYaml = root["dependencies"];
    if (depsListYaml.Size() > 0)
    {

        // Iterate over each individual depdenency and parse it
        for(auto depsIter = depsListYaml.Begin(); depsIter != depsListYaml.End(); depsIter++)
        {

            // Extract the current node information and only continue if it
            // is in fact a dependency node
            auto& depsYaml = (*depsIter).second;
            auto targetType = depsYaml["type"].As<std::string>();
            auto depName = depsYaml["name"].As<std::string>();
            auto depSource = depsYaml["source"].As<std::string>();
            if ((!depName.empty()) && (!depSource.empty()))
            {

                // Process "git" dependencies
                if (depSource == "git")
                {

                    // Add the dependency information to the Peru settings
                    // TODO - Handle missing information
                    _peruSettings->addDependency(depName, PeruSettings::DependencyType::TYPE_GIT);
                    _peruSettings->addDependencyProperty(depName, "url", depsYaml["url"].As<std::string>());
                    _peruSettings->addDependencyProperty(depName, "rev", depsYaml["rev"].As<std::string>());
                }

                // Process "curl" dependencies
                if (depSource == "curl")
                {

                    // Add the dependency information to the Peru settings
                    // TODO - Handle missing information
                    _peruSettings->addDependency(depName, PeruSettings::DependencyType::TYPE_CURL);
                    _peruSettings->addDependencyProperty(depName, "url", depsYaml["url"].As<std::string>());
                    _peruSettings->addDependencyProperty(depName, "unpack", depsYaml["unpack"].As<std::string>());
                }

                // Create a new dependency object to add to the list
                std::shared_ptr<Dependency> dependency;

                // If the dependency type is manual, set it up accordingly
                // TODO - Make more extensible/generic based on type
                if (targetType == "manual")
                {

                    // Add-in the targets for the individual dependency
                    for(const auto& target : getConfiguredTargets())
                    {

                        // Filter-out the "any" target
                        if (target != "any")
                        {

                            // Create the actual manual dependency
                            std::string depDir = tmpDir + "/external/raw/" + depName;
                            ExecShell::exec("mkdir -p " + depDir);
                            auto dependencyRaw = std::make_shared<ManualDependency>(depDir, depName, getConfiguredTargets());
                            dependency = dependencyRaw;

                            // Setup the build-steps, starting with the deps information
                            std::vector<std::string> buildSteps;
                            buildSteps.push_back("HIGGS_BOSON_DEPS_DIR=" + tmpDir + "/external/raw");

                            // If this is the default target, define the compiler information
                            if (target == "default")
                            {

                                // Define the compiler information for the default target
                                buildSteps.push_back("CC=/usr/bin/clang");
                                buildSteps.push_back("CXX=/usr/bin/clang++");
                            }

                            // Iterate over the build-steps and collect them in a vector
                            auto buildStepsYaml = getConfigurationForTarget(depsYaml, target)["build"];
                            if (buildStepsYaml.Size() > 0)
                                for(auto stepIter = buildStepsYaml.Begin(); stepIter != buildStepsYaml.End(); stepIter++)
                                    buildSteps.push_back(
                                            substituteBuildVariables(target, (*stepIter).second.As<std::string>()));

                            // If there were no targets matching the provided one,
                            // then we can try to search for and add an any-target
                            if (buildStepsYaml.Size() == 0)
                            {

                                // Attempt to read-in the any-target value
                                buildStepsYaml = depsYaml["target any"]["build"];
                                if (buildStepsYaml.Size() > 0)
                                    for(auto stepIter = buildStepsYaml.Begin(); stepIter != buildStepsYaml.End(); stepIter++)
                                        buildSteps.push_back(
                                                substituteBuildVariables(target, (*stepIter).second.As<std::string>()));
                            }

                            // Add the build steps to the individual target
                            dependencyRaw->setBuildSteps(target, buildSteps);

                            // Save the corresponding output directory information for both
                            // libraries and header files for later compilation use
                            auto libOutputPathsYaml = getConfigurationForTarget(depsYaml, target)["libs"];
                            if (libOutputPathsYaml.Size() > 0)
                                for(auto libPathIter = libOutputPathsYaml.Begin();
                                        libPathIter != libOutputPathsYaml.End(); libPathIter++)
                                    _outputLibsMap[depName][target].push_back(
                                            substituteBuildVariables(target, (*libPathIter).second.As<std::string>()));
                            auto headerOutputPathsYaml = getConfigurationForTarget(depsYaml, target)["include"];
                            if (headerOutputPathsYaml.Size() > 0)
                                for(auto headerPathIter = headerOutputPathsYaml.Begin();
                                        headerPathIter != headerOutputPathsYaml.End(); headerPathIter++)
                                    _outputHeadersMap[depName][target].push_back(
                                            substituteBuildVariables(target, (*headerPathIter).second.As<std::string>()));

                            // If we failed (again) to get target-specific libraries
                            // and header files, then use the any-target once again
                            if ((libOutputPathsYaml.Size() == 0) && (headerOutputPathsYaml.Size() == 0))
                            {

                                // Get the any-target library files
                                libOutputPathsYaml = depsYaml["target any"]["libs"];
                                if (libOutputPathsYaml.Size() > 0)
                                    for(auto libPathIter = libOutputPathsYaml.Begin();
                                            libPathIter != libOutputPathsYaml.End(); libPathIter++)
                                        _outputLibsMap[depName][target].push_back(
                                                substituteBuildVariables(target, (*libPathIter).second.As<std::string>()));

                                // Get the any-target header files
                                headerOutputPathsYaml = depsYaml["target any"]["include"];
                                if (headerOutputPathsYaml.Size() > 0)
                                    for(auto headerPathIter = headerOutputPathsYaml.Begin();
                                            headerPathIter != headerOutputPathsYaml.End(); headerPathIter++)
                                        _outputHeadersMap[depName][target].push_back(
                                                substituteBuildVariables(target, (*headerPathIter).second.As<std::string>()));
                            }

                            // If no header files were found for the target, add the default ones
                            if (_outputHeadersMap[depName][target].empty())
                                _outputHeadersMap[depName][target].push_back("");
                        }
                    }
                }

                // If the dependency type is higgs-boson, set it up accordingly
                if (targetType == "higgs-boson")
                {

                    // Deduce the Higgs-Boson YAML configuration file information
                    std::string depDir = tmpDir + "/external/raw/" + depName;
                    auto higgsConfYaml = depsYaml["conf"].As<std::string>();
                    if (higgsConfYaml.empty())
                        higgsConfYaml = "higgs-boson.yaml";
                    higgsConfYaml = depDir + "/" + higgsConfYaml;

                    // Create the actual higgs-boson dependency
                    ExecShell::exec("mkdir -p " + depDir);
                    auto dependencyRaw = std::make_shared<HiggsBosonDependency>(depDir, depName, higgsConfYaml);
                    dependency = dependencyRaw;

                    // Setup the output libraries/headers as place-holder (this value will be auto-generated later)
                    for (const auto& target : dependencyRaw->getAvailableTargets())
                    {
                        _outputLibsMap[dependency->getName()][target].push_back("HIGGS_BOSON_PLACEHOLDER_VALUE");
                        _outputHeadersMap[dependency->getName()][target].push_back("HIGGS_BOSON_PLACEHOLDER_VALUE");
                    }
                }

                // Add-in the dependency (if available) in to the list
                if (dependency != nullptr)
                    _dependencies.push_back(dependency);
            }
        }
    }

    // Setup the CMakeLists.txt file for the project
    _cMakeSettings = std::make_shared<CMakeSettings>(projectName, projectVersion, projectDir, tmpDir);

    // Read-in the pre-test commands for the project configuration
    auto buildCommandsYaml = root["commands"]["test"]["pre"];
    if (buildCommandsYaml.Size() > 0)
        for(auto cmdIter = buildCommandsYaml.Begin(); cmdIter != buildCommandsYaml.End(); cmdIter++)
            _cMakeSettings->addPreTestCommand((*cmdIter).second.As<std::string>());

    // Read-in the post-test commands for the project configuration
    buildCommandsYaml = root["commands"]["test"]["post"];
    if (buildCommandsYaml.Size() > 0)
        for(auto cmdIter = buildCommandsYaml.Begin(); cmdIter != buildCommandsYaml.End(); cmdIter++)
            _cMakeSettings->addPostTestCommand((*cmdIter).second.As<std::string>());

    // Read-in the pre-build commands for the project configuration
    buildCommandsYaml = root["commands"]["build"]["pre"];
    if (buildCommandsYaml.Size() > 0)
        for(auto cmdIter = buildCommandsYaml.Begin(); cmdIter != buildCommandsYaml.End(); cmdIter++)
            _cMakeSettings->addPreBuildCommand((*cmdIter).second.As<std::string>());

    // Read-in the post-build commands for the project configuration
    buildCommandsYaml = root["commands"]["build"]["post"];
    if (buildCommandsYaml.Size() > 0)
        for(auto cmdIter = buildCommandsYaml.Begin(); cmdIter != buildCommandsYaml.End(); cmdIter++)
            _cMakeSettings->addPostBuildCommand((*cmdIter).second.As<std::string>());

    // Add-in the C++ source files for the project
    // TODO - Normpath required here
    for (const auto& sourceFile : Utils::listFilesInDirectory(projectDir + "/" + projectSource))
        if ((Utils::splitStringByDelimiter(sourceFile, '.').back() == "cpp")
                || (Utils::splitStringByDelimiter(sourceFile, '.').back() == "c")
                || (Utils::splitStringByDelimiter(sourceFile, '.').back() == "cxx"))
            if (projectMain.empty() || (sourceFile != (projectDir + "/" + projectMain)))
                _cMakeSettings->addSourceFile(sourceFile);
    if (!projectMain.empty())
        _cMakeSettings->setMainSource(projectDir + "/" + projectMain);

    // Add-in the C++ header files for the project
    for (const auto& headerFile : Utils::listFilesInDirectory(projectDir + "/" + projectSource))
        if ((Utils::splitStringByDelimiter(headerFile, '.').back() == "h")
                || (Utils::splitStringByDelimiter(headerFile, '.').back() == "hxx")
                || (Utils::splitStringByDelimiter(headerFile, '.').back() == "hpp"))
            _cMakeSettings->addHeaderFile(headerFile);

    // Add-in the C++ testing files for the project
    for (const auto& testingFile : Utils::listFilesInDirectory(projectDir + "/" + projectTest))
        if ((Utils::splitStringByDelimiter(testingFile, '.').back() == "h")
                || (Utils::splitStringByDelimiter(testingFile, '.').back() == "hxx")
                || (Utils::splitStringByDelimiter(testingFile, '.').back() == "hpp"))
            _cMakeSettings->addTestingFile(testingFile);
}

/**
 * Function used to get the project-wide configured targets
 *
 * @return Vector of Strings representing the configured targets
 */
std::vector<std::string> Configuration::getConfiguredTargets()
{

    // Return the corresponding member variable
    return _configuredTargets;
}

/**
 * Function used to get the project settings for the configuration
 *
 * @return ProjectSettings for the configuration
 */
std::shared_ptr<ProjectSettings> Configuration::getProjectSettings()
{

    // Return the corresponding member variable
    return _projectSettings;
}

/**
 * Function used to get the Peru settings for the configuration
 *
 * @return PeruSettings for the configuration
 */
std::shared_ptr<PeruSettings> Configuration::getPeruSettings()
{

    // Return the corresponding member variable
    return _peruSettings;
}

/**
 * Function used to get the CMake settings for the configuration
 *
 * @return CMakeSettings for the configuration
 */
std::shared_ptr<CMakeSettings> Configuration::getCMakeSettings()
{

    // Return the corresponding member variable
    return _cMakeSettings;
}

/**
 * Function used to get all of the dependencies for the configuration
 *
 * @return Vector of Dependencies for the configuration
 */
std::vector<std::shared_ptr<Dependency>> Configuration::getDependencies()
{

    // Return the corresponding member variable
    return _dependencies;
}

/**
 * Function used to get the specified output libraries for the provided dependency
 *
 * @param dependency Dependencies representing the dependency to use
 * @param target String representing the target to get the information for
 * @return Vector of Strings representing the output libraries for the target
 */
std::vector<std::string> Configuration::getLibrariesOutputForDependency(
        std::shared_ptr<Dependency> dependency, const std::string& target)
{

    // Return the corresponding member variable value
    return _outputLibsMap[dependency->getName()][target];
}

/**
 * Function used to get the specified output headers for the provided dependency
 *
 * @param dependency Dependencies representing the dependency to use
 * @param target String representing the target to get the information for
 * @return Vector of Strings representing the output headers for the target
 */
std::vector<std::string> Configuration::getHeadersOutputForDependency(
        std::shared_ptr<Dependency> dependency, const std::string& target)
{

    // Return the corresponding member variable value
    return _outputHeadersMap[dependency->getName()][target];
}

/**
 * Internal function used to get the library extension for the given target
 *
 * @param target String representing the target to get the extension for
 * @return String representing the lirbary extension for the target
 */
std::string Configuration::getLibExtensionForTarget(const std::string& target)
{

    // Create a return string
    std::string retString = "so";

    // If the target is windows-related, set it to dll
    if (target.find("windows") != std::string::npos)
        retString = "dll";

    // If the target is osx-related, set it to dylib
    if (target.find("darwin") != std::string::npos)
        retString = "dylib";

    // Return the return string
    return retString;
}

/**
 * Internal function used to replace any build-variables for YAML file
 *
 * @param target String representing the target to reference in the replacement
 * @param textToUse String representing the text to do the replacement in
 * @return String representing the input text with the replaced build-variables
 */
std::string Configuration::substituteBuildVariables(const std::string& target,
        const std::string& textToUse)
{

    // Create a copy of the input string for modification
    std::string retString = textToUse;

    // Create a mapping of values to be replaced
    std::map<std::string, std::string> replacementMap;
    replacementMap["${TARGET_TRIPLE}"] = target;
    replacementMap["${LIB_EXT}"] = getLibExtensionForTarget(target);

    // Replace each variable in the line at a time
    for (const auto& replacementItem : replacementMap) {

        // Find the text to be replaced and continuously replace it
        size_t startPos = 0;
        while ((startPos = retString.find(replacementItem.first, startPos)) != std::string::npos) {

            // Perform the actual replacement on the input string
            retString.replace(startPos, replacementItem.first.length(), replacementItem.second);

            // Update the start position for the next iteration
            startPos += replacementItem.second.length();
        }
    }

    // Return the modified string
    return retString;
}

/**
 * Internal function used to get the configuration for a target
 * NOTE: This will attempt to adapt target-triples to OS if needed
 *
 * @param yamlConfig YAML Node representing the configuration to source from
 * @param target String representing the target to get the configuration for
 * @return YAML Node representing the desired target-specific configuration
 */
Yaml::Node Configuration::getConfigurationForTarget(Yaml::Node& yamlConfig, const std::string& target)
{

    // Create a return node
    Yaml::Node returnNode;

    // First attempt to get the target-based node directly
    returnNode = yamlConfig["target " + target];

    // If the node is still empty we need to try the os-level
    // TODO - Define these mappings somewhere else
    if (returnNode.Size() == 0) {
        auto osTargetName = Constants::getTargetOsForImageTriple(target);
        returnNode = yamlConfig["target " + osTargetName];
    }

    // Return the return node
    return returnNode;
}
