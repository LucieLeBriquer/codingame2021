#include "../bot.cpp"
#define ALLY 2
#define OPPONENT 1

vector<int> nums{25,24,23,22,26,11,10,9,21,27,12,3,2,8,20,28,13,4,0,1,7,19,29,14,5,6,18,36,30,15,16,17,35,31,32,33,34};
vector<int> first{0,4,9,15,22,28,33,37};
vector<int> width{4,5,6,7,6,5,4};

void	Game::drawBoard()
{
	int		space;
	int		cell;
	int		n;


	cout << "[DAY " << setw(2) << day << "]   " << "\033[31m ally sun(" << allySun << ") score(" << allyScore << ")";
	cout << "   " << "\033[34m opp. sun(" << allySun << ") score(" << allyScore << ")";
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
			if (owner[cell] == ALLY)
				cout << "\033[31m";
			else if (owner[cell] == OPPONENT)
				cout << "\033[34m";
			if (size[cell] < 0)
				cout << setw(2) << cell << setw(n) << " " << "\033[0m";
			else
				cout << setw(2) << cell << "\033[90m[" << size[cell] << "]" << setw(n - 3) << " " << "\033[0m";
		}
		if (j < 6)
		{
			for (int k = n / 2; k >= 0; k--)
				cout << endl;
		}
	}
	cout << endl << endl << endl;
}

// apply both moves on the Board
void	Game::updateBoard()
{
	if (allyDo[0] != WAIT && oppDo[0] != WAIT && allyDo[1] == oppDo[1])
		return ;

	// both waiting -> new day and collect of sun points
	if (allyDo[0] == WAIT && oppDo[0] == WAIT)
	{
		for (int i = 0; i < cells; i++)
		{
			if (spooky[i] == 0 && owner[i] == ALLY)
				allySun += size[i];
			else if (spooky[i] == 0 && owner[i] == OPPONENT)
				oppSun += size[i];
			dormant[i] = 0;
		}
		day++;
	}

	// ally action
	if (allyDo[0] != WAIT)
	{
		allySun -= costOf(allyDo[1]);
		if (allyDo[0] == GROW)
		{
			size[allyDo[1]]++;
			dormant[allyDo[1]] = 1;
		}
		else if (allyDo[0] == SEED)
		{
			owner[allyDo[2]] = ALLY;
			dormant[allyDo[1]] = 1;
			dormant[allyDo[2]] = 1;
		}
		else
		{
			allySun -= 4;
			allyScore += nutrients + 2 * (richness[allyDo[1]] - 1);
			nutrients--;
		}
	}

	// opponent action
	if (oppDo[0] != WAIT)
	{
		oppSun -= costOf(oppDo[1]);
		if (oppDo[0] == GROW)
		{
			size[oppDo[1]]++;
			dormant[oppDo[1]] = 1;
		}
		else if (oppDo[0] == SEED)
		{
			owner[oppDo[2]] = OPPONENT;
			dormant[oppDo[1]] = 1;
			dormant[oppDo[2]] = 1;
		}
		else
		{
			oppSun -= 4;
			oppScore += nutrients + 2 * (richness[allyDo[1]] - 1);
			nutrients--;
		}
	}
}

void	printActionColor(int action[3], int player)
{
	if (player == ALLY)
		cout << "\033[31m";
	else
		cout << "\033[34m";
	cout << "> ";
	if (action[0] == WAIT)
		cout << "WAIT zZZzzZzzz";
	else if (action[0] == SEED)
		cout << "SEED " << action[1] << " " << action[2] << " welcome";
	else if (action[0] == GROW)
		cout << "GROW " << action[1] << " be strong";
	else
		cout << "COMPLETE " << action[1] << " jackpot!";
	cout << "\033[0m";
	cout << endl;
}

void	drawLine()
{
	cout << endl << "-----------------------------------------------------------" << endl << endl;
}

void	playAndDraw()
{
	Game	game;

	game.boardRich0 = 0;
	game.boardRich1 = 0;
	game.boardRich2 = 0;
	game.boardRich3 = 0;
	game.maxTreeSize3 = 4;
	game.day = 0;
	game.scanGrid();
	game.fillDist();
	while (game.day < 5) {
		if (game.day == 0)
		{
			game.scanInfo();
			game.initBoard();
			game.scanTrees();
			game.scanMoves();
		}
		game.fillDiag();
		game.calculateSpooky();
		game.calculateReachable();
		game.calculateCost();
		game.calculateScore();
		game.sortOnScore();
		game.richnessImportance = 2 * sqrt((double)game.boardRich0);
		if (game.richnessImportance == 0)
			game.richnessImportance = 1;
		game.drawBoard();
		game.action();
		game.oppDo[0] = WAIT;

		cout << "Decided actions :" << endl;
		printActionColor(game.allyDo, ALLY);
		printActionColor(game.oppDo, OPPONENT);
		drawLine();
		game.updateBoard();
	}
	return ;
}

int	main(void)
{
	playAndDraw();
	return (0);
}