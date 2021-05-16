#include "bot.cpp"
#include <sstream>
#include <cstring>
#include <chrono>
#include <thread>
#include <algorithm>
using std::cerr;
using std::cin;
using std::endl;
using std::this_thread::sleep_for;
using namespace std;
constexpr int TIME_TO_SLEEP = 1000;

vector<int> nums{25,24,23,22,26,11,10,9,21,27,12,3,2,8,20,28,13,4,0,1,7,19,29,14,5,6,18,36,30,15,16,17,35,31,32,33,34};
vector<int> first{0,4,9,15,22,28,33,37};
vector<int> width{4,5,6,7,6,5,4};
vector<int> partOne{3,2,1,12,11,10, 9, 8, 7,26,25,24,23,22,21,20,19,36};
vector<int> partTwo{6,5,4,18,17,16,15,14,13,35,34,33,32,31,30,29,28,27};
vector<string> grids{"grids/grid0", "grids/grid1", "grids/grid2", "grids/grid3", "grids/grid4", "grids/grid5", "grids/grid6", "grids/grid7", "grids/grid8", "grids/grid9", "grids/grid10"};

class Stat
{
	public :
		int	winner;
		int allyScore;
		int oppScore;
};

// -----------------------------
//    	 drawing functions
// -----------------------------

void	drawLine()
{
	cerr << endl << "-----------------------------------------------------------" << endl << endl;
}

void	Game::drawBoard()
{
	int		space;
	int		cell;
	int		n;

	cerr << "[DAY " << day << "]   " << "\033[31msun(" << allySun << ") score(" << allyScore << ")";
	cerr << "   " << "\033[34msun(" << oppSun << ") score(" << oppScore << ")";
	cerr << "\033[0m" << endl;
	n = 3;
	n *= 2;
	cerr << endl << endl;
	for (int j = 0; j < 7; j++)
	{
		space = (7 - width[j]) * (2 + n) / 2 + 4;
		cerr << setw(space) << " ";
		for (int i = first[j]; i < first[j + 1]; i++)
		{
			cell = nums[i];
			if (owner[cell] == ALLY && spooky[cell])
				cerr << "\033[31m";
			else if (owner[cell] == OPPONENT && spooky[cell])
				cerr << "\033[34m";
			else if (owner[cell] == ALLY)
				cerr << "\033[31;1m";
			else if (owner[cell] == OPPONENT)
				cerr << "\033[34;1m";
			else if (richness[cell] == 0)
				cerr << "\033[90;1m" ;
			else if (spooky[cell])
				cerr << "\033[90m";
			if (size[cell] < 0)
				cerr << setw(2) << cell << "\033[0m" << setw(n) << " " ;
			else
				cerr << setw(2) << cell << "\033[0m\033[90m[" << size[cell] << "]" << setw(n - 3) << " " << "\033[0m";
		}
		if (j < 6)
		{
			for (int k = n / 2 - 1; k >= 0; k--)
				cerr << endl;
		}
	}
	cerr << endl << endl << endl;
}

// -----------------------------
//    	  scan from file
// -----------------------------

void	Game::scanGridStream(ifstream& file)
{
	int rich;
	int index;
	int neigh0;
	int neigh1;
	int neigh2;
	int neigh3;
	int neigh4;
	int neigh5;

	file >> cells; file.ignore();
	for (int i = 0; i < cells; i++)
	{
		for (int j = 0; j < cells; j++)
			dist[i][j] = 0;
	}
	for (int i = 0; i < cells; i++)
	{
		file >> index >> rich >> neigh0 >> neigh1 >> neigh2 >> neigh3 >> neigh4 >> neigh5; file.ignore();
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

void	Game::scanInfoStream(ifstream& file)
{
	int opp_sun;
	int opp_score;
	int opp_is_waiting;
	int number_of_trees;

	file >> day; file.ignore();
	file >> nutrients; file.ignore();
	file >> allySun >> allyScore; file.ignore();
	file >> oppSun >> oppScore >> oppIsWaiting; file.ignore();
	file >> boardTrees; file.ignore();
}

void	Game::scanTreesStream(ifstream& file)
{
	int cell_index;
	int size_tree;
	int is_mine;
	int is_dormant;

	for (int i = 0; i < boardTrees; i++) {
		file >> cell_index >> size_tree >> is_mine >> is_dormant; file.ignore();
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
					allySize3++;
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

void	Game::scanMovesStream(ifstream& file)
{
	file >> numberOfActions; file.ignore();
	for (int i = 0; i < numberOfActions; i++) {
		string possibleAction;
		getline(file, possibleAction);
		actions.push_back(possibleAction);
	}
}


// -----------------------------
//   determine opponent's move
// -----------------------------

void	Game::calculateAllActions()
{
	oppActionsVect.clear();
	allyActionsVect.clear();
	oppActionsVect.push_back(WAIT);
	oppActionsVect.push_back(0);
	oppActionsVect.push_back(0);
	allyActionsVect.push_back(WAIT);
	allyActionsVect.push_back(0);
	allyActionsVect.push_back(0);
	for (int i = 0; i < cells; i++)
	{
		if (owner[i] == ALLY)		// calculate all the moves for an ally tree
		{
			if (size[i] == 0)
			{
				if (costOf(i, ALLY) <= allySun)
				{
					allyActionsVect.push_back(GROW);
					allyActionsVect.push_back(i);
					allyActionsVect.push_back(0);
				}
			}
			else if (size[i] < 3)
			{
				if (costOf(i, ALLY) <= allySun)
				{
					allyActionsVect.push_back(GROW);
					allyActionsVect.push_back(i);
					allyActionsVect.push_back(0);
				}
				for (int k = 0; k < cells; k++)
				{
					if (dist[k][i] <= size[i] && owner[k] == 0 && richness[k] > 0 && costOf(k, ALLY) <= allySun)
					{
						allyActionsVect.push_back(SEED);
						allyActionsVect.push_back(i);
						allyActionsVect.push_back(k);
					}
				}
			}
			else
			{
				if (costOf(i, ALLY) <= allySun)
				{
					allyActionsVect.push_back(COMPLETE);
					allyActionsVect.push_back(i);
					allyActionsVect.push_back(0);
				}
				for (int k = 0; k < cells; k++)
				{
					if (dist[k][i] <= size[i] && owner[k] == 0 && richness[k] > 0 && costOf(k, ALLY) <= allySun)
					{
						allyActionsVect.push_back(SEED);
						allyActionsVect.push_back(i);
						allyActionsVect.push_back(k);
					}
				}
			}
		}
		else if (owner[i] == OPPONENT)
		{
			if (size[i] == 0)
			{
				if (costOf(i, OPPONENT) <= oppSun)
				{
					oppActionsVect.push_back(GROW);
					oppActionsVect.push_back(i);
					oppActionsVect.push_back(0);
				}
			}
			else if (size[i] < 3)
			{
				if (costOf(i, OPPONENT) <= oppSun)
				{
					oppActionsVect.push_back(GROW);
					oppActionsVect.push_back(i);
					oppActionsVect.push_back(0);
				}
				for (int k = 0; k < cells; k++)
				{
					if (dist[k][i] <= size[i] && owner[k] == 0 && richness[k] > 0 && costOf(k, OPPONENT) <= oppSun)
					{
						oppActionsVect.push_back(SEED);
						oppActionsVect.push_back(i);
						oppActionsVect.push_back(k);
					}
				}
			}
			else
			{
				if (costOf(i, OPPONENT) <= oppSun)
				{
					oppActionsVect.push_back(COMPLETE);
					oppActionsVect.push_back(i);
					oppActionsVect.push_back(0);
				}
				for (int k = 0; k < cells; k++)
				{
					if (dist[k][i] <= size[i] && owner[k] == 0 && richness[k] > 0 && costOf(k, OPPONENT) <= oppSun)
					{
						oppActionsVect.push_back(SEED);
						oppActionsVect.push_back(i);
						oppActionsVect.push_back(k);
					}
				}
			}
		}
	}
}

void	Game::randomOpponentMove(int seed)
{
	int				i;

	srand(seed);
	i = rand() % (oppActionsVect.size() / 3);
	oppDo[0] = oppActionsVect[3 * i + 0];
	oppDo[1] = oppActionsVect[3 * i + 1];
	oppDo[2] = oppActionsVect[3 * i + 2];
}

// -----------------------------
// apply both moves on the board
// -----------------------------

void	printSunCollect(int allyCollect, int oppCollect)
{
	cerr << " \033[41mllebriq\033[0m has collected " << allyCollect << " sun points" << endl;
	cerr << " \033[44mopponent\033[0m has collected " << oppCollect << " sun points" << endl;
	drawLine();
}

void	Game::updateBoard()
{
	if (allyDo[0] != WAIT && oppDo[0] != WAIT && allyDo[1] == oppDo[1])
	{
		if (allyDo[0] == SEED)
			dormant[allyDo[2]] == 1;
		if (oppDo[0] == SEED)
			dormant[oppDo[2]] == 1;
		return ;
	}

	// both waiting -> new day and collect of sun points
	if (allyDo[0] == WAIT && oppDo[0] == WAIT)
	{
		int	allyCollect;
		int	oppCollect;

		allyCollect = 0;
		oppCollect = 0;
		for (int i = 0; i < cells; i++)
		{
			if (spooky[i] == 0 && owner[i] == ALLY)
				allyCollect += size[i];
			else if (spooky[i] == 0 && owner[i] == OPPONENT)
				oppCollect += size[i];
			dormant[i] = 0;
		}
		allySun += allyCollect;
		oppSun += oppCollect;
		//printSunCollect(allyCollect, oppCollect);
		day++;
	}

	// ally action
	if (allyDo[0] != WAIT)
	{
		if (allyDo[0] == GROW)
		{
			allySun -= costOf(allyDo[1], ALLY);
			if (size[allyDo[1]] == 0)
			{
				allySize0--;
				allySize1++;
			}
			else if (size[allyDo[1]] == 1)
			{
				allySize1--;
				allySize2++;
			}
			else if (size[allyDo[1]] == 2)
			{
				allySize2--;
				allySize3++;
			}
			size[allyDo[1]]++;
			dormant[allyDo[1]] = 1;
		}
		else if (allyDo[0] == SEED)
		{
			allySun -= costOf(allyDo[2], ALLY);
			owner[allyDo[2]] = ALLY;
			size[allyDo[2]] = 0;
			allySize0++;
			dormant[allyDo[1]] = 1;
			dormant[allyDo[2]] = 1;
		}
		else
		{
			allySun -= 4;
			allyScore += nutrients + 2 * (richness[allyDo[1]] - 1);
			owner[allyDo[1]] = 0;
			size[allyDo[1]] = -1;
			allySize3--;
			nutrients--;
		}
	}

	// opponent action
	if (oppDo[0] != WAIT)
	{
		if (oppDo[0] == GROW)
		{
			oppSun -= costOf(oppDo[1], OPPONENT);
			if (size[oppDo[1]] == 0)
			{
				oppSize0--;
				oppSize1++;
			}
			else if (size[oppDo[1]] == 1)
			{
				oppSize1--;
				oppSize2++;
			}
			else if (size[oppDo[1]] == 2)
			{
				oppSize2--;
				oppSize3++;
			}
			size[oppDo[1]]++;
			dormant[oppDo[1]] = 1;
		}
		else if (oppDo[0] == SEED)
		{
			oppSun -= costOf(oppDo[2], OPPONENT);
			owner[oppDo[2]] = OPPONENT;
			size[oppDo[2]] = 0;
			oppSize0++;
			dormant[oppDo[1]] = 1;
			dormant[oppDo[2]] = 1;
		}
		else
		{
			oppSun -= 4;
			oppScore += nutrients + 2 * (richness[allyDo[1]] - 1);
			owner[oppDo[1]] = 0;
			size[oppDo[1]] = -1;
			oppSize3--;
			nutrients--;
		}
	}
}

void	printFinalScore(int allyScore, int oppScore, int allyTrees, int oppTrees, int winner)
{
	cerr << "RESULT" << endl;
	if (winner == ALLY)
	{
		cerr << "\033[31;1m1st \033[0m\033[41mllebriq\033[0m   " << allyScore << "pts" << endl;
		if (allyScore == oppScore)
			cerr << " and " << allyTrees << " trees" << endl;
		cerr << "\033[34;1m2nd \033[0m\033[44mopponent\033[0m  " << oppScore << "pts" << endl;
		if (allyScore == oppScore)
			cerr << " and " << oppTrees << " trees" << endl;
		return ;
	}
	if (winner == OPPONENT)
	{
		cerr << "\033[34;1m1st \033[0m\033[44mopponent\033[0m  " << oppScore << "pts" << endl;
		if (allyScore == oppScore)
			cerr << " and " << oppTrees << " trees" << endl;
		cerr << "\033[31;1m2nd \033[0m\033[41mllebriq\033[0m   " << allyScore << "pts" << endl;
		if (allyScore == oppScore)
			cerr << " and " << allyTrees << " trees" << endl;
		return ;
	}
	cerr << "\033[31;1m1st \033[0m\033[41mllebriq\033[0m   " << allyScore << "pts and " << allyTrees << " trees" << endl;
	cerr << "\033[34;1m1st \033[0m\033[44mopponent\033[0m  " << oppScore << "pts and " << oppTrees << " trees" << endl;
}

void	Game::getFinalScore(int print, int seed)
{
	allyScore = allyScore + (allySun / 3);
	oppScore = oppScore + (oppSun / 3);
	winner = DRAW;
	if (allyScore > oppScore || (allyScore == oppScore && allyTrees > oppTrees))
		winner = ALLY;
	else if (oppScore < allyScore || (allyScore == oppScore && allyTrees < oppTrees))
		winner = OPPONENT;
	if (print > 1)
		printFinalScore(allyScore, oppScore, allyTrees, oppTrees, winner);
	else if (print == 1)
		cerr << "\tRESULT " << allyScore << " vs " << oppScore << "  \tseed " << seed << endl ;
}

void	printActionColor(int action[3], int player)
{
	cerr << " ";
	if (player == ALLY)
		cerr << "\033[41mllebriq";
	else
		cerr << "\033[44mopponent";
	cerr << "\033[0m ";
	if (action[0] == WAIT)
		cerr << "is waiting";
	else if (action[0] == SEED)
		cerr << "is planting a seed on cell " << action[2] << " from cell " << action[1];
	else if (action[0] == GROW)
		cerr << "is growing a tree on cell " << action[1];
	else
		cerr << "is ending their tree life on cell " << action[1];
	cerr << endl;
}

Stat	playAndDraw(int end, int print, int seed, int gridNumber)
{
	ifstream file;
	file.open(grids[gridNumber], ios::in);
	Game	game;
	Stat	stat;

	game.initAll();
	game.scanGridStream(file);
	game.fillDist();
	if (print)
		cerr << endl;
	while (game.day < end) {
		if (game.gameTurns == 0)
		{
			game.scanInfoStream(file);
			game.initBoard();
			game.scanTreesStream(file);
			game.scanMovesStream(file);
			file.close();
			game.stopSeeding = 18;
		}
		game.maxTreeSize3 = 5 - 2 * (game.day >= 19);
		game.calculateAll();
		if (print > 3 || print > 2 && game.gameTurns % 5 == 0)
			game.drawBoard();
		game.calculateAllActions();
		game.randomOpponentMove(seed);
		if (print > 3)
			game.printScoreOfBoard();
		if (print > 1)
		{
			printActionColor(game.allyDo, ALLY);
			printActionColor(game.oppDo, OPPONENT);
			drawLine();
		}
		game.gameTurns++;
		game.updateBoard();
		if (print > 3)
			getchar();
	}
	game.getFinalScore(print, seed);
	stat.winner = game.winner;
	stat.allyScore = game.allyScore;
	stat.oppScore = game.oppScore;
	return (stat);
}

int	main(int argc, char **argv)
{
	int		seed;
	Stat	stat;
	int		nbSimu;
	int		nbWin;
	int		moyAlly;
	int		moyOpp;
	int		moyTotAlly;
	int		moyTotOpp;
	int		nbTotWin;
	int		minGrid;
	int		maxGrid;
	int		endDay;
	int		print;

	minGrid = 0;
	maxGrid = 1;
	nbTotWin = 0;
	moyTotAlly = 0;
	moyTotOpp = 0;
	nbSimu = atoi(argv[1]);
	endDay = atoi(argv[2]);
	print = atoi(argv[3]);
	srand(time(NULL));
	for (int gridNumber = minGrid; gridNumber < maxGrid; gridNumber++)
	{
		nbWin = 0;
		moyAlly = 0;
		moyOpp = 0;
		for (int i = 0; i < nbSimu; i++)
		{
			seed = rand();
			stat = playAndDraw(24, print, seed, gridNumber);
			nbWin += (stat.winner == ALLY);
			moyAlly += stat.allyScore;
			moyOpp += stat.oppScore;
			sleep_for(std::chrono::milliseconds(TIME_TO_SLEEP));
		}
		nbTotWin += nbWin;
		moyTotAlly += moyAlly;
		moyTotOpp += moyOpp;
		cerr << endl << "Grid " << gridNumber << endl ;
		cerr << "Pourcentage de victoires\t" << (int)((double)nbWin / (double)nbSimu * 100.0) << "%" << endl;
		cerr << "Moyenne du score allié  \t" << (int)((double)moyAlly / (double)nbSimu) << "pts" << endl;
		cerr << "Moyenne du score ennemi  \t" << (int)((double)moyOpp / (double)nbSimu) << "pts" << endl;
	}
	cerr << endl << "TOTAL " << endl ;
	cerr << "Pourcentage de victoires\t" << (int)((double)nbTotWin / (double)(nbSimu * (maxGrid - minGrid)) * 100.0) << "%" << endl;
	cerr << "Moyenne du score allié  \t" << (int)((double)moyTotAlly / (double)(nbSimu * (maxGrid - minGrid))) << "pts" << endl;
	cerr << "Moyenne du score ennemi  \t" << (int)((double)moyTotOpp / (double)(nbSimu * (maxGrid - minGrid))) << "pts" << endl;
	return (0);
}