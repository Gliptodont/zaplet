/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/scenario/player.h"

#include "zaplet/logging/logger.h"
#include "zaplet/scenario/yaml_parser.h"

#include <nlohmann/json.hpp>

#include <chrono>
#include <regex>
#include <thread>

namespace zaplet::scenario
{
    Player::Player(std::shared_ptr<http::Client> client, std::shared_ptr<output::Formatter> formatter)
        : m_client(std::move(client))
        , m_formatter(std::move(formatter))
    {
    }

    bool Player::play(const Scenario& scenario)
    {
        LOG_INFO_FMT("Starting scenario: {}", scenario.getName());
        LOG_INFO_FMT("Description: {}", scenario.getDescription());

        m_variables = scenario.getEnvironment();

        int iterations = 1;
        if (scenario.getRepeatCount().has_value())
        {
            iterations = scenario.getRepeatCount().value();
            LOG_INFO_FMT("Will repeat scenario {} times", iterations);
        }
        else
        {
            LOG_INFO("Will repeat scenario infinitely (until manually stopped)");
            iterations = std::numeric_limits<int>::max();
        }

        bool success = true;
        for (int i = 0; i < iterations; ++i)
        {
            LOG_INFO_FMT("Starting iteration {}", i + 1);

            for (const auto& step : scenario.getSteps())
            {
                if (step.delay.has_value())
                {
                    LOG_DEBUG_FMT("Waiting for {} ms before executing step '{}'", step.delay.value().count(), step.name);
                    std::this_thread::sleep_for(step.delay.value());
                }

                if (step.condition.has_value() && !evaluateCondition(step.condition))
                {
                    LOG_INFO_FMT("Skipping step '{}' because condition is false", step.name);
                    continue;
                }

                LOG_INFO_FMT("Executing step: {}", step.name);
                LOG_INFO_FMT("Step description: {}", step.description);
                bool stepSuccess = executeStep(step);

                if (!stepSuccess)
                {
                    LOG_ERROR_FMT("Step '{}' failed", step.name);
                    success = false;

                    if (!scenario.getContinueOnError())
                    {
                        LOG_ERROR("Stopping scenario due to error");
                        return false;
                    }
                }
            }

            LOG_INFO_FMT("Completed iteration {}", i + 1);

            if (i < iterations - 1)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        LOG_INFO_FMT("Scenario '{}' completed with {}", scenario.getName(), success ? "success" : "failures");
        return success;
    }

    bool Player::playFile(const std::string& filePath)
    {
        try
        {
            YamlParser parser;
            Scenario scenario = parser.parseFile(filePath);
            return play(scenario);
        } catch (const std::exception& e)
        {
            LOG_ERROR_FMT("Failed to play scenario file {}: {}", filePath, e.what());
            return false;
        }
    }

    bool Player::executeStep(const Step& step)
    {
        try
        {
            http::Request processedRequest = replaceVariablesInRequest(step.request);

            LOG_DEBUG_FMT("Executing {} request to {}", processedRequest.getMethod(), processedRequest.getUrl());
            auto response = m_client->execute(processedRequest);

            extractVariables(step, response);

            bool validationResult = true;
            if (step.expectedResponse.has_value())
            {
                validationResult = validateResponse(step, response);
            }

            http::printResponse(m_formatter->format(response), response.getStatusCode());

            return response.isSuccess() && validationResult;
        } catch (const std::exception& e)
        {
            LOG_ERROR_FMT("Exception during step execution: {}", e.what());
            return false;
        }
    }

    std::string Player::replaceVariables(const std::string& input)
    {
        std::string result = input;
        std::regex variablePattern("\\$\\{([\\w.]+)\\}");

        std::smatch matches;
        std::string::const_iterator searchStart(input.cbegin());

        while (std::regex_search(searchStart, input.cend(), matches, variablePattern))
        {
            std::string varName = matches[1].str();
            std::string replacement;

            if (m_variables.find(varName) != m_variables.end())
            {
                replacement = m_variables[varName];
                LOG_DEBUG_FMT("Replacing variable '{}' with '{}'", varName, replacement);
            }
            else
            {
                LOG_WARNING_FMT("Variable '{}' not found, leaving as is", varName);
                replacement = matches[0].str();
            }

            size_t pos = result.find(matches[0].str());
            if (pos != std::string::npos)
            {
                result.replace(pos, matches[0].length(), replacement);
            }

            searchStart = matches.suffix().first;
        }

        return result;
    }

    http::Request Player::replaceVariablesInRequest(const http::Request& request)
    {
        http::Request result = request;

        result.setUrl(replaceVariables(request.getUrl()));

        std::map<std::string, std::string> headers = request.getHeaders();
        std::map<std::string, std::string> processedHeaders;

        for (const auto& [key, value] : headers)
        {
            processedHeaders[key] = replaceVariables(value);
        }
        result.setHeaders(processedHeaders);

        if (request.getBody().has_value())
        {
            result.setBody(replaceVariables(request.getBody().value()));
        }

        std::map<std::string, std::string> params = request.getQueryParams();
        std::map<std::string, std::string> processedParams;

        for (const auto& [key, value] : params)
        {
            processedParams[key] = replaceVariables(value);
        }
        result.setQueryParams(processedParams);

        return result;
    }

    void Player::extractVariables(const Step& step, const http::Response& response)
    {
        for (const auto& [varName, extractionRule] : step.variables)
        {
            try
            {
                if (extractionRule.starts_with("$."))
                {
                    try
                    {
                        auto json = nlohmann::json::parse(response.getBody());

                        // Format: $.field1.field2[0].field3
                        std::string path = extractionRule.substr(2);
                        std::vector<std::string> parts;

                        std::regex pathRegex(R"((\w+)(?:\[(\d+)\])?)");
                        std::sregex_iterator it(path.begin(), path.end(), pathRegex);
                        std::sregex_iterator end;

                        nlohmann::json current = json;

                        while (it != end)
                        {
                            std::smatch match = *it;
                            std::string fieldName = match[1].str();

                            if (current.contains(fieldName))
                            {
                                current = current[fieldName];

                                if (match[2].matched)
                                {
                                    int index = std::stoi(match[2].str());
                                    if (current.is_array() && index >= 0 && index < current.size())
                                    {
                                        current = current[index];
                                    }
                                    else
                                    {
                                        LOG_WARNING_FMT("Invalid array index {} for field {}", index, fieldName);
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                LOG_WARNING_FMT("Field {} not found in JSON", fieldName);
                                break;
                            }

                            ++it;
                        }

                        if (current.is_string())
                        {
                            m_variables[varName] = current.get<std::string>();
                        }
                        else
                        {
                            m_variables[varName] = current.dump();
                        }

                        LOG_DEBUG_FMT("Extracted variable '{}' = '{}' using JSON path", varName, m_variables[varName]);
                    } catch (const nlohmann::json::exception& e)
                    {
                        LOG_WARNING_FMT("Failed to parse response as JSON: {}", e.what());
                    }
                }
                else if (extractionRule.starts_with("header."))
                {
                    std::string headerName = extractionRule.substr(7);
                    auto headers = response.getHeaders();

                    if (headers.find(headerName) != headers.end())
                    {
                        m_variables[varName] = headers[headerName];
                        LOG_DEBUG_FMT("Extracted variable '{}' = '{}' from header", varName, m_variables[varName]);
                    }
                    else
                    {
                        LOG_WARNING_FMT("Header {} not found in response", headerName);
                    }
                }
                else if (extractionRule == "status_code")
                {
                    m_variables[varName] = std::to_string(response.getStatusCode());
                    LOG_DEBUG_FMT("Extracted variable '{}' = '{}' from status code", varName, m_variables[varName]);
                }
                else if (extractionRule == "body")
                {
                    m_variables[varName] = response.getBody();
                    LOG_DEBUG_FMT("Extracted variable '{}' from response body", varName);
                }
                else if (extractionRule.starts_with("regex:"))
                {
                    std::string pattern = extractionRule.substr(6);
                    std::regex regex(pattern);
                    std::smatch matches;

                    if (std::regex_search(response.getBody(), matches, regex) && matches.size() > 1)
                    {
                        m_variables[varName] = matches[1].str();
                        LOG_DEBUG_FMT("Extracted variable '{}' = '{}' using regex", varName, m_variables[varName]);
                    }
                    else
                    {
                        LOG_WARNING_FMT("Regex pattern {} did not match or has no capture group", pattern);
                    }
                }
                else
                {
                    LOG_WARNING_FMT("Unknown variable extraction rule: {}", extractionRule);
                }
            } catch (const std::exception& e)
            {
                LOG_ERROR_FMT("Error extracting variable {}: {}", varName, e.what());
            }
        }
    }

    bool Player::evaluateCondition(const std::optional<std::string>& condition)
    {
        if (!condition.has_value() || condition->empty())
        {
            return true;
        }

        std::string processedCondition = replaceVariables(condition.value());

        // Format: variable == value, variable != value, etc.
        std::regex conditionRegex(R"((\S+)\s*(==|!=|>|<|>=|<=)\s*(\S+))");
        std::smatch matches;

        if (std::regex_match(processedCondition, matches, conditionRegex))
        {
            std::string left = matches[1].str();
            std::string op = matches[2].str();
            std::string right = matches[3].str();

            if (left.front() == '"' && left.back() == '"')
            {
                left = left.substr(1, left.length() - 2);
            }
            if (right.front() == '"' && right.back() == '"')
            {
                right = right.substr(1, right.length() - 2);
            }

            LOG_DEBUG_FMT("Evaluating condition: {} {} {}", left, op, right);

            if (op == "==")
            {
                return left == right;
            }
            else if (op == "!=")
            {
                return left != right;
            }
            else if (op == ">")
            {
                return std::stod(left) > std::stod(right);
            }
            else if (op == "<")
            {
                return std::stod(left) < std::stod(right);
            }
            else if (op == ">=")
            {
                return std::stod(left) >= std::stod(right);
            }
            else if (op == "<=")
            {
                return std::stod(left) <= std::stod(right);
            }
        }

        LOG_WARNING_FMT("Invalid condition format: {}", processedCondition);
        return false;
    }

    bool Player::validateResponse(const Step& step, const http::Response& actualResponse) const
    {
        if (!step.expectedResponse.has_value())
        {
            return true;
        }

        const http::Response& expectedResponse = step.expectedResponse.value();
        bool isValid = true;

        if (expectedResponse.getStatusCode() != 0 && expectedResponse.getStatusCode() != actualResponse.getStatusCode())
        {
            LOG_ERROR_FMT(
                "Status code validation failed: expected {}, got {}", expectedResponse.getStatusCode(), actualResponse.getStatusCode());
            isValid = false;
        }

        for (const auto& [name, value] : expectedResponse.getHeaders())
        {
            if (actualResponse.getHeaders().find(name) == actualResponse.getHeaders().end() ||
                actualResponse.getHeaders().at(name) != value)
            {
                LOG_ERROR_FMT(
                    "Header validation failed for {}: expected {}, got {}",
                    name,
                    value,
                    actualResponse.getHeaders().find(name) != actualResponse.getHeaders().end() ? actualResponse.getHeaders().at(name)
                                                                                                : "not present");
                isValid = false;
            }
        }

        if (!expectedResponse.getBody().empty())
        {
            try
            {
                auto expectedJson = nlohmann::json::parse(expectedResponse.getBody());
                auto actualJson = nlohmann::json::parse(actualResponse.getBody());

                if (expectedJson != actualJson)
                {
                    LOG_ERROR("JSON body validation failed");
                    LOG_DEBUG_FMT("Expected: {}", expectedResponse.getBody());
                    LOG_DEBUG_FMT("Actual: {}", actualResponse.getBody());
                    isValid = false;
                }
            } catch (const nlohmann::json::exception&)
            {
                if (expectedResponse.getBody() != actualResponse.getBody())
                {
                    LOG_ERROR("Body validation failed");
                    LOG_DEBUG_FMT("Expected: {}", expectedResponse.getBody());
                    LOG_DEBUG_FMT("Actual: {}", actualResponse.getBody());
                    isValid = false;
                }
            }
        }

        return isValid;
    }
} // namespace zaplet::scenario