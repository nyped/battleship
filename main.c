#include "variables.h"
#include "graphic.h"
#include "core.h"
#include "populate.h"

int main(void)
{
	set_curses();

	two_player_loop();

	return 0;
}
