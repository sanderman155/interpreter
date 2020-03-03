#ifndef INTERP_HPP
#define INTERP_HPP

//#include "interpreter_share.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include <locale>
#include <map>
#include <unordered_map>

enum ERRORS {
    ERR_UNDEFINED_OPERATOR,
    ERR_WITH_GOTO_OR_LABELS,
    NO_ALTERATION,
    ERR_NOT_BALANCED_BRACKETS,
    ERR_NOT_BALANCED_CONDITIONAL_OPERATORS,
    ERR_NAME_FUNCTION,
    ERR_FOO_START_VARS,
    ERR_UNDEFINED_FUNCTION,
    ERR_WITH_CONDITIONAL_OPERATORS,
    ERR_WITH_START,
    ERR_WITH_ASSIGN,
    ERR_UNDEFINED_VARIABLE,
    SYNTAX_ERROR,
    RUNTIME_ERR
};

enum {UNDEFINED = -INT32_MAX};

enum LEXEMTYPE {
    NONE,
    NUMBER,
    OPERATION,
    VARIABLE,
    BOOLEAN,
    LABEL,
    FUNCTION
};

enum OPERATOR {
    COMMA,
    DEF, RET, ENDDEF,
    IF, THEN,
    ELSE, ENDIF,
    WHILE, ENDWHILE,
    GOTO, ASSIGN, COLON,
    OR,
    AND,
    BITOR,
    XOR,
    BITAND,
    EQ, NEQ,
    SHL, SHR,
    LEQ, LT, GEQ, GT,
    PLUS, MINUS,
    MULTIPLY, DIV, MOD,
    LBRACKET, RBRACKET
};

extern std::string ERRORTEXT[];

extern int PRIORITY[];

extern std::string OPERTEXT [];

extern std::string tab;
extern std::unordered_map <std::string, int> GlobalVarTable;
extern std::map <std::string, int> Ftable;
extern std::map <std::string, std::unordered_map <std::string, int>> FstartVars;
extern std::map <std::string, int> Labels;
extern std::stack <int> RetAddr;

//explicit std::;
class Lexem {
    LEXEMTYPE type;
public:
    Lexem() {
        type = NONE;
    }
    virtual ~Lexem() {};
    virtual void print() {
    }
    LEXEMTYPE getType() {
        return type;
    }
    void setType(LEXEMTYPE t) {
        type = t;
    }
};

class Number: public Lexem {
    int value;
public:
    Number() {};
    ~Number() {};
    Number(int val) {
        setType(NUMBER);
        value = val;
    }
    virtual int getValue() {
        return value;
    }
    void print() {
        std::cout << "[" << value << "]";
    }
};

class Oper: public Lexem {
    OPERATOR opertype;
    std::string out;
public:
    Oper() {}
    ~Oper() {}
    Oper(int n);
    void print() {
        std::cout << "["<< out << "]";
    }
    OPERATOR get_type() {
        return opertype;
    }
    int getPriority();
};


class Variable: public Lexem {
    int value;
    std::string name;
public:
    Variable() {}
    Variable(std::string n) {
        name = n;
        setType(VARIABLE);
    }
    Variable(std::string, int);
    bool exist(std::unordered_map <std::string, int> VarTable) {
        if(VarTable.find(name) == VarTable.end()) {
            return false;
        }
        return true;
    }
    void initVar();
    void setValue(int v, std::unordered_map <std::string , int> &VarTable) {
        value = v;
        VarTable[name] = value;
    }
    std::string getName() {
        return name;
    }
    int getValue(std::unordered_map <std::string, int>);
    void print() {
        std::cout << "[" << name << "]";
    }
};

class Boolean: public Number {
    bool bit;
public:
    Boolean() {}
    Boolean(bool ans) {
        setType(BOOLEAN);
        bit = ans;
    }
    void print() {
        std::cout << "[" << bit << "]";
    }
    int getValue() {
        return bit;
    }
};

class Label: public Lexem {
    int row;
    std::string name;
public:
    Label() {}
    Label(std::string name) {
        Label::name = name;
        setType(LABEL);
    }
    std::string getName() {
        return name;
    }
    void print() {
        std::cout << "["<< name << "]";
    }
};

class Goto: public Oper {
    int row = UNDEFINED;
    int operation;
public:
    Goto() {}
    Goto(int operation): Oper(operation) {
        row  = UNDEFINED;
        Goto::operation = operation;
    }
    void setRow(int row) {
        Goto::row = row;
    }
    void setRow(const std::string &);
    int getRow() {
        return row;
    }
    void print();
};


class Foo: public Goto {
    std::string name;
    int startVariables = 0;
public:
    Foo(std::string name, int row, int startVars) {
        Foo::name = name;
        startVariables = startVars;
        ((Goto *)this) -> setRow(row);
        setType(FUNCTION);
    }
    void setName(std::string name) {
        Foo::name = name;
    }
    std::string getName() {
        return name;
    }
    void setRow(int row) {
        ((Goto *)this) -> setRow(row);
    }
    int getRow();
    int getNumStartVars() {
        return startVariables;
    }
    void print() {
        std::cout << "[" << name << "-> " << ((Goto *)this) -> getRow() << "]";
    }
};

void print(std::vector <Lexem *>);

void print_all(std::vector <std::vector <Lexem *>>);

void print_map(std::unordered_map <std::string, int> &);

void del(std::vector <Lexem *>);

Lexem *get_number(std::string codeline, std::string::iterator &);

Lexem *get_operator(std::string, std::string::iterator &, int);

Lexem *get_variable(std::string, std::string::iterator &);

Lexem *get_label(std::string, std::string::iterator &);

std::vector <Lexem *> parseLexem(std::string);

void initLabels(std::vector<Lexem *> &, int);

void initjumps(std::vector<std::vector <Lexem *>> &);

void joinGotoandLabel(Label *, std::vector<Lexem *> &);

std::vector <Lexem *> buildPostfix(std::vector<Lexem *>);

Lexem* do_op(Lexem*, Lexem*, Lexem*, std::unordered_map <std::string, int> &);

int evaluatePostfix(std::vector <std::vector <Lexem *>>, int, int &, bool &, std::unordered_map<std::string, int> &);

void initGlobalVarTable(std::vector <std::vector < Lexem *>>);

#endif
