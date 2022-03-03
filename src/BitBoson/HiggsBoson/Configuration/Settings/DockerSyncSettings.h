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

#ifndef HIGGS_BOSON_DOCKER_SYNC_SETTINGS_H
#define HIGGS_BOSON_DOCKER_SYNC_SETTINGS_H

#include <string>

namespace BitBoson
{

    class DockerSyncSettings
    {

        // Private member variables
        private:
            std::string _projectDir;
            std::string _projectCacheDir;
            std::string _globalCacheDir;
            std::string _projectDirHash;
            std::string _containerName;

        // Public member functions
        public:

            /**
             * Constructor used to setup the docker-sync settings object instance
             *
             * @param projectDir String representing the project directory
             * @param projectCacheDir String representing the project cache directory
             * @param projectDirHash String representing the project directory hash
             * @param globalCacheDir String representing the global cache directory
             */
            DockerSyncSettings(const std::string& projectDir,
                    const std::string& projectCacheDir, const std::string& projectDirHash,
                    const std::string& globalCacheDir);

            /**
             * Function used to get the docker-sync volume
             *
             * @return String representing the docker-sync volume
             */
            std::string getVolume();

            /**
             * Function used to force a docker-sync operation
             * NOTE: This is a blocking operation to wait for the sync
             *
             * @return Boolean indcating whether the operation was performed
             */
            bool sync();

            /**
             * Destructor used to cleanup the instance
             */
            virtual ~DockerSyncSettings() = default;

        // Private member functions
        private:

            /**
             * Internal function used to write the internal state to the configured
             * docker-sync file on disk
             *
             * @param fileLocation String representing the location to store the file
             * @return Boolean indicating whether the docker-sync file was written
             */
            bool writeDockerSyncFile(const std::string& fileLocation);

            /**
             * Internal function used to remove orphaned/abandoned sync-containers
             */
            void removeOrphanedSyncContainers();
    };
}

#endif //HIGGS_BOSON_DOCKER_SYNC_SETTINGS_H
