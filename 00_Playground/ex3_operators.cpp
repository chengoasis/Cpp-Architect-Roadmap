#include <iostream>

class Point {
 public:
  Point(int x, int y) : x_(x), y_(y) {}

  // 1. 重载 + 
  // 场景: Point p3 = p1 + p2;
  // 这里 this 是 p1, other 是 p2
  // TODO: 请实现返回一个新的 Point，其 x = this->x_ + other.x_ ...
  Point operator+(const Point& other) const {
      return Point(x_ + other.x_, y_ + other.y_);
  }

  // 2. 重载 == 
  // 场景: if (p1 == p2) ...
  bool operator==(const Point& other) const {
      return x_ == other.x_ && y_ == other.y_;
  }

  // 为了让外面的全局函数能访问我的私有变量 x_, y_
  // 我必须把这个全局函数声明为"友元" (Friend)
  friend std::ostream& operator<<(std::ostream& os, const Point& p);

 private:
  int x_;
  int y_;
};

/*
  为什么不能写成成员函数？
  因为成员函数的左操作数必须是对象本身(Point)。
  如果：
  class Point {
  public:
    std::ostream& operator<<(std::ostream& os) {
      os << "(" << x_ << ", " << y_ << ")";
        return os;
    }
  };
  那么调用的时候必须这样写：p1 << std::cout;  不符合习惯！
  但这里的左操作数是 std::cout (类型是 ostream)。
  所以必须写成全局函数。
*/

// 3. 重载 <<  (流输出)
// 想要自定义类对象与标准输出流（如 cout）的交互时，需要重载 << 运算符
// 第一个参数：std::ostream& os - 输出流对象
// 第二个参数：const Point& p - 要输出的Point对象
// 返回值：std::ostream& - 返回流的引用以支持链式调用
// cout << p1 就等价于 operator<<(cout, p1)，参数顺序完全正确。
std::ostream& operator<<(std::ostream& os, const Point& p) {
    // TODO: 请利用 os 打印: (x, y)
    // 提示: 用法和 std::cout 一样，比如 os << "hello";
    // 记得最后 return os; 以支持链式调用 (cout << p1 << p2)
    os << "(" << p.x_ << ", " << p.y_ << ")";
    return os;
}

int main() {

  Point p1(10, 20);
  Point p2(5, 5);

  Point p3 = p1 + p2;
  
  // 现在用户可以自由控制格式了
  std::cout << "p1: " << p1 << std::endl;
  std::cout << "p2: " << p2 << std::endl;
  std::cout << "p3: " << p3 << " is the sum." << std::endl;

  Point p4(15, 25);
  if (p3 == p4) {
    std::cout << "验证成功" << std::endl;
  } else {
    std::cout << "验证失败" << std::endl;
  }

  return 0;
}