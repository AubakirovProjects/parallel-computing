#include <iostream>
#include <vector>
#include "Person7.hpp"
#include "Student7.hpp"

class Teacher : public Person {
private:
    std::vector<Student*> students;

public:
    Teacher(const std::string& name) : Person(name), students() {}
    Teacher(const std::string& name, const std::vector<Student*>& students) : Person(name), students(students) {}

    void print() const override {
        std::cout << "Учитель " << name << ", обучает студентов:";
        for (const Student* student : students) {
            std::cout << " " << student->getName();
        }
        std::cout << std::endl;
    }

    void addStudent(Student* student) {
        students.push_back(student);
        std::cout << "Студент " << student->getName() << " добавлен к учителю " << name << std::endl;
    }

    const std::vector<Student*>& getStudents() const {
        return students;
    }
};