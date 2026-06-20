#include <ncurses.h>
#include "../include/common.h"
#include "../include/render.h"

void init_render(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(33);
}

void draw_game(void) {
    pthread_mutex_lock(&game.state_mutex);

    erase();

    for (int i = 0; i < SCREEN_WIDTH; i++) {
        mvaddch(0, i, '#');
        mvaddch(SCREEN_HEIGHT - 1, i, '#');
    }
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        mvaddch(i, 0, '#');
        mvaddch(i, SCREEN_WIDTH - 1, '#');

    
    }

    int win_thresh = (game.total_aliens + 1) / 2;
    int lose_thresh =
    game.total_aliens / 2 + 1;
    
    mvprintw(1, 2, "Foguetes: %d/%d  |  Abatidas: %d/%d  |  Escapadas: %d/%d",
    
    game.rockets_in_battery,
    game.max_launchers,

    game.aliens_destroyed,
    win_thresh,
    game.aliens_escaped,
    lose_thresh) ;

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

    int ax = SCREEN_WIDTH / 2; 
    int ay = SCREEN_HEIGHT - 2;
    for (int step = 1; step <= 8; step++) {
    if      (game.current_angle == ANGLE_VERTICAL)    { ay--; }
    else if (game.current_angle == ANGLE_DIAG_LEFT)   { ay--; ax--; }
    else if (game.current_angle == ANGLE_DIAG_RIGHT)  { ay--; ax++; }
    else if (game.current_angle == ANGLE_HORIZ_LEFT)  { ax--; }
    else if (game.current_angle == ANGLE_HORIZ_RIGHT) { ax++; }
    if (ay > 0 && ax > 0 && ax < SCREEN_WIDTH - 1)
        mvaddch(ay, ax, '.');
}
     for (int i = 0; i < MAX_ROCKETS; i++) {
        if (game.pool_rockets[i].active) {
            mvaddch(game.pool_rockets[i].pos.y, game.pool_rockets[i].pos.x, '*');
        }
    }

    refresh();

    pthread_mutex_unlock(&game.state_mutex);
}

void cleanup_render(void) {
    endwin();
}