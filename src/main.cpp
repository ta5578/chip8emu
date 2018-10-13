#include <iostream>
#include <SDL2/SDL.h>
#include "cpu.h"
#include <cstring>
#include "utils.h"

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
    SDL_LockSurface(surface);
    uint32_t *pixels = static_cast<uint32_t*>(surface->pixels);
    std::memset(pixels, 0, surface->w * surface->h * sizeof(*pixels));

    for (uint32_t i = 0; i < CHIP8_WINDOW_HEIGHT; ++i) {
        for (uint32_t j = 0; j < CHIP8_WINDOW_WIDTH; ++j) {
            pixels[j + i * surface->w] = gfx[(j / CHIP8_WINDOW_SCALAR) + (i / CHIP8_WINDOW_SCALAR) * CHIP8_PIXELS_WIDTH] ? 0xFFFFFFFF : 0;
        }
    }
    SDL_UnlockSurface(surface);
    SDL_UpdateWindowSurface(win);
    SDL_Delay(15);
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

        ROM rom = read_bin_file(argv[1]);
        CPU cpu(std::move(rom));

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "Couldn't initialize SDL!\n";
            return EXIT_FAILURE;
        }

        SDL_Window *win = SDL_CreateWindow(
            "Chip8 Emulator",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
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
            if (cpu.needsDraw()) {
                draw(win, cpu.getGFX());
            }
        }

        SDL_DestroyWindow(win);
        SDL_Quit();

    } catch (...) {
        std::cerr << "Unknown error! Please retry!\n";
    }
}
