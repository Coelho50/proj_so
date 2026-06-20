#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>
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
        game.pool_aliens[i].thread_started = false;
    }
    for (int i = 0; i < MAX_ROCKETS; i++) {
        game.pool_rockets[i].active = false;
        game.pool_rockets[i].thread_started = false;
    }

    pthread_mutex_init(&game.state_mutex, NULL);
}

void check_victory_conditions(void) {
    pthread_mutex_lock(&game.state_mutex);
    
    int win_threshold = (game.total_aliens + 1) / 2; 

    int lost_threshold = (game.total_aliens) / 2 + 1; 

    if (game.aliens_destroyed >= win_threshold) {
        game.game_over = true;
        game.player_won = true;
    } else if (game.aliens_escaped >= lost_threshold) {
        game.game_over = true;
        game.player_won = false;
    }
    
    pthread_mutex_unlock(&game.state_mutex);
}

void cleanup_game(void) {
    for (int i = 0; i < MAX_ALIENS_CONCURRENT; i++)
        if (game.pool_aliens[i].thread_started)
            pthread_join(game.pool_aliens[i].thread_id, NULL);

    for (int i = 0; i < MAX_ROCKETS; i++)
        if (game.pool_rockets[i].thread_started)
            pthread_join(game.pool_rockets[i].thread_id, NULL);

    pthread_mutex_destroy(&game.state_mutex);
}

int main(void) {
    int choice = 1;
    pthread_t reloader_tid;
    unsigned int spawn_timer = 0;

    srand(time(NULL));

    printf("Selecione a dificuldade:\n1 - Facil\n2 - Medio\n3 - Dificil\nEscolha: ");
    if (scanf("%d", &choice) != 1) {
        choice = 1;
    }

    init_game(choice);
    init_render();

    if (pthread_create(&reloader_tid, NULL, reloader_thread_fn, NULL) != 0) {
        cleanup_render();
        cleanup_game();
        fprintf(stderr, "Erro ao criar a thread do carregador.\n");
        return 1;
    }

    while (!game.game_over) {
        check_victory_conditions();
        draw_game();
        int ch = getch();
        if (ch != ERR)
            process_input(ch);
        spawn_timer += 33;

        if (spawn_timer >= 1500) {
            spawn_timer = 0;
            
            pthread_mutex_lock(&game.state_mutex);
            if (game.total_aliens_spawned < game.total_aliens) {
                for (int i = 0; i < MAX_ALIENS_CONCURRENT; i++) {
                    if (!game.pool_aliens[i].active) {
                        game.pool_aliens[i].active = true;
                        game.pool_aliens[i].pos.y = 2;
                        game.pool_aliens[i].pos.x = 1 + (rand() % (SCREEN_WIDTH - 2));
                        game.total_aliens_spawned++;

                        int* arg = malloc(sizeof(int));
                        *arg = i;
                        pthread_create(&game.pool_aliens[i].thread_id, NULL, alien_thread_fn, arg);
                        game.pool_aliens[i].thread_started = true;
                        break;
                    }
                }
            }
            pthread_mutex_unlock(&game.state_mutex);
        }
    }
    
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