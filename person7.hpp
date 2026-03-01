#include <string>

class Person {
protected:
    std::string name;

public:
    explicit Person(const std::string& name) : name(name) {}
    
    virtual ~Person() {}

    virtual void print() const = 0;

    std::string getName() const {
        return name;
    }
};