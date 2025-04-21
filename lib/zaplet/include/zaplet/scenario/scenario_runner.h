/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef SCENARIO_RUNNER_H
#define SCENARIO_RUNNER_H

#include "zaplet/http/client.h"
#include "zaplet/output/formatter.h"
#include "zaplet/scenario/scenario.h"
#include "zaplet/scenario/step.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace zaplet::scenario
{
    enum class ValidationResult
    {
        Success,
        StatusCodeMismatch,
        HeaderMismatch,
        BodyMismatch,
        Error
    };

    class ScenarioRunner
    {
    public:
        ScenarioRunner(std::shared_ptr<http::Client> client, std::shared_ptr<output::Formatter> formatter);

        bool run(const Scenario& scenario);

        void setGlobalVariable(const std::string& name, const std::string& value);
        void clearGlobalVariables();

        [[nodiscard]] const std::string& getLastError() const;

    private:
        std::shared_ptr<http::Client> m_client;
        std::shared_ptr<output::Formatter> m_formatter;
        std::map<std::string, std::string> m_globalVariables;
        std::string m_lastError;

        bool executeStep(const Step& step, std::map<std::string, std::string>& environment);

        std::string applyVariables(const std::string& input, const std::map<std::string, std::string>& environment) const;
        http::Request applyVariablesToRequest(const http::Request& request, const std::map<std::string, std::string>& environment) const;
        void extractVariablesFromResponse(const http::Response& response, const std::map<std::string, std::string>& variableDefs, std::map<std::string, std::string>& environment) const;

        bool evaluateCondition(const std::string& condition, const std::map<std::string, std::string>& environment) const;

        ValidationResult validateResponse(const http::Response& actual, const http::Response& expected) const;
    };
}

#endif // SCENARIO_RUNNER_H
