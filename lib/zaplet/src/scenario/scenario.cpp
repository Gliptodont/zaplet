/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/scenario/scenario.h"

namespace zaplet::scenario
{
    const std::string& Scenario::getName() const
    {
        return m_name;
    }

    void Scenario::setName(const std::string& name)
    {
        m_name = name;
    }

    const std::string& Scenario::getDescription() const
    {
        return m_description;
    }

    void Scenario::setDescription(const std::string& description)
    {
        m_description = description;
    }

    const std::vector<Step>& Scenario::getSteps() const
    {
        return m_steps;
    }

    void Scenario::setSteps(const std::vector<Step>& steps)
    {
        m_steps = steps;
    }

    void Scenario::addStep(const Step& step)
    {
        m_steps.push_back(step);
    }

    const std::map<std::string, std::string>& Scenario::getEnvironment() const
    {
        return m_environment;
    }

    void Scenario::setEnvironment(const std::map<std::string, std::string>& environment)
    {
        m_environment = environment;
    }

    void Scenario::addEnvironmentVariable(const std::string& name, const std::string& value)
    {
        m_environment[name] = value;
    }

    std::optional<int> Scenario::getRepeatCount() const
    {
        return m_repeatCount;
    }

    void Scenario::setRepeatCount(std::optional<int> repeatCount)
    {
        m_repeatCount = repeatCount;
    }

    bool Scenario::getContinueOnError() const
    {
        return m_continueOnError;
    }

    void Scenario::setContinueOnError(bool continueOnError)
    {
        m_continueOnError = continueOnError;
    }
} // namespace zaplet::scenario
