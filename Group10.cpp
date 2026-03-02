#include <algorithm>
#include <iostream> 
#include "Group10.hpp"

Group::Group(const std::string& groupName) : groupName(groupName), students() {}

Group::Group(const std::string& groupName, const std::vector<Student*>& students) : groupName(groupName), students(students) {}

void Group::addStudent(Student* student) {
    students.push_back(student);
}

void Group::removeStudent(const std::string& bookNumber) {
    int initialSize = students.size();
    students.erase(
        std::remove_if(students.begin(), students.end(), 
            [bookNumber](const Student* student) {
                return student->getBookNumber() == bookNumber;
            }
        ), 
        students.end()
    );
}

double Group::groupMean() const {
    int size = students.size();
    if (size == 0) 
        return 0;

    double sum = 0;
    for (const Student* student : students) {
        sum += student->mean();
    }
    return sum / size;
}