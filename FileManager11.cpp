#include "FileManager11.hpp"
#include <fstream>
#include <iostream>
#include <cstring>

bool FileManager::saveGroup(const Group& group, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла для записи!" << std::endl;
        return false;
    }
    
    FileHeader header;
    header.signature[0] = 'G';
    header.signature[1] = 'R';
    header.signature[2] = 'P';
    header.signature[3] = 0;
    header.version = 1;
    header.studentsCount = group.getSize();
    
    file.write(reinterpret_cast<char*>(&header), sizeof(header));
    
    size_t nameLength = group.getName().size();
    file.write(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    file.write(group.getName().c_str(), nameLength);
    
    for (const Student* student : group.getStudents()) {
        size_t studentNameLength = student->getName().size();
        file.write(reinterpret_cast<char*>(&studentNameLength), sizeof(studentNameLength));
        file.write(student->getName().c_str(), studentNameLength);
        
        std::string bookNumber = student->getBookNumber();
        size_t bookLength = bookNumber.size();
        file.write(reinterpret_cast<char*>(&bookLength), sizeof(bookLength));
        file.write(bookNumber.c_str(), bookLength);
        
        int marksCount = student->getRecordBook().getMarksCount();
        file.write(reinterpret_cast<char*>(&marksCount), sizeof(marksCount));
        
        for (double mark : student->getRecordBook().getMarks()) {
            file.write(reinterpret_cast<char*>(&mark), sizeof(mark));
        }
    }
    
    file.close();
    std::cout << "Группа сохранена в файл " << filename << std::endl;
    return true;
}

bool FileManager::loadGroup(Group& group, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла для чтения!" << std::endl;
        return false;
    }
    
    FileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    if (header.signature[0] != 'G' || header.signature[1] != 'R' || header.signature[2] != 'P') {
        std::cout << "Неверная сигнатура файла!" << std::endl;
        file.close();
        return false;
    }
    
    size_t nameLength;
    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    char* groupNameBuffer = new char[nameLength + 1];
    file.read(groupNameBuffer, nameLength);
    groupNameBuffer[nameLength] = '\0';
    std::string groupName(groupNameBuffer);
    delete[] groupNameBuffer;
    
    Group newGroup(groupName);
    std::vector<Student*> loadedStudents;
    
    for (int i = 0; i < header.studentsCount; ++i) {
        size_t studentNameLength;
        file.read(reinterpret_cast<char*>(&studentNameLength), sizeof(studentNameLength));
        char* studentNameBuffer = new char[studentNameLength + 1];
        file.read(studentNameBuffer, studentNameLength);
        studentNameBuffer[studentNameLength] = '\0';
        std::string studentName(studentNameBuffer);
        delete[] studentNameBuffer;
        
        size_t bookLength;
        file.read(reinterpret_cast<char*>(&bookLength), sizeof(bookLength));
        char* bookBuffer = new char[bookLength + 1];
        file.read(bookBuffer, bookLength);
        bookBuffer[bookLength] = '\0';
        std::string bookNumber(bookBuffer);
        delete[] bookBuffer;
        
        int marksCount;
        file.read(reinterpret_cast<char*>(&marksCount), sizeof(marksCount));
        
        Student* student = new Student(studentName, bookNumber);
        for (int j = 0; j < marksCount; ++j) {
            double mark;
            file.read(reinterpret_cast<char*>(&mark), sizeof(mark));
            student->addMark(mark);
        }
        
        newGroup.addStudent(student);
        loadedStudents.push_back(student);
    }
    
    file.close();
    group = newGroup;
    std::cout << "Группа загружена из файла " << filename << std::endl;
    return true;
}