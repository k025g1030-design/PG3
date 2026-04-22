#include <iostream>
#include <variant>



template <typename T>
std::variant<T, std::string> MinPlus(T a, T b) {
	if constexpr (std::is_same_v<T, char>) {
		return "数字以外は代入できません";
	}
	return (a < b) ? a : b;
}

void print_result(const auto& result) {
	std::visit([](auto&& arg) {
		std::cout << arg;
	}, result);
	std::cout << std::endl;
}

template <typename T>
T Min(T a, T b) {
	return (a < b) ? a : b;
}

std::string Min(char a, char b) {
	return "数字以外は代入できません";
}


int main() {

	std::cout << "整数型：";
	print_result(MinPlus(3, 5));

	std::cout << "浮動小数点型：";
	print_result(MinPlus(3.14f, 2.71f));

	std::cout << "倍精度浮動小数点型：";
	print_result(MinPlus(3.14, 2.71));

	std::cout << "文字型：";
	print_result(MinPlus('A', 'B'));


	std::cout << "整数型：" << Min(6, 9) << std::endl;
	std::cout << "浮動小数点型：" << Min(3.14f, 8.71f) << std::endl;
	std::cout << "倍精度浮動小数点型：" << Min(3.14, 1.71) << std::endl;
	std::cout << "文字型：" << Min('A', 'B') << std::endl;

    return 0;
}

