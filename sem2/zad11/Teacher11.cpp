#include "Teacher11.hpp"
#include <iostream>

Teacher::Teacher(const std::string& name, const std::string& subject) : Person(name), subject(subject) {}

void Teacher::print() const {
    std::cout << "Преподаватель: " << name << std::endl;
    std::cout << "  Предмет: " << subject << std::endl;
}

std::string Teacher::getSubject() const {
    return subject;
}