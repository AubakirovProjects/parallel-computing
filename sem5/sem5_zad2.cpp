#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <semaphore>
#include <atomic>
#include <vector>

class ParkingLot {
private:
    int capacity;
    std::counting_semaphore<> semaphore;
    std::mutex mtx;
    std::atomic<int> occupied;

public:
    ParkingLot(int cap) : capacity(cap), semaphore(cap), occupied(0) {}

    void park(bool isVIP, int timeout_ms) {
        std::this_thread::yield();
        
        auto start = std::chrono::steady_clock::now();
        
        if (!semaphore.try_acquire_until(start + std::chrono::milliseconds(timeout_ms))) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "Поток " << std::this_thread::get_id()  << " (" << (isVIP ? "VIP" : "Обычный") << ") "
                          << "не дождался места, таймаут\n";
            }
            return;
        }
        
        occupied++;
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Поток " << std::this_thread::get_id()  << " (" << (isVIP ? "VIP" : "Обычный") << ") "
                      << "припарковался. Занято: " << occupied.load()  << ", свободно: " << (capacity - occupied.load()) << "\n";
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        leave();
    }
    
    void leave() {
        occupied--;
        semaphore.release();
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Поток " << std::this_thread::get_id()  << " освободил место. Занято: " << occupied.load() 
                      << ", свободно: " << (capacity - occupied.load()) << "\n";
        }
    }
    
    void set_capacity(int new_capacity) {
        std::lock_guard<std::mutex> lock(mtx);
        int diff = new_capacity - capacity;
        if (diff > 0) {
            for (int i = 0; i < diff; i++) {
                semaphore.release();
            }
        }
        capacity = new_capacity;
        std::cout << "Вместимость парковки изменена на: " << capacity << "\n";
    }
};

void car_thread(ParkingLot& lot, bool isVIP, int timeout_ms, int id) {
    lot.park(isVIP, timeout_ms);
}

int main() {
    ParkingLot lot(2);
    
    std::vector<std::thread> threads;
    
    threads.emplace_back(car_thread, std::ref(lot), true, 500, 1);
    threads.emplace_back(car_thread, std::ref(lot), false, 500, 2);
    threads.emplace_back(car_thread, std::ref(lot), false, 300, 3);
    threads.emplace_back(car_thread, std::ref(lot), true, 500, 4);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    lot.set_capacity(3);
    
    for (auto& t : threads) {
        t.detach();
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    return 0;
}