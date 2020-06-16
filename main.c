#include "variables.h"
#include "graphic.h"
#include "core.h"
#include "populate.h"

int main(void)
{
	set_curses();

	one_player_loop();

	return 0;
}
