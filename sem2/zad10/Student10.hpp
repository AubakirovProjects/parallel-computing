#pragma once
#include <string>
#include "RecordBook10.hpp"  

class Student {
private:
    std::string name;
    RecordBook recordBook;

public:
    Student(const std::string& name, const std::string& bookNumber);
    Student(const std::string& name, const RecordBook& book);
    
    void addMark(double mark);
    double mean() const;
    std::string getBookNumber() const;
    std::string getName() const;
    
    inline bool hasName() const {
        return !name.empty();
    }
};