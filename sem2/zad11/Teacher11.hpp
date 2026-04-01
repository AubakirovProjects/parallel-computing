#pragma once
#include <string>
#include "Person11.hpp"

class Teacher : public Person {
private:
    std::string subject;

public:
    Teacher(const std::string& name, const std::string& subject);
    
    void print() const override;
    std::string getSubject() const;
};