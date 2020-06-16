#include "variables.h"
#include "graphic.h"

int
number_neighbours(Point point, Panel panel, int tile)
{
	int i, j, number = 0;
	for (i = -1; i <= 1; ++i)
		for (j = -1; j <= 1; ++j)
			if ((point.x + i < 10) && (point.x + i > -1) && (point.y + j < 10) && (point.y + j > -1))
				if (panel.val_grid[point.x + i][point.y + j] == tile)
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
ship_position_ok(Point point, Panel panel, DIRECTION direction, int index) /* 0, 1 -> horizontal, vertical */
{
	Point coord = point;
	int *var;
	int initial_coord;

	if (direction == HOR)
		var = &(coord.y);
	else
		var = &(coord.x);

	initial_coord = (*var);

	while ((panel.val_grid[coord.x][coord.y] == POPULATING) && (*var < 10) && (*var > -1)) {
		if (number_neighbours(coord, panel, POPULATING) > 3)
			return(FALSE);
		++(*var);
	}
	--(*var);
	if (number_neighbours(coord, panel, POPULATING) > min(2, index_to_len(index)))
		return(FALSE);

	*var = initial_coord;
	while ((panel.val_grid[coord.x][coord.y] == POPULATING) && (*var < 10) && (*var > -1)) {
		if (number_neighbours(coord, panel, POPULATING) > 3)
			return(FALSE);
		--(*var);
	}
	++(*var);
	if (number_neighbours(coord, panel, POPULATING) > min(2, index_to_len(index)))
		return(FALSE);

	return(TRUE);
}

static int
count_len(Point point, Panel panel, DIRECTION direction) /* 0, 1 -> Horizontal, vertical */
{
	Point coord = point;
	int *var;
	int sum = 1, initial_coord;

	if (direction == HOR)
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

bool
populate_ok(Point point, Panel panel, int index) /* index of the ship in status */
{
	if ((ship_position_ok(point, panel, 0, index)) && (ship_position_ok(point, panel, 1, index))
			&& (max(count_len(point, panel, 0), count_len(point, panel, 1)) == index_to_len(index)))
		return(TRUE);
	return(FALSE);
}

bool
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
		if (panel->val_grid[point->x][point->y] == POPULATING)
			return(FALSE);
		panel->val_grid[point->x][point->y] = POPULATING;
		++(*var);
	} while ((*var < temp + index_to_len(type)) && (*var < 10) && (*var > -1));
	*var = temp;
	return(TRUE);
}

void
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

void
populate_panel(Panel *panel)
{
	int index;
	Panel new_panel = *(panel);
	Point coord = {.x = 4, .y = 4};

	mvwprintw(stdscr, LINES - 2, (COLS - 16)/ 2, "Place your ships");

	for (index = 0; index < 6; ++index) {
		while (new_panel.val_status[index] > 0) {
			choose_target(new_panel, &coord);
			if (new_panel.val_grid[coord.x][coord.y] == POPULATING)
				continue;
			if (!add_ship(&coord, &new_panel, index)) {
				new_panel = *panel;
				colorize_grid(*panel);
				continue;
			}
			if (populate_ok(coord, new_panel, index)) {
				--(new_panel.val_status[index]);
				*panel = new_panel;
				colorize_grid(*panel);
				draw_status(*panel);
			} else {
				new_panel = *panel;
				colorize_grid(*panel);
			}
		}
	}
	population_to_real_grid(&new_panel);
	*panel = new_panel;
	colorize_grid(*panel);
	draw_grid(*panel);
	mvwprintw(stdscr, LINES - 2, (COLS - 16)/ 2, "                ");
}

void
populate(Screen *screen)
{
	populate_panel(&(screen->left));
	populate_panel(&(screen->right));
}
