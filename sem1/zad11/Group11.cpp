#include "Group11.hpp"
#include <iostream>
#include <algorithm>

Group::Group(const std::string& name) : groupName(name), students() {}

Group::Group(const std::string& name, const std::vector<Student*>& students) : groupName(name), students(students) {}

void Group::addStudent(Student* student) {
    if (student) {
        students.push_back(student);
    }
}

bool Group::removeStudent(const std::string& studentName) {
    for (auto it = students.begin(); it != students.end(); ++it) {
        if ((*it)->getName() == studentName) {
            students.erase(it);
            return true;
        }
    }
    return false;
}

Student* Group::findStudent(const std::string& studentName) const {
    for (Student* student : students) {
        if (student->getName() == studentName) {
            return student;
        }
    }
    return nullptr;
}

void Group::printAllStudents() const {
    std::cout << "\n=== Группа: " << groupName << " ===" << std::endl;
    if (students.empty()) {
        std::cout << "В группе нет студентов" << std::endl;
        return;
    }
    
    for (size_t i = 0; i < students.size(); ++i) {
        std::cout << "\n--- Студент " << i + 1 << " ---" << std::endl;
        students[i]->print();
    }
}

double Group::getGroupAverage() const {
    if (students.empty()) 
        return 0.0;
    
    double totalSum = 0;
    int totalMarks = 0;
    
    for (const Student* student : students) {
        totalSum += student->getAverage() * student->getRecordBook().getMarksCount();
        totalMarks += student->getRecordBook().getMarksCount();
    }
    
    return totalMarks > 0 ? totalSum / totalMarks : 0.0;
}

void Group::sortStudentsByAverage() {
    std::sort(students.begin(), students.end(),
        [](const Student* a, const Student* b) {
            return a->getAverage() > b->getAverage();
        }
    );
}

void Group::filterStudents(double threshold) {
    students.erase(
        std::remove_if(students.begin(), students.end(),
            [threshold](const Student* student) {
                return student->getAverage() < threshold;
            }
        ),
        students.end()
    );
}

int Group::countExcellentStudents() const {
    int count = 0;
    for (const Student* student : students) {
        if (student->getAverage() >= 4.5) {
            count++;
        }
    }
    return count;
}

int Group::countPoorStudents() const {
    int count = 0;
    for (const Student* student : students) {
        if (student->getAverage() < 3.0) {
            count++;
        }
    }
    return count;
}

std::string Group::getName() const {
    return groupName;
}

const std::vector<Student*>& Group::getStudents() const {
    return students;
}

int Group::getSize() const {
    return students.size();
}