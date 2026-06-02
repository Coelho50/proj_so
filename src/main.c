#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/common.h"
#include "../include/config.h"
#include "../include/main.h"
#include "../include/render.h"

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
    pthread_mutex_destroy(&game.state_mutex);
    sem_destroy(&game.battery_sem);
}

int main(void) {
    int choice = 1;
    
    printf("Selecione a dificuldade:\n1 - Facil\n2 - Medio\n3 - Dificil\nEscolha: ");
    if (scanf("%d", &choice) != 1) {
        choice = 1;
    }
    
    init_game(choice);
    init_render();
    
    while (!game.game_over) {
        check_victory_conditions();
        draw_game();
        usleep(33333);
    }
    
    cleanup_render();
    
    if (game.player_won) {
        printf("\nVitoria!\n");
    } else {
        printf("\nDerrota!\n");
    }
    
    cleanup_game();
    return 0;
}