#include <stdlib.h>
#include <unistd.h>

#include "variables.h"
#include "graphic.h"
#include "populate.h"

int
rdm(int borne)
{
	static int seme = 0;
	if (!seme) {
		srandom(getpid());
		seme = 1;
	}
	return (int) random() % borne;
}

Point
random_point(void)
{
	int x = (int) rdm(11);
	int y = (int) rdm(11);
	Point point;

	point.x = x;
	point.y = y;

	return(point);
}

void
autopopulate(Panel *panel)
{
	int index;
	Panel new_panel = *panel;
	Point coord;

	for (index = 0; index < 6; ++index) {
		while (new_panel.val_status[index] > 0) {
			coord = random_point();
			if (new_panel.val_grid[coord.x][coord.y] == POPULATING)
				continue;
			add_ship(&coord, &new_panel, index);
			if (populate_ok(coord, new_panel, index)) {
				--(new_panel.val_status[index]);
				*panel = new_panel;
			} else {
				new_panel = *panel;
			}
		}
	}
	population_to_real_grid(&new_panel);
	*panel = new_panel;
	colorize_grid(*panel);
	draw_grid(*panel);
	mvwprintw(stdscr, LINES - 2, (COLS - 16)/ 2, "                ");
}

Point // returns the coordinates of the first touched ship seen
get_coord_touched(Panel panel)
{
	int i, j;
	Point point;
	for (i = 0; i < 10; ++i)
		for (j = 0; j < 10; ++j)
			if (panel.val_grid[i][j] == TOUCHED) {
				point.x = i;
				point.y = j;
				return point;
			}
	point.x = 20; // nothing found
	point.y = 20; // nothing found
	return point;
}

DIRECTION
get_direction(Panel panel)
{
	int hor = 0, ver = 0, i;
	for (i = 1; i <= 3; ++i)
		hor += panel.val_status[i];
	for (i = 4; i <= 5; ++i)
		ver += panel.val_status[i];
	return ((hor > ver)? HOR : VER);
}

void
guess_next(Point *coord, Panel *panel)
{
	int *var;
	DIRECTION direction = (DIRECTION) rdm(2);

	if (get_direction(*panel) == HOR)
		var = &(coord->y);
	else
		var = &(coord->x);

	do {
		if (direction == HOR)
			++(*var);
		else 
			--(*var);
	} while ((panel->val_grid[coord->x][coord->y] == TOUCHED) && (*var < 10) && (*var > -1));
}
