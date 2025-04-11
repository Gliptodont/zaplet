/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/http/response.h"

#include <iostream>

namespace zaplet::http
{
    int Response::getStatusCode() const
    {
        return m_statusCode;
    }

    void Response::setStatusCode(int statusCode)
    {
        m_statusCode = statusCode;
    }

    const std::map<std::string, std::string>& Response::getHeaders() const
    {
        return m_headers;
    }

    void Response::setHeaders(const std::map<std::string, std::string>& headers)
    {
        m_headers = headers;
    }

    void Response::addHeader(const std::string& name, const std::string& value)
    {
        m_headers[name] = value;
    }

    const std::string& Response::getBody() const
    {
        return m_body;
    }

    void Response::setBody(const std::string& body)
    {
        m_body = body;
    }

    std::chrono::milliseconds Response::getLatency() const
    {
        return m_latency;
    }

    void Response::setLatency(std::chrono::milliseconds latency)
    {
        m_latency = latency;
    }

    const std::optional<std::string>& Response::getError() const
    {
        return m_error;
    }

    void Response::setError(const std::string& error)
    {
        m_error = error;
    }

    bool Response::hasError() const
    {
        return m_error.has_value();
    }

    bool Response::isSuccess() const
    {
        return m_statusCode >= 200 && m_statusCode < 300 && !hasError();
    }

    void printResponse(const std::string& response, int statusCode)
    {
        if (statusCode >= 100 && statusCode < 400)
        {
            LOG_INFO_FMT("Response Success: \n{}", response);
        }
        else if (statusCode >= 400 && statusCode < 500)
        {
            LOG_WARNING_FMT("Response Warning: \n{}", response);
        }
        else if (statusCode >= 500 && statusCode <= 599)
        {
            LOG_ERROR_FMT("Response Error: \n{}", response);
        }
        else
        {
            LOG_DEBUG("Unhandled status code: " + std::to_string(statusCode) + " | " + response);
        }
    }
} // namespace zaplet::http