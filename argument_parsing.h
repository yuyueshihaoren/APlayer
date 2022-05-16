// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
#ifndef APLAYER_CPP_CLION_ARGUMENT_PARSING_H
#define APLAYER_CPP_CLION_ARGUMENT_PARSING_H

#include <argp.h>

struct arguments {
    char *args[1];
    int verbose;
    int dangerous;
    const char *mode;
    int log_level;
    long blk_size;
    const char *custom_command;
};

extern struct argp argp;

// Parse options (Free args don't go through this function)
error_t parse_opt(int key, char *arg, struct argp_state *state);

// Stuff to do before calling argp_parse() in main()
// Return a struct arguments containing default values
struct arguments args_init();

#endif //APLAYER_CPP_CLION_ARGUMENT_PARSING_H
