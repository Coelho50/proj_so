#include <unistd.h>
#include <stdlib.h>
#include "../include/common.h"
#include "../include/entities.h"

void* reloader_thread_fn(void* arg) {
    (void)arg;

    while (1) {
        pthread_mutex_lock(&game.state_mutex);
        if (game.game_over) {
            pthread_mutex_unlock(&game.state_mutex);
            break;
        }
        
        bool precisa_recarregar = (game.rockets_in_battery < game.max_launchers);
        pthread_mutex_unlock(&game.state_mutex);

        if (precisa_recarregar) {
            usleep(game.reload_delay_ms * 1000);

            pthread_mutex_lock(&game.state_mutex);
            if (game.game_over) {
                pthread_mutex_unlock(&game.state_mutex);
                break;
            }
            
            if (game.rockets_in_battery < game.max_launchers) {
                game.rockets_in_battery++;
            }
            pthread_mutex_unlock(&game.state_mutex);
        } else {
            usleep(50000);
        }
    }

    return NULL;
}

void* alien_thread_fn(void* arg) {
    int pool_index = *(int*)arg;
    free(arg);

    while (1) {
        usleep(game.alien_speed_ms * 1000);

        pthread_mutex_lock(&game.state_mutex);
        if (game.game_over) {
            game.pool_aliens[pool_index].active = false;
            pthread_mutex_unlock(&game.state_mutex);
            break;
        }

        game.pool_aliens[pool_index].pos.y++;

        if (game.pool_aliens[pool_index].pos.y >= SCREEN_HEIGHT - 2) {
            game.pool_aliens[pool_index].active = false;
            game.aliens_escaped++;
            pthread_mutex_unlock(&game.state_mutex);
            break;
        }

        pthread_mutex_unlock(&game.state_mutex);
    }

    return NULL;
}



void* rocket_thread_fn(void* arg) {
    int pool_index = *(int*)arg;
    free(arg);

    pthread_mutex_lock(&game.state_mutex);
    int x = game.pool_rockets[pool_index].pos.x;
    int y = game.pool_rockets[pool_index].pos.y;
    CannonAngle angle = game.pool_rockets[pool_index].trajectory;
    pthread_mutex_unlock(&game.state_mutex);

    while (1) {
        usleep(20000);

        pthread_mutex_lock(&game.state_mutex);

        if (game.game_over) {
            game.pool_rockets[pool_index].active = false;
            pthread_mutex_unlock(&game.state_mutex);
            break;
        }

        if (angle == ANGLE_VERTICAL)         { y--; }
        else if (angle == ANGLE_DIAG_LEFT)   { y--; x--; }
        else if (angle == ANGLE_DIAG_RIGHT)  { y--; x++; }
        else if (angle == ANGLE_HORIZ_LEFT)  { x--; }
        else if (angle == ANGLE_HORIZ_RIGHT) { x++; }

        game.pool_rockets[pool_index].pos.x = x;
        game.pool_rockets[pool_index].pos.y = y;

        if (y <= 0 || x <= 0 || x >= SCREEN_WIDTH - 1) {
            game.pool_rockets[pool_index].active = false;
            pthread_mutex_unlock(&game.state_mutex);
            break;
        }

        bool hit = false;
        for (int i = 0; i < MAX_ALIENS_CONCURRENT; i++) {
            if (game.pool_aliens[i].active &&
               (game.pool_aliens[i].pos.x == x     ||
                game.pool_aliens[i].pos.x == x + 1 ||
                game.pool_aliens[i].pos.x == x - 1) &&
               (game.pool_aliens[i].pos.y == y     ||
                game.pool_aliens[i].pos.y == y + 1 ||
                game.pool_aliens[i].pos.y == y - 1)) {
                game.pool_aliens[i].active = false;
                game.aliens_destroyed++;
                hit = true;
                break;
            }
        }

        if (hit) {
            game.pool_rockets[pool_index].active = false;
            pthread_mutex_unlock(&game.state_mutex);
            break;
        }

        pthread_mutex_unlock(&game.state_mutex);
    }

    return NULL;
}
