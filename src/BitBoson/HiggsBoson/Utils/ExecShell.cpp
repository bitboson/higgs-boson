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

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <mutex>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>

using namespace BitBoson;

// Namespace mutex to coordinate shell operations
std::recursive_mutex shellMutex;

/**
 * Function used to execute a given shell command on the
 * command-line for the system/operating-system
 *
 * @param command String representing the command to run
 * @param background Boolean indicating whether to background
 *                   the execution of the provided command
 * @return String representing the STDOUT for the command
 */
std::string ExecShell::exec(std::string command, bool background)
{

    // Create a return value
    std::string retValue;

    // Lock the shell operation
    shellMutex.lock();

    // Handle the non-background case
    if (!background)
    {

        // Run the command while piping STDERR to STDOUT
        command += " 2>&1";

        // Run the command using popen with a buffer for the
        // STDOUT to write to the local string variable
        std::array<char, 128> buffer;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);

        // Only continue if the pipe was opened properly and
        // continuously append the buffer to the results
        while (pipe && (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr))
            retValue += buffer.data();
    }

    // Handle the non-background case
    if (background)
    {

        // Execute the command using the system operation
        system(std::string(command + " > /dev/null &").c_str());
    }

    // Unlock the shell operation
    shellMutex.unlock();

    // Return the return value
    return retValue;
}

/**
 * Function used to execute a given shell command on the
 * command-line for the system/operating-system with live output
 *
 * @param command String representing the command to run
 * @return Boolean indicating whether the command was successful
 */
bool ExecShell::execLive(std::string command)
{

    // Setup a return flag
    bool retFlag;

    // Lock the shell operation
    shellMutex.lock();

    // Simply run the command and extract the results
    retFlag = (system(std::string(command).c_str()) == 0);

    // Unlock the shell operation
    shellMutex.unlock();

    // Return the return flag
    return retFlag;
}

/**
 * Function used to execute a given shell command on the
 * command-line for the system/operating-system with a
 * response in the following format:
 * <message> ... <OK/FAIL>
 * If failed, additional output here...
 *
 * @param message String representing the message for output
 * @param command String representing the command to run
 * @param printedResponse String reference representing the printed
 *                        response for internal use/reference
 * @return Boolean indicating whether the command was successful
 */
bool ExecShell::execWithResponse(std::string message, std::string command,
        std::string& printedResponse)
{

    // Create a return flag
    bool retFlag = false;

    // Lock the shell operation
    shellMutex.lock();

    // Clear the printed-response string for a clean output
    printedResponse = "";

    // Print the message for the command that is running
    std::cout << message << " ... " << std::flush;
    printedResponse += (message + " ... ");

    // Run the command while piping STDERR to STDOUT
    command += " 2>&1";

    // Run the provided command with an echo of a response tag
    // so we can evaluate the results of the command being run
    std::string responseTag = "HIGGSBOSONSUCCESSFULRESPONSE";
    command += " && echo " + responseTag;
    auto response = exec(command);

    // Determine whether the response contained the tag or not
    std::size_t tagPosition = response.find(responseTag);
    if (tagPosition != std::string::npos)
        retFlag = true;

    // Print the result of the command on the output
    std::cout << (retFlag ? "OK" : "FAIL") << std::endl;
    printedResponse += (retFlag ? "OK\n" : "FAIL\n");

    // If the command execution was a failure, print the output
    if (!retFlag)
    {
        std::cout << response << std::endl;
        printedResponse += (response + "\n");
    }

    // Unlock the shell operation
    shellMutex.unlock();

    // Return the return flag
    return retFlag;
}
