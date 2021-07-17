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

#
# This dockerfile can be used to setup an environment for manually
# building higgs-boson (not for higgs-boson to use though)
#

# Use Ubuntu Linux (18.04) as the image
FROM ubuntu:18.04

# Update/Upgrade the image
RUN apt-get update -y
RUN apt-get upgrade -y

# Install all dependencies from APT-GET
RUN apt-get install -y cmake
RUN apt-get install -y gcc g++
RUN apt-get install -y make
RUN apt-get install -y git
RUN apt-get install -y libc-dev
RUN apt-get install -y autoconf
RUN apt-get install -y libtool
RUN apt-get install -y linux-headers-generic
RUN apt-get install -y clang-6.0
RUN apt-get install -y llvm

# Setup the aliases for clang to be used properly
RUN cp /usr/bin/clang-6.0 /usr/bin/clang
RUN cp /usr/bin/clang++-6.0 /usr/bin/clang++

# Create higgs-boson user and group
RUN useradd -d /home/higgs-boson -ms /bin/bash higgs-boson
RUN echo 'higgs-boson:password' | chpasswd
RUN adduser higgs-boson sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER higgs-boson
