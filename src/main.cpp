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
           -c               display controls

)EOF",
           progname);
}

void controls()
{
    printf(R"EOF(
Controls:
    Keyboard:
           wasd         move the camera around
           space        hold to run simulation as fast as possible
           c            press to advance by one simulation step

    Mouse:
           Left         hold to use current drawing tool
           Right        cycle through tools
           Wheel        zoom in/out

)EOF");
}

int main(int argc, char **argv)
{
    int width = 1000;
    int height = 600;

    char o;
    while ((o = getopt(argc, argv, "w:h:c")) != -1)
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
            case 'c':
                controls();
                return 0;
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

    Conway game("Here's Conway 👀", width, height);
    game.run();

    return 0;
}
