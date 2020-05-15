#include "variables.h"
#include "graphic.h"
#include "core.h"

int main(int argc, char *argv[])
{
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	start_color();
	noecho();
	curs_set(0);

	if (has_colors() == FALSE) {
		endwin();
		return(1);
	}

	screen screen;
	screen = init_screen();
	refresh();
	draw_screen(screen);
	colorize_screen(screen);
	Point x = choose_target(screen.left);
	make_move(x, &(screen));
	colorize_screen(screen);
	draw_screen(screen);
	getch ();

	endwin();
	printf("Les coordonnées étaient: %d %d\n", x.x, x.y);
	return 0;
}


