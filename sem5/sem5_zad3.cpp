#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <semaphore>
#include <random>

template<typename T>
class SemaphoreBuffer {
private:
    std::vector<std::vector<T>> buffers;
    std::vector<std::counting_semaphore<>> empty;
    std::vector<std::counting_semaphore<>> full;
    std::vector<std::mutex> mtx;
    std::vector<size_t> buffer_positions;

public:
    SemaphoreBuffer(int num_buffers, size_t buffer_size) : buffers(num_buffers, std::vector<T>(buffer_size))
        , empty(num_buffers, std::counting_semaphore<>(buffer_size))
        , full(num_buffers, std::counting_semaphore<>(0))
        , mtx(num_buffers)
        , buffer_positions(num_buffers, 0) {}

    void produce(T value, int buffer_index, int timeout_ms) {
        std::this_thread::yield();
        
        auto start = std::chrono::steady_clock::now();
        
        if (!empty[buffer_index].try_acquire_until(start + std::chrono::milliseconds(timeout_ms))) {
            {
                std::lock_guard<std::mutex> lock(mtx[buffer_index]);
                std::cout << "Поток " << std::this_thread::get_id()  << " буфер " << buffer_index  << " PRODUCE таймаут\n";
            }
            return;
        }
        
        {
            std::lock_guard<std::mutex> lock(mtx[buffer_index]);
            buffers[buffer_index][buffer_positions[buffer_index]] = value;
            buffer_positions[buffer_index]++;
            
            std::cout << "Поток " << std::this_thread::get_id()  << " буфер " << buffer_index 
                      << " PRODUCE значение " << value  << " (таймаут " << timeout_ms << "ms)\n";
        }
        
        full[buffer_index].release();
    }
    
    T consume(int buffer_index, int timeout_ms) {
        std::this_thread::yield();
        
        auto start = std::chrono::steady_clock::now();
        
        if (!full[buffer_index].try_acquire_until(start + std::chrono::milliseconds(timeout_ms))) {
            {
                std::lock_guard<std::mutex> lock(mtx[buffer_index]);
                std::cout << "Поток " << std::this_thread::get_id()  << " буфер " << buffer_index 
                          << " CONSUME таймаут\n";
            }
            throw std::runtime_error("consume timeout");
        }
        
        T value;
        {
            std::lock_guard<std::mutex> lock(mtx[buffer_index]);
            buffer_positions[buffer_index]--;
            value = buffers[buffer_index][buffer_positions[buffer_index]];
            
            std::cout << "Поток " << std::this_thread::get_id()  << " буфер " << buffer_index 
                      << " CONSUME значение " << value  << " (таймаут " << timeout_ms << "ms)\n";
        }
        
        empty[buffer_index].release();
        return value;
    }
    
    int get_random_buffer(int num_buffers) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, num_buffers - 1);
        return dis(gen);
    }
};

void producer(SemaphoreBuffer<int>& buffer, int id, int num_buffers) {
    for (int i = 0; i < 3; i++) {
        int buf_idx = buffer.get_random_buffer(num_buffers);
        buffer.produce(id * 100 + i, buf_idx, 200);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumer(SemaphoreBuffer<int>& buffer, int id, int num_buffers) {
    for (int i = 0; i < 3; i++) {
        int buf_idx = buffer.get_random_buffer(num_buffers);
        try {
            int val = buffer.consume(buf_idx, 200);
        } catch (const std::runtime_error& e) {
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

int main() {
    SemaphoreBuffer<int> buffer(3, 5);
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 2; i++) {
        threads.emplace_back(producer, std::ref(buffer), i, 3);
    }
    
    for (int i = 0; i < 2; i++) {
        threads.emplace_back(consumer, std::ref(buffer), i, 3);
    }
    
    for (auto& t : threads) {
        t.detach();
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    return 0;
}