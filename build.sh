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
set -e

# Get the current directory for reference
HIGGS_BOSON_MANUAL_ORIG_DIR=$(pwd)

# Setup the manual temporary build directory
rm -rf $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/external/raw
mkdir -p $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/external/raw
cd $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/external/raw

# Clone the external dependencies into the corresponding directory
git clone git://github.com/bitboson-deps/mini-yaml.git
git clone git://github.com/bitboson-deps/picosha2.git

# Build the mini-yaml dependency and copy the artifacts
cd $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/external/raw/mini-yaml
mkdir -p $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/output/manual/miniyaml
rm -rf ./higgs-boson_default_headers && mkdir -p ./higgs-boson_default_headers
clang++ -D_GLIBCXX_USE_CXX11_ABI=0 -std=c++17 -stdlib=libstdc++ -fPIC -shared yaml/Yaml.cpp -o libminiyaml.so
cp -r ./libminiyaml.so $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/output/manual/miniyaml/
cp -r ./yaml ./higgs-boson_default_headers

# Build the picosha2 dependency and copy the artifacts
cd $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/external/raw/picosha2
mkdir -p $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/output/manual/picosha2
rm -rf ./higgs-boson_default_headers && mkdir -p ./higgs-boson_default_headers/picosha2
cp -r ./picosha2.h ./higgs-boson_default_headers/picosha2

# Build Steps for the Compile operation for target default
mkdir -p $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/builds/manual
cd $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/builds/manual
cmake -DHIGGS_BOSON_MANUAL_DIR=$HIGGS_BOSON_MANUAL_ORIG_DIR -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release $HIGGS_BOSON_MANUAL_ORIG_DIR

# Run the Make operation to build the binary file itself
cd $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/builds/manual && make higgs-boson

# Create the output directory and copy the artifacts to it
mkdir -p $HIGGS_BOSON_MANUAL_ORIG_DIR/output/manual
mkdir -p $HIGGS_BOSON_MANUAL_ORIG_DIR/output/manual/bin
mkdir -p $HIGGS_BOSON_MANUAL_ORIG_DIR/output/manual/deps
cp $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/output/manual/miniyaml/* $HIGGS_BOSON_MANUAL_ORIG_DIR/output/manual/deps
cp $HIGGS_BOSON_MANUAL_ORIG_DIR/.higgs-boson/builds/manual/bin/higgs-boson $HIGGS_BOSON_MANUAL_ORIG_DIR/output/manual/bin
