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
#include <vector>
#include <signal.h>
#include <BitBoson/HiggsBoson/HiggsBoson.h>
#include <BitBoson/HiggsBoson/Utils/Utils.h>
#include <BitBoson/HiggsBoson/Utils/FileWriter.h>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
#include <BitBoson/HiggsBoson/Utils/Constants.h>

using namespace BitBoson;

// Define some useful "constants" (will be modified)
std::string HIGGS_BUILDER_NAME = Constants::DOCKER_HIGGS_BUILDER_PREFIX;

/**
 * Function used to handle a Ctrl-C Interrupt for the application
 *
 * @param signal Integer representing the interrupt signal
 */
void handleInterrupt(int signal)
{

    // Stop any running docker containers
    ExecShell::exec("docker stop " + HIGGS_BUILDER_NAME);

    // Gracefully (yet forcefully) exit the application runtime
    std::exit(1);
}

/**
 * Function used to check if the given image name is valid for Dockcross
 *
 * @param image String representing the image name to validate
 * @return Boolean indicating whether the image name is valid or not
 */
bool isValidDockcrossImage(const std::string& image)
{

    // Create a return flag
    bool retFlag = false;

    // Check if the image is in the list of valid ones
    for (const auto& validImage : Constants::getValidImages())
        if (image == validImage)
            retFlag = true;

    // Return the return flag
    return retFlag;
}

/**
 * Function used to checkout (if necessary) the dockcross project
 *
 * @param cacheDir String representing the cache directory to use
 * @param update Boolean indicating that we want to update dockcross
 */
void checkoutDockross(const std::string& cacheDir, bool update=false)
{

    // Check-out the BitBoson fork of the dockcross project
    ExecShell::exec(std::string("git clone --recurse-submodules -b higgs-boson")
            + std::string(" https://github.com/bitboson-deps/dockcross.git ")
            + cacheDir + std::string("/dockcross"));

    // Update the dockcross installation if desired
    if (update)
    {

        // Ensure that the latest dockcross/higgs-boson submodule is available
        ExecShell::execWithResponse("Updating Dockcross Installation",
                "cd " + cacheDir + std::string("/dockcross")
                + " && git fetch && git pull origin higgs-boson");
        ExecShell::execWithResponse("Updating Internal Higgs-Boson Installation",
                "cd " + cacheDir + std::string("/dockcross/higgs-boson")
                + " && git fetch && git pull origin mainline");
    }
}

/**
 * Function used to setup osx build support for higgs-boson
 *
 * @param xcodeSdkPath String representing the XCode SDK path
 * @param projectDir String representing the project directory to use
 * @param cacheDir String representing the cache directory to use
 * @param isOsxMachine Boolean indicating if this is an OSX Machine
 * @return Boolean indicating if the operation was successful or not
 */
bool addOsxBuildSupport(const std::string& xcodeSdkPath,
        const std::string& projectDir, const std::string& cacheDir,
        bool isOsxMachine)
{

    // Create a return flag
    bool retFlag = true;

    // Ensure the Dockross project is checked-out
    checkoutDockross(cacheDir);

    // Determine if this is a OSX Machine or not
    // If so, package the SDK locally instead
    if (isOsxMachine)
    {

        // Package the SDK locally
        ExecShell::execWithResponse("Packaging Local XCode SDK",
            "cd " + cacheDir + "/dockcross/osxcross && "
            + "./tools/gen_sdk_package.sh && mv MacOSX*.xz ./tarballs");
    }

    // Handle the case where we are building from
    // a provided XCode SDK file instead
    else
    {

        // Package SDK from the provided XCode file
        std::string osxCrossDir = cacheDir + "/dockcross/osxcross";
        ExecShell::execWithResponse("Packaging Provided XCode SDK",
            "cd " + cacheDir + "/dockcross "
            + "&& echo \"docker run --name " + HIGGS_BUILDER_NAME + " --rm -w " + osxCrossDir
            + " -v " + osxCrossDir + ":" + osxCrossDir
            + (xcodeSdkPath.empty() ? "" : std::string(" -v" + xcodeSdkPath + ":" + xcodeSdkPath))
            + " -t bitboson/osxcross-pkg \"\\$\\@\"\" > ./osxcross/bitboson-macos"
            + " && chmod +x ./osxcross/bitboson-macos"
            + " && make osxcross-pkg"
            + " && cd osxcross"
            + " && ./bitboson-macos ./tools/gen_sdk_package_pbzx.sh " + xcodeSdkPath
            + " && mv MacOSX*.xz ./tarballs");
    }

    // Return the return flag
    return retFlag;
}

/**
 * Function used to setup the provided docker image for the specified target
 *
 * @param target String representing the target to use
 * @param projectDir String representing the project directory to use
 * @param globalCacheDir String representing the cache directory to use
 * @param localCacheDir String representing the local cache directory to use
 * @param dirHash String representing the directory hash for volume-names to use
 * @param makeDockerContainer Boolean indicating whether to build the container
 * @param interactive Boolean indicating whether the container should be interactive
 * @return String representing the run command to use
 */
std::string setupDockerImage(const std::string& target,
        const std::string& projectDir, const std::string& globalCacheDir,
        const std::string& localCacheDir, const std::string& dirHash,
        bool makeDockerContainer=true, bool interactive=false)
{

    // Create a return value
    std::string retVal = "bash";

    // Ensure the Dockross project is checked-out
    checkoutDockross(globalCacheDir);

    // Get the name of the project-directory volume for docker to use
    std::string dockerSyncVolume = projectDir;
    if (HiggsBoson::RunTypeSingleton::getDockerSync() != nullptr)
        dockerSyncVolume = HiggsBoson::RunTypeSingleton::getDockerSync()->getVolume();

    // Re-configure the container-name based on the target
    HIGGS_BUILDER_NAME = HIGGS_BUILDER_NAME + "-" + target;
    FileWriter::FileWriter::FileWriterConfigSingleton::setDockerContainerName(HIGGS_BUILDER_NAME);

    // Handle the higgs-boson target specifically
    if (target == "higgs-boson")
    {

        // Build the docker image and setup the executable
        ExecShell::execWithResponse("Building Higgs-Boson Docker Image",
                "cd " + globalCacheDir + "/dockcross"
                + std::string(makeDockerContainer ? " && TAG=latest make higgs-boson" : "")
                + " && echo \"docker run --name " + HIGGS_BUILDER_NAME
                    + (interactive ? " --interactive" : "")
                    + " --rm -w " + projectDir
                    + " --mount type=tmpfs,destination=/ramdisk "
                    + " -v " + dockerSyncVolume + ":" + projectDir
                    + " -t bitboson/higgs-builder \"\\$\\@\"\" > ./bitboson-higgs-builder"
                + " && chmod +x ./bitboson-higgs-builder");

        // Setup the return value accordingly
        retVal = globalCacheDir + "/dockcross/bitboson-higgs-builder "
                + (interactive ? "bash" : "container-watch-dog");
    }

    // Handle all other targets accordingly for docross
    if (target != "higgs-boson")
    {

        // Determine if this is a darwin (macos) related container or not
        bool isMacOsxTarget = false;
        if ((target.find("apple") != std::string::npos)
                || (target.find("darwin") != std::string::npos))
            isMacOsxTarget = true;

        // Ensure we setup init commands for MacOSX instances
        if (isMacOsxTarget)
            HiggsBoson::RunTypeSingleton::setDockerRunInitCommand("init-osx");

        // Build the docker image and setup the executable
        ExecShell::execWithResponse("Building Docker Image " + target,
                "cd " + globalCacheDir + "/dockcross"
                + std::string(makeDockerContainer ? " && TAG=latest make " + target : "")
                + " && echo \"docker run --name " + HIGGS_BUILDER_NAME
                + (interactive ? " --interactive" : "")
                + " --rm -w " + projectDir
                + " -v " + dockerSyncVolume + ":" + projectDir
                + " -t bitboson/" + target + " \"\\$\\@\"\" > ./bitboson-" + target
                + " && chmod +x ./bitboson-" + target);

        // Setup the return value accordingly
        retVal = globalCacheDir + "/dockcross/bitboson-" + target + " ";
        retVal += (std::string(isMacOsxTarget ? " init-osx " : "") +
                + (interactive ? "bash" : "container-watch-dog"));
    }

    // Return the return value
    return retVal;
}

/**
 * Function used to get the run-type command for the given details
 *
 * @param target String representing the target to use
 * @param projectDir String representing the project directory to use
 * @param globalCacheDir String representing the cache directory to use
 * @param localCacheDir String representing the local cache directory to use
 * @param dirHash String representing the directory hash for volume-names to use
 * @param interactive Boolean indicating whether the container should be interactive
 * @return String representing the run command to use
 */
std::string getRunTypeCommand(const std::string& target,
        const std::string& projectDir, const std::string& globalCacheDir,
        const std::string& localCacheDir, const std::string& dirHash,
        bool interactive=false)
{

    // Setup the shell file for running the docker container
    // and return the run-type command for the docker container
    return setupDockerImage(target, projectDir, globalCacheDir, localCacheDir,
            dirHash, false, interactive);
}

/**
 * Main function used to run the application instance from the command-line
 *
 * @param argc Integer representing the number of command-line arguments
 * @param argv Character* Array representing the difference command-line arguments
 * @return Integer representing the return code of the application instance
 */
int main(int argc, char* argv[])
{

    // Setup a Ctrl-C Interrupt handler to exit the application
    signal(SIGINT, handleInterrupt);

    // Check if the command-line argument is "help"
    if ((argc > 1) && (std::string(argv[1]) == "help"))
    {

        // Print the "help" message
        std::cout << "Higgs-Boson Command-Line Build System" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  higgs-boson <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  list-targets                  List all possible targets supported by higgs-boson" << std::endl;
        std::cout << "  update-builders               Update the builder-containers to the latest version" << std::endl;
        std::cout << "  setup <target> [XCode|local]  Setup cross-compilation support for the provided target" << std::endl;
        std::cout << "  download [local]              Download all external dependencies (local is outside of docker)" << std::endl;
        std::cout << "  build-deps <target*>          Build all external dependencies for a given target" << std::endl;
        std::cout << "  build <target*>               Build the main project for a given target" << std::endl;
        std::cout << "  test <filter>                 Run the provided/desired tests (wild-card filter)" << std::endl;
        std::cout << "  profile <filter>              Run the provided/desired profile tests (wild-card filter)" << std::endl;
        std::cout << "  debug                         Run the provided/desired tests in debugging mode" << std::endl;
        std::cout << "  coverage                      Run all tests and produce a code-coverage report (including html)" << std::endl;
        std::cout << "  sanitize <type**>             Run the provided/desired code sanitizer for code quality" << std::endl;
        std::cout << "  cli <target*>                 Run an interactive shell on the provided build container" << std::endl;
        std::cout << "  cmd <target*> <options>       Run generic commands (via bash) on the provided build container" << std::endl;
        std::cout << "  run <additional args>         Run the built executable on the current platform" << std::endl;
        std::cout << std::endl;
        std::cout << "*Possible targets depend on each individual project" << std::endl;
        std::cout << "**Test/Sanitize types include: address, behavior, thread, and leak" << std::endl;

        // Return zero to exit early
        return 0;
    }

    // Get the current path for the running instance
    auto currentPath = Utils::splitStringByDelimiter(ExecShell::exec("pwd"), '\n').front();

    // Deduce a unique hash which represents this project's path on disk
    auto projectDirHash = Utils::sha256(currentPath);
    HIGGS_BUILDER_NAME = HIGGS_BUILDER_NAME + projectDirHash;
    FileWriter::FileWriter::FileWriterConfigSingleton::setDockerContainerName(HIGGS_BUILDER_NAME);

    // Define the cache directory to live in the user's home-path
    // If this does not exist we can define it to exist locally
    std::string userHomePath = Utils::getHomePath();
    if (!userHomePath.empty() && ((userHomePath.back() == '/') || (userHomePath.back() == '\\')))
        userHomePath.pop_back();
    std::string appCacheDir = (currentPath + "/.higgs-boson");
    std::string globalCacheDir = (userHomePath.empty()
            ? (currentPath + "/.higgs-boson")
            : (userHomePath + "/.higgs-boson"));

    // Ensure that the cache directory exists
    ExecShell::exec("mkdir -p " + appCacheDir);

    // Use/setup docker-sync if it is installed (or configured to do so)
    bool dockerSyncInstalled = (ExecShell::exec("docker-sync --version")
            .find("not found") == std::string::npos);
    dockerSyncInstalled |= (ExecShell::exec("docker-sync --version")
            .find("docker-sync:") == std::string::npos);
    if (dockerSyncInstalled && (std::string(argv[1]) != "setup"))
        HiggsBoson::RunTypeSingleton::getDockerSync(currentPath, appCacheDir, projectDirHash, globalCacheDir);

    // Handle list-targets command (if applicable)
    if ((argc > 1) && (std::string(argv[1]) == "list-targets"))
    {

        // Simply get and list all possible image targets
        for (const auto& imageTarget : Constants::getValidImages())
            std::cout << imageTarget << std::endl;

        // Return zero to exit early
        return 0;
    }

    // Handle update-builders command (if applicable)
    if ((argc > 1) && (std::string(argv[1]) == "update-builders"))
    {

        // Simply update dockcross and related repositories
        checkoutDockross(globalCacheDir, true);

        // Return zero to exit early
        return 0;
    }

    // Handle setup command (if applicable)
    if ((argc > 1) && (std::string(argv[1]) == "setup"))
    {

        // Determine the target we are setting-up (if provided)
        std::string setupTarget;
        if ((argc <= 2) || ((argc > 2) && (!std::string(argv[2]).empty())))
            setupTarget = std::string(argv[2]);

        // Only continue if the target specified is valid
        bool response = false;
        if (isValidDockcrossImage(setupTarget))
        {

            // Handle the case where we are building an OSX target
            bool handledOsxTarget = true;
            if (setupTarget.find("darwin") != std::string::npos)
            {

                // Determine if we are building locally or not based
                // on whether a file was give for XCode or not
                // (otherwise we'll assume locally)
                bool buildingLocally = ((argc > 3) && (std::string(argv[3]) == "local"));

                // Determine if we are building using an already packaged XCode SDK
                bool isPrePackaged = ((argc > 3) && (std::string(argv[3]) == "packaged"));

                // Extract the XCode file if not building locally
                std::string xcodeFile;
                if (!buildingLocally && (argc > 3) && !std::string(argv[3]).empty())
                    xcodeFile = std::string(argv[3]);

                // Attempt to actually setup the OSX cross-build support
                if (!isPrePackaged && (buildingLocally || !xcodeFile.empty()))
                    handledOsxTarget = addOsxBuildSupport(xcodeFile,
                            currentPath, globalCacheDir, buildingLocally);
                else
                    handledOsxTarget = true;
            }

            // Attempt to build the actual docker image itself
            if (handledOsxTarget)
                response = (setupDockerImage(setupTarget, currentPath, globalCacheDir,
                        appCacheDir, projectDirHash) != "sh");

        }

        // Exit from the execution based on the result
        return (response ? 0 : 1);
    }

    // Handle cli command (if applicable)
    if ((argc > 1) && (std::string(argv[1]) == "cli"))
    {

        // Determine the target we are setting-up (if provided)
        std::string cliTarget = "higgs-boson";
        if ((argc > 2) && (!std::string(argv[2]).empty()))
            cliTarget = std::string(argv[2]);

        // Ensure the corresponding container is stopped
        std::cout << "Stopping Running Container (if running) ... " << std::flush;
        ExecShell::exec("docker stop " + HIGGS_BUILDER_NAME + "-" + cliTarget);
        std::cout << "OK" << std::endl;

        // Simply run the desired interactive docker container
        return ExecShell::execLive(getRunTypeCommand(cliTarget,
                currentPath, globalCacheDir, appCacheDir, projectDirHash, true));
    }

    // Handle all generic commands here
    if ((argc > 1) && (std::string(argv[1]) == "cmd"))
    {

        // Setup a return flag
        bool retFlag = false;

        // Determine the target we are setting-up (if provided)
        bool foundTarget = false;
        std::string cliTarget = "higgs-boson";
        if ((argc > 2) && (!std::string(argv[2]).empty()))
        {

            // Extract the attribute for the container name
            std::string possibleTarget = std::string(argv[2]);

            // Check if it is "valid" and use it if so
            for (const auto& imageTarget : Constants::getValidImages())
                if (possibleTarget == imageTarget)
                    foundTarget = true;
            if (foundTarget)
                cliTarget = possibleTarget;
        }

        // Setup the docker container for building files
        HiggsBoson::RunTypeSingleton::setDockerRunCommand(
                getRunTypeCommand(cliTarget, currentPath, globalCacheDir,
                                  appCacheDir, projectDirHash), HIGGS_BUILDER_NAME);
        HiggsBoson::RunTypeSingleton::runIdleContainer();

        // Collect all remaining arguments to pass to the command
        std::string commandInfo;
        for (int ii = (foundTarget ? 3 : 2); ii < argc; ii++)
            commandInfo += (std::string(argv[ii]) + std::string(" "));

        // Actually handle the generic command here
        HiggsBoson::RunTypeSingleton::executeInContainer(commandInfo);

        // Return the status of the operation
        return (retFlag ? 0 : 1);
    }

    // Setup the Higgs-Boson configuration for the instance
    auto higgsBoson = HiggsBoson(currentPath, currentPath + "/higgs-boson.yaml", appCacheDir);

    // Handle run command (if applicable)
    if ((argc > 1) && (std::string(argv[1]) == "run"))
    {

        // Collect all remaining arguments to pass to the run command
        std::string runArgs;
        for (int ii = 2; ii < argc; ii++)
            runArgs += (std::string(argv[ii]) + std::string(" "));

        // Handle the run-command based on the current platform (from pre-processor macros)
        #ifdef HIGGS_BOSON_TARGET_OS
            #if HIGGS_BOSON_TARGET_OS == linux
                std::string binaryPath = "output/default/bin/" + higgsBoson.getProjectName();
                std::string dependenciesPath = "output/default/deps";
                ExecShell::execLive("LD_LIBRARY_PATH=" + dependenciesPath + " ./" + binaryPath + " " + runArgs);
            #elif HIGGS_BOSON_TARGET_OS == darwin
                std::string binaryPath = "output/default/bin/" + higgsBoson.getProjectName();
                std::string dependenciesPath = "output/default/deps";
                ExecShell::execLive("DYLD_LIBRARY_PATH=" + dependenciesPath + " ./" + binaryPath + " " + runArgs);
            #elif HIGGS_BOSON_TARGET_OS == windows
                std::string binaryPath = "output/default/bin/" + higgsBoson.getProjectName();
                std::string dependenciesPath = "output/default/deps";
                ExecShell::execLive("PATH=" + dependenciesPath + " " + binaryPath + " " + runArgs);
            #endif
        #endif

        // Return zero to exit early
        return 0;
    }

    // Assume all commands are run in the default Higgs-Boson docker container
    // unless otherwise specified
    if ((argc <= 2) || ((std::string(argv[1]) != "internal") && (std::string(argv[2]) != "internal")))
        HiggsBoson::RunTypeSingleton::setDockerRunCommand(setupDockerImage("higgs-boson",
                currentPath, globalCacheDir, appCacheDir, projectDirHash), HIGGS_BUILDER_NAME);

    // Handle download command (if applicable)
    if ((argc > 1) && (std::string(argv[1]) == "download"))
    {

        // Setup a return flag
        bool retFlag = false;

        // Handle the default download operation (if applicable)
        if ((argc <= 2) || ((argc > 2) && (std::string(argv[2]) == "default")))
            retFlag = higgsBoson.download();

        // Handle the local download operation (if applicable)
        if ((argc > 2) && (std::string(argv[2]) == "local"))
        {
            HiggsBoson::RunTypeSingleton::setDockerRunCommand("sh");
            retFlag = higgsBoson.download();
        }

        // Handle the internal download operation (if applicable)
        if ((argc > 2) && (std::string(argv[2]) == "internal"))
        {
            HiggsBoson::RunTypeSingleton::setDockerRunCommand("sh");
            retFlag = higgsBoson.download();
        }

        // Return the status of the operation
        return (retFlag ? 0 : 1);
    }

    // Handle dependencies command (if applicable)
    if ((argc > 1) && (std::string(argv[1]) == "build-deps"))
    {

        // Setup a return flag
        bool retFlag = false;

        // Handle the default build operation (if applicable)
        if ((argc <= 2) || ((argc > 2) && (std::string(argv[2]) == "default")))
            retFlag = higgsBoson.buildDependencies("default");

        // Handle the local build operation (if applicable)
        if ((argc > 2) && (std::string(argv[2]) == "local"))
        {
            HiggsBoson::RunTypeSingleton::setDockerRunCommand("sh");
            retFlag = higgsBoson.buildDependencies("local");
        }

        // Handle the internal build operation (if applicable)
        if (((argc > 2) && (std::string(argv[2]) == "internal"))
                && ((argc > 3) && (!std::string(argv[3]).empty())))
        {
            HiggsBoson::RunTypeSingleton::setDockerRunCommand("sh");
            retFlag = higgsBoson.buildDependencies(std::string(argv[3]));
        }

        // If the target was anything else, attempt to use
        // it as a dockcross target instead
        if ((argc > 2) && isValidDockcrossImage(std::string(argv[2])))
        {
            HiggsBoson::RunTypeSingleton::setDockerRunCommand(
                    getRunTypeCommand(std::string(argv[2]),
                            currentPath, globalCacheDir, appCacheDir,
                            projectDirHash), HIGGS_BUILDER_NAME);
            retFlag = higgsBoson.buildDependencies(std::string(argv[2]));
        }

        // Return the status of the operation
        return (retFlag ? 0 : 1);
    }

    // Handle build command (if applicable)
    if ((argc > 1) && (std::string(argv[1]) == "build"))
    {

        // Setup a return flag
        bool retFlag = false;

        // Handle the default build operation (if applicable)
        if ((argc <= 2) || ((argc > 2) && (std::string(argv[2]) == "default")))
            retFlag = higgsBoson.buildProject("default");

        // Handle the local build operation (if applicable)
        if ((argc > 2) && (std::string(argv[2]) == "local"))
        {
            HiggsBoson::RunTypeSingleton::setDockerRunCommand("sh");
            retFlag = higgsBoson.buildProject("local");
        }

        // Handle the internal build operation (if applicable)
        if (((argc > 2) && (std::string(argv[2]) == "internal"))
                && ((argc > 3) && (!std::string(argv[3]).empty())))
        {
            HiggsBoson::RunTypeSingleton::setDockerRunCommand("sh");
            retFlag = higgsBoson.buildProject(std::string(argv[3]));
        }

        // If the target was anything else, attempt to use
        // it as a dockcross target instead
        if ((argc > 2) && isValidDockcrossImage(std::string(argv[2])))
        {
            HiggsBoson::RunTypeSingleton::setDockerRunCommand(
                    getRunTypeCommand(std::string(argv[2]),
                            currentPath, globalCacheDir, appCacheDir,
                            projectDirHash), HIGGS_BUILDER_NAME);
            retFlag = higgsBoson.buildProject(std::string(argv[2]));
        }

        // Return the status of the operation
        return (retFlag ? 0 : 1);
    }

    // Setup a return flag to handle all remaining (test) cases
    bool retFlag = false;

    // If there is a filter criteria, read it in as well
    // to apply to tests when they are run
    std::string testFilter = "";
    bool isInternal = (((argc > 2) && (std::string(argv[1]) == "internal"))
            || ((argc > 3) && (std::string(argv[1]) == "internal")));
    if (!isInternal && ((((argc > 1) && (std::string(argv[1]) == "sanitize"))
            && (argc > 3) && (!std::string(argv[3]).empty()))
            || ((argc > 1) && (std::string(argv[1]) != "sanitize")
                && (argc > 2) && (!std::string(argv[2]).empty()))))
        testFilter = std::string(argv[2]);
    else if (isInternal && ((((argc > 2) && (std::string(argv[2]) == "sanitize"))
            && (argc > 4) && (!std::string(argv[4]).empty()))
            || ((argc > 2) && (std::string(argv[2]) != "sanitize")
                && (argc > 3) && (!std::string(argv[3]).empty()))))
        testFilter = std::string(argv[3]);

    // Handle test command (if applicable)
    if (((argc > 1) && (std::string(argv[1]) == "profile"))
            || (isInternal && (std::string(argv[2]) == "profile")))
        retFlag = higgsBoson.testProject(CMakeSettings::TestType::PROFILE, testFilter);

    // Handle test command (if applicable)
    if (((argc > 1) && (std::string(argv[1]) == "test"))
            || (isInternal && (std::string(argv[2]) == "test")))
        retFlag = higgsBoson.testProject(CMakeSettings::TestType::TEST, testFilter);

    // Handle debug command (if applicable)
    if (((argc > 1) && (std::string(argv[1]) == "debug"))
            || (isInternal && (std::string(argv[2]) == "debug")))
        retFlag = higgsBoson.testProject(CMakeSettings::TestType::DEBUG, testFilter);

    // Handle coverage command (if applicable)
    if (((argc > 1) && (std::string(argv[1]) == "coverage"))
            || (isInternal && (std::string(argv[2]) == "coverage")))
        retFlag = higgsBoson.testProject(CMakeSettings::TestType::COVERAGE, testFilter);

    // Handle sanitize command (if applicable)
    if (((argc > 1) && (std::string(argv[1]) == "sanitize"))
            || (isInternal && (std::string(argv[2]) == "sanitize")))
    {

        // Handle the sanitize address operation (if applicable)
        if (((argc > 2) && (std::string(argv[2]) == "address"))
                || (isInternal && (std::string(argv[3]) == "address")))
            retFlag = higgsBoson.testProject(CMakeSettings::TestType::SANITIZE_ADDRESS);

        // Handle the sanitize address operation (if applicable)
        else if (((argc > 2) && (std::string(argv[2]) == "behavior"))
                || (isInternal && (std::string(argv[3]) == "behavior")))
            retFlag = higgsBoson.testProject(CMakeSettings::TestType::SANITIZE_BEHAVIOR);

        // Handle the sanitize address operation (if applicable)
        else if (((argc > 2) && (std::string(argv[2]) == "thread"))
                || (isInternal && (std::string(argv[3]) == "thread")))
            retFlag = higgsBoson.testProject(CMakeSettings::TestType::SANITIZE_THREAD);

        // Handle the sanitize address operation (if applicable)
        else if (((argc > 2) && (std::string(argv[2]) == "leak"))
                || (isInternal && (std::string(argv[3]) == "leak")))
            retFlag = higgsBoson.testProject(CMakeSettings::TestType::SANITIZE_LEAK);

        // Handle the case where no desired sanitizer was selected
        else
            std::cout << "A valid sanitizer must be chosen: address, behavior, thread, or leak" << std::endl;
    }

    // Return the result of the return-flag
    return (retFlag ? 0 : 1);
};
