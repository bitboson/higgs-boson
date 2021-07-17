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

//
// JetBrains Space Automation
// This Kotlin-script file lets you automate build activities
// For more info, see https://www.jetbrains.com/help/space/automation.html
//

// Build the standard builder container (with C++ tools) for Higgs-Boson
job("Build the standard builder container for Higgs-Boson to use") {

    // Only run the job if the Dockerfile changes
    startOn {
        gitPush {
            pathFilter {
                + "docker/Dockerfile"
            }
        }
    }

    // Actually construct the container
    docker {
        build {
            context = "docker"
            file = "./docker/Dockerfile"
            labels["vendor"] = "bitboson"
        }

        push(Secrets("build-tools-internal-registry") + "/higgs-boson-project-build-image") {
            tags("version1.0")
        }
    }
}

// Build the Docker-container for building Higgs-Boson projects
job("Build Higgs-Boson Default Binaries and Builder container") {

    // Build the Default Linux Binaries for the Docker Image
    // Output: /mnt/space/share/higgs-boson
    container(displayName = "Build Default Linux Binaries",
            image = Secrets("build-tools-internal-registry") + "/higgs-boson-project-build-image") {
        shellScript {
            content = """
                make build
                mkdir -p /mnt/space/share/higgs-boson/bin
                mkdir -p /mnt/space/share/higgs-boson/deps
                cp output/manual/bin/* /mnt/space/share/higgs-boson/bin/
                cp output/manual/deps/* /mnt/space/share/higgs-boson/deps/
            """
        }
    }

    // Clone Dockcross to use as the build-image
    // Location: /mnt/space/work/dockcross
    git("dockcross") {
        refSpec = "refs/heads/higgs-boson"
        depth = UNLIMITED_DEPTH
    }

    // Construct the Higgs-Boson container for use in future builds
    docker {
        build {
            context = "/mnt/space/share/higgs-boson"
            file = "/mnt/space/work/dockcross/Dockerfile.higgs-boson.manual"
            labels["vendor"] = "bitboson"
        }

        push(Secrets("build-tools-internal-registry") + "/higgs-boson-builder") {
            tags("version1.0")
        }
    }
}
