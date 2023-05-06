#include<iostream>
#include<fstream>
#include<string>
#include<stack>
#include<queue>
#include<unordered_map>
class RNP {/*全称逆波兰表达式*/
	std::queue<std::pair<char, long double>> _operandQueue;//操作数队列
	std::stack<char> _operatorStack;//操作符堆栈
	std::stack<long double> _calculateStack;//计算专用堆栈
	std::ofstream oputFile;
	std::ifstream iputFile;
	void GenerateRNP(std::string&);//创建逆波兰表达式
	void MovStackToQueue() {
		_operandQueue.push(std::make_pair(_operatorStack.top(), 0));
		_operatorStack.pop();
	}
	bool PushWhenEmpty(char it) {
		if (_operatorStack.empty() == true) {
			_operatorStack.push(it);
			return true;
		}return false;
	}
	static bool FindNextDigit(std::string::const_iterator iter, std::string& s) {
		bool isNotLBracket = true;
		for (++iter; iter != s.end(); ++iter) {
			switch (*iter) {
			case '(': isNotLBracket = false;
			case '+': if (isNotLBracket) return false;
			case '-': if (isNotLBracket) return false;
			default: if (isdigit(*iter)) return true;
			}
		}
		return false;
	}
public:
	std::unordered_map<std::string, int> _commandHash;
	std::unordered_map<char, int> _priority;
	std::unordered_map<char, int> _action;
	RNP() {//受VS2010影响，只能这样初始化了...
		_commandHash["cls"] = 0; _commandHash["save"] = 1; _commandHash["exit"] = 2; _commandHash["input"] = 3;
		_priority['+'] = 1; _priority['-'] = 1; _priority['('] = 1; _priority['*'] = 2; _priority['/'] = 2; _priority['%'] = 2; _priority['^'] = 2;
		_action['+'] = 1; _action['-'] = 2; _action['*'] = 3; _action['/'] = 4; _action['^'] = 5; _action['%'] = 6;
	}
	long double CalculateRNP(std::string&);//计算逆波兰表达式
	bool CheckVaildation(std::string&);//校验字符串合法性
	void OpenMode(std::string path,std::string curStr,long double it) {
		iputFile.open(path, std::ios_base::in);//一般直接文件拖拽
		if (iputFile.is_open() == false) {//OpenCheck
			std::cout << "Falied to open." << std::endl;
			return;
		}
		size_t pos = path.rfind('\\');
		path = path.substr(0, pos + 1);
		path.append("Output.txt");
		std::cout <<'\n' << "Valid Results:" << std::endl;
		std::cout << "#----BEGIN----#" << '\n';
		oputFile.open(path, std::ios_base::app);
		while (!iputFile.eof()) {
			std::getline(iputFile, curStr);
			if (Console.CheckVaildation(curStr)) {
				it = Console.CalculateRNP(curStr);
				oputFile << curStr << "=" << it << '\n';
				std::cout << curStr << " = " << it << '\n';
			} else std::cout << "Invalid Inputs!" << '\n';
		}
		std::cout << "#-----END-----#" << std::endl;
		iputFile.close();
		oputFile.close();
		system("pause");
	}
	void SaveRes(std::string prvStr,long double it) {
		oputFile.open("CalculateOutput.txt", std::ios_base::app);
		oputFile << prvStr << "=" << it << '\n';
		std::cout << "Saved." << '\n';
		oputFile.close();
	}
}Console;
bool RNP::CheckVaildation(std::string& s) {
	size_t lBracketNum = 0, rBracketNum = 0;
	bool foreDigit = false;
	for (std::string::const_iterator ptrCh = s.begin(); ptrCh != s.end(); ptrCh++) {
		switch (*ptrCh) {
		case '*': case '/':case '%':case '^':
			//如果是符号，则判断符号前后是否有数字
			if (foreDigit && FindNextDigit(ptrCh, s)) {
				foreDigit = false;//重置记录
			} else return false;//符号没有匹配数字直接返回
		case '+': case '-':
			if (!FindNextDigit(ptrCh, s)) return false;//符号没有匹配数字直接返回
		case ' ': case '.':		break;
		case '(':lBracketNum++; break;
		case ')':rBracketNum++; break;
		default:
			if (isdigit(*ptrCh)) {
				foreDigit = true;
				break;
			} else return false;//存在非法字符直接返回
		}
	}
	return lBracketNum == rBracketNum;//顺带检验括号数量是否匹配
}
void RNP::GenerateRNP(std::string& s) {
	std::string octaWord;//存储双精度浮点数对应字符串
	long double isPostitiveNum = 1;
	for (size_t low = 0, high = 0; low < s.length(); low++) {
		char it = s[low];/*优先级 ')' == '('#inPut  >> /' == ' * ' == '%' >> ' - ' == ' + ' >> '('#inStack*/
		/*如果运算符优先级>=运算符栈顶,则移动栈顶至队列直到栈顶priority<该运算符并压栈,反之直接压栈*/
		switch (it) {
		case '+':case '-'://pri == 1
			if (PushWhenEmpty(it)) break;//空栈直接push
			while (!_operatorStack.empty() && _operatorStack.top() != '(') MovStackToQueue();
			_operatorStack.push(it);
			break;
		case '%':case '*':case '/':case '^':// pri == 2
			if (PushWhenEmpty(it)) break;
			while (!_operatorStack.empty() && _priority.find(_operatorStack.top())->second== 2)	MovStackToQueue();
			_operatorStack.push(it);
			break;
		case '(':
			++low;
			while (s[low] == ' ') { ++low; }
			switch (s[low]) {
			case '-':isPostitiveNum = -1; break;
			case '+':isPostitiveNum = 1; break;
			case '(': default :--low;
			}
			_operatorStack.push(it);
			break;
		case ')':
			while (_operatorStack.top() != '(') MovStackToQueue();
			_operatorStack.pop();
			break;
		default:
			if (isdigit(it)) {//数字入队
				high = low;
				while (high < s.length() && (isdigit(s[high]) || s[high] == '.')) ++high;//寻找浮点数字尾
				octaWord = s.substr(low, high - low);//获取浮点数字符
				_operandQueue.push(std::make_pair('\0',atof(octaWord.c_str())*isPostitiveNum));
				low = high - 1;
				isPostitiveNum = 1;
				break;
			};
		}
	}
	while (_operatorStack.empty() != true) MovStackToQueue();
}
long double  RNP::CalculateRNP(std::string& s) {
	for (GenerateRNP(s); !_operandQueue.empty(); _operandQueue.pop()) {
		if (_operandQueue.front().first == '\0') {/*双精度浮点数项:*/
			_calculateStack.push(_operandQueue.front().second);
		} else {/*操作符项:*/
			auto num1 = _calculateStack.top(); _calculateStack.pop();
			auto num2 = _calculateStack.empty() ? 0 : _calculateStack.top();
			if (!_calculateStack.empty()) _calculateStack.pop();
			switch (_action.find(_operandQueue.front().first)->second) {
			case 1:_calculateStack.push(num2 + num1); break;
			case 2:_calculateStack.push(num2 - num1); break;
			case 3:_calculateStack.push(num2 * num1); break;
			case 4:_calculateStack.push(num2 / num1); break;
			case 5:_calculateStack.push(powl(num2,num1)); break;// ^
			case 6:_calculateStack.push((long long) num2 % (long long) num1); break;
			}
		}
	}
	long double ans = _calculateStack.top();
	_calculateStack.pop();
	return ans;
}
int main(int argc, char* argv[]) {
	long double it(0);
	std::string curStr;
	std::string prvStr;
	if (argc == 1) {
		while (true) {
			prvStr = curStr;
			std::cout << "Cin>> ";
			std::getline(std::cin, curStr);
			if (curStr[0] == '-'&&curStr[1] == '-') {
				std::string cmd = curStr.substr(2,curStr.size()-2);
				auto findResult = Console._commandHash.find(cmd);
				if (findResult != Console._commandHash.end()) {
					switch (findResult->second) {
					case 0:system("cls"); break;
					case 1:Console.SaveRes(prvStr, it); break;
					case 2:exit(0);
					case 3:
						std::cout << "PATH>>";
						std::getline(std::cin, curStr);
						Console.OpenMode(curStr.c_str(), curStr, it); break;
					}
				} else std::cout << "Invalid Command!" << '\n';
				continue;//如果为指令类，直接进入下一个循环中
			}
			if (curStr.empty()) continue;//空输入直接跳过
			if (Console.CheckVaildation(curStr)) {/*合法性校验通过*/
				it = Console.CalculateRNP(curStr);
				std::cout << "ANS>> " <<  it << '\n';
			} else std::cout << "Invalid Inputs!" << '\n';
		}
	} else Console.OpenMode(argv[1], curStr, it);
	system("pause");
	return 0;
} 