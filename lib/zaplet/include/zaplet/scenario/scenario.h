/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef SCENARIO_H
#define SCENARIO_H

#include "zaplet/http/request.h"
#include "zaplet/http/response.h"

#include <chrono>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace zaplet::scenario
{
    struct Step
    {
        std::string name;
        std::string description;
        http::Request request;
        std::optional<http::Response> expectedResponse;
        std::map<std::string, std::string> variables;
        std::optional<std::string> condition;
        std::optional<std::chrono::milliseconds> delay;
    };

    class Scenario
    {
    public:
        Scenario() = default;
        ~Scenario() = default;

        [[nodiscard]] const std::string& getName() const;
        void setName(const std::string& name);

        [[nodiscard]] const std::string& getDescription() const;
        void setDescription(const std::string& description);

        [[nodiscard]] const std::vector<Step>& getSteps() const;
        void setSteps(const std::vector<Step>& steps);
        void addStep(const Step& step);

        [[nodiscard]] const std::map<std::string, std::string>& getEnvironment() const;
        void setEnvironment(const std::map<std::string, std::string>& env);
        void addEnvironmentVariable(const std::string& name, const std::string& value);

        [[nodiscard]] std::optional<int> getRepeatCount() const;
        void setRepeatCount(const std::optional<int>& count);

        [[nodiscard]] bool getContinueOnError() const;
        void setContinueOnError(bool continue_);

    private:
        std::string m_name;
        std::string m_description;
        std::vector<Step> m_steps;
        std::map<std::string, std::string> m_env;
        std::optional<int> m_repeatCount;
        bool m_continueOnError{ false };
    };
} // namespace zaplet::scenario

#endif // SCENARIO_H
