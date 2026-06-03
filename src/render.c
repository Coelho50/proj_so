#include <ncurses.h>
#include "../include/common.h"
#include "../include/render.h"

void init_render(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(0);
}

void draw_game(void) {
    pthread_mutex_lock(&game.state_mutex);

    clear();

    for (int i = 0; i < SCREEN_WIDTH; i++) {
        mvaddch(0, i, '#');
        mvaddch(SCREEN_HEIGHT - 1, i, '#');
    }
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        mvaddch(i, 0, '#');
        mvaddch(i, SCREEN_WIDTH - 1, '#');
    }

    mvprintw(1, 2, "Foguetes na Bateria: %d / %d", game.rockets_in_battery, game.max_launchers);
    mvprintw(2, 2, "Naves Abatidas: %d / %d", game.aliens_destroyed, game.total_aliens);
    mvprintw(3, 2, "Naves Escaparam: %d", game.aliens_escaped);

    for (int i = 0; i < MAX_ALIENS_CONCURRENT; i++) {
        if (game.pool_aliens[i].active) {
            mvaddch(game.pool_aliens[i].pos.y, game.pool_aliens[i].pos.x, 'W');
        }
    }

    char angle_char = '|';
    if (game.current_angle == ANGLE_HORIZ_LEFT) angle_char = '_';
    else if (game.current_angle == ANGLE_DIAG_LEFT) angle_char = '\\';
    else if (game.current_angle == ANGLE_VERTICAL) angle_char = '|';
    else if (game.current_angle == ANGLE_DIAG_RIGHT) angle_char = '/';
    else if (game.current_angle == ANGLE_HORIZ_RIGHT) angle_char = '_';

    mvaddch(SCREEN_HEIGHT - 2, SCREEN_WIDTH / 2, angle_char);

    refresh();

    pthread_mutex_unlock(&game.state_mutex);
}

void cleanup_render(void) {
    endwin();
}