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

#ifndef HIGGS_BOSON_PROJECT_SETTINGS_TEST_HPP
#define HIGGS_BOSON_PROJECT_SETTINGS_TEST_HPP

#include <catch.hpp>
#include <string>
#include <BitBoson/HiggsBoson/Configuration/Settings/ProjectSettings.h>

using namespace BitBoson;

TEST_CASE ("Generic Project Settings Test", "[ProjectSettingsTest]")
{

    // Setup a library project settings object
    auto libProject = ProjectSettings("EpicProject", ProjectSettings::ProjectType::TYPE_EXE, "0.1.1",
            "src", "test", "higgs-boson/main.cpp");

    // Verify the values of the project accordingly
    REQUIRE (libProject.getProjectName() == "EpicProject");
    REQUIRE (libProject.getProjectType() == ProjectSettings::ProjectType::TYPE_EXE);
    REQUIRE (libProject.getProjectVersion() == "0.1.1");
    REQUIRE (libProject.getProjectSource() == "src");
    REQUIRE (libProject.getProjectTest() == "test");
    REQUIRE (libProject.getProjectMain() == "higgs-boson/main.cpp");

    // Setup an executable project settings object
    auto exeProject = ProjectSettings("EpicProject2", ProjectSettings::ProjectType::TYPE_LIB, "1.7.3",
            "source", "testing", "higgs-boson/proj.cxx");

    // Verify the values of the project accordingly
    REQUIRE (exeProject.getProjectName() == "EpicProject2");
    REQUIRE (exeProject.getProjectType() == ProjectSettings::ProjectType::TYPE_LIB);
    REQUIRE (exeProject.getProjectVersion() == "1.7.3");
    REQUIRE (exeProject.getProjectSource() == "source");
    REQUIRE (exeProject.getProjectTest() == "testing");
    REQUIRE (exeProject.getProjectMain() == "higgs-boson/proj.cxx");
}

#endif //HIGGS_BOSON_PROJECT_SETTINGS_TEST_HPP
