#include<windows.h>
#include<iostream>
#include<thread>
#include<vector>
#include<unordered_map>
class OCX {
	HANDLE stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE stdInputHandle = GetStdHandle(STD_INPUT_HANDLE);
	HWND stdHwnd = GetConsoleWindow();
	tagPOINT currentCursorPos{};
	tagPOINT stdCursorPos{};
	tagPOINT prevStdCursorPos{};
	COORD RectSize{};
	std::unordered_map<long,const char*> clickedTextPos;
	bool VK_R, VK_L;
	void GotoXY(short x, short y) { SetConsoleCursorPosition(stdOutputHandle, COORD{ x,y }); }
	bool IsKeyDown(char vkey) { return (GetAsyncKeyState(vkey) & 0x8000) ? true : false; }
public:
	OCX() {
		DWORD mode;
		GetConsoleMode(stdInputHandle, &mode);
		mode &= ~ENABLE_QUICK_EDIT_MODE; //移除快速编辑模式
		mode &= ~ENABLE_INSERT_MODE; //移除插入模式
		mode &= ~ENABLE_MOUSE_INPUT;
		SetConsoleMode(stdInputHandle, mode);
		CONSOLE_CURSOR_INFO CCIMode;
		GetConsoleCursorInfo(stdOutputHandle, &CCIMode);//获取控制台光标信息
		CCIMode.bVisible = false;//隐藏控制台光标
		SetConsoleCursorInfo(stdOutputHandle, &CCIMode);//设置控制台光标状态
	}
	void GetCursorStdPos() {
		while (true) {
			GetCursorPos(&currentCursorPos);
			ScreenToClient(stdHwnd, &currentCursorPos);
			//x->col 0-15	y->row 0-7
			prevStdCursorPos = stdCursorPos;
			stdCursorPos = { currentCursorPos.x / 8,currentCursorPos.y / 16 };
			Sleep(20);
		}
	}
	void GetCurMouseSta(void) {
		while (true) {
			VK_R = IsKeyDown(0x02);//VK_RBUTTON或者MOUSE_EVENT
			VK_L = IsKeyDown(0x01);//或者MOUSE_MOVED
			Sleep(50);
		}
	};
	void DrawCursorPos() {
		while (true) {
			if (prevStdCursorPos.x != stdCursorPos.x || prevStdCursorPos.y != stdCursorPos.y) {
				auto it = clickedTextPos.find((prevStdCursorPos.x << 16) + prevStdCursorPos.y);
				if(it!=clickedTextPos.end()){ 
					GotoXY((short) prevStdCursorPos.x, (short) prevStdCursorPos.y);
					std::cout << it->second;
				} 
				it = clickedTextPos.find((stdCursorPos.x<<16)+ stdCursorPos.y);
				if (it!=clickedTextPos.end()) {
					GotoXY((short) stdCursorPos.x, (short) stdCursorPos.y);
					std::cout << "\033[0m\033[1;36m" << it->second << "\033[0m";
				} 
			}
			Sleep(5);
		}
	}
	void OCX_Register(long x, long y, const char* ch) { clickedTextPos.emplace((x << 16) + y, ch); }
	void OCX_Destroy(long x, long y) { clickedTextPos.erase((x << 16) + y); }
	void Init() {
		SetConsoleTitleW(TEXT("选择"));
		short RectChoice;
		std::cout << "Easy	Medium	Expert" << "\n";
		std::cout << "1		2		3" << "\n";
		std::cout << "Cin Your Choice ->";
		std::cin >> RectChoice;
		switch (RectChoice) {
		case 1:
			system("mode con cols=36 lines=18");
			RectSize = { 36,18 };
			break;
		case 2:
			system("mode con cols=64 lines=32");
			RectSize = { 64,32 };
			break;
		case 3:
			system("mode con cols=120 lines=32");
			RectSize = { 120,32 };
			break;
		default:
			std::cout << "\n\033[0m\033[1;31mInvalid Choice!\033[0m";
			system("pause");
			exit(0);
		}
		SetConsoleTitleW(TEXT("Gaming"));
	}
};

int main() {
	const char chs[10][2] = {"1","2","3","4","5","6","7","8","0"," " };
	OCX Console;
	Console.Init();
	Console.OCX_Register(0, 0, chs[0]);
	std::thread GETCURPOS(&OCX::GetCursorStdPos, &Console);
	std::thread REDRAWSCR(&OCX::DrawCursorPos, &Console);
	GETCURPOS.join();
	REDRAWSCR.join();
	system("pause");
}