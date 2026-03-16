#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <queue>  

template<typename T>
class Buffer {
private:
    std::queue<T> buffer;      
    std::mutex mtx;                
    std::condition_variable cv_prod; 
    std::condition_variable cv_cons; 
    
public:
    Buffer(size_t cap) : capacity(cap) {}
    
    void produce(T value) {
        std::unique_lock<std::mutex> lock(mtx);
        
        cv_prod.wait(lock, [this]() { 
            return buffer.size() < capacity; 
        });
        
        buffer.push(value);
        
        std::cout << "Производитель " << std::this_thread::get_id()   << " добавил: " << value << " [размер: " << buffer.size() << "]\n";
        
        std::this_thread::yield();
        
        cv_cons.notify_one();
    }
    
    T consume() {
        std::unique_lock<std::mutex> lock(mtx);
        
        cv_cons.wait(lock, [this]() { 
            return !buffer.empty(); 
        });
        
        // Извлекаем элемент
        T value = buffer.front();
        buffer.pop();
        
        std::cout << "Потребитель " << std::this_thread::get_id()   << " извлек: " << value << " [размер: " << buffer.size() << "]\n";
        
        std::this_thread::yield();
        
        cv_prod.notify_one();
        
        return value;
    }
    
    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        return buffer.size();
    }
    
    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return buffer.empty();
    }
    
    bool full() {
        std::lock_guard<std::mutex> lock(mtx);
        return buffer.size() >= capacity;
    }
};

int main() {
    Buffer<int> buffer(3);

    std::thread producer1([&buffer]() {
        for (int i = 1; i <= 5; i++) {
            buffer.produce(i);
        }
    });
    
    std::thread producer2([&buffer]() {
        for (int i = 10; i <= 50; i += 10) {
            buffer.produce(i);
        }
    });
    
    std::thread consumer1([&buffer]() {
        for (int i = 0; i < 4; i++) {
            buffer.consume();
        }
    });
    
    std::thread consumer2([&buffer]() {
        for (int i = 0; i < 4; i++) {
            buffer.consume();
        }
    });
    
    producer1.join();
    producer2.join();
    consumer1.join();
    consumer2.join();
    
    return 0;
}