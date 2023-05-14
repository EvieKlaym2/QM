// Программа генератор неструктурированных поверхностных прямоугольных сеток при помощи алгоритма Q-Morph для трехмерных B-rep моделей на языке программирования c++
// На вход подается файл геометрии формата IGES
// Выходной файл сетки имеет формат NEU

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

// Структура для хранения точки в трехмерном пространстве
struct Point {
  double x, y, z;
  Point(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
};

// Структура для хранения вектора в трехмерном пространстве
struct Vector {
  double x, y, z;
  Vector(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
  // Длина вектора
  double length() const {
    return sqrt(x * x + y * y + z * z);
  }
  // Нормализация вектора
  void normalize() {
    double l = length();
    if (l > 0) {
      x /= l;
      y /= l;
      z /= l;
    }
  }
};

// Оператор сложения двух точек
Point operator+(const Point& p1, const Point& p2) {
  return Point(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
}

// Оператор вычитания двух точек
Point operator-(const Point& p1, const Point& p2) {
  return Point(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
}

// Оператор умножения точки на число
Point operator*(const Point& p, double k) {
  return Point(p.x * k, p.y * k, p.z * k);
}

// Оператор деления точки на число
Point operator/(const Point& p, double k) {
  return Point(p.x / k, p.y / k, p.z / k);
}

// Оператор скалярного произведения двух векторов
double operator*(const Vector& v1, const Vector& v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Оператор векторного произведения двух векторов
Vector operator^(const Vector& v1, const Vector& v2) {
  return Vector(v1.y * v2.z - v1.z * v2.y,
                v1.z * v2.x - v1.x * v2.z,
                v1.x * v2.y - v1.y * v2.x);
}

// Структура для хранения уравнения плоскости в виде a*x + b*y + c*z + d = 0
struct Plane {
  double a, b, c, d;
  Plane(double a = 0, double b = 0, double c = 0, double d = 0) : a(a), b(b), c(c), d(d) {}
};

// Функция для вычисления расстояния от точки до плоскости
double distance(const Point& p, const Plane& pl) {
  return abs(pl.a * p.x + pl.b * p.y + pl.c * p.z + pl.d) / sqrt(pl.a * pl.a + pl.b * pl.b + pl.c * pl.c);
}

// Функция для вычисления проекции точки на плоскость
Point project(const Point& p, const Plane& pl
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Функция для вычисления проекции точки на плоскость
Point project(const Point& p, const Plane& pl) {
  // Находим нормальный вектор к плоскости
  Vector n(pl.a, pl.b, pl.c);
  // Нормализуем его
  n.normalize();
  // Находим расстояние от точки до плоскости
  double d = distance(p, pl);
  // Вычитаем из точки нормальный вектор, умноженный на расстояние
  return p - n * d;
}

// Структура для хранения треугольника в трехмерном пространстве
struct Triangle {
  Point p1, p2, p3; // Вершины треугольника
  Plane pl; // Плоскость, на которой лежит треугольник
  Triangle(const Point& p1 = Point(), const Point& p2 = Point(), const Point& p3 = Point()) : p1(p1), p2(p2), p3(p3) {
    // Вычисляем коэффициенты уравнения плоскости по трем точкам
    Vector v1 = p2 - p1; // Вектор из первой вершины во вторую
    Vector v2 = p3 - p1; // Вектор из первой вершины в третью
    Vector n = v1 ^ v2; // Нормальный вектор к плоскости - векторное произведение двух векторов
    pl.a = n.x;
    pl.b = n.y;
    pl.c = n.z;
    pl.d = -n * p1; // Скалярное произведение нормального вектора и любой точки на плоскости с обратным знаком
  }
};

// Функция для проверки, лежит ли точка внутри треугольника или на его границе
bool inside(const Point& p, const Triangle& t) {
  // Проецируем точку на плоскость треугольника
  Point q = project(p, t.pl);
  // Вычисляем площади треугольников, образованных вершинами треугольника и проекцией точки
  double s1 = ((t.p1 - q) ^ (t.p2 - q)).length() / 2; // Площадь треугольника t.p1 t.p2 q
  double s2 = ((t.p2 - q) ^ (t.p3 - q)).length() / 2; // Площадь треугольника t.p2 t.p3 q
  double s3 = ((t.p3 - q) ^ (t.p1 - q)).length() / 2; // Площадь треугольника t.p3 t.p1 q
  double s = ((t.p1 - t.p2) ^ (t.p3 - t.p2)).length() / 2; // Площадь исходного треугольника t.p1 t.p2 t.p3
  // Если сумма площадей меньших треугольников равна площади исходного треугольника с некоторой погрешностью,
  // то точка лежит внутри или на границе исходного треугольника
  return abs(s1 + s2 + s3 - s) < 1e-6;
}

// Структура для хранения ребра в трехмерном пространстве
struct Edge {
  Point p1, p2; // Концы ребра
  Edge(const Point& p1 = Point(), const Point& p2 = Point()) : p1(p1), p2(p2) {}
};

// Функция
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Функция для вычисления длины ребра
double length(const Edge& e) {
  return (e.p1 - e.p2).length();
}

// Функция для вычисления угла между двумя ребрами
double angle(const Edge& e1, const Edge& e2) {
  // Находим векторы, соответствующие ребрам
  Vector v1 = e1.p2 - e1.p1;
  Vector v2 = e2.p2 - e2.p1;
  // Нормализуем их
  v1.normalize();
  v2.normalize();
  // Вычисляем скалярное произведение векторов
  double dot = v1 * v2;
  // Ограничиваем его по модулю единицей, чтобы избежать ошибок округления
  dot = max(-1.0, min(1.0, dot));
  // Возвращаем угол в радианах, используя арккосинус
  return acos(dot);
}

// Структура для хранения четырехугольника в трехмерном пространстве
struct Quad {
  Point p1, p2, p3, p4; // Вершины четырехугольника в порядке обхода против часовой стрелки
  Plane pl; // Плоскость, на которой лежит четырехугольник
  Quad(const Point& p1 = Point(), const Point& p2 = Point(), const Point& p3 = Point(), const Point& p4 = Point()) : p1(p1), p2(p2), p3(p3), p4(p4) {
    // Вычисляем коэффициенты уравнения плоскости по трем точкам
    Vector v1 = p2 - p1; // Вектор из первой вершины во вторую
    Vector v2 = p3 - p1; // Вектор из первой вершины в третью
    Vector n = v1 ^ v2; // Нормальный вектор к плоскости - векторное произведение двух векторов
    pl.a = n.x;
    pl.b = n.y;
    pl.c = n.z;
    pl.d = -n * p1; // Скалярное произведение нормального вектора и любой точки на плоскости с обратным знаком
  }
};

// Функция для проверки, лежит ли точка внутри четырехугольника или на его границе
bool inside(const Point& p, const Quad& q) {
  // Проецируем точку на плоскость четырехугольника
  Point r = project(p, q.pl);
  // Вычисляем площади треугольников, образованных вершинами четырехугольника и проекцией точки
  double s1 = ((q.p1 - r) ^ (q.p2 - r)).length() / 2; // Площадь треугольника q.p1 q.p2 r
  double s2 = ((q.p2 - r) ^ (q.p3 - r)).length() / 2; // Площадь треугольника q.p2 q.p3 r
  double s3 = ((q.p3 - r) ^ (q.p4 - r)).length() / 2; // Площадь треугольника q.p3 q.p4 r
  double s4 = ((q.p4 - r) ^ (q.p1 - r)).length() / 2; // Площадь треугольника q.p4 q.p1 r
  double s = ((q.p1 - q.p2) ^ (q.p3 - q.p2)).
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Вычисляем площади треугольников, образованных вершинами четырехугольника и проекцией точки
  double s1 = ((q.p1 - r) ^ (q.p2 - r)).length() / 2; // Площадь треугольника q.p1 q.p2 r
  double s2 = ((q.p2 - r) ^ (q.p3 - r)).length() / 2; // Площадь треугольника q.p2 q.p3 r
  double s3 = ((q.p3 - r) ^ (q.p4 - r)).length() / 2; // Площадь треугольника q.p3 q.p4 r
  double s4 = ((q.p4 - r) ^ (q.p1 - r)).length() / 2; // Площадь треугольника q.p4 q.p1 r
  double s = ((q.p1 - q.p2) ^ (q.p3 - q.p2)).length() / 2 + ((q.p3 - q.p4) ^ (q.p1 - q.p4)).length() / 2; // Площадь исходного четырехугольника q.p1 q.p2 q.p3 q.p4
  // Если сумма площадей меньших треугольников равна площади исходного четырехугольника с некоторой погрешностью,
  // то точка лежит внутри или на границе исходного четырехугольника
  return abs(s1 + s2 + s3 + s4 - s) < 1e-6;
}

// Структура для хранения грани в трехмерном пространстве
struct Face {
  vector<Point> points; // Вершины грани в порядке обхода против часовой стрелки
  Plane pl; // Плоскость, на которой лежит грань
  Face(const vector<Point>& points = vector<Point>()) : points(points) {
    // Вычисляем коэффициенты уравнения плоскости по трем точкам
    Vector v1 = points[1] - points[0]; // Вектор из первой вершины во вторую
    Vector v2 = points[2] - points[0]; // Вектор из первой вершины в третью
    Vector n = v1 ^ v2; // Нормальный вектор к плоскости - векторное произведение двух векторов
    pl.a = n.x;
    pl.b = n.y;
    pl.c = n.z;
    pl.d = -n * points[0]; // Скалярное произведение нормального вектора и любой точки на плоскости с обратным знаком
  }
};

// Функция для проверки, лежит ли точка внутри грани или на его границе
bool inside(const Point& p, const Face& f) {
  // Проецируем точку на плоскость грани
  Point q = project(p, f.pl);
  // Для каждого ребра грани проверяем, с какой стороны от него лежит проекция точки
  int n = f.points.size(); // Количество вершин грани
  for (int i = 0; i < n; i++) {
    // Берем ребро из i-й вершины в i+1-ю вершину (по модулю n)
    Point a = f.points[i];
    Point b = f.points[(i + 1) % n];
    // Находим нормальный вектор к ребру, направленный внутрь грани
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Берем ребро из i-й вершины в i+1-ю вершину (по модулю n)
    Point a = f.points[i];
    Point b = f.points[(i + 1) % n];
    // Находим нормальный вектор к ребру, направленный внутрь грани
    Vector v = b - a; // Вектор, соответствующий ребру
    Vector n = v ^ f.pl; // Векторное произведение вектора ребра и нормали к плоскости грани
    n.normalize(); // Нормализуем нормальный вектор
    // Вычисляем скалярное произведение нормального вектора и вектора из начала ребра в проекцию точки
    double dot = n * (q - a);
    // Если скалярное произведение отрицательно, то точка лежит снаружи грани
    if (dot < 0) return false;
  }
  // Если точка лежит с внутренней стороны всех ребер грани, то она лежит внутри или на границе грани
  return true;
}

// Структура для хранения тела в трехмерном пространстве
struct Body {
  vector<Face> faces; // Грани тела
  Body(const vector<Face>& faces = vector<Face>()) : faces(faces) {}
};

// Функция для чтения тела из файла формата IGES
Body read_iges(const string& filename) {
  // Открываем файл для чтения
  ifstream fin(filename);
  if (!fin) {
    cerr << "Error: cannot open file " << filename << endl;
    exit(1);
  }
  // Создаем пустое тело
  Body body;
  // Читаем файл построчно
  string line;
  while (getline(fin, line)) {
    // Если строка начинается с символа 'S', то это начало секции спецификации
    if (line[0] == 'S') {
      // Пропускаем эту секцию, так как она не содержит информации о геометрии тела
      continue;
    }
    // Если строка начинается с символа 'G', то это начало секции глобальных параметров
    if (line[0] == 'G') {
      // Пропускаем эту секцию, так как она не содержит информации о геометрии тела
      continue;
    }
    // Если строка начинается с символа 'D', то это начало секции каталога записей
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Пропускаем эту секцию, так как она не содержит информации о геометрии тела
      continue;
    }
    // Если строка начинается с символа 'D', то это начало секции каталога записей
    if (line[0] == 'D') {
      // Пропускаем эту секцию, так как она не содержит информации о геометрии тела
      continue;
    }
    // Если строка начинается с символа 'P', то это начало секции параметров
    if (line[0] == 'P') {
      // Читаем тип сущности из первых восьми символов строки
      string type = line.substr(0, 8);
      // Если тип сущности равен "128     ", то это B-spline поверхность
      if (type == "128     ") {
        // Читаем параметры поверхности из строки
        int k1, k2; // Степени B-spline базисных функций по двум направлениям
        int m1, m2; // Количество контрольных точек по двум направлениям
        int n1, n2; // Количество узловых векторов по двум направлениям
        int prop1, prop2, prop3, prop4; // Свойства поверхности (замкнутость, периодичность и т.д.)
        vector<double> u; // Узловой вектор по первому направлению
        vector<double> v; // Узловой вектор по второму направлению
        vector<vector<Point>> p; // Контрольные точки поверхности
        fin >> k1 >> k2 >> m1 >> m2 >> n1 >> n2 >> prop1 >> prop2 >> prop3 >> prop4;
        u.resize(n1);
        v.resize(n2);
        p.resize(m1);
        for (int i = 0; i < n1; i++) fin >> u[i];
        for (int i = 0; i < n2; i++) fin >> v[i];
        for (int i = 0; i < m1; i++) {
          p[i].resize(m2);
          for (int j = 0; j < m2; j++) {
            double x, y, z, w; // Координаты и вес контрольной точки в однородном пространстве
            fin >> x >> y >> z >> w;
            p[i][j] = Point(x / w, y / w, z / w); // Переводим в неоднородное пространство
          }
        }
        // Создаем пустую грань для хранения четырехугольников, аппроксимирующих поверхность
        Face face;
        // Задаем параметры разбиения поверхности на четырехугольники
        int n = 10; // Количество четырехугольников по каждому направлению
////////////////////////////////////////////////////////////////////////////////////////////////////////
        Face face;
        // Задаем параметры разбиения поверхности на четырехугольники
        int n = 10; // Количество четырехугольников по каждому направлению
        double du = 1.0 / n; // Шаг по первому параметру
        double dv = 1.0 / n; // Шаг по второму параметру
        // Перебираем четырехугольники по параметрам
        for (int i = 0; i < n; i++) {
          for (int j = 0; j < n; j++) {
            // Вычисляем четыре вершины четырехугольника на поверхности по формуле B-spline
            Point p1 = evaluate_b_spline_surface(p, u, v, k1, k2, i * du, j * dv); // Вершина с параметрами (i * du, j * dv)
            Point p2 = evaluate_b_spline_surface(p, u, v, k1, k2, (i + 1) * du, j * dv); // Вершина с параметрами ((i + 1) * du, j * dv)
            Point p3 = evaluate_b_spline_surface(p, u, v, k1, k2, (i + 1) * du, (j + 1) * dv); // Вершина с параметрами ((i + 1) * du, (j + 1) * dv)
            Point p4 = evaluate_b_spline_surface(p, u, v, k1, k2, i * du, (j + 1) * dv); // Вершина с параметрами (i * du, (j + 1) * dv)
            // Создаем четырехугольник из этих вершин и добавляем его к грани
            Quad quad(p1, p2, p3, p4);
            face.points.push_back(p1);
            face.points.push_back(p2);
            face.points.push_back(p3);
            face.points.push_back(p4);
          }
        }
        // Добавляем грань к телу
        body.faces.push_back(face);
      }
      // Если тип сущности не равен "128     ", то это не B-spline поверхность и мы ее игнорируем
      else {
        continue;
      }
    }
    // Если строка начинается с символа 'T', то это начало секции завершения
    if (line[0] == 'T') {
      // Заканчиваем чтение файла
      break;
    }
  }
  // Закрываем файл
  fin.close();
  // Возвращаем прочитанное тело
  return body;
}

// Функция для вычисления точки на B-spline поверхности по заданным параметрам
Point evaluate_b_spline_surface(const vector<vector<Point>>& p,
                                const vector<double>& u,
                                const vector<double>& v,
                                int k1,
                                int k2,
                                double u0,
                                double v0) {
  // Используем алгоритм де Бора для вычисления точки на B-spline кривой по одному параметру
  // Алгоритм де Бора: https://en.wikipedia.org/wiki/De_Boor%27s_algorithm
  int m1 = p.size(); // Количество контрольных точек по первому направлению
  int m2 = p[0].size(); // Количество контрольных точек по второму направлению
  int n1 = u.size(); // Количество узловых векторов по первому направлению
  int n2 = v.size(); // Количество узловых векторов по второму направлению
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  int m2 = p[0].size(); // Количество контрольных точек по второму направлению
  int n1 = u.size(); // Количество узловых векторов по первому направлению
  int n2 = v.size(); // Количество узловых векторов по второму направлению
  // Находим индекс i такой, что u[i] <= u0 < u[i+1]
  int i = 0;
  while (i < n1 - 1 && u[i + 1] <= u0) i++;
  // Находим индекс j такой, что v[j] <= v0 < v[j+1]
  int j = 0;
  while (j < n2 - 1 && v[j + 1] <= v0) j++;
  // Создаем вспомогательный массив для хранения промежуточных точек
  vector<vector<Point>> q(k1 + 1);
  for (int a = 0; a <= k1; a++) {
    q[a].resize(k2 + 1);
    for (int b = 0; b <= k2; b++) {
      q[a][b] = p[i - k1 + a][j - k2 + b]; // Копируем соответствующие контрольные точки
    }
  }
  // Выполняем k1 + 1 раз алгоритм де Бора по второму параметру
  for (int a = 1; a <= k1 + 1; a++) {
    for (int b = k2; b >= a; b--) {
      for (int c = j - k2 + b; c <= j; c++) {
        // Вычисляем коэффициент альфа
        double alpha = (v0 - v[c]) / (v[c + k2 - a + 1] - v[c]);
        // Обновляем точку в массиве
        q[a][b] = q[a - 1][b] * (1 - alpha) + q[a - 1][b - 1] * alpha;
      }
    }
  }
  // Выполняем k2 + 1 раз алгоритм де Бора по первому параметру
  for (int b = 1; b <= k2 + 1; b++) {
    for (int a = k1; a >= b; a--) {
      for (int c = i - k1 + a; c <= i; c++) {
        // Вычисляем коэффициент бета
        double beta = (u0 - u[c]) / (u[c + k1 - b + 1] - u[c]);
        // Обновляем точку в массиве
        q[a][b] = q[a][b - 1] * (1 - beta) + q[a - 1][b - 1] * beta;
      }
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Обновляем точку в массиве
        q[a][b] = q[a][b - 1] * (1 - beta) + q[a - 1][b - 1] * beta;
      }
    }
  }
  // Возвращаем конечную точку в массиве
  return q[k1][k2];
}

// Функция для записи тела в файл формата NEU
void write_neu(const string& filename, const Body& body) {
  // Открываем файл для записи
  ofstream fout(filename);
  if (!fout) {
    cerr << "Error: cannot open file " << filename << endl;
    exit(1);
  }
  // Записываем заголовок файла
  fout << "        CONTROL INFO\n";
  fout << "** GAMBIT NEUTRAL FILE\n";
  fout << "PROGRAM:                Bing\n";
  fout << "VERSION:                1.0\n";
  fout << "Written by Bing on " << __DATE__ << " at " << __TIME__ << "\n";
  fout << "     NUMNP     NELEM     NGRPS    NBSETS     NDFCD     NDFVL\n";
  // Подсчитываем количество узлов и элементов в теле
  int numnp = 0; // Количество узлов
  int nelem = 0; // Количество элементов
  for (const Face& face : body.faces) {
    numnp += face.points.size(); // Каждая вершина грани - узел
    nelem += face.points.size() / 4; // Каждые четыре вершины грани - четырехугольный элемент
  }
  // Записываем количество узлов и элементов в файл
  fout << setw(10) << numnp << setw(10) << nelem << setw(10) << "1" << setw(10) << "0" << setw(10) << "3" << setw(10) << "3\n";
  fout << "ENDOFSECTION\n";
  // Записываем секцию узлов в файл
  fout << "   NODAL COORDINATES\n";
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Записываем количество узлов и элементов в файл
  fout << setw(10) << numnp << setw(10) << nelem << setw(10) << "1" << setw(10) << "0" << setw(10) << "3" << setw(10) << "3\n";
  fout << "ENDOFSECTION\n";
  // Записываем секцию узлов в файл
  fout << "   NODAL COORDINATES\n";
  // Перебираем все грани тела
  int node = 1; // Номер текущего узла
  for (const Face& face : body.faces) {
    // Перебираем все вершины грани
    for (const Point& point : face.points) {
      // Записываем номер и координаты узла в файл
      fout << setw(10) << node << setw(20) << point.x << setw(20) << point.y << setw(20) << point.z << "\n";
      node++; // Увеличиваем номер узла
    }
  }
  fout << "ENDOFSECTION\n";
  // Записываем секцию элементов в файл
  fout << "      ELEMENTS/CELLS\n";
  // Перебираем все грани тела
  int elem = 1; // Номер текущего элемента
  node = 1; // Номер текущего узла
  for (const Face& face : body.faces) {
    // Перебираем все четырехугольники грани
    for (int i = 0; i < face.points.size(); i += 4) {
      // Записываем номер и тип элемента в файл
      fout << setw(10) << elem << setw(10) << "3" << "\n";
      // Записываем номера узлов элемента в файл
      fout << setw(10) << node + i << setw(10) << node + i + 1 << setw(10) << node + i + 2 << setw(10) << node + i + 3;
      fout <<
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Записываем номера узлов элемента в файл
      fout << setw(10) << node + i << setw(10) << node + i + 1 << setw(10) << node + i + 2 << setw(10) << node + i + 3;
      fout << "\n";
      elem++; // Увеличиваем номер элемента
    }
    node += face.points.size(); // Увеличиваем номер узла на количество вершин грани
  }
  fout << "ENDOFSECTION\n";
  // Закрываем файл
  fout.close();
}

// Функция для генерации неструктурированной поверхностной прямоугольной сетки при помощи алгоритма Q-Morph для трехмерного тела
void generate_mesh(const Body& body) {
  // Алгоритм Q-Morph: https://www.researchgate.net/publication/220562461_Q-Morph_An_Indirect_Approach_to_Advancing_Front_Quad_Meshing
  // Шаг 1: Создаем начальную сетку из четырехугольников, аппроксимирующих поверхность тела
  // Этот шаг уже выполнен при чтении тела из файла формата IGES
  // Шаг 2: Определяем качество каждого четырехугольника в сетке по метрике углов
  // Метрика углов: https://www.researchgate.net/publication/220562461_Q-Morph_An_Indirect_Approach_to_Advancing_Front_Quad_Meshing
  vector<double> quality; // Вектор для хранения качества каждого четырехугольника в сетке
  for (const Face& face : body.faces) {
    for (int i = 0; i < face.points.size(); i += 4) {
      // Берем четыре вершины четырехугольника
      Point p1 = face.points[i];
      Point p2 = face.points[i + 1];
      Point p3 = face.points[i + 2];
      Point p4 = face.points[i + 3];
      // Берем четыре ребра четырехугольника
      Edge e1(p1, p2);
      Edge e2(p2, p3);
      Edge e3(p3, p4);
      Edge e4(p4, p1);
      // Вычисляем четыре угла четырехугольника в радианах
      double a1 = angle(e1, e4);
      double a2 = angle(e2, e1);
      double a3 = angle(e3, e2);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Вычисляем четыре угла четырехугольника в радианах
      double a1 = angle(e1, e4);
      double a2 = angle(e2, e1);
      double a3 = angle(e3, e2);
      double a4 = angle(e4, e3);
      // Вычисляем качество четырехугольника по метрике углов
      double q = min(a1, min(a2, min(a3, a4))) / max(a1, max(a2, max(a3, a4)));
      // Добавляем качество четырехугольника в вектор
      quality.push_back(q);
    }
  }
  // Шаг 3: Создаем очередь приоритетов для хранения четырехугольников в сетке по убыванию их качества
  priority_queue<pair<double, int>> pq; // Очередь приоритетов из пар (качество, индекс)
  for (int i = 0; i < quality.size(); i++) {
    pq.push({quality[i], i}); // Добавляем пару (качество, индекс) в очередь
  }
  // Шаг 4: Пока очередь не пуста и качество наиболее низкого четырехугольника меньше заданного порога, выполняем следующее:
  double threshold = 0.8; // Порог для качества четырехугольников
  while (!pq.empty() && pq.top().first < threshold) {
    // Извлекаем наиболее низкое качество и соответствующий индекс из очереди
    double q = pq.top().first;
    int i = pq.top().second;
    pq.pop();
    // Находим грань и четырехугольник в теле по индексу
    int f = i / (body.faces[0].points.size() / 4); // Индекс грани
    int j = i % (body.faces[0].points.size() / 4); // Индекс четырехугольника в грани
    Face& face = body.faces[f]; // Ссылка на грань
    Point& p1 = face.points[j * 4]; // Ссылка на первую вершину четырехугольника
    Point& p2 = face.points[j * 4 + 1]; // Ссылка на вторую вершину четырехугольника
    Point& p3 = face.points[j * 4 + 2]; // Ссылка на третью вершину четырехугольника
    Point& p4 = face.points[j * 4 + 3]; // Ссылка на четвертую вершину четырехугольника
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Point& p1 = face.points[j * 4]; // Ссылка на первую вершину четырехугольника
    Point& p2 = face.points[j * 4 + 1]; // Ссылка на вторую вершину четырехугольника
    Point& p3 = face.points[j * 4 + 2]; // Ссылка на третью вершину четырехугольника
    Point& p4 = face.points[j * 4 + 3]; // Ссылка на четвертую вершину четырехугольника
    // Шаг 4.1: Проверяем, является ли четырехугольник выпуклым или вогнутым
    // Для этого вычисляем знаки скалярных произведений нормалей к смежным ребрам четырехугольника
    Vector n1 = (p2 - p1) ^ face.pl; // Нормаль к ребру p1 p2
    Vector n2 = (p3 - p2) ^ face.pl; // Нормаль к ребру p2 p3
    Vector n3 = (p4 - p3) ^ face.pl; // Нормаль к ребру p3 p4
    Vector n4 = (p1 - p4) ^ face.pl; // Нормаль к ребру p4 p1
    double s1 = n1 * n2; // Скалярное произведение нормалей к ребрам p1 p2 и p2 p3
    double s2 = n2 * n3; // Скалярное произведение нормалей к ребрам p2 p3 и p3 p4
    double s3 = n3 * n4; // Скалярное произведение нормалей к ребрам p3 p4 и p4 p1
    double s4 = n4 * n1; // Скалярное произведение нормалей к ребрам p4 p1 и p1 p2
    bool convex = (s1 >= 0 && s2 >= 0 && s3 >= 0 && s4 >= 0); // Четырехугольник выпуклый, если все скалярные произведения неотрицательны
    bool concave = (s1 <= 0 && s2 <= 0 && s3 <= 0 && s4 <= 0); // Четырехугольник вогнутый, если все скалярные произведения неположительны
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool convex = (s1 >= 0 && s2 >= 0 && s3 >= 0 && s4 >= 0); // Четырехугольник выпуклый, если все скалярные произведения неотрицательны
    bool concave = (s1 <= 0 && s2 <= 0 && s3 <= 0 && s4 <= 0); // Четырехугольник вогнутый, если все скалярные произведения неположительны
    // Шаг 4.2: Если четырехугольник выпуклый, то применяем к нему операцию сглаживания
    if (convex) {
      // Операция сглаживания: https://www.researchgate.net/publication/220562461_Q-Morph_An_Indirect_Approach_to_Advancing_Front_Quad_Meshing
      // Вычисляем центр масс четырехугольника
      Point c = (p1 + p2 + p3 + p4) / 4;
      // Вычисляем новые координаты вершин четырехугольника по формуле сглаживания
      Point p1_new = c + (p1 - c) * 0.5;
      Point p2_new = c + (p2 - c) * 0.5;
      Point p3_new = c + (p3 - c) * 0.5;
      Point p4_new = c + (p4 - c) * 0.5;
      // Проверяем, что новые вершины лежат внутри или на границе тела
      bool valid = true; // Флаг валидности новых вершин
      for (const Face& f : body.faces) {
        if (!inside(p1_new, f)) valid = false; // Если вершина не лежит внутри или на границе грани, то флаг становится ложным
        if (!inside(p2_new, f)) valid = false; // Аналогично для остальных вершин
        if (!inside(p3_new, f)) valid = false;
        if (!inside(p4_new, f)) valid = false;
      }
      // Если флаг валидности истинный, то обновляем координаты вершин четырехугольника
      if (valid) {
        p1 = p1_new;
        p2 = p2_new;
        p3 = p3_new;
        p4 = p4_new;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Если флаг валидности истинный, то обновляем координаты вершин четырехугольника
      if (valid) {
        p1 = p1_new;
        p2 = p2_new;
        p3 = p3_new;
        p4 = p4_new;
        // Вычисляем новое качество четырехугольника по метрике углов
        double q_new = min(a1, min(a2, min(a3, a4))) / max(a1, max(a2, max(a3, a4)));
        // Если новое качество больше старого, то добавляем четырехугольник в очередь приоритетов с новым качеством
        if (q_new > q) {
          pq.push({q_new, i});
        }
      }
    }
    // Шаг 4.3: Если четырехугольник вогнутый, то применяем к нему операцию перестройки
    if (concave) {
      // Операция перестройки: https://www.researchgate.net/publication/220562461_Q-Morph_An_Indirect_Approach_to_Advancing_Front_Quad_Meshing
      // Находим индекс наименьшего угла четырехугольника
      int k = 0; // Индекс наименьшего угла
      double a_min = a1; // Значение наименьшего угла
      if (a2 < a_min) {
        k = 1;
        a_min = a2;
      }
      if (a3 < a_min) {
        k = 2;
        a_min = a3;
      }
      if (a4 < a_min) {
        k = 3;
        a_min = a4;
      }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      double a_min = a1; // Значение наименьшего угла
      if (a2 < a_min) {
        k = 1;
        a_min = a2;
      }
      if (a3 < a_min) {
        k = 2;
        a_min = a3;
      }
      if (a4 < a_min) {
        k = 3;
        a_min = a4;
      }
      // Находим соседний четырехугольник, разделяющий ребро с наименьшим углом
      int l = -1; // Индекс соседнего четырехугольника
      for (int m = 0; m < face.points.size(); m += 4) {
        if (m != j) { // Пропускаем сам четырехугольник
          // Берем четыре вершины соседнего четырехугольника
          Point q1 = face.points[m];
          Point q2 = face.points[m + 1];
          Point q3 = face.points[m + 2];
          Point q4 = face.points[m + 3];
          // Проверяем, есть ли общее ребро с исходным четырехугольником
          if ((p1 == q1 && p2 == q4) || (p1 == q4 && p2 == q1)) {
            l = m; // Запоминаем индекс соседнего четырехугольника
            break; // Выходим из цикла
          }
          if ((p2 == q1 && p3 == q4) || (p2 == q4 && p3 == q1)) {
            l = m;
            break;
          }
          if ((p3 == q1 && p4 == q4) || (p3 == q4 && p4 == q1)) {
            l = m;
            break;
          }
          if ((p4 == q1 && p1 == q4) || (p4 == q4 && p1 == q1)) {
            l = m;
            break;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
          if ((p3 == q1 && p4 == q4) || (p3 == q4 && p4 == q1)) {
            l = m;
            break;
          }
          if ((p4 == q1 && p1 == q4) || (p4 == q4 && p1 == q1)) {
            l = m;
            break;
          }
        }
      }
      // Если соседний четырехугольник найден, то выполняем операцию перестройки
      if (l != -1) {
        // Берем четыре вершины соседнего четырехугольника
        Point& q1 = face.points[l];
        Point& q2 = face.points[l + 1];
        Point& q3 = face.points[l + 2];
        Point& q4 = face.points[l + 3];
        // Вычисляем углы соседнего четырехугольника в радианах
        double b1 = angle(Edge(q1, q2), Edge(q4, q1));
        double b2 = angle(Edge(q2, q3), Edge(q1, q2));
        double b3 = angle(Edge(q3, q4), Edge(q2, q3));
        double b4 = angle(Edge(q4, q1), Edge(q3, q4));
        // Вычисляем качество соседнего четырехугольника по метрике углов
        double r = min(b1, min(b2, min(b3, b4))) / max(b1, max(b2, max(b3, b4)));
        // Выполняем перестройку в зависимости от индекса наименьшего угла исходного четырехугольника
        switch (k) {
          case 0: // Наименьший угол при вершине p1
            // Проверяем условие перестройки: a_min + b_min < pi
            if (a_min + min(b2, b3) < M_PI) {
              // Меняем ребро p2 p3 на ребро p1 q3
              p2 = q3;
              q3 = p1;
              // Вычисляем новые углы исходного и соседнего четырехугольников в радианах
              a1 = angle(Edge(p1, p2), Edge(p4, p1));
              a2 = angle(Edge(p2, p3), Edge(p1, p2));
              a3 = angle(Edge(p3, p4), Edge(p2, p3));
              a4 = angle(Edge(p4, p1), Edge(p3, p4));
              b1 = angle(Edge(q1, q2), Edge(q4, q1));
              b2 = angle(Edge(q2, q3), Edge(q1, q2));
              b3 = angle(Edge(q3, q4), Edge(q2, q3));
              b4 = angle(Edge(q4, q1), Edge(q3, q4));
////////////////////////////////////////////////////////////////////////////////////////////////////////
              b1 = angle(Edge(q1, q2), Edge(q4, q1));
              b2 = angle(Edge(q2, q3), Edge(q1, q2));
              b3 = angle(Edge(q3, q4), Edge(q2, q3));
              b4 = angle(Edge(q4, q1), Edge(q3, q4));
              // Вычисляем новые качества исходного и соседнего четырехугольников по метрике углов
              double q_new = min(a1, min(a2, min(a3, a4))) / max(a1, max(a2, max(a3, a4)));
              double r_new = min(b1, min(b2, min(b3, b4))) / max(b1, max(b2, max(b3, b4)));
              // Если новые качества больше старых, то добавляем четырехугольники в очередь приоритетов с новыми качествами
              if (q_new > q) {
                pq.push({q_new, i});
              }
              if (r_new > r) {
                pq.push({r_new, l});
              }
            }
            break;
          case 1: // Наименьший угол при вершине p2
            // Проверяем условие перестройки: a_min + b_min < pi
            if (a_min + min(b3, b4) < M_PI) {
              // Меняем ребро p3 p4 на ребро p2 q4
              p3 = q4;
              q4 = p2;
              // Вычисляем новые углы исходного и соседнего четырехугольников в радианах
              a1 = angle(Edge(p1, p2), Edge(p4, p1));
              a2 = angle(Edge(p2, p3), Edge(p1, p2));
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Вычисляем новые углы исходного и соседнего четырехугольников в радианах
              a1 = angle(Edge(p1, p2), Edge(p4, p1));
              a2 = angle(Edge(p2, p3), Edge(p1, p2));
              a3 = angle(Edge(p3, p4), Edge(p2, p3));
              a4 = angle(Edge(p4, p1), Edge(p3, p4));
              b1 = angle(Edge(q1, q2), Edge(q4, q1));
              b2 = angle(Edge(q2, q3), Edge(q1, q2));
              b3 = angle(Edge(q3, q4), Edge(q2, q3));
              b4 = angle(Edge(q4, q1), Edge(q3, q4));
              // Вычисляем новые качества исходного и соседнего четырехугольников по метрике углов
              double q_new = min(a1, min(a2, min(a3, a4))) / max(a1, max(a2, max(a3, a4)));
              double r_new = min(b1, min(b2, min(b3, b4))) / max(b1, max(b2, max(b3, b4)));
              // Если новые качества больше старых, то добавляем четырехугольники в очередь приоритетов с новыми качествами
              if (q_new > q) {
                pq.push({q_new, i});
              }
              if (r_new > r) {
                pq.push({r_new, l});
              }
            }
            break;
          case 2: // Наименьший угол при вершине p3
            // Проверяем условие перестройки: a_min + b_min < pi
            if (a_min + min(b4, b1) < M_PI) {
              // Меняем ребро p4 p1 на ребро p3 q1
              p4 = q1;
              q1 = p3;
              // Вычисляем новые углы исходного и соседнего четырехугольников в радианах
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Меняем ребро p4 p1 на ребро p3 q1
              p4 = q1;
              q1 = p3;
              // Вычисляем новые углы исходного и соседнего четырехугольников в радианах
              a1 = angle(Edge(p1, p2), Edge(p4, p1));
              a2 = angle(Edge(p2, p3), Edge(p1, p2));
              a3 = angle(Edge(p3, p4), Edge(p2, p3));
              a4 = angle(Edge(p4, p1), Edge(p3, p4));
              b1 = angle(Edge(q1, q2), Edge(q4, q1));
              b2 = angle(Edge(q2, q3), Edge(q1, q2));
              b3 = angle(Edge(q3, q4), Edge(q2, q3));
              b4 = angle(Edge(q4, q1), Edge(q3, q4));
              // Вычисляем новые качества исходного и соседнего четырехугольников по метрике углов
              double q_new = min(a1, min(a2, min(a3, a4))) / max(a1, max(a2, max(a3, a4)));
              double r_new = min(b1, min(b2, min(b3, b4))) / max(b1, max(b2, max(b3, b4)));
              // Если новые качества больше старых, то добавляем четырехугольники в очередь приоритетов с новыми качествами
              if (q_new > q) {
                pq.push({q_new, i});
              }
              if (r_new > r) {
                pq.push({r_new, l});
              }
            }
            break;
          case 3: // Наименьший угол при вершине p4
            // Проверяем условие перестройки: a_min + b_min < pi
            if (a_min + min(b1, b2) < M_PI) {
              // Меняем ребро p1 p2 на ребро p4 q2
              p1 = q2;
              q2 = p4;
              // Вычисляем новые углы исходного и соседнего четырехугольников в радианах
////////////////////////////////////////////////////////////////////////////////////
