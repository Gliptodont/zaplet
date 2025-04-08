/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include <zaplet/zaplet.h>

#include <iostream>

int main()
{
    try
    {
        LOG_INITIALIZE_FROM_INI("config/logger.conf");

    } catch (const std::exception& e)
    {
        std::cerr << "Fatal: " << e.what() << std::endl;
    }

    return 0;
}
