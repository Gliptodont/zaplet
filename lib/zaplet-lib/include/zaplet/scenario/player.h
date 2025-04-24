/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "zaplet/http/client.h"
#include "zaplet/output/formatter.h"
#include "zaplet/scenario/scenario.h"

#include <map>
#include <memory>
#include <string>

namespace zaplet::scenario
{
    class Player
    {
    public:
        Player(std::shared_ptr<http::Client> client, std::shared_ptr<output::Formatter> formatter);
        ~Player() = default;

        bool play(const Scenario& scenario);
        bool playFile(const std::string& filePath);

    private:
        std::shared_ptr<http::Client> m_client;
        std::shared_ptr<output::Formatter> m_formatter;
        std::map<std::string, std::string> m_variables;

        bool executeStep(const Step& step);

        std::string replaceVariables(const std::string& input);
        http::Request replaceVariablesInRequest(const http::Request& request);

        void extractVariables(const Step& step, const http::Response& response);
        bool evaluateCondition(const std::optional<std::string>& condition);
        bool validateResponse(const Step& step, const http::Response& actualResponse) const;
    };
} // namespace zaplet::scenario

#endif // PLAYER_H
