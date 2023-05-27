#include<iostream>
#include<vector>
#include<tuple>

class board {
	char playerType;//'O' 'X'
	char gameState;
	int OsuccNum;
	int OloseNum;
	int OdrawNum;
	int XsuccNum;
	int XloseNum;
	int XdrawNum;
	int WIN;
	std::vector<std::vector<char> > achBoard;
	std::vector<std::tuple<char, int, int> > regret;
	void drawBoard();
	void regretOp();
	void playTurn(char playerType);
	char checkState(char playerType);
	void showData();
public:
	board(int n,int win) :achBoard(n, std::vector<char>(n)) {
		OsuccNum = 0;
		OloseNum = 0;
		OdrawNum = 0;
		XsuccNum = 0;
		XloseNum = 0;
		XdrawNum = 0;
		playerType = 0;
		gameState = -1;
		WIN = win;
	}
	void newGame();
	void printRule();
};

void board::newGame() {
	//Reset Board
	for (int row = 0; row < achBoard.size(); ++row) {
		for (int col = 0; col < achBoard[0].size(); ++col) {
			achBoard[row][col] = ' ';
		}
	}
	//Reset GameState
	gameState = -1;
	//Reset Regret
	regret.clear();
	//infinte-loop untill Gameover
	while (gameState == -1) {
		playTurn('O');
		if (gameState != -1) break;
		playTurn('X');
	}
	//Print Result
	switch (gameState) {
	case 0:
		std::cout << "OX DRAW";
		++OdrawNum;
		++XdrawNum;
		break;
	case 1:
		std::cout << "O WIN";
		++OsuccNum;
		++XloseNum;
		break;
	case 2:
		std::cout << "X WIN";
		++XsuccNum;
		++OloseNum;
		break;
	}
	std::cout << '\n';
	showData();
}

void board::printRule() {
	system("cls");
	std::cout << "\t\t欢迎使用三子连珠游戏!" << '\n';
	std::cout << "\t游戏规则:" << '\n';
	std::cout << "\t1.每1步输入要下棋的格子的x，y坐标，x为行坐标，y为列坐标，按Enter键\n";
	std::cout << "\t2.有1方首先3个棋子连成一线即判获胜\n";
	std::cout << "\t3.当棋子布满棋盘但仍无获胜方即为平局\n";
	std::cout << "\t4.X方先行\n";
	std::cout << "\n\n\t\t请按任意键开始下棋!\n\n";
	system("pause>nul");
}

void board::playTurn(char playerType) {
	int row, col;
	drawBoard();
	std::cout << playerType << "'s Turn:" << '\n' << "Pos(x y)>>";
	while (1) {//infinte-loop untill get vaild input
		std::cin >> row >> col;
		if (row == -1 && col == -1) {
			//OP:regret
			regretOp();
		} else if (row > 0 && col > 0 &&
			row <= achBoard.size() && col <= achBoard[0].size()) {
			//OP:Vaild
			if (achBoard[row - 1][col - 1] == ' ') {//check
				achBoard[row - 1][col - 1] = playerType;
				drawBoard();
				gameState = checkState(playerType);
				regret.push_back(std::make_tuple(playerType, row, col));
				break;
			} else
				std::cout << "This Pos already has piece" << '\n';
		} else {
			//OP:Invaild
			std::cout << "Invaild Pos" << '\n';
		}
		std::cout<< "Pos(x y)>>";
	}
}

char board::checkState(char playerType) {
	//Double Circle Iterate
	size_t rows = achBoard.size();
	size_t cols = achBoard[0].size();
	int blankCount = 0;
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			if (achBoard[row][col] == ' ') ++blankCount;
			int max = 0;//

			/* -> */
			int tmp = 0;
			for (int colIter = 0; colIter + col < cols && colIter < WIN; ++colIter) {
				if (achBoard[row][col + colIter] == playerType) ++tmp;
			}
			max = max > tmp ? max : tmp;
			/*  ↓ */
			tmp = 0;
			for (int rowIter = 0; rowIter + row < rows && rowIter < WIN; ++rowIter) {
				if (achBoard[row + rowIter][col] == playerType) ++tmp;
			}
			max = max > tmp ? max : tmp;
			/*  \ */
			tmp = 0;
			for (int Iter = 0; Iter + row < rows && Iter + col < cols && Iter < WIN; ++Iter) {
				if (achBoard[row + Iter][col + Iter] == playerType) ++tmp;
			}
			max = max > tmp ? max : tmp;

			if (max == WIN) return playerType == 'O' ? 1 : 2;//1:O-win 2:X-win
		}
	}
	if (blankCount == 0) return 0;
	else return -1;
}

void board::showData() {
	std::cout << "X Win>>" << XsuccNum << '\t' << "Lose>>" << XloseNum << '\t' << "Draw>>" << XdrawNum << '\n';
	std::cout << "O Win>>" << OsuccNum << '\t' << "Lose>>" << OloseNum << '\t' << "Draw>>" << OdrawNum << '\n';
}

void board::drawBoard() {
	system("cls");
	for (int col = 1; col <= achBoard[0].size(); ++col) {
		std::cout << "   " << col;
	}
	std::cout << std::endl;
	for (int row = 0; row < achBoard.size(); ++row) {
		std::cout << row + 1 << "  ";
		for (int col = 0; col < achBoard[0].size(); ++col) {
			std::cout << achBoard[row][col] << " | ";
		}
		std::cout << '\n' << std::endl;
	}
}

void board::regretOp() {
	char player;
	int row, col;
	if (regret.size()<2) {
			std::cout << "Failed To Regret" << '\n';
			return;
	}
	for (int c = 0; c < 2; ++c) {
		
		std::tie(player, row, col) = regret.back();
		regret.pop_back();
		achBoard[row - 1][col - 1] = ' ';
		std::cout << "Regret:" << player << '\t' << row << '\t' << col << '\n';
	}
	drawBoard();
	return;
}

int main() {
	int bsize, win;
	std::cout << "Board Size>>";
	std::cin >> bsize;
	std::cout << '\n' << "Win>>";
	std::cin >> win;
	board b(bsize,win);
	b.printRule();
	while (true) {
		b.newGame();
		std::cout << "PlayAgain? Y|N" << '\n';
		char sel;
		std::cin >> sel;
		if (sel == 'N' || sel == 'n') break;
	}
}
