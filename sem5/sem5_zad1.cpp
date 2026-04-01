#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <semaphore>
#include <random>

template<typename T>
class ResourcePool {
private:
    std::vector<T> resources;
    std::counting_semaphore<> semaphore;
    std::mutex mtx;
    std::atomic<int> failed_attempts;

public:
    ResourcePool(size_t count, const std::vector<T>& res_list)
        : semaphore(count), failed_attempts(0) {
        resources = res_list;
    }

    T acquire(int priority, int timeout_ms) {
        auto start = std::chrono::steady_clock::now();
        
        if (!semaphore.try_acquire_until(start + std::chrono::milliseconds(timeout_ms))) {
            failed_attempts++;
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "Поток " << std::this_thread::get_id()  << " приоритет " << priority  << " время ожидания получения\n";
            }
            throw std::runtime_error("время ожидания получения");
        }
        
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Поток " << std::this_thread::get_id()  << " приоритет " << priority  << " успешно\n";
        }
        
        std::lock_guard<std::mutex> lock(mtx);
        T res = resources.back();
        resources.pop_back();
        return res;
    }

    void release(T res) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            resources.push_back(res);
            std::cout << "Поток " << std::this_thread::get_id() << " осовободил ресурс\n";
        }
        semaphore.release();
    }

    int get_failed_attempts() const {
        return failed_attempts.load();
    }
};

void worker(ResourcePool<int>& pool, int priority, int timeout_ms, int id) {
    std::this_thread::yield();
    try {
        int res = pool.acquire(priority, timeout_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        pool.release(res);
    } catch (const std::runtime_error& e) {
    }
}

int main() {
    std::vector<int> initial_resources = {1, 2, 3};
    ResourcePool<int> pool(3, initial_resources);
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 5; i++) {
        int priority = (i % 2 == 0) ? 1 : 2;
        threads.emplace_back(worker, std::ref(pool), priority, 200, i);
    }
    
    for (auto& t : threads) {
        t.detach();
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "Неудачные попытки: " << pool.get_failed_attempts() << "\n";
    
    return 0;
}