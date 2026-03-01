#include <iostream>
#include <vector>
#include <algorithm>

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

double subjectAverage(const std::vector<std::vector<double>>& allMarks, int subjectIndex) {
    double sum = 0;
    for (int i = 0; i < allMarks.size(); i++) {
        sum += allMarks[i][subjectIndex];
    }
    return sum / allMarks.size();
}

int findBestStudent(const std::vector<std::vector<double>>& allMarks) {
    int bestIndex = 0;
    double bestAverage = 0;
    
    for (int i = 0; i < allMarks.size(); i++) {
        double currentAverage = studentAverage(allMarks[i]);
        if (currentAverage > bestAverage) {
            bestAverage = currentAverage;
            bestIndex = i;
        }
    }
    return bestIndex;
}

int main() {
    int studentsCount, subjectsCount;
    
    std::cout << "Введите количество студентов: ";
    std::cin >> studentsCount;
    std::cout << "Введите количество предметов: ";
    std::cin >> subjectsCount;
    
    std::vector<std::vector<double>> allMarks(studentsCount, std::vector<double>(subjectsCount));
    
    for (int student = 0; student < studentsCount; student++) {
        std::cout << "Студент " << student + 1 << ":" << std::endl;
        for (int subject = 0; subject < subjectsCount; subject++) {
            char message[50];
            sprintf(message, "  Оценка по предмету %d: ", subject + 1);
            allMarks[student][subject] = inputMarks(message);
        }
    }
    

    std::vector<std::pair<int, double>> studentRatings;
    
    for (int i = 0; i < studentsCount; i++) {
        studentRatings.push_back({i, studentAverage(allMarks[i])});
    }
    
    std::sort(studentRatings.begin(), studentRatings.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
            if (a.second != b.second) {
                return a.second > b.second;
            }
            return a.first < b.first;
        }
    );
    
    std::cout << "Средние баллы студентов" << std::endl;
    for (int student = 0; student < studentsCount; student++) {
        double average = studentAverage(allMarks[student]);
        std::cout << "Студент " << student + 1 << ": " << average << std::endl;
    }
    
    std::cout << "Средние баллы по предметам" << std::endl;
    for (int subject = 0; subject < subjectsCount; subject++) {
        double average = subjectAverage(allMarks, subject);
        std::cout << "Предмет " << subject + 1 << ": " << average << std::endl;
    }
    
    int bestStudent = findBestStudent(allMarks);
    std::cout << "Лучший студент" << std::endl;
    std::cout << "Студент " << bestStudent + 1 
              << " со средним баллом " << studentAverage(allMarks[bestStudent]) << std::endl;
    
    std::cout << "Отсортированный список" << std::endl;
    for (int i = 0; i < studentRatings.size(); i++) {
        int studentNumber = studentRatings[i].first + 1; 
        double average = studentRatings[i].second;
        std::cout << "   " << i + 1 << "    |  Студент " << studentNumber << "  |     " << average << std::endl;
    }
    
    double border;
    std::cout << "Введите порог среднего балла для удаления студентов: ";
    std::cin >> border;
    
    int initialSize = allMarks.size();
    
    int GoodMarks = 0;
    int BadMarks = 0;
    
    for (int i = 0; i < allMarks.size(); i++) {
        double avg = studentAverage(allMarks[i]);
        if (avg >= 4.5) 
            GoodMarks++;
        if (avg < 3.0) 
            BadMarks++;
    }
    
    std::cout << "Статистика до удаления:" << std::endl;
    std::cout << "  Отличники (>= 4.5): " << GoodMarks << std::endl;
    std::cout << "  Близкие к отчислению (< 3.0): " << BadMarks << std::endl;
        
    auto newEnd = std::remove_if(allMarks.begin(), allMarks.end(), [border](const std::vector<double>& studentMarks) {
            return studentAverage(studentMarks) < border;
        }
    );
    
    allMarks.erase(newEnd, allMarks.end());
    
    studentsCount = allMarks.size();

    std::cout << "Статистика почле удаления:" << std::endl;
    std::cout << "  Удалено студентов: " << initialSize - studentsCount << std::endl;
    std::cout << "  Осталось студентов: " << studentsCount << std::endl;
    
    if (studentsCount > 0) {
        std::cout << "Оставшиеся студенты:" << std::endl;
        for (int student = 0; student < studentsCount; student++) {
            double avg = studentAverage(allMarks[student]);
            std::cout << "  Студент " << student + 1 << " (средний балл: " << avg << ")" << std::endl;
            
            std::cout << "    Оценки: ";
            for (int subject = 0; subject < subjectsCount; subject++) {
                std::cout << allMarks[student][subject] << " ";
            }
            std::cout << std::endl;
        }
        
        studentRatings.clear();
        for (int i = 0; i < studentsCount; i++) {
            studentRatings.push_back({i, studentAverage(allMarks[i])});
        }
        
        std::sort(studentRatings.begin(), studentRatings.end(),
            [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
                if (a.second != b.second) {
                    return a.second > b.second;
                }
                return a.first < b.first;
            }
        );
        
        std::cout << "Обновленный рейтинг студентов:" << std::endl;
        for (int i = 0; i < studentRatings.size(); i++) {
            int studentNumber = studentRatings[i].first + 1;
            double average = studentRatings[i].second;
            std::cout << "  " << i + 1 << ". Студент " << studentNumber << " - " << average << std::endl;
        }
    } else {
        std::cout << "Все студенты были удалены!" << std::endl;
    }
    
    return 0;
}