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
#include <cstdlib>
#include <algorithm>
#include <BitBoson/HiggsBoson/HiggsBoson.h>
#include <BitBoson/HiggsBoson/Utils/Utils.h>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
#include <BitBoson/HiggsBoson/Utils/FileWriter.h>

using namespace BitBoson;

/**
 * Constructor used to setup the higgs-boson object with the specified file
 *
 * @param projectDir String representing the project directory for finding files
 * @param filePath String representing the path to the YAML configuration file
 * @param tmpDir String representing the temp/cache file-path for managing files
 */
HiggsBoson::HiggsBoson(const std::string& projectDir, const std::string& filePath,
        const std::string& tmpDir)
{

    // Setup the internal path variables for later use
    _cacheDir = tmpDir;
    _projectDir = projectDir;

    // Setup the configuration information for the provided files/directories
    _configuration = std::make_shared<Configuration>(projectDir, filePath, tmpDir);
}

/**
 * Function used to download the external dependencies for the project
 *
 * @return Boolean indicating whether the download was successful
 */
bool HiggsBoson::download()
{

    // Create a return flag
    bool retFlag = false;

    // Start the Higgs-Boson builder container
    HiggsBoson::RunTypeSingleton::runIdleContainer();

    // Call the Peru Download operation and setup the return value
    retFlag = _configuration->getPeruSettings()->peruSync();

    // Stop the Higgs-Boson builder container
    HiggsBoson::RunTypeSingleton::stopIdleContainer();

    // Return the return flag
    return retFlag;
}

/**
 * Function used to build the external dependencies for the project
 *
 * @param target String representing the target to build for
 * @return Boolean indicating whether the build was successful
 */
bool HiggsBoson::buildDependencies(const std::string& target)
{

    // Create a return flag
    bool retFlag = true;

    // Start the Higgs-Boson builder container
    HiggsBoson::RunTypeSingleton::runIdleContainer();

    // Define the appropriate directories for the target
    std::string targetCacheDir = _cacheDir + "/output/" + target;
    std::string targetHeaderCacheDir = _cacheDir + "/includes/" + target;

    // Only continue if the provided target is a valid one for the project
    auto validTargets = _configuration->getConfiguredTargets();
    if(std::find(validTargets.begin(), validTargets.end(), target) != validTargets.end())
    {

        // Remove the corresponding output directory
        HiggsBoson::RunTypeSingleton::executeInContainer("rm -rf " + targetCacheDir);
        HiggsBoson::RunTypeSingleton::executeInContainer("rm -rf " + targetHeaderCacheDir);

        // Re-create the output header cache directory
        retFlag &= HiggsBoson::RunTypeSingleton::executeInContainer("mkdir -p " + targetHeaderCacheDir);

        // Handle the building and outputs for all of the dependencies
        for (const auto& dependency : _configuration->getDependencies())
        {

            // Define the appropriate directories for the dependency
            std::string depCacheDir = targetCacheDir + "/" + dependency->getName();

            // Build the dependency for the provided target
            retFlag &= dependency->compileTarget(target,
                    _configuration->getLibrariesOutputForDependency(dependency, target),
                    _configuration->getHeadersOutputForDependency(dependency, target));

            // Ensure the dependency-target directory exist
            retFlag &= HiggsBoson::RunTypeSingleton::executeInContainer("mkdir -p " + depCacheDir);

            // Write the libraries to the cache output directories
            for (const auto& library : dependency->getLibraries(target))
                retFlag &= HiggsBoson::RunTypeSingleton::executeInContainer("cp " + library + " " + depCacheDir);

            // Write the headers to the cache output directories
            auto depOutputHeaderDir = std::string(dependency->getHeaderDir(target) + "/");
            auto cacheOutputHeaderDir = std::string(targetHeaderCacheDir + "/");
            retFlag &= HiggsBoson::RunTypeSingleton::executeInContainer(
                    "rsync -av " + depOutputHeaderDir + " " + cacheOutputHeaderDir);
        }
    }

    // Stop the Higgs-Boson builder container
    HiggsBoson::RunTypeSingleton::stopIdleContainer();

    // Return the return flag
    return retFlag;
}

/**
 * Function used to build the project itself for the given target
 *
 * @param target String representing the target to build for
 * @return Boolean indicating whether the build was successful
 */
bool HiggsBoson::buildProject(const std::string& target)
{

    // Create a return flag
    bool retFlag = false;

    // Start the Higgs-Boson builder container
    HiggsBoson::RunTypeSingleton::runIdleContainer();

    // Define the appropriate directories for the target
    std::string targetCacheDir = _cacheDir + "/output/" + target;
    std::string targetOutputDir = _projectDir + "/output/" + target;

    // Only continue if the provided target is a valid one for the project
    auto validTargets = _configuration->getConfiguredTargets();
    if(std::find(validTargets.begin(), validTargets.end(), target) != validTargets.end())
    {

        // Remove the corresponding output directory
        HiggsBoson::RunTypeSingleton::executeInContainer("rm -rf " + targetOutputDir);

        // Write-in all of the library dependencies into the CMakeLists.txt file
        for (const auto& dependency : _configuration->getDependencies())
            for (const auto& libraryFile : Utils::listFilesInDirectory(targetCacheDir + "/" + dependency->getName()))
                _configuration->getCMakeSettings()->addLibrary(libraryFile);

        // Write-in all of the header dependencies into the CMakeLists.txt file
        for (const auto& dependency : _configuration->getDependencies())
            _configuration->getCMakeSettings()->addIncludeDir(dependency->getHeaderDir(target));

        // Build the main project for the provided target
        auto buildSuccessfully = _configuration->getCMakeSettings()->buildCMakeProject(target);

        // Only continue if the build operation was successful
        if (buildSuccessfully)
        {

            // Open-up a new file to write the package script into
            std::string packageScriptPath = _cacheDir + "/builds/package-" + target + ".sh";
            auto packageScript = FileWriter(packageScriptPath);
            if (packageScript.isOpen())
            {

                // Ensure the output directories exist
                packageScript.writeLine("mkdir -p " + targetOutputDir);
                packageScript.writeLine("mkdir -p " + targetOutputDir + "/bin");
                packageScript.writeLine("mkdir -p " + targetOutputDir + "/lib");
                packageScript.writeLine("mkdir -p " + targetOutputDir + "/deps");
                packageScript.writeLine("mkdir -p " + targetOutputDir + "/pkg");

                // Copy the output file for the project into the appropriate directory
                std::string cMakeOutputDir = _cacheDir + "/builds/compile/" + target;
                if (_configuration->getProjectSettings()->getProjectType() == ProjectSettings::ProjectType::TYPE_EXE)
                    packageScript.writeLine("mv " + cMakeOutputDir + "/bin/* " + targetOutputDir + "/bin/");
                else
                    packageScript.writeLine("mv " + cMakeOutputDir + "/lib/* " + targetOutputDir + "/lib/");

                // Copy the dependencies into the appropriate directory
                for (const auto& dependency : _configuration->getDependencies())
                    for (const auto& libraryFile : Utils::listFilesInDirectory(targetCacheDir + "/" + dependency->getName()))
                        packageScript.writeLine("cp " + libraryFile + " " + targetOutputDir + "/deps/");

                // Compress the output files into the corresponding higgs-boson tar/package file
                std::string projectName = _configuration->getProjectSettings()->getProjectName();
                std::string projectVersion = _configuration->getProjectSettings()->getProjectVersion();
                std::string pkgName = projectName + "-" + projectVersion + "-" + target + ".hbsn";
                packageScript.writeLine("mkdir -p " + _cacheDir + "/pkg");
                packageScript.writeLine("cd " + targetOutputDir + " && tar -c -f " + _cacheDir + "/" + pkgName + " .");
                packageScript.writeLine("mv " + _cacheDir + "/" + pkgName + " " + targetOutputDir + "/pkg");

                // Close the package script once writing is complete
                packageScript.close();

                // Execute the written package script for the generated artifacts
                retFlag &= HiggsBoson::RunTypeSingleton::executeInContainer("Packaging Artifacts for " + projectName,
                        "bash " + packageScriptPath);

                // Ensure that the package was written successfully before closing-down the container
                HiggsBoson::RunTypeSingleton::waitForFileOrDirectoryExistence(targetOutputDir + "/pkg/" + pkgName);
            }
        }
    }

    // Stop the Higgs-Boson builder container
    HiggsBoson::RunTypeSingleton::stopIdleContainer();

    // Return the return flag
    return retFlag;
}

/**
 * Function used to test the project itself for the given test
 *
 * @param testType TestType representing the test to run
 * @param testFilter String representing the test filter to apply
 * @return Boolean indicating whether the test was successful
 */
bool HiggsBoson::testProject(CMakeSettings::TestType testType, const std::string& testFilter)
{

    // Create a return flag
    bool retFlag = false;

    // Start the Higgs-Boson builder container
    HiggsBoson::RunTypeSingleton::runIdleContainer();

    // Define the appropriate directories for the target
    std::string targetCacheDir = _cacheDir + "/output/default";

    // Write-in all of the library dependencies into the CMakeLists.txt file
    for (const auto& dependency : _configuration->getDependencies())
        for (const auto& libraryFile : Utils::listFilesInDirectory(targetCacheDir + "/" + dependency->getName()))
            _configuration->getCMakeSettings()->addLibrary(libraryFile);

    // Write-in all of the header dependencies into the CMakeLists.txt file
    for (const auto& dependency : _configuration->getDependencies())
        _configuration->getCMakeSettings()->addIncludeDir(dependency->getHeaderDir("default"));

    // Test the main project for the provided test-type
    retFlag = _configuration->getCMakeSettings()->testCMakeProject(testType, testFilter);

    // Stop the Higgs-Boson builder container
    HiggsBoson::RunTypeSingleton::stopIdleContainer();

    // Return the return flag
    return retFlag;
}
