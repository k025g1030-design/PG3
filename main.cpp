#include <iostream>

template<typename T>
class Math {
private:
    T number1_;
    T number2_;

    

public:
    Math(T n1, T n2) : number1_(n1), number2_(n2) {}
    T Min() {
        return (number1_ < number2_) ? number1_ : number2_;
    }
};

int main() {
    std::cout << "--- Math判断開始 ---" << std::endl;

    Math<int> mathInt(5, 10);
    std::cout << "[int]最小値: " << mathInt.Min() << std::endl;
    Math<double> mathDouble(3.14, 2.71);
    std::cout << "[double]最小値: " << mathDouble.Min() << std::endl;
    Math<float> mathFloat(0.14f, 2.71f);
    std::cout << "[float]最小値: " << mathFloat.Min() << std::endl;

    return 0;
}