#include <curses.h>
#include <locale.h>


int main () {
	setlocale(LC_ALL, "");
	initscr();
	clear();
	move(10,20);
	addstr("Hello, world");
	move(LINES-1,0);
	refresh();
	getch();
	endwin();

	return(0);
}
