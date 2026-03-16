#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

template<typename T>
class ParallelSum {
private:
    std::vector<T> data;
    size_t n_threads;
    T total_sum;
    std::mutex mtx;
    std::condition_variable cv;
    int completed_threads;
    std::mutex cv_mtx;
    static std::mutex cout_mtx;

    inline T sum_segment(size_t start, size_t end) {
        T segment_sum = 0;
        for (size_t i = start; i < end; ++i) {
            segment_sum += data[i];
            if (i % 1000 == 0) {
                std::this_thread::yield();
            }
        }
        return segment_sum;
    }

public:
    ParallelSum(const std::vector<T>& vec, size_t threads) : data(vec), n_threads(threads), total_sum(0), completed_threads(0) {}

    T compute_sum() {
        size_t chunk_size = data.size() / n_threads;
        
        for (size_t i = 0; i < n_threads; ++i) {
            size_t start = i * chunk_size;
            size_t end;
            if (i == n_threads - 1) {      
              end = data.size();           
            } else {                         
              end = start + chunk_size;    
            }
            
            std::thread([this, start, end]() {
                auto thread_id = std::this_thread::get_id();
                T segment_sum = this->sum_segment(start, end);
                
                {
                    std::lock_guard<std::mutex> lock(cout_mtx);
                    std::cout << "Поток " << thread_id << " обработал сегмент с суммой: " << segment_sum << std::endl;
                }
                
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    total_sum += segment_sum;
                }
                
                {
                    std::lock_guard<std::mutex> lock(cv_mtx);
                    completed_threads++;
                }
                cv.notify_one();
            }).detach(); 
        }
        
        std::unique_lock<std::mutex> lock(cv_mtx);
        cv.wait(lock, [this]() { return completed_threads == n_threads; });
        
        std::cout << "Итоговая сумма: " << total_sum << std::endl;
        return total_sum;
    }
};

template<typename T>
std::mutex ParallelSum<T>::cout_mtx;

int main() {
    std::vector<int> int_data(10000);
    for (int i = 0; i < 10000; ++i) {
        int_data[i] = i + 1;
    }
    
    ParallelSum<int> int_sum(int_data, 4);
    std::cout << "Суммирование int (1..10000):" << std::endl;
    int_sum.compute_sum();

    return 0;
}