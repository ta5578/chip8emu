#include <iostream>
#include <exception>
#include <string>
#include "vm_def.h"
#include <SDL2/SDL.h>

static bool parse_args(int argc, char **argv, AsmOpts *opts)
{
    if (argc < 2) {
        return false;
    }
    std::string first(argv[1]);
    if (first == "-h" || first == "--help") {
        opts->show_help = true;
        return true;
    }
    opts->in_file = argv[1];

    for (int i = 2; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--verbose" || arg == "-v") {
            opts->verbose = true;
        } else {
            return false;
        }
    }
    return true;
}

static void show_help()
{
    std::cout << "chip8emu is an emulator for the chip 8 VM.\n";
    std::cout << "The only required argument is the input .rom file.\n";
    std::cout << "Here are the supported options:\n";
    std::cout << "   --verbose | -v -- verbose output\n";
    std::cout << "   --help | -h -- displays this help screen\n";
}

int main(int argc, char **argv)
{
    try {
        AsmOpts opts;
        if (!parse_args(argc, argv, &opts)) {
            show_help();
            return EXIT_FAILURE;
        }

        if (opts.show_help) {
            show_help();
            return EXIT_SUCCESS;
        }

        std::cout << "Reading from '" << opts.in_file << "'\n";
        std::cout << "Verbose: " << (opts.verbose ? "true" : "false") << "\n";

        SDL_Init(SDL_INIT_VIDEO);

        SDL_Window *win;

        win = SDL_CreateWindow(
            "Chip8 Emulator",
            SDL_WINDOWPOS_UNDEFINED, /* Initial X Position */
            SDL_WINDOWPOS_UNDEFINED, /* Initial Y Position */
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL /* Flags */
        );

        if (!win) {
            throw std::invalid_argument("Couldn't create window!");
        }

        /* Now our window is open */

        SDL_Delay(3000); /* Pause execution for 3 seconds */

        /* Close the window and cleanup */
        SDL_DestroyWindow(win);
        SDL_Quit();

    } catch (const std::invalid_argument& e) {
        std::cerr << "Caught invalid argument: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Caught generic exception: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Unknown error! Please retry!\n";
    }
}
