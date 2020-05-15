#include <ncurses.h>
#include "graphic.h"

#define GRID_H 21
#define GRID_W 41

#define STATUS_H 10
#define STATUS_W 12

#define WIN_MARGIN ((COLS - 2 * (GRID_W + STATUS_W) ) / 5)

#define LEFT 0
#define RIGHT 1

static void
draw_grid(panel panel)
{
	int i, j;
	box(panel.grid, 0, 0);

	for (i = 0; i < 19; ++i) {
		if (i % 2 == 0)	/* vertical seps */
			for (j = 1; j <= 9; ++j)
				mvwaddch(panel.grid, i + 1, 4 * j, ACS_VLINE);
		else	/* intersections and horizontal seps */
			for (j = 1; j <= 10; ++j) {
				if (j != 10)
					mvwaddch(panel.grid, i + 1, 4 * j, ACS_PLUS);
				mvwaddch(panel.grid, i + 1, 4 * j - 1, ACS_HLINE);
				mvwaddch(panel.grid, i + 1, 4 * j - 2, ACS_HLINE);
				mvwaddch(panel.grid, i + 1, 4 * j - 3, ACS_HLINE);
			}
	}
	wrefresh(panel.grid);
}

static void
draw_status(panel panel)
{
	int line;
	char message[6][STATUS_W] = { 	"1x1",
									"1x2",
									"1x3",
									"1x4",
									"2x1",
									"3x1",
								};

	box(panel.status, 0, 0);
	mvwprintw(panel.status,  1, 1, "left (HxL)" );
	mvwhline(panel.status, 2, 1, ACS_HLINE, STATUS_W - 2);

	for (line = 0; line < 6; ++line)
		mvwprintw(panel.status, line + 3, 1, "  %s: %d", message[line], panel.val_status[line] );

	wrefresh(panel.status);
}

void
draw_screen(screen screen)
{
	draw_grid(screen.left);
	draw_grid(screen.right);
	draw_status(screen.left);
	draw_status(screen.right);
}

static panel
create_panel(int side)
{
	int i, j, GRID_Y, GRID_X, STATUS_Y, STATUS_X;
	panel panel = { .val_status = {4, 1, 1, 1, 2, 1},
					.val_grid = {[0 ... 9][0 ... 19] = 0} };

	GRID_Y = (LINES - GRID_H) / 2;
	STATUS_Y = (LINES - STATUS_H) / 2;

	if (side == LEFT) {
		GRID_X = WIN_MARGIN;
		STATUS_X = 2 * WIN_MARGIN + GRID_W;
	} else {
		GRID_X = 3 * WIN_MARGIN + GRID_W + STATUS_W;
		STATUS_X = 4 * WIN_MARGIN + 2 * GRID_W + STATUS_W;
	}

	panel.grid = newwin(GRID_H, GRID_W, GRID_Y, GRID_X);
	panel.status = newwin(STATUS_H, STATUS_W, STATUS_Y, STATUS_X);

	return(panel);
}

screen
init_screen(void)
{
	screen screen;
	screen.left = create_panel(LEFT);
	screen.right = create_panel(RIGHT);
	return(screen);
}
