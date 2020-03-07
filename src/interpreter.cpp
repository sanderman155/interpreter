#include "interpreter.hpp"

std::string ERRORTEXT[] = {
    "incorrect start of the program",
    "entered unexisted operator",
    "error with label",
    "need to be alternation between operator and Variables or Numbers",
    "brackets are not balansed",
    "Conditional or service operators are not balansed",
    "bad function name declartion after DEF",
    "error with declaration of start variables function",
    "undefined reference to function",
    "придумать текст про if/while then else",
    "program need function main",
    "problems with operator assign(:=)",
    "work with undefined Variable",
    "runtime error?!"
};

int PRIORITY[] = {
    -1,
    -1, -1, -1,
    -1, -1,
    -1, -1,
    -1, -1,
    -1, -1, -1,
    0,
    1,
    2,
    3,
    4,
    5, 5,
    7, 7,
    6, 6, 6, 6,
    8, 8,
    9, 9, 9,
    10, 10
};

std::string OPERTEXT [] = {
    ",",
    "def", "ret", "enddef",
    "if", "then",
    "else", "endif",
    "while", "endwhile",
    "goto", ":=", ":",
    "or",
    "and",
    "|",
    "^",
    "&",
    "==", "!=",
    "<<", ">>",
    "<=", "<", ">=", ">",
    "+", "-",
    "*", "/", "%",
    "(", ")"
};

std::string tab = "";
std::unordered_map <std::string, int> GlobalVarTable;
std::map <std::string, int> Ftable;
std::map <std::string, std::unordered_map <std::string, int>> FstartVars;
std::map <std::string, int> Labels;
std::stack <int> RetAddr;
std::stack<std::unordered_map <std::string, int>> LocTables; //???
std::stack<std::string> tabStack;
int output_lvl;

Oper::Oper(int n) {
        setType(OPERATION); //tab
        out = OPERTEXT[n];
        opertype = (OPERATOR)n;
}

int Oper::getPriority() {
    return PRIORITY[opertype];
}

Variable::Variable(std::string n, int v) {
    name = n;
    value = v;
    GlobalVarTable[name] = value;
}

void Variable::initVar() {
    GlobalVarTable[name] = UNDEFINED;
}

int Variable::getValue(std::unordered_map <std::string, int> VarTable) {
    if(exist(VarTable)) {
        value = VarTable[name];
        return VarTable[name];
    } else if (exist(GlobalVarTable)) {
        value = GlobalVarTable[name];
        return GlobalVarTable[name];
    } else {
        return UNDEFINED;
    }
}

void Goto::setRow(const std::string &labelname) {
    row = Labels[labelname];
}

void Goto::print() {
    std::cout << "[" << OPERTEXT[operation] << " -> " << "row = " << row << "]";
}

int Foo::getRow() { //Foo ???
    if (Ftable.find(name) != Ftable.end()) {
        return Ftable[name];
    } else {
        return -1;
    }
}

void print_VarTables() { //case print_var_tables() VarTable
    while(!LocTables.empty()) {
        tab = tabStack.top();
        tabStack.pop();
        print_map(LocTables.top());
        LocTables.pop();
        std::cout << std::endl;
    }
}

void print(std::vector <Lexem *> array) { //print_lexem_array()
    for (auto ptr: array) {
        if (ptr == nullptr) {
            continue;
        }
        if (ptr -> getType() == OPERATION &&
        ((Oper *)ptr) -> get_type() == GOTO) {
        } else {
            ptr -> print();
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}

void print_all(std::vector <std::vector <Lexem *>> matrix) { // print_lexem_arrays()
    for (int free = 0; free < (int)matrix.size(); free++) {
        std::cout << free << ": ";
        print(matrix[free]);
    }
}


void print_map(std::unordered_map <std::string, int> &VarTable) {
    for(std::unordered_map<std::string,int>::iterator it = VarTable.begin();
    it != VarTable.end(); it++) {
        std::cout << tab << it -> first << " = " << it -> second << std::endl; //it->first
    }
}

void del(std::vector <Lexem *> v) { //clear_vector
    for(auto ptr: v) {
        delete ptr;
    }
}

void setOutputSettings(int argc, char **argv) { //set_output...
    int i;
    for(i = 0; i < argc; i++) {
        std::string check = std::string(argv[i]);
        if(check == "output") {
            if (argv[i + 1][0] != '=') {
                throw(ERR_WITH_START_PROGRAM);
            }
            output_lvl = atoi(argv[i + 2]);
            if (output_lvl > 4) {
                throw(ERR_WITH_START_PROGRAM);
            }
            break;
        }
    }
    if(i == argc) {
        throw(ERR_WITH_START_PROGRAM);
    }
}
Lexem *get_number(std::string codeline, std::string::iterator &ptr) { 
    if(!isdigit(*ptr) || isspace(*ptr)) {
        return nullptr;
    }
    int value = 0;
    while ((ptr != codeline.end()) && isdigit(*ptr)) {
        value *= 10;
        value += (*ptr) - '0';
        ptr++;
    }
    Lexem *num = new Number(value);
    return num;
}

Lexem *get_operator(std::string codeline, std::string::iterator &ptr,
                    int counter) {
    if(isdigit(*ptr) || isspace(*ptr)) {
        return nullptr;
    }
    int n = sizeof(OPERTEXT) / sizeof(std::string);
    int op;
    for (op = 0; op < n; op++) {
        int len = OPERTEXT[op].size();
        if (OPERTEXT[op] == codeline.substr(counter, len)) {
            ptr += OPERTEXT[op].size();
            counter += OPERTEXT[op].size();
            if(op == GOTO || op == IF || op == ELSE ||
                op == WHILE || op == ENDWHILE || op == RET) {
                return new Goto(op);
            } else {
                return new Oper(op);
            }
            break;
        }
    }
    if (op == n && !isalpha(*ptr)) {
        throw(ERR_UNDEFINED_OPERATOR);
    }
    return nullptr;
}

Lexem *get_variable(std::string codeline, std::string::iterator &ptr) {
    if(!isalpha(*ptr) || isspace(*ptr)) {
        return nullptr;
    }
    Lexem* var;
    std::string name;
    while ((ptr != codeline.end()) && isalpha(*ptr)) {
        name += *ptr;
        ptr++;
    }
    var = new Variable(name);
    return var;
}

Lexem *get_label(std::string codeline, std::string::iterator &ptr) {
    while(isspace(*ptr)) {
        ptr++;
    }
    if(!isalpha(*ptr)) {
        return nullptr;
    }
    Lexem* var;
    std::string name;
    while ((ptr != codeline.end()) && isalpha(*ptr)) {
        name += *ptr;
        ptr++;
    }
    if(GlobalVarTable.find(name) != GlobalVarTable.end()) {
        GlobalVarTable.erase(name);
    }
    var = new Label(name);
    return var;
}

std::vector <Lexem *> parseLexem(std::string codeline) { //parse_lexem()
    std::string var;
    Lexem *ptr;
    std::vector <Lexem *> infix;
    int alternation = 2;//start case
    std::stack <bool> bracket;
//    bool lbracket = false;
    try {
        for(auto pointer = codeline.begin(); pointer != codeline.end();) {
//            std::cout << alternation << std::endl;
            ptr = get_number(codeline, pointer);
            if (ptr != nullptr) {
                if (alternation == false || alternation == 2) {
                    alternation = true;
                    infix.push_back(ptr);
                    continue;
                } else if (alternation == true) {
                    delete ptr;
                    throw(NO_ALTERATION);
                }
            }
            ptr = get_operator(codeline, pointer, pointer - codeline.begin());
            if (ptr != nullptr) {
                Oper *op = (Oper *)ptr;
                if (op -> get_type() == RBRACKET) {
                    if(bracket.empty()) {
                        delete ptr;
                        throw(ERR_NOT_BALANCED_BRACKETS);
                    }
                    bracket.pop();
                    infix.push_back(ptr);
                    continue;
                } else if (op -> get_type() == LBRACKET) {
                    alternation = false;
                    bracket.push(true);
                    infix.push_back(ptr);
                    continue;
                } else if(alternation == true || alternation == 2) {
                    alternation = false;
                    if(op -> get_type() == COLON) {
                        Lexem *p = new Label(((Variable *)infix.back()) -> getName());
                        delete infix.back();
                        infix.pop_back();
                        infix.push_back(p);
                    } else if(op -> get_type() == GOTO) {
                        infix.push_back(ptr);
                        ptr = get_label(codeline, pointer);
                    }
                    infix.push_back(ptr);
                    continue;
                } else if (alternation == false) {
                    delete ptr;
                    throw(NO_ALTERATION);
                }
            }
            ptr = get_variable(codeline, pointer);
            if (ptr != nullptr) {
                if(alternation == false || alternation == 2) {
                    alternation = true;
                    infix.push_back(ptr);
                    continue;
                } else if (alternation == true) {
                    delete ptr;
                    throw(NO_ALTERATION);
                }
            }
            pointer++;
        }
    } catch (ERRORS e) {
        std::cout << codeline << std::endl;
        del(infix);
        throw(e);
    }
    return infix;
}

void initLabels(std::vector<Lexem *> &infix, int row) { //init_labels
    for (int i = 1; i < (int)infix.size(); i++) {
        if(infix[i - 1] == nullptr) {
            continue;
        }
        if(infix[i - 1] -> getType() == LABEL &&
            infix[i] -> getType() == OPERATION) {
            if (((Oper*)infix[i]) -> get_type() == COLON) {
                Label *lexemlabel = (Label *)infix[i - 1];
                Labels[lexemlabel -> getName()] = row;
                delete infix[i - 1];
                delete infix[i];
                infix[i - 1] = nullptr;
                infix[i] = nullptr;
                i++;
            } else {
                throw(ERR_WITH_GOTO_OR_LABELS);
            }
        }
    }
}

void initjumps(std::vector<std::vector <Lexem *>> &infixLines) { //init_jumps
    std::stack <Goto *> stackIfElse;
    for (int row = 0; row < (int)infixLines.size(); row++) {
        for (int i = 0; i < (int)infixLines[row].size(); i++) {
            if (infixLines[row][i] == nullptr) {
                continue;
            }
            if (infixLines[row][i] -> getType() == OPERATION) {
                Oper *lexemoper = (Oper *)infixLines[row][i];
                if (lexemoper -> get_type() == IF) {
                    stackIfElse.push((Goto *)lexemoper);
                } else if (lexemoper -> get_type() == ELSE) {
                    if (stackIfElse.empty()) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    if (stackIfElse.top() -> get_type() != IF) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    stackIfElse.top() -> setRow(row + 1);
                    stackIfElse.pop();
                    stackIfElse.push((Goto *)lexemoper);
                } else if (lexemoper -> get_type() == ENDIF) {
                    if (stackIfElse.empty()) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    if (stackIfElse.top() -> get_type() != IF &&
                        stackIfElse.top() -> get_type() != ELSE) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    stackIfElse.top() -> setRow(row + 1);
                    stackIfElse.pop();
                }
            }
        }
    }
    if(!stackIfElse.empty()) {
        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
    }
    std::stack <Goto *> stackWhile;
    for (int row = 0; row < (int)infixLines.size(); row++) {
        for (int i = 0; i < ((int)infixLines[row].size()); i++) {
            if (infixLines[row][i] == nullptr) {
                continue;
            }
            if (infixLines[row][i] -> getType() == OPERATION) {
                Oper *lexemoper = (Oper *)infixLines[row][i];
                if (lexemoper -> get_type() == WHILE) {
                    Goto *lexemgoto = (Goto *)lexemoper;
                    lexemgoto -> setRow(row);
                    stackWhile.push(lexemgoto);
                } else if (lexemoper -> get_type() == ENDWHILE) {
                    if (stackWhile.empty()) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    if(stackWhile.top() -> get_type() != WHILE) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    Goto *lexemgoto = (Goto *)lexemoper;
                    lexemgoto -> setRow(stackWhile.top() -> getRow());
                    stackWhile.top() -> setRow(row + 1);
                    stackWhile.pop();
                }
            }
        }
    }
    if(!stackWhile.empty()) {
        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
    }

    std::stack <int> stackFoo;
    std::map <std::string, int> FooStartVars;
    int startVariables = 0;
    for (int row = 0; row < (int)infixLines.size(); row++) {
        for (int i = 0; i < ((int)infixLines[row].size()); i++) {
            if (infixLines[row][i] == nullptr) {
                continue;
            } else if (infixLines[row][i] -> getType() == OPERATION) {
                if (((Oper *)infixLines[row][i]) -> get_type() == DEF) {
                    if(infixLines[row][i + 1] -> getType() != VARIABLE) {
                        throw(ERR_NAME_FUNCTION);
                    }
                    std::string Fname = ((Variable *)infixLines[row][i + 1]) -> getName();
                    Ftable[Fname] = row + 1;
                    delete infixLines[row][i];
                    delete infixLines[row][i + 1];
                    infixLines[row][i] = nullptr;
                    infixLines[row][i + 1] = nullptr;
                    i += 2;
                    while(i < (int)infixLines[row].size()) {
                        if (infixLines[row][i] -> getType() == OPERATION) {
                            OPERATOR op = ((Oper*)infixLines[row][i]) -> get_type();
                            if(op != COMMA && op != LBRACKET && op != RBRACKET) {
                                throw(ERR_FOO_START_VARS);
                            }
                        } else if (infixLines[row][i] -> getType() == VARIABLE){
                            FstartVars[Fname][((Variable *)infixLines[row][i]) -> getName()] = UNDEFINED;
                            startVariables++;
                        }
                        delete infixLines[row][i];
                        infixLines[row][i] = nullptr;
                        i++;
                    }
                    FooStartVars[Fname] = startVariables;
                    stackFoo.push(row);
                    startVariables = 0;
                } else if (((Oper *)infixLines[row][i]) -> get_type() == ENDDEF) {
                    delete infixLines[row][i];
                    infixLines[row][i] = nullptr;
                    if(stackFoo.empty()) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    stackFoo.pop();
                }
            } else if (infixLines[row][i] -> getType() == VARIABLE) {
                Variable *var = (Variable *)infixLines[row][i];
                if (Ftable.find(var -> getName()) != Ftable.end()) {
                    Foo *foo = new Foo(var -> getName(), row, FooStartVars[var -> getName()]);
                    delete infixLines[row][i];
                    infixLines[row][i] = foo;
                } else if (i != (int)infixLines[row].size() - 1){
                    if(infixLines[row][i + 1] -> getType() == OPERATION &&
                        ((Oper *)infixLines[row][i + 1]) -> get_type() == LBRACKET) {
                            throw(ERR_UNDEFINED_FUNCTION);
                    }
                }
            }
        }
    }
    if (!stackFoo.empty()) {
        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
    }
}

void joinGotoandLabel(Label *label, std::vector<Lexem *> &postfix) { //join_goto_and_label()
    if (((Oper *)postfix.back()) -> get_type() == GOTO) {
        Goto *lexemgoto = (Goto *)postfix.back();
        lexemgoto -> setRow(label -> getName());
    }
}

std::vector <Lexem *> buildPostfix(std::vector<Lexem *> inf) {
    std::stack <Lexem *> st;
    std::vector <Lexem *> postfix;
    std::stack <bool> err;
    Oper *q;
    bool end = false;
    for(auto s: inf) {
        if (end) {
            throw(ERR_WITH_CONDITIONAL_OPERATORS);
        }
        if (s == nullptr) {
            continue;
        }
        if (s -> getType() == NUMBER || s -> getType() == VARIABLE) {
            postfix.push_back(s);
        } else if (s -> getType() == FUNCTION) {
            st.push(s);
        } else if (s -> getType() == LABEL) {
            joinGotoandLabel((Label *)s, postfix);
        } else if (s -> getType() == OPERATION) {
            q = (Oper *)s;
            if (q -> get_type() == GOTO) {
                while (!st.empty()) {
                    postfix.push_back(st.top());
                    st.pop();
                }
                postfix.push_back(s);
                continue;
            } else if (q -> get_type() == COMMA) {
                while (((Oper *)st.top()) -> get_type() != LBRACKET) {
                    postfix.push_back(st.top());
                    st.pop();
                }
                continue;
            } else if (q -> get_type() == THEN) {
                if(err.empty()) {
                    throw(ERR_WITH_CONDITIONAL_OPERATORS);
                }
                err.pop();
                end = true;
                continue;
            } else if (q -> get_type() == ENDIF || q -> get_type() == ENDDEF) {
                continue;
            } else if (q -> get_type() == RBRACKET) {
                if (!st.empty()) {
                    while ((!st.empty()) && ((Oper *)st.top()) -> get_type() != LBRACKET) {
                        postfix.push_back(st.top());
                        st.pop();
                    }
                }
                if(!st.empty()) {
                    st.pop();
                }
                if(!st.empty() && st.top() -> getType() == FUNCTION) {
                    postfix.push_back(st.top());
                    st.pop();
                }
                continue;
            } else if(q -> get_type() != LBRACKET) {
                if(q -> get_type() == WHILE || q -> get_type() == IF) {
                    err.push(true);
                }
                while (!(st.empty()) && st.top() -> getType() != FUNCTION &&
                        ((Oper *)st.top()) -> get_type() != LBRACKET &&
                        (q -> getPriority()) < ((Oper *)st.top()) -> getPriority()) {
                    postfix.push_back(st.top());
                    st.pop();
                }
            }
            st.push(s);
        }
    }
    while (!st.empty()) {
        postfix.push_back(st.top());
        st.pop();
    }
    if (!err.empty()) {
        throw(ERR_WITH_CONDITIONAL_OPERATORS);
    }
    return postfix;
};

Lexem* do_op(Lexem* left, Lexem* right, Lexem* operation,
            std::unordered_map <std::string, int> &LocalVarTable) { //apply_operation
    OPERATOR opertype = ((Oper *)operation) -> get_type();
    int value = 0, l = 0, r = 0;
    Lexem *val;
    if (opertype == ASSIGN) {
        if(right -> getType() == VARIABLE) {
            if (((Variable *)right) -> exist(LocalVarTable)) {
                value = ((Variable *)right) -> getValue(LocalVarTable);
            } else if (((Variable *)right) -> exist(GlobalVarTable)) {
                value = ((Variable *)right) -> getValue(GlobalVarTable);
            } else {
                throw(ERR_UNDEFINED_VARIABLE);
            }
        } else if (right -> getType() == NUMBER) {
            value = ((Number *)right) -> getValue();
        } else if (right -> getType() == BOOLEAN) {
            value = ((Boolean *)right) -> getValue();
        }
        if (left -> getType() != VARIABLE) {
            throw(ERR_WITH_ASSIGN);
        }
        if (((Variable *)left) -> exist(GlobalVarTable)) {
            ((Variable *)left) -> setValue(value, GlobalVarTable); //GLOBAL
        } else {
            ((Variable *)left) -> setValue(value, LocalVarTable); //LOCAL
        }
        val = new Number(value);
        return val;
    }
    if(left -> getType() == VARIABLE) {
        if (((Variable *)left) -> exist(LocalVarTable)) {
            l = ((Variable *)left) -> getValue(LocalVarTable);
        } else if (((Variable *)left) -> exist(GlobalVarTable)) {
            l = ((Variable *)left) -> getValue(GlobalVarTable);
        } else {
            throw(ERR_UNDEFINED_VARIABLE);
        }
    } else if (left -> getType() == NUMBER) {
        l = ((Number *)left) -> getValue();
    } else if (left -> getType() == BOOLEAN) {
        l = ((Boolean *)left) -> getValue();
    }
    if(right -> getType() == VARIABLE) {
        if (((Variable *)right) -> exist(LocalVarTable)) {
            r = ((Variable *)right) -> getValue(LocalVarTable);
        } else if (((Variable *)right) -> exist(GlobalVarTable)) {
            r = ((Variable *)right) -> getValue(GlobalVarTable);
        } else {
            throw(ERR_UNDEFINED_VARIABLE);
        }
    } else if (right -> getType() == NUMBER) {
        r = ((Number *)right) -> getValue();
    } else if (right -> getType() == BOOLEAN) {
        r = ((Boolean *)right) -> getValue();
    }
    if (opertype == OR) {
        val = new Boolean(l || r);
    }
    else if (opertype == AND) {
        val = new Boolean(l && r);
    }
    else if (opertype == BITOR) {
        value = l | r;
        val = new Number(value);
    }
    else if (opertype == XOR) {
        value = l ^ r;
        val = new Number(value);
    }
    else if (opertype == BITAND) {
        value = l & r;
        val = new Number(value);
    }
    else if (opertype == EQ) {
        val = new Boolean(l == r);
    }
    else if (opertype == NEQ) {
        val = new Boolean(l != r);
    }
    else if (opertype == LEQ) {
        val = new Boolean(l <= r);
    }
    else if (opertype == LT) {
        val = new Boolean(l < r);
    }
    else if (opertype == GEQ) {
        val = new Boolean(l >= r);
    }
    else if (opertype == GT) {
        val = new Boolean(l > r);
    }
    else if (opertype == SHL) {
        value = l << r;
        val = new Number(value);
    }
    else if (opertype == SHR) {
        value = l >> r;
        val = new Number(value);
    }
    else if (opertype == PLUS) {
        value = l + r;
        val = new Number(value);
    } else if (opertype == MINUS) {
        value = l - r;
        val = new Number(value);
    } else if (opertype == MULTIPLY) {
        value = l * r;
        val = new Number(value);
    } else if (opertype == DIV) {
        value = l / r;
        val = new Number(value);
    } else if (opertype == MOD) {
        value = l % r;
        val = new Number(value);
    }
    return val;
}

int evaluatePostfix(std::vector <std::vector <Lexem *>> postfixLines, int row, int &val,
                    bool &returned, std::unordered_map<std::string, int> &LocalVarTable) {
    Lexem *left, *right, *value;
    std::stack<Lexem *> st;
    std::vector<Lexem *> free;
    std::vector<Lexem *> postfix = postfixLines[row];
    if (output_lvl >= 3) {
        std::cout << row << " ---> ";
        print(postfix);
    }
    bool r = false;
    int ret;
    for(auto s: postfix) {
        if (s == nullptr) {
            continue;
        }
        if (s -> getType() == NUMBER || s -> getType() == VARIABLE ||
            s -> getType() == BOOLEAN) {
            st.push(s);
        } else if (s -> getType() == FUNCTION) {
            r = false;
            RetAddr.push(row);
            int locale_row = ((Foo *)s) -> getRow();
            int ret_val;
            tab += "\t";
            std::string Fname = ((Foo *)s) -> getName();
            std::unordered_map <std::string, int> LocTable = FstartVars[Fname];
            for (auto it = LocTable.begin();
                it != LocTable.end(); it++) {
                    value = st.top();
                    if (value -> getType() == NUMBER) {
                        it -> second = ((Number *)value) -> getValue();
                    } else if (value -> getType() == VARIABLE) {
                        it -> second = ((Variable *)value) -> getValue(LocalVarTable);
                    }
                    st.pop();
            }
            do {
                locale_row = evaluatePostfix(postfixLines, locale_row, ret_val,
                                            r, LocTable);
            } while (r == false);
            LocTables.push(LocTable);
            tabStack.push(tab);
            tab.pop_back();
            std::cout << std::endl;
            value = new Number(ret_val);
            free.push_back(value);
            st.push(value);
        } else if (s -> getType() == OPERATION) {
            Oper *q = (Oper *)s;
            if (q -> get_type() == GOTO ||
                q -> get_type() == ELSE ||
                q -> get_type() == ENDWHILE) {
                Goto *lexemgoto = (Goto *)q;
                del(free);
                return lexemgoto -> getRow();
            } else if (q -> get_type() == RET) {
                if (!st.empty()) {
                    value = st.top();
                    if (value -> getType() == NUMBER) {
                        val = ((Number *)value) -> getValue();
                    } else if (value -> getType() == BOOLEAN) {
                        val = ((Boolean *)value) -> getValue();
                    } else if (value -> getType() == VARIABLE) {
                        if (((Variable *)value) -> exist(LocalVarTable)) {
                            val = ((Variable *)value) -> getValue(LocalVarTable);
                        } else if (((Variable *)value) -> exist(GlobalVarTable)) {
                            val = ((Variable *)value) -> getValue(GlobalVarTable);
                        } else {
                            throw(ERR_UNDEFINED_VARIABLE);
                        }
                    }
                }
                if (!RetAddr.empty()) {
                    int retRow = RetAddr.top();
                    RetAddr.pop();
                    del(free);
                    returned = true;
                    return retRow;
                } else {
                    return -1;
                }
            } else if (q -> get_type() == IF || q -> get_type() == WHILE) {
                Goto *lexemgoto = (Goto *)q;
                Boolean *rvalue = (Boolean *)st.top();
                st.pop();
                if (!(rvalue -> getValue())) {
                    del(free);
                    return lexemgoto -> getRow();
                }
                continue;
            }
            right = st.top();
            st.pop();
            left = st.top();
            st.pop();
            value = do_op(left, right, s, LocalVarTable);
            st.push(value);
            free.push_back(value);
        }
    }
    if (!st.empty()) {
        value = st.top();
        st.pop();
        if(value -> getType() == BOOLEAN) {
            ret = ((Boolean *)value) -> getValue();
        } else  if (value -> getType() == NUMBER) {
            ret = ((Number *)value) -> getValue();
        }
        val = ret;
        if(!st.empty()) {
            throw(RUNTIME_ERR);
        }
    }
    del(free);
    row++;
    return row;
}

void initGlobalVarTable(std::vector <std::vector < Lexem *>> infixLines) {
    std::stack <Lexem *> defs;
    for (unsigned int i = 0; i < infixLines.size(); i++) {
        for (unsigned int j = 0; j < infixLines[i].size(); j++) {
            if (infixLines[i][j] -> getType() == OPERATION) {
                if (((Oper *)infixLines[i][j]) -> get_type() == DEF) {
                    defs.push(infixLines[i][j]);
                } else if (((Oper *)infixLines[i][j]) -> get_type() == ENDDEF) {
                    defs.pop();
                }
            } else if (infixLines[i][j] -> getType() == VARIABLE && defs.empty()) {
                ((Variable *)infixLines[i][j]) -> initVar();
            }
        }

    }
}
