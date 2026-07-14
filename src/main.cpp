#include<iostream>
#include "Board.h"

int main()
{
	srand(time(nullptr));
	Board b(100,100,1000);
	b.init();
	std::cout << "打印地图" << std::endl;
	//b.displayDebug();
	//b.display();

	b.exportSVG("minefield.svg");
}
