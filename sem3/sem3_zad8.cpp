#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>

struct Task {
    int value;
    int priority;
    int duration;
    int steps;
    int currentStep;

    Task(int v, int p, int d, int s) : value(v), priority(p), duration(d), steps(s), currentStep(0) {}
};

class VirtualThread {
public:
    void addTask(const Task& task) {
        tasks.push_back(task);
    }

    bool hasWork() const {
        for (const auto& t : tasks) {
            if (t.currentStep < t.steps) 
                return true;
        }
        return false;
    }

    void runStep(int threadId) {
        if (!hasWork()) return;

        int bestIndex = -1;
        int bestPriority = -1;
        for (std::size_t i = 0; i < tasks.size(); ++i) {
            if (tasks[i].currentStep < tasks[i].steps && tasks[i].priority > bestPriority) {
                bestPriority = tasks[i].priority;
                bestIndex = static_cast<int>(i);
            }
        }

        if (bestIndex == -1) 
            return;

        Task& task = tasks[bestIndex];
        ++task.currentStep;

        int stepDuration = task.duration / task.steps;
        if (stepDuration <= 0) 
            stepDuration = 1;

        std::cout << "Виртуальный поток " << threadId << " выполняет шаг " << task.currentStep << "/" << task.steps << " задачи " << task.value << " с приоритетом " << task.priority << '\n';

        std::this_thread::sleep_for(std::chrono::milliseconds(stepDuration));

        if (task.currentStep == task.steps) {
            int result = static_cast<int>(task.value * task.value);
            std::cout << "Виртуальный поток " << threadId << " завершил задачу " << task.value << ": результат = " << result << '\n';
        }
    }

private:
    std::vector<Task> tasks;
};

class HyperThreadingSimulator {
public:
    HyperThreadingSimulator() {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<int> valueDist(1, 50);
        std::uniform_int_distribution<int> durationDist(200, 1000);
        std::uniform_int_distribution<int> priorityDist(1, 10);
        std::uniform_int_distribution<int> stepsDist(2, 5);

        const int tasksPerThread = 4;

        for (int i = 0; i < tasksPerThread; ++i) {
            vt1.addTask(Task(
                valueDist(gen),
                priorityDist(gen),
                durationDist(gen),
                stepsDist(gen)
            ));
            vt2.addTask(Task(
                valueDist(gen),
                priorityDist(gen),
                durationDist(gen),
                stepsDist(gen)
            ));
        }
    }

    void execute() {
        while (vt1.hasWork() || vt2.hasWork()) {
            if (vt1.hasWork()) {
                vt1.runStep(1);
            }
            if (vt2.hasWork()) {
                vt2.runStep(2);
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
