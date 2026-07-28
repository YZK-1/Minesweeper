#include "Board.h"
#include<string>


int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "请输入有效数字" << std::endl;
            continue;
        }
		return value;
    }
}

void readCoordinate(const std::string& prompt,int& row, int& col) {
	while (true) {
		std::cout << prompt;
		std::cin >> row >> col;
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "请输入有效数字" << std::endl;
			continue;
		}
		return;
	}
}

int main()
{
	int rows;
	int cols;
	int mineCount;

	rows = readInt("请输入行数:");
	cols = readInt("请输入列数:");
	mineCount = readInt("请输入雷数:");

	Board b(rows, cols, mineCount);
	b.init();
	//b.displayDebug();

	int row;
	int col;

	while (true) {
		b.display();

		if (b.getState() == GameState::Win) {	//	检查胜利
			std::cout << "步数:" << b.getSteps() << std::endl;
			std::cout << "胜利" << std::endl;
			break;
		}
		else if (b.getState() == GameState::Playing) {
			std::cout << "游戏中" << std::endl;
			std::cout << "步数:" << b.getSteps() << std::endl;
		}
		else if (b.getState() == GameState::Lose) {
			b.displayDebug();
			std::cout << "步数:" << b.getSteps() << std::endl;
			std::cout << "失败" << std::endl;
			break;
		}

		readCoordinate("请输入坐标:",col,row);		//	(横坐标,竖坐标)
		b.reveal(row,col);

	}
	
}
