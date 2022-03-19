#include <iostream>
#include <math.h>
using namespace std;

class Polynom
{
    int n;
    double *koef;

public:
    Polynom();                      
    Polynom(int deg);               
    Polynom(const Polynom &p);     

    double operator()(double a);            // перегрузка для вычисления значения многочлена для заданного аргумента
    Polynom operator+(const Polynom &p);    // перегрузка для вычисления суммы многочленов
    Polynom operator-(const Polynom &p);    // перегрузка для вычисления разности многочленов
    Polynom operator=(const Polynom &p);    // перегрузка для присваивания одного многочлена другому
    Polynom operator*(const Polynom &p);    // перегрузка для вычисления произведения многочленов 

    friend istream &operator>>(istream &s, Polynom &c);            // перегрузка оператора ввода
    friend ostream &operator<<(ostream &s, const Polynom &c);      // перегрузка оператора вывода

    ~Polynom();     // деструктор для удаления массива коэффициентов
};

//==============================================

Polynom::Polynom()
{
    koef = NULL;
}

//==============================================

Polynom::Polynom(int deg)
{
    n = deg;
    koef = new double[n + 1];
    for (int i = 0; i <= n; i++)
    {
        koef[i] = 0;
    }
}

//==============================================

Polynom::Polynom(const Polynom &p)
{
    n = p.n;
    koef = new double[n + 1];
    for (int i = 0; i <= n; i++)
    {
        koef[i] = p.koef[i];
    }
}

//==============================================

double Polynom::operator()(double a)
{
    int t = koef[0];
    for (int i = 1; i <= n; i++)
        t += koef[i] * pow(a, i);
    return t;
}

//==============================================

Polynom Polynom::operator+(const Polynom &p)
{
    int i;
    if (n >= p.n)
    {
        Polynom Z = *this;
        for (i = 0; i <= n; i++)
            Z.koef[i] = koef[i] + p.koef[i];
        return Z;
    }
    else
    {
        Polynom Z = p;
        for (i = 0; i <= n; i++)
            Z.koef[i] = p.koef[i] + koef[i];
        return Z;
    }
}

//==============================================

Polynom Polynom::operator-(const Polynom &p)
{
    int i;
    if (n >= p.n)
    {
        Polynom Z = *this;
        for (i = p.n; i >= 0; i--)
            Z.koef[i] = koef[i] - p.koef[i];
        return Z;
    }
    else
    {
        Polynom Z(p.n);
        for (i = p.n; i >= n+1; i--)
            Z.koef[i] = -p.koef[i];
        for (i = n; i >= 0; i--)
            Z.koef[i] = koef[i] - p.koef[i];
        return Z;
    }
}

//==============================================

Polynom Polynom::operator=(const Polynom &p)
{
    delete[]koef;
    n = p.n;
    koef = new double[n + 1];
    for (int i = 0; i <= n; i++)
        koef[i] = p.koef[i];
    return *this;
}

//==============================================

Polynom Polynom::operator*(const Polynom &p)
{
    int d = p.n+n;
    Polynom M(d), T(d), N(d); // в M хранится произведение, T — вспомогательный, N - нулевой

    for (int i = p.n; i >= 0; i--)
    {
        for (int j = n; j >= 0; j--)
        {
            T.koef[i+j] = koef[j] * p.koef[i];
        }
        M = M + T;
        T = N;
    }
    return M;
}

//==============================================

Polynom::~Polynom()
{
    delete[]koef;
}

//==============================================

istream &operator>>(istream &s, Polynom &c)
{
    for (int i = c.n; i >= 0; i--)
    {
        s >> c.koef[i];
    }
    return s;
}

//==============================================

ostream &operator<<(ostream &s, const Polynom &c)
{
    int i;
    int n = 0;

    for (i = 0; i <= c.n; i++)
    {
        if (c.koef[i] != 0)
            n++;
    }
    
    if (n != 0)
    {
        if (c.n > 1)
        {
            if (c.koef[c.n] != 0)
            {
                if (c.koef[c.n] < 0)
                {
                    if (c.koef[c.n] != -1)
                        s << c.koef[c.n] << "x^" << c.n;
                    else
                        s << "-x^" << c.n;
                }
                else
                {
                    if (c.koef[c.n] != 1)
                        s << c.koef[c.n] << "x^" << c.n;
                    else
                        s << "x^" << c.n;                
                }

            }
            for (i = (c.n-1); i >= 2; i--)
            {
                if (c.koef[i] != 0)
                {
                    if (c.koef[i] < 0)
                    {
                        if (c.koef[i] != -1)
                            s << c.koef[i] << "x^" << i;
                        else
                            s << "-x^" << i;
                    }
                    else
                    {
                        if (c.koef[i] != 1)
                            s << "+" << c.koef[i] << "x^" << i;
                        else
                            s << "+x^" << i;
                    }
                }
            }
        }
        if (c.koef[1] != 0)
        {
            if (c.koef[1] < 0)
            {
                if (c.koef[1] != -1)
                    s << c.koef[1] << "x";
                else
                    s << "-x";
            }
            else
            {
                if (c.n != 1)
                {
                    if (c.koef[1] != 1)
                        s << "+" << c.koef[1] << "x";
                    else
                        s << "+x";
                }
                else
                {
                    if (c.koef[1] != 1)
                        s << c.koef[1] << "x";
                    else
                        s << "x";                   
                }
            }
        }
        if (c.koef[0] != 0)
        {
            if (c.koef[0] < 0)
                s << c.koef[0]; 
            else
                s << "+" << c.koef[0]; 
        }
    }
    else
    {
        s << 0;
    }
    return s;
}

//==============================================

int main() 
{   
    cout << endl;
    cout << "Правило ввода коэффициентов: по одному через Enter, начиная с наивысшей степени" << endl;
    cout << "Пример отображения многочлена: 4x^3+3x^2+2x+1" << endl;   
    cout << endl;

    int n, m;
    cout << "Введите степень многочлена A: " << endl;
    cin >> n;
    Polynom A(n);
    cout << "Введите коэффициенты многочлена A:" << endl;
    cin >> A;
    
    cout << "Введите степень многочлена B: " << endl;
    cin >> m;
    Polynom B(m);
    cout << "Введите коэффициенты многочлена B:" << endl;
    cin >> B;

    int a;
    cout << "Введите число для вычисления значения многочлена А: " << endl;
    cin >> a;

    cout << endl;
    cout << "Многочлен А: " << A << endl;
    cout << "Многочлен B: " << B << endl;
    cout << endl;

    Polynom S, D, C;
    cout << "Значение многочлена: A(" << a << ") = " << A(a) << endl;
    cout << "Сумма многочленов: A+B = " << (S = A + B) << endl;
    cout << "Разность многочленов: A-B = " << (D = A - B) << endl;
    cout << "Произведение многочленов: A*B = " << (C = A * B) << endl;
    cout << endl;

    return 0;
}