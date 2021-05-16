#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#define WAIT 0
#define SEED 1
#define GROW 2
#define COMPLETE 3

#define ALLY 2
#define OPPONENT 1
#define	DRAW 0

using namespace std;

class Game
{
	public:
		// Attributes
		int		dist[37][37];
		int		richness[37];
		int		neigh[37][6];
		int		onSameDiag[37][37];
		int     numberOfDiag[37];
		int		reachable[37];
		int		owner[37];
		int		size[37];
		vector<int> sorted;
		vector<int> sortedSeed;
		vector<int> sortedComplete;
		vector<int> sortedGrow;
		int		dormant[37];
		float	score[37];
		int		spooky[37];
		int		spookyT1[37];
		int		spookyT2[37];
		int		spookyT3[37];
		int		spookyT4[37];
		int		spookyT5[37];
		float	spookyMoy[37];
		int		cost[37];
		int		oppNeighs[37];
		int		allyNeighs[37];
        int     totNeighs[37];

		int		allySize0;
		int		allySize1;
		int		allySize2;
		int		allySize3;
        int     allyExternSize3;
		int		oppSize0;
		int		oppSize1;
		int		oppSize2;
		int		oppSize3;
		int		boardRich0;
		int		boardRich1;
		int		boardRich2;
		int		boardRich3;

		int		allyTrees;
		int		oppTrees;
		int		boardTrees;
		int		sunDirection;
		int		allySun;
		int		allyScore;
		int		nutrients;
		int		day;
		int		oppSun;
		int		oppScore;
		bool	oppIsWaiting;
		int		cells;

		int		gameTurns;
		float	richnessImportance;
		int		aloneImportance;
		int     maxTreeSize3;
		float   maxMoveScore;
		int		stopSeeding;
		int     numberOfActions;
		vector<string>   actions;
		vector<int>   oppActionsVect;
		vector<int>   allyActionsVect;

		int		allyDo[3];
		int		oppDo[3];
		int		winner;

		int		sunPerTurn;
		int		sunPerTurnNotSpooky;

		// Methods

		void	scanGrid();
		void	scanInfo();
		void	initBoard();
		void	scanTrees();
		void	scanMoves();
		void	scanGridStream(ifstream& file);
		void	scanInfoStream(ifstream& file);
		void	scanTreesStream(ifstream& file);
		void	scanMovesStream(ifstream& file);
		void	fillDist();
		void	printDiag();
		void	fillDistance(int index);
		void    calculateSpooky();
		void    calculateReachable();
		void	numberNeigh(int i);
		float   sizeFactor(int i);
		float   spotFactor(int i);
		float   costFactor(int i);
		float   sunFactor(int i);
		void    calculateCost();
		void    calculateScore();
		void	sortOnScore();
		int		costOf(int i, int player);
		int     plantSeed(int i);
		int     executeAction(int i);
		void    actualizeScore();
		int		timeToScore();
		void    action();
		void	fillDiag();
		int     isReachable(int i, int j);
		void	printScoreOfBoard();
		void	printDist();
		void    printInfo();
		void    printInput();
		void	drawBoard();
		void	updateBoard();
		void	getFinalScore(int print, int seed);
		void	randomOpponentMove(int seed);
		void	calculateAllActions();
		void	getRichnessImportance();
		void	getSunPerTurn();

		void	calculateAll();
		void	initAll();
};

// -----------------------------
//        Useful printing
// -----------------------------

void    printTab(int tab[37])
{
	for (int i = 0; i < 37; i++)
	{
		if (tab[i] >= 0)
			cerr << setw(3) << tab[i] << " ";
	}
	cerr << endl;
}

void    printFloat(float tab[37])
{
	for (int i = 0; i < 37; i++)
	{
		if (tab[i] >= 0.0)
			cerr << setw(3) << (int)tab[i] << " ";
	}
	cerr << endl;
}

void	Game::printScoreOfBoard()
{
	cerr << "Sort :  ";
	for (int i = 0; i < sorted.size(); i++)
		cerr << setw(3) << sorted[i] << " ";
    cerr << endl;
	cerr << "Cost :  ";
    for (int i = 0; i < sorted.size(); i++)
		cerr << setw(3) << cost[sorted[i]] << " ";
	cerr << endl;
    cerr << "Score : ";
    for (int i = 0; i < sorted.size(); i++)
		cerr << setw(3) << (int) score[sorted[i]] << " ";
	cerr << endl;
}

void	Game::printDist()
{
	for (int i = 0; i < cells; i++)
	{
		for (int j = 0; j < cells; j++)
			cerr << dist[i][j] << " ";
		cerr << endl;
	}
}

void    Game::printInfo()
{
	cerr << "[DAY " << day << "] sun " << allySun << " r0[" << boardRich0 << "]";
	cerr << " r1[" << boardRich1 << "]";
	cerr << " r2[" << boardRich2 << "]";
	cerr << " r3[" << boardRich3 << "]";
	cerr << "  rich[" << richnessImportance << "]" << endl;
}

void    Game::printDiag()
{
	cerr << "Free diag : ";
	for (int i = 0; i < cells; i++)
	{
		if (numberOfDiag[i] > 0)
			cerr << i << "[" << numberOfDiag[i] << "] ";
	}
	cerr << endl;
}

void    Game::printInput() {
	cerr << cells << endl;
	for (int i = 0; i < cells; i++) {
		cerr << i << " " << richness[i] << " ";
		for (int j = 0; j < 6; j++) {
			cerr << neigh[i][j];
			if (j < 5)
				cerr << " ";
		}
		cerr << endl;
	}
	cerr << day << endl;
	cerr << nutrients << endl;
	cerr << allySun << " " << allyScore << endl;
	cerr << oppSun << " " << oppScore << " " << oppIsWaiting << endl;
	cerr << boardTrees << endl;
	for (int i = 0; i < cells; i++)
	{
		if (owner[i] > 0)
			cerr << i << " " << size[i] << " " << owner[i] - 1 << " " << dormant[i] << endl;
	}
	cerr << numberOfActions << endl;
	for (int i = 0; i < numberOfActions; i++) {
		cerr << actions[i];
		if (i < numberOfActions - 1)
			cerr << endl;
	}
	cerr << endl;
}


// -----------------------------
//        Is on diagonal
// -----------------------------

int     Game::isReachable(int i, int j)
{
	int neighs[18];

	for (int k = 0; k < 6; k++)
		neighs[k] = neigh[i][k];
	for (int k = 0; k < 6; k++)
	{
		if (neighs[k] >= 0)
			neighs[6 + k] = neigh[neighs[k]][k];
		else
			neighs[6 + k] = -1;
	}
	for (int k = 0; k < 6; k++)
	{
		if (neighs[6 + k] >= 0)
			neighs[12 + k] = neigh[neighs[6 + k]][k];
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

void    Game::fillDiag()
{
	int isOnSameDiag;

	for (int i = 0; i < cells; i++)
	{
		for (int j = i; j < cells; j++)
		{
			if (j > i)
			{
				isOnSameDiag = isReachable(i, j);
				onSameDiag[i][j] = isOnSameDiag;
				onSameDiag[j][i] = isOnSameDiag;
			}
			else
				onSameDiag[i][j] = 0;
		}
		numberOfDiag[i] = 0;
	}

	for (int i = 0; i < cells; i++)
	{
		for (int j = 0; j < cells; j++)
		{
			if (owner[j] > 1 && onSameDiag[i][j])
				numberOfDiag[i]++;
		}
	}
}

// -----------------------------
//	     Sun and shadows
// -----------------------------

void	Game::getSunPerTurn()
{
	sunPerTurn = 0;
	sunPerTurnNotSpooky = 0;
	for (int i = 0; i < cells; i++)
	{
		if (owner[i] == ALLY)
		{
			sunPerTurn += size[i];
			sunPerTurnNotSpooky += size[i] * spookyMoy[i];
		}
	}
}

// -----------------------------
//      Score based version
// -----------------------------

void	Game::getRichnessImportance()
{
	float	f;
	int		n0;
	int		n1;
	int		n2;
	int		n3;

	n0 = boardRich0;
	n1 = boardRich1;
	n2 = boardRich2;
	n3 = boardRich3;
	richnessImportance = (float)63 / (float)(3 * n3 + 2 * n2 + n1) + 2.0 * (n0 / 2 + 1);
}

void	Game::sortOnScore()
{
	int max;
	int max_index;
	int score_aux[37];

	sorted.clear();
	sortedComplete.clear();
	sortedGrow.clear();
	sortedSeed.clear();
	for (int i = 0; i < cells; i++)
		score_aux[i] = score[i];
	for (int i = 0; i < cells; i++)
	{
		max = -1;
		for (int j = 0; j < cells; j++)
		{
			if (score_aux[j] > max)
			{
				max = score_aux[j];
				max_index = j;
			}
		}
		if (max <= 0)
			break ;
		sorted.push_back(max_index);
		if (size[max_index] == 3)
			sortedComplete.push_back(max_index);
		else if (size[max_index] == -1)
			sortedSeed.push_back(max_index);
		else
			sortedGrow.push_back(max_index);
		score_aux[max_index] = -1;
	}
}

int		Game::costOf(int i, int player)
{
	if (owner[i] == 2)
	{
		if (size[i] == 0)
			return (1 + allySize1);
		if (size[i] == 1)
			return (3 + allySize2);
		if (size[i] == 2)
			return (7 + allySize3);
		if (size[i] == 3)
			return (4);
	}
	if (owner[i] == 1)
	{
		if (size[i] == 0)
			return (1 + oppSize1);
		if (size[i] == 1)
			return (3 + oppSize2);
		if (size[i] == 2)
			return (7 + oppSize3);
		if (size[i] == 3)
			return (4);
	}
	if (player == ALLY)
		return (allySize0);
	if (player == OPPONENT)
		return (oppSize0);
	return (0);
}

int     Game::plantSeed(int i)
{
	for (int j = sorted.size() - 1; j >= 0; j--)
	{
		if (dist[i][sorted[j]] > 1 && dist[i][sorted[j]] <= size[sorted[j]])
		{
			allyDo[0] = SEED;
			allyDo[1] = sorted[j];
			allyDo[2] = i;
			return (1);
		}
		j--;
	}
	return (0);
}

int     Game::executeAction(int i)
{
	if (owner[i] == 0)// && (allySize0 < 1 || (day == 23 && allySun % 3 >= costOf(i, ALLY))))
		return (plantSeed(i));
	if (size[i] >= 0 && size[i] < 3)
	{
		allyDo[0] = GROW;
		allyDo[1] = i;
		allyDo[2] = 0;
		return (1);
	}
	if (size[i] == 3 && (sunPerTurn >= 15 || day > 22)) // && day > 10 && (i < 19 || day > 21 || allyExternSize3 > 2))
	{
		allyDo[0] = COMPLETE;
		allyDo[1] = i;
		allyDo[2] = 0;
		return (1);
	}
	return (0);
}

void    Game::actualizeScore()
{
	for (int i = 0; i < 37; i++)
	{
		if (owner[i] == 2 && size[i] == 3)
			score[i] = maxMoveScore + richness[i]; // + (6 - allyNeighs[i] - oppNeighs[i]);
        if (owner[i] == 2)
            score[i] = size[i] * score[i];
    }
}

int     Game::timeToScore()
{
    if (23 - day <= allySize3)
		return (1);
	if (allySize3 >= maxTreeSize3)
		return (1);
    //if (allySize3 > oppSize3 + 1)
      //  return (1);
	return (0);
}

void    Game::action()
{
	allyDo[0] = WAIT;
	allyDo[1] = 0;
	allyDo[2] = 0;
	for (int i = sortedComplete.size() - 1; i >= 0; i--)
	{
		if (cost[sortedComplete[i]] <= allySun && executeAction(sortedComplete[i]))
			return ;
	}
	for (int i = 0; i < sortedGrow.size(); i++)
	{
		if (cost[sortedGrow[i]] <= allySun && executeAction(sortedGrow[i]))
				return ;
	}
	if (allySize0 > 0)
		return ;
	for (int i = 0; i < sortedSeed.size(); i++)
	{
		if (totNeighs[sortedSeed[i]] == 0 && cost[sortedSeed[i]] <= allySun && executeAction(sortedSeed[i]))
				return ;
	}
    for (int i = 0; i < sortedSeed.size(); i++)
	{
		if (allyNeighs[sortedSeed[i]] == 0 && cost[sortedSeed[i]] <= allySun && executeAction(sortedSeed[i]))
				return ;
	}
    for (int i = 0; i < sortedSeed.size(); i++)
	{
		if (cost[sortedSeed[i]] <= allySun && executeAction(sortedSeed[i]))
				return ;
	}
}

// -----------------------------
//     Calculate useful data
// -----------------------------

void    Game::calculateSpooky()
{
	int nb;
	int next;

	sunDirection = day % 6;
	for (int i = 0; i < 37; i++)
		spooky[i] = 0;
	for (int i = 0; i < 37; i++)
	{
		nb = size[i];
		next = i;
		while (nb > 0)
		{
			next = neigh[next][sunDirection];
			if (next < 0)
				break;
			if (size[next] <= size[i])
				spooky[next]++;
			nb--;
		}
		nb = size[i];
		next = i;
		while (nb > 0)
		{
			next = neigh[next][(sunDirection + 1) % 6];
			if (next < 0)
				break;
			if (size[next] <= size[i])
				spookyT1[next]++;
			nb--;
		}
		nb = size[i];
		next = i;
		while (nb > 0)
		{
			next = neigh[next][(sunDirection + 2) % 6];
			if (next < 0)
				break;
			if (size[next] <= size[i])
				spookyT2[next]++;
			nb--;
		}
		nb = size[i];
		next = i;
		while (nb > 0)
		{
			next = neigh[next][(sunDirection + 3) % 6];
			if (next < 0)
				break;
			if (size[next] <= size[i])
				spookyT3[next]++;
			nb--;
		}
		nb = size[i];
		next = i;
		while (nb > 0)
		{
			next = neigh[next][(sunDirection + 4) % 6];
			if (next < 0)
				break;
			if (size[next] <= size[i])
				spookyT4[next]++;
			nb--;
		}
		nb = size[i];
		next = i;
		while (nb > 0)
		{
			next = neigh[next][(sunDirection + 5) % 6];
			if (next < 0)
				break;
			if (size[next] <= size[i])
				spookyT5[next]++;
			nb--;
		}
		spookyMoy[i] = (float)((spooky[i] > 0) + (spookyT1[i] > 0) + (spookyT2[i] > 0) + (spookyT3[i] > 0) + (spookyT4[i] > 0) + (spookyT5[i] > 0)) / 6.0;
	}
};

void    Game::calculateReachable()
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

void    Game::numberNeigh(int i)
{
	int neighbour;

	for (int j = 0; j < 6; j++)
	{
		neighbour = neigh[i][j];
		if (neighbour >= 0)
		{
			if (owner[neighbour] == 1)
				oppNeighs[i]++;
			else if (owner[neighbour] == 2)
				allyNeighs[i]++;
            totNeighs[i]++;
		}
	}
}

float   Game::sizeFactor(int i)
{
	float   size_f;

    if (size[i] < 3 || allySize3 >= maxTreeSize3)
		size_f = size[i] + day;
    else
		size_f = 1;
	return (size[i] + 3);
}

float   Game::spotFactor(int i)
{
	float   alone_f;
	float   aggressive;
	float   diag_f;
	float   spot_f;
	float	spooky_f;

	alone_f = (float)(totNeighs[i] - oppNeighs[i] - allyNeighs[i]) / (float)totNeighs[i];
	diag_f = 7.0 / (1 + numberOfDiag[i]);
    aloneImportance = 10 * (37 - boardTrees - boardRich0) / (37 - boardRich0);
    spot_f = (aloneImportance * alone_f + richnessImportance * richness[i]);// * diag_f;
	return (spot_f);
}

float	Game::sunFactor(int i)
{
	return ((1 - spookyMoy[i]) * (size[i] + 2));
}

float   Game::costFactor(int i)
{
	float cost_f;

	cost_f = cost[i] / (size[i] + 2) + 1;
	return (cost_f);
}

void    Game::calculateCost()
{
	for (int i = 0; i < cells; i++)
		cost[i] = costOf(i, ALLY);
}

void    Game::calculateScore()
{
	float max;

	max = 0.0;
	for (int i = 0; i < cells; i++)
	{
        numberNeigh(i);
		if ((owner[i] == 2 && dormant[i] == 0 && day <= 20 + size[i])
				|| (owner[i] == 0 && reachable[i] && richness[i] > 0))
			score[i] = spotFactor(i) * sizeFactor(i) * costFactor(i) * sunFactor(i);
		else
			score[i] = 0.0;
		if (max < score[i])
			max = score[i];
	} 
	maxMoveScore = max;
}

void	Game::calculateAll()
{
	getRichnessImportance();
	getSunPerTurn();
	fillDiag();
	calculateSpooky();
	calculateReachable();
	calculateCost();
	calculateScore();
	sortOnScore();
	action();
}

void	Game::initAll()
{
	boardRich0 = 0;
	boardRich1 = 0;
	boardRich2 = 0;
	boardRich3 = 0;
	gameTurns = 0;
	day = 0;
}

// -----------------------------
//        Scan game info
// -----------------------------

void	Game::fillDistance(int index)
{
	for (int i = 0; i < 6; i++)
	{
		if (neigh[index][i] >= 0)
			dist[index][neigh[index][i]] = 1;
	}
}

void	Game::scanGrid()
{
	int rich;
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
		cin >> index >> rich >> neigh0 >> neigh1 >> neigh2 >> neigh3 >> neigh4 >> neigh5; cin.ignore();
		richness[index] = rich;
		if (rich == 0)
			boardRich0++;
		else if (rich == 1)
			boardRich1++;
		else if (rich == 2)
			boardRich2++;
		else
			boardRich3++;
		neigh[index][0] = neigh0;
		neigh[index][1] = neigh1;
		neigh[index][2] = neigh2;
		neigh[index][3] = neigh3;
		neigh[index][4] = neigh4;
		neigh[index][5] = neigh5;
	}
}

void	Game::scanInfo()
{
	int opp_sun;
	int opp_score;
	int opp_is_waiting;
	int number_of_trees;

	cin >> day; cin.ignore();
	cin >> nutrients; cin.ignore();
	cin >> allySun >> allyScore; cin.ignore();
	cin >> oppSun >> oppScore >> oppIsWaiting; cin.ignore();
	cin >> boardTrees; cin.ignore();
}

void    Game::initBoard()
{
	for (int i = 0 ; i < cells; i++)
	{
		owner[i] = 0;
		size[i] = -1;
		dormant[i] = 0;
		spooky[i] = 0;
		reachable[i] = 0;
		score[i] = 0;
		allyNeighs[i] = 0;
		oppNeighs[i] = 0;
		cost[i] = 0;
	}
	allySize0 = 0;
	allySize1 = 0;
	allySize2 = 0;
	allySize3 = 0;
    allyExternSize3 = 0;
	oppSize0 = 0;
	oppSize1 = 0;
	oppSize2 = 0;
	oppSize3 = 0;
	aloneImportance = 4;
	stopSeeding = 18;
	maxTreeSize3 = 4; //5 - 2 * (day >= 19);
	actions.clear();
}

void	Game::scanTrees()
{
	int cell_index;
	int size_tree;
	int is_mine;
	int is_dormant;

	for (int i = 0; i < boardTrees; i++) {
		cin >> cell_index >> size_tree >> is_mine >> is_dormant; cin.ignore();
		if (cell_index < cells && cell_index >= 0)
		{
			owner[cell_index] = 1 + is_mine;
			size[cell_index] = size_tree;
			if (owner[cell_index] == 2)
			{
				if (size_tree == 0)
					allySize0++;
				else if (size_tree == 1)
					allySize1++;
				else if (size_tree == 2)
					allySize2++;
				else if (size_tree == 3)
                {
					allySize3++;
                    if (cell_index >= 19)
                        allyExternSize3++;
                }
			}
			else if (owner[cell_index] == 1)
			{
				if (size_tree == 0)
					oppSize0++;
				else if (size_tree == 1)
					oppSize1++;
				else if (size_tree == 2)
					oppSize2++;
				else if (size_tree == 3)
					oppSize3++;
			}
			dormant[cell_index] = is_dormant;
		}
	}
	allyTrees = allySize0 + allySize1 + allySize2 + allySize3;
	oppTrees = oppSize0 + oppSize1 + oppSize2 + oppSize3;
}

void	Game::scanMoves()
{
	cin >> numberOfActions; cin.ignore();
	for (int i = 0; i < numberOfActions; i++) {
		string possibleAction;
		getline(cin, possibleAction);
		actions.push_back(possibleAction);
	}
}

void    Game::fillDist()
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

void	printAction(int action[3])
{
	if (action[0] == WAIT)
		std::cout << "WAIT zZZzzZzzz" << endl;
	else if (action[0] == SEED)
		std::cout << "SEED " << action[1] << " " << action[2] << " welcome" << endl;
	else if (action[0] == GROW)
		std::cout << "GROW " << action[1] << " be strong" << endl;
	else
		std::cout << "COMPLETE " << action[1] << " jackpot!" << endl;
}

/*
void	play()
{
	Game	game;

	game.initAll();
	game.scanGrid();
	game.fillDist();
	while (1) {
		game.scanInfo();
		game.initBoard();
		game.scanTrees();
		game.scanMoves();
		//game.printInput();
		game.calculateAll();
		printAction(game.allyDo);
	}
	return ;
}

int	main()
{
	play();
	return (0);
}
*/