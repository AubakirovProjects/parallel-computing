#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <sstream>  

template<typename T>
class Logger {
private:
    std::ofstream log_file;   
    std::mutex mtx;           
    std::string filename;      
    
    inline std::string to_string(const T& message) {
        std::stringstream ss;
        ss << message;
        return ss.str();
    }
    
public:
    Logger(const std::string& fname) : filename(fname) {
        log_file.open(filename, std::ios::app);  
        if (!log_file.is_open()) {
            std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        }
    }
    
    ~Logger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
    
    void log(const T& message) {
        auto thread_id = std::this_thread::get_id();
        
        std::stringstream ss_id;
        ss_id << thread_id;
        std::string id_str = ss_id.str();
        
        std::string msg_str = to_string(message);
        
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);
        
        {
            std::lock_guard<std::mutex> lock(mtx);
            
            std::cout << std::ctime(&now_time) << "Поток " << id_str << ": " << msg_str << std::endl;
            
            if (log_file.is_open()) {
                log_file << std::ctime(&now_time)  << "Поток " << id_str << ": " << msg_str << std::endl;
                log_file.flush(); 
            }
        } 
    }
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

void test_logger(Logger<std::string>& logger, int thread_num, int count) {
    for (int i = 0; i < count; i++) {
        std::string message = "Привет от потока " + std::to_string(thread_num) +   ", сообщение #" + std::to_string(i);
        logger.log(message);
    }
}

int main() {
    Logger<std::string> logger("log.txt");
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 5; i++) {
        threads.emplace_back([&logger, i]() {
            test_logger(logger, i, 3);  
        });
    }
    
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "\nВСЕ ПОТОКИ ЗАВЕРШЕНЫ";
    
    return 0;
}