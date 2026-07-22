#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex gMutex;
std::condition_variable gCondition;
int gCurrentThread = 1;

void PrintThread(int threadNumber) {
    // lock the mutex to ensure exclusive access to shared resources
    std::unique_lock<std::mutex> lock(gMutex);

    gCondition.wait(lock, [threadNumber]() {
        return gCurrentThread == threadNumber;
        });

    std::cout << "thread " << threadNumber << std::endl;
    ++gCurrentThread;

    // unlock the mutex and notify other threads that they can proceed
    lock.unlock();
    gCondition.notify_all();
}

int main() {
    std::thread thread1(PrintThread, 1);
    std::thread thread2(PrintThread, 2);
    std::thread thread3(PrintThread, 3);

    thread1.join();
    thread2.join();
    thread3.join();

    return 0;
}