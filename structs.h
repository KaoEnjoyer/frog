#include <ncurses.h>
#include <time.h>

typedef int Color;

typedef enum {
    enemy,
    tricky,
    freindly
}car_type;

typedef enum{
    UP,
    DOWN,
    LEFT,
    RIGHT
}jump_direction;

typedef struct{
    unsigned int height;
    unsigned int lenght;
    char symbol;
    Color color;
    unsigned int x;
    unsigned int y;
}Frog;

typedef struct
{
    unsigned int x;
    unsigned int y; 
    unsigned int length;
    Color color;
    int speed;
    car_type type;
}Car;

typedef struct
{
    time_t t;
}Timer;

typedef struct{
    WINDOW* win;
    // x oraz y to pozycja okna od lewego gornego rogu
    unsigned int x;
    unsigned int y;
    // rozmiary okna
    unsigned int cols;
    unsigned int lines;
    //czy ma ramke
    bool border;
}WIN;

typedef struct{
    int cols; 
    int lines;
    int **field;
}Map;

typedef struct{
    WIN* status_win;
    WIN* gameplay_win;
    Frog* my_frog;
    Map* my_map;
    Timer* my_timer;
    bool is_over;

}Game; 
