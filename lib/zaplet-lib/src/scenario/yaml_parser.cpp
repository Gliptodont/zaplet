/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/scenario/yaml_parser.h"

namespace zaplet::scenario
{
    Scenario YamlParser::parseFile(const std::string& filePath) const
    {
        try
        {
            if (!std::filesystem::exists(filePath))
            {
                throw std::runtime_error("Scenario file does not exist: " + filePath);
            }

            YAML::Node config = YAML::LoadFile(filePath);
            return parseScenario(config);
        } catch (const YAML::Exception& e)
        {
            throw std::runtime_error("Failed to parse file: " + std::string(e.what()));
        }
    }

    Scenario YamlParser::parseString(const std::string& yamlContent) const
    {
        try
        {
            LOG_DEBUG("Parsing scenario from string");
            YAML::Node config = YAML::Load(yamlContent);
            return parseScenario(config);
        } catch (const YAML::Exception& e)
        {
            throw std::runtime_error("Failed to parse string: " + std::string(e.what()));
        }
    }

    Scenario YamlParser::parseScenario(const YAML::Node& node) const
    {
        Scenario scenario;

        if (node["name"])
        {
            scenario.setName(node["name"].as<std::string>());
        }
        else
        {
            throw std::runtime_error("Scenario must have a name");
        }

        if (node["description"])
        {
            scenario.setDescription(node["description"].as<std::string>());
        }

        if (node["repeat"])
        {
            std::string repeatValue = node["repeat"].as<std::string>();
            if (repeatValue == "infinite" || repeatValue == "inf")
            {
                scenario.setRepeatCount(std::nullopt);
            }
            else
            {
                try
                {
                    scenario.setRepeatCount(std::stoi(repeatValue));
                } catch (const std::exception& e)
                {
                    LOG_WARNING_FMT("Invalid repeat value '{}', defaulting to 1", repeatValue);
                    scenario.setRepeatCount(1);
                }
            }
        }
        else
        {
            scenario.setRepeatCount(1);
        }

        if (node["continue_on_error"])
        {
            scenario.setContinueOnError(node["continue_on_error"].as<bool>());
        }

        if (node["environment"] && node["environment"].IsMap())
        {
            std::map<std::string, std::string> env;
            for (const auto& it : node["environment"])
            {
                std::string key = it.first.as<std::string>();
                std::string value = it.second.as<std::string>();
                env[key] = value;
            }
            scenario.setEnvironment(env);
        }

        if (node["steps"] && node["steps"].IsSequence())
        {
            std::vector<Step> steps;
            for (const auto& stepNode : node["steps"])
            {
                steps.push_back(parseStep(stepNode));
            }
            scenario.setSteps(steps);
        }
        else
        {
            throw std::runtime_error("Scenario must have steps");
        }

        LOG_INFO_FMT("Parsed scenario '{}' with {} steps", scenario.getName(), scenario.getSteps().size());
        return scenario;
    }

    Step YamlParser::parseStep(const YAML::Node& node) const
    {
        Step step;

        if (node["name"])
        {
            step.name = node["name"].as<std::string>();
        }
        else
        {
            throw std::runtime_error("Step must have a name");
        }

        if (node["description"])
        {
            step.description = node["description"].as<std::string>();
        }

        if (node["request"])
        {
            step.request = parseRequest(node["request"]);
        }
        else
        {
            throw std::runtime_error("Step must have a request");
        }

        if (node["expected_response"])
        {
            step.expectedResponse = parseResponse(node["expected_response"]);
        }

        if (node["delay"])
        {
            int delayMs = node["delay"].as<int>();
            step.delay = std::chrono::milliseconds(delayMs);
        }

        if (node["condition"])
        {
            step.condition = node["condition"].as<std::string>();
        }

        if (node["variables"] && node["variables"].IsMap())
        {
            std::map<std::string, std::string> variables;
            for (const auto& it : node["variables"])
            {
                std::string key = it.first.as<std::string>();
                std::string value = it.second.as<std::string>();
                variables[key] = value;
            }
            step.variables = variables;
        }

        LOG_DEBUG_FMT("Parsed step '{}' with method {} to URL {}", step.name, step.request.getMethod(), step.request.getUrl());
        return step;
    }

    http::Request YamlParser::parseRequest(const YAML::Node& node) const
    {
        http::Request request;

        if (node["url"])
        {
            request.setUrl(node["url"].as<std::string>());
        }
        else
        {
            throw std::runtime_error("Request must have a URL");
        }

        if (node["method"])
        {
            request.setMethod(node["method"].as<std::string>());
        }
        else
        {
            request.setMethod("GET");
        }

        if (node["headers"] && node["headers"].IsMap())
        {
            std::map<std::string, std::string> headers;
            for (const auto& it : node["headers"])
            {
                std::string key = it.first.as<std::string>();
                std::string value = it.second.as<std::string>();
                headers[key] = value;
            }
            request.setHeaders(headers);
        }

        if (node["body"])
        {
            request.setBody(node["body"].as<std::string>());
        }

        if (node["query_params"] && node["query_params"].IsMap())
        {
            std::map<std::string, std::string> params;
            for (const auto& it : node["query_params"])
            {
                std::string key = it.first.as<std::string>();
                std::string value = it.second.as<std::string>();
                params[key] = value;
            }
            request.setQueryParams(params);
        }

        if (node["timeout"])
        {
            request.setTimeout(node["timeout"].as<int>());
        }

        return request;
    }

    http::Response YamlParser::parseResponse(const YAML::Node& node) const
    {
        http::Response response;

        if (node["status_code"])
        {
            response.setStatusCode(node["status_code"].as<int>());
        }

        if (node["headers"] && node["headers"].IsMap())
        {
            std::map<std::string, std::string> headers;
            for (const auto& it : node["headers"])
            {
                std::string key = it.first.as<std::string>();
                std::string value = it.second.as<std::string>();
                headers[key] = value;
            }
            response.setHeaders(headers);
        }

        if (node["body"])
        {
            response.setBody(node["body"].as<std::string>());
        }

        return response;
    }
} // namespace zaplet::scenario