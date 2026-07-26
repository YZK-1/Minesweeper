#pragma once

#include <vector>
#include <iostream>
#include <random>
#include "GameState.h"



class Board
{
private:		

	int rows;				//	行数
	int cols;				//	列数
	int mineCount;			//	雷数
	std::mt19937 rng;		//	随机数
	int steps;				//	步数
	GameState state;		//	胜负状态
	int remainingSafeCells;	//	未打开的非雷格子数

	std::vector<std::vector<int>>board;		//	雷图
	std::vector<std::vector<bool>>revealed;	//	格状态

	static constexpr int direction[8][2] = {	//	方向
			{-1,-1},{-1,0},{-1,1},	//	上左  上  上右
			{0 ,-1},       {0, 1},	//	  左        右
			{1 ,-1},{1, 0},{1, 1}	//	下左  下  下右
	};

public:
	//	读入配置
	void inputBoardSize() {

	}
	//	难度选择
	
	//	构造函数
	Board(int r, int c, int m)
		:rows(r),
		cols(c),
		mineCount(m),
		rng(std::random_device{}())
	{
	}
	//	初始化
	void init() {
		steps = 0;										//	步数
		state = GameState::Playing;						//	状态
		remainingSafeCells = rows * cols - mineCount;	//	剩余安全块

		board.resize(rows, std::vector<int>(cols, 0));			//	雷图
		revealed.resize(rows, std::vector<bool>(cols, false));	//	格状态

		placeMines();		//	布雷
		calculateNumber();	//	计算雷数
	}
	//	随机放雷
	void placeMines() {
		int correntMineCount = 0;	//	当前写入雷数

		std::uniform_int_distribution<int> rowDist(0, rows-1);
		std::uniform_int_distribution<int> colDist(0, cols-1);
		int cnt = 0;
		while (correntMineCount < mineCount) {
			std::cout << cnt<<std::endl;
			cnt++;
			int r = rowDist(rng);
			int c = colDist(rng);

			if (board[r][c] == -1) {
				continue;
			}
			board[r][c] = -1;
			correntMineCount++;

		}
		std::cout << "写入雷数: " << correntMineCount << std::endl;
	}
	//	判断越界
	bool inRange(int r, int c) {
		return r >= 0 &&		//	true	未越界
			r < rows &&	//	false	越界
			c >= 0 &&
			c < cols;
	}
	//	计算附近雷数	
	void calculateNumber() {
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
	//	打开格子
	void reveal(int row, int col) {
		if (!inRange(row, col) || revealed[row][col]) {	//	越界 已开
			return;
		}
		//	标记
		revealed[row][col] = true;
		steps++;				//	步数 +1

		if (board[row][col] == -1) {		//	是雷
			state = GameState::Lose;
			return;
		}

		remainingSafeCells--;	//	安全格 -1

		if (board[row][col] > 0) {			//	是数字
			return;
		}


		expandEmpty(row, col);				//	是空块
	}

	//	展开空块
	void expandEmpty(int row, int col) {	
		if (board[row][col] != 0) {			//	确保空块进入
			return;
		}



		for (auto& dir : direction) {
			int newrow = row + dir[0];
			int newcol = col + dir[1];
			if (inRange(newrow, newcol)&& !revealed[newrow][newcol]) {

				if (board[newrow][newcol] > 0) {		//	数字打开并跳过
					revealed[newrow][newcol] = true;
					remainingSafeCells--;				//	安全格 -1
					continue;
				}

				if (board[newrow][newcol] == 0) {		//	0 打开并继续递归
					revealed[newrow][newcol] = true;
					remainingSafeCells--;				//	安全格 -1

					expandEmpty(newrow, newcol);
				}
			}
		}
	}

	//	更新胜负状态
	void updateState() {	
		if (state == GameState::Lose) { return; }
		if (remainingSafeCells == 0) { state = GameState::Win; }
	}

	//	获取胜负状态
	GameState getState(){
		return state;
	}

	//	获取步数
	int getSteps() {
		return steps;
	}

	//	读取输入
	void getCoordinate(int row,int col) {
		
		reveal(row, col);
	}


	//	打印地图 (玩家视角)
	void display() {
		//	横坐标
		if (cols <= 59) {	//	控制台限制到59			
			std::cout << "   ";
			for (int col = 0; col < cols; col++) {
				if (col < 10) {
					std::cout << "0" << col << " ";
				}
				else {
					std::cout << col << " ";
				}
			}
			std::cout << std::endl;
		}
		//	竖坐标及地图
		for (int row = 0; row < rows; row++) {			
			for (int col = 0; col < cols; col++) {
				//	竖坐标
				if (col == 0) {	
					if (row < 10) {
						std::cout << "0" << row << " ";
					}
					else {
						std::cout << row << " ";
					}
				}

				if(!revealed[row][col]){				//	未打开
					std::cout << " □ ";
				}
				else {
					if (board[row][col] == -1) {		//	雷
						std::cout << " * ";
					}
					else if (board[row][col] == 0) {	//	0不输出
						std::cout << "   ";
					}
					else {								//	数字
						std::cout << " " << board[row][col] << " ";
					}
				}
			}
			std::cout<< std::endl<<std::endl;
		}
		std::cout << std::endl << std::endl;
	}

	//	打印地图 (上帝视角)	*****	//调试、失败时调用
	void displayDebug() {
		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				if (board[row][col] == -1) {
					std::cout << " * ";
				}
				else if (board[row][col] == 0) {	//	0不输出
					std::cout << "   ";
				}
				else {
					std::cout <<" " << board[row][col] << " ";
				}
			}
			std::cout << std::endl << std::endl;
		}
	}

};