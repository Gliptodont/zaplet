/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "cli/application.h"
#include "cli/commands/http/delete.h"
#include "cli/commands/http/get.h"
#include "cli/commands/http/head.h"
#include "cli/commands/http/options.h"
#include "cli/commands/http/patch.h"
#include "cli/commands/http/post.h"
#include "cli/commands/http/put.h"

#include <format>
#include <iostream>


namespace zaplet::cli
{
    Application::Application(int argc, char** argv)
        : m_argc(argc)
        , m_argv(argv)
        , m_client(std::make_shared<http::Client>())
    {
        m_cliApp.description(ASCII_ART);

        m_formatter = output::FormatterFactory::create(m_outputFormat);

        setupCommands();
    }

    int Application::run()
    {
        try
        {
            m_cliApp.final_callback([&]()
            {
                logging::Logger::getInstance().flush();
            });
            m_cliApp.parse(m_argc, m_argv);

            parseGlobalOptions();

            if (m_cliApp.get_subcommands().empty() && m_cliApp.count("--help") == 0)
            {
                std::cout << std::format("{}\n", m_cliApp.help()) << std::endl;
                return 0;
            }

            return 0;
        } catch (const CLI::ParseError& e)
        {
            return m_cliApp.exit(e);
        }
    }

    void Application::setupCommands()
    {
        auto get = m_cliApp.add_subcommand("get", "Execute HTTP GET request");
        auto getCmd = std::make_unique<GetCommand>(get, m_client, m_formatter);
        getCmd->setupOptions();
        m_commands.push_back(std::move(getCmd));

        m_cliApp.require_subcommand(1);
    }

    void Application::parseGlobalOptions() const
    {
    }
} // namespace zaplet::cli