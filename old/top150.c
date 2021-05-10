#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct s_game
{
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
	int		day;
	int		cells;
	float	f_rich;
	int		f_alone;
    int     max_tree3;
	float   max_score;
	int		stop_seed;
}          t_game;

/*
 ** Print useful
 */

void    print_tab(int tab[37])
{
	for (int i = 0; i < 37; i++)
	{
		if (tab[i] >= 0)
			fprintf(stderr, "%3d ", tab[i]);
	}
	fprintf(stderr, "\n");
}

void    print_float(float tab[37])
{
	for (int i = 0; i < 37; i++)
	{
		if (tab[i] >= 0.0)
			fprintf(stderr, "%3d ", (int)tab[i]);
	}
	fprintf(stderr, "\n");
}

void	print_score_info(t_game game)
{
	fprintf(stderr, "        ");
	for (int i = 0; i < 37; i++)
		fprintf(stderr, "%3d ", i);
	fprintf(stderr, "\n");
	fprintf(stderr, "Score : ");
	print_float(game.score);
	fprintf(stderr, "Reach : ");
	print_tab(game.reachable);
	fprintf(stderr, "Cost :  ");
	print_tab(game.cost);
	fprintf(stderr, "\nSort :  ");
	print_tab(game.sorted);
}

void	print_dist(t_game game)
{
	for (int i = 0; i < game.cells; i++)
	{
		for (int j = 0; j < game.cells; j++)
			fprintf(stderr, "%d ", game.dist[i][j]);
		fprintf(stderr, "\n");
	}
}

void    print_day_info(t_game game)
{
	fprintf(stderr, "[DAY %d] sun %d r0[%d] r1[%d] r2[%d] r3[%d]\n", game.day, game.sun, game.nb_rich0, game.nb_rich1, game.nb_rich2, game.nb_rich3);
	fprintf(stderr, "\t richness[%f]\n", game.f_rich);
}

/*
** Is on diagonal
*/

int     is_reachable(t_game game, int i, int j)
{
    int neighs[18];

    for (int k = 0; k < 6; k++)
        neighs[k] = game.neigh_tab[i][k];
    for (int k = 0; k < 6; k++)
    {
        if (neighs[k] >= 0)
            neighs[6 + k] = game.neigh_tab[neighs[k]][k];
        else
            neighs[6 + k] = -1;
    }
    for (int k = 0; k < 6; k++)
    {
        if (neighs[6 + k] >= 0)
            neighs[12 + k] = game.neigh_tab[neighs[6 + k]][k];
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

void    fill_diag(t_game *game)
{
    int is_on_diag;

    for (int i = 0; i < game->cells; i++)
    {
        for (int j = i; j < game->cells; j++)
        {
            if (j > i)
            {
                is_on_diag = is_reachable(*game, i, j);
                game->on_diag[i][j] = is_on_diag;
                game->on_diag[j][i] = is_on_diag;
            }
            else
                game->on_diag[i][j] = 0;
        }
        game->no_diag[i] = 0;
    }

    for (int i = 0; i < game->cells; i++)
    {
        for (int j = 0; j < game->cells; j++)
        {
            if (game->owner[j] > 1 && game->on_diag[i][j])  // > 1 pour uniquement mes arbres
                game->no_diag[i]++;
        }
    }
}

/*
** Score based version
*/

void sort_on_score(t_game *game)
{
	int max;
	int max_index;
	int score_aux[37];

	for (int i = 0; i < 37; i++)
	{
		score_aux[i] = game->score[i];
		game->sorted[i] = -1;
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
		game->sorted[i] = max_index;
		score_aux[max_index] = -1;
	}
}

int cost(t_game game, int i)
{
	if (game.size[i] == 0)
		return (1 + game.nb_size1);
	if (game.size[i] == 1)
		return (3 + game.nb_size2);
	if (game.size[i] == 2)
		return (7 + game.nb_size3);
	if (game.size[i] == 3)
		return (4);
	if (game.owner[i] == 0)
		return (game.nb_size0);
	return (0);
}

int     plant_seed(t_game *game, int i)
{
	int j;

	j = game->cells - 1;
    if (game->neigh_allies[i] != 0)
        return (0);
	while (j >= 0 && game->sorted[j] == -1)
		j--;
	while (j >= 0)
	{
		if (game->dist[i][game->sorted[j]] > 1 && game->dist[i][game->sorted[j]] <= game->size[game->sorted[j]])
		{
			printf("SEED %d %d welcome\n", game->sorted[j], i);
			return (1);
		}
		j--;
	}
	return (0);
}

int     execute_action(t_game *game, int i)
{
	if (game->owner[i] == 0)
		return (plant_seed(game, i));
	if (game->size[i] >= 0 && game->size[i] < 3)
	{
		printf("GROW %d be strong\n", i);
		return (1);
	}
	if (game->size[i] == 3)
	{
		printf("COMPLETE %d jackpot!\n", i);
		return (1);
	}
	return (0);
}

void    actualize_score(t_game *game)
{
	for (int i = 0; i < 37; i++)
	{
		if (game->owner[i] == 2 && game->size[i] == 3)
			game->score[i] = game->max_score + (6 - game->neigh_allies[i] - game->neigh_opp[i]);
	}
}

int     time_to_score(t_game game)
{
	static int  prev_opp_size3;

	if (23 - game.day <= game.nb_size3)
		return (1);
	if (game.nb_size3 > game.max_tree3)
		return (1);
	if (game.nb_size3 >= game.opp_size3 + game.opp_size2 + game.opp_size1)
		return (1);
	return (0);
}

void    action(t_game *game)
{
	int     father;
	int     i;

	if (time_to_score(*game)) // a calculer en fonction de l'opposant
	{
		actualize_score(game);
		sort_on_score(game);
	}
	print_score_info(*game);
	i = 0;
	while (i < game->cells && game->sorted[i] != -1 )
	{
		if (game->cost[game->sorted[i]] <= game->sun)
		{	
            if (execute_action(game, game->sorted[i]))
				return ;
		}
		i++;
	}
	printf("WAIT zZZzzZzzz\n");
}

/*
 ** Calculate useful data
 */

void    calculate_spooky_cells(t_game *game)
{
	int nb;
	int next;

	for (int i = 0; i < 37; i++)
	{
		nb = game->size[i];
		while (nb > 0)
		{
			next = game->neigh_tab[i][game->sun_dir];
			if (next < 0)
				break;
			game->spooky[next]++;
			nb--;
		}
	}
}

void    calculate_reachable(t_game *game)
{
	int next;
	int nb;

	for (int i = 0; i < 37; i++)
	{
		for (int j = 0; j < 37; j++)
		{
			if (game->owner[j] == 2 && game->dist[i][j] <= game->size[j])
			{
				game->reachable[i] = 1;
				break ;
			}
		}
	}
}

void     nb_neigh(t_game *game, int i)
{
	int neigh;

	for (int j = 0; j < 6; j++)
	{
		neigh = game->neigh_tab[i][j];
		if (neigh >= 0)
		{
			if (game->owner[neigh] == 1)
				game->neigh_opp[i]++;
			else if (game->owner[neigh] == 2)
				game->neigh_allies[i]++;
		}
	}
}

float   size_factor(t_game game, int i)
{
	float   size_f;

	if (game.size[i] < 3)
		size_f = game.size[i] + 2;
    else
        size_f = 1;
	return (size_f);
}

float   spot_factor(t_game game, int i)
{
	float   alone_f;
	float   richness_f;
    float   richness_bgn;
	float   shadow_f;
    float   diag_f;
	float   spot_f;

	alone_f = (float)(7 - game.neigh_opp[i] - game.neigh_allies[i]);
	richness_f = game.richness_tab[i];
	shadow_f = (game.neigh_opp[i] > game.neigh_allies[i]) * 1.0;
    diag_f = 7.0 / (1 + game.no_diag[i]);
	spot_f = game.f_alone * alone_f + game.f_rich * richness_f * diag_f + shadow_f + game.spooky[i];
	return (spot_f);
}

float   cost_factor(t_game game, int i)
{
	float cost_f;

	cost_f = 12.0 / (1.0 + game.cost[i]);
	return (cost_f);
}

void    calculate_cost(t_game *game)
{
	for (int i = 0; i < game->cells; i++)
		game->cost[i] = cost(*game, i);
}

void    calculate_score(t_game *game)
{
	float max;

	max = 0.0;
	for (int i = 0; i < game->cells; i++)
		nb_neigh(game, i);
	for (int i = 0; i < game->cells; i++)
	{
		if ((game->owner[i] == 2 && game->dormant[i] == 0)
				|| (game->owner[i] == 0 && game->day < game->stop_seed && game->reachable[i] && game->richness_tab[i] > 0))
			game->score[i] = spot_factor(*game, i) * size_factor(*game, i);
		else
			game->score[i] = 0.0;
		if (max < game->score[i])
			max = game->score[i];
	}
	game->max_score = max;
}

/*
** Scan game info
*/

void	fill_distance(t_game *game, int index)
{
	for (int i = 0; i < 6; i++)
	{
		if (game->neigh_tab[index][i] >= 0)
			game->dist[index][game->neigh_tab[index][i]] = 1;
	}
}

void	scan_grid(t_game *game)
{
	int richness;
	int index;
	int neigh_0;
	int neigh_1;
	int neigh_2;
	int neigh_3;
	int neigh_4;
	int neigh_5;
	int number_of_cells;

	scanf("%d", &number_of_cells);
	game->cells = number_of_cells;
	for (int i = 0; i < game->cells; i++)
	{
		for (int j = 0; j < game->cells; j++)
			game->dist[i][j] = 0;
	}
	for (int i = 0; i < game->cells; i++)
	{
		scanf("%d%d%d%d%d%d%d%d", &index, &richness, &neigh_0, &neigh_1, &neigh_2, &neigh_3, &neigh_4, &neigh_5);
		game->richness_tab[index] = richness;
		if (richness == 0)
			game->nb_rich0++;
		else if (richness == 1)
			game->nb_rich1++;
		else if (richness == 2)
			game->nb_rich2++;
		else
			game->nb_rich3++;
		game->neigh_tab[index][0] = neigh_0;
		game->neigh_tab[index][1] = neigh_1;
		game->neigh_tab[index][2] = neigh_2;
		game->neigh_tab[index][3] = neigh_3;
		game->neigh_tab[index][4] = neigh_4;
		game->neigh_tab[index][5] = neigh_5;
	}
}

void	scan_info(t_game *game)
{
	int day;
	int sun;
	int score;
	int nutrients;
	int opp_sun;
	int opp_score;
	int opp_is_waiting;
	int number_of_trees;

	scanf("%d", &day);
	game->day = day;
	game->sun_dir = day % 6;
	scanf("%d", &nutrients);
	scanf("%d%d", &sun, &score);
	game->sun = sun;
	scanf("%d%d%d", &opp_sun, &opp_score, &opp_is_waiting);
	scanf("%d", &number_of_trees);
	game->tot_tree = number_of_trees;
}

void    init_all(t_game *game)
{
	for (int i = 0 ; i < game->cells; i++)
	{
		game->owner[i] = 0;
		game->size[i] = -1;
		game->dormant[i] = 0;
		game->spooky[i] = 0;
		game->reachable[i] = 0;
		game->score[i] = 0;
		game->neigh_allies[i] = 0;
		game->neigh_opp[i] = 0;
		game->cost[i] = 0;
	}
	game->nb_size0 = 0;
	game->nb_size1 = 0;
	game->nb_size2 = 0;
	game->nb_size3 = 0;
	game->opp_size0 = 0;
	game->opp_size1 = 0;
	game->opp_size2 = 0;
	game->opp_size3 = 0;
	game->f_rich = 2;
	game->f_alone = 3;
	game->stop_seed = 16;
}

void	scan_trees(t_game *game)
{
	int cell_index;
	int size;
	int is_mine;
	int is_dormant;

	for (int i = 0; i < game->tot_tree; i++) {
		scanf("%d%d%d%d", &cell_index, &size, &is_mine, &is_dormant);
		game->owner[cell_index] = 1 + is_mine;
		game->size[cell_index] = size;
		if (game->owner[cell_index] == 2)
		{
			if (size == 0)
				game->nb_size0++;
			else if (size == 1)
				game->nb_size1++;
			else if (size == 2)
				game->nb_size2++;
			else if (size == 3)
				game->nb_size3++;
		}
		else if (game->owner[cell_index] == 1)
		{
			if (size == 0)
				game->opp_size0++;
			else if (size == 1)
				game->opp_size1++;
			else if (size == 2)
				game->opp_size2++;
			else if (size == 3)
				game->opp_size3++;
		}
		game->dormant[cell_index] = is_dormant;
	}
	game->nb_tree = game->nb_size0 + game->nb_size1 + game->nb_size2 + game->nb_size3;
}

void	scan_moves(t_game *game)
{
	int		number_of_possible_moves;
	char	possible_move[32];

	(void)game;
	scanf("%d", &number_of_possible_moves); fgetc(stdin);
	for (int i = 0; i < number_of_possible_moves; i++){
		scanf("%[^\n]", possible_move); fgetc(stdin);
	}
}

void    fill_dist(t_game *game)
{
	int     k;
	char    *str_dist = "0111111222222222222333333333333333333\n012221112233333221222333444444433322\n01222211122333332322222333444444433\n0122322111223333433322222333444444\n012333221112233444433322222333444\n01333332211122444444433322222333\n0223333322111333444444433322222\n012344444322112334555555543321\n01233444332211223444555544432\n0123444443321112334555555543\n012334443432211223444555544\n01234444543321112334555555\n0123344544432211223444555\n012344555543321112334555\n01233555544432211223444\n0123555555543321112334\n012444555544432211223\n01334555555543321112\n0223444555544432211\n012345666666654321\n01234555666654432\n0123445666655543\n012345666666654\n01234555666654\n0123445666655\n013345666666\n01234555666\n0123445666\n012345666\n01234555\n0123445\n012345\n01234\n0123\n012\n01\n0";

	k = 0;
	for (int i = 0; i < 37; i++)
	{
		for (int j = 0; j < 37; j++)
		{
			if (j >= i)
			{
				game->dist[i][j] = str_dist[k] - '0';
				game->dist[j][i] = str_dist[k] - '0';
				k++;
			}
		}
		k++;
	}
}

void    print_diag(t_game game)
{
    fprintf(stderr, "Free diag : ");
    for (int i = 0; i < game.cells; i++)
    {
        if (game.no_diag[i] > 0)
            fprintf(stderr, "%d[%d] ", i, game.no_diag[i]);
    }
    fprintf(stderr, "\n");
}

int main()
{
	t_game  game;

	game.nb_rich0 = 0;
	game.nb_rich1 = 0;
	game.nb_rich2 = 0;
	game.nb_rich3 = 0;
    game.max_tree3 = 4; // 4 seems opti
	scan_grid(&game);
	fill_dist(&game);
	while (1) {
		scan_info(&game);
		init_all(&game);
		scan_trees(&game);
		scan_moves(&game);
        fill_diag(&game);
		calculate_spooky_cells(&game);
		calculate_reachable(&game);
		calculate_cost(&game);
		calculate_score(&game);
		sort_on_score(&game);
		game.f_rich = 2 * sqrt((double)game.nb_rich0);
		if (game.f_rich == 0)
			game.f_rich = 1;
		print_day_info(game);
        print_diag(game);
		action(&game);
	}
	return (0);
}
