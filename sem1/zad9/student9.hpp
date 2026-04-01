#include <iostream>
#include <string>
#include "RecordBook9.hpp"

class Student {
private:
    std::string name;  
    RecordBook recordBook;  

public:
    Student(const std::string& name, const std::string& bookNumber) : name(name), recordBook(bookNumber) {}
    Student(const std::string& name, const RecordBook& recordBook) : name(name), recordBook(recordBook) {}


    void addMark(double mark) {
        recordBook.addMark(mark);
    }

    double average() const {
        return recordBook.average();
    }

    std::string getBookNumber() const {
        return recordBook.getBookNumber();
    }

    std::string getName() const {
        return name;
    }

    void printInfo() const {
        std::cout << "Студент: " << name << std::endl;
        recordBook.printInfo();  
    }
};
