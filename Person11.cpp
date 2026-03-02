#include "Person11.hpp"
#include <iostream>

Person::Person(const std::string& name) : name(name) {}

Person::~Person() {}

std::string Person::getName() const {
    return name;
}