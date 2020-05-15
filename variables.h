#include <ncurses.h>

typedef struct panel {
	WINDOW *grid, *status;
	int val_grid[10][20];
	int val_status[6];
} panel;

typedef struct screen {
	panel left, right;
	int player;
	int victory;
} screen;

typedef struct Point {
	int x, y;
} Point;

#define NOTHING_HIDDEN 0
#define SHIP_HIDDEN 1
#define SEA 2
#define TOUCHED 3
#define SUNKED 4
#define POPULATING 5

#define LEFT_PLAYER 0
#define RIGHT_PLAYER 1
