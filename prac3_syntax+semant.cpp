#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>

using namespace std;
ifstream file;

const char * TW[] = { 
    "", 
    "program",
    "undefined",
    "boolean", "int",
    "else", "false",
    "if", "number",
    "return", "string",
    "true",
    "while", "write", "read",
    "and", "or", "not",
    NULL 
};

const char * TD[] = {
    ";", ",", ":", ".", "(", 
    ")", "{", "}", 
    "=", "==", "<", ">", "+", 
    "-", "*", "/", "%",
    "<=", "!=", ">=",
    NULL 
};

enum LexType {
    // TW 0-17
    LEX_NULL,
    LEX_PROGRAM,
    LEX_UNDEFINED,                            
    LEX_BOOL, LEX_INT,
    LEX_ELSE, LEX_FALSE,
    LEX_IF, LEX_NUMBER, 
    LEX_RETURN, LEX_STRING, 
    LEX_TRUE,
    LEX_WHILE, LEX_WRITE, LEX_READ,
    LEX_AND, LEX_OR, LEX_NOT,
    // TD 18-37                                                                                 
    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_DOT, LEX_LPAREN,
    LEX_RPAREN, LEX_BEGIN, LEX_END,             
    LEX_EQ, LEX_DEQ, LEX_LSS, LEX_GTR, LEX_PLUS,
    LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_PERCENT,
    LEX_LEQ, LEX_NEQ, LEX_GEQ,
    // 38-40
    LEX_ID,
    LEX_NUMB, LEX_STR_CONST                                                                                              
};

enum state { H, IDENT, NUMB, COM, HELPCOM, SLSH, MUL_PER, DOUBLE_OP1, DOUBLE_OP2, PLUS, MINUS, QUOTE };

//==========================================================================================
// Классы Ident, Scanner и Lex - лексический анализ
//==========================================================================================

class Ident 
{
    string id_name;       
    LexType id_type;
    int id_value;   

    bool assign;
    bool declare; 
public:

    Ident (string n) : id_name(n), assign(false), declare(false) {}
    bool operator==(const string& s) const { return id_name == s; }

    LexType GetType() const { return id_type; }
    int GetValue() const    { return id_value; }
    string GetName() const  { return id_name; }
    bool GetAssign() const  { return assign; }
    bool GetDeclare() const { return declare; }

    void SetType(LexType t)  { id_type = t; }
    void SetValue(int v)     { id_value = v; }
    void SetName(string str) { id_name = str;}
    void SetAssign()         { assign = true; }
    void SetDeclare()        { declare = true; }
};

vector<Ident> TID;
int addtoTID(const string& str)
{
    vector<Ident>::iterator i;
    i = find(TID.begin(), TID.end(), str);
    if (i != TID.end()) return(i - TID.begin());
    else
    {
        TID.push_back(Ident(str));
        return(TID.size() - 1);
    }
}

class Lex
{
    LexType l_type;
    int l_value;
    string l_str;   
public:
    Lex (LexType t = LEX_NULL, int v = 0, string str = "") : l_type(t), l_value(v), l_str(str) {}

    LexType GetType()     { return l_type; }
    int GetValue()        { return l_value; }
    string GetStr() const { return l_str; }

    void SetType(LexType t)     { l_type = t; }
    void SetValue(int v)        { l_value = v; }
    void SetString(string s)    { l_str = s; }

    friend ostream& operator<< (ostream &out, Lex l);
};

ostream& operator<< (ostream &out, Lex l)
{
    string type, type_of_table;
    if (l.l_type <= LEX_NOT)               
    {
        type = (string)TW[l.l_type];
        type_of_table = "TW: ";
    }
    else if ((l.l_type <= LEX_GEQ) && (l.l_type >= LEX_SEMICOLON))
    {
        type = (string)TD[l.l_type - LEX_SEMICOLON];
        type_of_table = "TD: ";
    }
    else if (l.l_type == LEX_NUMB)
    {
        type = "NUM";
        type_of_table = "";
    }

    if (l.l_type == LEX_ID)
    {
        type = TID[l.l_value].GetName();
        out << " < " << type << " | " << "TID: " << l.l_value  << " > " <<  "\n";
    }
    else if (l.l_type == LEX_STR_CONST)
    {
        type = "STR";
        out << " < " << type << " | " << type_of_table << l.l_str << " > " << "\n";
    }
    else 
        out << " < " << type << " | " << type_of_table << l.l_value << " > " << "\n";
    return out;
}

class Scanner 
{
    char c;
    bool flag = true;
    char gc() 
    { 
        cin.read(&c, 1);
        return(c); 
    }
    int find(string s, const char** table)
    {
        int i = 0;
        while (table[i] != NULL)
        {
            if (s == table[i])
                return(i);
            i++;
        }
        return(0);
    }
public:

    Scanner() {}
    Scanner(const char* name)
    {
        file.open(name);
        if (!file) 
        {
            cout << "File not found\n";
            exit(1);
        }
        cin.rdbuf(file.rdbuf());
    } 

    Lex GetLex()
    {
        int dig, j;
        state CS = H;
        string str;
        do
        {
            if (flag) gc(); else flag = true;
            switch(CS)
            {
                case H:
                    if ( c == ' ' || c == '\n' || c == '\r' || c == '\t' ) ;
                    else if (isalpha(c))
                    {
                        str.push_back(c);
                        CS = IDENT;
                    }
                    else if (isdigit(c))
                    {
                        dig = c - '0';
                        CS = NUMB;
                    }
                    else if (c == '+')
                    {
                        str.push_back(c);
                        CS = PLUS;
                    }                   
                    else if (c == '-')
                    {
                        str.push_back(c);
                        CS = MINUS;
                    }
                    else if (c == '/')
                    {
                        str.push_back(c);
                        CS = SLSH;
                    }
                    else if(c == '*' || c == '%')
                    {
                        str.push_back(c);
                        CS = MUL_PER;
                    }
                    else if (c == '!' || c == '=')
                    {
                        str.push_back(c);
                        CS = DOUBLE_OP1;
                    }
                    else if (c == '<' || c == '>')
                    {
                        str.push_back(c);
                        CS = DOUBLE_OP2;
                    }
                    else if (c == '"')
                    {
                        CS = QUOTE;
                    }
                    else
                    {
                        str.push_back(c);
                        j = find(str, TD);
                        return(Lex((LexType)(j + (int)LEX_SEMICOLON), j));
                    }
                    break;  
                case IDENT:
                    if (isalpha(c) || isdigit(c))
                    {
                        str.push_back(c);
                    }        
                    else
                    {
                        flag = false;
                        if ((j = find(str, TW)))
                        {
                            return Lex((LexType)j, j);
                        }
                        else 
                        {
                            j = addtoTID(str);
                            return Lex(LEX_ID, j);
                        }
                    }
                    break;
                case NUMB:
                    if (isdigit(c))
                    {
                        dig = 10 * dig + (c - '0');
                    }
                    else if (isalpha(c))
                        throw c;
                    else
                    {
                        flag = false;
                        return Lex(LEX_NUMB, dig);
                    }
                    break;
                case PLUS:
                    if (c == '=' || c == '+')
                    {
                        str.push_back(c);
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j);
                    }
                    else
                    {
                        flag = false;
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j);
                    }
                    break;     
                case MINUS:
                    if (c == '=' || c == '-')
                    {
                        str.push_back(c);
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j);
                    }
                    else
                    {
                        flag = false;
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j);
                    }
                    break;
                case MUL_PER:
                    j = find(str, TD);
                    return Lex((LexType)(j + (int)LEX_SEMICOLON), j);
                    break;      
                case QUOTE:
                    if (c == '"')
                    {
                        string quoted_str = "";
                        quoted_str += str;
                        return Lex(LEX_STR_CONST, 0, quoted_str);
                    }  
                    str.push_back(c);
                    break;    
                case SLSH:
                    if (c == '*')
                    {
                        str.pop_back();
                        CS = COM;
                    }
                    else
                    {
                        flag = false;
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j);
                    }
                    break;
                case COM:
                    if (c == '*')
                        CS = HELPCOM;
                    break;
                case HELPCOM:
                    if (c == '/')
                        CS = H;
                    else
                        CS = COM; 
                    break;
                case DOUBLE_OP1:
                    if (c == '=')
                    {
                        str.push_back(c);
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j); 
                    }  
                    else
                    {
                        flag = false;
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j);
                    }
                    break;
                case DOUBLE_OP2: 
                    if (c == '=')
                    {
                        str.push_back(c);
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j);            
                    }   
                    else
                    {
                        flag = false;
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j);                       
                    }    
                    break;    
            }
        } while(1);
    }
};

//==========================================================================================
// Класс Parser - синтаксический и семантический анализ
//==========================================================================================

template <class T, class T_EL>
void from_st(T& st, T_EL& i) 
{
    i = st.top();
    st.pop();
}

class Parser 
{
    Lex     curr_lex;
    LexType c_type;
    int     c_val;
    string  c_str;
    Scanner scan;

    stack<int>     st_int;
    stack<LexType> st_lex;

    // начальное состояние
    void S(); 
    // работа с содержимым между LEX_BEGIN и LEX_END
    void B(); 
    // работа с описанием переменных
    void D();
    // вызывает рекурсивно T() и F()
    void E();
    void T();
    void F();
    // работа с read
    void RD();
    // работа с write
    void WR();
    
    // проверка переменной на повторное описание
    void dec(LexType type, int i); 
    // проверка переменной на факт описания в программе
    void check_id();    
    // проверка оператора ввода
    void check_id_in_read();
    // проверка соответствия типов операндов для двуместной операции
    void check_op();
    // проверка на равенство типов при операторе присваивания           
    void eq_type(LexType&);
    // проверка на то, что в условном операторе и операторе цикла находится логическое выражение
    void eq_bool(LexType&); 

    void gl()
    {
        curr_lex = scan.GetLex();
        c_type = curr_lex.GetType();
        c_val = curr_lex.GetValue();
        c_str = curr_lex.GetStr();
        cout << curr_lex;
    }

public:
    Parser() : scan() {}                           
    Parser(const char* name) : scan(name) {}      
    void analyze();
};

void Parser::analyze() 
{
    gl();
    S();
    cout << endl << "Your program is syntactically and semantically correct!" << endl;
}

void Parser::S() 
{
    if (c_type == LEX_PROGRAM)
    {
        gl();
        B();
    }
    else if (c_type == LEX_IF) 
    {
        LexType new_val;
        gl();
        if (c_type != LEX_LPAREN) 
        {
            throw curr_lex;
        } 
        else 
        {
            gl();
            E();
            eq_bool(new_val);
            if (c_type == LEX_RPAREN) 
            {
                gl();
                B();
                if (c_type == LEX_ELSE) 
                {
                    gl();
                    B();
                } 
            } 
            else 
            {
                throw curr_lex;
            }
        }
    } 
    else if (c_type == LEX_WHILE) 
    {
        LexType new_val;
        gl();
        if (c_type != LEX_LPAREN) 
        {
            throw curr_lex;
        }
        else 
        {
            gl();
            E();
            eq_bool(new_val);
            if (c_type == LEX_RPAREN) 
            {
                gl();
                B();
            } 
            else
                throw curr_lex;
        }
    } 
    else if (c_type == LEX_ID)
    {
        int l_v_index = curr_lex.GetValue();
        LexType new_val;
        check_id();
        gl();
        if (c_type == LEX_EQ) 
        {
            gl();
            E();
            eq_type(new_val);
            TID[l_v_index].SetType(new_val);
            if (c_type == LEX_SEMICOLON)
            {
                gl();
            }
            else if (cin.eof())
            {
                return;
            }
            else
            {
                throw curr_lex;
            }
        } 
        else
        {
            throw curr_lex;
        }
    } 
    else if (c_type == LEX_INT || c_type == LEX_STRING)
    {
        gl();
        D();
    }
    else if (c_type == LEX_NUMB) 
    {
        st_lex.push(LEX_NUMB);
        E();
        gl();
    }
    else if (cin.eof()) 
    {
        return;
    }
    else if (c_type == LEX_END) 
    {
        return;
    }
    else if (c_type == LEX_READ)
    {
        RD();
        check_id_in_read();
        gl();
    }
    else if (c_type == LEX_WRITE) 
    {
        WR();
        gl();
    } 
    S();
}

void Parser::B() 
{
    if (c_type == LEX_BEGIN) 
    {
        gl();
        S();
        if (c_type == LEX_END) 
        {
            gl();
        } 
        else 
        {
            throw curr_lex;
        }
    } 
    else 
    {
        throw curr_lex;
    }
}

void Parser::D() 
{
    if (c_type != LEX_ID) 
    {
        throw curr_lex;
    } 
    else 
    {
        st_int.push(c_val);
        int l_v_index = c_val;
        gl();
        if (c_type == LEX_EQ) 
        {
            LexType i;
            gl();
            E();
            from_st(st_lex, i);
            dec(i, l_v_index);
        }
        else 
        {
            dec(LEX_UNDEFINED, l_v_index);
        }

        while (c_type == LEX_COMMA)
        {
            gl();
            if (c_type != LEX_ID)
            {
                throw curr_lex;
            }
            else
            {
                st_int.push(c_val);
                int l_v_index = c_val;
                gl();
                if (c_type == LEX_EQ)
                {
                    LexType i;
                    gl();
                    E();
                    from_st(st_lex, i);
                    dec(i, l_v_index);
                } 
                else
                {
                    dec(LEX_UNDEFINED, l_v_index);
                }
            }
        }
        if (c_type != LEX_SEMICOLON && (!cin.eof()))
        {
            throw curr_lex;
        }
        else if (c_type == LEX_SEMICOLON)
        {
            gl();
        }
    }
}

void Parser::E()  
{
    T();
    while ( c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_GTR || c_type == LEX_LEQ || 
            c_type == LEX_GEQ || c_type == LEX_NEQ || c_type == LEX_DEQ || 
            c_type == LEX_OR || c_type == LEX_AND || c_type == LEX_NOT) 
    {
        st_lex.push(c_type);
        gl();
        T();
        check_op();
    }
}

void Parser::T() 
{
    F();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_PERCENT) 
    {
        st_lex.push(c_type);
        gl();
        F();
        check_op();
    }
}

void Parser::F() 
{
    if (c_type == LEX_ID) 
    {
        check_id();
        gl();
    } 
    else if (c_type == LEX_NUMB) 
    {
        st_lex.push(LEX_NUMB);
        gl();
    } 
    else if (c_type == LEX_TRUE) 
    {
        st_lex.push(LEX_BOOL);
        gl();
    } 
    else if (c_type == LEX_FALSE) 
    {
        st_lex.push(LEX_BOOL);
        gl();
    } 
    else if (c_type == LEX_STR_CONST) 
    {
        st_lex.push(LEX_STR_CONST);
        gl();
    } 
    else if (c_type == LEX_NOT) 
    {
        gl();
        F();
    } 
    else if (c_type == LEX_LPAREN) 
    {
        gl();
        E();
        if (c_type == LEX_RPAREN) 
        {
            gl();
        } 
        else 
        {
            throw curr_lex;
        }
    } 
    else 
    {
        throw curr_lex;
    }
}

void Parser::RD()
{
    gl();
    if (c_type != LEX_LPAREN)
    {
        throw curr_lex;
    }
    else
    {
        gl();
        E();
        if (c_type != LEX_RPAREN)
        {
            throw curr_lex;
        }
        else
        {
            gl();
        }
    }
}

void Parser::WR()
{
    gl();
    if (c_type != LEX_LPAREN)
    {
        throw curr_lex;
    }
    else
    {
        gl();
        E();
        while (c_type == LEX_COMMA)
        {
            gl();
            E();
        }
        if (c_type != LEX_RPAREN)
        {
            throw curr_lex;
        }
        else
        {
            gl();
        }
    }
}

void Parser::dec(LexType type, int i)
{
    if (TID[i].GetDeclare())
    {
        throw "twice";
    }
    else
    {
        TID[i].SetDeclare();
        TID[i].SetType(type);
    }
}

void Parser::check_id()
{
    if (TID[c_val].GetDeclare())
    {
        st_lex.push(TID[c_val].GetType());
    }
    else
    {
        throw "not declared";
    }
}

void Parser::check_id_in_read()
{
    if (!TID [c_val].GetDeclare()) 
        throw "not declared";
}

void Parser::check_op()
{
    LexType t1, t2, op;
    LexType operation_type = LEX_NUMB;
    LexType put_in_stack = LEX_BOOL;

    from_st(st_lex, t2);
    from_st(st_lex, op);
    from_st(st_lex, t1);

    if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH || op == LEX_PERCENT)
        put_in_stack = LEX_NUMB;
    
    if ( op == LEX_OR || op == LEX_AND)
        operation_type = LEX_BOOL;

    if ((t1 == t2) && (t1 == operation_type))
    {
        st_lex.push(put_in_stack);
    }
    else throw "wrong types in last operation";
}

void Parser::eq_type(LexType& new_val)
{
    from_st(st_lex, new_val);
    if (new_val == LEX_UNDEFINED)
    {
        throw "wrong types in '=' operation";
    }
}

void Parser::eq_bool(LexType& new_val) 
{
    from_st(st_lex, new_val);
    cout << new_val << endl;
    if (new_val != LEX_BOOL)
        throw "expression is not boolean";
}

class Translator
{
    Parser p;
public:
    Translator() : p() {}
    Translator(const char* name) : p(name) {};
    void Translate ()
    {
        p.analyze();
    }
};

//==========================================================================================

int main(int argc, char** argv)
{

    cout << "Syntax analysator starts: " << endl;

    Translator t;
    if (argc == 2) t = Translator(argv[1]);
    else
    {
        printf ("Error with argc\n");
        return 1;
    }

    try { t.Translate(); }
    catch (char c)
    { 
        cout << "ERROR: wrong NUM, extra character: " << c << "\n";
        return 2;
    }
    catch (LexType type)
    {
        cout << "ERROR: wrong type: " << to_string(type) << "\n";
        return 3;
    }
    catch (Lex l)
    {
        cout << "ERROR: wrong lexem:" << l << "\n";
        return 4;
    }
    catch (const char* str)
    {
        cout << "ERROR: " << str << "\n";
        return 5;
    }

    cout << "Syntax analysator ends!\n" << endl;

    return 0;
}