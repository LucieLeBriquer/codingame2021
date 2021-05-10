#include "../bot.cpp"

void	Game::drawBoard()
{

}

void	playAndDraw()
{
	Game	game;

	game.boardRich0 = 0;
	game.boardRich1 = 0;
	game.boardRich2 = 0;
	game.boardRich3 = 0;
	game.maxTreeSize3 = 4;
	game.scanGrid();
	game.fillDist();
	while (1) {
		game.scanInfo();
		game.initBoard();
		game.scanTrees();
		game.scanMoves();

		//game.printInput();

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
		return ;
	}
	return ;
}

int	main(void)
{
	playAndDraw();
	return (0);
}