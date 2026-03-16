#include <iostream>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>

template<typename Key, typename Value>
class Cache {
private:
    std::map<Key, Value> data;
    std::mutex mtx;
    std::condition_variable cv;
    static std::mutex cout_mtx;

public:
    void set(const Key& key, const Value& value) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            data[key] = value;
            
            std::lock_guard<std::mutex> cout_lock(cout_mtx);
            std::cout << "Поток " << std::this_thread::get_id() << " установил " << key << " = " << value << std::endl;
        }
        cv.notify_all();
        std::this_thread::yield();
    }

    Value get(const Key& key) {
        std::unique_lock<std::mutex> lock(mtx);
        
        cv.wait(lock, [this, &key]() { 
            return data.find(key) != data.end(); 
        });
        
        Value val = data[key];
        
        {
            std::lock_guard<std::mutex> cout_lock(cout_mtx);
            std::cout << "Поток " << std::this_thread::get_id() << " прочитал " << key << " = " << val << std::endl;
        }
        
        std::this_thread::yield();
        return val;
    }

    void print_all() {
        std::lock_guard<std::mutex> lock(mtx);
        std::lock_guard<std::mutex> cout_lock(cout_mtx);
        
        std::cout << " КЭШ \n";
        for (const auto& [k, v] : data) {
            std::cout << k << " : " << v << std::endl;
        }
    }
};

template<typename Key, typename Value>
std::mutex Cache<Key, Value>::cout_mtx;

int main() {
    Cache<std::string, int> cache;
    std::atomic<int> active_threads{3};

    std::cout << "Запуск с detach()...\n";

    std::thread([&cache, &active_threads]() {
        cache.set("a", 1);
        cache.set("b", 2);
        active_threads--;
    }).detach();

    std::thread([&cache, &active_threads]() {
        cache.set("c", 3);
        cache.set("d", 4);
        active_threads--;
    }).detach();

    std::thread([&cache, &active_threads]() {
        cache.get("a");  
        cache.get("c");  
        active_threads--;
    }).detach();
 
    while (active_threads > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    cache.print_all();

    return 0;
}