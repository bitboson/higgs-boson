# This file is part of higgs-boson.
#
# Copyright (c) BitBoson
#
# higgs-boson is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# higgs-boson is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with higgs-boson.  If not, see <https://www.gnu.org/licenses/>.
#
# Written by:
#     - Tyler Parcell <OriginLegend>
#

# Cleanup the current environment (removes all temporary files)
clean:
	rm -rf .higgs-boson/
	rm -rf output/

# Build the builder docker container and ssh-into it for easy building
docker_ssh: clean
	docker build -f ./docker/Dockerfile -t higgs-boson-project-build-image .
	docker run -v $(CURDIR):$(CURDIR) -w $(CURDIR) -i -t higgs-boson-project-build-image bash

# Setup the actual build command (without higgs-boson installed)
build: clean
	bash build.sh

# Setup the build command (assuming manual build is complete)
build2: build
	LD_LIBRARY_PATH="output/manual/deps" output/manual/bin/higgs-boson download
	LD_LIBRARY_PATH="output/manual/deps" output/manual/bin/higgs-boson build-deps
	LD_LIBRARY_PATH="output/manual/deps" output/manual/bin/higgs-boson build

# Setup the target to run coverage tests
coverage: build2
	LD_LIBRARY_PATH="output/manual/deps" output/manual/bin/higgs-boson coverage

# Setup the target to run the address sanitizer
sanitize_address: build2
	LD_LIBRARY_PATH="output/manual/deps" output/manual/bin/higgs-boson sanitize address

# Setup the target to run the behavior sanitizer
sanitize_behavior: build2
	LD_LIBRARY_PATH="output/manual/deps" output/manual/bin/higgs-boson sanitize behavior

# Setup the target to run the leak sanitizer
sanitize_leak: build2
	LD_LIBRARY_PATH="output/manual/deps" output/manual/bin/higgs-boson sanitize leak

# Setup the target to run the thread sanitizer
sanitize_thread: build2
	LD_LIBRARY_PATH="output/manual/deps" output/manual/bin/higgs-boson sanitize thread
