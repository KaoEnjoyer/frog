#include <ncurses.h>
#include <time.h>

typedef int Color;

typedef enum {
    ENEMY,
    TRICKY,
    FREIND
}car_type;

typedef enum {
    CAR_LEFT,
    CAR_RIGHT
}car_diercion;

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
    unsigned int speed;
}Frog;

typedef struct
{
    unsigned int x;
    unsigned int y; 
    unsigned int length;
    int color;
    int speed;
    car_diercion direction;
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
    Car* car_list;
    unsigned int num_of_cars;
    bool is_over;

}Game; 
