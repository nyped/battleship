#include <ncurses.h>
#include "graphic.h"

int main(int argc, char *argv[])
{	
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	start_color();
	noecho();
	refresh();

	screen screen;
	screen = init_screen();
	draw_screen(screen);
	refresh();

	getch ();

	endwin();			/* End curses mode		  */
	return 0;
}


