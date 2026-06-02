#include <unistd.h>
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