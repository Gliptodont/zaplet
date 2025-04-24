/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "cli/commands/command.h"

#include <CLI/CLI.hpp>
#include <zaplet/zaplet.h>

#include <memory>
#include <string>
#include <vector>

namespace zaplet::cli
{
    const std::string ASCII_ART = R"(
                 _      _
  ______ _ _ __ | | ___| |_
 |_  / _` | '_ \| |/ _ \ __|
  / / (_| | |_) | |  __/ |_
 /___\__,_| .__/|_|\___|\__|
          |_|
)";


    class Application
    {
    public:
        Application(int argc, char** argv);
        ~Application() = default;

        int run();

    private:
        int m_argc;
        char** m_argv;
        CLI::App m_cliApp;
        std::vector<std::unique_ptr<Command>> m_commands;
        std::shared_ptr<http::Client> m_client;
        std::shared_ptr<output::Formatter> m_formatter;

        std::string m_outputFormat = "yaml";

        void setupCommands();
        void parseGlobalOptions();
    };
} // namespace zaplet::cli

#endif // APPLICATION_H
