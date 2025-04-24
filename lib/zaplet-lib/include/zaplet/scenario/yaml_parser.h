/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include "zaplet/logging/logger.h"
#include "zaplet/scenario/scenario.h"

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace zaplet::scenario
{
    class YamlParser
    {
    public:
        YamlParser() = default;
        ~YamlParser() = default;

        Scenario parseFile(const std::string& filePath) const;
        Scenario parseString(const std::string& yamlContent) const;

    private:
        Scenario parseScenario(const YAML::Node& node) const;
        Step parseStep(const YAML::Node& node) const;
        http::Request parseRequest(const YAML::Node& node) const;
        http::Response parseResponse(const YAML::Node& node) const;
    };
} // namespace zaplet::scenario

#endif // YAML_PARSER_H
