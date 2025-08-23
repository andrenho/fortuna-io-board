#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct Config {
    float zoom;
    bool  nearest;
} Config;

extern Config config;

void config_load(int argc, char* argv[]);

#endif //CONFIG_H
