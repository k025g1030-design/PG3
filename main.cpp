#include <iostream>
#include <ctime>
#include <thread>

class Enemy {
private:
    using StateHandler = void (Enemy::*)();


    StateHandler state_;

    // 1. 接近状態
    void Approach() {
        std::cout << "[状態: 接近]" << std::endl;
        // 次の状態（射撃）へ遷移
        state_ = &Enemy::Shoot;
        // 接近のための処理
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // 2. 射撃状態
    void Shoot() {
        std::cout << "[状態: 射撃]" << std::endl;
        // 次の状態（離脱）へ遷移
        state_ = &Enemy::Leave;
        // 射撃のための処理
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 3. 離脱状態
    void Leave() {
        std::cout << "[状態: 離脱]" << std::endl;
        // ループを止めるために nullptr にするか
        state_ = nullptr;
        // 離脱のための処理
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

public:
    Enemy() : state_(&Enemy::Approach) {}

    void Update() {
        if (state_ != nullptr) {
            // メンバ関数ポインタの呼び
            (this->*state_)();
        } else {
            std::cout << "敵はすでに離脱しています。" << std::endl;
        }
    }

    // すべての状態遷移が終わったかチェック
    bool Finalize() const {
        return state_ == nullptr;
    }
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Enemy enemy;

    std::cout << "--- 敵の行動開始 ---" << std::endl;

    while (!enemy.Finalize()) {
        enemy.Update();
    }

    std::cout << "--- 敵の行動終了 ---" << std::endl;

    return 0;
}