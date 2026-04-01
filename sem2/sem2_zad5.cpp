#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

#pragma pack(push, 1)
struct FileHeader {
    char signature[3];
    int version;
    int studentsCount;
};
#pragma pack(pop)

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

double studentAverage(const std::vector<double>& studentMarks) {
    double sum = 0;
    for (int i = 0; i < studentMarks.size(); i++) {
        sum += studentMarks[i];
    }
    return sum / studentMarks.size();
}

void saveToFile(const std::vector<std::vector<double>>& allMarks, int subjectsCount) {
    std::string filename;
    std::cout << "Введите имя файла для сохранения: ";
    std::cin >> filename;
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла для записи!" << std::endl;
        return;
    }
    
    FileHeader header;
    header.signature[0] = 'A';
    header.signature[1] = 'B';
    header.signature[2] = 'C';
    header.version = 1;
    header.studentsCount = allMarks.size();
    
    std::cout << "Размер структуры заголовка: " << sizeof(FileHeader) << " байт" << std::endl;
    
    file.write((char*)&header, sizeof(FileHeader));
    file.write((char*)&subjectsCount, sizeof(int));
    
    for (int i = 0; i < allMarks.size(); i++) {
        for (int j = 0; j < subjectsCount; j++) {
            file.write((char*)&allMarks[i][j], sizeof(double));
        }
    }
    
    file.close();
    std::cout << "Данные успешно сохранены в файл " << filename << std::endl;
}

bool loadFromFile(std::vector<std::vector<double>>& allMarks, int& studentsCount, int& subjectsCount) {
    std::string filename;
    std::cout << "Введите имя файла для загрузки: ";
    std::cin >> filename;
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла для чтения!" << std::endl;
        return false;
    }
    
    FileHeader header;
    file.read((char*)&header, sizeof(FileHeader));
    
    if (header.signature[0] != 'A' || header.signature[1] != 'B' || header.signature[2] != 'C') {
        std::cout << "Ошибка: неверная сигнатура файла!" << std::endl;
        file.close();
        return false;
    }
    
    std::cout << "Сигнатура файла корректна" << std::endl;
    std::cout << "Версия формата: " << header.version << std::endl;
    std::cout << "Количество студентов в файле: " << header.studentsCount << std::endl;
    
    file.read((char*)&subjectsCount, sizeof(int));
    studentsCount = header.studentsCount;
    
    allMarks.clear();
    allMarks.resize(studentsCount, std::vector<double>(subjectsCount));
    
    for (int i = 0; i < studentsCount; i++) {
        for (int j = 0; j < subjectsCount; j++) {
            file.read((char*)&allMarks[i][j], sizeof(double));
        }
    }
    
    file.close();
    std::cout << "Данные успешно загружены из файла " << filename << std::endl;
    return true;
}

int main() {
    int studentsCount, subjectsCount;
    std::vector<std::vector<double>> allMarks;
    
    std::cout << "1. Ввести новые данные" << std::endl;
    std::cout << "2. Загрузить данные из файла" << std::endl;
    std::cout << "Ваш выбор: ";
    
    int choice;
    std::cin >> choice;
    
    if (choice == 2) {
        if (!loadFromFile(allMarks, studentsCount, subjectsCount)) {
            std::cout << "Не удалось загрузить данные." << std::endl;
            return 1;
        }
    } else {
        std::cout << "Введите количество студентов: ";
        std::cin >> studentsCount;
        std::cout << "Введите количество предметов: ";
        std::cin >> subjectsCount;
        
        if (studentsCount <= 0 || subjectsCount <= 0) {
            std::cout << "Ошибка! Количество должно быть больше 0" << std::endl;
            return 1;
        }
        
        allMarks.assign(studentsCount, std::vector<double>(subjectsCount));
        
        std::cout << "\nВвод оценок" << std::endl;
        for (int student = 0; student < studentsCount; student++) {
            std::cout << "\nСтудент " << student + 1 << ":" << std::endl;
            for (int subject = 0; subject < subjectsCount; subject++) {
                char message[50];
                sprintf(message, "  Оценка по предмету %d: ", subject + 1);
                allMarks[student][subject] = inputMarks(message);
            }
        }
        
        saveToFile(allMarks, subjectsCount);
    }
    
    std::cout << "\nСредние баллы студентов" << std::endl;
    for (int student = 0; student < studentsCount; student++) {
        double average = studentAverage(allMarks[student]);
        std::cout << "Студент " << student + 1 << ": " << average << std::endl;
    }
    
    return 0;
}