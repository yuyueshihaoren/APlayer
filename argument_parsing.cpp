// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
#include "argument_parsing.h"

#include <cstdlib>

// Here are GNU argp globals
const char *argp_program_version = "APlayer 0.1";
const char *argp_program_bug_address = "[Github Issues]";

// My filescope variables
char doc[] = "A simple program using libmpv, defining custom mpv protocols\n"
             "Modes include:\n"
             "----------------------\n"
                "\t\"popen-gpg\"\n"
                    "\t\tSame as --custom-popen-command=\"gpg -d\", decrypt and play GnuPG via pipe (see security note)\n"
                "\t\"popen-cat\"\n"
                    "\t\tWho doesn't want to popen a cat?, same as `aplayer --dangerous --mode=custom-popen-command --custom-command=cat [MEDIA_FILE]`\n"
                "\t\"regular-file\"\n"
                    "\t\tPlay a regular multimedia file"
            "The following modes need --dangerous\n"
                "\t\"custom-popen-command\"\n"
                    "\t\tcommand-string will be executed in a shell and its stdout will be feed in mpv, [MEDIA_FILE] will be where to specify the command\n"
                "\t\"binary-split\"\n"
                    "\t\tplay a folder filled with output of command split -b=bytes, file names must start with x and ordered alphabetically. bytes option takes integer\n"
            "----EXAMPLES-----------\n"
            "$ ./aplayer --mode=popen-gpg --blk-size=$((5*1024*1024)) ~/Videos/free_software_forever.mp4\n"

        ;
char args_doc[] = "MEDIA_FILE";

struct argp_option options[] = {
        // name         key         arg     flags   doc                 (group)
        {"verbose", 'v', 0, OPTION_ARG_OPTIONAL, "Mpv verbose output"},
        {"dangerous", 'D', 0, OPTION_ARG_OPTIONAL, "See popen-command"},
        {"mode", 'M', "mode-string", 0, "Specify modes, see --help for all modes, default regular-file"},
        {"blk-size", 'b',  "blk_size", 0,  "Integer block size (bytes)"},
        {"custom-command", 'C', "command", 0, "(Dangerous) custom command string"},
//        {"custom-popen-command", 'P', "command-string", 0, "(needs to turn on --dangerous) command-string will be executed in a shell and its stdout will be feed in mpv, file path is not used (but you have to specify it to work. (/dev/null works)"},
//        {"popen-gpg", 'G', 0, OPTION_ARG_OPTIONAL, "Same as --custom-popen-command=\"gpg -d\", decrypt and play GnuPG via pipe (see security note)"},
//        {"regular-file", 'N', 0, OPTION_ARG_OPTIONAL, "play a regular multimedia file"},
//        {"binary-split", 'S', "bytes", 0, "(needs to turn on --dangerous) play a folder filled with output of command split -b=bytes, file names must start with x and ordered alphabetically. bytes option takes integer"},
        {"log-level", 'L', "level", 0, "Log level in integers\n"
                "NONE = 0,"
                "FATAL = 10,"
                "ERROR = 20,"
                "WARN = 30,"
                "INFO = 40,"
                "V = 50,"
                "DEBUG = 60,"
                "TRACE = 70,"
            },
        {0} // Terminate
};

struct argp argp = {options, parse_opt, args_doc, doc};


static long positive_atol(const char *input_str) {
    using namespace std;
    char *end;
    const long number = strtol(input_str, &end, 10);
    if (input_str == end) {
        return -1;
    }
    return number;
}

// Parse options (Free args don't go through this function)
error_t parse_opt(int key, char *arg, struct argp_state *state) {
    const int EXACT_NUM_ARGS = 1;
    struct arguments *arguments = (struct arguments *) state->input;

    switch (key) {
        case 'v':
            arguments->verbose = 1;
            break;
        case 'D':
            arguments->dangerous = 1;
            break;
        case 'M':
            arguments->mode = arg;
            break;
        case 'b':
            if ((arguments->blk_size = positive_atol(arg)) < 0)
            {
                argp_usage(state);
            }
        case 'L':
            if ((arguments->log_level = positive_atol(arg)) < 0) {
                argp_usage(state);
            }
            break;
        case 'C':
            arguments->custom_command = arg;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= EXACT_NUM_ARGS) {
                argp_usage(state);
            }
            arguments->args[state->arg_num] = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < EXACT_NUM_ARGS) {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
            break;
    }

    return 0;
}

// Stuff to do before calling argp_parse() in main()
// Return a struct arguments containing default values
struct arguments args_init() {
    // Create such struct
    struct arguments arguments;
    // Setting default values
    arguments.verbose = 0;
    arguments.mode = "regular-file";
    arguments.log_level = 40;
    arguments.dangerous = 0;
    arguments.blk_size = 0;
    return arguments;
}