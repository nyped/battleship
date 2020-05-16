#include "variables.h"
#include "graphic.h"

#define GRID_H 21
#define GRID_W 41

#define STATUS_H 10
#define STATUS_W 12

#define WIN_MARGIN ((COLS - 2 * (GRID_W + STATUS_W) ) / 5)

void
draw_grid(Panel panel)
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

void
draw_status(Panel panel)
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
draw_screen(Screen screen)
{
	draw_grid(screen.left);
	draw_grid(screen.right);
	draw_status(screen.left);
	draw_status(screen.right);
}

void
colorize_grid(Panel panel)
{
	int i, j;
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_BLACK);

	for (i = 0; i < 10; ++i)
		for (j = 0; j < 10; ++j) {
			switch (panel.val_grid[i][j]) {
			case SEA:
				wattron(panel.grid, COLOR_PAIR(1));
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 2, ACS_BULLET);
				wattroff(panel.grid, COLOR_PAIR(1));
				break;
			case SUNKED:
				wattron(panel.grid, COLOR_PAIR(2));
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 1, ACS_CKBOARD);
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 2, ACS_CKBOARD);
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 3, ACS_CKBOARD);
				wattroff(panel.grid, COLOR_PAIR(2));
				break;
			case TOUCHED:
				wattron(panel.grid, COLOR_PAIR(3));
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 1, ACS_CKBOARD);
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 2, ACS_CKBOARD);
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 3, ACS_CKBOARD);
				wattroff(panel.grid, COLOR_PAIR(3));
				break;
			case POPULATING:
				wattron(panel.grid, COLOR_PAIR(4));
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 1, ACS_CKBOARD);
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 2, ACS_CKBOARD);
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 3, ACS_CKBOARD);
				wattroff(panel.grid, COLOR_PAIR(4));
				break;
			default:
				wattron(panel.grid, COLOR_PAIR(5));
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 1, ACS_CKBOARD);
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 2, ACS_CKBOARD);
				mvwaddch(panel.grid, 2 * i + 1, 4 * j + 3, ACS_CKBOARD);
				wattroff(panel.grid, COLOR_PAIR(5));
				break;

			}
	}
	wrefresh(panel.grid);
}

void
colorize_screen(Screen screen)
{
	colorize_grid(screen.left);
	colorize_grid(screen.right);
}

static void
move_target(Point coord, Panel panel)
{
	draw_grid(panel);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	wattron(panel.grid, COLOR_PAIR(1));

	/* top and bottom lines */
	for (int i = 0; i < 2; ++i) {
		mvwaddch(panel.grid, 2 * coord.x + 2 * i, 4 * coord.y + 1, ACS_HLINE);
		mvwaddch(panel.grid, 2 * coord.x + 2 * i, 4 * coord.y + 2, ACS_HLINE);
		mvwaddch(panel.grid, 2 * coord.x + 2 * i, 4 * coord.y + 3, ACS_HLINE);
	}
	/* left, right lines */
	mvwaddch(panel.grid, 2 * coord.x + 1, 4 * coord.y, ACS_VLINE);
	mvwaddch(panel.grid, 2 * coord.x + 1, 4 * coord.y + 4, ACS_VLINE);
	/* four corners */
	mvwaddch(panel.grid, 2 * coord.x, 4 * coord.y, ACS_ULCORNER);
	mvwaddch(panel.grid, 2 * coord.x, 4 * coord.y + 4, ACS_URCORNER);
	mvwaddch(panel.grid, 2 * coord.x + 2, 4 * coord.y + 4, ACS_LRCORNER);
	mvwaddch(panel.grid, 2 * coord.x + 2, 4 * coord.y, ACS_LLCORNER);

	wattroff(panel.grid, COLOR_PAIR(1));
	wrefresh(panel.grid);
}

Point
choose_target(Panel panel)
{
	int ch;
	Point coord = {.x = 4, .y = 4};
	move_target(coord, panel);

	while ((ch = getch()) != 10) {
		switch (ch) {
		case KEY_LEFT:
			if (coord.y > 0)
				--coord.y;
			break;
		case KEY_RIGHT:
			if (coord.y < 9)
				++coord.y;
			break;
		case KEY_UP:
			if (coord.x > 0)
				--coord.x;
			break;
		case KEY_DOWN:
			if (coord.x < 9)
				++coord.x;
			break;
		}
		move_target(coord, panel);
	}
	return (coord);
}

static Panel
create_panel(int side)
{
	int i, j, GRID_Y, GRID_X, STATUS_Y, STATUS_X;
	Panel panel = { .val_status = {4, 1, 1, 1, 2, 1},
					.val_grid = {[0 ... 9][0 ... 9] = NOTHING_HIDDEN} };

	GRID_Y = (LINES - GRID_H) / 2;
	STATUS_Y = (LINES - STATUS_H) / 2;

	if (side == LEFT_PLAYER) {
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

Screen
init_screen(void)
{
	Screen screen = {
						.left = create_panel(LEFT_PLAYER),
						.right = create_panel(RIGHT_PLAYER),
						.player = LEFT_PLAYER,
						.victory = FALSE
					};
	return(screen);
}
