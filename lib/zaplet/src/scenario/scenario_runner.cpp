/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/scenario/scenario_runner.h"

#include "zaplet/logging/logger.h"

#include <nlohmann/json.hpp>

#include <chrono>
#include <format>
#include <regex>
#include <thread>

namespace zaplet::scenario
{
    ScenarioRunner::ScenarioRunner(std::shared_ptr<http::Client> client, std::shared_ptr<output::Formatter> formatter)
        : m_client(std::move(client))
        , m_formatter(std::move(formatter))
    {
    }

    bool ScenarioRunner::run(const Scenario& scenario)
    {
        LOG_INFO_FMT("Running scenario: {}", scenario.getName());

        if (!scenario.getDescription().empty())
        {
            LOG_INFO_FMT("Description: {}", scenario.getDescription());
        }

        std::map<std::string, std::string> environment = m_globalVariables;

        for (const auto& [name, value] : scenario.getEnvironment())
        {
            environment[name] = value;
        }

        int repeatCount = 1;
        bool infiniteRepeat = false;

        if (scenario.getRepeatCount().has_value())
        {
            repeatCount = scenario.getRepeatCount().value();
            LOG_INFO_FMT("Scenario will repeat {} times", repeatCount);
        }
        else
        {
            infiniteRepeat = true;
            LOG_INFO("Scenario will repeat infinitely");
        }

        bool success = true;
        int iteration = 0;

        while (infiniteRepeat || iteration < repeatCount)
        {
            iteration++;
            LOG_INFO_FMT("Starting iteration {}", iteration);

            for (size_t i = 0; i < scenario.getSteps().size(); i++)
            {
                const auto& step = scenario.getSteps()[i];

                LOG_INFO_FMT("Executing step {} of {}", i + 1, scenario.getSteps().size());

                if (step.getDescription())
                {
                    LOG_INFO_FMT("Step: {}", step.getDescription().value());
                }

                bool stepSuccess = executeStep(step, environment);

                if (!stepSuccess)
                {
                    LOG_ERROR_FMT("Step {} failed", i + 1);

                    if (scenario.getContinueOnError())
                    {
                        LOG_WARNING("Continuing executing despite error (continue_on_error=true)");
                        success = false;
                    }
                    else
                    {
                        LOG_ERROR("Aborting scenario (continue_on_error=false)");
                        return false;
                    }
                }
            }
            LOG_INFO_FMT("Completed iteration {}", iteration);
        }
        return success;
    }

    void ScenarioRunner::setGlobalVariable(const std::string& name, const std::string& value)
    {
        m_globalVariables[name] = value;
        LOG_DEBUG_FMT("Set global variable: {}={}", name, value);
    }

    void ScenarioRunner::clearGlobalVariables()
    {
        m_globalVariables.clear();
        LOG_DEBUG("Cleared all global variables");
    }

    const std::string& ScenarioRunner::getLastError() const
    {
        return m_lastError;
    }

    bool ScenarioRunner::executeStep(const Step& step, std::map<std::string, std::string>& environment)
    {
        try
        {
            if (step.getCondition())
            {
                std::string condition = step.getCondition().value();
                LOG_DEBUG_FMT("Evaluating condition: {}", condition);

                bool conditionResult = evaluateCondition(condition, environment);
                if (!conditionResult)
                {
                    LOG_INFO("Skipping step - condition evaluated to false");
                    return true;
                }
            }

            auto delay = step.getDelay();
            if (delay.count() > 0)
            {
                LOG_DEBUG_FMT("Waiting for {} ms", delay.count());
                std::this_thread::sleep_for(delay);
            }

            http::Request request = applyVariablesToRequest(step.getRequest(), environment);

            LOG_INFO_FMT("Executing {} request to {}", request.getMethod(), request.getUrl());
            auto response = m_client->execute(request);

            if (response.hasError())
            {
                m_lastError = std::format("Request failed: {}", response.getError().value());
                LOG_ERROR(m_lastError);
                return false;
            }

            std::string formattedResponse = m_formatter->format(response);
            http::printResponse(formattedResponse, response.getStatusCode());

            if (step.getExpectedResponse())
            {
                ValidationResult validationResult = validateResponse(response, step.getExpectedResponse().value());

                if (validationResult != ValidationResult::Success)
                {
                    std::string errorMsg;
                    switch (validationResult)
                    {
                    case ValidationResult::StatusCodeMismatch:
                        errorMsg = "Status code mismatch";
                        break;
                    case ValidationResult::HeaderMismatch:
                        errorMsg = "Header mismatch";
                        break;
                    case ValidationResult::BodyMismatch:
                        errorMsg = "Body mismatch";
                        break;
                    default:
                        errorMsg = "Unknown validation error";
                        break;
                    }

                    m_lastError = std::format("Response validation failed: {}", errorMsg);
                    LOG_ERROR(m_lastError);
                    return false;
                }

                LOG_INFO("Response validation successful");
            }

            if (!step.getVariables().empty())
            {
                extractVariablesFromResponse(response, step.getVariables(), environment);
            }

            return true;
        } catch (const std::exception& e)
        {
            m_lastError = std::format("Exception during step execution: {}", e.what());
            LOG_ERROR(m_lastError);
            return false;
        }
    }

    std::string ScenarioRunner::applyVariables(const std::string& input, const std::map<std::string, std::string>& environment) const
    {
        std::string result = input;

        std::regex varPattern(R"(\$\{([^}]+)\})");
        std::smatch matches;

        auto searchStart = result.cbegin();
        while (std::regex_search(searchStart, result.cend(), matches, varPattern))
        {
            std::string varName = matches[1].str();
            std::string replacement;

            auto it = environment.find(varName);
            if (it != environment.end())
            {
                replacement = it->second;
            }
            else
            {
                LOG_WARNING_FMT("Variable not found: {}", varName);
                replacement = "";
            }

            result.replace(matches[0].first - result.cbegin(), matches[0].length(), replacement);

            searchStart = result.cbegin() + (matches.position() + replacement.length());
        }

        return result;
    }

    http::Request ScenarioRunner::applyVariablesToRequest(
        const http::Request& request, const std::map<std::string, std::string>& environment) const
    {
        http::Request result = request;

        result.setUrl(applyVariables(request.getUrl(), environment));

        std::map<std::string, std::string> headers;
        for (const auto& [name, value] : request.getHeaders())
        {
            headers[name] = applyVariables(value, environment);
        }
        result.setHeaders(headers);

        if (request.getBody())
        {
            result.setBody(applyVariables(request.getBody().value(), environment));
        }

        std::map<std::string, std::string> params;
        for (const auto& [name, value] : request.getQueryParams())
        {
            params[name] = applyVariables(value, environment);
        }
        result.setQueryParams(params);

        return result;
    }

    void ScenarioRunner::extractVariablesFromResponse(
        const http::Response& response,
        const std::map<std::string, std::string>& variableDefs,
        std::map<std::string, std::string>& environment) const
    {
        for (const auto& [varName, extractPath] : variableDefs)
        {
            try
            {
                std::string source;
                std::string path;

                size_t dotPos = extractPath.find('.');
                if (dotPos != std::string::npos)
                {
                    source = extractPath.substr(0, dotPos);
                    path = extractPath.substr(dotPos + 1);
                }
                else
                {
                    source = extractPath;
                    path = "";
                }

                std::string lowerSource = source;
                std::transform(lowerSource.begin(), lowerSource.end(), lowerSource.begin(), ::tolower);

                if (lowerSource == "status")
                {
                    environment[varName] = std::to_string(response.getStatusCode());
                    LOG_DEBUG_FMT("Extracted variable '{}' = '{}' from status code", varName, environment[varName]);
                }
                else if (lowerSource == "header")
                {
                    auto headers = response.getHeaders();
                    bool found = false;

                    for (const auto& [headerName, headerValue] : headers)
                    {
                        std::string lowerHeaderName = headerName;
                        std::transform(lowerHeaderName.begin(), lowerHeaderName.end(), lowerHeaderName.begin(), ::tolower);

                        std::string lowerPath = path;
                        std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);

                        if (lowerHeaderName == lowerPath)
                        {
                            environment[varName] = headerValue;
                            LOG_DEBUG_FMT("Extracted variable '{}' = '{}' from header '{}'", varName, environment[varName], headerName);
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                    {
                        LOG_WARNING_FMT("Header '{}' not found for variable '{}'", path, varName);
                    }
                }
                else if (lowerSource == "body")
                {
                    if (path.empty())
                    {
                        environment[varName] = response.getBody();
                        LOG_DEBUG_FMT("Extracted variable '{}' from entire body", varName);
                    }
                    else
                    {
                        try
                        {
                            nlohmann::json bodyJson = nlohmann::json::parse(response.getBody());

                            std::vector<std::string> pathParts;
                            std::string currentPart;
                            std::istringstream pathStream(path);

                            while (std::getline(pathStream, currentPart, '.'))
                            {
                                pathParts.push_back(currentPart);
                            }

                            nlohmann::json currentJson = bodyJson;
                            bool valid = true;

                            for (const auto& part : pathParts)
                            {
                                if (!part.empty() && part.front() == '[' && part.back() == ']')
                                {
                                    try
                                    {
                                        size_t index = std::stoi(part.substr(1, part.length() - 2));
                                        if (!currentJson.is_array() || index >= currentJson.size())
                                        {
                                            LOG_WARNING_FMT("Invalid array access: {}", part);
                                            valid = false;
                                            break;
                                        }
                                        currentJson = currentJson[index];
                                    } catch (const std::exception& e)
                                    {
                                        LOG_WARNING_FMT("Invalid array index: {}", e.what());
                                        valid = false;
                                        break;
                                    }
                                }
                                else
                                {
                                    if (!currentJson.contains(part))
                                    {
                                        LOG_WARNING_FMT("JSON path '{}' not found in body", path);
                                        valid = false;
                                        break;
                                    }
                                    currentJson = currentJson[part];
                                }
                            }

                            if (valid)
                            {
                                if (currentJson.is_string())
                                {
                                    environment[varName] = currentJson.get<std::string>();
                                }
                                else if (currentJson.is_boolean())
                                {
                                    environment[varName] = currentJson.get<bool>() ? "true" : "false";
                                }
                                else if (currentJson.is_number())
                                {
                                    environment[varName] = currentJson.dump();
                                }
                                else
                                {
                                    environment[varName] = currentJson.dump();
                                }
                            }
                        } catch (const nlohmann::json::exception& e)
                        {
                            LOG_WARNING_FMT("Failed to parse response body as JSON: {}", e.what());
                        }
                    }
                }
                else
                {
                    LOG_WARNING_FMT("Unknown source '{}' for variable '{}'", source, varName);
                }
            } catch (const std::exception& e)
            {
                LOG_ERROR_FMT("Exception extracting variable '{}': '{}'", varName, e.what());
            }
        }
    }

    bool ScenarioRunner::evaluateCondition(const std::string& condition, const std::map<std::string, std::string>& environment) const
    {
        std::string processedCondition = applyVariables(condition, environment);

        // Simple condition evaluation (equality, inequality)
        // Format: value1 == value2 or value1 != value2

        size_t eqPos = processedCondition.find("==");
        if (eqPos != std::string::npos)
        {
            std::string left = processedCondition.substr(0, eqPos);
            std::string right = processedCondition.substr(eqPos + 2);

            left.erase(0, left.find_first_not_of(" \t"));
            left.erase(left.find_last_not_of(" \t") + 1);
            right.erase(0, right.find_first_not_of(" \t"));
            right.erase(right.find_last_not_of(" \t") + 1);

            if (left.size() >= 2 && left.front() == '"' && left.back() == '"')
            {
                left = left.substr(1, left.size() - 2);
            }
            if (right.size() >= 2 && right.front() == '"' && right.back() == '"')
            {
                right = right.substr(1, right.size() - 2);
            }

            return left == right;
        }

        size_t nePos = processedCondition.find("!=");
        if (nePos != std::string::npos)
        {
            std::string left = processedCondition.substr(0, nePos);
            std::string right = processedCondition.substr(nePos + 2);

            left.erase(0, left.find_first_not_of(" \t"));
            left.erase(left.find_last_not_of(" \t") + 1);
            right.erase(0, right.find_first_not_of(" \t"));
            right.erase(right.find_last_not_of(" \t") + 1);

            if (left.size() >= 2 && left.front() == '"' && left.back() == '"')
            {
                left = left.substr(1, left.size() - 2);
            }
            if (right.size() >= 2 && right.front() == '"' && right.back() == '"')
            {
                right = right.substr(1, right.size() - 2);
            }

            return left != right;
        }

        size_t gtPos = processedCondition.find(">");
        if (gtPos != std::string::npos && processedCondition.find(">=") == std::string::npos)
        {
            std::string left = processedCondition.substr(0, gtPos);
            std::string right = processedCondition.substr(gtPos + 1);

            left.erase(0, left.find_first_not_of(" \t"));
            left.erase(left.find_last_not_of(" \t") + 1);
            right.erase(0, right.find_first_not_of(" \t"));
            right.erase(right.find_last_not_of(" \t") + 1);

            try
            {
                double leftVal = std::stod(left);
                double rightVal = std::stod(right);
                return leftVal > rightVal;
            } catch (const std::exception&)
            {
                return left > right;
            }
        }

        size_t ltPos = processedCondition.find("<");
        if (ltPos != std::string::npos && processedCondition.find("<=") == std::string::npos)
        {
            std::string left = processedCondition.substr(0, ltPos);
            std::string right = processedCondition.substr(ltPos + 1);

            left.erase(0, left.find_first_not_of(" \t"));
            left.erase(left.find_last_not_of(" \t") + 1);
            right.erase(0, right.find_first_not_of(" \t"));
            right.erase(right.find_last_not_of(" \t") + 1);

            try
            {
                double leftVal = std::stod(left);
                double rightVal = std::stod(right);
                return leftVal < rightVal;
            } catch (const std::exception&)
            {
                return left < right;
            }
        }

        size_t gePos = processedCondition.find(">=");
        if (gePos != std::string::npos)
        {
            std::string left = processedCondition.substr(0, gePos);
            std::string right = processedCondition.substr(gePos + 2);

            left.erase(0, left.find_first_not_of(" \t"));
            left.erase(left.find_last_not_of(" \t") + 1);
            right.erase(0, right.find_first_not_of(" \t"));
            right.erase(right.find_last_not_of(" \t") + 1);

            try
            {
                double leftVal = std::stod(left);
                double rightVal = std::stod(right);
                return leftVal >= rightVal;
            } catch (const std::exception&)
            {
                return left >= right;
            }
        }

        size_t lePos = processedCondition.find("<=");
        if (lePos != std::string::npos)
        {
            std::string left = processedCondition.substr(0, lePos);
            std::string right = processedCondition.substr(lePos + 2);

            left.erase(0, left.find_first_not_of(" \t"));
            left.erase(left.find_last_not_of(" \t") + 1);
            right.erase(0, right.find_first_not_of(" \t"));
            right.erase(right.find_last_not_of(" \t") + 1);

            try
            {
                double leftVal = std::stod(left);
                double rightVal = std::stod(right);
                return leftVal <= rightVal;
            } catch (const std::exception&)
            {
                return left <= right;
            }
        }

        return !processedCondition.empty() && processedCondition != "0" && processedCondition != "false" && processedCondition != "FALSE";
    }

    ValidationResult ScenarioRunner::validateResponse(const http::Response& actual, const http::Response& expected) const
    {
        if (expected.getStatusCode() != 0 && actual.getStatusCode() != expected.getStatusCode())
        {
            LOG_ERROR_FMT("Status code mismatch: expected {}, got {}", expected.getStatusCode(), actual.getStatusCode());
            return ValidationResult::StatusCodeMismatch;
        }

        for (const auto& [expectedName, expectedValue] : expected.getHeaders())
        {
            bool headerFound = false;

            for (const auto& [actualName, actualValue] : actual.getHeaders())
            {
                std::string lowerExpectedName = expectedName;
                std::transform(lowerExpectedName.begin(), lowerExpectedName.end(), lowerExpectedName.begin(), ::tolower);

                std::string lowerActualName = actualName;
                std::transform(lowerActualName.begin(), lowerActualName.end(), lowerActualName.begin(), ::tolower);

                if (lowerExpectedName == lowerActualName)
                {
                    headerFound = true;

                    if (expectedValue != actualValue)
                    {
                        LOG_ERROR_FMT("Header value mismatch for '{}': expected '{}', got '{}'", expectedName, expectedValue, actualValue);
                        return ValidationResult::HeaderMismatch;
                    }

                    break;
                }
            }

            if (!headerFound)
            {
                LOG_ERROR_FMT("Expected header '{}' not found in response", expectedName);
                return ValidationResult::HeaderMismatch;
            }
        }

        if (!expected.getBody().empty())
        {
            try
            {
                nlohmann::json expectedJson = nlohmann::json::parse(expected.getBody());
                nlohmann::json actualJson = nlohmann::json::parse(actual.getBody());

                if (expectedJson != actualJson)
                {
                    LOG_ERROR("JSON body mismatch");
                    LOG_DEBUG_FMT("Expected: {}", expected.getBody());
                    LOG_DEBUG_FMT("Actual: {}", actual.getBody());
                    return ValidationResult::BodyMismatch;
                }
            } catch (const nlohmann::json::exception&)
            {
                if (actual.getBody() != expected.getBody())
                {
                    if (actual.getBody().find(expected.getBody()) == std::string::npos)
                    {
                        LOG_ERROR("Body content mismatch");
                        LOG_DEBUG_FMT("Expected body to contain: {}", expected.getBody());
                        LOG_DEBUG_FMT("Actual body: {}", actual.getBody());
                        return ValidationResult::BodyMismatch;
                    }
                }
            }
        }

        return ValidationResult::Success;
    }

} // namespace zaplet::scenario
