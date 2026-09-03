#include <cstdlib>
#include <cstdio>

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
		if (std::cin.eof()) {	//	结束输入(如: Ctrl + Z)
			std::cout << "检测到程序结束,退出程序" << std::endl;
			std::exit(0);		//	退出程序
		}
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

	if (std::cin.fail()) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return false;
	}
	else {
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		if (choice == 0) {			//	原图重开
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

//	对局
bool match(Board& b, std::chrono::steady_clock::time_point& startTime) {
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
			startTime = std::chrono::steady_clock::time_point{};	//	重置开始时间戳
			std::printf("\033[2J\033[1;1H");	//	清屏	
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

	auto startTime = std::chrono::steady_clock::time_point{};	//	开始时间戳	

	while (true) {
		std::printf("\033[2J\033[1;1H");		//	清屏	
		if (!match(b, startTime)) { break; }	//	不重开 结束	

		std::cout << (flagMode ? "[插旗]" : "[揭开]") << std::endl;
		std::cout << "输入坐标,m=切换: ";

		//	输入及校验
		std::string line;						//	存储输入流
		if (!std::getline(std::cin, line)) {	//	读取失败 (如: Ctrl + Z 的 EOF)
			std::cout << "检测到输入结束,退出程序" << std::endl;
			std::exit(0);
		}
		std::istringstream iss(line);			//	写入输入流

		if (!readCoordinate(iss, col, row)) {	//	输入列、行
			//	列、行写入失败
			std::string token;
			iss.clear();		//	清除 fail 状态
			iss.str(line);		//	重新写入输入流
			iss >> token;		//	检查第一个 token 是不是 m
			if (token == "m" || token == "M") { flagMode = !flagMode; }
			continue;
		}

		const bool move = b.playerMove(row, col, flagMode);	//	执行玩家操作

		//	首开计时
		if (move && startTime == std::chrono::steady_clock::time_point{}) {
			startTime = std::chrono::steady_clock::now();
		}

	}
}
