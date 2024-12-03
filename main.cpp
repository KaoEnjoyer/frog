#include <iostream>
#include "structs.h"
#include <ncurses.h>
#include <signal.h>

#define NODE_BACKGROUND 0
#define NODE_FROG 1
#define NODE_CAR 2
#define NODE_ROCK 3

#define STATUS_HEIGHT 3

// default values
#define DEF_FROG_HEIGHT 2
#define DEF_FROG_LENGHT 2
#define DEF_FROG_SYMBOL ' '

// color macros
#define COLOR_BACKGROUND 1
#define COLOR_FROG 2
#define COLOR_ROCK 3
#define COLOR_WATER 4
#define COLOR_CAR_ENEMY 5
#define COLOR_CAR_TRICKY 6
#define COLOR_CAR_FREIND 7

//helper functions 


// constructors

WIN *Init_Window(){
    WIN *my_win = new WIN;
    my_win->win = initscr();
    if (my_win->win == NULL)
    {
        fprintf(stderr, "O CURSES NIE DZIALA");
        exit(-1);
    }
    my_win->border = true;
    my_win->x = 0;
    my_win->y = 0;
    my_win->cols = COLS;
    my_win->lines = LINES;
    noecho();
    start_color(); // initialize colors
    init_pair(COLOR_BACKGROUND, COLOR_WHITE, COLOR_WHITE);
    init_pair(COLOR_FROG, COLOR_GREEN, COLOR_GREEN);
    init_pair(COLOR_ROCK, COLOR_WHITE, COLOR_BLUE);
    init_pair(COLOR_WATER, COLOR_GREEN, COLOR_WHITE);
    init_pair(COLOR_CAR_ENEMY, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_CAR_TRICKY, COLOR_RED, COLOR_WHITE);
    init_pair(COLOR_CAR_FREIND, COLOR_RED, COLOR_BLUE);

    curs_set(0);
    return my_win;
}

WIN *Init_status_win(WIN *screen){
    WIN *new_win = new WIN;
    new_win->win = newwin(STATUS_HEIGHT, screen->cols, screen->lines - STATUS_HEIGHT, 0);
    new_win->lines = STATUS_HEIGHT;
    new_win->cols = screen->cols;
    new_win->x = 0;
    new_win->y = screen->lines - STATUS_HEIGHT + 1;
    box(new_win->win, 0, 0);
    return new_win;
}

WIN *Init_gameplay_win(WIN *screen)
{
    WIN *new_win = new WIN;
    new_win->lines = screen->lines - STATUS_HEIGHT;
    new_win->cols = screen->cols;
    new_win->x = 0;
    new_win->y = 0;
    new_win->win = newwin(new_win->lines, new_win->cols, new_win->y, new_win->x);
    box(new_win->win, 0, 0);
    return new_win;
}

Map *Init_map(WIN *window)
{
    Map *new_map = new Map;
    new_map->cols = window->cols;
    new_map->lines = window->lines;
    new_map->field = (int **)malloc(sizeof(int *) * new_map->lines);
    if (new_map->field == NULL)
    {
        fprintf(stderr, "malloc nie dziala");
        exit(-1);
    }
    wprintw(window->win, "cols : %d , lines : %d", new_map->cols, new_map->lines);
    for (int i = 0; i < new_map->lines; i++)
    {
        new_map->field[i] = (int *)malloc(sizeof(int) * new_map->cols);
        if (new_map->field[i] == NULL)
        {
            fprintf(stderr, "malloc nie dziala");
        }
        for (int j = 0; j < new_map->cols; j++)
        {
            new_map->field[i][j] = NODE_BACKGROUND;
        }
    }
    return new_map;
}

Frog *Init_frog(WIN *gameplay_win)
{
    Frog *new_frog = new Frog;
    new_frog->color = COLOR_FROG;
    new_frog->height = DEF_FROG_HEIGHT;
    new_frog->lenght = DEF_FROG_LENGHT;
    new_frog->symbol = DEF_FROG_SYMBOL;

    // setting frogs x , y to center frog and don't let it to go over the screen :)
    new_frog->x = gameplay_win->cols / 2;
    new_frog->y = gameplay_win->lines - new_frog->height;

    return new_frog;
}

// destructors

void Close_window(WIN *win)
{
    delwin(win->win);
    delete (win);
}

void Close_game(WIN *screen)
{
    endwin();
    delete screen;
    exit(0);
}

// status window functions

void Clear_screen(WIN *screen)
{
    for (int i = 0; i < screen->lines; i++)
    {
        wmove(screen->win, i, 0);
        for (int j = 0; j < screen->cols; j++)
        {
            waddch(screen->win, ' ');
        }
    }
    wrefresh(screen->win);
}

void Refresh_game(Game *Game_instance)
{
    wrefresh(Game_instance->gameplay_win->win);
    wrefresh(Game_instance->status_win->win);
}

void Move_frog(jump_direction dir, Game *Game_instance)
{

    // we store data of last position of frog
    // thats becasuse if frog would go outside of game window we will set it's last position
    unsigned int last_x = Game_instance->my_frog->x;
    unsigned int last_y = Game_instance->my_frog->y;

    // ncurses positioning looks like this:
    //     0 1 2 3 4 5 6 7 8 9
    //     _____________________\
    // 0   |                    / x
    // 1   |
    // 2   |
    // 3   |
    // 4   |
    //   y v
    // that's why in orded to move frog Up we need to modify y to y - 1

    // at this point it is important to say that a frog {x = 2 , y = 2 , height = 2 , lenght = 2} 
    //will look like this:
    //     0 1 2 3 4 5 6 7 8 9
    //     _____________________\
    // 0   |                    / x
    // 1   |
    // 2   |   f f
    // 3   |   f f 
    // 4   |
    //   y v

    switch (dir)
    {
    case jump_direction::UP:
        Game_instance->my_frog->y--;
        break;

    case jump_direction::DOWN:
        Game_instance->my_frog->y++;
        break;

    case jump_direction::LEFT:
        Game_instance->my_frog->x--;
        break;

    case jump_direction::RIGHT:
        Game_instance->my_frog->x++;
        break;
    default:
        break;
    }
    if(Game_instance->my_frog->x < Game_instance->my_frog->lenght 
    || Game_instance->my_frog->x >= (Game_instance->gameplay_win->cols - Game_instance->my_frog->lenght)){
        Game_instance->my_frog->x = last_x;
    }
    if(Game_instance->my_frog->y >= (Game_instance->gameplay_win->lines - Game_instance->my_frog->height) 
    || Game_instance->my_frog->y < Game_instance->my_frog->height){
        Game_instance->my_frog->y = last_y;
    }


}

void Handle_input(Game *game_instance)
{
    int ch = wgetch(game_instance->gameplay_win->win);
    switch (ch)
    {
    case 'Q':
        game_instance->is_over = true;
        break;
    case 'w':
    case KEY_UP:
        Move_frog(jump_direction::UP , game_instance);
        break;
    case 's':
    case KEY_DOWN:
        Move_frog(jump_direction::DOWN , game_instance);
        break;
    case 'd':
    case KEY_RIGHT:
        Move_frog(jump_direction::RIGHT , game_instance);
        break;
    case 'a':
    case KEY_LEFT:
        Move_frog(jump_direction::LEFT , game_instance);
        break;
    default:
        break;
    }
}

void Update_status_win(Game* game_instance){
    // mvwprintw(game_instance->status_win->win , game_instance->status_win->y , 0,
    // "frog x: %d , frog y: %d , lines: %d , cols: %d",
    // game_instance->my_frog->x,
    // game_instance->my_frog->y,
    // game_instance->gameplay_win->lines,
    // game_instance->gameplay_win->cols
    // );


    mvwaddstr(game_instance->status_win->win , game_instance->status_win->y , 0 , "  dadadawda                                                ");
    //  wprintw(game_instance->status_win->win ,
    //  "frog x: %d , frog y: %d , lines: %d , cols: %d",
    //  game_instance->my_frog->x,
    //  game_instance->my_frog->y,
    //  game_instance->gameplay_win->lines,
    //  game_instance->gameplay_win->cols
    //  );

   // waddstr(game_instance->status_win->win , "FROG");

}

void Game_loop(Game *game_instance)
{
    nodelay(game_instance->gameplay_win->win, true);
    while (!(game_instance->is_over))
    {
        Handle_input(game_instance);
        //exit(game_instance->my_frog->x);
        // Update_gameplay_scr(game_instance);
        Update_status_win(game_instance);
        //waddstr(game_instance->gameplay_win->win, "ww");
        Refresh_game(game_instance);
    }
}

void Enter_game(WIN *screen)
{
    Clear_screen(screen);
    Game *game_insatnce = new Game;
    // game instant is a stack allocated data because there is no need to store it on the heap becasue is is only for storing pointers to all game objects

    WIN *status_win = Init_status_win(screen);
    WIN *gameplay_win = Init_gameplay_win(screen);
    Frog *frog_instance = Init_frog(gameplay_win);
    Map *map_instance = Init_map(gameplay_win);

    // setting frog in the middle of screen

    game_insatnce->gameplay_win = gameplay_win;
    game_insatnce->my_frog = frog_instance;
    game_insatnce->status_win = status_win;
    game_insatnce->my_map = map_instance;
    game_insatnce->my_timer = new Timer;
    game_insatnce->is_over = false;

    Refresh_game(game_insatnce);
    getch();
    Game_loop(game_insatnce);

    Close_window(status_win);
    Close_window(gameplay_win);
    return;
}

void Start_screen(WIN *screen)
{
    int ch;
    do
    {
        Clear_screen(screen);
        wmove(screen->win, screen->lines / 2 - 4, screen->cols / 2 - 9);
        waddstr(screen->win, "WELCOME TO FROGGER!!");
        wmove(screen->win, getcury(screen->win), screen->cols / 2 - 9);
        waddstr(screen->win, "TO PLAY TYPE 'P'\n");
        wmove(screen->win, getcury(screen->win), screen->cols / 2 - 9);
        waddstr(screen->win, "TO EXIT TYPE 'Q'\n");
        wmove(screen->win, getcury(screen->win), screen->cols / 2 - 9);

        waddstr(screen->win, "TO LOAD CONFIG FILE TYPE 'L'\n");
        wmove(screen->win, getcury(screen->win), screen->cols / 2 - 9);

        waddstr(screen->win, "TO LOAD SAVE FILE TYPE 'L'\n");

        switch (ch)
        {
        case 'P':
            Enter_game(screen);
            break;
        case 'Q':
            Close_game(screen);
            break;
        default:
            break;
        }

    } while (ch = wgetch(screen->win));
}

int main()
{
    WIN *main_window = Init_Window();
    Start_screen(main_window);
    endwin();
    exit(-1);
    delete (main_window);
}