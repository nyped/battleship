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

typedef struct point {
	int x, y;
} point;

void
draw_screen(screen screen);

screen
init_screen(void);

void
colorize_screen(screen screen);

point
choose_target(panel panel);
