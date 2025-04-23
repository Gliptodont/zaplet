/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "cli/commands/http/get.h"

#include <zaplet/zaplet.h>

#include <map>

namespace zaplet::cli
{
    void GetCommand::setupOptions()
    {
        m_app->add_option("url", m_url, "URL to request")->required();
        m_app->add_option("-H,--header", m_headers, "HTTP headers (can be specified multiple times)");
        m_app->add_option("-t,--timeout", m_timeout, "Request timeout in seconds")->default_val(30);
    }

    void GetCommand::execute()
    {
        LOG_INFO_FMT("Executing GET request to {}", m_url);

        const std::map<std::string, std::string> headerMap = http::headersVectorToMap(m_headers);

        http::Request request;
        request.setUrl(m_url);
        request.setMethod("GET");
        request.setHeaders(headerMap);
        request.setTimeout(m_timeout);

        auto response = m_client->execute(request);

        http::printResponse(m_formatter->format(response), response.getStatusCode());
    }
} // namespace zaplet::cli
