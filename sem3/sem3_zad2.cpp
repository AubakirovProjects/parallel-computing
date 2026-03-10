#include <iostream>
#include <thread>
#include <chrono>

int main() {
    int seconds;
    
    std::cout << "Введите количество секунд: ";
    std::cin >> seconds;
    
    std::cout << "\nТаймер запущен!\n" << std::endl;
    
    for (int i = seconds; i > 0; i--) {
        std::cout << "Осталось: " << i << " секунд" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    std::cout << "\nВремя вышло!" << std::endl;
    
    return 0;
}