#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <stdbool.h>

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 24
#define MAX_ALIENS_CONCURRENT 20
#define MAX_ROCKETS 64

typedef enum {
    ANGLE_HORIZ_LEFT,
    ANGLE_DIAG_LEFT,
    ANGLE_VERTICAL,
    ANGLE_DIAG_RIGHT,
    ANGLE_HORIZ_RIGHT
} CannonAngle;

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    int id;
    Position pos;
    bool active;
    bool thread_started;
    pthread_t thread_id;
} Alien;

typedef struct {
    Position pos;
    CannonAngle trajectory;
    bool active;
    bool thread_started;
    pthread_t thread_id;
} Rocket;

typedef struct {
    int rockets_in_battery;
    CannonAngle current_angle;
    
    int total_aliens_spawned;
    int aliens_destroyed;
    int aliens_escaped;
    
    bool game_over;
    bool player_won;

    int max_launchers;
    int total_aliens;
    unsigned int reload_delay_ms;
    unsigned int alien_speed_ms;

    Alien pool_aliens[MAX_ALIENS_CONCURRENT];
    Rocket pool_rockets[MAX_ROCKETS];
    pthread_mutex_t state_mutex;
} GameState;

extern GameState game;

#endif // COMMON_H