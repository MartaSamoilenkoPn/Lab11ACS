//
// Created by kaba4ok on 12.04.24.
//
#include "../include/config_parser.h"
#include <fstream>
#include "exceptions.h"

void ClearLeadingAndTrailingCharacter(std::string& strToClean, char charToRemove)
{
    auto first = strToClean.find_first_not_of(charToRemove);
    auto last = strToClean.find_last_not_of(charToRemove);

    if(first == std::string::npos)
    {
        first = 0;
    }

    if(last == std::string::npos)
    {
        last = strToClean.size()-1;
    }

    strToClean = strToClean.substr(first, last-first + 1);
}

std::unordered_map<std::string, std::vector<std::string>> ConfigurationParser::ParseConfiguration()
{
    if(!exists(configPath))
    {
        throw OpenConfigFileException();
    }

    std::unordered_map<std::string, std::vector<std::string>> config;
    std::ifstream configReader{};
    try
    {
        configReader.open(configPath);
    } catch (const std::ios_base::failure& e)
    {
        throw OpenConfigFileException();
    }

    std::string newLine;
    while(std::getline(configReader, newLine))
    {
        auto commentIdx = newLine.find_first_of('#');
        if (commentIdx != std::string::npos) {
            newLine.erase(commentIdx);
        }

        auto sep = newLine.find_first_of('=');
        if (sep == std::string::npos) {
            throw ReadConfigFileException();
        }
        std::string key = newLine.substr(0, sep);
        std::string value = newLine.substr(sep + 1, newLine.size() - sep);

        ClearLeadingAndTrailingCharacter(key, ' ');
        ClearLeadingAndTrailingCharacter(value, ' ');

        ClearLeadingAndTrailingCharacter(key, '"');
        ClearLeadingAndTrailingCharacter(value, '"');

        if (!config.contains(key)) {
            config.insert(std::make_pair(key, std::vector<std::string>{}));
        }

        config[key].emplace_back(value);
        newLine.clear();
    }

    if((configReader.fail() || configReader.bad()) && !configReader.eof())
    {
        throw ReadConfigFileException();
    }

    return std::move(config);
}