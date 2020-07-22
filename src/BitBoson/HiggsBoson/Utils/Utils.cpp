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
#include <sstream>
#include <algorithm>
#include <BitBoson/HiggsBoson/Utils/Utils.h>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>

using namespace BitBoson;

/**
 * Function used to list the files (recursively) in the given
 * directory
 *
 * @param dir String representing the directory to list recursively
 * @return Vector of Strings representing the listed files
 */
std::vector<std::string> Utils::listFilesInDirectory(const std::string& dir)
{

    // Create a return vector
    std::vector<std::string> retVect;

    // List all of the files in the the specified directory
    auto listedHeaders = ExecShell::exec("find " + dir + " -type f");
    if (!listedHeaders.empty())
    {

        // Setup string-stream for file-listing
        std::string headerListing;
        std::stringstream stringStream(listedHeaders);

        // Split the listing into parts using newline characters
        // and add the individual listings to the output vector
        while(std::getline(stringStream, headerListing, '\n'))
            retVect.push_back(headerListing);

        // Sort the results once we have them
        if (!retVect.empty())
            std::sort(retVect.begin(), retVect.end());
    }

    // Return the return vector
    return retVect;
}

/**
 * Function used to split the given string into a vector of strings based on the delimiter given
 *
 * @param stringToSplit String to split on the delimiter
 * @param delimiter char representing the delimiter to split the original string on
 * @return Vector of String representing the parts of the original string split on the delimiter
 */
std::vector<std::string> Utils::splitStringByDelimiter(const std::string& stringToSplit,
        char delimiter)
{

    // Create a vector to return
    std::vector<std::string> stringItems;

    // Iterate a "pointer" over the length of the string to split it
    unsigned long currPtr = 0;
    unsigned long prevPtr = 0;
    while (currPtr < stringToSplit.length())
    {

        // Search for characters which match the delimiter
        if (stringToSplit[currPtr] == delimiter)
        {

            // Sub-string the input string since the last pointer and add
            // it to the output vector
            if (currPtr > prevPtr)
                stringItems.push_back(stringToSplit.substr(prevPtr, (currPtr - prevPtr)));

            // Since we found a delimiter character, keep track of it
            prevPtr = currPtr + 1;
        }

        // Always increment the current pointer
        currPtr++;
    }

    // If we got to the end, and still have a string, add it as well
    if (currPtr > prevPtr)
        stringItems.push_back(stringToSplit.substr(prevPtr, (currPtr - prevPtr)));

    // Return the vector of items
    return stringItems;
}
