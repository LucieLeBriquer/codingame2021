#include "bot.cpp"
#define ALLY 2
#define OPPONENT 1
#define	DRAW 0
#include <sstream>
#include <cstring>

using namespace std;
vector<int> nums{25,24,23,22,26,11,10,9,21,27,12,3,2,8,20,28,13,4,0,1,7,19,29,14,5,6,18,36,30,15,16,17,35,31,32,33,34};
vector<int> first{0,4,9,15,22,28,33,37};
vector<int> width{4,5,6,7,6,5,4};

void	drawLine()
{
	cout << endl << "-----------------------------------------------------------" << endl << endl;
}

void	Game::drawBoard()
{
	int		space;
	int		cell;
	int		n;

	cout << "[DAY " << day << "]   " << "\033[31msun(" << allySun << ") score(" << allyScore << ")";
	cout << "   " << "\033[34msun(" << oppSun << ") score(" << oppScore << ")";
	cout << "\033[0m" << endl;
	n = 3;
	n *= 2;
	cout << endl << endl;
	for (int j = 0; j < 7; j++)
	{
		space = (7 - width[j]) * (2 + n) / 2 + 4;
		cout << setw(space) << " ";
		for (int i = first[j]; i < first[j + 1]; i++)
		{
			cell = nums[i];
			if (owner[cell] == ALLY && spooky[cell])
				cout << "\033[31m";
			else if (owner[cell] == OPPONENT && spooky[cell])
				cout << "\033[34m";
			else if (owner[cell] == ALLY)
				cout << "\033[31;1m";
			else if (owner[cell] == OPPONENT)
				cout << "\033[34;1m";
			else if (richness[cell] == 0)
				cout << "\033[90;1m" ;
			else if (spooky[cell])
				cout << "\033[90m";
			if (size[cell] < 0)
				cout << setw(2) << cell << "\033[0m" << setw(n) << " " ;
			else
				cout << setw(2) << cell << "\033[0m\033[90m[" << size[cell] << "]" << setw(n - 3) << " " << "\033[0m";
		}
		if (j < 6)
		{
			for (int k = n / 2 - 1; k >= 0; k--)
				cout << endl;
		}
	}
	cout << endl << endl << endl;
}

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
	stringstream	newAction;

	allyActions.clear();
	oppActions.clear();
	allyActions.push_back("WAIT");
	oppActions.push_back("WAIT");
	newAction.clear();
	newAction.str("");
	for (int i = 0; i < cells; i++)
	{
		if (owner[i] == ALLY)		// calculate all the moves for an ally tree
		{
			if (size[i] == 0)
			{
				if (costOf(i) <= allySun)
				{
					newAction << "GROW " << i;
					allyActions.push_back(newAction.str()); newAction.clear(); newAction.str("");
				}
			}
			else if (size[i] < 3)
			{
				if (costOf(i) <= allySun)
				{
					newAction << "GROW " << i;
					allyActions.push_back(newAction.str()); newAction.clear(); newAction.str("");
				}
				for (int k = 0; k < cells; k++)
				{
					if (dist[k][i] <= size[i] && owner[k] == 0 && richness[k] > 0 && costOf(k) <= allySun)
					{
						newAction << "SEED " << i << " " << k;
						allyActions.push_back(newAction.str()); newAction.clear(); newAction.str("");
					}
				}
			}
			else
			{
				if (costOf(i) <= allySun)
				{
					newAction << "COMPLETE " << i;
					allyActions.push_back(newAction.str()); newAction.clear(); newAction.str("");
				}
				for (int k = 0; k < cells; k++)
				{
					if (dist[k][i] <= size[i] && owner[k] == 0 && richness[k] > 0 && costOf(k) <= allySun)
					{
						newAction << "SEED " << i << " " << k;
						allyActions.push_back(newAction.str()); newAction.clear(); newAction.str("");
					}
				}
			}
		}
		else if (owner[i] == OPPONENT)
		{
			if (size[i] == 0)
			{
				if (costOf(i) <= oppSun)
				{
					newAction << "GROW " << i;
					oppActions.push_back(newAction.str()); newAction.clear(); newAction.str("");
				}
			}
			else if (size[i] < 3)
			{
				if (costOf(i) <= oppSun)
				{
					newAction << "GROW " << i;
					oppActions.push_back(newAction.str()); newAction.clear(); newAction.str("");
				}
				for (int k = 0; k < cells; k++)
				{
					if (dist[k][i] <= size[i] && owner[k] == 0 && richness[k] > 0 && costOf(k) <= oppSun)
					{
						newAction << "SEED " << i << " " << k;
						oppActions.push_back(newAction.str()); newAction.clear(); newAction.str("");
					}
				}
			}
			else
			{
				if (costOf(i) <= oppSun)
				{
					newAction << "COMPLETE " << i;
					oppActions.push_back(newAction.str()); newAction.clear(); newAction.str("");
				}
				for (int k = 0; k < cells; k++)
				{
					if (dist[k][i] <= size[i] && owner[k] == 0 && richness[k] > 0 && costOf(k) <= oppSun)
					{
						newAction << "SEED " << i << " " << k;
						oppActions.push_back(newAction.str()); newAction.clear(); newAction.str("");
					}
				}
			}
		}
	}
}

void	Game::randomOpponentMove()
{
	int				i;
	int				k;
	string			oppAction;;
	string			words[3];
    string			token;
    stringstream	ss(oppAction);

	i = rand() % oppActions.size();
	oppAction = oppActions[i];
	k = 0;
    while (getline(ss, token, ' ')) {
        words[k % 3]= token;
		k++;
    }
	if (words[0] == "SEED")
	{
		oppDo[0] = SEED;
		oppDo[1] = stoi(words[1]);
		oppDo[2] = stoi(words[2]);
	}
	else if (words[0] == "COMPLETE")
	{
		oppDo[0] = COMPLETE;
		oppDo[1] = stoi(words[1]);
	}
	else if (words[0] == "WAIT")
		oppDo[0] = WAIT;
	else if (words[0] == "GROW")
	{
		oppDo[0] = GROW;
		oppDo[1] = stoi(words[1]);
	}
}

// -----------------------------
// apply both moves on the board
// -----------------------------

void	printSunCollect(int allyCollect, int oppCollect)
{
	cout << " \033[41mllebriq\033[0m has collected " << allyCollect << " sun points" << endl;
	cout << " \033[44mopponent\033[0m has collected " << oppCollect << " sun points" << endl;
	drawLine();
}

void	Game::updateBoard()
{
	if (allyDo[0] != WAIT && oppDo[0] != WAIT && allyDo[1] == oppDo[1])
		return ;

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
			allySun -= costOf(allyDo[1]);
			size[allyDo[1]]++;
			dormant[allyDo[1]] = 1;
		}
		else if (allyDo[0] == SEED)
		{
			allySun -= costOf(allyDo[2]);
			owner[allyDo[2]] = ALLY;
			size[allyDo[2]] = 0;
			dormant[allyDo[1]] = 1;
			dormant[allyDo[2]] = 1;
		}
		else
		{
			allySun -= 4;
			allyScore += nutrients + 2 * (richness[allyDo[1]] - 1);
			owner[allyDo[1]] = 0;
			size[allyDo[1]] = -1;
			nutrients--;
		}
	}

	// opponent action
	if (oppDo[0] != WAIT)
	{
		if (oppDo[0] == GROW)
		{
			oppSun -= costOf(oppDo[1]);
			size[oppDo[1]]++;
			dormant[oppDo[1]] = 1;
		}
		else if (oppDo[0] == SEED)
		{
			oppSun -= costOf(oppDo[2]);
			owner[oppDo[2]] = OPPONENT;
			dormant[oppDo[1]] = 1;
			dormant[oppDo[2]] = 1;
		}
		else
		{
			oppSun -= 4;
			oppScore += nutrients + 2 * (richness[allyDo[1]] - 1);
			owner[oppDo[1]] = 0;
			size[oppDo[1]] = -1;
			nutrients--;
		}
	}
}

void	printFinalScore(int allyScore, int oppScore, int allyTrees, int oppTrees, int winner)
{
	cout << "RESULT" << endl;
	if (winner == ALLY)
	{
		cout << "\033[31;1m1st \033[0m\033[41mllebriq\033[0m   " << allyScore << "pts" << endl;
		if (allyScore == oppScore)
			cout << " and " << allyTrees << " trees" << endl;
		cout << "\033[34;1m2nd \033[0m\033[44mopponent\033[0m  " << oppScore << "pts" << endl;
		if (allyScore == oppScore)
			cout << " and " << oppTrees << " trees" << endl;
		return ;
	}
	if (winner == OPPONENT)
	{
		cout << "\033[34;1m1st \033[0m\033[44mopponent\033[0m  " << oppScore << "pts" << endl;
		if (allyScore == oppScore)
			cout << " and " << oppTrees << " trees" << endl;
		cout << "\033[31;1m2nd \033[0m\033[41mllebriq\033[0m   " << allyScore << "pts" << endl;
		if (allyScore == oppScore)
			cout << " and " << allyTrees << " trees" << endl;
		return ;
	}
	cout << "\033[31;1m1st \033[0m\033[41mllebriq\033[0m   " << allyScore << "pts and " << allyTrees << " trees" << endl;
	cout << "\033[34;1m1st \033[0m\033[44mopponent\033[0m  " << oppScore << "pts and " << oppTrees << " trees" << endl;
}

void	Game::getFinalScore(int print)
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
		cout << "RESULT " << allyScore << " vs " << oppScore << endl ;
}

void	printActionColor(int action[3], int player)
{
	cout << " ";
	if (player == ALLY)
		cout << "\033[41mllebriq";
	else
		cout << "\033[44mopponent";
	cout << "\033[0m ";
	if (action[0] == WAIT)
		cout << "is waiting";
	else if (action[0] == SEED)
		cout << "is planting a seed on cell " << action[2] << " from cell " << action[1];
	else if (action[0] == GROW)
		cout << "is growing a tree on cell " << action[1];
	else
		cout << "is ending their tree life on cell " << action[1];
	cout << endl;
}

void	printActions(Game game)
{
	cout << "ACTIONS" << endl << endl;
	cout << "--> ally" << endl;
	for (int i = 0; i < game.allyActions.size(); i++)
		cout << game.allyActions[i] << endl;
	cout << endl << "--> opponent" << endl;
	for (int i = 0; i < game.oppActions.size(); i++)
		cout << game.oppActions[i] << endl;
	cout << "number of actions : " << game.oppActions.size() + game.allyActions.size() << endl << endl;

	for (int i = 0; i < game.actions.size(); i++)
		cout << game.actions[i] << endl;
}

int		playAndDraw(int end, int print, int stopSeeding, float r)
{
	ifstream file;
	file.open("start", ios::in);
	Game	game;

	game.boardRich0 = 0;
	game.boardRich1 = 0;
	game.boardRich2 = 0;
	game.boardRich3 = 0;
	game.gameTurns = 0;
	game.day = 0;
	game.scanGridStream(file);
	game.fillDist();
	if (print)
		cout << endl;
	while (game.day < end) {
		if (game.day == 0)
		{
			game.scanInfoStream(file);
			game.initBoard();
			game.scanTreesStream(file);
			game.scanMovesStream(file);
			file.close();
			// fix variables
			game.stopSeeding = stopSeeding;
		}
		game.calculateAllActions();
		game.maxTreeSize3 = 4 - (game.day > 19) - (game.day > 21);

		game.fillDiag();
		game.calculateSpooky();
		game.calculateReachable();
		game.calculateCost();
		game.calculateScore();
		game.sortOnScore();
		game.richnessImportance = r * sqrt((double)game.boardRich0);
		if (game.richnessImportance == 0)
			game.richnessImportance = r;

		if (print > 1 && game.gameTurns % 5 == 0)
			game.drawBoard();
		game.action();
		game.oppDo[0] = WAIT;
		game.randomOpponentMove();
		if (print)
		{
			printActionColor(game.allyDo, ALLY);
			printActionColor(game.oppDo, OPPONENT);
			drawLine();
		}
		game.gameTurns++;
		game.updateBoard();
	}
	file.close();
	game.getFinalScore(print);
	return (game.winner);
}

int	main(void)
{
	int	res;

	res = playAndDraw(24, 1, 18, 10.0);
	return (0);
}