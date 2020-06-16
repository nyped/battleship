#include "variables.h"
#include "graphic.h"
#include "populate.h"
#include "bot.h"

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

static bool  /* makes the move: if it is a ship, touches the edges */
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
					panel->val_grid[i][j] = SEA;
	return(TRUE);
}

static bool /* checks recursively if all the cases are touched */
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
		change_player(screen);

	colorize_screen(*screen);
	draw_all_status(*screen);
}

static void
reveal_panel(Panel *panel)
{
	int i, j;
	for (i = 0; i < 10; ++i)
		for (j = 0; j < 10; ++j)
			if (panel->val_grid[i][j] == SHIP_HIDDEN)
				panel->val_grid[i][j] = REVEAL;
}

static void
reveal(Screen *screen)
{
	reveal_panel(&(screen->left));
	reveal_panel(&(screen->right));
	colorize_screen(*screen);
}

static void
set_curses(void)
{
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	start_color();
	noecho();
	curs_set(0);
	refresh();
}

static void
two_player_loop(void)
{
	int ch;
	Screen screen;
	Point l_cursor = {.x = 4, .y = 4};
	Point r_cursor = {.x = 4, .y = 4};

	init_screen(&screen);
	draw_screen(screen);
	colorize_screen(screen);
	populate(&screen);
	draw_all_status(screen);
	mvwprintw(stdscr, LINES - 2, (COLS - 20)/ 2, "Touch the enemy's ships");

	while (! is_finished(screen)) {
		if (current_player(screen) == LEFT_PLAYER)
			choose_target(screen.left, &l_cursor);
		else
			choose_target(screen.right, &r_cursor);
		make_move(l_cursor, r_cursor, &screen);
	}

	reveal(&screen);
	mvwprintw(stdscr, LINES - 2, (COLS - 20)/ 2, "                         ");
	mvwprintw(stdscr, LINES - 2, (COLS - 35)/ 2, "What a game ! Hit space to continue");
	while ((ch = getch()) != ' ')
		continue;
	werase(stdscr);
	refresh();
}

/* bot stuff */

static void
bot_move(Screen *screen, int trys)
{
	Point coord = get_coord_touched(screen->left);

	if (coord.x == 20) {
		do {
			coord = random_point();
		} while (((screen->left).val_grid[coord.x][coord.y] > 1)
				|| ((number_neighbours(coord, screen->left, SEA) > 5) && ((screen->left).val_status[1] > 0) &&
					((screen->left).val_status[2] > 0) && ((screen->left).val_status[3] > 0) &&
					((screen->left).val_status[4] > 0) && ((screen->left).val_status[1] > 0)));
	} else {
		guess_next(&coord, &(screen->left), trys);
	}
	if (make_move_panel(coord, &(screen->left)))
		change_player(screen);

	colorize_screen(*screen);
	draw_all_status(*screen);
}

static void
one_player_populate(Screen *screen)
{
	populate_panel(&(screen->left));
	autopopulate(&(screen->right));
	/*populate_panel(&(screen->right));*/
}

static void
one_player_loop(void)
{
	int ch, trys = 0;
	Screen screen;
	Point l_cursor = {.x = 4, .y = 4};
	Point r_cursor = {.x = 4, .y = 4};

	init_screen(&screen);
	draw_screen(screen);
	colorize_screen(screen);
	one_player_populate(&screen);
	draw_all_status(screen);
	mvwprintw(stdscr, LINES - 2, (COLS - 20)/ 2, "Touch the enemy's ships");

	while (! is_finished(screen)) {
		if (current_player(screen) == LEFT_PLAYER) {
			bot_move(&screen, ++trys);
			continue;
		} else {
			choose_target(screen.right, &r_cursor);
			make_move(l_cursor, r_cursor, &screen);
		}
		trys = 0;
	}

	reveal(&screen);
	mvwprintw(stdscr, LINES - 2, (COLS - 20)/ 2, "                         ");
	mvwprintw(stdscr, LINES - 2, (COLS - 35)/ 2, "What a game ! Hit space to continue");
	while ((ch = getch()) != ' ')
		continue;
	werase(stdscr);
	refresh();
}

void
main_loop(void)
{
	set_curses();
	if (!has_colors()) {
		endwin();
		return;
	}
	while (TRUE)
		switch(greetings()) {
		case 1:
			werase(stdscr);
			refresh();
			one_player_loop();
			break;
		case 2:
			werase(stdscr);
			refresh();
			endwin();
			two_player_loop();
			break;
		case 3:
			endwin();
			return;
		default:
			break;
		}
	clrtoeol();
	refresh();
	endwin();
}
