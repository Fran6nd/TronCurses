#include "stdlib.h"
#include <ncurses.h>

#include <string.h>
#include <unistd.h>
#include <time.h>

#define COL_RED 1
#define COL_GREEN 2
#define COL_GREY 3
#define COL_BLACK 4
#define COL_RED_BLACK 5

#define DIR_RIGHT 4
#define DIR_LEFT 1
#define DIR_UP 2
#define DIR_DOWN 3

int RUNNING = 1;
int ERROR = 0;

int random_between(int min, int max)
{
    return rand() % ((max + 1) - min) + min;
}

struct position
{
    int x;
    int y;
};
struct player
{
    int direction;
    struct position position;
};
struct player RED;
struct player GREEN;
struct neighbor
{
    int n;
    struct position paths[8];
};
struct position pos = {4, 3};
struct position end_point;

char *map;
char get_tile(int _x, int _y)
{
    return map[_y * COLS + _x];
}

int is_odd(int n)
{
    if (n % 2 == 0)
    {
        return 1;
    }
    return 0;
}

int can_move_v(struct position *p)
{
    if (is_odd(p->x))
    {
        return 1;
    }
    return 0;
}
int can_move_h(struct position *p)
{
    if (is_odd(p->y))
    {
        return 0;
    }
    return 1;
}

void draw_green_at(struct position p)
{
    move(p.y, p.x);
    if (has_colors)
    {
        attron(COLOR_PAIR(COL_GREEN));
        printw(" ");
        attroff(COLOR_PAIR(COL_GREEN));
    }
    else
    {
        addch('O');
    }
}
void draw_red_at(struct position p)
{
    move(p.y, p.x);
    if (has_colors)
    {
        attron(COLOR_PAIR(COL_RED));
        printw(" ");
        attroff(COLOR_PAIR(COL_RED));
    }
    else
    {
        addch('O');
    }
}

void draw_map()
{
    int _x = 0;
    int _y = 0;
    for (_y = 0; _y < LINES; _y++)
    {
        for (_x = 0; _x < COLS; _x++)
        {
            move(_y, _x);
            if (!(_x == 0 || _y == 0 || _x == COLS - 1 || _y == LINES - 1))
            {
                struct position p = {_x, _y};
                if (has_colors && get_tile(_x, _y) == 'G')
                {

                    draw_green_at(p);
                }
                else if (has_colors && get_tile(_x, _y) == 'R')
                {

                    draw_red_at(p);
                }
                else if (has_colors && get_tile(_x, _y) == ' ')
                {
                    attron(COLOR_PAIR(COL_GREY));
                    addch(ACS_CKBOARD);
                    attroff(COLOR_PAIR(COL_GREY));
                }
                else
                {
                    addch(get_tile(_x, _y));
                }
            }
        }
    }
    if (pos.y < 0)
    {
        pos.y = 0;
    }
    if (pos.x < 0)
    {
        pos.x = 0;
    }
    if (pos.y >= LINES)
    {
        pos.y = LINES - 1;
    }
    if (pos.x >= COLS)
    {
        pos.x = COLS - 1;
    }
    attron(COLOR_PAIR(COL_RED_BLACK));
    move(0, 0);
    hline(ACS_HLINE, COLS - 1);
    vline(ACS_VLINE, LINES - 1);
    addch(ACS_ULCORNER);
    move(LINES - 1, 0);
    hline(ACS_HLINE, COLS - 1);
    addch(ACS_LLCORNER);
    move(0, COLS - 1);
    vline(ACS_VLINE, LINES - 1);
    addch(ACS_URCORNER);
    move(LINES - 1, COLS - 1);
    addch(ACS_LRCORNER);
    attroff(COLOR_PAIR(COL_RED_BLACK));
    {
        draw_red_at(RED.position);
        draw_green_at(GREEN.position);
    }
    move(0, 5);
    if (has_colors)
    {
        attron(COLOR_PAIR(5));
        printw("TronCurses by Fran6nd.");
        attroff(COLOR_PAIR(5));
    }
    else
    {
        printw("TronCurses by Fran6nd.");
    }
    move(LINES - 1, COLS - 20);
    if (has_colors)
    {
        attron(COLOR_PAIR(5));
        printw("Press [q] to exit.");
        attroff(COLOR_PAIR(5));
    }
    else
    {
        printw("Press [q] to exit.");
    }
}
int is_on_map(struct position p)
{
    if (p.x >= 1 && p.x < COLS - 1 && p.y >= 1 && p.y < LINES - 1)
    {
        return 1;
    }
    return 0;
}

void set_tile(int _x, int _y, char _c)
{
    struct position p = {_x, _y};
    map[_y * COLS + _x] = _c;
}
struct neighbor get_neighboors(struct position p)
{
    struct position up, down, right, left;
    up.x = p.x;
    up.y = p.y - 1;
    down.x = p.x;
    down.y = p.y + 1;
    right.y = p.y;
    right.x = p.x + 1;
    left.y = p.y;
    left.x = p.x - 1;
    struct neighbor nb;
    nb.n = 0;
    if (is_on_map(up))
    {
        nb.paths[nb.n] = up;
        nb.n++;
    }
    if (is_on_map(down))
    {
        nb.paths[nb.n] = down;
        nb.n++;
    }
    if (is_on_map(right))
    {
        nb.paths[nb.n] = right;
        nb.n++;
    }
    if (is_on_map(left))
    {
        nb.paths[nb.n] = left;
        nb.n++;
    }
    return nb;
}
int can_go(struct position p)
{
    if (is_on_map(p))
        if (get_tile(p.x, p.y) == ' ')
            return 1;
    return 0;
}

int update_player(struct player *p)
{
    struct position next_pos = {p->position.x, p->position.y};
    switch (p->direction)
    {
    case DIR_DOWN:
        /* code */
        next_pos.y++;
        break;
    case DIR_UP:
        /* code */
        next_pos.y--;
        break;
    case DIR_RIGHT:
        /* code */
        next_pos.x++;
        break;
    case DIR_LEFT:
        /* code */
        next_pos.x--;
        break;
    default:
        break;
    }
    if (can_go(next_pos))
    {

        p->position.x = next_pos.x;
        p->position.y = next_pos.y;
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        if (strcmp(argv[1], "-h") == 0)
        {
            printf("------------------------------------------\n");
            printf("TronCurses, a tron game made with ncurses.\n");
            printf("------------------------------------------\n");
            printf("by Fran6nd\n\n");
            printf("Usage:\n");
            printf("./TronCurses\n");
            printf("./TronCurses -h to display help.\n");
            return 0;
        }
        else if (strcmp(argv[1], "--nodelay") == 0)
        {
            //GENERATE_DELAY = 0;
        }
        else
        {
            printf("Unknown arg \"%s\" see -h for help.\n", argv[1]);
            return 0;
        }
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], "--delay") == 0)
        {
            //GENERATE_DELAY = atoi(argv[2]);
        }
        else
        {
            printf("Unknown arg \"%s\" see -h for help.\n", argv[1]);
            return 0;
        }
    }
    else if (argc > 3)
    {
        printf("Too many args... see -h for help.\n");
        return 0;
    }

    initscr();
    curs_set(0);
    if (has_colors)
    {
        start_color();
        init_pair(COL_RED, COLOR_RED, COLOR_RED);
        init_pair(COL_GREEN, COLOR_GREEN, COLOR_GREEN);
        init_pair(COL_GREY, COLOR_BLACK, COLOR_WHITE);
        init_pair(COL_BLACK, COLOR_BLACK, COLOR_BLACK);
        init_pair(COL_RED_BLACK, COLOR_RED, COLOR_BLACK);
    }
    else
    {
        return 0;
    }
    srand(time(NULL));

    map = malloc(((LINES * COLS)) * sizeof(char));
begin:
    memset(map, ' ', LINES * COLS);
    int x = 0;
    int y = 0;
    for (; x < COLS; x++)
    {
        set_tile(x, y, '+');
    }
    x = 0;
    for (; y < LINES; y++)
    {
        set_tile(x, y, '+');
    }
    y = LINES - 1;
    for (; x < COLS; x++)
    {
        set_tile(x, y, '+');
    }
    y = 0;
    x = COLS - 1;
    for (; y < LINES; y++)
    {
        set_tile(x, y, '+');
    }
    RED.direction = DIR_RIGHT;
    RED.position.x = 1;
    RED.position.y = 1;
    GREEN.direction = DIR_LEFT;
    GREEN.position.x = COLS - 2;
    GREEN.position.y = LINES - 2;
    timeout(100);
    while (RUNNING)
    {
        set_tile(RED.position.x, RED.position.y, 'R');
        set_tile(GREEN.position.x, GREEN.position.y, 'G');
        int ok = update_player(&RED);
        if (!ok)
        {
            while (getch() == ERR)
                ;
            goto begin;
        }
        ok = update_player(&GREEN);
        if (!ok)
        {
            while (getch() == ERR)
                ;
            goto begin;
        }
        draw_map();
        doupdate();
        char c = getch();

        if (c == '\033')
        {
            getch();
            switch (getch())
            {
            case 'A':
                RED.direction = DIR_UP;
                break;
            case 'B':
                RED.direction = DIR_DOWN;
                break;
            case 'C':
                RED.direction = DIR_RIGHT;
                break;
            case 'D':
                RED.direction = DIR_LEFT;
                break;
            }
        }
        else if (c == 'q')
        {
                goto quit;
        }
        else if (c == 'e')
        {
            GREEN.direction = DIR_UP;
        }
        else if (c == 'd')
        {
            GREEN.direction = DIR_DOWN;
        }
        else if (c == 's')
        {
            GREEN.direction = DIR_LEFT;
        }
        else if (c == 'f')
        {
            GREEN.direction = DIR_RIGHT;
        }
        if (get_tile(pos.x, pos.y) != ' ')
        {
        }
        if (pos.x == end_point.x && pos.y == end_point.y)
        {
            RUNNING = 0;
        }
    }
    RUNNING = 1;
    goto begin;
quit:
    free(map);
    endwin();

    return ERROR;
}