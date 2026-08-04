#include "Board.h"
#include <string>
#include <limits>

template<typename T>
void readValue(const std::string& prompt, T& value) {
	while (true) {
		std::cout << prompt;
		std::cin >> value;
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "请输入有效数字" << std::endl;
			continue;
		}
		return;
	}
}

void readCoordinate(const std::string& prompt, int& col, int& row) {
	while (true) {
		std::cout << prompt;
		std::cin >> col >> row;		//	先横坐标,后纵坐标
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "请输入有效数字" << std::endl;
			continue;
		}
		return;
	}
}

//	询问是否重开
bool askRestart(Board& b) {
	int choice;

	std::cout << "(0=原图重开,1=新图重开,其它=退出)" << std::endl;
	std::cout << "是否重新开始:";
	std::cin >> choice;

	if (std::cin.fail()) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return false;
	}
	else if (choice == 0) {		//	原图重开
		b.reset(false);
		return true;
	}
	else if (choice == 1) {		//	新图重开
		b.reset(true);
		return true;
	}
	else {
		return false;
	}
}
//	游戏中
bool playing(const Board& b) {
	if (b.getState() == GameState::Playing) {
		b.display();
		std::cout << "游戏中" << std::endl;
		std::cout << "旗数:" << b.getFlags() << std::endl;
		return true;	//	游戏继续
	}
	else {
		return false;
	}
}

bool game(Board& b) {
	bool continueGame = false;
	if (b.getState() == GameState::Win) {	//	检查胜利
		b.display();
		std::cout << "旗数:" << b.getFlags() << std::endl;
		std::cout << "胜利" << std::endl;
	}
	else if (b.getState() == GameState::Lose) {
		b.displayDebug();
		std::cout << "旗数:" << b.getFlags() << std::endl;
		std::cout << "失败" << std::endl;
	}
	else { 
		if (playing(b)) {
			continueGame = true;
		}
	}

	if (!continueGame) {
		if (askRestart(b)) {
			playing(b);
			continueGame = true;
		}
	}
	return continueGame;
}

int main()
{
	int rows;
	int cols;
	int mineCount;

	readValue("请输入行数:",rows);
	readValue("请输入列数:",cols);
	readValue("请输入雷数:", mineCount);

	Board b(rows, cols, mineCount);
	b.init();

	int row;
	int col;
	bool flagMode;

	while (true) {
		if (!game(b)) { break; }			//	不重开 结束	

		readValue("请输入模式(0=揭开,1=插旗):", flagMode);
		readCoordinate("请输入坐标:",col,row);		//	(横坐标,竖坐标)

		b.playerMove(row, col, flagMode);
	}
}