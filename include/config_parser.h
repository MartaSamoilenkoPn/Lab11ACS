//
// Created by kaba4ok on 12.04.24.
//

#ifndef COUNTWORDS_SEQ_CONFIG_PARSER_H
#define COUNTWORDS_SEQ_CONFIG_PARSER_H
#include <filesystem>
#include <unordered_map>
#include <utility>
#include <vector>
#include "common.h"

class ConfigurationParser
{
private:
    std::filesystem::path configPath;
public:
    explicit ConfigurationParser(std::filesystem::path configPath):configPath(std::move(configPath)){}
    ConfigurationParser() = delete;

    ConfigurationParser(ConfigurationParser&& parser) = delete;
    ConfigurationParser operator = (ConfigurationParser&& parser) = delete;

    std::unordered_map<std::string, std::vector<std::string>> ParseConfiguration();
};

#endif //COUNTWORDS_SEQ_CONFIG_PARSER_H
