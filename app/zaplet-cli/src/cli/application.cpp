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
#include "cli/commands/scenario/play.h"

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

        auto post = m_cliApp.add_subcommand("post", "Execute HTTP POST request");
        auto postCmd = std::make_unique<PostCommand>(post, m_client, m_formatter);
        postCmd->setupOptions();
        m_commands.push_back(std::move(postCmd));

        auto put = m_cliApp.add_subcommand("put", "Execute HTTP PUT request");
        auto putCmd = std::make_unique<PutCommand>(put, m_client, m_formatter);
        putCmd->setupOptions();
        m_commands.push_back(std::move(putCmd));

        auto del = m_cliApp.add_subcommand("delete", "Execute HTTP DELETE request");
        auto delCmd = std::make_unique<DeleteCommand>(del, m_client, m_formatter);
        delCmd->setupOptions();
        m_commands.push_back(std::move(delCmd));

        auto patch = m_cliApp.add_subcommand("patch", "Execute HTTP PATCH request");
        auto patchCmd = std::make_unique<PatchCommand>(patch, m_client, m_formatter);
        patchCmd->setupOptions();
        m_commands.push_back(std::move(patchCmd));

        auto head = m_cliApp.add_subcommand("head", "Execute HTTP HEAD request");
        auto headCmd = std::make_unique<HeadCommand>(head, m_client, m_formatter);
        headCmd->setupOptions();
        m_commands.push_back(std::move(headCmd));

        auto opt = m_cliApp.add_subcommand("options", "Execute HTTP OPTIONS request");
        auto optCmd = std::make_unique<OptionsCommand>(opt, m_client, m_formatter);
        optCmd->setupOptions();
        m_commands.push_back(std::move(optCmd));

        auto play = m_cliApp.add_subcommand("play", "Execute the .zpl script");
        auto playCmd = std::make_unique<PlayCommand>(play, m_client, m_formatter);
        playCmd->setupOptions();
        m_commands.push_back(std::move(playCmd));

        m_cliApp.require_subcommand(1);
    }

    void Application::parseGlobalOptions() const
    {
    }
} // namespace zaplet::cli