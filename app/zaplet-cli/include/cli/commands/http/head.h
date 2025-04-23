/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef HEAD_H
#define HEAD_H

#include "cli/commands/command.h"

#include <string>

namespace zaplet::cli
{
    class HeadCommand final : public Command
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

#endif // HEAD_H
