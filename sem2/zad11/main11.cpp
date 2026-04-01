#include "Student11.hpp"
#include "Group11.hpp"
#include "FileManager11.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== Задача 11: Демонстрация работы системы ===" << std::endl;
    
    Student Aubakirov("Аубакиров", "1");
    Aubakirov.addMark(4.8);
    Aubakirov.addMark(4.6);
    Aubakirov.addMark(5.0);
    
    Student Mlikov("Мликов", "2");
    Mlikov.addMark(4.3);
    Mlikov.addMark(3.5);
    Mlikov.addMark(3.8);
    
    Student Belousov("Белоусов", "3");
    Belousov.addMark(2.5);
    Belousov.addMark(3.9);
    Belousov.addMark(4.5);
    
    Student Vinogradov("Виноградов", "4");
    Vinogradov.addMark(3.2);
    Vinogradov.addMark(4.0);
    Vinogradov.addMark(5.0);
    
    Student Bobrov("Бобров", "5");
    Bobrov.addMark(3.0);
    Bobrov.addMark(3.5);
    Bobrov.addMark(4.5);
    
    Student Lesnikov("Лесников", "6");
    Lesnikov.addMark(4.5);
    Lesnikov.addMark(4.7);
    Lesnikov.addMark(4.2);
    
    Student Chernenkov("Черненков", "7");
    Chernenkov.addMark(3.5);
    Chernenkov.addMark(4.0);
    Chernenkov.addMark(45);
    
    Student Dykin("Дыкин", "8");
    Dykin.addMark(4.4);
    Dykin.addMark(3.5);
    Dykin.addMark(5.0);
    
    Student Sidorov("Сидоров", "9");
    Sidorov.addMark(2.6);
    Sidorov.addMark(5.0);
    Sidorov.addMark(3.5);
    
    Student Sidorin("Сидорин", "10");
    Sidorin.addMark(4.5);
    Sidorin.addMark(3.5);
    Sidorin.addMark(4.6);

    std::vector<Student*> students = {
        &Aubakirov, &Mlikov, &Belousov, &Vinogradov, &Bobrov, &Lesnikov, &Chernenkov, &Dykin, &Sidorov, &Sidorin
    };
    
    Group Banya("Баня", students);

    std::cout << "\n--- Исходная группа ---" << std::endl;
    Banya.printAllStudents();
    
    std::cout << "\n--- После сортировки по среднему баллу ---" << std::endl;
    Banya.sortStudentsByAverage();
    Banya.printAllStudents();
    
    std::cout << "\n--- После фильтрации (порог 3.6) ---" << std::endl;
    Banya.filterStudents(3.6);
    Banya.printAllStudents();
    
    std::cout << "\n--- Сохранение группы в файл ---" << std::endl;
    FileManager::saveGroup(Banya, "group.dat");
    
    std::cout << "\n--- Загрузка группы из файла ---" << std::endl;
    Group loadedGroup("");
    if (FileManager::loadGroup(loadedGroup, "group.dat")) {
        std::cout << "Загруженная группа: " << loadedGroup.getName() << std::endl;
        std::cout << "Количество студентов: " << loadedGroup.getSize() << std::endl;
        loadedGroup.printAllStudents();
    }

    std::cout << "\n--- Статистика группы ---" << std::endl;
    std::cout << "Отличников (>= 4.5): " << Banya.countExcellentStudents() << std::endl;
    std::cout << "Риск отчисления (< 3.0): " << Banya.countPoorStudents() << std::endl;
    std::cout << "Средний балл группы: " << Banya.getGroupAverage() << std::endl;

    return 0;
}