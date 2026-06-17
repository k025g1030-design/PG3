#include <iostream>
#include <ctime>
#include <thread>
#include <vector>

class Cast {
public:
    virtual void Spell() = 0;
};

class FireClazz : public Cast {
public:
    void Spell() override {
        std::cout << "灰燼の園に灯をともせ。天を焦がす赤き翼、我が拳に宿りて業火と化せ。焼き尽くせ、地上のすべてを。" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "――『プロミネンス・フレイム』" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
};

class WaterClazz : public Cast {
public:
    void Spell() override {
        std::cout << "静寂なる水の調べ、万物を浸す慈愛と深淵よ。巡り来る奔流となりて、理不尽を飲み込み、氷の棺へと変貌せよ。" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "――『アビス・ゼロ』" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
};

class WindClazz : public Cast {
public:
    void Spell() override {
        std::cout << "見えざる刃、大気そのものよ。切り裂け、虚空の彼方まで。我が意志に纏（まと）い、世界を切り刻む旋風となれ。" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "――『ゲイル・スラッシュ』" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
};



int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::cout << "--- 呪文を唱える ---" << std::endl;
    Cast* cast = nullptr;
    std::vector<Cast*> casts = { new FireClazz(), new WaterClazz(), new WindClazz() };

    for (const auto& c : casts) { 
        c->Spell();
    }

    return 0;
}