/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef MODEL_H
#define MODEL_H

#include "zaplet/http/request.h"
#include "zaplet/http/response.h"

#include <chrono>
#include <map>
#include <optional>
#include <string>

namespace zaplet::scenario
{
    class Step
    {
    public:
        Step() = default;
        ~Step() = default;

        [[nodiscard]] const http::Request& getRequest() const;
        void setRequest(const http::Request& request);

        [[nodiscard]] const std::optional<http::Response>& getExpectedResponse() const;
        void setExpectedResponse(const http::Response& response);

        [[nodiscard]] const std::map<std::string, std::string>& getVariables() const;
        void setVariables(const std::map<std::string, std::string>& variables);
        void addVariable(const std::string& name, const std::string& expression);

        [[nodiscard]] const std::optional<std::string>& getCondition() const;
        void setCondition(const std::string& condition);

        [[nodiscard]] const std::optional<std::string>& getDescription() const;
        void setDescription(const std::string& description);

        [[nodiscard]] std::chrono::milliseconds getDelay() const;
        void setDelay(std::chrono::milliseconds delay);

    private:
        http::Request m_request;
        std::optional<http::Response> m_expectedResponse;
        std::map<std::string, std::string> m_variables;
        std::optional<std::string> m_condition;
        std::optional<std::string> m_description;
        std::chrono::milliseconds m_delay{ 0 };
    };
} // namespace zaplet::scenario

#endif // MODEL_H
