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
#include <cctype>
#include <locale>
#include <memory>
#include <string>
#include <array>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <stdexcept>
#include <algorithm>
#include <picosha2/picosha2.h>
#include <BitBoson/HiggsBoson/HiggsBoson.h>
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
    auto listedFiles = HiggsBoson::RunTypeSingleton::executeInContainerWithResponse(
            "find " + dir + " -type f");
    if (!listedFiles.empty())
    {

        // Setup string-stream for file-listing
        std::string fileListing;
        std::stringstream stringStream(listedFiles);

        // Split the listing into parts using newline characters
        // and add the individual listings to the output vector
        while(std::getline(stringStream, fileListing, '\n'))
            retVect.push_back(trim(fileListing));

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

/**
 * Function used to get the current executing user's home-path on disk
 *
 * @return String representation of the user's home-path
 */
std::string Utils::getHomePath()
{

    // Setup the return string
    std::string retString;

    // Start by trying to get the user-profile variable
    // This should be defined for Windows only
    const char* userProfileValue = getenv("USERPROFILE");
    if (userProfileValue != NULL)
        retString = std::string(userProfileValue);

    // If we still haven't figured out the home directory
    // next try using the HOMEDRIVE and HOMEPATH variables
    if (retString.empty()) {
        const char* homeDriveValue = getenv("HOMEDRIVE");
        const char* homePathValue = getenv("HOMEPATH");
        if ((homeDriveValue != NULL) && (homePathValue != NULL))
            retString = std::string(homeDriveValue) + std::string(homePathValue);
    }

    // If we still haven't figured out the home directory
    // next we can then try to get the HOME variable
    if (retString.empty()) {
        const char* linuxHomeDirValue = getenv("HOME");
        if (linuxHomeDirValue != NULL)
            retString = std::string(linuxHomeDirValue);
    }

    // If we still haven't figured out the home directory
    // next we can then try to run the "pwd" command
    if (retString.empty()) {
        std::string pwdValue = HiggsBoson::RunTypeSingleton::executeInContainerWithResponse("pwd");
        if (!pwdValue.empty())
            retString = Utils::splitStringByDelimiter(pwdValue, '\n').front();
    }

    // Return the return string
    return retString;
}

/**
 * Function used to get the SHA256 hash of the given string in a hex format
 *
 * @param data String to get the hash of
 * @return String representing the hashed value of the given data
 */
std::string Utils::sha256(const std::string& data)
{

    // Create the destination vector and calculate the hash
    std::vector<unsigned char> hash(picosha2::k_digest_size);
    picosha2::hash256(data.begin(), data.end(), hash.begin(), hash.end());

    // Calculate the hex-form of the SHA256 hash and return it
    return picosha2::bytes_to_hex_string(hash.begin(), hash.end());
}

/**
 * Function used to trim the provided string (in-place)
 * NOTE: We also return the string for convenience
 *
 * @param stringToTrim String to trim (in-place)
 * @return String representing the trimmed string
 */
std::string Utils::trim(std::string &stringToTrim)
{

    // Define the special characters to trim
    const char* badChars = " \t\n\r\f\v";

    // Perfrom both right-to-left and left-to-right trimming
    stringToTrim.erase(stringToTrim.find_last_not_of(badChars) + 1);
    stringToTrim.erase(0, stringToTrim.find_first_not_of(badChars));

    // Return the trimmed string
    return stringToTrim;
}
