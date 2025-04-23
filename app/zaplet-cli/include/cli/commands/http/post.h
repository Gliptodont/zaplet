/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef POST_H
#define POST_H

#include "cli/commands/command.h"

#include <string>

namespace zaplet::cli
{
    class PostCommand final : public Command
    {
    public:
        using Command::Command;

        void setupOptions() override;

    protected:
        void execute() override;

    private:
        std::string m_url;
        std::vector<std::string> m_headers;
        std::string m_body;
        std::string m_contentType = "application/json";
        int m_timeout = 30;
    };
}

#endif // POST_H
