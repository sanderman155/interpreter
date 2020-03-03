#include "interpreter.hpp"

int main() {
    std::string codeline;
    std::vector < std::vector <Lexem *> > infixLines;
    std::vector < std::vector <Lexem *> > postfixLines;
    try {
        while (getline(std::cin, codeline) && codeline != "quit")
            infixLines.push_back(parseLexem(codeline));
        print_all(infixLines);
        initGlobalVarTable(infixLines);
        initjumps(infixLines);
        for (int row = 0; row < (int)infixLines.size(); row++)
            initLabels(infixLines[row], row);
        for (auto infix: infixLines)
            postfixLines.push_back(buildPostfix(infix));
        std::cout << std::endl;
        print_all(postfixLines);
        std::cout << std::endl;
        if (Ftable.find("main") == Ftable.end()) {
            throw(ERR_WITH_START);
        }
        int row = Ftable["main"], value;
        bool ret = false;
        while (0 <= row && row < (int)postfixLines.size()) {
            row = evaluatePostfix(postfixLines, row, value, ret, GlobalVarTable);
        }
        std::cout << std::endl << "Финальная таблица переменных" << std::endl;;
        print_map(GlobalVarTable);
        for (int free = 0; free < (int)infixLines.size(); free++)
            del(infixLines[free]);
    } catch (ERRORS e) {
        std::cerr << ERRORTEXT[int(e)] << std::endl;
        for (int free = 0; free < (int)infixLines.size(); free++)
            del(infixLines[free]);
    }
    return 0;
}
