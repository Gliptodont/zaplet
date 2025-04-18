/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef SCENARIO_H
#define SCENARIO_H

#include "zaplet/scenario/step.h"

#include <chrono>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace zaplet::scenario
{
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
        void setEnvironment(const std::map<std::string, std::string>& environment);
        void addEnvironmentVariable(const std::string& name, const std::string& value);

        [[nodiscard]] std::optional<int> getRepeatCount() const;
        void setRepeatCount(std::optional<int> repeatCount);

        [[nodiscard]] bool getContinueOnError() const;
        void setContinueOnError(bool continueOnError);

    private:
        std::string m_name;
        std::string m_description;
        std::vector<Step> m_steps;
        std::map<std::string, std::string> m_environment;
        std::optional<int> m_repeatCount{ 1 };
        bool m_continueOnError{ false };
    };
} // namespace zaplet::scenario

#endif // SCENARIO_H
