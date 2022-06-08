// firstTask.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <math.h>

#define PI 3.14159265

class Point {
public:
    int m_x = 0;
    int m_y = 0;

    Point(int x, int y) :m_y(y), m_x(x) {};
    Point(const Point& p) :m_y(p.m_y), m_x(p.m_x) {};
    Point() {};
    ~Point() {};

    friend std::ostream& operator<<(std::ostream& stream, Point const& point) {
        return stream << "{x:"<< point.m_x<<",y:"<< point.m_y<<"}";
    }

    Point operator=(Point& b) {
        m_x = b.m_x;
        m_y = b.m_y;
        return *this;
    }
     Point operator-(const Point& b) const {
          Point tmp(m_x - b.m_x, m_y - b.m_y);
          return tmp;
     }
     
};



class Lomanay {
public:
    std::vector<Point> m_points;
    Lomanay(std::vector<Point>& points) :m_points(points) {};
    Lomanay(const Lomanay& l) :m_points(l.m_points) {};
    Lomanay() { };
    void pushPoint(Point point)  {
        m_points.push_back(point);
    };
    ~Lomanay() {};

    friend std::ostream& operator<<(std::ostream& stream, Lomanay const& lomanay) {
        stream << "[ ";
        for (auto& i : lomanay.m_points)
            stream << i << ", ";
        stream << "]";

        return stream;
    }

    virtual std::string type() { return "Lomanay"; }
};

class ClosedLomanay :public Lomanay{
public:
    ClosedLomanay(const std::vector<Point>& points) 
        { 
        if (points.size() < 3)
            throw "lol";

        m_points = points;
    };
    ClosedLomanay(const Lomanay& l) :Lomanay(l) {
        if (l.m_points.size() < 3)
            throw "lol";
    }
    ClosedLomanay() {}

    double perimetr() {
        if (m_points.size() < 3)throw "error";
        double length=0;
        for (int i = 0; i < m_points.size(); i++) {
            Point first;
            if(i!=0)
                 first = m_points[i-1];
            else
                 first = m_points[m_points.size()-1];
            Point second = m_points[i];
            int dx = first.m_x - second.m_x;
            int dy = first.m_y - second.m_y;
            length += sqrt(dx * dx + dy * dy);
        }
        return length;
    }

     std::string type() { return "ClosedLomanay"; }

};

class Mnogougolnik :public ClosedLomanay {
public:

    Mnogougolnik(const std::vector<Point>& l) :ClosedLomanay(l) {}
    Mnogougolnik(const Lomanay& l) :ClosedLomanay(l) {}
    Mnogougolnik() :ClosedLomanay() {}
    
    
    double area() {
        double area = 0.0;

        int n = m_points.size();
        Point first = m_points[ n-1];
        
        for (int i = 0; i < n; i++)
        {
            Point second = m_points[i];
            area += (first.m_x + second.m_x) * (first.m_y - second.m_y);
            first = second;
        }

        return abs(area / 2.0);
    }
     std::string type() { return "Mnogougolnik"; }

};

class Triangle :public Mnogougolnik{
public:
    Triangle(const std::vector<Point>& points):Mnogougolnik(points) {
        if (points.size() != 3)throw "should have 3 points";
    }
    Triangle(Point& a, Point& b, Point& c) :Mnogougolnik({a,b,c}) {
        //m_points.push_back(a);
        //m_points.push_back(b);
        //m_points.push_back(c);
    }

    Triangle(Triangle& s) :Mnogougolnik(s) {}
     std::string type() { return "Triangle"; }

};

class Trapecia :public Mnogougolnik {
public:
    Trapecia(const std::vector<Point>& points):Mnogougolnik(points) {
        if (points.size() != 4)throw "should have 4 points";
        Point bottom = points[0] - points[3];
        Point top = points[1] - points[2];
        if ( top.m_x!=0&&top.m_y != 0 &&
            (abs(bottom.m_x/top.m_x)!= abs(bottom.m_y / top.m_y)))throw "should be paralel";
        //m_points = points;
    }
    Trapecia(Trapecia& s) :Mnogougolnik(s) {}
    Trapecia(Point& a, Point& b, Point& c, Point& d):Trapecia({ a,b,c,d }) {   }
     std::string type() { return "Trapecia"; }

};

class RightMnogougolnik :public Mnogougolnik {
public:
    RightMnogougolnik(RightMnogougolnik& s):Mnogougolnik(s){}
    RightMnogougolnik(int r, int n):Mnogougolnik() {
        double shag = ((double)360 / n) *PI/180;

        for (int i = 0; i < n; i++){

            m_points.push_back(Point(r * sin(shag * i), r * cos(shag * i)));
    }

    }
    virtual std::string type() { return "RightMnogougolnik"; }

};

int main()
{

    Point i(1, 2);
    Lomanay b;
    b.pushPoint(i);
    b.pushPoint(Point(3,4));
    b.pushPoint(Point(5, 3));

    ClosedLomanay d(b.m_points);
    std::cout << d << '\n';
    Mnogougolnik m(b);
    std::cout <<  m.area()<<"  "<<m.perimetr() << '\n';

    Trapecia t({ Point(0, 0), Point(2, 3), Point(4, 3), Point(5, 0) });

    std::cout << t.area()<<"   " << t.perimetr() << "\n";
    RightMnogougolnik rm(50, 5);

    std::cout << rm.area() << "   " << rm.perimetr() << "\n";
    
    std::cout << "\n\n" << i << '\n' << b << '\n';

    std::vector<Lomanay*> dinamicOOP = {& b,&d,&m,&t };
    for (auto i : dinamicOOP) {
        std::cout << i->type() << '\n';
    }
    //d.print();
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.