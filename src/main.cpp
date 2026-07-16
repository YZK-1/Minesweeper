#include "Board.h"

int main()
{
	srand(time(nullptr));

	int rows;
	int cols;
	int mineCount;

	std::cout << "请输入行数:";
	std::cin >> rows;
	std::cout << "请输入列数:";
	std::cin >> cols;
	std::cout << "请输入雷数:";
	std::cin >> mineCount;
	Board b(rows, cols, mineCount);
	b.init();
	b.displayDebug();

	int row;
	int col;

	while (true) {
		b.display();

		if (b.checkWin() == GameState::Win) {	//	检查胜利
			std::cout << "步数:" << b.steps << std::endl;
			std::cout << "胜利" << std::endl;
			break;
		}
		else if (b.checkWin() == GameState::Playing) {
			std::cout << "游戏中" << std::endl;
			std::cout << "步数:" << b.steps << std::endl;
		}
		else if (b.checkWin() == GameState::Lose) {
			b.displayDebug();
			std::cout << "步数:" << b.steps << std::endl;
			std::cout << "失败" << std::endl;
			break;
		}

		std::cout << "请输入坐标:";
		std::cin >> col >> row;
		b.getCoordinate(row,col);	//	(横坐标,竖坐标)


	}
	

	//b.exportSVG("minefield.svg");
}
