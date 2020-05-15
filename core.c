#include "variables.h"
#include "core.h"
#include "graphic.h"

static bool
is_finished_player(panel panel)
{
	int i, j, tile;
	for (i = 0; i < 10; ++i)
		for (j = 0; j < 10; ++j) {
			tile = panel.val_grid[i][j];
			if (tile == SHIP_HIDDEN || tile == NOTHING_HIDDEN)
				return(FALSE);
		}
	return(TRUE);
}

bool
is_finished(screen screen)
{
	if (is_finished_player(screen.left) || is_finished_player(screen.right))
		return(TRUE);
	return(FALSE);
}

int
current_player(screen screen)
{
	return(screen.player);
}

int
next_player(screen screen)
{
	if (current_player(screen))
		return(LEFT_PLAYER);
	return(RIGHT_PLAYER);
}

static bool
move_ok(Point point, panel panel)
{
	int tile = panel.val_grid[point.x][point.y];
	if ( tile == NOTHING_HIDDEN || tile == SHIP_HIDDEN)
		return(TRUE);
	return(FALSE);
}

static bool
touch(Point point, panel *panel)
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
				panel->val_grid[i][j] = SEA;
	return(TRUE);
}

static bool
check_neighbours(Point point, panel panel, int trys)
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
					check_neighbours(neighbour, panel, --trys);
				}
			}
	return(TRUE);
}

static void
sunk_neighbours(Point point, panel *panel)
{
	int i, j, tile;
	Point neighbour;

	for (i = point.x - 1; i <= point.x + 1; ++i)
		for (j = point.y - 1; j <= point.y + 1; ++j)
			if (i < 10 && j < 10 && i > -1 && j > -1) {
				tile = panel->val_grid[i][j];
				if (tile == TOUCHED) {
					panel->val_grid[i][j] = SUNKED;
					neighbour.x = i;
					neighbour.y = j;
					sunk_neighbours(neighbour, panel);
				}
			}
}

static int
count_len(Point *point, panel panel, int direction) /* 0, 1 -> Horizontal, vertical */
{
	int *var;
	int sum = 1, initial_coord;

	if (direction == 0)
		var = &(point->y);
	else
		var = &(point->x);

	initial_coord = *var;
	(*var)++;

	while ((panel.val_grid[point->x][point->y] == SUNKED) && (*var < 10) && (*var > -1)) {
		++(*var);
		++sum;
	}

	*var = --initial_coord;
	while ((panel.val_grid[point->x][point->y] == SUNKED) && (*var < 10) && (*var > -1)) {
		--(*var);
		++sum;
	}
	*var = initial_coord;

	return(sum);
}

static void
update_status(Point point, panel *panel)
{
	int width, heigth, index;

	width = count_len(&point, *(panel), 0);
	heigth = count_len(&point, *(panel), 1);

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
change_player(screen screen)
{
	if (screen.player)
		screen.player = LEFT_PLAYER;
	else
		screen.player = RIGHT_PLAYER;
}

static void
make_move_panel(Point point, panel *panel)
{
	if (move_ok(point, *panel) && touch(point, panel) && check_neighbours(point, *panel, 5)) {
			sunk_neighbours(point, panel);
			update_status(point, panel);
	}
}

void
make_move(Point point, screen *screen)
{
	if (current_player(*screen) == LEFT_PLAYER)
		make_move_panel(point, &(screen->left));
	else
		make_move_panel(point, &(screen->right));
}
