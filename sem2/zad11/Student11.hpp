#pragma once
#include <string>
#include "Person11.hpp"
#include "RecordBook11.hpp"

class Student : public Person {
private:
    RecordBook recordBook;

public:
    Student(const std::string& name, const std::string& bookNumber);
    Student(const std::string& name, const RecordBook& book);
    
    void addMark(double mark);
    double getAverage() const;
    std::string getBookNumber() const;
    const RecordBook& getRecordBook() const;
    
    void print() const override;
};