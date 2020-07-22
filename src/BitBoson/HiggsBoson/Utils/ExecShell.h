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

#ifndef HIGGS_BOSON_EXEC_SHELL_H
#define HIGGS_BOSON_EXEC_SHELL_H

#include <string>
#include <vector>

namespace BitBoson::ExecShell
{

    // Namespace String value for use in the previous response
    static std::string previousPrintedResponse;

    /**
     * Function used to execute a given shell command on the
     * command-line for the system/operating-system
     *
     * @param command String representing the command to run
     * @return String representing the STDOUT for the command
     */
    std::string exec(std::string command);

    /**
     * Function used to execute a given shell command on the
     * command-line for the system/operating-system with live output
     *
     * @param command String representing the command to run
     * @return Boolean indicating whether the command was successful
     */
    bool execLive(std::string command);

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
    bool execWithResponse(std::string message, std::string command,
            std::string& printedResponse = previousPrintedResponse);
}

#endif //HIGGS_BOSON_EXEC_SHELL_H
