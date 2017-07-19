#include <iostream>
#include <exception>
#include <string>
#include "vm_def.h"
#include "utils.h"
#include <SDL2/SDL.h>
#include "cpu.h"
#include <cstring>

static void show_help()
{
    std::cout << "chip8emu is an emulator for the chip 8 VM.\n";
    std::cout << "The only required argument is the input .rom file.\n";
    std::cout << "Here are the supported options:\n";
    std::cout << "   --help | -h -- displays this help screen\n";
}

static void draw(SDL_Window *win, uint8_t *gfx)
{
    SDL_Surface *surface = SDL_GetWindowSurface(win);
    uint8_t *pixels = static_cast<uint8_t*>(surface->pixels);
    std::memset(pixels, 0, surface->w * surface->h * sizeof(*pixels));

    for (uint8_t x = 0; x < CHIP8_PIXELS_WIDTH; ++x) {
        for (uint8_t y = 0; y < CHIP8_PIXELS_HEIGHT; ++y) {
            pixels[y + (x * surface->w)] = gfx[(y / 10) + (x / 10) * 64] ? 0xFF : 0;
        }
    }
    SDL_UpdateWindowSurface(win);
}

int main(int argc, char **argv)
{
    try {
        if (argc < 2) {
            show_help();
            return EXIT_FAILURE;
        }

        if (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0) {
            show_help();
            return EXIT_SUCCESS;
        }

        std::cout << "Reading from '" << argv[1] << "'\n";

        std::FILE *rom = std::fopen(argv[1], "rb");
        if (!rom) {
            std::cerr << "Couldn't open "  << argv[1] << " for reading!\n";
            return EXIT_FAILURE;
        }
        CPU cpu(rom);
        std::fclose(rom);

        SDL_Init(SDL_INIT_VIDEO);
        SDL_Window *win = SDL_CreateWindow(
            "Chip8 Emulator",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            CHIP8_WINDOW_WIDTH,
            CHIP8_WINDOW_HEIGHT,
            0
        );

        if (!win) {
            std::cerr << "Error creating window! Error: " << SDL_GetError() << "\n";
            return EXIT_FAILURE;
        }

        bool isRunning = true;
        while (isRunning) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    isRunning = false;
                }
            }
            cpu.emulate_cycle();
            if (cpu.need_draw) {
                draw(win, cpu.gfx);
            }
        }

        SDL_DestroyWindow(win);
        SDL_Quit();

    } catch (const std::exception& e) {
        std::cerr << "Caught generic exception: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Unknown error! Please retry!\n";
    }
}
