#ifndef INTERP_HPP
#define INTERP_HPP

#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include <locale>
#include <map>
#include <unordered_map>
#include <queue>
enum ERRORS {
    ERR_WITH_START_PROGRAM,
    ERR_UNDEFINED_OPERATOR,
    ERR_WITH_GOTO_OR_LABELS,
    NO_ALTERATION,
    ERR_NOT_BALANCED_BRACKETS,
    ERR_NOT_BALANCED_CONDITIONAL_OPERATORS,
    ERR_NAME_FUNCTION,
    ERR_FUNCTION_START_VARS,
    ERR_UNDEFINED_FUNCTION,
    ERR_WITH_CONDITIONAL_OPERATORS,
    ERR_WITH_START,
    ERR_WITH_ASSIGN,
    ERR_UNDEFINED_VARIABLE,
    RUNTIME_ERR,
//    SYNTAX_ERROR
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
extern std::stack <std::unordered_map <std::string, int>> LocTables;
extern std::stack <std::string> tabStack;
extern std::queue <int> QofFunctios;
extern int OutputLvl;
extern int SerialNumberOfFunction;

class Lexem {
    LEXEMTYPE type;
public:
    Lexem();
    virtual ~Lexem() {};
    virtual void print() {};
    LEXEMTYPE getType();
    void setType(LEXEMTYPE);
};

class Number: public Lexem {
    int value;
public:
    Number() {};
    ~Number() {};
    Number(int);
    int getValue();
    void print();
};

class Oper: public Lexem {
    OPERATOR opertype;
    std::string out;
public:
    Oper() {}
    ~Oper() {}
    Oper(int);
    void print();
    OPERATOR get_type();
    int getPriority();
};


class Variable: public Lexem {
    int value;
    std::string name;
public:
    Variable() {}
    Variable(std::string);
    Variable(std::string, int);
    bool exist(std::unordered_map <std::string, int>);
    void initVar();
    void setValue(int, std::unordered_map <std::string , int> &);
    std::string getName();
    int getValue(std::unordered_map <std::string, int>);
    void print();
};

class Boolean: public Number {
    bool bit;
public:
    Boolean() {}
    Boolean(bool);
    void print();
    int getValue();
};

class Label: public Lexem {
    int row;
    std::string name;
public:
    Label() {}
    Label(std::string);
    std::string getName();
    void print();
};

class Goto: public Oper {
    int row = UNDEFINED;
    int operation;
public:
    Goto() {}
    Goto(int);
    void setRow(int);
    void setRow(const std::string &);
    int getRow();
    void print();
};


class Function: public Goto {
    std::string name;
    int startVariables = 0;
public:
    Function(std::string, int, int);
    void setName(std::string);
    std::string getName();
    void setRow(int);
    int getRow();
    int getNumStartVars();
    void print();
};

void print_var_tables();

void print_lexem_arrays(std::vector <Lexem *>);

void print_lexem_arrays(std::vector <std::vector <Lexem *>>);

void print_map(std::unordered_map <std::string, int> &);

void clear_vector(std::vector <Lexem *>);

void set_output_settings(int, char **);

Lexem *get_number(std::string codeline, std::string::iterator &);

Lexem *get_operator(std::string, std::string::iterator &, int);

Lexem *get_variable(std::string, std::string::iterator &);

Lexem *get_label(std::string, std::string::iterator &);

std::vector <Lexem *> parse_lexem(std::string);

void init_labels(std::vector<Lexem *> &, int);

void init_jumps(std::vector<std::vector <Lexem *>> &);

void join_goto_and_label(Label *, std::vector<Lexem *> &);

std::vector <Lexem *> build_postfix(std::vector<Lexem *>);

Lexem* apply_operation(Lexem*, Lexem*, Lexem*, std::unordered_map <std::string, int> &);

int evaluate_postfix(std::vector <std::vector <Lexem *>>, int, int &, bool &, std::unordered_map<std::string, int> &);

void init_global_var_table(std::vector <std::vector < Lexem *>>);

#endif
