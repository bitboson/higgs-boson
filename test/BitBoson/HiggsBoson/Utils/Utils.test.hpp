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

#ifndef HIGGS_BOSON_UTILS_TEST_HPP
#define HIGGS_BOSON_UTILS_TEST_HPP

#include <catch.hpp>
#include <BitBoson/HiggsBoson/Utils/Utils.h>

using namespace BitBoson;

/**
 * Test helper function used to compare two vector's contents
 *
 * @param vector1 Vector of Strings representing one of the two vectors to compare
 * @param vector2 Vector of Strings representing one of the two vectors to compare
 * @return Boolean indicating whether the two vectors are the same or not
 */
bool compareFileVectors(const std::vector<std::string>& vector1, const std::vector<std::string>& vector2)
{

    // Create a retrun flag
    bool retFlag = false;

    // Only continue if the vectors are the same size
    if (vector1.size() == vector2.size())
    {

        // Determine if the contents match
        bool contentsMatch = true;
        for (long ii = 0; ii < vector1.size(); ii++)
            if (vector1[ii] != vector2[ii])
                contentsMatch = false;

        // Setup the return value based on the matched contents
        retFlag = contentsMatch;
    }

    // Return the return flag
    return retFlag;
}

TEST_CASE ("Recursively List Files Test", "[UtilsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/utils-test").c_str()) == 0);

    // Setup the test files for listing
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/utils-test/abc/def/ghi").c_str()) == 0);
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/utils-test/zzzzzzzzzzzzzzzz").c_str()) == 0);
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/utils-test/blah-blah-blah").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/utils-test/f1").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/utils-test/f2").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/utils-test/f3.txt").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/utils-test/abc/def/ghi/f3.txt").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/utils-test/zzzzzzzzzzzzzzzz/f1.txt").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/utils-test/zzzzzzzzzzzzzzzz/f1.xml").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/utils-test/zzzzzzzzzzzzzzzz/f1.csv").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/utils-test/blah-blah-blah/f9").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/utils-test/blah-blah-blah/f8").c_str()) == 0);
    REQUIRE (system(std::string("touch /tmp/higgs-boson/utils-test/blah-blah-blah/f7").c_str()) == 0);

    // Validate that the listing operation returns the expected files
    REQUIRE (compareFileVectors(Utils::listFilesInDirectory("/tmp/higgs-boson/utils-test"),
            {
                "/tmp/higgs-boson/utils-test/abc/def/ghi/f3.txt",
                "/tmp/higgs-boson/utils-test/blah-blah-blah/f7",
                "/tmp/higgs-boson/utils-test/blah-blah-blah/f8",
                "/tmp/higgs-boson/utils-test/blah-blah-blah/f9",
                "/tmp/higgs-boson/utils-test/f1",
                "/tmp/higgs-boson/utils-test/f2",
                "/tmp/higgs-boson/utils-test/f3.txt",
                "/tmp/higgs-boson/utils-test/zzzzzzzzzzzzzzzz/f1.csv",
                "/tmp/higgs-boson/utils-test/zzzzzzzzzzzzzzzz/f1.txt",
                "/tmp/higgs-boson/utils-test/zzzzzzzzzzzzzzzz/f1.xml"
            }));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/utils-test").c_str()) == 0);
}

TEST_CASE ("Split String By Delimiter Test", "[UtilsTest]")
{

    // Create the test string to split
    auto stringToSplit = "Hello,World,This,Is,Awesome!";

    // Split the string into its parts
    auto splitString = Utils::splitStringByDelimiter(stringToSplit, ',');

    // Verify the string parts were split properly
    REQUIRE (splitString.size() == 5);
    REQUIRE (splitString[0] == "Hello");
    REQUIRE (splitString[1] == "World");
    REQUIRE (splitString[2] == "This");
    REQUIRE (splitString[3] == "Is");
    REQUIRE (splitString[4] == "Awesome!");

    // Split the string into its parts (not really)
    splitString = Utils::splitStringByDelimiter(stringToSplit, '~');

    // Verify the string wasn't split into parts
    REQUIRE(splitString.size() == 1);
    REQUIRE(splitString[0] == "Hello,World,This,Is,Awesome!");

    // Create a new test string to split
    stringToSplit = ",,,Hello,,,,,,,,,World,This,,,Is,,,,,Awesome!,,,,,";

    // Split the string into its parts
    splitString = Utils::splitStringByDelimiter(stringToSplit, ',');

    // Verify the string parts were split properly
    REQUIRE (splitString.size() == 5);
    REQUIRE (splitString[0] == "Hello");
    REQUIRE (splitString[1] == "World");
    REQUIRE (splitString[2] == "This");
    REQUIRE (splitString[3] == "Is");
    REQUIRE (splitString[4] == "Awesome!");
}

#endif //HIGGS_BOSON_UTILS_TEST_HPP
