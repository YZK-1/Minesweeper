#include <cstdlib>

#include <iostream>
#include <string>
#include <limits>
#include <sstream>
#include <chrono>

#include "Board.h"

//	读取单个变量
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

// 读取坐标
bool readCoordinate(std::istringstream& iss, int& col, int& row) {
	iss >> col >> row;
	return !iss.fail();	//	0 读取失败
}

//	询问是否重开
bool askRestart(Board& b) {
	int choice;

	std::cout << "(0=原图重开,1=新图重开,其它=退出)" << std::endl;
	std::cout << "是否重新开始:";
	std::cin >> choice;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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

bool game(Board& b, std::chrono::steady_clock::time_point& startTime) {
	bool continueGame = false;

	if (playing(b)) {	//	游戏中
		continueGame = true;
	}
	else {
		const auto endTime = std::chrono::steady_clock::now();
		const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

		if (b.getState() == GameState::Win) {	//	检查胜利
			b.display();
			std::cout << "旗数:" << b.getFlags() << std::endl;
			std::cout << "用时:" << seconds << " 秒" << std::endl;
			std::cout << "胜利" << std::endl;
		}
		else if (b.getState() == GameState::Lose) {
			b.displayDebug();
			std::cout << "旗数:" << b.getFlags() << std::endl;
			std::cout << "用时:" << seconds << " 秒" << std::endl;
			std::cout << "失败" << std::endl;
		}
	}

	if (!continueGame) {
		if (askRestart(b)) {
			startTime = std::chrono::steady_clock::now();	//	更新开始时间戳
			system("cls");	//	清屏	
			playing(b);		//	打印新图
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

	readValue("请输入行数:", rows);
	readValue("请输入列数:", cols);
	readValue("请输入雷数:", mineCount);
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	//	清除缓冲区,否则 getline 会读到

	Board b(rows, cols, mineCount);
	b.init();

	int row;
	int col;
	bool flagMode = false;	//	默认揭开

	auto startTime = std::chrono::steady_clock::now();	//	开始时间戳	

	while (true) {
		system("cls");	//	清屏	
		if (!game(b, startTime)) { break; }		//	不重开 结束	

		std::cout << (flagMode ? "[插旗]" : "[揭开]") << std::endl;
		std::cout << "输入坐标,m=切换: ";

		//	输入及校验
		std::string line;				//	存储输入流
		std::getline(std::cin, line);	//	读取输入流
		std::istringstream iss(line);	//	写入输入流

		if (!readCoordinate(iss, col, row)) {	//	输入行、列
			//	行、列写入失败
			std::string token;
			iss.clear();		//	清除 fail 状态
			iss.str(line);		//	重新写入输入流
			iss >> token;		//	检查第一个 token 是不是 m
			if (token == "m" || token == "M") { flagMode = !flagMode; }
			continue;
		}

		b.playerMove(row, col, flagMode);
	}
}