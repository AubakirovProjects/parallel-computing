#include <iostream>
#include <vector>
#include <string>

class VirtualThread {
public:
    VirtualThread(const std::vector<std::string>& tasks) : tasks(tasks), started(tasks.size(), false), finished(tasks.size(), false) {}

    bool hasWork() const {
        for (std::size_t i = 0; i < tasks.size(); ++i) {
            if (!finished[i]) return true;
        }
        return false;
    }

    void runNextWork(int threadId) {
        std::size_t idx = 0;
        while (idx < tasks.size() && finished[idx]) {
            ++idx;
        }
        if (idx == tasks.size()) return;

        if (!started[idx]) {
            std::cout << "Виртуальный поток " << threadId  << " начал " << tasks[idx] << '\n';
            started[idx] = true;
        } else {
            std::cout << "Виртуальный поток " << threadId  << " закончил " << tasks[idx] << '\n';
            finished[idx] = true;
        }
    }

private:
    std::vector<std::string> tasks;
    std::vector<bool> started;
    std::vector<bool> finished;
};

class HyperThreadingSimulator {
public:
    HyperThreadingSimulator() : vt1({"Задача A", "Задача C"}),  vt2({"Задача B", "Задача D"}) {}

    void execute() {
        while (vt1.hasWork() || vt2.hasWork()) {
            if (vt1.hasWork()) {
                vt1.runNextWork(1);
            }
            if (vt2.hasWork()) {
                vt2.runNextWork(2);
            }
        }
    }

private:
    VirtualThread vt1;
    VirtualThread vt2;
};

int main() {
    HyperThreadingSimulator simulator;
    simulator.execute();
    return 0;
}
