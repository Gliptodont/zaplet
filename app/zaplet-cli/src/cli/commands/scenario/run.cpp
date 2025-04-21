/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "cli/commands/scenario/run.h"

#include <zaplet/zaplet.h>

#include <filesystem>
#include <format>
#include <iostream>

namespace zaplet::cli
{
    void RunScenarioCommand::setupOptions()
    {
        m_app->add_option("scenario_file", m_scenarioFile, "Path to scenario file (.zpl)")->required()->check(CLI::ExistingFile);
        m_app->add_option("-v,--variable", m_variables, "Set a variable in format name=value (can be specified multiple times)");
        m_app->add_flag("--continue-on-error", m_continueOnError, "Continue execution even if steps fail");
        m_app->add_option("-r,--repeat", m_repeatCount, "Number of times to repeat the scenario, overrides the value in the file")
            ->default_val(0);
    }

    void RunScenarioCommand::execute()
    {
        try
        {
            LOG_INFO_FMT("Running scenario from file: {}", m_scenarioFile);

            auto zslProcessor = scenario::zsl::ZslProcessor();
            auto scenarioOpt = zslProcessor.parseFile(m_scenarioFile);

            if (!scenarioOpt)
            {
                LOG_ERROR("Failed to parse scenario file");
                return;
            }

            auto scenario = scenarioOpt.value();

            if (m_continueOnError)
            {
                scenario.setContinueOnError(true);
            }

            if (m_repeatCount != 0)
            {
                scenario.setRepeatCount(m_repeatCount);
            }

            scenario::ScenarioRunner runner(m_client, m_formatter);

            for (const auto& var : m_variables)
            {
                auto eqPos = var.find('=');
                if (eqPos != std::string::npos)
                {
                    std::string name = var.substr(0, eqPos);
                    std::string value = var.substr(eqPos + 1);
                    runner.setGlobalVariable(name, value);
                    LOG_DEBUG_FMT("Set variable: {}={}", name, value);
                }
                else
                {
                    LOG_WARNING_FMT("Invalid variable format, expected name=value: {}", var);
                }
            }

            bool success = runner.run(scenario);

            if (success)
            {
                LOG_INFO("Scenario execution completed successfully");
            }
            else
            {
                LOG_ERROR_FMT("Scenario execution failed: {}", runner.getLastError());
            }
        }
        catch (const std::exception& e)
        {
            LOG_ERROR_FMT("Exception during scenario execution: {}", e.what());
        }
    }
} // namespace zaplet::cli