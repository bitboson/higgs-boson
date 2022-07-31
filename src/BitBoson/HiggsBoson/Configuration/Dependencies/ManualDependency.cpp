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

#include <regex>
#include <string>
#include <algorithm>
#include <BitBoson/HiggsBoson/HiggsBoson.h>
#include <BitBoson/HiggsBoson/Utils/Utils.h>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
#include <BitBoson/HiggsBoson/Utils/FileWriter.h>
#include <BitBoson/HiggsBoson/Configuration/Dependencies/ManualDependency.h>

using namespace BitBoson;

/**
 * Constructor used to setup the dependency with the given name and targets
 *
 * @param dir String representing the path to the directory for the project
 * @param name String representing the unique name of the dependency
 * @param targets Vector of Strings representing the available targets
 */
ManualDependency::ManualDependency(const std::string& dir, const std::string& name,
        const std::vector<std::string>& targets) : Dependency(dir, name, targets)
{

    // Unused...
}

/**
 * Function used to set the build-steps for the given target
 *
 * @param target String representing the target to set the build steps for
 * @param buildSteps Vector of Strings representing the build-steps for
 *                   the target
 * @return Boolean indicating whether the operation was successful
 */
bool ManualDependency::setBuildSteps(const std::string& target,
        const std::vector<std::string>& buildSteps)
{

    // Create a return flag
    bool retFlag = false;

    // Get the targets and directory for the dependency
    auto dir = getDir();
    auto targets = getTargets();

    // Simply add the build-steps to the appropriate target (if it exists)
    if (std::find(targets.begin(), targets.end(), target) != targets.end())
    {

        // Open the build file
        auto buildFile = FileWriter(dir + "/higgs-build_" + target + ".sh");
        if (buildFile.isOpen())
        {

            // Write-in the environment variables for building
            buildFile.writeLine("cd " + dir);
            buildFile.writeLine("HIGGS_TARGET=" + target);
            buildFile.writeLine("HIGGS_HEADER_DIR=" + getHeaderDir(target));
            buildFile.writeLine("HIGGS_LIBRARY_DIR=" + getLibraryDir(target));
            buildFile.writeLine("mkdir -p " + getHeaderDir(target));
            buildFile.writeLine("mkdir -p " + getLibraryDir(target));

            // Write the build-steps to the corresponding file
            for (const auto& buildStep : buildSteps)
            {

                // Perform relevant replacements for the build-step
                auto modifiedBuildStep = std::regex_replace(buildStep, std::regex("__COLON__"), ":");
                modifiedBuildStep = std::regex_replace(modifiedBuildStep, std::regex("__QUOTE__"), "\"");

                // Write-in the modified build-setup
                buildFile.writeLine(modifiedBuildStep);
            }

            // Close the build file
            buildFile.close();

            // If we get here, mark the operation as successful
            retFlag = true;
        }
    }

    // Return the return flag
    return retFlag;
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
bool ManualDependency::compileTarget(const std::string& target,
        const std::vector<std::string>& libPaths,
        const std::vector<std::string>& headerDirs)
{

    // Create a return flag
    bool retFlag = false;

    // Remove the corresponding library and header files for output
    HiggsBoson::RunTypeSingleton::executeInContainer("rm -rf " + getLibraryDir(target));
    HiggsBoson::RunTypeSingleton::executeInContainer("rm -rf " + getHeaderDir(target));

    // Setup the build-file for the pre-configured build-target
    auto buildFile = getDir() + "/higgs-build_" + target + ".sh";

    // If we get here, it means that we got a non-empty response
    // So we'll have to determine if the build-process failed
    // or not for this particular target
    retFlag = HiggsBoson::RunTypeSingleton::executeInContainer(
            "Building " + getName() + " for Target " + target,
            "bash " + buildFile);

    // Archive/cache atrifacts if they are present and need archiving/caching
    if (retFlag)
        retFlag = postBuildArtifactCache(target, libPaths, headerDirs);

    // Return the return flag
    return retFlag;
}

/**
 * Function used to cache/save atrifacts (libraries/headers) after a build
 *
 * @param target String representing the target to cache for
 * @param libPaths Vector of Strings representing the output libraries
 * @param headerDirs Vector of Strings representing the header directories
 * @param fullPathsGiven Boolean indicating whether all paths are full (absolute)
 * @return Boolean indicating whether the artifact cache was successful
 */
bool ManualDependency::postBuildArtifactCache(const std::string& target,
        const std::vector<std::string>& libPaths,
        const std::vector<std::string>& headerDirs,
        bool fullPathsGiven)
{

    // Create a return flag
    bool retFlag = true;

    // Determine the path prefix based on whether paths are absolute or not
    std::string pathPrefix = (fullPathsGiven ? "" : (getDir() + "/"));

    // Copy the libraries contents into the expected
    // output directory for the particular target we are on
    if (retFlag && !libPaths.empty())
    {

        // Actually perform the copy operation
        std::string buildMessage = "Caching " + getName() + " Binary ";
        auto buildMessage2 = " for Target " + target;
        for (const auto& libPath : libPaths)
            retFlag = HiggsBoson::RunTypeSingleton::executeInContainer(
                    buildMessage + (
                            (fullPathsGiven && (libPath.find_last_of('/') != std::string::npos)) ? libPath.substr(libPath.find_last_of('/') + 1) : libPath
                        ) + buildMessage2,
                    "cp " + pathPrefix + libPath + " " + getLibraryDir(target) + "/");
    }

    // Copy the headers contents into the expected
    // output directory for the particular target we are on
    if (retFlag && !headerDirs.empty())
    {

        // Actually perform the copy operation
        std::string buildMessage = "Caching " + getName() + " Headers ";
        auto buildMessage2 = " for Target " + target;
        for (const auto& headerDir : headerDirs)
            retFlag = HiggsBoson::RunTypeSingleton::executeInContainer(
                    buildMessage + (
                            (fullPathsGiven && (headerDir.find_last_of('/') != std::string::npos)) ? headerDir.substr(headerDir.find_last_of('/') + 1) : headerDir
                        ) + buildMessage2,
                    "rsync -av --exclude='*/higgs-boson_*' " + pathPrefix + headerDir + " " + getHeaderDir(target) + "/");
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
std::vector<std::string> ManualDependency::getLibraries(const std::string& target)
{

    // Create a return vector
    std::vector<std::string> retVect;

    // Only continue if the given target exists
    auto targets = getTargets();
    if (std::find(targets.begin(), targets.end(), target) != targets.end())
    {

        // List all of the library files in the library output for the
        // given target and add them to the return vector object
        retVect = Utils::listFilesInDirectory(getLibraryDir(target));
    }

    // Return the return vector
    return retVect;
}
