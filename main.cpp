#include <iostream>

/**
株式会社かまトッの賃金体系はとても特殊で、次のように決まる。

1)一般的な賃金体系(時給1,072円)で働くか、再帰的な賃金で働くかを選べる
2)再帰的な賃金体系の場合、最初の1時間働いたときの時給は100円である
3)再帰的な賃金体系の場合、次の1時間からは時給はアップして「前の1時間でもらった時給 * 2 - 50円」となる
**/

const int standardHourlyWage = 1072;
const int initialHourlyWage = 100;
const int totalStandardHours = 8;

void compareWages(int hour, int current, int totalStandard, int totalRecursive) {
    totalStandard += standardHourlyWage;
    totalRecursive += current;

    std::cout << "" << hour << "時間\t"
        << totalStandard << " 円\t\t"
        << totalRecursive << " 円\t\t"
        << "(" << current << " 円)\n";

    if (totalRecursive > totalStandard) {
        std::cout << "\n=> 結論：仕事 " << hour << " 時間後、再帰的な体系の方が、一般的な体系よりも時給が高くなる。\n";
        return; 
    }

    long long next_recursive_wage = (current * 2) - 50;

    compareWages(hour + 1, next_recursive_wage, totalStandard, totalRecursive);

}

int main() {

    std::cout << "時間\t一般的な賃金体系\t再帰的な賃金体系\t(再帰的な現在の時給)\n";
    std::cout << "------------------------------------------------------\n";

    compareWages(1, 100, 0, 0);
    return 0;
}

