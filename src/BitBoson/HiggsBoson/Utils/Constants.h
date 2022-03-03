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

#ifndef HIGGS_BOSON_CONSTANTS_H
#define HIGGS_BOSON_CONSTANTS_H

#include <string>
#include <vector>

namespace BitBoson::Constants
{

    // Define Docker-container related constants
    const std::string DOCKER_HIGGS_BUILDER_PREFIX = "bitbosonhiggsbosonbuilderprocess-";
    const std::string DOCKER_SYNC_PREFIX = "higgsboson-sync-";

    /**
     * Function used to get a list (vector) of valid image targets
     *
     * @return Vector of Strings representing the valid images available
     */
    std::vector<std::string> getValidImages();

    /**
     * Function used to get the OS-level name for the given target/image triple
     * NOTE: Can be darwin, windows, linux, and android
     *
     * @param targetTriple String representing the target/image triple
     * @return String representing the os-level representation
     */
    std::string getTargetOsForImageTriple(const std::string& targetTriple);
}

#endif //HIGGS_BOSON_CONSTANTS_H
