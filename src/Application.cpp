#include <CLI/CLI.hpp>

int main(int argc, char** argv)
{
    CLI::App app("SumaTest");
    argv = app.ensure_utf8(argv);

    return 0;
}

