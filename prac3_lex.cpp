#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>

using namespace std;
ifstream file;

const char * TW[] = { 
    "", 
    "program",
    "boolean", "int",
    "else", "false",
    "if", "number",
    "return", "string",
    "true",
    "while", "write",
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
    // TW 
    LEX_NULL,
    LEX_PROGRAM,                            
    LEX_BOOL, LEX_INT,
    LEX_ELSE, LEX_FALSE,
    LEX_IF, LEX_NUMBER, 
    LEX_RETURN, LEX_STRING, 
    LEX_TRUE,
    LEX_WHILE, LEX_WRITE,
    LEX_AND, LEX_OR, LEX_NOT,
    // TD                                                                                    
    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_DOT, LEX_LPAREN,
    LEX_RPAREN, LEX_BEGIN, LEX_END,             
    LEX_EQ, LEX_DEQ, LEX_LSS, LEX_GTR, LEX_PLUS,
    LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_PERCENT,
    LEX_LEQ, LEX_NEQ, LEX_GEQ,
    LEX_ID,
    LEX_NUMB, LEX_STR_CONST                                                                                              
};

enum state { H, IDENT, NUMB, COM, HELPCOM, SLSH, MUL_PER, DOUBLE_OP1, DOUBLE_OP2, PLUS, MINUS, QUOTE };

class Ident 
{
    string id_name;       
    LexType id_type;
    int id_value;      
public:

    Ident (string n) : id_name(n) {}
    bool operator==(const string& s) const { return id_name == s; }

    LexType GetType () { return id_type; }
    int GetValue ()    { return id_value; }
    string GetName ()  { return id_name; }

    void SetType (LexType t) { id_type = t; }
    void SetValue (int v)    { id_value = v; }
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

    LexType GetType ()  { return l_type; }
    int GetValue ()     { return l_value; }

    void SetType (LexType t) { l_type = t; }
    void SetValue (int v)    { l_value = v; }

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
    int read_flag;
    char c;
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
        read_flag = 1;
        int dig, j;
        state CS = H;
        string str;
        do
        {
            if (read_flag) gc(); 
            else read_flag = 1;
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
                        read_flag = 0;
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
                        read_flag = 0;
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
                        read_flag = 0;
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
                        read_flag = 0;
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j);
                    }
                    break;
                case MUL_PER:
                    read_flag = 0;
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
                        read_flag = 0;
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
                        read_flag = 0;
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
                        read_flag = 0;
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j);                       
                    }    
                    break;    
            }
        } while(1);
    }
};

int main(int argc, char** argv)
{
    cout << "Lex analysator starts: " << endl;
    Scanner A;
    if (argc == 2) A = Scanner(argv[1]);
    else
    {
        printf ("Error with argc\n");
        return 1;
    }

    Lex a;
    while(!(cin.eof()))
    {
        try { a = A.GetLex(); }
        catch (char c)
        { 
            cout << "ERROR: " << c << "\n";
        }
        cout << a;
    }
    cout << "Lex analysator ends!\n" << endl;

    return 0;
}