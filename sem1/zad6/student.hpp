#include <iostream>
#include <vector>
#include <string>

class Student {
private:
    std::string name;            
    std::vector<double> marks;      

public:
    explicit Student(const std::string& studentName) : name(studentName), marks() {}
    
    explicit Student(const std::string& studentName, const std::vector<double>& studentMarks) 
        : name(studentName), marks(studentMarks) {}

    ~Student() {}

    double average() const {
        if (marks.empty()) return 0.0;
        
        double sum = 0;
        for (double mark : marks) {
            sum += mark;
        }
        return sum / marks.size();
    }

    void addMark(double mark) {
        if (mark >= 0 && mark <= 5) {
            marks.push_back(mark);
            std::cout << "Оценка " << mark << " добавлена" << std::endl;
        } else {
            std::cout << "Ошибка! Оценка должна быть от 0 до 5" << std::endl;
        }
    }

    void printInfo() const {
        std::cout << "Студент " << name << ", оценки:";
        if (marks.empty()) {
            std::cout << " нет оценок";
        } else {
            for (double mark : marks) {
                std::cout << " " << mark;
            }
        }
        std::cout << ", средний балл: " << average() << std::endl;
    }

    std::string getName() const {
        return name;
    }

    const std::vector<double>& getMarks() const {
        return marks;
    }
};
