#include <iostream>



template <typename T>
T Min(T a, T b) {
	return (a < b) ? a : b;
}

template <>
const char* Min<const char*>(const char* a, const char* b) {
	return "数字以外は代入できません";
}




int main() {

	std::cout << "整数型：" << Min(6, 9) << std::endl;
	std::cout << "浮動小数点型：" << Min(3.14f, 8.71f) << std::endl;
	std::cout << "倍精度浮動小数点型：" << Min(3.14, 1.71) << std::endl;
	std::cout << "文字型：" << Min("A", "B") << std::endl;

    return 0;
}

