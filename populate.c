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

static int
number_neighbours(Point point, Panel panel)
{
	int i, j, number = 0;
	for (i = -1; i <= 1; ++i)
		for (j = -1; j <= 1; ++j)
			if ((point.x + i < 10) && (point.x + i > -1) && (point.y + j < 10) && (point.y + j > -1))
				if (panel.val_grid[point.x + i][point.y + j] == POPULATING)
					++number;
	return(number);
}

static int
index_to_len(int index)
{
	int len = index + 1;
	return ((len > 4)? (len / 2) : len);
}

static int
max(int x, int y)
{
	return ((x < y)? y : x);
}

static int
min(int x, int y)
{
	return ((x < y)? x : y);
}

static bool
ship_position_ok(Point point, Panel panel, int direction, int index) /* 0, 1 -> horizontal, vertical */
{
	Point coord = point;
	int *var;
	int initial_coord;

	if (direction == 0)
		var = &(coord.y);
	else
		var = &(coord.x);

	initial_coord = (*var);

	while ((panel.val_grid[coord.x][coord.y] == POPULATING) && (*var < 10) && (*var > -1)) {
		if (number_neighbours(coord, panel) > 3)
			return(FALSE);
		++(*var);
	}
	--(*var);
	if (number_neighbours(coord, panel) > min(2, index_to_len(index)))
		return(FALSE);

	*var = initial_coord;
	while ((panel.val_grid[coord.x][coord.y] == POPULATING) && (*var < 10) && (*var > -1)) {
		if (number_neighbours(coord, panel) > 3)
			return(FALSE);
		--(*var);
	}
	++(*var);
	if (number_neighbours(coord, panel) > min(2, index_to_len(index)))
		return(FALSE);

	return(TRUE);
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

	while ((panel.val_grid[coord.x][coord.y] == POPULATING) && (*var < 10) && (*var > -1)) {
		++(*var);
		++sum;
	}

	*var = --initial_coord;
	while ((panel.val_grid[coord.x][coord.y] == POPULATING) && (*var < 10) && (*var > -1)) {
		--(*var);
		++sum;
	}
	*var = initial_coord;
	return(sum);
}

static bool
populate_ok(Point point, Panel panel, int index) /* index of the ship in status */
{
	if ((ship_position_ok(point, panel, 0, index)) && (ship_position_ok(point, panel, 1, index))
			&& (max(count_len(point, panel, 0), count_len(point, panel, 1)) == index_to_len(index)))
		return(TRUE);
	return(FALSE);
}

static void
add_ship(Point *point, Panel *panel, int type)
{
	int *var;
	int temp;
	if (type > 3)
		var = &(point->x);
	else
		var = &(point->y);
	temp = *var;

	do {
		panel->val_grid[point->x][point->y] = POPULATING;
		++(*var);
	} while ((*var < temp + index_to_len(type)) && (*var < 10) && (*var > -1));
	*var = temp;
}

static void
population_to_real_grid(Panel *panel)
{
	int i, j;
	for (i = 0; i < 10; ++i)
		for (j = 0; j < 10; ++j)
			if (panel->val_grid[i][j] == POPULATING)
				panel->val_grid[i][j] = SHIP_HIDDEN;

	for (i = 1; i < 6; ++i)
		panel->val_status[i] = 1;
	panel->val_status[0] = 4;
	panel->val_status[4] = 2;
}

static void
populate_panel(Panel *panel)
{
	int index;
	Panel new_panel = *(panel);
	Point coord = {.x = 4, .y = 4};

	for (index = 0; index < 6; ++index) {
		while (new_panel.val_status[index] > 0) {
			coord = choose_target(new_panel, coord);
			if (new_panel.val_grid[coord.x][coord.y] == POPULATING)
				continue;
			add_ship(&coord, &new_panel, index);
			if (populate_ok(coord, new_panel, index)) {
				--(new_panel.val_status[index]);
				*panel = new_panel;
				colorize_grid(*panel);
				draw_status(*panel);
				continue;
			}
			new_panel = *panel;
			colorize_grid(*panel);
		}
	}
	population_to_real_grid(&new_panel);
	*panel = new_panel;
	colorize_grid(*panel);
	draw_grid(*panel);
}

void
populate(Screen *screen)
{
	populate_panel(&(screen->left));
	populate_panel(&(screen->right));
}
