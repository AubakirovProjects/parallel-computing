#include <iostream>
#include <chrono>
#include <thread>

long long calculateSum(int N) {
    long long sum = 0;
    for (int i = 1; i <= N; i++) {
        sum += i;
    }
    return sum;
}

int main() {
    int N;
    
    std::cout << "Введите число N: ";
    std::cin >> N;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    long long result = calculateSum(N);
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Сумма чисел от 1 до " << N << " = " << result << std::endl;
    std::cout << "Время выполнения: " << duration.count() << " миллисекунды" << std::endl;
    
    return 0;
}