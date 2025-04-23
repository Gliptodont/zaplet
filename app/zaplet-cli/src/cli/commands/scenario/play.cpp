/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "cli/commands/scenario/play.h"

#include "zaplet/scenario/yaml_parser.h"

namespace zaplet::cli
{
    void PlayCommand::setupOptions()
    {
        m_app->add_option("scenario_file", m_scenarioFile, "Scenario file to play")->required();
        m_app->add_option("-v,--variable", m_variables, "Variables in KEY=VALUE format (can be specified multiple times)");
    }

    void PlayCommand::execute()
    {
        LOG_INFO_FMT("Playing scenario from file: {}", m_scenarioFile);

        try
        {
            std::map<std::string, std::string> variables;
            for (const auto& var : m_variables)
            {
                auto pos = var.find('=');
                if (pos != std::string::npos)
                {
                    std::string key = var.substr(0, pos);
                    std::string value = var.substr(pos + 1);
                    variables[key] = value;
                    LOG_DEBUG_FMT("Variable: {}={}", key, value);
                }
                else
                {
                    LOG_WARNING_FMT("Invalid variable format: {}, should be KEY=VALUE", var);
                }
            }

            scenario::YamlParser parser;
            auto scenario = parser.parseFile(m_scenarioFile);

            auto env = scenario.getEnvironment();
            for (const auto& [key, value] : variables)
            {
                env[key] = value;
            }
            scenario.setEnvironment(env);

            scenario::Player player(m_client, m_formatter);
            bool success = player.play(scenario);

            if (success)
            {
                LOG_INFO("Scenario completed successfully");
            }
            else
            {
                LOG_ERROR("Scenario completed with errors");
            }
        }
        catch (const std::exception& e)
        {
            LOG_ERROR_FMT("Error playing scenario: {}", e.what());
        }
    }
} // namespace zaplet::cli