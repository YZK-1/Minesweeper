#include <cstdlib>

#include <vector>
#include <iostream>
#include <random>

#include "Board.h"

//	构造函数
Board::Board(int r, int c, int m)
	:rows(r),
	cols(c),
	mineCount(m),
	flagCount(0),
	rng(std::random_device{}()),
	state(GameState::Playing),
	remainingSafeCells(0),
	firstMove(true)
{
	if (rows <= 0) { rows = 1; }
	if (cols <= 0) { cols = 1; }
	if (mineCount > rows * cols) { mineCount = rows * cols; }
	if (mineCount <= 0) { mineCount = 1; }
	initialMineCount = mineCount;
}

//	初始化					
void Board::init() {
	mineCount = initialMineCount;					//	恢复雷数
	flagCount = mineCount;							//	剩余旗数
	remainingSafeCells = rows * cols - mineCount;	//	剩余安全块
	state = GameState::Playing;						//	游戏状态

	board.assign(rows, std::vector<int>(cols, 0));							//	雷图
	revealed.assign(rows, std::vector<bool>(cols, false));					//	格状态
	flagged.assign(rows, std::vector< FlagState>(cols, FlagState::None));	//	旗状态
}

//	首次点击				
void Board::firstClick(int row, int col) {
	firstMove = false;

	//	计算安全区实际块数(处理越界后)
	int safeCell = 0;
	for (int dr = -1; dr <= 1; dr++) {
		for (int dc = -1; dc <= 1; dc++) {
			if (inRange(row + dr, col + dc)) {
				safeCell++;
			}
		}
	}

	//	计算允许最大雷数
	int maxMines = rows * cols - safeCell;
	if (mineCount > maxMines) {
		mineCount = maxMines;	//	削减雷数
		flagCount = maxMines;	//	同步旗数
		remainingSafeCells = rows * cols - mineCount;	//	同步剩余安全块数
	}

	placeMines(row, col);		//	布雷
	calculateNumber();			//	计算雷数
	reveal(row, col);			//	第一次默认开块
}

//	玩家操作
void Board::playerMove(int row, int col, bool flagMode) {
	if (!inRange(row, col)) {		//	越界
		return;
	}
	if (firstMove) {				//	首次操作
		firstClick(row, col);
		return;
	}

	if (revealed[row][col]) {		//	已打开
		chordReveal(row, col);		//	数字展开(chord)
	}
	else if (flagMode) {			//	旗模式
		toggleFlag(row, col);
	}
	else {							//	铲模式
		reveal(row, col);
	}
}

//	获取胜负状态
GameState Board::getState() const noexcept {
	return state;
}

//	获取旗数
int Board::getFlags() const noexcept {
	return flagCount;
}

//	重新开始
void Board::reset(bool newMap) {
	if (newMap) {	//	新图 重开
		init();
		firstMove = true;
	}
	else {			//	原图 重开
		revealed.assign(rows, std::vector<bool>(cols, false));
		flagged.assign(rows, std::vector< FlagState>(cols, FlagState::None));
		remainingSafeCells = rows * cols - mineCount;
		flagCount = mineCount;
		state = GameState::Playing;
	}
}

//	打印地图 (玩家视角)
void Board::display() const {
	//	打印横坐标
	printXLabel();

	//	竖坐标及地图
	for (int row = 0; row < rows; row++) {
		//	打印竖坐标
		printYLabel(row);

		for (int col = 0; col < cols; col++) {
			if (!revealed[row][col]) {				//	未打开
				if (flagged[row][col] == FlagState::None) { std::cout << " □ "; }
				else if (flagged[row][col] == FlagState::Flag) { std::cout << " P "; }
				else if (flagged[row][col] == FlagState::Question) { std::cout << " ? "; }

			}
			else {
				if (board[row][col] == -1) {		//	雷
					std::cout << " * ";
				}
				else if (board[row][col] == 0) {	//	0 不输出
					std::cout << "   ";
				}
				else {								//	数字
					std::cout << " " << board[row][col] << " ";
				}
			}
		}
		std::cout << std::endl << std::endl;
	}
}

//	打印地图 (上帝视角)		//调试、失败时调用
void Board::displayDebug() const {
	//	打印横坐标
	printXLabel();

	for (int row = 0; row < rows; row++) {
		//	打印竖坐标
		printYLabel(row);

		for (int col = 0; col < cols; col++) {
			if (board[row][col] == -1) {
				std::cout << " * ";
			}
			else if (board[row][col] == 0) {	//	0 不输出
				std::cout << "   ";
			}
			else {
				std::cout << " " << board[row][col] << " ";
			}
		}
		std::cout << std::endl << std::endl;
	}
}

//	随机放雷		
void Board::placeMines(const int safeRow, const int safeCol) {
	//	收集所有安全区之外的格子
	std::vector<std::pair<int, int>>candidates;

	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			if (std::abs(row - safeRow) <= 1 &&
				std::abs(col - safeCol) <= 1) {
				continue;
			}
			candidates.push_back({ row,col });
		}
	}

	//	随机打乱格子
	std::shuffle(candidates.begin(), candidates.end(), rng);

	//	取前 mineCount 个格布雷
	for (int i = 0; i < mineCount; i++) {
		auto [r, c] = candidates[i];
		board[r][c] = -1;
	}
}

//	计算附近雷数	
void Board::calculateNumber() {
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			if (board[row][col] != -1) {			//	不是雷跳过
				continue;
			}

			for (const auto& dir : direction) {
				const int newRow = row + dir[0];
				const int newCol = col + dir[1];

				if (inRange(newRow, newCol)				//	判断越界
					&& board[newRow][newCol] != -1) {	//	遇雷不加
					board[newRow][newCol]++;
				}
			}
		}
	}
}

//	判断越界	
bool Board::inRange(int r, int c) const {
	return r >= 0 &&		//	true	未越界
		r < rows &&			//	false	越界
		c >= 0 &&
		c < cols;
}

//	打开格子
void Board::reveal(int row, int col) {
	if (!inRange(row, col) || revealed[row][col]) {	//	越界 已打开
		return;
	}

	//	标记
	revealed[row][col] = true;

	if (board[row][col] == -1) {					//	是雷
		state = GameState::Lose;
		return;
	}
	else {
		remainingSafeCells--;		//	安全格 -1

		if (board[row][col] == 0) {					//	是空块
			expandEmpty(row, col);
		}

		if (remainingSafeCells == 0) { state = GameState::Win; }
	}
}

//	展开空块	
void Board::expandEmpty(int row, int col) {
	if (board[row][col] != 0) {			//	确保空块进入
		return;
	}

	for (const auto& dir : direction) {
		const int newRow = row + dir[0];
		const int newCol = col + dir[1];
		if (inRange(newRow, newCol) && !revealed[newRow][newCol]	//	没有越界,没有展开
			&& flagged[newRow][newCol] != FlagState::Flag) {		//	不是旗

			if (board[newRow][newCol] == -1) {		//	雷跳过
				continue;
			}

			if (board[newRow][newCol] > 0) {		//	数字打开并跳过
				revealed[newRow][newCol] = true;
				remainingSafeCells--;				//	安全格 -1
				continue;
			}

			if (board[newRow][newCol] == 0) {		//	0 打开并继续递归
				revealed[newRow][newCol] = true;
				remainingSafeCells--;				//	安全格 -1

				expandEmpty(newRow, newCol);
			}
		}
	}
}

//	插旗
void Board::toggleFlag(int row, int col) {
	if (!revealed[row][col]) {		//	标记
		if (flagged[row][col] == FlagState::None) {				//	标旗
			if (flagCount > 0) {
				flagged[row][col] = FlagState::Flag;
				flagCount--;
			}
		}
		else if (flagged[row][col] == FlagState::Flag) {		//	标 ?
			flagged[row][col] = FlagState::Question;
			flagCount++;
		}
		else if (flagged[row][col] == FlagState::Question) {	//	取消旗
			flagged[row][col] = FlagState::None;
		}
	}
}

//	数字展开
void Board::chordReveal(int row, int col) {
	if (!revealed[row][col] || board[row][col] <= 0) {		//	没有展开,不是数字	
		return;
	}

	int cnt = 0;
	for (const auto& dir : direction) {			//	遍历 8 个方向
		const int newrow = row + dir[0];
		const int newcol = col + dir[1];

		if (inRange(newrow, newcol) && flagged[newrow][newcol] == FlagState::Flag) {		//	计算旗数
			cnt++;
		}
	}
	if (board[row][col] == cnt) {
		for (auto& dir : direction) {		//	遍历 8 个方向
			const int newrow = row + dir[0];
			const int newcol = col + dir[1];

			if (inRange(newrow, newcol) && flagged[newrow][newcol] != FlagState::Flag) {	//	不是旗
				if (!revealed[newrow][newcol]) {	//	没打开
					reveal(newrow, newcol);
				}
			}
		}
	}
}

//	打印横坐标
void Board::printXLabel() const {
	std::cout << "   行数:" << rows
		<< "  列数:" << cols
		<< "  雷数:" << mineCount
		<< std::endl << std::endl;

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
}

//	打印竖坐标
void Board::printYLabel(int row) const {
	if (row < 10) {
		std::cout << "0" << row << " ";
	}
	else {
		std::cout << row << " ";
	}
}