#include <iostream>
#include <chrono>

int main() {
    int N;
    std::cout << "Введите натуральное число: ";
    std::cin >> N;

    std::chrono::seconds seconds(N);

    std::chrono::hours hours = std::chrono::duration_cast<std::chrono::hours>(seconds);
    seconds -= std::chrono::duration_cast<std::chrono::seconds>(hours);
    std::chrono::minutes minutes = std::chrono::duration_cast<std::chrono::minutes>(seconds);
    seconds -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

    std::cout << hours.count() << " час " << minutes.count() << " минута " << seconds.count() << " секунда" << std::endl;

    return 0;
}