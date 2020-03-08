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

std::string OPERTEXT[] = {
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
std::stack <std::unordered_map <std::string, int>> LocTables; //???
std::stack <std::string> tabStack;
std::queue <int> QofFunctios;
int OutputLvl;
int SerialNumberOfFunction = 0;

Oper::Oper(int n) {
        setType(OPERATION);
        out = OPERTEXT[n];
        opertype = (OPERATOR)n;
}

Lexem::Lexem() {
    type = NONE;
}

LEXEMTYPE Lexem::getType() {
    return type;
}

void Lexem::setType(LEXEMTYPE t) {
    type = t;
}

Number::Number(int val) {
    setType(NUMBER);
    value = val;
}

void Number::print() {
    std::cout << "[" << value << "]";
}

int Number::getValue() {
    return value;
}

void Oper::print() {
    std::cout << "["<< out << "]";
}

OPERATOR Oper::get_type() {
    return opertype;
}

int Oper::getPriority() {
    return PRIORITY[opertype];
}

Variable::Variable(std::string name) {
    Variable::name = name;
    setType(VARIABLE);
}

Variable::Variable(std::string name, int value) {
    Variable::name = name;
    Variable::value = value;
    GlobalVarTable[name] = value;
}

bool Variable::exist(std::unordered_map <std::string, int> VarTable) {
    if (VarTable.find(name) == VarTable.end()) {
        return false;
    }
    return true;
}

void Variable::initVar() {
    GlobalVarTable[name] = UNDEFINED;
}

void Variable::setValue(int value,
                        std::unordered_map <std::string , int> &VarTable) {
    Variable::value = value;
    VarTable[name] = value;
}

std::string Variable::getName() {
    return name;
}

int Variable::getValue(std::unordered_map <std::string, int> VarTable) {
    if (exist(VarTable)) {
        value = VarTable[name];
        return VarTable[name];
    } else if (exist(GlobalVarTable)) {
        value = GlobalVarTable[name];
        return GlobalVarTable[name];
    } else {
        return UNDEFINED;
    }
}

void Variable::print() {
    std::cout << "[" << name << "]";
}

Boolean::Boolean(bool ans) {
    setType(BOOLEAN);
    bit = ans;
}

void Boolean::print() {
    std::cout << "[" << bit << "]";
}

int Boolean::getValue() {
    return bit;
}

Label::Label(std::string name) {
    Label::name = name;
    setType(LABEL);
}

void Label::print() {
    std::cout << "["<< name << "]";
}

Goto::Goto(int operation): Oper::Oper(operation) {
    row  = UNDEFINED;
    Goto::operation = operation;
}

std::string Label::getName() {
    Label::name = name;
    setType(LABEL);
}

void Goto::setRow(int row) {
    Goto::row = row;
}

void Goto::setRow(const std::string &labelname) {
    row = Labels[labelname];
}

int Goto::getRow() {
    return row;
}

void Goto::print() {
    std::cout << "[" << OPERTEXT[operation] << "->" << "row = " << row << "]";
}

Function::Function(std::string name, int row, int startVars) {
    Function::name = name;
    startVariables = startVars;
    ((Goto *)this)->setRow(row);
    setType(FUNCTION);
}

void Function::setName(std::string name) {
    Function::name = name;
}

std::string Function::getName() {
    return name;
}

void Function::setRow(int row) {
    ((Goto *)this)->setRow(row);
}

int Function::getRow() { //Foo ???
    if (Ftable.find(name) != Ftable.end()) {
        return Ftable[name];
    } else {
        return -1;
    }
}

int Function::getNumStartVars() {
    return startVariables;
}

void Function::print() {
    std::cout << "[" << name << "-> " << ((Goto *)this)->getRow() << "]";
}

void print_var_tables() { //case print_var_tables() VarTable
    while (!LocTables.empty()) {
        tab = tabStack.top();
        std::cout << tab << QofFunctios.front() << " вызов" << std::endl;
        QofFunctios.pop();
        tabStack.pop();
        print_map(LocTables.top());
        LocTables.pop();
        std::cout << std::endl;
    }
    tab = "    ";
}

void print_lexem_array(std::vector <Lexem *> array) { //print_lexem_array()
    for (auto ptr: array) {
        if (ptr == nullptr) {
            continue;
        }
        if (ptr->getType() == OPERATION &&
        ((Oper *)ptr)->get_type() == GOTO) {
        } else {
            ptr->print();
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}

void print_lexem_arrays(std::vector <std::vector <Lexem *>> matrix) { // print_lexem_arrays()
    for (int free = 0; free < (int)matrix.size(); free++) {
        std::cout << free << ": ";
        print_lexem_array(matrix[free]);
    }
}


void print_map(std::unordered_map <std::string, int> &VarTable) {
    for (std::unordered_map<std::string,int>::iterator it = VarTable.begin();
    it != VarTable.end(); it++) {
        std::cout << tab << it->first << " = " << it->second << std::endl; //it->first
    }
}

void clear_vector(std::vector <Lexem *> v) { //clear_vector
    for (auto ptr: v) {
        delete ptr;
    }
}

void set_output_settings(int argc, char **argv) { //set_output...
    int i;
    for (i = 0; i < argc; i++) {
        std::string check = std::string(argv[i]);
        if (check == "output") {
            if (argv[i + 1][0] != '=') {
                throw(ERR_WITH_START_PROGRAM);
            }
            OutputLvl = atoi(argv[i + 2]);
            if (OutputLvl > 4) {
                throw(ERR_WITH_START_PROGRAM);
            }
            break;
        }
    }
    if (i == argc) {
        throw(ERR_WITH_START_PROGRAM);
    }
}
Lexem *get_number(std::string codeline, std::string::iterator &ptr) {
    if (!isdigit(*ptr) || isspace(*ptr)) {
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
    if (isdigit(*ptr) || isspace(*ptr)) {
        return nullptr;
    }
    int n = sizeof(OPERTEXT) / sizeof(std::string);
    int op;
    for (op = 0; op < n; op++) {
        int len = OPERTEXT[op].size();
        if (OPERTEXT[op] == codeline.substr(counter, len)) {
            ptr += OPERTEXT[op].size();
            counter += OPERTEXT[op].size();
            if (op == GOTO || op == IF || op == ELSE ||
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
    if (!isalpha(*ptr) || isspace(*ptr)) {
        return nullptr;
    }
    Lexem* var;
    std::string name;
    while ((ptr != codeline.end()) && (isalpha(*ptr) || isdigit(*ptr))) {
        name += *ptr;
        ptr++;
    }
    std::cout << name << std::endl;
    var = new Variable(name);
    return var;
}

Lexem *get_label(std::string codeline, std::string::iterator &ptr) {
    while (isspace(*ptr)) {
        ptr++;
    }
    if (!isalpha(*ptr)) {
        return nullptr;
    }
    Lexem* var;
    std::string name;
    while ((ptr != codeline.end()) && isalpha(*ptr)) {
        name += *ptr;
        ptr++;
    }
    if (GlobalVarTable.find(name) != GlobalVarTable.end()) {
        GlobalVarTable.erase(name);
    }
    var = new Label(name);
    return var;
}

std::vector <Lexem *> parse_lexem(std::string codeline) { //parse_lexem()
    std::string var;
    Lexem *ptr;
    std::vector <Lexem *> infix;
    int alternation = 2;//start case
    std::stack <bool> bracket;
//    bool lbracket = false;
    try {
        for (auto pointer = codeline.begin(); pointer != codeline.end();) {
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
                if (op->get_type() == RBRACKET) {
                    if (bracket.empty()) {
                        delete ptr;
                        throw(ERR_NOT_BALANCED_BRACKETS);
                    }
                    bracket.pop();
                    infix.push_back(ptr);
                    continue;
                } else if (op->get_type() == LBRACKET) {
                    alternation = false;
                    bracket.push(true);
                    infix.push_back(ptr);
                    continue;
                } else if (alternation == true || alternation == 2) {
                    alternation = false;
                    if (op->get_type() == COLON) {
                        Lexem *p = new Label(((Variable *)infix.back())->getName());
                        delete infix.back();
                        infix.pop_back();
                        infix.push_back(p);
                    } else if (op->get_type() == GOTO) {
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
                if (alternation == false || alternation == 2) {
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
        clear_vector(infix);
        throw(e);
    }
    return infix;
}

void init_labels(std::vector<Lexem *> &infix, int row) { //init_labels
    for (int i = 1; i < (int)infix.size(); i++) {
        if (infix[i - 1] == nullptr) {
            continue;
        }
        if (infix[i - 1]->getType() == LABEL &&
            infix[i]->getType() == OPERATION) {
            if (((Oper*)infix[i])->get_type() == COLON) {
                Label *lexemlabel = (Label *)infix[i - 1];
                Labels[lexemlabel->getName()] = row;
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

void init_jumps(std::vector<std::vector <Lexem *>> &infixLines) { //init_jumps
    std::stack <Goto *> stackIfElse;
    for (int row = 0; row < (int)infixLines.size(); row++) {
        for (int i = 0; i < (int)infixLines[row].size(); i++) {
            if (infixLines[row][i] == nullptr) {
                continue;
            }
            if (infixLines[row][i]->getType() == OPERATION) {
                Oper *lexemoper = (Oper *)infixLines[row][i];
                if (lexemoper->get_type() == IF) {
                    stackIfElse.push((Goto *)lexemoper);
                } else if (lexemoper->get_type() == ELSE) {
                    if (stackIfElse.empty()) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    if (stackIfElse.top()->get_type() != IF) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    stackIfElse.top()->setRow(row + 1);
                    stackIfElse.pop();
                    stackIfElse.push((Goto *)lexemoper);
                } else if (lexemoper->get_type() == ENDIF) {
                    if (stackIfElse.empty()) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    if (stackIfElse.top()->get_type() != IF &&
                        stackIfElse.top()->get_type() != ELSE) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    stackIfElse.top()->setRow(row + 1);
                    stackIfElse.pop();
                }
            }
        }
    }
    if (!stackIfElse.empty()) {
        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
    }
    std::stack <Goto *> stackWhile;
    for (int row = 0; row < (int)infixLines.size(); row++) {
        for (int i = 0; i < ((int)infixLines[row].size()); i++) {
            if (infixLines[row][i] == nullptr) {
                continue;
            }
            if (infixLines[row][i]->getType() == OPERATION) {
                Oper *lexemoper = (Oper *)infixLines[row][i];
                if (lexemoper->get_type() == WHILE) {
                    Goto *lexemgoto = (Goto *)lexemoper;
                    lexemgoto->setRow(row);
                    stackWhile.push(lexemgoto);
                } else if (lexemoper->get_type() == ENDWHILE) {
                    if (stackWhile.empty()) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    if (stackWhile.top()->get_type() != WHILE) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    Goto *lexemgoto = (Goto *)lexemoper;
                    lexemgoto->setRow(stackWhile.top()->getRow());
                    stackWhile.top()->setRow(row + 1);
                    stackWhile.pop();
                }
            }
        }
    }
    if (!stackWhile.empty()) {
        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
    }

    std::stack <int> stackFoo;
    std::map <std::string, int> FooStartVars;
    int startVariables = 0;
    for (int row = 0; row < (int)infixLines.size(); row++) {
        for (int i = 0; i < ((int)infixLines[row].size()); i++) {
            if (infixLines[row][i] == nullptr) {
                continue;
            } else if (infixLines[row][i]->getType() == OPERATION) {
                if (((Oper *)infixLines[row][i])->get_type() == DEF) {
                    if (infixLines[row][i + 1]->getType() != VARIABLE) {
                        throw(ERR_NAME_FUNCTION);
                    }
                    std::string Fname = ((Variable *)infixLines[row][i + 1])->getName();
                    Ftable[Fname] = row + 1;
                    delete infixLines[row][i];
                    delete infixLines[row][i + 1];
                    infixLines[row][i] = nullptr;
                    infixLines[row][i + 1] = nullptr;
                    i += 2;
                    while (i < (int)infixLines[row].size()) {
                        if (infixLines[row][i]->getType() == OPERATION) {
                            OPERATOR op = ((Oper*)infixLines[row][i])->get_type();
                            if (op != COMMA && op != LBRACKET && op != RBRACKET) {
                                throw(ERR_FUNCTION_START_VARS);
                            }
                        } else if (infixLines[row][i]->getType() == VARIABLE){
                            FstartVars[Fname][((Variable *)infixLines[row][i])->getName()] = UNDEFINED;
                            startVariables++;
                        }
                        delete infixLines[row][i];
                        infixLines[row][i] = nullptr;
                        i++;
                    }
                    FooStartVars[Fname] = startVariables;
                    stackFoo.push(row);
                    startVariables = 0;
                } else if (((Oper *)infixLines[row][i])->get_type() == ENDDEF) {
                    delete infixLines[row][i];
                    infixLines[row][i] = nullptr;
                    if (stackFoo.empty()) {
                        throw(ERR_NOT_BALANCED_CONDITIONAL_OPERATORS);
                    }
                    stackFoo.pop();
                }
            } else if (infixLines[row][i]->getType() == VARIABLE) {
                Variable *var = (Variable *)infixLines[row][i];
                if (Ftable.find(var->getName()) != Ftable.end()) {
                    Function *function = new Function(var->getName(), row, FooStartVars[var->getName()]);
                    delete infixLines[row][i];
                    infixLines[row][i] = function;
                } else if (i != (int)infixLines[row].size() - 1){
                    if (infixLines[row][i + 1]->getType() == OPERATION &&
                        ((Oper *)infixLines[row][i + 1])->get_type() == LBRACKET) {
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

void join_goto_and_label(Label *label, std::vector<Lexem *> &postfix) { //join_goto_and_label()
    if (((Oper *)postfix.back())->get_type() == GOTO) {
        Goto *lexemgoto = (Goto *)postfix.back();
        lexemgoto->setRow(label->getName());
    }
}

std::vector <Lexem *> build_postfix(std::vector<Lexem *> inf) {
    std::stack <Lexem *> st;
    std::vector <Lexem *> postfix;
    std::stack <bool> err;
    Oper *q;
    bool end = false;
    for (auto s: inf) {
        if (end) {
            throw(ERR_WITH_CONDITIONAL_OPERATORS);
        }
        if (s == nullptr) {
            continue;
        }
        if (s->getType() == NUMBER || s->getType() == VARIABLE) {
            postfix.push_back(s);
        } else if (s->getType() == FUNCTION) {
            st.push(s);
        } else if (s->getType() == LABEL) {
            join_goto_and_label((Label *)s, postfix);
        } else if (s->getType() == OPERATION) {
            q = (Oper *)s;
            if (q->get_type() == GOTO) {
                while (!st.empty()) {
                    postfix.push_back(st.top());
                    st.pop();
                }
                postfix.push_back(s);
                continue;
            } else if (q->get_type() == COMMA) {
                while (((Oper *)st.top())->get_type() != LBRACKET) {
                    postfix.push_back(st.top());
                    st.pop();
                }
                continue;
            } else if (q->get_type() == THEN) {
                if (err.empty()) {
                    throw(ERR_WITH_CONDITIONAL_OPERATORS);
                }
                err.pop();
                end = true;
                continue;
            } else if (q->get_type() == ENDIF || q->get_type() == ENDDEF) {
                continue;
            } else if (q->get_type() == RBRACKET) {
                if (!st.empty()) {
                    while ((!st.empty()) && ((Oper *)st.top())->get_type() != LBRACKET) {
                        postfix.push_back(st.top());
                        st.pop();
                    }
                }
                if (!st.empty()) {
                    st.pop();
                }
                if (!st.empty() && st.top()->getType() == FUNCTION) {
                    postfix.push_back(st.top());
                    st.pop();
                }
                continue;
            } else if (q->get_type() != LBRACKET) {
                if (q->get_type() == WHILE || q->get_type() == IF) {
                    err.push(true);
                }
                while (!(st.empty()) && st.top()->getType() != FUNCTION &&
                        ((Oper *)st.top())->get_type() != LBRACKET &&
                        (q->getPriority()) < ((Oper *)st.top())->getPriority()) {
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

Lexem* apply_operation(Lexem* left, Lexem* right, Lexem* operation,
            std::unordered_map <std::string, int> &LocalVarTable) { //apply_operation
    OPERATOR opertype = ((Oper *)operation)->get_type();
    int value = 0, l = 0, r = 0;
    Lexem *val;
    if (opertype == ASSIGN) {
        if (right->getType() == VARIABLE) {
            if (((Variable *)right)->exist(LocalVarTable)) {
                value = ((Variable *)right)->getValue(LocalVarTable);
            } else if (((Variable *)right)->exist(GlobalVarTable)) {
                value = ((Variable *)right)->getValue(GlobalVarTable);
            } else {
                throw(ERR_UNDEFINED_VARIABLE);
            }
        } else if (right->getType() == NUMBER) {
            value = ((Number *)right)->getValue();
        } else if (right->getType() == BOOLEAN) {
            value = ((Boolean *)right)->getValue();
        }
        if (left->getType() != VARIABLE) {
            throw(ERR_WITH_ASSIGN);
        }
        if (((Variable *)left)->exist(GlobalVarTable)) {
            ((Variable *)left)->setValue(value, GlobalVarTable); //GLOBAL
        } else {
            ((Variable *)left)->setValue(value, LocalVarTable); //LOCAL
        }
        val = new Number(value);
        return val;
    }
    if (left->getType() == VARIABLE) {
        if (((Variable *)left)->exist(LocalVarTable)) {
            l = ((Variable *)left)->getValue(LocalVarTable);
        } else if (((Variable *)left)->exist(GlobalVarTable)) {
            l = ((Variable *)left)->getValue(GlobalVarTable);
        } else {
            throw(ERR_UNDEFINED_VARIABLE);
        }
    } else if (left->getType() == NUMBER) {
        l = ((Number *)left)->getValue();
    } else if (left->getType() == BOOLEAN) {
        l = ((Boolean *)left)->getValue();
    }
    if (right->getType() == VARIABLE) {
        if (((Variable *)right)->exist(LocalVarTable)) {
            r = ((Variable *)right)->getValue(LocalVarTable);
        } else if (((Variable *)right)->exist(GlobalVarTable)) {
            r = ((Variable *)right)->getValue(GlobalVarTable);
        } else {
            throw(ERR_UNDEFINED_VARIABLE);
        }
    } else if (right->getType() == NUMBER) {
        r = ((Number *)right)->getValue();
    } else if (right->getType() == BOOLEAN) {
        r = ((Boolean *)right)->getValue();
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

int evaluate_postfix(std::vector <std::vector <Lexem *>> postfixLines, int row, int &val,
                    bool &returned, std::unordered_map<std::string, int> &LocalVarTable) {
    Lexem *left, *right, *value;
    std::stack<Lexem *> st;
    std::vector<Lexem *> free;
    std::vector<Lexem *> postfix = postfixLines[row];
    if (OutputLvl >= 3) {
        std::cout << row << " ---> ";
        print_lexem_array(postfix);
    }
    bool r = false;
    int ret;
    for (auto s: postfix) {
        if (s == nullptr) {
            continue;
        }
        if (s->getType() == NUMBER || s->getType() == VARIABLE ||
            s->getType() == BOOLEAN) {
            st.push(s);
        } else if (s->getType() == FUNCTION) {
            r = false;
            RetAddr.push(row);
            int locale_row = ((Function *)s)->getRow();
            int ret_val;
            tab += "    ";
            std::string Fname = ((Function *)s)->getName();
            std::unordered_map <std::string, int> LocTable = FstartVars[Fname];
            for (auto it = LocTable.begin();
                it != LocTable.end(); it++) {
                    value = st.top();
                    if (value->getType() == NUMBER) {
                        it->second = ((Number *)value)->getValue();
                    } else if (value->getType() == VARIABLE) {
                        it->second = ((Variable *)value)->getValue(LocalVarTable);
                    }
                    st.pop();
            }
            SerialNumberOfFunction++;
            QofFunctios.push(SerialNumberOfFunction);
            do {
                locale_row = evaluate_postfix(postfixLines, locale_row, ret_val,
                                            r, LocTable);
            } while (r == false);
            LocTables.push(LocTable);
            tab.pop_back();
            tab.pop_back();
            tab.pop_back();
            tab.pop_back();
            tabStack.push(tab);
            value = new Number(ret_val);
            free.push_back(value);
            st.push(value);
        } else if (s->getType() == OPERATION) {
            Oper *q = (Oper *)s;
            if (q->get_type() == GOTO ||
                q->get_type() == ELSE ||
                q->get_type() == ENDWHILE) {
                Goto *lexemgoto = (Goto *)q;
                clear_vector(free);
                return lexemgoto->getRow();
            } else if (q->get_type() == RET) {
                if (!st.empty()) {
                    value = st.top();
                    if (value->getType() == NUMBER) {
                        val = ((Number *)value)->getValue();
                    } else if (value->getType() == BOOLEAN) {
                        val = ((Boolean *)value)->getValue();
                    } else if (value->getType() == VARIABLE) {
                        if (((Variable *)value)->exist(LocalVarTable)) {
                            val = ((Variable *)value)->getValue(LocalVarTable);
                        } else if (((Variable *)value)->exist(GlobalVarTable)) {
                            val = ((Variable *)value)->getValue(GlobalVarTable);
                        } else {
                            throw(ERR_UNDEFINED_VARIABLE);
                        }
                    }
                }
                if (!RetAddr.empty()) {
                    int retRow = RetAddr.top();
                    RetAddr.pop();
                    clear_vector(free);
                    returned = true;
                    return retRow;
                } else {
                    return -1;
                }
            } else if (q->get_type() == IF || q->get_type() == WHILE) {
                Goto *lexemgoto = (Goto *)q;
                Boolean *rvalue = (Boolean *)st.top();
                st.pop();
                if (!(rvalue->getValue())) {
                    clear_vector(free);
                    return lexemgoto->getRow();
                }
                continue;
            }
            right = st.top();
            st.pop();
            left = st.top();
            st.pop();
            value = apply_operation(left, right, s, LocalVarTable);
            st.push(value);
            free.push_back(value);
        }
    }
    if (!st.empty()) {
        value = st.top();
        st.pop();
        if (value->getType() == BOOLEAN) {
            ret = ((Boolean *)value)->getValue();
        } else  if (value->getType() == NUMBER) {
            ret = ((Number *)value)->getValue();
        }
        val = ret;
        if (!st.empty()) {
            throw(RUNTIME_ERR);
        }
    }
    clear_vector(free);
    row++;
    return row;
}

void init_global_var_table(std::vector <std::vector < Lexem *>> infixLines) {
    std::stack <Lexem *> defs;
    for (unsigned int i = 0; i < infixLines.size(); i++) {
        for (unsigned int j = 0; j < infixLines[i].size(); j++) {
            if (infixLines[i][j]->getType() == OPERATION) {
                if (((Oper *)infixLines[i][j])->get_type() == DEF) {
                    defs.push(infixLines[i][j]);
                } else if (((Oper *)infixLines[i][j])->get_type() == ENDDEF) {
                    defs.pop();
                }
            } else if (infixLines[i][j]->getType() == VARIABLE && defs.empty()) {
                ((Variable *)infixLines[i][j])->initVar();
            }
        }

    }
}
