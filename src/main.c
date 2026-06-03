#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "../include/common.h"
#include "../include/config.h"
#include "../include/main.h"
#include "../include/render.h"
#include "../include/input.h"
#include "../include/entities.h"

GameState game;

void init_game(int difficulty_choice) {
    DifficultyConfig config;
    
    if (difficulty_choice == 1) {
        config = EASY_CONFIG;
    } else if (difficulty_choice == 2) {
        config = MEDIUM_CONFIG;
    } else {
        config = HARD_CONFIG;
    }

    game.rockets_in_battery = config.max_launchers;
    game.current_angle = ANGLE_VERTICAL;
    game.total_aliens_spawned = 0;
    game.aliens_destroyed = 0;
    game.aliens_escaped = 0;
    game.game_over = false;
    game.player_won = false;

    game.max_launchers = config.max_launchers;
    game.total_aliens = config.total_aliens;
    game.reload_delay_ms = config.reload_delay_ms;
    game.alien_speed_ms = config.alien_speed_ms;

    for (int i = 0; i < MAX_ALIENS_CONCURRENT; i++) {
        game.pool_aliens[i].active = false;
    }

    pthread_mutex_init(&game.state_mutex, NULL);
    sem_init(&game.battery_sem, 0, game.max_launchers);
}

void check_victory_conditions(void) {
    pthread_mutex_lock(&game.state_mutex);
    
    int win_threshold = (game.total_aliens + 1) / 2; 

    if (game.aliens_destroyed >= win_threshold) {
        game.game_over = true;
        game.player_won = true;
    } else if (game.aliens_escaped >= win_threshold) {
        game.game_over = true;
        game.player_won = false;
    }
    
    pthread_mutex_unlock(&game.state_mutex);
}

void cleanup_game(void) {
    for (int i = 0; i < MAX_ALIENS_CONCURRENT; i++) {
        if (game.pool_aliens[i].active) {
            pthread_join(game.pool_aliens[i].thread_id, NULL);
        }
    }
    pthread_mutex_destroy(&game.state_mutex);
    sem_destroy(&game.battery_sem);
}

int main(void) {
    int choice = 1;
    pthread_t input_tid;
    pthread_t reloader_tid;
    unsigned int spawn_timer = 0;
    
    srand(time(NULL));
    
    printf("Selecione a dificuldade:\n1 - Facil\n2 - Medio\n3 - Dificil\nEscolha: ");
    if (scanf("%d", &choice) != 1) {
        choice = 1;
    }
    
    init_game(choice);
    init_render();
    
    if (pthread_create(&input_tid, NULL, input_thread_fn, NULL) != 0) {
        cleanup_render();
        cleanup_game();
        fprintf(stderr, "Erro ao criar a thread de input.\n");
        return 1;
    }

    if (pthread_create(&reloader_tid, NULL, reloader_thread_fn, NULL) != 0) {
        game.game_over = true;
        pthread_join(input_tid, NULL);
        cleanup_render();
        cleanup_game();
        fprintf(stderr, "Erro ao criar a thread do carregador.\n");
        return 1;
    }
    
    while (!game.game_over) {
        check_victory_conditions();
        draw_game();
        usleep(33333);
        spawn_timer += 33;

        if (spawn_timer >= 1500) {
            spawn_timer = 0;
            
            pthread_mutex_lock(&game.state_mutex);
            if (game.total_aliens_spawned < game.total_aliens) {
                for (int i = 0; i < MAX_ALIENS_CONCURRENT; i++) {
                    if (!game.pool_aliens[i].active) {
                        game.pool_aliens[i].active = true;
                        game.pool_aliens[i].pos.y = 1;
                        game.pool_aliens[i].pos.x = 1 + (rand() % (SCREEN_WIDTH - 2));
                        game.total_aliens_spawned++;

                        int* arg = malloc(sizeof(int));
                        *arg = i;
                        pthread_create(&game.pool_aliens[i].thread_id, NULL, alien_thread_fn, arg);
                        break;
                    }
                }
            }
            pthread_mutex_unlock(&game.state_mutex);
        }
    }
    
    pthread_join(input_tid, NULL);
    pthread_join(reloader_tid, NULL);
    cleanup_render();
    
    if (game.player_won) {
        printf("Vitoria!\n");
    } else {
        printf("Derrota!\n");
    }
    
    cleanup_game();
    return 0;
}