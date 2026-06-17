#include <iostream>

template<typename T1, typename T2>
class Math {
private:
    T1 number1_;
    T2 number2_;

public:
    Math(T1 n1, T2 n2) : number1_(n1), number2_(n2) {}
    auto Min() -> decltype(number1_) {
        return (number1_ < number2_) ? number1_ : number2_;
    }
};

int main() {
    std::cout << "--- Math判断開始 ---" << std::endl;

    Math<int, int> mathInt(5, 10);
    std::cout << "[int]最小値: " << mathInt.Min() << std::endl;
    Math<double, double> mathDouble(3.14, 2.71);
    std::cout << "[double]最小値: " << mathDouble.Min() << std::endl;
    Math<float, float> mathFloat(0.14f, 2.71f);
    std::cout << "[float]最小値: " << mathFloat.Min() << std::endl;

    return 0;
}