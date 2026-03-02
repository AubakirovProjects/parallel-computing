#pragma once
#include <string>
#include <vector>
#include "Student10.hpp" 

class Group {
private:
    std::string groupName;
    std::vector<Student*> students;

public:
    Group(const std::string& groupName);
    Group(const std::string& groupName, const std::vector<Student*>& students);
    
    void addStudent(Student* student);
    void removeStudent(const std::string& bookNumber);
    double groupMean() const;
    
    inline bool isEmpty() const {
        return students.empty();
    }
    
    inline std::string getName() const {
        return groupName;
    }
};