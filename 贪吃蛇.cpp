#include<thread>//922104780732编写
#include<Windows.h>
#include<conio.h>
#include<random>
#include<queue>
class DOT {
	std::random_device rd;
	std::default_random_engine e{ rd() };
	std::uniform_int_distribution<short> dist{ 0,225 - len };
	std::queue<_COORD> HIS;
	HANDLE StdHandle = GetStdHandle(DWORD(-11));
	HWND__* stdHwnd = GetConsoleWindow();
	UCHAR len{ 1 }, vkey = 0x0F;
	char x, y, output[32]{}, map[15][15]{};
	bool life = true;
public:
	DOT(char _X, char _Y) :x(_X), y(_Y) {
		tagRECT rect;
		system("mode con cols=30 lines=15");
		auto scrWidth = GetSystemMetrics(SM_CXSCREEN),scrHeight = GetSystemMetrics(SM_CYSCREEN);
		GetWindowRect(stdHwnd, &rect);
		rect.left = (scrWidth - rect.right) / 2,rect.top = (scrHeight - rect.bottom) / 2;
		SetWindowPos(stdHwnd, HWND_TOP, rect.left, rect.top, rect.right, rect.bottom,SWP_SHOWWINDOW);
		system("mode con cols=30 lines=15");
		CONSOLE_CURSOR_INFO CCIMode;
		GetConsoleCursorInfo(StdHandle, &CCIMode);//获取控制台光标信息
		CCIMode.bVisible = false;//隐藏控制台光标
		SetConsoleCursorInfo(StdHandle, &CCIMode);//设置控制台光标状态
		for (auto n = 0; n < 15; n++) printf_s("□□□□□□□□□□□□□□□");
		SummonFood();
	}
	~DOT() { while (HIS.empty() == 0) { HIS.pop(); } }
	void SummonFood(void) {
		UCHAR X(0), Y(0), CT(0), Random = UCHAR(dist(e));
		for (unsigned char loop(0); loop < Random; CT++) {
			X = CT % 15;
			Y = (CT / 15);// % 15;
			if (map[X][Y] == 0) loop++;
		}
		map[X][Y] = -1;
		SetConsoleCursorPosition(StdHandle, { short(2 * Y),X });
		printf_s("%s", "●");
	}
	void RecordPos(void) { HIS.emplace(_COORD{ this->x, this->y }); }
	void BlankLastPos(void) {
		map[HIS.front().X][HIS.front().Y] = 0;
		SetConsoleCursorPosition(StdHandle, { short(2 * HIS.front().Y),HIS.front().X });
		printf_s("%s", "□");
		HIS.pop();
	}
	void Detect(void) {
		while (life) {
			if (_kbhit()) {
				auto key = _getch();//上 ↑：72 下 ↓： 80 左 ←： 75右  →： 77
				switch (key) {//len==1判断 避免一种愚蠢死法
				case 72:	if (len == 1 || vkey != 0xFF) vkey = 0x00; break;
				case 80:	if (len == 1 || vkey != 0x00) vkey = 0xFF; break;
				case 75:	if (len == 1 || vkey != 0x0F) vkey = 0xF0; break;
				case 77:	if (len == 1 || vkey != 0xF0) vkey = 0x0F; break;
				};
			}
			Sleep(20);
		}
	}
	void Display(void) {
		while (life) {
			RecordPos();
			switch (vkey) {//节省if执行次数
			case 0x00:	x--;	if (x < 0)	goto GAMEOVER; 	break;
			case 0xFF:	x++;	if (x > 14)	goto GAMEOVER; 	break;
			case 0xF0:	y--;	if (y < 0)	goto GAMEOVER;	break;
			case 0x0F:	y++;	if (y > 14)	goto GAMEOVER;	break;
			}
			if (map[x][y] == 1)		goto GAMEOVER;
			if (map[x][y] == -1) {
				SummonFood();
				len++;
			} else BlankLastPos();
			map[x][y] = 1;
			SetConsoleCursorPosition(StdHandle,{short(2*y),x});
			printf_s("%s","■");
			Sleep(75);
		}
	GAMEOVER://GOTO语句一次性跳出多个结构
		life = false;
		if (len == 225)	MessageBoxA(0, "ThankYouForPlaying!", "!WOW!", MB_ICONASTERISK);
		else {
			sprintf_s(output, "WellPlayer!\nYour Point is %u", len);
			MessageBoxA(0, output, "GAMEOVER", MB_ICONASTERISK);
		}
	}
}; DOT SN(7, 7);
signed main(void) {
	std::thread DP(&DOT::Display, &SN);
	std::thread DT(&DOT::Detect, &SN);
	DT.join();
	DP.join();
	return 0;
}