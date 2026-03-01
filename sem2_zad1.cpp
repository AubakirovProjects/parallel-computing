#include <iostream>

double inputMarks(const char* message) {
    double value;
    do {
        std::cout << message;
        std::cin >> value;
        if (value < 0 || value > 5)
            std::cout << "Балл должен быть от 0 до 5" << std::endl;
        else
            break;
    } while (true);
    return value;
}

double average(double* arr, int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum / n;
}

double max(double* arr, int n) {
    double maxVal = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] > maxVal) 
            maxVal = arr[i];
    }
    return maxVal;
}

double min(double* arr, int n) {
    double minVal = 5;
    for (int i = 1; i < n; i++) {
        if (arr[i] < minVal) 
            minVal = arr[i];
    }
    return minVal;
}


int countAbove(double* arr, int n, double border) {
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] >= border) count++;
    }
    return count;
}

int main() {
    int N;
    
    std::cout << "Введите количество студентов: ";
    std::cin >> N;
    
    if (N <= 0) {
        std::cout << "Ошибка! Количество студентов должно быть больше 0" << std::endl;
        return 0;
    }
    
    double* marks = new double[N];
    
    for (int i = 0; i < N; i++) {
        char message[50];
        sprintf(message, "Введите балл студента %d: ", i + 1);
        marks[i] = inputMarks(message);
    }
    
    std::cout << "Результаты:" << std::endl;
    std::cout << "Средний балл: " << average(marks, N) << std::endl;
    std::cout << "Максимальный балл: " << max(marks, N) << std::endl;
    std::cout << "Минимальный балл: " << min(marks, N) << std::endl;
    
    double border;
    std::cout << "Введите порог для подсчета: ";
    std::cin >> border;
    std::cout << "Студентов выше порога: " << countAbove(marks, N, border) << std::endl;
    
    delete[] marks;
    
    return 0;
}