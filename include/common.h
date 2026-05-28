#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 24

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
    pthread_t thread_id;
} Alien;

typedef struct {
    Position pos;
    CannonAngle trajectory;
    bool active;
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

    pthread_mutex_t state_mutex;
    sem_t battery_sem;
} GameState;

extern GameState game;

#endif // COMMON_H