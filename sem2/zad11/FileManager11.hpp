#pragma once
#include <string>
#include "Group11.hpp"

class FileManager {
public:
    static bool saveGroup(const Group& group, const std::string& filename);
    static bool loadGroup(Group& group, const std::string& filename);
    
private:
    struct FileHeader {
        char signature[4];
        int version;
        int studentsCount;
    };
};