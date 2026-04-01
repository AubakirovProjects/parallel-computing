#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <semaphore>
#include <queue>
#include <string>
#include <condition_variable>

struct PrintJob {
    std::string doc;
    int priority;
    int id;
    
    bool operator<(const PrintJob& other) const {
        return priority < other.priority;
    }
};

class PrinterQueue {
private:
    int n_printers;
    std::counting_semaphore<> semaphore;
    std::mutex mtx;
    std::priority_queue<PrintJob> queue;
    int next_id;

public:
    PrinterQueue(int printers) : n_printers(printers), semaphore(printers), next_id(0) {}

    void printJob(std::string doc, int priority, int timeout_ms) {
        std::this_thread::yield();
        
        int job_id;
        {
            std::lock_guard<std::mutex> lock(mtx);
            job_id = next_id++;
            queue.push({doc, priority, job_id});
            std::cout << "Поток " << std::this_thread::get_id()  << " задание " << job_id 
                      << " приоритет " << priority << " добавлено в очередь\n";
        }
        
        auto start = std::chrono::steady_clock::now();
        
        if (!semaphore.try_acquire_until(start + std::chrono::milliseconds(timeout_ms))) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "Поток " << std::this_thread::get_id()  << " задание " << job_id 
                          << " приоритет " << priority  << " прервано (таймаут)\n";
            }
            return;
        }
        
        PrintJob current_job;
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!queue.empty() && queue.top().id == job_id) {
                current_job = queue.top();
                queue.pop();
            } else {
                semaphore.release();
                return;
            }
        }
        
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Поток " << std::this_thread::get_id()  << " задание " << current_job.id 
                      << " приоритет " << current_job.priority  << " печатает: " << current_job.doc << "\n";
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Поток " << std::this_thread::get_id()  << " задание " << current_job.id  << " завершено\n";
        }
        
        semaphore.release();
    }
};

void worker(PrinterQueue& pq, std::string doc, int priority, int timeout_ms) {
    pq.printJob(doc, priority, timeout_ms);
}

int main() {
    PrinterQueue pq(2);
    
    std::vector<std::thread> threads;
    
    threads.emplace_back(worker, std::ref(pq), "Документ_A", 1, 500);
    threads.emplace_back(worker, std::ref(pq), "Документ_B", 3, 500);
    threads.emplace_back(worker, std::ref(pq), "Документ_C", 2, 500);
    threads.emplace_back(worker, std::ref(pq), "Документ_D", 3, 300);
    threads.emplace_back(worker, std::ref(pq), "Документ_E", 1, 500);
    
    for (auto& t : threads) {
        t.detach();
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    return 0;
}