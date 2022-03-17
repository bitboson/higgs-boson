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

#include <BitBoson/HiggsBoson/HiggsBoson.h>
#include <BitBoson/HiggsBoson/Utils/FileWriter.h>

using namespace BitBoson;

/**
 * Constructor used to setup the File-Writer instance on the given file
 *
 * @param fileToWriteTo String representing the file to write to
 * @param forceLocal Boolean indicating whether to forcibly write locally
 */
FileWriter::FileWriter(const std::string& fileToWriteTo, bool forceLocal)
{

    // Setup member variables
    _isContainer = HiggsBoson::RunTypeSingleton::isRunningCommandsInContainer();
    _filePath = fileToWriteTo;
    _isClosed = false;

    // Force local writes (if applicable)
    if (forceLocal)
        _isContainer = false;

    // Open the file based on where we are operating
    if (_isContainer)
        HiggsBoson::RunTypeSingleton::executeInContainer("rm -rf " + _filePath);
    _localFile.open(_filePath);

    // Determine if the file is open
    _isClosed = !_localFile.is_open();
}

/**
 * Function used to get whether the file is open
 *
 * @return Boolean indicating whether the file is open
 */
bool FileWriter::isOpen()
{

    // Simply return if the file is open
    return !_isClosed;
}

/**
 * Function used to write the provided text to the file
 *
 * @param textToWrite String representing the text to write
 */
void FileWriter::write(const std::string& textToWrite)
{

    // Only handle if the file is not closed
    if (!_isClosed)
    {

        // Write the actual text to the file
        _localFile << textToWrite;
    }
}

/**
 * Function used to write the provided line to the file
 * NOTE: This adds a new-line character to the end
 *
 * @param lineToWrite String representing the line to write
 */
void FileWriter::writeLine(const std::string& lineToWrite)
{

    // Only handle if the file is not closed
    if (!_isClosed)
    {

        // Write the actual line to the file
        _localFile << lineToWrite << std::endl;
    }
}

/**
 * Function used to close the current file
 */
void FileWriter::close()
{

    // Only handle if the file is not closed
    if (!_isClosed)
    {

        // Start by closing the locally written file
        _isClosed = true;
        _localFile.close();

        // Copy the file to the container if necessary
        if (_isContainer)
            ExecShell::exec("docker cp " + _filePath + " "
                    + FileWriterConfigSingleton::getDockerContainerName() + ":" + _filePath);
    }
}

/**
 * Destructor used to cleanup the instance
 */
FileWriter::~FileWriter()
{

    // Ensure that the file is closed
    close();
}
