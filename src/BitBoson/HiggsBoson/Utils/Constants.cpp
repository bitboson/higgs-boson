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


#include <set>
#include <BitBoson/HiggsBoson/Utils/Constants.h>

using namespace BitBoson;

/**
 * Function used to get a list (vector) of valid image targets
 *
 * @return Vector of Strings representing the valid images available
 */
std::vector<std::string> Constants::getValidImages()
{

    // Return the list (vector) of valid images
    return {

            // Android-based targets
            "android-arm",
            "android-arm64",

            // Linux-based targets
            "linux-arm64",
            "linux-armv5-musl",
            "linux-armv5",
            "linux-armv6",
            "linux-armv7",
            "linux-armv7a",
            "linux-mips",
            "linux-mipsel",
            "linux-ppc64le",
            "linux-s390x",
            "linux-x64",
            "linux-x86",
            "manylinux-common",
            "manylinux1-x64",
            "manylinux1-x86",
            "manylinux2010-x64",
            "manylinux2010-x86",
            "manylinux2014-aarch64",
            "manylinux2014-x64",
            "manylinux2014-x86",

            // Web-based targets
            "web-wasm",

            // Windows-based targets
            "windows-shared-x64-posix",
            "windows-shared-x64",
            "windows-shared-x86",
            "windows-static-x64-posix",
            "windows-static-x64",
            "windows-static-x86",

            // Darwin-based targets
            "apple-darwin-x64",
            "apple-darwin-x86",
            "apple-darwin-arm64"
    };
}

/**
 * Function used to get the OS-level name for the given target/image triple
 * NOTE: Can be darwin, windows, linux, and android
 *
 * @param targetTriple String representing the target/image triple
 * @return String representing the os-level representation
 */
std::string Constants::getTargetOsForImageTriple(const std::string& targetTriple)
{

    // Create a return string
    std::string returnOsName;

    // Check if the target is android
    std::set<std::string> androidSet = {"android-arm",
                                        "android-arm64"};
    if (androidSet.find(targetTriple) != androidSet.end()) {
        returnOsName = "android";
    }

    // Check if the target is linux
    std::set<std::string> linuxSet = {"default",
                                      "linux-arm64",
                                      "linux-armv5-musl",
                                      "linux-armv5",
                                      "linux-armv6",
                                      "linux-armv7",
                                      "linux-armv7a",
                                      "linux-mips",
                                      "linux-mipsel",
                                      "linux-ppc64le",
                                      "linux-s390x",
                                      "linux-x64",
                                      "linux-x86",
                                      "manylinux-common",
                                      "manylinux1-x64",
                                      "manylinux1-x86",
                                      "manylinux2010-x64",
                                      "manylinux2010-x86",
                                      "manylinux2014-aarch64",
                                      "manylinux2014-x64",
                                      "manylinux2014-x86"};
    if (linuxSet.find(targetTriple) != linuxSet.end()) {
        returnOsName = "linux";
    }

    // Check if the target is web
    std::set<std::string> webSet = {"web-wasm"};
    if (webSet.find(targetTriple) != webSet.end()) {
        returnOsName = "web";
    }

    // Check if the target is android
    std::set<std::string> windowsSet = {"windows-shared-x64-posix",
                                        "windows-shared-x64",
                                        "windows-shared-x86",
                                        "windows-static-x64-posix",
                                        "windows-static-x64",
                                        "windows-static-x86"};
    if (windowsSet.find(targetTriple) != windowsSet.end()) {
        returnOsName = "windows";
    }

    // Check if the target is android
    std::set<std::string> darwinSet = {"apple-darwin-x64",
                                       "apple-darwin-x86",
                                       "apple-darwin-arm64"};
    if (darwinSet.find(targetTriple) != darwinSet.end()) {
        returnOsName = "darwin";
    }

    // Return the return string
    return returnOsName;
}
