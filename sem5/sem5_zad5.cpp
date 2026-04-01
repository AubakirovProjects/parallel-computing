#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <semaphore>
#include <queue>
#include <atomic>
#include <condition_variable>

struct Task {
    int id;
    int required_slots;
    int duration_ms;
    int priority;
    
    bool operator<(const Task& other) const {
        return priority < other.priority;
    }
};

class TaskScheduler {
private:
    std::priority_queue<Task> task_queue;
    std::counting_semaphore<> resource_semaphore;
    std::mutex queue_mutex;
    std::atomic<int> completed_tasks;
    std::atomic<long long> total_wait_time;
    std::condition_variable cv;

public:
    TaskScheduler(int total_slots) : resource_semaphore(total_slots), completed_tasks(0), total_wait_time(0) {}

    void submit(Task task) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            task_queue.push(task);
            std::cout << "Поток " << std::this_thread::get_id()  << " добавил задачу " << task.id 
                      << " (ресурсы: " << task.required_slots  << ", приоритет: " << task.priority << ")\n";
        }
        cv.notify_one();
    }
    
    inline void execute_task(Task& task) {
        std::cout << "Поток " << std::this_thread::get_id()  << " выполняет задачу " << task.id 
                  << " (ресурсы: " << task.required_slots << ")\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(task.duration_ms));
        
        std::cout << "Поток " << std::this_thread::get_id()  << " завершил задачу " << task.id << "\n";
    }
    
    void worker() {
        while (true) {
            std::this_thread::yield();
            
            Task task;
            bool has_task = false;
            auto wait_start = std::chrono::steady_clock::now();
            
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                cv.wait_for(lock, std::chrono::milliseconds(100), [this] {
                    return !task_queue.empty();
                });
                
                if (!task_queue.empty()) {
                    task = task_queue.top();
                    task_queue.pop();
                    has_task = true;
                }
            }
            
            if (!has_task) {
                break;
            }
            
            auto wait_end = std::chrono::steady_clock::now();
            long long wait_ms = std::chrono::duration_cast<std::chrono::milliseconds>(wait_end - wait_start).count();
            total_wait_time += wait_ms;
            
            std::cout << "Поток " << std::this_thread::get_id()  << " ожидал задачу " << task.id << " " << wait_ms << "ms\n";
            
            resource_semaphore.acquire();
            
            for (int i = 1; i < task.required_slots; i++) {
                resource_semaphore.acquire();
            }
            
            std::cout << "Поток " << std::this_thread::get_id()  << " захватил " << task.required_slots 
                      << " ресурсов для задачи " << task.id << "\n";
            
            execute_task(task);
            
            for (int i = 0; i < task.required_slots; i++) {
                resource_semaphore.release();
            }
            
            completed_tasks++;
            
            std::cout << "Поток " << std::this_thread::get_id()  << " освободил " << task.required_slots 
                      << " ресурсов. Завершено задач: " << completed_tasks << "\n";
        }
    }
    
    int get_completed_tasks() const {
        return completed_tasks.load();
    }
    
    double get_average_wait_time() const {
        if (completed_tasks == 0) return 0;
        return static_cast<double>(total_wait_time.load()) / completed_tasks.load();
    }
};

int main() {
    TaskScheduler scheduler(4);
    
    std::vector<std::thread> workers;
    
    for (int i = 0; i < 3; i++) {
        workers.emplace_back([&scheduler]() {
            scheduler.worker();
        });
    }
    
    std::vector<Task> tasks = {
        {1, 2, 500, 3},
        {2, 1, 300, 1},
        {3, 3, 400, 4},
        {4, 1, 200, 2},
        {5, 2, 600, 3},
        {6, 1, 250, 1}
    };
    
    for (auto& task : tasks) {
        scheduler.submit(task);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    for (auto& t : workers) {
        t.detach();
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::cout << "\n=== Статистика ===\n";
    std::cout << "Завершено задач: " << scheduler.get_completed_tasks() << "\n";
    std::cout << "Среднее время ожидания: " << scheduler.get_average_wait_time() << " ms\n";
    
    return 0;
}