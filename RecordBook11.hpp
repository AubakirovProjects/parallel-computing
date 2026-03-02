#pragma once
#include <string>
#include <vector>

class RecordBook {
private:
    std::string bookNumber;
    std::vector<double> marks;

public:
    explicit RecordBook(const std::string& number);
    RecordBook(const std::string& number, const std::vector<double>& marks);
    
    void addMark(double mark);
    double getAverage() const;
    std::string getBookNumber() const;
    const std::vector<double>& getMarks() const;
    void printMarks() const;
    int getMarksCount() const;
};