#include<iostream>
#include<fstream>
#include<string>
#include<stack>
#include<cmath>
#include<queue>
#include<cstdint>

#if __cplusplus >= 201103L

/*全称逆波兰表达式*/
class RPN {
    std::queue<std::pair<char, long double>> _operandQueue; //操作数队列
    std::stack<char> stack_Operator; //操作符堆栈
    std::stack<long double> stack_Calculate; //计算专用堆栈
    std::ofstream oputFile;
    std::ifstream iputFile;

    /**
     * \brief 创建逆波兰表达式
     * \param String 经过合法性检验后的表达式
     */
    void RPN_Generate(const std::string& String);

    void MovStackToQueue() {
        _operandQueue.emplace(stack_Operator.top(), 0);
        stack_Operator.pop();
    }

    bool PushWhenEmpty(const char it) {
        if (stack_Operator.empty() == true) {
            stack_Operator.push(it);
            return true;
        }
        return false;
    }

public:
    /**
     * \brief 计算逆波兰表达式
     * \param String 逆波兰表达式
     */
    long double RPN_Calculate(const std::string& String);

    /**
    * \brief 校验字符串合法性
    * \param String 预处理的字符串
    */
    static bool Vaild_Check(const std::string& String);

    /**
    * \brief 打开防止计算表达式的文件
    * \param path 路径
    */
    long double OpenMode(std::string path);

    void SaveRes(const std::string& prvStr, const long double it) {
        oputFile.open("Output.txt", std::ios_base::app);
        oputFile << prvStr << "=" << it << '\n';
        std::cout << "Saved.\n";
        oputFile.close();
    }
} Console;

bool RPN::Vaild_Check(const std::string& String) {
    enum {
        BEGIN = -1,
        NUM,
        SIGNAL,
    };
    int bracketNum = 0;
    int statusCode = BEGIN;
    for (const char pChar: String) {
        switch (pChar) {
            case '*':
            case '/':
            case '%':
            case '^':
                if (statusCode != NUM) return false;
                statusCode = SIGNAL;
                break;
            case '+':
            case '-':
                if (statusCode == NUM || statusCode == BEGIN) statusCode = SIGNAL;
                else return false;
            case ' ':
            case '.': break;
            case '(': ++bracketNum;
                break;
            case ')':
                if (statusCode == SIGNAL) return false;
                --bracketNum;
                break;
            default:
                if (isdigit(pChar)) {
                    statusCode = NUM;
                    break;
                } else return false;
        }
        if (bracketNum < 0) return false;
    }
    return (bracketNum == 0) && statusCode != SIGNAL;
}

void RPN::RPN_Generate(const std::string& String) {
    long double isPostitiveNum = 1;
    for (size_t low = 0; low < String.length(); low++) {
        /*优先级 ')' == '('#inPut  >> /' == ' * ' == '%' >> ' - ' == ' + ' >> '('#inStack*/
        char temp = String[low];
        /*如果运算符优先级>=运算符栈顶,则移动栈顶至队列直到栈顶priority<该运算符并压栈,反之直接压栈*/
        switch (String[low]) {
            case '+':
            case '-': //pri == 1
                if (PushWhenEmpty(temp)) break; //空栈直接push
                while (!stack_Operator.empty() && stack_Operator.top() != '(') MovStackToQueue();
                stack_Operator.push(temp);
                break;
            case '%':
            case '*':
            case '/':
            case '^': // pri >= 2
                if (PushWhenEmpty(temp)) break;
                while (!stack_Operator.empty() && stack_Operator.top() != '(' && 
                       stack_Operator.top() != '+' && stack_Operator.top() != '-')
                    MovStackToQueue();
                stack_Operator.push(temp);
                break;
            case '(':
                do {
                    ++low;
                } while (String[low] == ' ');
                switch (String[low]) {
                    case '-': isPostitiveNum = -1;
                        break;
                    case '+': isPostitiveNum = 1;
                        break;
                    case '(': default: --low;
                }
                stack_Operator.push(temp);
                break;
            case ')':
                while (stack_Operator.top() != '(') MovStackToQueue();
                stack_Operator.pop();
                break;
            default:
                if (isdigit(temp)) {
                    char* endptr;
                    _operandQueue.emplace('\0', strtold(&String[low], &endptr) * isPostitiveNum);
                    low += endptr - &String[low] - 1;
                    isPostitiveNum = 1;
                    break;
                }
        }
    }
    while (stack_Operator.empty() != true) MovStackToQueue();
}

long double RPN::RPN_Calculate(const std::string& String) {
    for (RPN_Generate(String); !_operandQueue.empty(); _operandQueue.pop()) {
        if (_operandQueue.front().first == '\0') {
            stack_Calculate.push(_operandQueue.front().second);
        } else {
            auto num1 = stack_Calculate.top();
            stack_Calculate.pop();
            auto num2 = stack_Calculate.empty() ? 0 : stack_Calculate.top();
            if (!stack_Calculate.empty()) stack_Calculate.pop();
            const char op = _operandQueue.front().first;
            if (op == '+') stack_Calculate.push(num2 + num1);
            else if (op == '-') stack_Calculate.push(num2 - num1);
            else if (op == '*') stack_Calculate.push(num2 * num1);
            else if (op == '/') stack_Calculate.push(num2 / num1);
            else if (op == '^') stack_Calculate.push(std::pow(num2, num1));
            else if (op == '%') stack_Calculate.push(static_cast<long long>(num2) % static_cast<long long>(num1));
        }
    }
    const auto ans = stack_Calculate.top();
    stack_Calculate.pop();
    return ans;
}

long double RPN::OpenMode(std::string path) {
    long double result = 0;
    iputFile.open(path, std::ios_base::in);
    if (!iputFile.is_open()) {
        std::cout << "Failed to open.\n";
        std::system("pause");
        return 0;
    }
    const size_t pos = path.rfind('\\');
    if (pos != std::string::npos) {
        path = path.substr(0, pos + 1);
        path.append("Output.txt");
    }
    std::cout << "\nValid Results:\n#----BEGIN----#\n";
    oputFile.open(path, std::ios_base::app);
    std::string curStr;
    while (std::getline(iputFile, curStr)) {
        if (Vaild_Check(curStr)) {
            result = RPN_Calculate(curStr);
            oputFile << curStr << "=" << result << '\n';
            std::cout << curStr << " = " << result << '\n';
        } else {
            std::cout << "Invalid Inputs!\n";
        }
    }
    std::cout << "#-----END-----#\n";
    iputFile.close();
    oputFile.close();
    std::system("pause");
    return result;
}

// 命令解析：使用编译期字符串比较
enum class CmdType : uint8_t { Unknown, Cls, Save, Exit, Input };

constexpr CmdType ParseCommand(std::string_view cmd) {
    // C++20 可用 <=> 运算符，C++17 用三元链式比较
    return (cmd == "cls")   ? CmdType::Cls :
           (cmd == "save")  ? CmdType::Save :
           (cmd == "exit")  ? CmdType::Exit :
           (cmd == "input") ? CmdType::Input :
                              CmdType::Unknown;
}

int main(const int argc, char* argv[]) {
    if (argc == 1) {
        std::string curStr;
        std::string prvStr;
        long double it(0);
        while (true) {
            prvStr = curStr;
            std::cout << "Cin>> ";
            std::getline(std::cin, curStr);
            if (curStr.empty()) continue;
            
            // 使用string_view避免拷贝，直接解析命令
            if (curStr.size() >= 2 && curStr[0] == '-' && curStr[1] == '-') {
                const std::string_view cmd_view(curStr.c_str() + 2, curStr.size() - 2);
                switch (ParseCommand(cmd_view)) {
                    case CmdType::Cls: std::system("cls"); break;
                    case CmdType::Save: Console.SaveRes(prvStr, it); break;
                    case CmdType::Exit: return 0;
                    case CmdType::Input:
                        std::cout << "PATH>>";
                        std::getline(std::cin, curStr);
                        it = Console.OpenMode(curStr);
                        break;
                    default: std::cout << "Invalid Command!\n"; break;
                }
                continue;
            }
            
            if (RPN::Vaild_Check(curStr)) {
                it = Console.RPN_Calculate(curStr);
                std::cout << "ANS>> " << it << '\n';
            } else {
                std::cout << "Invalid Inputs!\n";
                curStr.clear(); // 错误时清空，避免prvStr保存无效表达式
            }
        }
    } else {
        Console.OpenMode(argv[1]);
    }
    return 0;
}
#else
int main() {
    std::cout << "ONLY SUPPORT C++11 C++14 C++17 ...\n";
    std::system("pause");
    return 0;
}
#endif
