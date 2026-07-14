#pragma once

#include <vector>
#include <iostream>
#include <random>
#include "GameState.h"

#include <fstream>

class Board
{
private:
	int rows;				//	行数
	int cols;				//	列数
	int mineCount;			//	雷数
	int steps;				//	步数
	GameState state;		//	胜负状态
	int remainingSafeCells;	//	未打开的非雷格子数

	std::vector<std::vector<int>>board;
	std::vector<std::vector<bool>>revealed;

public:
	//	构造函数
	Board(int r, int c, int m) {
		rows = r;
		cols = c;
		mineCount = m;
	}
	//	初始化
	void init() {
		std::cout << "初始化" << std::endl;	//
		steps = rows * cols;
		state = GameState::Playing;
		remainingSafeCells = rows * cols - mineCount;

		std::cout << "构建board" << std::endl;	//
		board.resize(rows, std::vector<int>(cols, 0));
		std::cout << "构建revealed" << std::endl;	//
		revealed.resize(rows, std::vector<bool>(cols, false));

		std::cout << "放雷" << std::endl;	//
		placeMines();

		std::cout << "计算雷数" << std::endl;	//
		calculateNumber();
	}
	//	随机放雷
	void placeMines() {
		int correntMineCount = 0;	//	当前写入雷数

		while (correntMineCount < mineCount) {
			int correntrow = rand() % rows;	//	当前行
			int correntcol = rand() % cols;	//	当前列


			if (board[correntrow][correntcol] == -1) {
				continue;
			}
			board[correntrow][correntcol] = -1;
			correntMineCount++;
			std::cout << correntrow			//	临时展示雷坐标
				<< ":"
				<< correntcol
				<< std::endl;
		}
		std::cout << "写入雷数: " << correntMineCount << std::endl;
	}
	//	判断越界
	bool inRange(int r,int c) {
		return r>=0 &&		//	ture	未越界
			   r<rows &&	//	false	越界
			   c>=0 &&
			   c<cols;
	}
	//	计算附近雷数	
	void calculateNumber() {
		int direction[8][2] = {
			{-1,-1},{-1,0},{-1,1},	//	上左  上  上右
			{0 ,-1},       {0, 1},	//	  左        右
			{1 ,-1},{1, 0},{1, 1}	//	下左  下  下右
		};

		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				if (board[row][col] != -1) {			//	不是雷跳过
					continue;
				}

				for (auto& dir : direction) {
					int newRow = row + dir[0];
					int newCol = col + dir[1];
					
					if (inRange(newRow, newCol)				//	判断越界
						&& board[newRow][newCol] != -1) {	//	遇雷不加
						board[newRow][newCol]++;
					}
				}
			}
		}
	}
	// 导出SVG地图
	void exportSVG(const std::string& filename)
	{
		std::ofstream file(filename);

		if (!file.is_open())
		{
			std::cout << "SVG文件打开失败\n";
			return;
		}

		int cellSize = 30;

		int width = cols * cellSize;
		int height = rows * cellSize;


		// SVG头
		file << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
			<< "width=\"" << width << "\" "
			<< "height=\"" << height << "\">\n";


		// 绘制每个格子
		for (int row = 0; row < rows; row++)
		{
			for (int col = 0; col < cols; col++)
			{
				int x = col * cellSize;
				int y = row * cellSize;


				// 格子边框
				file << "<rect x=\"" << x
					<< "\" y=\"" << y
					<< "\" width=\"" << cellSize
					<< "\" height=\"" << cellSize
					<< "\" fill=\"white\" stroke=\"black\"/>\n";


				// 文字
				file << "<text x=\"" << x + 10
					<< "\" y=\"" << y + 22
					<< "\" font-size=\"20\">";


				if (board[row][col] == -1)
				{
					file << "*";
				}
				else
				{
					file << board[row][col];
				}


				file << "</text>\n";
			}
		}


		// SVG结束
		file << "</svg>";

		file.close();

		std::cout << "SVG生成完成\n";
	}
	// 打印地图(玩家视角)
	void display() {
		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				if (revealed[row][col] == false) {
					std::cout << "□ ";
				}
				else {
					std::cout << board[row][col]<<"  ";
				}
			}
			std::cout << std::endl;
		}
	}
	//	临时打印地图(上帝视角)	*****	//调试、胜利、失败时调用
	void displayDebug() {
		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				if (board[row][col] == -1) {
					std::cout << " * ";
				}
				else {
					std::cout << " " << board[row][col] << " ";
				}
			}
			std::cout << std::endl;
		}
	}

};