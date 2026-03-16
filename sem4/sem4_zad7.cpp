#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>

template<typename T>
struct QueueItem {
    T value;
    int priority;
    
    QueueItem(T val, int prio) : value(val), priority(prio) {}
    
    bool operator<(const QueueItem& other) const {
        return priority < other.priority;
    }
};

template<typename T>
class PriorityQueue {
private:
    std::priority_queue<QueueItem<T>> queue;
    std::mutex mtx;
    std::condition_variable cv;
    static std::mutex cout_mtx;

public:
    void push(T value, int priority) {
        auto id = std::this_thread::get_id();
        
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.emplace(value, priority);
            
            std::lock_guard<std::mutex> cout_lock(cout_mtx);
            std::cout << "Поток " << id << ": добавил " << value 
                     << " (приор=" << priority << ") [размер=" << queue.size() << "]\n";
        }
        
        std::this_thread::yield();
        cv.notify_one();  
    }
    
    T pop() {
        auto id = std::this_thread::get_id();
        
        std::unique_lock<std::mutex> lock(mtx);
        
        cv.wait(lock, [this]() { return !queue.empty(); });
        
        QueueItem<T> item = queue.top();
        queue.pop();
        
        {
            std::lock_guard<std::mutex> cout_lock(cout_mtx);
            std::cout << "Поток " << id << ": извлек " << item.value 
                     << " (приор=" << item.priority << ") [размер=" << queue.size() << "]\n";
        }
        
        std::this_thread::yield();
        return item.value;
    }
    
    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }
    
    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }
};

template<typename T>
std::mutex PriorityQueue<T>::cout_mtx;

int main() {
    PriorityQueue<int> queue;
    std::atomic<int> producers_done{0}; 
    const int TOTAL_PRODUCERS = 2;
    const int TOTAL_CONSUMERS = 3;
    
    std::cout << "Запуск потоков в режиме detach()...\n";
    
    std::thread([&queue, &producers_done]() {
        for (int i = 0; i < 5; i++) {
            queue.push(i * 10, i % 3 + 1);  
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        producers_done++;
        std::cout << "Производитель 1 завершил\n";
    }).detach();
    
    std::thread([&queue, &producers_done]() {
        for (int i = 0; i < 5; i++) {
            queue.push(i * 100, i % 5 + 1); 
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        producers_done++;
        std::cout << "Производитель 2 завершил\n";
    }).detach();
    
    std::thread([&queue, &producers_done, TOTAL_PRODUCERS]() {
        while (producers_done < TOTAL_PRODUCERS || !queue.empty()) {
            if (!queue.empty()) {
                int val = queue.pop();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        std::cout << "Потребитель 1 завершил\n";
    }).detach();
    
    std::thread([&queue, &producers_done, TOTAL_PRODUCERS]() {
        while (producers_done < TOTAL_PRODUCERS || !queue.empty()) {
            if (!queue.empty()) {
                int val = queue.pop();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        std::cout << "Потребитель 2 завершил\n";
    }).detach();
    
    std::thread([&queue, &producers_done, TOTAL_PRODUCERS]() {
        while (producers_done < TOTAL_PRODUCERS || !queue.empty()) {
            if (!queue.empty()) {
                int val = queue.pop();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        std::cout << "Потребитель 3 завершил\n";
    }).detach();
    
    while (producers_done < TOTAL_PRODUCERS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    return 0;
}