/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/http/request.h"

namespace zaplet::http
{

    const std::string& Request::getUrl() const
    {
        return m_url;
    }

    void Request::setUrl(const std::string& url)
    {
        m_url = url;
    }

    const std::string& Request::getMethod() const
    {
        return m_method;
    }

    void Request::setMethod(const std::string& method)
    {
        m_method = method;
    }

    const std::map<std::string, std::string>& Request::getHeaders() const
    {
        return m_headers;
    }

    void Request::setHeaders(const std::map<std::string, std::string>& headers)
    {
        m_headers = headers;
    }

    void Request::addHeader(const std::string& name, const std::string& value)
    {
        m_headers[name] = value;
    }

    const std::optional<std::string>& Request::getBody() const
    {
        return m_body;
    }

    void Request::setBody(const std::string& body)
    {
        m_body = body;
    }

    int Request::getTimeout() const
    {
        return m_timeout;
    }

    void Request::setTimeout(int timeout)
    {
        m_timeout = timeout;
    }

    const std::map<std::string, std::string>& Request::getQueryParams() const
    {
        return m_queryParams;
    }

    void Request::setQueryParams(const std::map<std::string, std::string>& params)
    {
        m_queryParams = params;
    }

    void Request::addQueryParam(const std::string& name, const std::string& value)
    {
        m_queryParams[name] = value;
    }
} // namespace zaplet::http
