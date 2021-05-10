#include "bot.h"

int main(void)
{
	Game	game;

	game.nb_rich0 = 0;
	game.nb_rich1 = 0;
	game.nb_rich2 = 0;
	game.nb_rich3 = 0;
	game.max_tree3 = 4;
	game.scan_grid();
	game.fill_dist();
	while (1) {
		game.scan_info();
		game.init_all();
		game.scan_trees();
		game.scan_moves();

		game.print_start_info();
		game.print_day();

		game.fill_diag();
		game.calculate_spooky_cells();
		game.calculate_reachable();
		game.calculate_cost();
		game.calculate_score();
		game.sort_on_score();
		game.f_rich = 2 * sqrt((double)game.nb_rich0);
		if (game.f_rich == 0)
			game.f_rich = 1;
		game.action();
		return (0);
	}
}

