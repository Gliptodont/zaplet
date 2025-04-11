/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef HTTP_WRAPPER_H
#define HTTP_WRAPPER_H

#include <httplib.h>

#include <chrono>
#include <memory>

namespace zaplet::http
{
    class IClientWrapper
    {
    public:
        virtual ~IClientWrapper() = default;

        virtual void setConnectionTimeout(std::chrono::seconds timeout) = 0;

        virtual httplib::Result get(const std::string& path, const httplib::Headers& headers) = 0;
        virtual httplib::Result post(const std::string& path, const httplib::Headers& headers, const std::string& body) = 0;
        virtual httplib::Result post(const std::string& path, const httplib::Headers& headers) = 0;
        virtual httplib::Result put(const std::string& path, const httplib::Headers& headers, const std::string& body) = 0;
        virtual httplib::Result put(const std::string& path, const httplib::Headers& headers) = 0;
        virtual httplib::Result del(const std::string& path, const httplib::Headers& headers) = 0;
        virtual httplib::Result head(const std::string& path, const httplib::Headers& headers) = 0;
        virtual httplib::Result options(const std::string& path, const httplib::Headers& headers) = 0;
        virtual httplib::Result patch(const std::string& path, const httplib::Headers& headers, const std::string& body) = 0;
        virtual httplib::Result patch(const std::string& path, const httplib::Headers& headers) = 0;
    };

    class ClientWrapper : public IClientWrapper
    {
    public:
        explicit ClientWrapper(const std::string& host, int port)
            : m_client(std::make_unique<httplib::Client>(host, port))
        {
        }

        void setConnectionTimeout(std::chrono::seconds timeout) override
        {
            m_client->set_connection_timeout(timeout);
        }

        httplib::Result get(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Get(path, headers);
        }

        httplib::Result post(const std::string& path, const httplib::Headers& headers, const std::string& body) override
        {
            return m_client->Post(path, headers, body, "");
        }

        httplib::Result post(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Post(path, headers);
        }

        httplib::Result put(const std::string& path, const httplib::Headers& headers, const std::string& body) override
        {
            return m_client->Put(path, headers, body, "");
        }

        httplib::Result put(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Put(path, headers, "", "");
        }

        httplib::Result del(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Delete(path, headers);
        }

        httplib::Result head(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Head(path, headers);
        }

        httplib::Result options(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Options(path, headers);
        }

        httplib::Result patch(const std::string& path, const httplib::Headers& headers, const std::string& body) override
        {
            return m_client->Patch(path, headers, body, "");
        }

        httplib::Result patch(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Patch(path, headers, "", "");
        }

    private:
        std::unique_ptr<httplib::Client> m_client;
    };

    class SSLClientWrapper : public IClientWrapper
    {
    public:
        explicit SSLClientWrapper(const std::string& host, int port)
            : m_client(std::make_unique<httplib::SSLClient>(host, port))
        {
        }

        void setConnectionTimeout(std::chrono::seconds timeout) override
        {
            m_client->set_connection_timeout(timeout);
        }

        httplib::Result get(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Get(path, headers);
        }

        httplib::Result post(const std::string& path, const httplib::Headers& headers, const std::string& body) override
        {
            return m_client->Post(path, headers, body, "");
        }

        httplib::Result post(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Post(path, headers);
        }

        httplib::Result put(const std::string& path, const httplib::Headers& headers, const std::string& body) override
        {
            return m_client->Put(path, headers, body, "");
        }

        httplib::Result put(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Put(path, headers, "", "");
        }

        httplib::Result del(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Delete(path, headers);
        }

        httplib::Result head(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Head(path, headers);
        }

        httplib::Result options(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Options(path, headers);
        }

        httplib::Result patch(const std::string& path, const httplib::Headers& headers, const std::string& body) override
        {
            return m_client->Patch(path, headers, body, "");
        }

        httplib::Result patch(const std::string& path, const httplib::Headers& headers) override
        {
            return m_client->Patch(path, headers, "", "");
        }

    private:
        std::unique_ptr<httplib::SSLClient> m_client;
    };
} // namespace zaplet::http

#endif // HTTP_WRAPPER_H
