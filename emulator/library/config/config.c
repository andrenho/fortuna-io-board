#include "config.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

Config config = {
    .zoom = 2.f,
    .nearest = false,
};

static void print_help(const char* program)
{
    printf("Usage: %s [OPTION]...\n", program);
    printf("Emulates the Fortuna I/O board <https://github.com/andrenho/fortuna-io-board>\n\n");
    printf("  -h, --help            Show this help\n");
    exit(EXIT_SUCCESS);
}

void config_load(int argc, char* argv[])
{
    while (true) {
        int option_index = 0;

        static struct option long_options[] = {
            { "help",    no_argument,       0, 'h' },
            { "zoom",    required_argument, 0, 'z' },
            { "nearest", required_argument, 0, 'N' },
            { NULL, 0, 0, 0 },
        };

        int c = getopt_long(argc, argv, "hz:N", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'h':
                print_help(argv[0]);
                break;
            case 'z':
                config.zoom = strtof(optarg, NULL);
                break;
            case 'N':
                config.nearest = true;
                break;
            case '?':
                break;
        }
    }

    if (optind < argc)
        print_help(argv[0]);
}
