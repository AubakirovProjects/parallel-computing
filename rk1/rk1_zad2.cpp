#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <semaphore>
#include <atomic>
#include <mutex>

const int Items = 10;
const int Machines = 3;

std::counting_semaphore<Machines> machineSemaphore(Machines);
std::counting_semaphore<Itmes> blankSemaphore(Itmes);

std::atomic<int> processedCount(0);
std::mutex printMutex;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> workTimeDist(1, 2);

void processBlank(int workerId) {
    while (true) {
        if (!blankSemaphore.try_acquire()) {
            if (processedCount.load() >= Items) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        machineSemaphore.acquire();
        
        int workTime = workTimeDist(gen);
        
        {
            std::lock_guard<std::mutex> lock(printMutex);
            std::cout << "Рабочий загрузил заготовку " << workerId  << "в машину. Время обработки: "  << workTime << " сек." << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(workTime));
        
        processedCount++;
        
        {
            std::lock_guard<std::mutex> lock(printMutex);
            std::cout << "Рабочий закончил обработку заготовки " << workerId   << ". Обработано заготовок: "   << processedCount.load() << std::endl;
        }
        
        machineSemaphore.release();
        
    }
    
    {
        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << "Рабочий " << workerId << " завершил работу (заготовки закончились)" << std::endl;
    }
}

int main() {
    
    for (int i = 0; i < Items; ++i) {
        blankSemaphore.release();
    }
    
    const int Machines = 5;
    std::vector<std::thread> workers;
    
    for (int i = 1; i <= Machines; ++i) {
        workers.emplace_back(processBlank, i);
    }
    
    for (auto& worker : workers) {
        worker.join();
    }
    
    return 0;
}