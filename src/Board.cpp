//#include "Board.h"
//
////	构造函数
//Board::Board(int r, int c, int m) {
//	rows = r;
//	cols = c;
//	mineCount = m;
//}
//
////	初始化
//void Board::init() {
//	steps = rows * cols;
//	state = GameState::Playing;
//	remainingSafeCells = rows * cols - mineCount;
//
//	board.resize(rows, std::vector<int>(cols, 0));
//	revealed.resize(rows, std::vector<bool>(cols, false));
//	std::cout << "***";
//	void placeMines();
//	std::cout << "***";
//}
////	随机放雷
//void Board::placeMines() {
//	std::cout << "***";
//	int correntMineCount = 0;	//	当前写入雷数
//	int correntrow;				//	当前行
//	int correntcol;				//	当前列
//
//	while (correntMineCount < mineCount) {
//		correntrow = rand() % rows;	//	当前行
//		correntcol = rand() % cols;	//	当前列
//
//		if (board[correntrow][correntcol] == -1) {
//			continue;
//		}
//		board[correntrow][correntcol] = -1;
//		correntMineCount++;
//		std::cout << correntMineCount << std::endl;
//	}
//}
//
////	临时打印	*****
//void Board::display() {
//	for (int i = 0; i < board.size(); i++) {
//		for (int j = 0; j < board.size(); j++) {
//			std::cout << board[i][j] << " ";
//		}
//		std::cout << std::endl;
//	}
//}