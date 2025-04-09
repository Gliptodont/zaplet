/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef CLIENT_H
#define CLIENT_H

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
    //using ClientVariant = std::variant<std::monostate, std::unique_ptr<httplib::Client>, std::unique_ptr<httplib::SSLClient>>;

    class Client
    {
    public:
        Client() = default;
        ~Client() = default;

        Response execute(const Request& request);

    private:

        std::unique_ptr<httplib::Client> createClient(const std::string& url);
        //std::unique_ptr<httplib::SSLClient> createSSLClient(const std::string& url);

        bool parseUrl(const std::string& url, std::string& scheme, std::string& host, int& port, std::string& path);
    };
} // namespace zaplet::http

#endif // CLIENT_H
