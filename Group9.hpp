#include <string>
#include <vector>
#include <algorithm>
#include <iostream> 
#include "Student9.hpp"

class Group {
private:
    std::string groupName;
    std::vector<Student*> students;
public:
    Group(const std::string& groupName, const std::vector<Student*>& students) : groupName(groupName), students(students) {}

    void addStudent(Student* student) {
        students.push_back(student);
    }

    void removeStudent(const std::string& bookNumber) {
        int initialSize = students.size();
        students.erase(std::remove_if(students.begin(), students.end(), [bookNumber](const Student* student){return student->getBookNumber() == bookNumber;}), students.end());
        int eventualSize = students.size();
    }
    
    double groupMean() const {
        int size = students.size();
        if (size == 0) 
            return 0;

        double sum = 0;
        for (const Student* student : students) 
            sum += student->mean();
            return sum / size;
    }   
};