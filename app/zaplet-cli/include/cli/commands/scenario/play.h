/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef PLAY_H
#define PLAY_H

#include "cli/commands/command.h"

#include <zaplet/zaplet.h>

#include <string>
#include <vector>

namespace zaplet::cli
{
    class PlayCommand final : public Command
    {
    public:
        using Command::Command;

        void setupOptions() override;

    protected:
        void execute() override;

    private:
        std::string m_scenarioFile;
        std::vector<std::string> m_variables;
    };
}

#endif // PLAY_H
