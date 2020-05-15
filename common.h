typedef struct panel {
	WINDOW *grid, *status;
	int val_grid[10][20];
	int val_status[6];
} panel;

typedef struct screen {
	panel left, right;
} screen;

typedef struct info {
	WINDOW *display;
	int current_player;
} info;

void
draw_screen(screen screen);

screen
init_screen(void);

