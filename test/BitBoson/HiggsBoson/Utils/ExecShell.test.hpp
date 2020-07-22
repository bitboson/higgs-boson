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

#ifndef HIGGS_BOSON_EXEC_SHELL_TEST_HPP
#define HIGGS_BOSON_EXEC_SHELL_TEST_HPP

#include <catch.hpp>
#include <BitBoson/HiggsBoson/Utils/ExecShell.h>

using namespace BitBoson;

TEST_CASE ("General Exec-Shell Test", "[ExecShellTest]")
{

    // Simply run a echo Hello World and ensure the response is
    // "Hello World" accordingly
    REQUIRE (ExecShell::exec("echo Hello World") == "Hello World\n");
}

TEST_CASE ("General Exec-Live Test", "[ExecShellTest]")
{

    // Simply run a echo Hello World and ensure the response is
    // "Hello World" accordingly
    REQUIRE (ExecShell::execLive("echo Hello World"));
}

TEST_CASE ("Execute with Response Exec-Shell Test", "[ExecShellTest]")
{

    // Validate that a successful Hello World message has the expected response
    REQUIRE (ExecShell::execWithResponse("Test Good Message", "echo Hello World"));
    REQUIRE (ExecShell::previousPrintedResponse == "Test Good Message ... OK\n");

    // Validate that an unsuccessful command has the expected response
    REQUIRE (!ExecShell::execWithResponse("Test Bad Message", "bad-cli-run"));
    REQUIRE (ExecShell::previousPrintedResponse == "Test Bad Message ... FAIL\nsh: 1: bad-cli-run: not found\n\n");
}

#endif //HIGGS_BOSON_EXEC_SHELL_TEST_HPP
