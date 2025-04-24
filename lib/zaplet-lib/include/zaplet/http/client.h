/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef CLIENT_H
#define CLIENT_H

#include "zaplet/http/http_wrapper.h"
#include "zaplet/http/request.h"
#include "zaplet/http/response.h"

#include <memory>
#include <string>

namespace httplib
{
    class Client;
    class SSLClient;
} // namespace httplib

namespace zaplet::http
{
    class Client
    {
    public:
        Client() = default;
        ~Client() = default;

        Response execute(const Request& request);

    private:
        std::unique_ptr<IClientWrapper> createClient(const std::string& url);

        bool parseUrl(const std::string& url, std::string& scheme, std::string& host, int& port, std::string& path);
    };
} // namespace zaplet::http

#endif // CLIENT_H
