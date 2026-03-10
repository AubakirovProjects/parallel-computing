#include <iostream>
#include <random>
#include "TaskTimer4.hpp"

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 1e6);
    std::vector<int> arr;

    for (int i = 0; i < 1e5; i++) {
        arr.push_back(dist(gen));
    }

    TaskTimer tt;

    std::chrono::milliseconds measuredTime = tt.measureSorting(arr);
    std::cout << "Время выполнения сортировки: " << measuredTime.count() << " мс" << std::endl;

    for (int& el : arr) el = dist(gen);
    measuredTime = tt.measureSorting(arr);
    std::cout << "Время выполнения сортировки: " << measuredTime.count() << " мс" << std::endl;


    return 0;
}