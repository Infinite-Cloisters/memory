#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define InputError (-1)

const char numLists[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

int UI_Main();

void UI_Branch_1();//Set operations
void UI_Branch_2();//Arithmetic evaluation
void UI_Branch_3();//Base conversion

int main() {
    while (true) {
        switch (UI_Main()) {
            case 0:
                system("pause");
                exit(0);
            case 1:
                UI_Branch_1();
                break;
            case 2:
                UI_Branch_2();
                break;
            case 3:
                UI_Branch_3();
                break;
            default:
                break;
        }
    }
}

//<进制转换>
void reverse(char obj[], int len) {
    char *back = obj + len - 1;
    char tmp;
    while (back >= obj) {
        tmp = *obj;
        *obj = *back;
        *back = tmp;
        --back, ++obj;
    }
}

int toNum(const char obj[], int type) {
    int ret = 0;
    while (isalnum(*obj)) {
        ret *= type;
        int plus = (int) *obj;
        if (plus >= (int) 'a') plus -= 'a' - 10;
        else if (plus >= (int) 'A') plus -= 'A' - 10;
        else plus -= '0';
        ret += plus;
        ++obj;
    }
    return ret;
}

int Convert(char origin[], int originType, int targetType, char result[]) {
    if (originType != 10 && targetType != 10) return InputError;
    int num = toNum(origin, originType);
    int n = 0;
    for (; num != 0; num /= targetType, ++n)
        result[n] = numLists[num % targetType];
    result[n] = '\0';
    reverse(result, n);
    return 0;
}
//</进制转换>
//<计算器>
/*不是C++我很难办啊*/
int FindLastBracket(const char expression[]) {
    int pBracketL = -1, pBracketR;//小括号位置寄存
    for (pBracketR = 0; expression[pBracketR] != '\0'; ++pBracketR)
        if (expression[pBracketR] == '(') pBracketL = pBracketR;
    return pBracketL;
}

void GetLRNum(int *pNumL, int *pNumR, int *numL, int *numR, int ptr, char expression[]) {
    *pNumR = ptr - 1;
    while (isdigit(expression[*pNumR]) == false) --*pNumR;
    *pNumL = *pNumR - 1;
    while (isdigit(expression[*pNumL]) == true) --*pNumL;
    *numL = toNum(&expression[++*pNumL], 10);
    *pNumR = ptr + 1;
    while (isdigit(expression[*pNumR]) == false) ++*pNumR;
    *numR = toNum(&expression[*pNumR], 10);
}

void TieNum(int *num, int *pNumL, const int *pNumR, char expression[]) {
    int len = 0;
    for (; *num != 0; *num /= 10, ++len) expression[*pNumL + len] = (*num) % 10 + '0';
    reverse(&expression[*pNumL], len);
    for (*pNumL += len; *pNumL < *pNumR; ++*pNumL) expression[*pNumL] = ' ';
    for (; isdigit(expression[*pNumL]); ++*pNumL) expression[*pNumL] = ' ';
}

void GetNum(int pos, char expression[]) {
    ++pos;
    int pNumL, pNumR, numL, numR, ptr = pos;
    for (; expression[ptr] && expression[ptr] != ')'; ++ptr)
        if (expression[ptr] == '*') {
            GetLRNum(&pNumL, &pNumR, &numL, &numR, ptr, expression);
            int num = numL * numR;
            TieNum(&num, &pNumL, &pNumR, expression);
        } else if (expression[ptr] == '/') {
            GetLRNum(&pNumL, &pNumR, &numL, &numR, ptr, expression);
            int num = numL / numR;
            TieNum(&num, &pNumL, &pNumR, expression);
        }
    for (ptr = pos; expression[ptr] && expression[ptr] != ')'; ++ptr)
        if (expression[ptr] == '+') {
            GetLRNum(&pNumL, &pNumR, &numL, &numR, ptr, expression);
            int num = numL + numR;
            TieNum(&num, &pNumL, &pNumR, expression);
        } else if (expression[ptr] == '-') {
            GetLRNum(&pNumL, &pNumR, &numL, &numR, ptr, expression);
            int num = numL - numR;
            TieNum(&num, &pNumL, &pNumR, expression);
        }
    if (expression[ptr] == ')') expression[ptr] = ' ';
}

int Calculate(char expression[]) {
    int bracketPos;
    do {
        bracketPos = FindLastBracket(expression);
        GetNum(bracketPos, expression);
        if (bracketPos != -1) expression[bracketPos] = ' ';
        int l = 0;
        while (expression[l] != ' ' && expression[l] != '\0') ++l;
        for (int r = l + 1; expression[r] != '\0'; ++r)
            if (expression[r] != ' ') {
                expression[l++] = expression[r];
                expression[r] = ' ';
            }
    } while (bracketPos != -1);
    int l = 0;
    while (expression[l] != ' ' && expression[l] != '\0') ++l;
    for (int r = l + 1; expression[r] != '\0'; ++r)
        if (expression[r] != ' ') {
            expression[l++] = expression[r];
            expression[r] = ' ';
        }
    return toNum(expression, 10);
}

//</计算器>
//<Set>
typedef struct SET {
    int l, r;
    bool isLC, isRC;//是否包含边界
} set;

void InitSet(set *s, char buff[]) {
    char *ptr = buff;
    while (*ptr != '(' && *ptr != '[') ++ptr;//寻找左括号
    if (*ptr == '(') s->isLC = false;
    else s->isLC = true;
    while (!isdigit(*ptr)) ++ptr;//寻找数字
    s->l = toNum(ptr, 10);
    while (*ptr != ',') ++ptr;
    while (!isdigit(*ptr)) ++ptr;
    s->r = toNum(ptr, 10);
    while (*ptr != ')' && *ptr != ']') ++ptr;//寻找右括号
    if (*ptr == ')') s->isRC = false;
    else s->isRC = true;
}

void PrintSet(set *s) {
    if (s->isLC) printf_s("[");
    else printf_s("(");
    printf_s("%d,%d", (s->l), (s->r));
    if (s->isRC) printf_s("]");
    else printf_s(")");
}

void Intersect(set *a, set *b) {//交
    printf_s("Intersect>>");
    if (a->r <= b->l) {
        if (a->r == b->l && a->isRC && b->isLC) printf_s("%d\n", (a->r));
        else printf_s("Null Set\n");
        return;
    }
    set c;
    c.l = b->l;
    if (b->l == a->l) c.isLC = b->isLC && a->isLC;
    else c.isLC = b->isLC;
    if (a->r <= b->r) {
        if (a->isRC && b->isRC) c.isRC = true;
        else c.isRC = false;
        c.r = a->r;
    } else {
        c.r = b->r;
        c.isRC = b->isRC;
    }
    PrintSet(&c);
    printf_s("\n");
}

void Union(set *a, set *b) {//并
    printf_s("Union>>");
    set c;
    if (a->r <= b->l) {
        if (a->r == b->l && a->isRC && b->isLC) {
            c.l = a->l;
            c.isLC = a->isLC;
            c.r = b->r;
            c.isRC = b->isRC;
            PrintSet(&c);
            printf_s("\n");
        } else {
            PrintSet(a);
            printf_s("U");
            PrintSet(b);
            printf_s("\n");
        }
        return;
    }
    c.l = a->l;
    if (a->l == b->l) c.isLC = (a->isLC || b->isLC);
    else c.isLC = a->isLC;
    if (a->r < b->r) {
        c.r = b->r;
        c.isRC = b->isRC;
    } else {
        c.r = a->r;
        c.isRC = a->isRC;
    }
    PrintSet(&c);
    printf_s("\n");
}

void Except(set *a, set *b) {
    printf_s("Except>>");
    bool print = false;
    bool flag = false;
    if (a->r <= b->l) {
        if (a->r == b->l && a->isRC && b->isLC) {
            a->isRC = false;
            b->isLC = false;
        }
        PrintSet(a);
        printf_s("U");
        PrintSet(b);
        printf_s("\n");
        return;
    }
    set c;
    if (a->l == b->l) {
        if (a->isLC != b->isLC) {
            printf_s("{%d}U", a->l);
            print = true;
        }
    } else {
        c.l = a->l;
        c.isLC = a->isLC;
        c.r = b->l;
        c.isRC = !(b->isRC);
        PrintSet(&c);
        print = true;
        flag = true;
    }
    if (a->r > b->r) {
        c.l = b->r;
        c.isLC = !(b->isRC);
        c.r = a->r;
        c.isRC = a->isRC;
        if (flag) printf_s("U");
        PrintSet(&c);
        print = true;
    } else if (a->r == b->r) {
        if (a->isRC != b->isRC) {
            if (flag) printf_s("U");
            printf_s("{%d}", a->r);
            print = true;
        }
    } else {
        c.l = a->r;
        c.isLC = !(a->isRC);
        c.r = b->r;
        c.isRC = b->isRC;
        if (print) printf_s("U");
        PrintSet(&c);
        print = true;
    }
    if (!print) printf_s("Null Set");
    printf_s("\n");
}

int ReArrange(char buff1[], char buff2[]) {
    set a, b;
    InitSet(&a, buff1);
    InitSet(&b, buff2);
    if (a.l > a.r || b.l > b.r) return InputError;
    if (a.l > b.l) {//swap
        set c = a;
        a = b;
        b = c;
    }
    Intersect(&a, &b);
    Union(&a, &b);
    Except(&a, &b);
    system("pause");
    system("cls");
    return 0;
}

//</Set>
//<UI>
int UI_Main() {
    char ret[32] = {'\0'};
    printf_s("   >>Please Make The Choice\n"
             " 1 >>Set operations\n"
             " 2 >>Arithmetic evaluation\n"
             " 3 >>Base conversion\n"
             " 0 >>Exit\n");
    while (true) {
        printf_s("INP>>");
        fflush(stdin);
        scanf_s("%[^\n]", &ret);
        if (ret[0] >= '0' && ret[0] <= '3') break;
        else while (getchar() != '\n');
    }
    return ret[0] - '0';
}

void UI_Branch_1() {
    system("cls");
    char buff1[32], buff2[32];
    printf_s("The First Set:\tEXAMPLE: [1,2)\n>>");
    fflush(stdin);
    getchar();
    scanf_s("%[^\n]", &buff1);
    printf_s("The Second Set:\n>>");
    fflush(stdin);
    getchar();
    scanf_s("%[^\n]", &buff2);
    ReArrange(buff1, buff2);
}

void UI_Branch_2() {
    system("cls");
    char buff[64] = {'\0'};
    printf_s("Please Cin Arithmetic evaluation\n\n>>");
    fflush(stdin);
    getchar();
    scanf_s("%[^\n]", &buff);
    int res = Calculate(buff);
    printf_s("The Result Is %d\n", res);
    system("pause");
    system("cls");
}

void UI_Branch_3() {
    char nums[16], result[32];
    int oriT, tarT;
    system("cls");
    printf_s("Please Cin The Num You Wanna Convert\n\n>>");
    scanf_s("%s", nums);
    printf_s("Please Cin OriginType\n>>");
    scanf_s("%d", &oriT);
    printf_s("Please Cin TargetType\n>>");
    scanf_s("%d", &tarT);
    if (Convert(nums, oriT, tarT, result) != InputError) printf_s("The Result Is %s\n", result);
    else printf_s("MSG>>Unvalidated Input!\n");
    system("pause");
    system("cls");
}
//</UI>