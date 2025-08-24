#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Config {
    float    zoom;
    bool     nearest;
    size_t   sd_ram_sz;
    char*    sd_image;
    bool     format;
} Config;

extern Config config;

void config_load(int argc, char* argv[]);

#endif //CONFIG_H
