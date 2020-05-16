/*
MIT License

Copyright (c) 2020 PEDERSEN Ny Aina

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "variables.h"
#include "core.h"
#include "graphic.h"
#include "populate.h"

static bool
is_finished_player(Panel panel)
{
	int i, j, tile;
	for (i = 0; i < 10; ++i)
		for (j = 0; j < 10; ++j) {
			tile = panel.val_grid[i][j];
			if (tile == SHIP_HIDDEN)
				return(FALSE);
		}
	return(TRUE);
}

bool
is_finished(Screen screen)
{
	if (is_finished_player(screen.left) || is_finished_player(screen.right))
		return(TRUE);
	return(FALSE);
}

int
current_player(Screen screen)
{
	return(screen.player);
}

int
next_player(Screen screen)
{
	if (current_player(screen))
		return(LEFT_PLAYER);
	return(RIGHT_PLAYER);
}

static bool
move_ok(Point point, Panel panel)
{
	int tile = panel.val_grid[point.x][point.y];
	if (tile == NOTHING_HIDDEN || tile == SHIP_HIDDEN)
		return(TRUE);
	return(FALSE);
}

static bool
touch(Point point, Panel *panel)
{
	int i, j;

	if (panel->val_grid[point.x][point.y] == NOTHING_HIDDEN) {
		panel->val_grid[point.x][point.y] = SEA;
		return(FALSE);
	}
	panel->val_grid[point.x][point.y] = TOUCHED;
	for (i = point.x - 1; i <= point.x + 1; i += 2)
		for (j = point.y - 1; j <= point.y + 1; j += 2)
			if (i < 10 && j < 10 && i > -1 && j > -1)
				if (panel->val_grid[i][j] == NOTHING_HIDDEN) /* for safety */
					panel->val_grid[i][j] = SEA;
	return(TRUE);
}

static bool
check_neighbours(Point point, Panel panel, int trys)
{
	int i, j, tile;
	Point neighbour;

	for (i = point.x - 1; i <= point.x + 1; ++i)
		for (j = point.y - 1; j <= point.y + 1; ++j)
			if (i < 10 && j < 10 && i > -1 && j > -1) {
				tile = panel.val_grid[i][j];
				if (tile == SHIP_HIDDEN)
					return(FALSE);
				if (trys == 0)
					continue;
				if (tile == TOUCHED) {
					neighbour.x = i;
					neighbour.y = j;
					if (check_neighbours(neighbour, panel, --trys) == FALSE)
						return(FALSE);
				}
			}
	return(TRUE);
}

static void
sink_neighbours(Point point, Panel *panel)
{
	int i, j, tile;
	Point neighbour;

	for (i = point.x - 1; i <= point.x + 1; ++i)
		for (j = point.y - 1; j <= point.y + 1; ++j)
			if (i < 10 && j < 10 && i > -1 && j > -1) {
				tile = panel->val_grid[i][j];
				if (tile == NOTHING_HIDDEN)
					panel->val_grid[i][j] = SEA;
				if (tile == TOUCHED) {
					panel->val_grid[i][j] = SUNKED;
					neighbour.x = i;
					neighbour.y = j;
					sink_neighbours(neighbour, panel);
				}
			}
}

static int
count_len(Point point, Panel panel, int direction) /* 0, 1 -> Horizontal, vertical */
{
	Point coord = point;
	int *var;
	int sum = 1, initial_coord;

	if (direction == 0)
		var = &(coord.y);
	else
		var = &(coord.x);

	initial_coord = (*var)++;

	while ((panel.val_grid[coord.x][coord.y] == SUNKED) && (*var < 10) && (*var > -1)) {
		++(*var);
		++sum;
	}

	*var = --initial_coord;
	while ((panel.val_grid[coord.x][coord.y] == SUNKED) && (*var < 10) && (*var > -1)) {
		--(*var);
		++sum;
	}
	return(sum);
}

static void
update_status(Point point, Panel *panel)
{
	int width, heigth, index;

	width = count_len(point, *(panel), 0);
	heigth = count_len(point, *(panel), 1);

	switch (heigth) {
	case 1:
		index = width - 1;
		break;
	case 2:
		index = 4;
		break;
	default: /* fallthrough -> heigth = 3 */
		index = 5;
		break;
	}
	--panel->val_status[index];
}

static void
change_player(Screen *screen)
{
	if (screen->player == RIGHT_PLAYER)
		screen->player = LEFT_PLAYER;
	else
		screen->player = RIGHT_PLAYER;
}

static bool
make_move_panel(Point point, Panel *panel)
{
	if (move_ok(point, *panel)) {
		if (touch(point, panel)) {
			if (check_neighbours(point, *panel, 5)) {
				sink_neighbours(point, panel);
				update_status(point, panel);
			}
		return(FALSE);
		}
		else
			return(TRUE);
	}
	return(FALSE);
}

static void
make_move(Point l_cursor, Point r_cursor, Screen *screen)
{
	bool ret;
	if (current_player(*screen) == LEFT_PLAYER)
		ret = make_move_panel(l_cursor, &(screen->left));
	else
		ret = make_move_panel(r_cursor, &(screen->right));

	if (ret) /* have to change player */
		screen->player = next_player(*screen);
}

void
two_player_loop(void)
{
	/* ncurses set up */
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	start_color();
	noecho();
	curs_set(0);

	/* if u have a bootled terminal lol */
	if (has_colors() == FALSE) {
		endwin();
	}

	Screen screen;
	Point l_cursor = {.x = 4, .y = 4};
	Point r_cursor = {.x = 4, .y = 4};

	screen = init_screen();
	refresh();
	draw_screen(screen);
	colorize_screen(screen);
	populate(&screen);
	draw_all_status(screen);

	while (! is_finished(screen)) {
		if (current_player(screen) == LEFT_PLAYER)
			l_cursor = choose_target(screen.left, l_cursor);
		else
			r_cursor = choose_target(screen.right, r_cursor);
		make_move(l_cursor, r_cursor, &screen);

		colorize_screen(screen);
		draw_all_status(screen);
	}
	endwin();
}
