/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "cli/application.h"

#include <zaplet/zaplet.h>

#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        LOG_INITIALIZE_FROM_INI("config/logger.conf");

        zaplet::cli::Application app(argc, argv);

        return app.run();
    } catch (const std::exception& e)
    {
        std::cerr << "Fatal: " << e.what() << std::endl;
    } catch (...)
    {
        std::cerr << "Unknown error occurred" << std::endl;
    }

    return 0;
}
