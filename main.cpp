#include <iostream>
#include <list>
#include <cstring>

// リストの内容を表示する共通関数
void printStations(const std::list<const char*>& stations, int year) {
    std::cout << "--- " << year << " ---" << std::endl;
    int index = 1;
    for (const char* station : stations) {
        std::cout << "JY" << index << ". " << station << std::endl;
        index++;
    }
    std::cout << std::endl;
}

// 指定した基準駅の「前」に新しい駅を挿入するヘルパー関数
void insertStationBefore(std::list<const char*>& stations, const char* targetStation, const char* newStation) {
    for (auto it = stations.begin(); it != stations.end(); ++it) {
        if (std::strcmp(*it, targetStation) == 0) {
            stations.insert(it, newStation);
            return; // 挿入完了後にループを抜ける
        }
    }
    // 基準駅が見つからなかった場合のエラーハンドリング（必要に応じて拡張可能）
    std::cerr << "Error: Station '" << targetStation << "' not found." << std::endl;
}

std::list<const char*> InitializeYamanoteLine() {
    std::list<const char*> yamanoteLine;
    yamanoteLine.push_back("Tokyo");
    yamanoteLine.push_back("Kanda");
    yamanoteLine.push_back("Akihabara");
    yamanoteLine.push_back("Okachimachi");
    yamanoteLine.push_back("Ueno");
    yamanoteLine.push_back("Uguisudani");
    yamanoteLine.push_back("Nippori");
    //yamanoteLine.push_back("Nishi-Nippori");
    yamanoteLine.push_back("Tabata");
    yamanoteLine.push_back("Komagome");
    yamanoteLine.push_back("Sugamo");
    yamanoteLine.push_back("Otsuka");
    yamanoteLine.push_back("Ikebukuro");
    yamanoteLine.push_back("Mejiro");
    yamanoteLine.push_back("Takadanobaba");
    yamanoteLine.push_back("Shin-Okubo");
    yamanoteLine.push_back("Shinjuku");
    yamanoteLine.push_back("Yoyogi");
    yamanoteLine.push_back("Harajuku");
    yamanoteLine.push_back("Shibuya");
    yamanoteLine.push_back("Ebisu");
    yamanoteLine.push_back("Meguro");
    yamanoteLine.push_back("Gotanda");
    yamanoteLine.push_back("Osaki");
    yamanoteLine.push_back("Shinagawa");
    //yamanoteLine.push_back("Takanawa Gateway");
    yamanoteLine.push_back("Tamachi");
    yamanoteLine.push_back("Hamamatsucho");
    yamanoteLine.push_back("Shimbashi");
    yamanoteLine.push_back("Yurakucho");
    return yamanoteLine;
}

int main() {
    // 双方向リストの宣言 (std::stringの代わりにconst char*を使用)
    std::list<const char*> yamanoteLine = InitializeYamanoteLine();


    // [1] 1970年の駅一覧を表示
    printStations(yamanoteLine, 1970);

    // [2] 1971年: 西日暮里駅の開業に伴う挿入
    insertStationBefore(yamanoteLine, "Tabata", "Nishi-Nippori");

    // [3] 2019年の駅一覧を表示
    printStations(yamanoteLine, 2019);

    // [4] 2020年: 高輪ゲートウェイ駅の開業に伴う挿入
    insertStationBefore(yamanoteLine, "Shinagawa", "Takanawa Gateway");

    // [5] 2022年の駅一覧を表示
    printStations(yamanoteLine, 2022);

    return 0;
}