/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <../../../../../build/debug/_deps/cli11-src/include/CLI/CLI.hpp>
#include <../../../../../lib/zaplet/include/zaplet/zaplet.h>

#include <memory>
#include <string>

namespace zaplet::cli
{
    class Command
    {
    public:
        Command(CLI::App* app, std::shared_ptr<zaplet::http::Client> httpClient, std::shared_ptr<output::Formatter> formatter);
        virtual ~Command() = default;

        [[nodiscard]] std::string name() const;

        virtual void setupOptions() = 0;

    protected:
        CLI::App* m_app;
        std::shared_ptr<http::Client> m_client;
        std::shared_ptr<output::Formatter> m_formatter;

        virtual void execute() = 0;
    };
} // namespace zaplet::cli

#endif // COMMAND_H
