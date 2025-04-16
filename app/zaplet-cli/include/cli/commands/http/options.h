/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include "cli/commands/command.h"

#include <string>

namespace zaplet::cli
{
    class OptionsCommand final : public Command
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
}

#endif // OPTIONS_H
