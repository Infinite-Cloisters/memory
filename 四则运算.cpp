#include<iostream>
#include<fstream>
#include<string>
#include<stack>
#include<queue>
class RNP {/*全称逆波兰表达式*/
	struct MixedData {
		char _op;//为NULL=>读取操作数
		long double _num;
	};
	std::queue<MixedData> _operandQueue;//操作数队列
	std::stack<char> _operatorStack;//操作符堆栈
	std::stack<long double> _calcuteStack;//计算专用堆栈
	void GenerateRNP(std::string&);//创建逆波兰表达式
	void MovStackToQueue(MixedData& tmp) {
		tmp._op = _operatorStack.top();
		_operandQueue.push(tmp);
		_operatorStack.pop();
	}
	bool PushWhenEmpty(char it) {
		if (_operatorStack.empty() == true) {
			_operatorStack.push(it);
			return true;
		}return false;
	}
	bool FindNextDigit(std::string::const_iterator iter, std::string& s) {
		bool isNotLBracket = true;
		for (iter++; iter != s.end();iter++) {
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
	long double CalculateRNP(std::string&);//计算逆波兰表达式
	bool CheckVaildation(std::string&);//校验字符串合法性
}Console;
bool RNP::CheckVaildation(std::string& s) {
	size_t lBracketNum = 0, rBracketNum = 0;
	bool foreDigit = false;
	for (std::string::const_iterator ptrCh = s.begin(); ptrCh != s.end(); ptrCh++) {
		switch (*ptrCh) {
		case '*': case '/':case '%':
			//如果是符号，则判断符号前后是否有数字
			if (foreDigit && FindNextDigit(ptrCh, s)) {
				foreDigit = false;//重置记录
			} else return false;//符号没有匹配数字直接返回
		case '+': case '-':
			if (!FindNextDigit(ptrCh, s)) return false;//符号没有匹配数字直接返回
		case ' ': case '.':
			break;
		case '(':lBracketNum++; break;
		case ')':rBracketNum++; break;
		default:
			if (isdigit(*ptrCh)) {
				foreDigit = true;
				break;
			} else return false;//存在非法字符直接返回
		}
	}
	return lBracketNum == rBracketNum ? true : false;//顺带检验括号数量是否匹配
}
void RNP::GenerateRNP(std::string& s) {
	std::string octaWord;//存储双精度浮点数对应字符串
	long double isPostitiveNum = 1;
	MixedData tmp = { '\0',0 };//作为压栈辅助用结构体
	for (size_t low = 0, high = 0; low < s.length(); low++) {
		auto it = s[low];
		/*优先级 ')' == '('#inPut  >> /' == ' * ' == '%' >> ' - ' == ' + ' >> '('#inStack*/
		/*如果 运算符优先级大于等于运算符栈顶 则移动栈顶至队列 直到 栈顶优先级小于 该运算符 并压栈*/
		/*反之直接压栈*/
		switch (it) {
		case '+':case '-':
			if (PushWhenEmpty(it)) break;//空栈直接push
			while (_operatorStack.empty() != true && _operatorStack.top() != '(') {
				MovStackToQueue(tmp);
			}
			_operatorStack.push(it);
			break;
		case '%':case '*':case '/':
			if (PushWhenEmpty(it)) break;
			while (_operatorStack.empty() != true &&
				(_operatorStack.top() == '%' || _operatorStack.top() == '*' || _operatorStack.top() == '/')
				) {
				MovStackToQueue(tmp);
			}
			_operatorStack.push(it);
			break;
		case '(':
			while (s[low] == ' ' || s[low] == '(') { low++; }
			if (s[low] == '-') isPostitiveNum = -1;
			else if (s[low] == '+') isPostitiveNum = 1;
			else low--;
			_operatorStack.push(it);
			break;
		case ')'://kazaizhelile
			while (_operatorStack.top() != '(') {
				MovStackToQueue(tmp);
			}
			_operatorStack.pop();
			break;
		default:
			if (isdigit(it)) {//数字入队
				high = low;
				//寻找浮点数字尾
				while (high < s.length() && (isdigit(s[high]) || s[high] == '.')) {
					high++;
				}
				octaWord = s.substr(low, high - low);//获取浮点数字符
				tmp._op = '\0';
				tmp._num = atof(octaWord.c_str());
				tmp._num *= isPostitiveNum;
				_operandQueue.push(tmp);
				low = high - 1;
				isPostitiveNum = 1;
				break;
			} else break;
		}
	}
	while (_operatorStack.empty() != true) MovStackToQueue(tmp);
}
long double  RNP::CalculateRNP(std::string& s) {
	std::cout << "RNP>> ";
	for (GenerateRNP(s); _operandQueue.empty() != true; _operandQueue.pop()) {
		if (_operandQueue.front()._op == '\0') {/*双精度浮点数项:*/
			std::cout << _operandQueue.front()._num << ' ';
			_calcuteStack.push(_operandQueue.front()._num);
		} else {/*操作符项:*/
			std::cout << _operandQueue.front()._op << ' ';//debug用
			auto num1 = _calcuteStack.top(); _calcuteStack.pop();
			auto num2 = _calcuteStack.empty() ? 0 : _calcuteStack.top();
			if (!_calcuteStack.empty()) _calcuteStack.pop();
			switch (_operandQueue.front()._op) {
			case '+':_calcuteStack.push(num2 + num1); break;
			case '-':_calcuteStack.push(num2 - num1); break;
			case '*':_calcuteStack.push(num2 * num1); break;
			case '/':_calcuteStack.push(num2 / num1); break;
			case '%':_calcuteStack.push((long long) num2 % (long long) num1); break;
			}
		}
	}
	std::cout << '\n';
	auto ans = _calcuteStack.top();
	_calcuteStack.pop();
	return ans;
}
signed main(int argc, char* argv[]) {
	long double it(0);
	std::string curStr;
	std::string prvStr;
	std::ofstream oputFile;
	std::ifstream iputFile;
	if (argc == 1) {
		while (true) {
			prvStr = curStr;
			std::cout << "Cin>> ";
			std::getline(std::cin, curStr);
			if (curStr[0] == '/') {
				switch (curStr[1]) {
				case 'c'://cls
					system("cls");
					break;
				case 's'://save
					oputFile.open("CalculateOutput.txt", std::ios_base::app);
					oputFile << prvStr << "=" << it << '\n';
					std::cout << "Saved." << '\n';
					oputFile.close();
					break;
				case 'e'://exit
					oputFile.close();
					exit(0);
				}
				continue;//如果为指令类，直接进入下一个循环中
			}
			if (curStr == "") continue;//空输入直接跳过
			if (Console.CheckVaildation(curStr) == true) {/*合法性校验通过*/
				it = Console.CalculateRNP(curStr);
				std::cout << "ANS>> " << curStr << " = " << it << '\n';
			} else std::cout << "Invalid Inputs!" << '\n';
		}
	} else {
		iputFile.open(argv[1], std::ios_base::in);//直接文件拖拽
		std::cout << "Valid Results:" << std::endl;
		std::cout << "#----BEGIN----#" << '\n';
		oputFile.open("CalculateOutput.txt", std::ios_base::out);
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
	return 0;
}

//2 3 2 * * 4 *