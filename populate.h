int
number_neighbours(Point point, Panel panel, int tile);

void
populate_panel(Panel *panel);

bool
add_ship(Point *point, Panel *panel, int type);

bool
populate_ok(Point point, Panel panel, int index);

void
populate(Screen *screen);

void
population_to_real_grid(Panel *panel);
