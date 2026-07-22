#include <chrono>
#include <iostream>
#include <string>
#include <utility>

int main() {

    std::string a(1000000, 'a');


    const auto copyStart = std::chrono::steady_clock::now();
    std::string copiedString(a);
    const auto copyEnd = std::chrono::steady_clock::now();

    const auto moveStart = std::chrono::steady_clock::now();
    std::string movedString(std::move(a));
    const auto moveEnd = std::chrono::steady_clock::now();

    const auto copyTime =
        std::chrono::duration_cast<std::chrono::microseconds>(
            copyEnd - copyStart
        );

    const auto moveTime =
        std::chrono::duration_cast<std::chrono::microseconds>(
            moveEnd - moveStart
        );

    std::cout << "copy time : "
        << copyTime.count() << " μs\n";

    std::cout << "move time : "
        << moveTime.count() << " μs\n";

    volatile std::size_t totalSize =
        copiedString.size() + movedString.size();
    (void)totalSize;

    return 0;
}