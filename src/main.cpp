#include "interpreter.hpp"

int main(int argc, char **argv) {
    std::string codeline;
    std::vector < std::vector <Lexem *> > infixLines;
    std::vector < std::vector <Lexem *> > postfixLines;
    try {
        if (argc < 4) {
            throw(ERR_WITH_START_PROGRAM);
        }
        set_output_settings(argc, argv);
        while (getline(std::cin, codeline) && codeline != "quit")
            infixLines.push_back(parse_lexem(codeline));
        init_global_var_table(infixLines);
        init_jumps(infixLines);
        for (int row = 0; row < (int)infixLines.size(); row++)
            init_labels(infixLines[row], row);
        for (auto infix: infixLines)
            postfixLines.push_back(build_postfix(infix));
        if (Ftable.find("main") == Ftable.end()) {
            throw(ERR_WITH_START);
        }
        int row = Ftable["main"], value;
        bool ret = false;
        while (0 <= row && row < (int)postfixLines.size()) {
            row = evaluate_postfix(postfixLines, row, value, ret, GlobalVarTable);
        }
        if (OutputLvl >= 1) {
            print_lexem_arrays(postfixLines);
            std::cout << std::endl;
        }
        if (OutputLvl >= 2) {
            print_lexem_arrays(infixLines);
            std::cout << std::endl;
            if (OutputLvl == 4) {
                print_var_tables();
            }
            std::cout << std::endl << "Global Variables Table" << std::endl;;
            print_map(GlobalVarTable);
        }
        for (int free = 0; free < (int)infixLines.size(); free++)
            clear_vector(infixLines[free]);
    } catch (ERRORS e) {
        std::cerr << ERRORTEXT[int(e)] << std::endl;
        for (int free = 0; free < (int)infixLines.size(); free++)
            clear_vector(infixLines[free]);
    }
    return 0;
}
