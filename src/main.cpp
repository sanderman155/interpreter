#include "interpreter.hpp"

int main(int argc, char **argv) {
    std::string codeline;
    std::vector < std::vector <Lexem *> > infixLines;
    std::vector < std::vector <Lexem *> > postfixLines;
    try {
        if (argc < 4) {
            throw(ERR_WITH_START_PROGRAM);
        }
        setOutputSettings(argc, argv);
        while (getline(std::cin, codeline) && codeline != "quit")
            infixLines.push_back(parseLexem(codeline));
        initGlobalVarTable(infixLines);
        initjumps(infixLines);
        for (int row = 0; row < (int)infixLines.size(); row++)
            initLabels(infixLines[row], row);
        for (auto infix: infixLines)
            postfixLines.push_back(buildPostfix(infix));
        if (Ftable.find("main") == Ftable.end()) {
            throw(ERR_WITH_START);
        }
        int row = Ftable["main"], value;
        bool ret = false;
        while (0 <= row && row < (int)postfixLines.size()) {
            row = evaluatePostfix(postfixLines, row, value, ret, GlobalVarTable);
        }
        if (output_lvl >= 1) {
            print_all(postfixLines);
            std::cout << std::endl;
        }
        if (output_lvl >= 2) {
            print_all(infixLines);
            std::cout << std::endl;
            if (output_lvl == 4) {
                print_VarTables();
            }
            std::cout << std::endl << "Финальная таблица переменных" << std::endl;;
            print_map(GlobalVarTable);
        }
        for (int free = 0; free < (int)infixLines.size(); free++)
            del(infixLines[free]);
    } catch (ERRORS e) {
        std::cerr << ERRORTEXT[int(e)] << std::endl;
        for (int free = 0; free < (int)infixLines.size(); free++)
            del(infixLines[free]);
    }
    return 0;
}
