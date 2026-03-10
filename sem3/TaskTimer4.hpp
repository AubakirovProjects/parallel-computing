#ifndef TASKTIMER_H
#define TASKTIMER_H
#include <chrono>
#include <vector>
#include <algorithm>

class TaskTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point stopTime;
public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        stopTime = std::chrono::high_resolution_clock::now();
    }

    std::chrono::milliseconds getDuration() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime);
    }

    std::chrono::milliseconds measureSorting(std::vector<int>& arr) {
        start();
        std::sort(arr.begin(), arr.end(), [](int a, int b) {
            return a < b;
        });
        stop();

        return getDuration();
    }
};

#endif