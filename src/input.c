#include <ncurses.h>
#include "../include/common.h"
#include "../include/input.h"

void* input_thread_fn(void* arg) {
    (void)arg;
    int ch;

    while (1) {
        pthread_mutex_lock(&game.state_mutex);
        if (game.game_over) {
            pthread_mutex_unlock(&game.state_mutex);
            break;
        }
        pthread_mutex_unlock(&game.state_mutex);

        ch = getch();

        if (ch == ERR) {
            continue;
        }

        pthread_mutex_lock(&game.state_mutex);

        if (ch == KEY_LEFT) {
            if (game.current_angle > ANGLE_HORIZ_LEFT) {
                game.current_angle--;
            }
        } else if (ch == KEY_RIGHT) {
            if (game.current_angle < ANGLE_HORIZ_RIGHT) {
                game.current_angle++;
            }
        } else if (ch == ' ') {
            if (game.rockets_in_battery > 0) {
                game.rockets_in_battery--;
                
                // aloca espaço para passar as coordenadas iniciais para a thread
                Rocket* novo_foguete = malloc(sizeof(Rocket));
                novo_foguete->pos.x = SCREEN_WIDTH / 2;
                novo_foguete->pos.y = SCREEN_HEIGHT - 3;
                novo_foguete->trajectory = game.current_angle;
                
                pthread_t rocket_tid;
                if (pthread_create(&rocket_tid, NULL, rocket_thread_fn, novo_foguete) == 0) {
                    pthread_detach(rocket_tid); 
                } else {
                    free(novo_foguete);
                }
            }
        }
            }
        } else if (ch == 'q' || ch == 'Q') {
            game.game_over = true;
        }

        pthread_mutex_unlock(&game.state_mutex);
    }

    return NULL;
}