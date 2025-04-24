/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/http/http_wrapper.h"

#include <httplib.h>

namespace zaplet::http
{

    HttpClientWrapper::HttpClientWrapper(std::unique_ptr<httplib::Client> client)
        : m_client(std::move(client))
    {
    }

    void HttpClientWrapper::setConnectionTimeout(std::chrono::seconds timeout)
    {
        m_client->set_connection_timeout(timeout);
    }

    httplib::Result HttpClientWrapper::get(const std::string& path, const class httplib::Headers& headers)
    {
        return m_client->Get(path, headers);
    }

    httplib::Result HttpClientWrapper::post(const std::string& path, const class httplib::Headers& headers, const std::string& body)
    {
        return body.empty() ? m_client->Post(path, headers) : m_client->Post(path, headers, body, "");
    }

    httplib::Result HttpClientWrapper::put(const std::string& path, const class httplib::Headers& headers, const std::string& body)
    {
        return m_client->Put(path, headers, body, "");
    }

    httplib::Result HttpClientWrapper::del(const std::string& path, const class httplib::Headers& headers)
    {
        return m_client->Delete(path, headers);
    }

    httplib::Result HttpClientWrapper::head(const std::string& path, const class httplib::Headers& headers)
    {
        return m_client->Head(path, headers);
    }

    httplib::Result HttpClientWrapper::options(const std::string& path, const class httplib::Headers& headers)
    {
        return m_client->Options(path, headers);
    }

    httplib::Result HttpClientWrapper::patch(const std::string& path, const class httplib::Headers& headers, const std::string& body)
    {
        return m_client->Patch(path, headers, body, "");
    }
} // namespace zaplet::http