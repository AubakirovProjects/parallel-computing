#include <iostream>
#include <vector>

class VirtualThread {
public:
    VirtualThread(const std::vector<int>& numbers) : tasks(numbers), currentIndex(0) {}

    bool hasWork() const {
        return currentIndex < tasks.size();
    }

    void run(int threadId) {
        if (!hasWork()) {
            return;
        }
        int n = tasks[currentIndex++];
        unsigned long long result = 1;
        for (int i = 2; i <= n; ++i) {
            result *= static_cast<unsigned long long>(i);
        }
        std::cout << "Виртуальный поток " << threadId  << " вычисляет " << n << "! = " << result << '\n';
    }

private:
    std::vector<int> tasks;
    std::size_t currentIndex;
};

class HyperThreadingSimulator {
public:
    HyperThreadingSimulator() : vt1({5, 10,}), vt2({7, 12}) {}

    void execute() {
        while (vt1.hasWork() || vt2.hasWork()) {
            if (vt1.hasWork()) {
                vt1.run(1);
            }
            if (vt2.hasWork()) {
                vt2.run(2);
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
