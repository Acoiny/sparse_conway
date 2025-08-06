#include "game.hpp"

#include <bits/getopt_core.h>
#include <getopt.h>
#include <stdexcept>
#include <string>

void usage(const char *progname)
{
    printf(R"EOF(
usage: %s <options>

options:
           -w <width>       sets the width of the window
           -h <height>      sets the height of the window

)EOF",
           progname);
}

int main(int argc, char **argv)
{
    int width = 1000;
    int height = 600;

    char o;
    while ((o = getopt(argc, argv, "w:h:")) != -1)
    {
        try
        {
            switch (o)
            {
            case 'w':
                width = std::stoi(optarg);
                break;
            case 'h':
                height = std::stoi(optarg);
                break;
            default:
                usage(argv[0]);
                return 1;
            }
        }
        catch (std::invalid_argument &ex)
        {
            printf("Invalid argument '%s' for option '%c'\n", optarg, o);
            return 1;
        }
    }

    Conway game("Conway's here", width, height);

    return game.run();
}
