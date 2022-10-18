#include <iostream>
#include <math.h>
using namespace std;

// Задание: Заказ разной мебели (отечественной, импортной)
// Идея программы: абстрактный базовый класс — класс "мебель";
// производные классы — российская и европейская мебель, для которых по-разному рассчитывается оптовая цена через розничную.

class Furniture
{
    char   *furname;            // название мебели
    double retail_price;        // розничная цена
    double wholesale_discount;  // скидка на оптовую покупку (скидка от розничной цены)
public:

    char*  GetFurName()  const { return furname; }
    double GetRetPrice() const { return retail_price; }
    double GetDiscount() const { return wholesale_discount; }

    void SetFurName(const char *fn)
    {
        delete [] furname;
        furname = new char [strlen(fn) + 1];
        strcpy (furname, fn);
    }  
    void SetRetPrice(double rp) { retail_price = rp; }
    void SetDiscount(double wd) { wholesale_discount = wd; }

    static double import_duty;  // статическая переменная - пошлина на ввоз европейской мебели
    static double euro_rate;    // статическая переменная - курс евро

    Furniture(const char *fn, double rp, double wd)
    {
        furname = new char [strlen(fn) + 1];
        strcpy (furname, fn);
        retail_price = rp;
        wholesale_discount = wd;
    }

    Furniture(const Furniture &obj)  // конструктор копирования
    {
        furname = new char [strlen(obj.GetFurName())+1];
        strcpy(furname, obj.GetFurName());
        retail_price = obj.GetRetPrice();  
        wholesale_discount = obj.GetDiscount();  
    }
    
    virtual ~Furniture() { delete [] furname; }  // деструктор

    virtual double RubPrice() const = 0;  // чистая виртуальная функция для расчёта цены оптовой покупки мебели в производных классах

};

//===================================================================

class RusFurniture : public Furniture
{
public:
    RusFurniture(const char *fn, double rp, double wd) : Furniture(fn, rp, wd) {}
    RusFurniture(const RusFurniture &obj) : Furniture(obj.GetFurName(), obj.GetRetPrice(), obj.GetDiscount()) {}

    RusFurniture& operator=(const RusFurniture &obj)
    {
        this->SetFurName(obj.GetFurName());
        this->SetRetPrice(obj.GetRetPrice());
        this->SetDiscount(obj.GetDiscount());

        return *this;
    }

    double RubPrice() const
    {
        return (this->GetRetPrice()*(1-this->GetDiscount()));
    }

    void operator()(double new_price)  // перегрузка операции () для изменения розничной цены существующего объекта
    {
        this->SetRetPrice(new_price);
    }

    friend ostream &operator<<(ostream &s, const RusFurniture &obj);
};

//===================================================================

class EurFurniture : public Furniture
{
    char *country_name;
public:

    char* GetCountry() const { return country_name; }
    void SetCountry(const char *cn)
    {
        delete [] country_name;
        country_name = new char [strlen(cn) + 1];
        strcpy (country_name, cn);
    }

    EurFurniture(const char *fn, double rp, double wd, const char *cn) : Furniture(fn, rp, wd) 
    {
        country_name = new char [strlen(cn) + 1];
        strcpy (country_name, cn);
    }

    EurFurniture(const EurFurniture &obj) : Furniture(obj.GetFurName(), obj.GetRetPrice(), obj.GetDiscount())
    {
        country_name = new char [strlen(obj.GetCountry())+1];
        strcpy(country_name, obj.GetCountry());
    }

    ~EurFurniture() { delete [] country_name; }

    EurFurniture& operator=(const EurFurniture &obj)
    {
        this->SetFurName(obj.GetFurName());
        this->SetRetPrice(obj.GetRetPrice());
        this->SetDiscount(obj.GetDiscount());
        this->SetCountry(obj.GetCountry());

        return *this;
    }

    double RubPrice() const
    {
        return (this->GetRetPrice()*Furniture::euro_rate*(1-this->GetDiscount())*(1+Furniture::import_duty));
    }

    void operator()(double new_price)  // перегрузка операции () для изменения розничной цены существующего объекта
    {
        this->SetRetPrice(new_price);
    }

    friend ostream &operator<<(ostream &s, const EurFurniture &obj);
};

//===================================================================

ostream &operator<<(ostream &s, const RusFurniture &obj)
{
    double discount = obj.GetDiscount() * 100;

    s << "Russian furniture:" << endl;
    s << "Furniture name: " << obj.GetFurName() << endl;
    s << "Retail price: " << obj.GetRetPrice() << "₽" << endl;
    s << "Wholesale discount: " << discount << "%" << endl;
    s << "Final price: " << obj.RubPrice() << "₽" << endl;
    return s;
}

//===================================================================

ostream &operator<<(ostream &s, const EurFurniture &obj)
{
    double discount = obj.GetDiscount() * 100;
    double duty = Furniture::import_duty * 100;

    s << "European furniture:" << endl;
    s << "Country name: " << obj.GetCountry() << endl;
    s << "Furniture name: " << obj.GetFurName() << endl;
    s << "Retail price: " << obj.GetRetPrice() << "€"  << endl;
    s << "Wholesale discount: " << discount << "%" << endl;
    s << "Current euro rate: " << Furniture::euro_rate << "₽"  << endl;
    s << "Import duty: " << duty << "%" << endl;
    s << "Final price: " << obj.RubPrice() << "₽" << endl;   
    return s;
}

//===================================================================

double Furniture::import_duty = 0.2;
double Furniture::euro_rate = 95;

//===================================================================

int main()
{
    cout << endl; 

    RusFurniture Sofa1("Sofa", 50000, 0.1);
    cout << Sofa1 << endl;

    EurFurniture Sofa2("Sofa", 1000, 0.1, "Germany");
    cout << Sofa2 << endl;

    cout << "Checking the assignment operation. Let's create a new table: " << endl;
    RusFurniture Table1("Table", 10000, 0.12);
    cout << Table1 << endl;
    cout << "Let's assign a Sofa to the Table and print an info about the Table:" << endl;
    Table1 = Sofa1;
    cout << Table1;
    cout << "As we can see, now the Table is a Sofa!" << endl;
    cout << endl; 

    cout << "Checking the '()' operation. Let's change the cost of the Sofa by 75000₽ and print it out: " << endl;
    Sofa1(75000);
    cout << Sofa1 << endl;
 
    return 0;
}