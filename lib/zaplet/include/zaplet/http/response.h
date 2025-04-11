/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef RESPONSE_H
#define RESPONSE_H

#include "zaplet/logging/logger.h"

#include <chrono>
#include <map>
#include <optional>
#include <string>

namespace zaplet::http
{
    class Response
    {
    public:
        Response() = default;

        [[nodiscard]] int getStatusCode() const;
        void setStatusCode(int statusCode);

        [[nodiscard]] const std::map<std::string, std::string>& getHeaders() const;
        void setHeaders(const std::map<std::string, std::string>& headers);
        void addHeader(const std::string& name, const std::string& value);

        [[nodiscard]] const std::string& getBody() const;
        void setBody(const std::string& body);

        [[nodiscard]] std::chrono::milliseconds getLatency() const;
        void setLatency(std::chrono::milliseconds latency);

        [[nodiscard]] const std::optional<std::string>& getError() const;
        void setError(const std::string& error);
        [[nodiscard]] bool hasError() const;

        [[nodiscard]] bool isSuccess() const;

    private:
        int m_statusCode = 0;
        std::map<std::string, std::string> m_headers;
        std::string m_body;
        std::chrono::milliseconds m_latency{ 0 };
        std::optional<std::string> m_error;
    };

    void printResponse(const std::string& response, int statusCode);
} // namespace zaplet::http

#endif // RESPONSE_H
