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
    std::queue<std::pair<char, long double>> queOper; //操作数队列
    std::stack<char> stkOper; //操作符堆栈
    std::stack<long double> stkCalc; //计算专用堆栈
    std::ofstream oputFile;
    std::ifstream iputFile;

    /**
     * \brief 创建逆波兰表达式
     * \param String 经过合法性检验后的表达式
     */
    void RPN_Generate(const std::string& String);

    void MovStackToQueue() {
        queOper.emplace(stkOper.top(), 0);
        stkOper.pop();
    }

    bool PushWhenEmpty(const char it) {
        if (stkOper.empty() == true) {
            stkOper.push(it);
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
    // 状态机状态定义
    // EXPECT_SIGN: 期待一元正负号或数字或小数点或左括号（操作数开始）
    // IN_INTEGER: 在整数部分（已读到数字）
    // IN_FRACTION: 在小数部分（已读到小数点）
    // EXPECT_OPERATOR: 期待运算符或右括号（操作数结束）
    enum class State : int8_t { EXPECT_SIGN, IN_INTEGER, IN_FRACTION, EXPECT_OPERATOR };
    State state = State::EXPECT_SIGN;
    int bracketNum = 0;
    
    for (const char c : String) {
        if (c == ' ') continue;
        
        switch (c) {
            case '(': 
                if (state != State::EXPECT_SIGN) return false;
                ++bracketNum;
                break;
                
            case ')':
                if (state != State::EXPECT_OPERATOR) return false;
                --bracketNum;
                if (bracketNum < 0) return false;
                break;
                
            case '+': case '-':
                if (state == State::EXPECT_SIGN) {
                    // 一元正负号，保持期待数字/小数点
                } else if (state == State::IN_INTEGER || state == State::IN_FRACTION || state == State::EXPECT_OPERATOR) {
                    // 二元运算符（在整数、小数或操作数结束后）
                    state = State::EXPECT_SIGN;
                } else {
                    return false;
                }
                break;
                
            case '*': case '/': case '%': case '^':
                if (state == State::IN_INTEGER || state == State::IN_FRACTION || state == State::EXPECT_OPERATOR) {
                    state = State::EXPECT_SIGN;
                } else {
                    return false;
                }
                break;
                
            case '.':
                if (state == State::EXPECT_SIGN) {
                    // .5 形式，小数点前无整数
                    state = State::IN_FRACTION;
                } else if (state == State::IN_INTEGER) {
                    // 0.5 形式，小数点前有整数
                    state = State::IN_FRACTION;
                } else {
                    // 其他状态（IN_FRACTION, EXPECT_OPERATOR）不允许小数点
                    return false;
                }
                break;
                
            default:
                if (std::isdigit(static_cast<unsigned char>(c))) {
                    if (state == State::EXPECT_SIGN) {
                        state = State::IN_INTEGER;
                    } else if (state == State::IN_FRACTION) {
                        // 小数部分读到数字，操作数有效但未结束
                        // 保持在 IN_FRACTION 或转到 EXPECT_OPERATOR？
                        // 0.5 后可能还有数字，也可能结束，所以设为 EXPECT_OPERATOR
                        // 但 0.5. 是非法的，所以 IN_FRACTION 后不能再有 .
                        state = State::EXPECT_OPERATOR;
                    }
                    // IN_INTEGER: 继续读整数，状态不变
                    // EXPECT_OPERATOR: 继续读数字（多位数），状态不变
                } else {
                    return false;
                }
                break;
        }
    }
    
    // 结束时必须是操作数结束状态且括号匹配
    // IN_FRACTION 也是合法的（如 "0." 或 "." 但 strtold 能处理）
    return (state == State::EXPECT_OPERATOR || state == State::IN_INTEGER || state == State::IN_FRACTION) 
           && (bracketNum == 0);
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
                while (!stkOper.empty() && stkOper.top() != '(') MovStackToQueue();
                stkOper.push(temp);
                break;
            case '%':
            case '*':
            case '/':
            case '^': // pri >= 2
                if (PushWhenEmpty(temp)) break;
                while (!stkOper.empty() && stkOper.top() != '(' && 
                       stkOper.top() != '+' && stkOper.top() != '-')
                    MovStackToQueue();
                stkOper.push(temp);
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
                stkOper.push(temp);
                break;
            case ')':
                while (stkOper.top() != '(') MovStackToQueue();
                stkOper.pop();
                break;
            default:
                if (isdigit(temp) || temp == '.') {
                    char* endptr;
                    queOper.emplace('\0', std::strtold(&String[low], &endptr) * isPostitiveNum);
                    low += static_cast<size_t>(endptr - &String[low]) - 1;
                    isPostitiveNum = 1;
                    break;
                }
        }
    }
    while (stkOper.empty() != true) MovStackToQueue();
}

long double RPN::RPN_Calculate(const std::string& String) {
    for (RPN_Generate(String); !queOper.empty(); queOper.pop()) {
        if (queOper.front().first == '\0') {
            stkCalc.push(queOper.front().second);
        } else {
            auto num1 = stkCalc.top();
            stkCalc.pop();
            auto num2 = stkCalc.empty() ? 0 : stkCalc.top();
            if (!stkCalc.empty()) stkCalc.pop();
            const char op = queOper.front().first;
            if (op == '+') stkCalc.push(num2 + num1);
            else if (op == '-') stkCalc.push(num2 - num1);
            else if (op == '*') stkCalc.push(num2 * num1);
            else if (op == '/') stkCalc.push(num2 / num1);
            else if (op == '^') stkCalc.push(std::pow(num2, num1));
            else if (op == '%') stkCalc.push(static_cast<long long>(num2) % static_cast<long long>(num1));
        }
    }
    const auto ans = stkCalc.top();
    stkCalc.pop();
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
