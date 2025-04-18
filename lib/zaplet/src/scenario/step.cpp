/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/scenario/step.h"

namespace zaplet::scenario
{
    const http::Request& Step::getRequest() const
    {
        return m_request;
    }

    void Step::setRequest(const http::Request& request)
    {
        m_request = request;
    }

    const std::optional<http::Response>& Step::getExpectedResponse() const
    {
        return m_expectedResponse;
    }

    void Step::setExpectedResponse(const http::Response& response)
    {
        m_expectedResponse = response;
    }

    const std::map<std::string, std::string>& Step::getVariables() const
    {
        return m_variables;
    }

    void Step::setVariables(const std::map<std::string, std::string>& variables)
    {
        m_variables = variables;
    }

    void Step::addVariable(const std::string& name, const std::string& expression)
    {
        m_variables[name] = expression;
    }

    const std::optional<std::string>& Step::getCondition() const
    {
        return m_condition;
    }

    void Step::setCondition(const std::string& condition)
    {
        m_condition = condition;
    }

    const std::optional<std::string>& Step::getDescription() const
    {
        return m_description;
    }

    void Step::setDescription(const std::string& description)
    {
        m_description = description;
    }

    std::chrono::milliseconds Step::getDelay() const
    {
        return m_delay;
    }

    void Step::setDelay(std::chrono::milliseconds delay)
    {
        m_delay = delay;
    }
} // namespace zaplet::scenario