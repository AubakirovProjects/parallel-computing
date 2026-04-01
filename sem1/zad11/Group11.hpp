#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "Student11.hpp"

class Group {
private:
    std::string groupName;
    std::vector<Student*> students;

public:
    explicit Group(const std::string& name);
    Group(const std::string& name, const std::vector<Student*>& students);
    
    void addStudent(Student* student);
    bool removeStudent(const std::string& studentName);
    Student* findStudent(const std::string& studentName) const;
    
    void printAllStudents() const;
    double getGroupAverage() const;
    void sortStudentsByAverage();
    void filterStudents(double threshold);
    
    int countExcellentStudents() const;
    int countPoorStudents() const;
    
    std::string getName() const;
    const std::vector<Student*>& getStudents() const;
    int getSize() const;
};