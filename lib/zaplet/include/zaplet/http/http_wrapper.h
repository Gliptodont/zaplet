/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef HTTP_WRAPPER_H
#define HTTP_WRAPPER_H

#include <chrono>
#include <memory>

namespace httplib
{
    class Client;
    class Result;
    class Headers;
} // namespace httplib

namespace zaplet::http
{
    class IHttpClient
    {
    public:
        virtual ~IHttpClient() = default;

        virtual void setConnectionTimeout(std::chrono::seconds timeout) = 0;

        virtual httplib::Result get(const std::string& path, const httplib::Headers& headers) = 0;
        virtual httplib::Result post(const std::string& path, const httplib::Headers& headers, const std::string& body = "") = 0;
        virtual httplib::Result put(const std::string& path, const httplib::Headers& headers, const std::string& body = "") = 0;
        virtual httplib::Result del(const std::string& path, const httplib::Headers& headers) = 0;
        virtual httplib::Result head(const std::string& path, const httplib::Headers& headers) = 0;
        virtual httplib::Result options(const std::string& path, const httplib::Headers& headers) = 0;
        virtual httplib::Result patch(const std::string& path, const httplib::Headers& headers, const std::string& body = "") = 0;
    };

    class HttpClientWrapper : public IHttpClient
    {
    public:
        explicit HttpClientWrapper(std::unique_ptr<httplib::Client> client);

        void setConnectionTimeout(std::chrono::seconds timeout) override;

        httplib::Result get(const std::string& path, const httplib::Headers& headers) override;
        httplib::Result post(const std::string& path, const httplib::Headers& headers, const std::string& body = "") override;
        httplib::Result put(const std::string& path, const httplib::Headers& headers, const std::string& body = "") override;
        httplib::Result del(const std::string& path, const httplib::Headers& headers) override;
        httplib::Result head(const std::string& path, const httplib::Headers& headers) override;
        httplib::Result options(const std::string& path, const httplib::Headers& headers) override;
        httplib::Result patch(const std::string& path, const httplib::Headers& headers, const std::string& body = "") override;

    private:
        std::unique_ptr<httplib::Client> m_client;
    };
} // namespace zaplet::http

#endif // HTTP_WRAPPER_H
