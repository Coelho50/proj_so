#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int max_launchers;
    int total_aliens;
    unsigned int reload_delay_ms;
    unsigned int alien_speed_ms;
} DifficultyConfig;

static const DifficultyConfig EASY_CONFIG = {
    .max_launchers = 3,
    .total_aliens = 10,
    .reload_delay_ms = 1500,
    .alien_speed_ms = 400
};

static const DifficultyConfig MEDIUM_CONFIG = {
    .max_launchers = 5,
    .total_aliens = 20,
    .reload_delay_ms = 800,
    .alien_speed_ms = 250
};

static const DifficultyConfig HARD_CONFIG = {
    .max_launchers = 8,
    .total_aliens = 40,
    .reload_delay_ms = 400,
    .alien_speed_ms = 100
};

#endif // CONFIG_H