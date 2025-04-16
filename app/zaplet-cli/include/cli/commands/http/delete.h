/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef DELETE_H
#define DELETE_H

#include "cli/commands/command.h"

#include <string>

namespace zaplet::cli
{
    class DeleteCommand final : public Command
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

#endif // DELETE_H
