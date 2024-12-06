#include <iostream>
#include "structs.h"
#include <ncurses.h>
#include <signal.h>

#define NODE_BACKGROUND 0
#define NODE_FROG 1
#define NODE_CAR_ENEMY 2
#define NODE_CAR_FREINDLY 3
#define NODE_CAR_TRICKY 4
#define NODE_ROCK 5
#define NODE_END_OF_MAP 666

#define STATUS_HEIGHT 3

// default values
#define DEF_FROG_HEIGHT 2
#define DEF_FROG_LENGHT 2
#define DEF_FROG_SYMBOL ' '
#define DEF_CAR_SPEED 3
#define DEF_NUM_OF_CARS 5
#define DEF_CAR_LEN 3
#define DEF_FROG_SPEED 4


// color macros
#define COLOR_BACKGROUND 1
#define COLOR_FROG 2
#define COLOR_ROCK 3
#define COLOR_WATER 4
#define COLOR_CAR_ENEMY 5
#define COLOR_CAR_TRICKY 6
#define COLOR_CAR_FREIND 7

// helper functions

// constructors

WIN *Init_Window()
{
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
    nodelay(stdscr, true);

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

WIN *Init_status_win(WIN *screen, bool with_border)
{
    WIN *new_win = new WIN;
    new_win->win = newwin(STATUS_HEIGHT, screen->cols, screen->lines - STATUS_HEIGHT, 0);
    new_win->lines = STATUS_HEIGHT - with_border;
    new_win->cols = screen->cols - with_border;
    new_win->x = 0;
    new_win->border = 1;
    new_win->y = screen->lines - STATUS_HEIGHT + 1;
    box(new_win->win, 0, 0);
    return new_win;
}

WIN *Init_gameplay_win(WIN *screen, bool with_border)
{
    WIN *new_win = new WIN;
    new_win->border = with_border;
    new_win->lines = screen->lines - STATUS_HEIGHT;
    new_win->cols = screen->cols;

    new_win->win = newwin(new_win->lines, new_win->cols, new_win->y, new_win->x);

    new_win->lines -= with_border;
    new_win->cols -= with_border;
    new_win->x = 0;
    new_win->y = 0;

    box(new_win->win, 0, 0);
    return new_win;
}

Map *Init_map(WIN *window)
{
    Map *new_map = new Map;
    new_map->cols = window->cols + window->border;
    new_map->lines = window->lines + window->border;
    new_map->field = (int **)malloc(sizeof(int *) * new_map->lines);
    if (new_map->field == NULL)
    {
        fprintf(stderr, "malloc nie dziala");
        exit(-1);
    }
    wprintw(window->win, "cols : %d , lines : %d", new_map->cols, new_map->lines);
    FILE *err;
    err = fopen("err.txt", "w");

    for (int i = 0; i < new_map->lines; i++)
    {
        new_map->field[i] = (int *)malloc(sizeof(int) * (new_map->cols));
        if (new_map->field[i] == NULL)
        {
            fprintf(stderr, "malloc nie dziala");
        }
        for (int j = 0; j < new_map->cols; j++)
        {
            fprintf(err, "%d , %d \n", i, j);
            if (j == 0 || i == 0 || i == new_map->lines -1  || j == new_map->cols-1 )
            {
                new_map->field[i][j] = NODE_END_OF_MAP;
            }else {
            new_map->field[i][j] = NODE_BACKGROUND;
                //zmieniamy collidery
            }
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

Car *Init_cars(unsigned int num_of_cars, WIN *gameplay_win)
{

    Car *temp_cars = (Car *)malloc(num_of_cars * sizeof(Car));
    bool *lines = (bool *)malloc(gameplay_win->lines * sizeof(bool));

    for (int i = 0; i < num_of_cars; i++)
    {
        temp_cars[i].length = DEF_CAR_LEN;
        int rand_num = rand();
        int line = rand_num % gameplay_win->lines;
        int direction = rand_num % 2;
        // we randomly choose one of 3 types of car
        int type = rand_num % 3;
        while (lines[line] != 0)
        {
            rand_num = rand();
            line = rand_num % gameplay_win->lines;
        }

        switch (type)
        {
        case 0:
            temp_cars[i].type = ENEMY;
            temp_cars[i].color = COLOR_CAR_ENEMY;
            break;
        case 1:
            temp_cars[i].type = FREIND;
            temp_cars[i].color = COLOR_CAR_FREIND;
            break;
        case 2:
            temp_cars[i].type = TRICKY;
            temp_cars[i].color = COLOR_CAR_TRICKY;
            break;
        }

        temp_cars[i].y = line + 1;
        if (direction == 0)
        {
            temp_cars[i].direction = CAR_RIGHT;
            temp_cars[i].x = gameplay_win->cols - temp_cars[i].length;
        }
        else
        {
            temp_cars[i].direction = CAR_LEFT;
            temp_cars[i].x = gameplay_win->border;
        }
        temp_cars[i].speed = DEF_CAR_SPEED;
    }
    return temp_cars;
}

// destructors

void Close_window(WIN *win)
{
    delwin(win->win);
    delete (win);
}

void Delete_game(Game *game_instance)
{
    // Close_window(game_instance->gameplay_win);
    // Close_window(game_instance->status_win);
    // delete (game_instance->gameplay_win);
    // delete (game_instance->status_win);
    // delete (game_instance->my_frog);
    // delete (game_instance->my_map);
    // delete (game_instance->my_timer);
    // delete (game_instance);
}

void Close_game(WIN *screen)
{
    endwin();
    delete screen;
    exit(0);
}

// rendering

void Clear_screen(WIN *screen)
{
    for (int i = screen->border; i < screen->lines; i++)
    {
        wmove(screen->win, i, screen->border);
        for (int j = screen->border; j < screen->cols; j++)
        {
            waddch(screen->win, '?');
        }
    }
    wrefresh(screen->win);
}

void Refresh_game(Game *Game_instance)
{
    wrefresh(Game_instance->gameplay_win->win);
    wrefresh(Game_instance->status_win->win);
}

// status window functions

void Update_status_win(Game *game_instance)
{
    // mvwprintw(game_instance->status_win->win , game_instance->status_win->y , 0,
    // "frog x: %d , frog y: %d , lines: %d , cols: %d",
    // game_instance->my_frog->x,
    // game_instance->my_frog->y,
    // game_instance->gameplay_win->lines,
    // game_instance->gameplay_win->cols
    // );

    // mvwaddstr(game_instance->status_win->win , game_instance->status_win->y , 0 , "  dadadawda                                                ");
    wprintw(game_instance->status_win->win,
            "frog x: %d , frog y: %d , lines: %d , cols: %d",
            game_instance->my_frog->x,
            game_instance->my_frog->y,
            game_instance->gameplay_win->lines,
            game_instance->gameplay_win->cols);

    // waddstr(game_instance->status_win->win , "FROG");
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
    // will look like this:
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
    case UP:
        Game_instance->my_frog->y--;
        break;

    case DOWN:
        Game_instance->my_frog->y++;
        break;

    case LEFT:
        Game_instance->my_frog->x--;
        break;

    case RIGHT:
        Game_instance->my_frog->x++;
        break;
    default:
        break;
    }
    for(int i = 0 ; i < Game_instance->my_frog->lenght; i++){
        for(int j = 0 ; j < Game_instance->my_frog->height ; j++){
             if(Game_instance->my_map->field[Game_instance->my_frog->y + j][Game_instance->my_frog->x+i] == NODE_END_OF_MAP){
            Game_instance->my_frog->x = last_x;
            Game_instance->my_frog->y = last_y;
             return;
            }
     }
    }
}

void Move_cars(Game *game_instance)
{
    Car* car_list = game_instance->car_list;
    for (int i = 0; i < game_instance->num_of_cars; i++)
    {
        if(car_list[i].direction == CAR_LEFT){
            car_list[i].x--;
        }else{
            car_list[i].x++;
        }
        if(game_instance->my_map->field[car_list[i].y][car_list[i].x] == NODE_END_OF_MAP){
            if(car_list[i].type==TRICKY){
                if(car_list[i].direction == CAR_LEFT){
                    car_list[i].direction == CAR_RIGHT;
                    car_list[i].x++;
                }else{
                    car_list[i].direction == CAR_LEFT;
                    car_list[i].x--;
                }
            }else{
                   if(car_list[i].direction == CAR_LEFT){
                    car_list[i].x== game_instance->gameplay_win->cols;
                }else{
                    car_list[i].x = 0;
                }
            }
        }
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
        Move_frog(UP, game_instance);
        break;
    case 's':
    case KEY_DOWN:
        Move_frog(DOWN, game_instance);
        break;
    case 'd':
    case KEY_RIGHT:
        Move_frog(RIGHT, game_instance);
        break;
    case 'a':
    case KEY_LEFT:
        Move_frog(LEFT, game_instance);
        break;
    default:
        break;
    }
}

void Delate_frog(Frog *frog, Map *map)
{
    unsigned int x = frog->x;
    unsigned int y = frog->y;
    for (int i = 0; i < frog->lenght; i++)
    {
        for (int j = 0; j < frog->height; j++)
        {
            map->field[y + i][x + j] = NODE_BACKGROUND;
        }
    }
}

void Delate_objects(Game *game_instance)
{
    Delate_frog(game_instance->my_frog, game_instance->my_map);
}

void Place_frog(Frog *frog, Map *map)
{
    unsigned int x = frog->x;
    unsigned int y = frog->y;
    for (int i = 0; i < frog->lenght; i++)
    {
        for (int j = 0; j < frog->height; j++)
        {
            map->field[y + j][x + i] = NODE_FROG;
        }
    }
}

bool Fits_in_map(Map *map, int x, int y)
{
    if (map->cols > x && x > 0)
    {
        if (map->lines > y && y > 0)
        {
            return true;
        }
    }
    return false;
}

void Place_cars(Car *cars, Map *map, unsigned int num)
{
    // troche lepiej przenos numer aut bo teraz to brzudko wyglada
    FILE *b;
    b = fopen("b.txt", "w");

    for (int i = 0; i < num; i++)
    {
        int len = cars[i].length;
        unsigned int x = cars[i].x;
        unsigned int y = cars[i].y;

        for (int i = 0; i < len; i++)
        {
            if (cars[i].type == ENEMY)
            {
                map->field[y][x + i] = NODE_CAR_ENEMY;
            }
            if (cars[i].type == FREIND)
            {
                map->field[y][x + i] = NODE_CAR_FREINDLY;
            }
            if (cars[i].type == TRICKY)
            {
                map->field[y][x + i] = NODE_CAR_TRICKY;
            }
            map->field[y][x] = NODE_CAR_ENEMY;
        }
        fprintf(b, "car: %d, x:%d , y:%d , dir:%d  , map[y][x]: %d \n", i, cars[i].x, cars[i].y, cars[i].direction, map->field[y][x]);
    }
    fclose(b);
}

void Place_objects(Game *game_instance)
{
    // placing frog
    Place_frog(game_instance->my_frog, game_instance->my_map);
    Place_cars(game_instance->car_list, game_instance->my_map, game_instance->num_of_cars);
}

void Update_gameplay_scr(Game *game_instance)
{

    WIN *screen = game_instance->gameplay_win;
    // int i = game_instance->gameplay_win->border;
    // int j = game_instance->gameplay_win->border;
    for (int i = screen->border; i < screen->lines; i++)
    {
        wmove(screen->win, i, screen->border);
        for (int j = screen->border; j < screen->cols; j++)
        {
            int el = game_instance->my_map->field[i][j];
            switch (el)
            {
            case NODE_BACKGROUND:
                wattron(game_instance->gameplay_win->win, COLOR_PAIR(COLOR_BACKGROUND));
                waddch(game_instance->gameplay_win->win, '#');
                break;
            case NODE_FROG:
                wattron(game_instance->gameplay_win->win, COLOR_PAIR(COLOR_FROG));
                waddch(game_instance->gameplay_win->win, 'O');
                break;
            case NODE_CAR_ENEMY:
                wattron(game_instance->gameplay_win->win, COLOR_PAIR(COLOR_CAR_ENEMY));
                waddch(game_instance->gameplay_win->win, 'B');
                break;
            case NODE_CAR_FREINDLY:
                wattron(game_instance->gameplay_win->win, COLOR_PAIR(COLOR_CAR_ENEMY));
                waddch(game_instance->gameplay_win->win, 'B');
                break;
            case NODE_CAR_TRICKY:
                wattron(game_instance->gameplay_win->win, COLOR_PAIR(COLOR_CAR_ENEMY));
                waddch(game_instance->gameplay_win->win, 'B');
                break;
            case NODE_END_OF_MAP:
                wattron(game_instance->gameplay_win->win, COLOR_PAIR(COLOR_CYAN));
                waddch(game_instance->gameplay_win->win, 'P');
                break;
            default:
                wattron(game_instance->gameplay_win->win, COLOR_PAIR(COLOR_CAR_TRICKY));
                waddch(game_instance->gameplay_win->win, '?');
            }

            // waddch(screen->win, '?');
        }
    }
    wrefresh(screen->win);

    // for (i; i < game_instance->gameplay_win->lines; i++)
    // {
    //     wmove(game_instance->gameplay_win->win, i, game_instance->gameplay_win->border);
    //     for (j; j < game_instance->gameplay_win->cols; j++)
    //     {
    //         int el = game_instance->my_map->field[i][j];
    //         switch (el)
    //         {
    //         case NODE_BACKGROUND:
    //             wattron(game_instance->gameplay_win->win, COLOR_PAIR(COLOR_BACKGROUND));
    //             waddch(game_instance->gameplay_win->win, '#');
    //             break;
    //         case NODE_FROG:
    //             wattron(game_instance->gameplay_win->win, COLOR_PAIR(COLOR_FROG));
    //             waddch(game_instance->gameplay_win->win, 'O');
    //             break;
    //         default:
    //             wattron(game_instance->gameplay_win->win, COLOR_PAIR(COLOR_CAR_TRICKY));
    //             waddch(game_instance->gameplay_win->win, '?');
    //         }
    //     }
    //     wrefresh(game_instance->gameplay_win->win);
    // }
}

void Game_loop(Game *game_instance)
{
    nodelay(game_instance->gameplay_win->win, true);
    while (!(game_instance->is_over))
    {
        // Clear_screen(game_instance->gameplay_win);
        Clear_screen(game_instance->status_win);
        Delate_objects(game_instance);
        Handle_input(game_instance);
        Place_objects(game_instance);
        Update_gameplay_scr(game_instance);
        Update_status_win(game_instance);
        // waddstr(game_instance->gameplay_win->win, "ww");
        Refresh_game(game_instance);
    }
    Delete_game(game_instance);
}

void Enter_game(WIN *screen)
{
    Clear_screen(screen);

    Game *game_insatnce = new Game;
    // game instant is a stack allocated data because there is no need to store it on the heap becasue is is only for storing pointers to all game objects
    game_insatnce->num_of_cars = DEF_NUM_OF_CARS;
    WIN *status_win = Init_status_win(screen, true);
    WIN *gameplay_win = Init_gameplay_win(screen, true);
    Frog *frog_instance = Init_frog(gameplay_win);
    Map *map_instance = Init_map(gameplay_win);
    Car *car_list_instance = Init_cars(DEF_NUM_OF_CARS, gameplay_win);

    // setting frog in the middle of screen

    game_insatnce->gameplay_win = gameplay_win;
    game_insatnce->my_frog = frog_instance;
    game_insatnce->status_win = status_win;
    game_insatnce->my_map = map_instance;
    game_insatnce->my_timer = new Timer;
    game_insatnce->is_over = false;
    game_insatnce->car_list = car_list_instance;
    Refresh_game(game_insatnce);
    Game_loop(game_insatnce);

    // Close_window(status_win);
    // Close_window(gameplay_win);
    return;
}

void Start_screen(WIN *screen)
{
    int ch = 0;
    do
    {

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
            Clear_screen(screen);
            break;
        case 'Q':
            Close_game(screen);
            return;
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