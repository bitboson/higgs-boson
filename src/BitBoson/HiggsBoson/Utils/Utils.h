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

#ifndef HIGGS_BOSON_UTILS_H
#define HIGGS_BOSON_UTILS_H

#include <string>
#include <vector>

namespace BitBoson::Utils
{

    /**
     * Function used to list the files (recursively) in the given
     * directory
     *
     * @param dir String representing the directory to list recursively
     * @return Vector of Strings representing the listed files
     */
    std::vector<std::string> listFilesInDirectory(const std::string& dir);

    /**
     * Function used to split the given string into a vector of strings based on the delimiter given
     *
     * @param stringToSplit String to split on the delimiter
     * @param delimiter char representing the delimiter to split the original string on
     * @return Vector of String representing the parts of the original string split on the delimiter
     */
    std::vector<std::string> splitStringByDelimiter(const std::string& stringToSplit, char delimiter);

}

#endif //HIGGS_BOSON_UTILS_H
