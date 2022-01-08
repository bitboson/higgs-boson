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

#ifndef HIGGS_BOSON_FILE_WRITER_H
#define HIGGS_BOSON_FILE_WRITER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace BitBoson
{

    class FileWriter
    {

        // Private member variables
        private:
            bool _isClosed;
            bool _isContainer;
            std::string _filePath;
            std::ofstream _localFile;

        // Public member functions
        public:

            /**
             * Constructor used to setup the File-Writer instance on the given file
             *
             * @param fileToWriteTo String representing the file to write to
             * @param forceLocal Boolean indicating whether to forcibly write locally
             */
            FileWriter(const std::string& fileToWriteTo, bool forceLocal=false);

            /**
             * Function used to get whether the file is open
             *
             * @return Boolean indicating whether the file is open
             */
            bool isOpen();

            /**
             * Function used to write the provided text to the file
             *
             * @param textToWrite String representing the text to write
             */
            void write(const std::string& textToWrite);

            /**
             * Function used to write the provided line to the file
             * NOTE: This adds a new-line character to the end
             *
             * @param lineToWrite String representing the line to write
             */
            void writeLine(const std::string& lineToWrite);

            /**
             * Function used to close the current file
             */
            void close();

            /**
             * Destructor used to cleanup the instance
             */
            virtual ~FileWriter();
    };
}

#endif //HIGGS_BOSON_FILE_WRITER_H
