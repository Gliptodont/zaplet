/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef GET_H
#define GET_H

#include "cli/commands/command.h"

#include <string>

namespace zaplet::cli
{
    class GetCommand final : public Command
    {
    public:
        using Command::Command;

        void setupOptions() override;

    protected:
        void execute() override;

    private:
        std::string m_url;
        std::vector<std::string> m_headers;
        int m_timeout = 30;
    };
} // namespace zaplet::cli

#endif // GET_H
