#pragma once

#include <iostream>

#define VLOG(x, y) do {\
        if ((x).verbose) {\
            std::cout << y << '\n';\
        }\
    } while(0)

#define V_UNUSED(x) ((void)(x))

struct AsmOpts {
    const char *in_file;
    bool show_help, verbose;

    AsmOpts(const char *in = "", bool show_help = false, bool verbose = false) :
        in_file(in), show_help(show_help), verbose(verbose) {}
};