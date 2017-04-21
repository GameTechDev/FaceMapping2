/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "CPUTParser.h"
#include <vector>
#include <sstream>

//
// The Command Parser class will retrieve values from a string and store them internally.
// An application can then query for different parameters. All of the query functions are
// guaranteed to not modify the return variable if the parameter was not found.
//

CommandParser::~CommandParser()
{
    CleanConfigurationOptions();
}

void CommandParser::ParseConfigurationOptions(int argc, char **argv, std::string delimiter)
{
    std::string commandLine;
    for (int i = 0; i < argc; i++) {
        commandLine.append(argv[i]);
        commandLine.append(" ");
    }

    ParseConfigurationOptions(commandLine, delimiter);
}

//
// This function parses configuration options from a text string. Removes any previous
// options stored in the configuration list.
//
void CommandParser::ParseConfigurationOptions(std::string arguments, std::string delimiter)
{
    CleanConfigurationOptions();

    std::vector<std::string> argumentList;

    size_t pos;
    size_t nextPos = arguments.find(delimiter.c_str(), 0);
    if (nextPos > 0) {
        AddParameter("default", arguments.substr(0, nextPos));
    }

    //
    // Break out parameters from command line
    //
    while (nextPos != std::string::npos) {
        pos = nextPos + delimiter.length();
        nextPos = arguments.find(delimiter.c_str(), pos);
        argumentList.push_back(arguments.substr(pos, nextPos - pos));
    }

    //
    // Remove leading spaces from arguments.
    //
    for (std::vector<std::string>::iterator it = argumentList.begin(); it != argumentList.end(); it++) {
        std::string::size_type pos = it->find_first_not_of(' ');
        if (pos != std::string::npos) {
            it->erase(0, pos);
        }
    }

    //
    // Remove trailing spaces from arguments
    //
    for (std::vector<std::string>::iterator it = argumentList.begin(); it != argumentList.end(); it++) {
        std::string::size_type pos = it->find_last_not_of(' ');
        if (pos != std::string::npos) {
            it->erase(pos + 1);
        }
    }

    //
    // Split the values from the parameter name
    //
    std::string arg;
    for (std::vector<std::string>::iterator it = argumentList.begin(); it != argumentList.end(); it++) {
        arg = *it;
        pos = arg.find_first_of(":", 0);
        if (pos != std::string::npos) {
            m_Arguments.insert(std::make_pair(arg.substr(0, pos), arg.substr(pos + 1, std::string::npos)));
        } else {
            m_Arguments.insert(std::make_pair(arg.substr(0, pos), ""));
        }
    }

    return;
}

void CommandParser::CleanConfigurationOptions(void)
{
    m_Arguments.clear();

    return;
}

bool CommandParser::GetParameter(std::string arg)
{
    std::map<std::string, std::string>::iterator it;

    it = m_Arguments.find(arg);
    if (it == m_Arguments.end())
        return false;

    return true;
}

bool CommandParser::GetParameter(std::string arg, int *pOut)
{
    std::map<std::string, std::string>::iterator it;
    std::stringstream ss;

    it = m_Arguments.find(arg);
    if (it == m_Arguments.end())
        return false;

    ss << it->second;
    ss >> *pOut;

    return true;
}

bool CommandParser::GetParameter(std::string arg, double *pOut)
{
    std::map<std::string, std::string>::iterator it;
    std::stringstream ss;

    it = m_Arguments.find(arg);
    if (it == m_Arguments.end())
        return false;

    ss << it->second;
    ss >> *pOut;

    return true;
}

bool CommandParser::GetParameter(std::string arg, unsigned int *pOut)
{
    std::map<std::string, std::string>::iterator it;
    std::stringstream ss;

    it = m_Arguments.find(arg);
    if (it == m_Arguments.end())
        return false;

    ss << it->second;
    ss >> *pOut;

    return true;
}

bool CommandParser::GetParameter(std::string arg, std::string *pOut)
{
    std::map<std::string, std::string>::iterator it;
    std::stringstream ss;

    it = m_Arguments.find(arg);
    if (it == m_Arguments.end())
        return false;

    ss << it->second;
    *pOut = ss.str();

    return true;
}

// buffer pointed to by pOut must be large enough for data
bool CommandParser::GetParameter(std::string arg, char *pOut)
{
    std::map<std::string, std::string>::iterator it;
    std::stringstream ss;

    it = m_Arguments.find(arg);
    if (it == m_Arguments.end())
        return false;

    ss << it->second;

    ss >> pOut;

    return true;
}

void CommandParser::AddParameter(std::string paramName, std::string paramValue)
{
    m_Arguments.insert(std::make_pair(paramName, paramValue));
}
