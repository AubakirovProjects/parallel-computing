#include "RecordBook10.hpp"

RecordBook::RecordBook(const std::string& number) : bookNumber(number), marks() {}

RecordBook::RecordBook(const std::string& number, const std::vector<double>& marks) : bookNumber(number), marks(marks) {}

void RecordBook::addMark(double mark) {
    if (mark >= 0 && mark <= 5) {
        marks.push_back(mark);
    }
}

double RecordBook::mean() const {
    if (marks.empty()) 
        return 0.0;
    
    double sum = 0;
    for (double mark : marks) {
        sum += mark;
    }
    return sum / marks.size();
}

std::string RecordBook::getBookNumber() const {
    return bookNumber;
}