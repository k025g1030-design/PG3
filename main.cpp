#include <iostream>
#include <string>


class Math {
public:
    template<class T1, class T2>
    static typename std::common_type<T1, T2>::type Min(T1 a, T2 b) {
        return (a < b) ? a : b;
    }
};


int main() {
    std::cout << "--- 判断開始 ---" << std::endl;

    auto int2int = Math::Min(3, 2);
    std::cout << "最小値: " << int2int << std::endl;

    auto int2float = Math::Min(3, 5.5f);
    std::cout << "最小値: " << int2float << std::endl;

    auto int2double = Math::Min(4, 3.0);
    std::cout << "最小値: " << int2double << std::endl;

    auto float2float = Math::Min(3.0f, 5.5f);
    std::cout << "最小値: " << float2float << std::endl;

    auto float2double = Math::Min(7.3f, 1.5);
    std::cout << "最小値: " << float2double << std::endl;

    auto double2double = Math::Min(3.0, 2.0);
    std::cout << "最小値: " << double2double << std::endl;

    return 0;
}