/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <chrono>
#include <map>
#include <optional>
#include <string>

namespace zaplet::http
{
    class Request
    {
    public:
        Request() = default;

        [[nodiscard]] const std::string& getUrl() const;
        void setUrl(const std::string& url);

        [[nodiscard]] const std::string& getMethod() const;
        void setMethod(const std::string& method);

        [[nodiscard]] const std::map<std::string, std::string>& getHeaders() const;
        void setHeaders(const std::map<std::string, std::string>& headers);
        void addHeader(const std::string& name, const std::string& value);

        [[nodiscard]] const std::optional<std::string>& getBody() const;
        void setBody(const std::string& body);

        [[nodiscard]] int getTimeout() const;
        void setTimeout(int timeout);

        [[nodiscard]] const std::map<std::string, std::string>& getQueryParams() const;
        void setQueryParams(const std::map<std::string, std::string>& params);
        void addQueryParam(const std::string& name, const std::string& value);

    private:
        std::string m_url;
        std::string m_method = "GET";
        std::map<std::string, std::string> m_headers;
        std::optional<std::string> m_body;
        std::map<std::string, std::string> m_queryParams;
        int m_timeout = 30;
    };
} // namespace zaplet::http

#endif // REQUEST_H
