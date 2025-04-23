/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "cli/application.h"

#include <zaplet/zaplet.h>

#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#endif

int main(int argc, char** argv)
{
    try
    {
#if defined(_WIN32)
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
#endif

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
