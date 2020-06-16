#include <ncurses.h>

typedef struct Panel {
	WINDOW *grid, *status;
	int val_grid[10][10];
	int val_status[6];
} Panel;

typedef struct Screen {
	Panel left, right;
	int player;
} Screen;

typedef struct Point {
	int x, y;
} Point;

#define NOTHING_HIDDEN 0
#define SHIP_HIDDEN 1
#define SEA 2
#define TOUCHED 3
#define SUNKED 4
#define POPULATING 5
#define REVEAL 6

#define LEFT_PLAYER 0
#define RIGHT_PLAYER 1

#define DIRECTION int
#define VER 1
#define HOR 0
