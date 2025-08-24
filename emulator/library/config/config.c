#include "config.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config config = {
    .zoom = 2.f,
    .nearest = false,
    .sd_ram_sz = 64,
    .sd_image = NULL,
    .format = false,
    .panel = 0b00,
};

static void print_help(const char* program)
{
    printf("Usage: %s [OPTION]...\n", program);
    printf("Emulates the Fortuna I/O board <https://github.com/andrenho/fortuna-io-board>\n\n");
    printf("  -z, --zoom              Video zoom (default: 2.0)\n");
    printf("  -N, --nearest           Nearest filter (sharp pixels)\n");
    printf("  -m, --sd-ram [SIZE]     Use SDCard image in RAM (default), with SIZE in MB (default: 64)\n");
    printf("  -i, --sd-image [FILE]   Use FILE as SDCard image (instead of RAM)\n");
    printf("  -F, --format            Format SDCard image upon initialization (default true for RAM disk, false for files)\n");
    printf("  -P, --panel             Initial status of front panel DIP switches (format: 00)\n");
    printf("  -h, --help              Show this help\n");
    printf("Shortcuts:\n");
    printf("  CTRL + SHIFT + F10/F11  Front panel DIP switches\n");
    printf("  CTRL + SHIFT + F12      Front panel button\n");
    exit(EXIT_SUCCESS);
}

void config_load(int argc, char* argv[])
{
    while (true) {
        int option_index = 0;

        static struct option long_options[] = {
            { "help",       no_argument,       0, 'h' },
            { "zoom",       required_argument, 0, 'z' },
            { "sd-ram",     required_argument, 0, 'm' },
            { "sd-image",   required_argument, 0, 'i' },
            { "nearest",    required_argument, 0, 'N' },
            { "format",     required_argument, 0, 'F' },
            { "panel",      required_argument, 0, 'P' },
            { NULL, 0, 0, 0 },
        };

        int c = getopt_long(argc, argv, "hz:Nm:i:FP:", long_options, &option_index);
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
            case 'm':
                config.sd_ram_sz = strtoll(optarg, NULL, 10);
                break;
            case 'i':
                config.sd_image = calloc(1, strlen(optarg));
                strcpy(config.sd_image, optarg);
                break;
            case 'F':
                config.format = true;
                break;
            case 'P':
                if (optarg[0] == '1')
                    config.panel = 0b01;
                if (strlen(optarg) > 1 && optarg[1] == '1')
                    config.panel |= 0b10;
                break;
            case '?':
                break;
        }
    }

    if (optind < argc)
        print_help(argv[0]);
}
