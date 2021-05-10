#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

class Game
{
	public:
		// Attributes
		int		dist[37][37];
		int		richness_tab[37];
		int		neigh_tab[37][6];
		int		on_diag[37][37];
		int     no_diag[37];
		int		reachable[37];
		int		owner[37];
		int		size[37];
		int		sorted[37];
		int		dormant[37];
		float	score[37];
		int		neigh_opp[37];
		int		neigh_allies[37];
		int		spooky[37];
		int		cost[37];
		int		nb_size0;
		int		nb_size1;
		int		nb_size2;
		int		nb_size3;
		int		opp_size0;
		int		opp_size1;
		int		opp_size2;
		int		opp_size3;
		int		nb_rich0;
		int		nb_rich1;
		int		nb_rich2;
		int		nb_rich3;
		int		nb_tree;
		int		tot_tree;
		int		sun_dir;
		int		sun;
		int		score_tot;
		int		nutrients;
		int		day;
		int		oppSun;
		int		oppScore;
		bool	oppIsWaiting;
		int		cells;
		float	f_rich;
		int		f_alone;
		int     max_tree3;
		float   max_score;
		int		stop_seed;
		int     nb_actions;
		vector<string>   actions;

		// Methods

		void	scan_grid(void);
		void	scan_info(void);
		void	init_all(void);
		void	scan_trees(void);
		void	scan_moves(void);
		void	fill_dist(void);
		void	print_diag(void);
		void	fill_distance(int index);
		void    calculate_spooky_cells(void);
		void    calculate_reachable(void);
		void	nb_neigh(int i);
		float   size_factor(int i);
		float   spot_factor(int i);
		float   cost_factor(int i);
		void    calculate_cost(void);
		void    calculate_score(void);
		void	sort_on_score(void);
		int		cost_case(int i);
		int     plant_seed(int i);
		int     execute_action(int i);
		void    actualize_score(void);
		int		time_to_score(void);
		void    action(void);
		void	fill_diag(void);
		int     is_reachable(int i, int j);
		void	print_score_info(void);
		void	print_dist(void);
		void    print_day_info(void);
		void    print_day(void);
		void    print_start_info(void);
};
