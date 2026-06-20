#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>
#include "../include/common.h"
#include "../include/entities.h"
#include "../include/input.h"

void process_input(int ch) {
    pthread_mutex_lock(&game.state_mutex);

    if (ch == KEY_LEFT) {
        if (game.current_angle > ANGLE_HORIZ_LEFT)
            game.current_angle--;
    } else if (ch == KEY_RIGHT) {
        if (game.current_angle < ANGLE_HORIZ_RIGHT)
            game.current_angle++;
    } else if (ch == ' ') {
        if (game.rockets_in_battery > 0) {
            int rocket_idx = -1;
            for (int i = 0; i < MAX_ROCKETS; i++) {
                if (!game.pool_rockets[i].active) {
                    rocket_idx = i;
                    break;
                }
            }
            if (rocket_idx != -1) {
                game.rockets_in_battery--;
                game.pool_rockets[rocket_idx].pos.x      = SCREEN_WIDTH / 2;
                game.pool_rockets[rocket_idx].pos.y      = SCREEN_HEIGHT - 3;
                game.pool_rockets[rocket_idx].trajectory = game.current_angle;
                game.pool_rockets[rocket_idx].active     = true;

                int *idx_arg = malloc(sizeof(int));
                *idx_arg = rocket_idx;
                if (pthread_create(&game.pool_rockets[rocket_idx].thread_id,
                                   NULL, rocket_thread_fn, idx_arg) == 0) {
                    game.pool_rockets[rocket_idx].thread_started = true;
                } else {
                    free(idx_arg);
                    game.pool_rockets[rocket_idx].active = false;
                    game.rockets_in_battery++;
                }
            }
        }
    } else if (ch == 'q' || ch == 'Q') {
        game.game_over = true;
    }

    pthread_mutex_unlock(&game.state_mutex);
}
