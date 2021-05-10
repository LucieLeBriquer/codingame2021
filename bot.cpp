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

// -----------------------------
//        Useful printing
// -----------------------------

void    print_tab(int tab[37])
{
	for (int i = 0; i < 37; i++)
	{
		if (tab[i] >= 0)
			cerr << setw(3) << tab[i] << " ";
	}
	cerr << endl;
}

void    print_float(float tab[37])
{
	for (int i = 0; i < 37; i++)
	{
		if (tab[i] >= 0.0)
			cerr << setw(3) << (int)tab[i] << " ";
	}
	cerr << endl;
}

void	Game::print_score_info(void)
{
	cerr << "        ";
	for (int i = 0; i < 37; i++)
		cerr << setw(3) << i << " ";
	cerr << endl;
	cerr << "Score : ";
	print_float(score);
	cerr << "Reach : ";
	print_tab(reachable);
	cerr << "Cost :  ";
	print_tab(cost);
	cerr << endl << "Sort :  ";
	print_tab(sorted);
}

void	Game::print_dist(void)
{
	for (int i = 0; i < cells; i++)
	{
		for (int j = 0; j < cells; j++)
			cerr << dist[i][j] << " ";
		cerr << endl;
	}
}

void    Game::print_day_info(void)
{
	cerr << "[DAY " << day << "] sun " << sun << " r0[" << nb_rich0 << "]";
	cerr << " r1[" << nb_rich1 << "]";
	cerr << " r2[" << nb_rich2 << "]";
	cerr << " r3[" << nb_rich3 << "]";
	cerr << "  rich[" << f_rich << "]" << endl;
}

void    Game::print_diag(void)
{
	cerr << "Free diag : ";
	for (int i = 0; i < cells; i++)
	{
		if (no_diag[i] > 0)
			cerr << i << "[" << no_diag[i] << "] ";
	}
	cerr << endl;
}

void    Game::print_start_info()
{
	cerr << cells << endl;
	for (int i = 0; i < cells; i++) {
		cerr << i << " " << richness_tab[i] << " ";
		for (int j = 0; j < 6; j++) {
			cerr << neigh_tab[i][j];
			if (j < 5)
				cerr << " ";
		}
		cerr << endl;
	}
}

void    Game::print_day() {
	cerr << day << endl;
	cerr << nutrients << endl;
	cerr << sun << " " << score_tot << endl;
	cerr << oppSun << " " << oppScore << " " << oppIsWaiting << endl;
	cerr << tot_tree << endl;
	for (int i = 0; i < cells; i++)
	{
		if (owner[i] > 0)
			cerr << i << " " << size[i] << " " << owner[i] - 1 << " " << dormant[i] << endl;
	}
	cerr << nb_actions << endl;
	for (int i = 0; i < nb_actions; i++) {
		cerr << actions[i];
		if (i < nb_actions - 1)
			cerr << " ";
	}
	cerr << endl;
}


// -----------------------------
//        Is on diagonal
// -----------------------------

int     Game::is_reachable(int i, int j)
{
	int neighs[18];

	for (int k = 0; k < 6; k++)
		neighs[k] = neigh_tab[i][k];
	for (int k = 0; k < 6; k++)
	{
		if (neighs[k] >= 0)
			neighs[6 + k] = neigh_tab[neighs[k]][k];
		else
			neighs[6 + k] = -1;
	}
	for (int k = 0; k < 6; k++)
	{
		if (neighs[6 + k] >= 0)
			neighs[12 + k] = neigh_tab[neighs[6 + k]][k];
		else
			neighs[12 + k] = -1;
	}
	for (int k = 0; k < 18; k++)
	{
		if (neighs[k] == j)
			return (1);
	}
	return (0);
}

void    Game::fill_diag(void)
{
	int is_on_diag;

	for (int i = 0; i < cells; i++)
	{
		for (int j = i; j < cells; j++)
		{
			if (j > i)
			{
				is_on_diag = is_reachable(i, j);
				on_diag[i][j] = is_on_diag;
				on_diag[j][i] = is_on_diag;
			}
			else
				on_diag[i][j] = 0;
		}
		no_diag[i] = 0;
	}

	for (int i = 0; i < cells; i++)
	{
		for (int j = 0; j < cells; j++)
		{
			if (owner[j] > 1 && on_diag[i][j])
				no_diag[i]++;
		}
	}
}

// -----------------------------
//      Score based version
// -----------------------------

void	Game::sort_on_score(void)
{
	int max;
	int max_index;
	int score_aux[37];

	for (int i = 0; i < 37; i++)
	{
		score_aux[i] = score[i];
		sorted[i] = -1;
	}
	for (int i = 0; i < 37; i++)
	{
		max = -1;
		for (int j = 0; j < 37; j++)
		{
			if (score_aux[j] > max)
			{
				max = score_aux[j];
				max_index = j;
			}
		}
		if (max <= 0)
			break ;
		sorted[i] = max_index;
		score_aux[max_index] = -1;
	}
}

int		Game::cost_case(int i)
{
	if (size[i] == 0)
		return (1 + nb_size1);
	if (size[i] == 1)
		return (3 + nb_size2);
	if (size[i] == 2)
		return (7 + nb_size3);
	if (size[i] == 3)
		return (4);
	if (owner[i] == 0)
		return (nb_size0);
	return (0);
}

int     Game::plant_seed(int i)
{
	int j;

	j = cells - 1;
	if (neigh_allies[i] != 0)
		return (0);
	while (j >= 0 && sorted[j] == -1)
		j--;
	while (j >= 0)
	{
		if (dist[i][sorted[j]] > 1 && dist[i][sorted[j]] <= size[sorted[j]])
		{
			cout << "SEED " << sorted[j] << " " << i << "welcome" << endl;
			return (1);
		}
		j--;
	}
	return (0);
}

int     Game::execute_action(int i)
{
	if (owner[i] == 0)
		return (plant_seed(i));
	if (size[i] >= 0 && size[i] < 3)
	{
		printf("GROW %d be strong\n", i);
		return (1);
	}
	if (size[i] == 3)
	{
		printf("COMPLETE %d jackpot!\n", i);
		return (1);
	}
	return (0);
}

void    Game::actualize_score(void)
{
	for (int i = 0; i < 37; i++)
	{
		if (owner[i] == 2 && size[i] == 3)
			score[i] = max_score + (6 - neigh_allies[i] - neigh_opp[i]);
	}
}

int     Game::time_to_score(void)
{
	static int  prev_opp_size3;

	if (23 - day <= nb_size3)
		return (1);
	if (nb_size3 > max_tree3)
		return (1);
	if (nb_size3 >= opp_size3 + opp_size2 + opp_size1)
		return (1);
	return (0);
}

void    Game::action(void)
{
	int     father;
	int     i;

	if (time_to_score())
	{
		actualize_score();
		sort_on_score();
	}
	print_score_info();
	i = 0;
	while (i < cells && sorted[i] != -1)
	{
		if (cost[sorted[i]] <= sun)
		{	
			if (execute_action(sorted[i]))
				return ;
		}
		i++;
	}
	printf("WAIT zZZzzZzzz\n");
}

// -----------------------------
//     Calculate useful data
// -----------------------------

void    Game::calculate_spooky_cells(void)
{
	int nb;
	int next;

	for (int i = 0; i < 37; i++)
	{
		nb = size[i];
		while (nb > 0)
		{
			next = neigh_tab[i][sun_dir];
			if (next < 0)
				break;
			spooky[next]++;
			nb--;
		}
	}
};

void    Game::calculate_reachable(void)
{
	int next;
	int nb;

	for (int i = 0; i < 37; i++)
	{
		for (int j = 0; j < 37; j++)
		{
			if (owner[j] == 2 && dist[i][j] <= size[j])
			{
				reachable[i] = 1;
				break ;
			}
		}
	}
}

void     Game::nb_neigh(int i)
{
	int neigh;

	for (int j = 0; j < 6; j++)
	{
		neigh = neigh_tab[i][j];
		if (neigh >= 0)
		{
			if (owner[neigh] == 1)
				neigh_opp[i]++;
			else if (owner[neigh] == 2)
				neigh_allies[i]++;
		}
	}
}

float   Game::size_factor(int i)
{
	float   size_f;

	if (size[i] < 3)
		size_f = size[i] + 2;
	else
		size_f = 1;
	return (size_f);
}

float   Game::spot_factor(int i)
{
	float   alone_f;
	float   richness_f;
	float   richness_bgn;
	float   shadow_f;
	float   diag_f;
	float   spot_f;

	alone_f = (float)(7 - neigh_opp[i] - neigh_allies[i]);
	richness_f = richness_tab[i];
	shadow_f = (neigh_opp[i] > neigh_allies[i]) * 1.0;
	diag_f = 7.0 / (1 + no_diag[i]);
	spot_f = f_alone * alone_f + f_rich * richness_f * diag_f + shadow_f + spooky[i];
	return (spot_f);
}

float   Game::cost_factor(int i)
{
	float cost_f;

	cost_f = 12.0 / (1.0 + cost[i]);
	return (cost_f);
}

void    Game::calculate_cost(void)
{
	for (int i = 0; i < cells; i++)
		cost[i] = cost_case(i);
}

void    Game::calculate_score(void)
{
	float max;

	max = 0.0;
	for (int i = 0; i < cells; i++)
		nb_neigh(i);
	for (int i = 0; i < cells; i++)
	{
		if ((owner[i] == 2 && dormant[i] == 0)
				|| (owner[i] == 0 && day < stop_seed && reachable[i] && richness_tab[i] > 0))
			score[i] = spot_factor(i) * size_factor(i);
		else
			score[i] = 0.0;
		if (max < score[i])
			max = score[i];
	}
	max_score = max;
}

// -----------------------------
//        Scan game info
// -----------------------------

void	Game::fill_distance(int index)
{
	for (int i = 0; i < 6; i++)
	{
		if (neigh_tab[index][i] >= 0)
			dist[index][neigh_tab[index][i]] = 1;
	}
}

void	Game::scan_grid(void)
{
	int richness;
	int index;
	int neigh0;
	int neigh1;
	int neigh2;
	int neigh3;
	int neigh4;
	int neigh5;

	cin >> cells; cin.ignore();
	for (int i = 0; i < cells; i++)
	{
		for (int j = 0; j < cells; j++)
			dist[i][j] = 0;
	}
	for (int i = 0; i < cells; i++)
	{
		cin >> index >> richness >> neigh0 >> neigh1 >> neigh2 >> neigh3 >> neigh4 >> neigh5; cin.ignore();
		richness_tab[index] = richness;
		if (richness == 0)
			nb_rich0++;
		else if (richness == 1)
			nb_rich1++;
		else if (richness == 2)
			nb_rich2++;
		else
			nb_rich3++;
		neigh_tab[index][0] = neigh0;
		neigh_tab[index][1] = neigh1;
		neigh_tab[index][2] = neigh2;
		neigh_tab[index][3] = neigh3;
		neigh_tab[index][4] = neigh4;
		neigh_tab[index][5] = neigh5;
	}
}

void	Game::scan_info(void)
{
	int opp_sun;
	int opp_score;
	int opp_is_waiting;
	int number_of_trees;

	cin >> day; cin.ignore();
	sun_dir = day % 6;
	cin >> nutrients; cin.ignore();
	cin >> sun >> score_tot; cin.ignore();
	cin >> oppSun >> oppScore >> oppIsWaiting; cin.ignore();
	cin >> tot_tree; cin.ignore();
}

void    Game::init_all(void)
{
	for (int i = 0 ; i < cells; i++)
	{
		owner[i] = 0;
		size[i] = -1;
		dormant[i] = 0;
		spooky[i] = 0;
		reachable[i] = 0;
		score[i] = 0;
		neigh_allies[i] = 0;
		neigh_opp[i] = 0;
		cost[i] = 0;
	}
	nb_size0 = 0;
	nb_size1 = 0;
	nb_size2 = 0;
	nb_size3 = 0;
	opp_size0 = 0;
	opp_size1 = 0;
	opp_size2 = 0;
	opp_size3 = 0;
	f_rich = 2;
	f_alone = 3;
	stop_seed = 16;
	actions.clear();
}

void	Game::scan_trees(void)
{
	int cell_index;
	int size_tree;
	int is_mine;
	int is_dormant;

	for (int i = 0; i < tot_tree; i++) {
		cin >> cell_index >> size_tree >> is_mine >> is_dormant; cin.ignore();
		if (cell_index < cells && cell_index >= 0)
		{
			owner[cell_index] = 1 + is_mine;
			size[cell_index] = size_tree;
			if (owner[cell_index] == 2)
			{
				if (size_tree == 0)
					nb_size0++;
				else if (size_tree == 1)
					nb_size1++;
				else if (size_tree == 2)
					nb_size2++;
				else if (size_tree == 3)
					nb_size3++;
			}
			else if (owner[cell_index] == 1)
			{
				if (size_tree == 0)
					opp_size0++;
				else if (size_tree == 1)
					opp_size1++;
				else if (size_tree == 2)
					opp_size2++;
				else if (size_tree == 3)
					opp_size3++;
			}
			dormant[cell_index] = is_dormant;
		}
	}
	nb_tree = nb_size0 + nb_size1 + nb_size2 + nb_size3;
}

void	Game::scan_moves(void)
{
	cin >> nb_actions; cin.ignore();
	for (int i = 0; i < nb_actions; i++) {
		string possibleAction;
		getline(cin, possibleAction);
		actions.push_back(possibleAction);
	}
}

void    Game::fill_dist(void)
{
	int     k;
	const char *str_dist = "0111111222222222222333333333333333333\n012221112233333221222333444444433322\n01222211122333332322222333444444433\n0122322111223333433322222333444444\n012333221112233444433322222333444\n01333332211122444444433322222333\n0223333322111333444444433322222\n012344444322112334555555543321\n01233444332211223444555544432\n0123444443321112334555555543\n012334443432211223444555544\n01234444543321112334555555\n0123344544432211223444555\n012344555543321112334555\n01233555544432211223444\n0123555555543321112334\n012444555544432211223\n01334555555543321112\n0223444555544432211\n012345666666654321\n01234555666654432\n0123445666655543\n012345666666654\n01234555666654\n0123445666655\n013345666666\n01234555666\n0123445666\n012345666\n01234555\n0123445\n012345\n01234\n0123\n012\n01\n0";

	k = 0;
	for (int i = 0; i < 37; i++)
	{
		for (int j = 0; j < 37; j++)
		{
			if (j >= i)
			{
				dist[i][j] = str_dist[k] - '0';
				dist[j][i] = str_dist[k] - '0';
				k++;
			}
		}
		k++;
	}
}

int main()
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
	}
	return (0);
}
