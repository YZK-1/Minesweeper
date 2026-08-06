#pragma once

#include <vector>
#include <iostream>
#include <random>
#include "GameState.h"

enum class FlagState {		//	旗状态
	None,		//	□
	Flag,		//	P
	Question	//	?
};

class Board
{
private:

	int rows;				//	行数
	int cols;				//	列数
	int mineCount;			//	雷数
	int flagCount;			//	旗数
	std::mt19937 rng;		//	随机数种子
	GameState state;		//	胜负状态
	int remainingSafeCells;	//	未打开的非雷格子数

	std::vector<std::vector<int>>board;			//	雷图
	std::vector<std::vector<bool>>revealed;		//	格状态
	std::vector<std::vector<FlagState>>flagged;	//	旗状态

	static constexpr int direction[8][2] = {	//	方向
			{-1,-1},{-1,0},{-1,1},	//	上左  上  上右
			{0 ,-1},       {0, 1},	//	  左        右
			{1 ,-1},{1, 0},{1, 1}	//	下左  下  下右
	};

public:
	
	Board(int r, int c, int m);							//	构造函数
	void init();										//	初始化
	void playerMove(int row, int col, bool flagMode);	//	玩家操作
	GameState getState()const;							//	获取胜负状态
	int getFlags()const;								//	获取旗数
	void reset(bool newMap = true);						//	重新开始
	void display() const;								//	打印地图 (玩家视角)
	void displayDebug() const;							//	打印地图 (上帝视角)		//调试、失败时调用

private:

	void placeMines();									//	随机放雷		
	void calculateNumber();								//	计算附近雷数	
	bool inRange(int r, int c) const;					//	判断越界	
	void reveal(int row, int col);						//	打开格子
	void expandEmpty(int row, int col);					//	展开空块
	void toggleFlag(int row, int col);					//	插旗
	void chordReveal(int row, int col);					//	数字展开
	void printXLabel() const;							//	打印横坐标
	void printYLabel(int row, int col) const;			//	打印竖坐标

};