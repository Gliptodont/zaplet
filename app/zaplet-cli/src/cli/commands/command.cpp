/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "cli/commands/command.h"

namespace zaplet::cli
{
    Command::Command(CLI::App* app, std::shared_ptr<zaplet::http::Client> httpClient, std::shared_ptr<output::Formatter> formatter)
        : m_app(app)
        , m_client(std::move(httpClient))
        , m_formatter(std::move(formatter))
    {
        m_app->callback(
            [this]()
            {
                this->execute();
            });
    }

    std::string Command::name() const
    {
        return m_app->get_name();
    }
} // namespace zaplet::cli
