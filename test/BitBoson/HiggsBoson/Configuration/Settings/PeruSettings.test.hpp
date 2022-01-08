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

#ifndef HIGGS_BOSON_PERU_SETTINGS_TEST_HPP
#define HIGGS_BOSON_PERU_SETTINGS_TEST_HPP

#include <catch.hpp>
#include <string>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>
#include <BitBoson/HiggsBoson/Configuration/Settings/PeruSettings.h>

using namespace BitBoson;

TEST_CASE ("Generic Peru Settings Test", "[PeruSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/peru-sync").c_str()) == 0);

    // Create a Peru Settings object for testing
    std::string peruFile = "/tmp/higgs-boson/peru.yaml";
    std::string peruSyncDir = "/tmp/higgs-boson/peru-sync";
    auto peruSettings = PeruSettings(peruFile, peruSyncDir);

    // Setup the Peru settings with a git dependency
    REQUIRE (peruSettings.addDependency("miniyaml", PeruSettings::DependencyType::TYPE_GIT));
    REQUIRE (peruSettings.addDependencyProperty("miniyaml", "url", " git://github.com/bitboson-deps/mini-yaml.git"));
    REQUIRE (peruSettings.addDependencyProperty("miniyaml", "rev", "22d3dcf5684a11f9c0508c1ad8b3282a1d888319"));

    // Setup the Peru settings with a curl dependency
    REQUIRE (peruSettings.addDependency("miniyamlcurl", PeruSettings::DependencyType::TYPE_CURL));
    REQUIRE (peruSettings.addDependencyProperty("miniyamlcurl", "url", "https://raw.githubusercontent.com/bitboson-deps/mini-yaml/22d3dcf5684a11f9c0508c1ad8b3282a1d888319/README.md"));

    // Run a Peru sync to download the dependency
    REQUIRE (peruSettings.peruSync());

    // Verify the Peru file was written properly
    std::string peruHash = "be6c9fe712c264ca3f352fa599f978a0736df44c30637123abe5f634f1d56c9f";
    REQUIRE (ExecShell::exec("sha256sum " + peruFile) == (peruHash + "  " + peruFile + "\n"));

    // Validate that the repo exists as expected
    std::string headerPath = peruSyncDir + "/miniyaml/yaml/Yaml.hpp";
    std::string headerHash = "e645fe459683c91a61624527e9f460c1a62bf8a1ff9ea47cb663dc72d207ad9a";
    REQUIRE (ExecShell::exec("sha256sum " + headerPath) == (headerHash + "  " + headerPath + "\n"));

    // Validate that the curl cloned file exists as expected
    std::string readmePath = peruSyncDir + "/miniyamlcurl/README.md";
    std::string readmeHash = "b02a5f7b695ea11871b668f6aaebc88c97d7218d652efdee20e8c5ebfb9717a0";
    REQUIRE (ExecShell::exec("sha256sum " + readmePath) == (readmeHash + "  " + readmePath + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/peru.yaml").c_str()) == 0);
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/peru-sync").c_str()) == 0);
}

TEST_CASE ("Invalid Peru Settings Test", "[PeruSettingsTest]")
{

    // Ensure that the directories we'll be using exists
    REQUIRE (system(std::string("mkdir -p /tmp/higgs-boson/peru-sync").c_str()) == 0);

    // Create a Peru Settings object for testing
    std::string peruFile = "/tmp/higgs-boson/peru.yaml";
    std::string peruSyncDir = "/tmp/higgs-boson/peru-sync";
    auto peruSettings = PeruSettings(peruFile, peruSyncDir);

    // Attempt to add a property to a dependency which doesn't yet exist
    REQUIRE ((!peruSettings.addDependencyProperty("curl", "url", "random-value")));

    // Setup the Peru settings with some dependencies
    REQUIRE (peruSettings.addDependency("miniyaml", PeruSettings::DependencyType::TYPE_GIT));
    REQUIRE (peruSettings.addDependencyProperty("miniyaml", "url", " git://github.com/bitboson-deps/mini-yaml.git"));
    REQUIRE (peruSettings.addDependencyProperty("miniyaml", "rev", "22d3dcf5684a11f9c0508c1ad8b3282a1d888319"));

    // Attempt to re-add a value which already exists
    REQUIRE ((!peruSettings.addDependencyProperty("miniyaml", "rev", "bad-revision-value")));

    // Validate we can still run a Peru-sync properly
    REQUIRE (peruSettings.peruSync());
    std::string headerPath = peruSyncDir + "/miniyaml/yaml/Yaml.hpp";
    std::string headerHash = "e645fe459683c91a61624527e9f460c1a62bf8a1ff9ea47cb663dc72d207ad9a";
    REQUIRE (ExecShell::exec("sha256sum " + headerPath) == (headerHash + "  " + headerPath + "\n"));

    // Add-in a bad dependency property to fail the sync
    REQUIRE (peruSettings.addDependencyProperty("miniyaml", "bad-prop", "bad-value"));
    REQUIRE (!peruSettings.peruSync());

    // Verify the Peru file is the same as before
    REQUIRE (ExecShell::exec("sha256sum " + headerPath) == (headerHash + "  " + headerPath + "\n"));

    // Cleanup the temporary files
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/peru.yaml").c_str()) == 0);
    REQUIRE (system(std::string("rm -rf /tmp/higgs-boson/peru-sync").c_str()) == 0);
}

#endif //HIGGS_BOSON_PERU_SETTINGS_TEST_HPP
