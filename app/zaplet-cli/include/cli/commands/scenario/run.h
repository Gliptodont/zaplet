/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef RUN_H
#define RUN_H

#include "cli/commands/command.h"

#include <string>
#include <vector>

namespace zaplet::cli
{
    class RunScenarioCommand final : public Command
    {
    public:
        using Command::Command;

        void setupOptions() override;

    protected:
        void execute() override;

    private:
        std::string m_scenarioFile;
        std::vector<std::string> m_variables;
        bool m_continueOnError = false;
        int m_repeatCount = 0;
    };
} // namespace zaplet::cli

#endif // RUN_H
