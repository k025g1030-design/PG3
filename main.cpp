#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>   
#include <chrono>   

typedef void (*ResultCallback)(int, int);

void JudgeAndPrint(int user_guess, int dice_result) {
    std::cout << "\n出目は「" << dice_result << "」でした！" << std::endl;

    int result_type = dice_result % 2;

    if (user_guess == result_type) {
        std::cout << "正解" << std::endl;
    } else {
        std::cout << "不正解" << std::endl;
    }
}


void PlayGame(int user_guess, ResultCallback callback) {
    std::cout << "サイコロを振っています......（結果発表まで3秒お待ちください）" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(3));

    int dice_result = (std::rand() % 6) + 1;

    callback(user_guess, dice_result);
}

int main() {

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int user_input;

    std::cout << "=== 丁半（偶数・奇数）当てゲーム ===" << std::endl;
    std::cout << "丁(偶数)なら「0」、半(奇数)なら「1」を入力してください: ";

    if (!(std::cin >> user_input) || (user_input != 0 && user_input != 1)) {
        std::cout << "入力エラーです。プログラムを再起動して 0 または 1 を入力してください。" << std::endl;
        return 1;
    }

    PlayGame(user_input, JudgeAndPrint);

    return 0;
}