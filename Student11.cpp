#include "Student11.hpp"
#include <iostream>

Student::Student(const std::string& name, const std::string& bookNumber) 
    : Person(name), recordBook(bookNumber) {}

Student::Student(const std::string& name, const RecordBook& book) 
    : Person(name), recordBook(book) {}

void Student::addMark(double mark) {
    recordBook.addMark(mark);
}

double Student::getAverage() const {
    return recordBook.getAverage();
}

std::string Student::getBookNumber() const {
    return recordBook.getBookNumber();
}

const RecordBook& Student::getRecordBook() const {
    return recordBook;
}

void Student::print() const {
    std::cout << "Студент: " << name << std::endl;
    std::cout << "  Зачётка №" << recordBook.getBookNumber() << std::endl;
    std::cout << "  Оценки: ";
    recordBook.printMarks();
    std::cout << std::endl;
    std::cout << "  Средний балл: " << recordBook.getAverage() << std::endl;
}