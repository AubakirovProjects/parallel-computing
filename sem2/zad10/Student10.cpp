#include "Student10.hpp"

Student::Student(const std::string& name, const std::string& bookNumber) : name(name), recordBook(bookNumber) {}

Student::Student(const std::string& name, const RecordBook& book) : name(name), recordBook(book) {}

void Student::addMark(double mark) {
    recordBook.addMark(mark);
}

double Student::mean() const {
    return recordBook.mean();
}

std::string Student::getBookNumber() const {
    return recordBook.getBookNumber();
}

std::string Student::getName() const {
    return name;
}