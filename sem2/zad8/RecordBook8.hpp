#include <iostream>
#include <vector>
#include <string>

class RecordBook {
private:
    std::string bookNumber;       
    std::vector<double> marks;   

public:
    explicit RecordBook(const std::string& number) : bookNumber(number), marks() {}

    RecordBook(const std::string& number, const std::vector<double>& marks) : bookNumber(number), marks(marks) {}

    void addMark(double mark) {
        if (mark >= 0 && mark <= 5) {
            marks.push_back(mark);
            std::cout << "Оценка " << mark << " добавлена в зачётку " << bookNumber << std::endl;
        } else {
            std::cout << "Ошибка! Оценка должна быть от 0 до 5" << std::endl;
        }
    }

    double average() const {
        if (marks.empty()) return 0.0;
        
        double sum = 0;
        for (double mark : marks) {
            sum += mark;
        }
        return sum / marks.size();
    }

    std::string getBookNumber() const {
        return bookNumber;
    }

    const std::vector<double>& getMarks() const {
        return marks;
    }

    void printInfo() const {
        std::cout << "Зачётная книжка №" << bookNumber << ", оценки:";
        if (marks.empty()) {
            std::cout << " нет оценок";
        } else {
            for (double mark : marks) {
                std::cout << " " << mark;
            }
        }
        std::cout << ", средний балл: " << average() << std::endl;
    }
};