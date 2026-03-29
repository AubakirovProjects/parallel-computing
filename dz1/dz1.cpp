#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <atomic>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cctype>
#include <algorithm>

#include <boost/thread.hpp>

struct FileStats {
    std::string filename;
    int wordCount;
    int charCount;
    int lineCount;
    std::chrono::duration<double> processingTime;
};

std::queue<std::string> taskQueue;
std::mutex queueMutex;
std::condition_variable queueCV;
std::atomic<int> processedFilesCount(0);
std::vector<FileStats> allStats;
std::mutex statsMutex;

void countStats(const std::string& text, int& wordCount, int& charCount, int& lineCount) {
    charCount = text.length();
    
    lineCount = 1;
    for (char c : text) {
        if (c == '\n') {
            lineCount++;
        }
    }
    
    wordCount = 0;
    std::stringstream ss(text);
    std::string word;
    
    while (ss >> word) {
        wordCount++;
    }
}

void processFile(const std::string& filename) {
    auto startTime = std::chrono::steady_clock::now();
    
    std::cout << "[Поток " << boost::this_thread::get_id()  << "] Начинает обработку файла: " << filename << std::endl;
    
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл " << filename << std::endl;
        processedFilesCount++;
        return;
    }
    
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    inFile.close();
    
    int wordCount = 0, charCount = 0, lineCount = 0;
    countStats(content, wordCount, charCount, lineCount);

    std::string outputFilename = "processed_" + filename;
    std::ofstream outFile(outputFilename);
    if (outFile.is_open()) {
        outFile << "Результаты обработки файла: " << filename << std::endl;
        outFile << "Количество символов: " << charCount << std::endl;
        outFile << "Количество слов: " << wordCount << std::endl;
        outFile << "Количество строк: " << lineCount << std::endl;
        outFile << "========================================" << std::endl;
        outFile << "\nСодержимое файла:" << std::endl;
        outFile << "----------------------------------------" << std::endl;
        outFile << content << std::endl;
        outFile.close();
        std::cout << "[Поток " << boost::this_thread::get_id()  << "] Сохранил результат в " << outputFilename << std::endl;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> processingTime = endTime - startTime;
    
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        allStats.push_back({filename, wordCount, charCount, lineCount, processingTime});
    }
    
    processedFilesCount++;
    
    std::cout << "[Поток " << boost::this_thread::get_id() 
              << "] Завершил обработку файла " << filename 
              << " (время: " << processingTime.count() << " сек)" << std::endl;
    std::cout << std::endl;
}

void workerThread(int workerId) {
    std::cout << "[Рабочий " << workerId << " (поток "  << boost::this_thread::get_id() << ") запущен]" << std::endl;
    
    while (true) {
        std::string filename;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [] {
                return !taskQueue.empty() || processedFilesCount.load() == -1;
            });
            
            if (taskQueue.empty() && processedFilesCount.load() == -1) {
                break;
            }
            
            if (!taskQueue.empty()) {
                filename = taskQueue.front();
                taskQueue.pop();
            } else {
                continue;
            }
        }
        
        if (!filename.empty()) {
            processFile(filename);
        }
    }
    
    std::cout << "[Рабочий " << workerId << " (поток "  << boost::this_thread::get_id() << ") завершает работу]" << std::endl;
}

void printFinalStats(const std::chrono::duration<double>& totalTime) {
    std::cout << "         ИТОГОВАЯ СТАТИСТИКА" << std::endl;
    std::cout << "Обработано файлов: " << processedFilesCount.load() << std::endl;
    std::cout << "Общее время выполнения: " << totalTime.count() << " секунд" << std::endl;
    std::cout << "\nДетальная статистика по файлам:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    int totalWords = 0, totalChars = 0, totalLines = 0;
    for (const auto& stats : allStats) {
        std::cout << "Файл: " << stats.filename << std::endl;
        std::cout << "  Слова: " << stats.wordCount << std::endl;
        std::cout << "  Символы: " << stats.charCount << std::endl;
        std::cout << "  Строки: " << stats.lineCount << std::endl;
        std::cout << "  Время обработки: " << stats.processingTime.count() << " сек" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        totalWords += stats.wordCount;
        totalChars += stats.charCount;
        totalLines += stats.lineCount;
    }
    
    std::cout << "\nОБЩАЯ СТАТИСТИКА:" << std::endl;
    std::cout << "Всего слов: " << totalWords << std::endl;
    std::cout << "Всего символов: " << totalChars << std::endl;
    std::cout << "Всего строк: " << totalLines << std::endl;
}

int main() {    
    auto totalStartTime = std::chrono::steady_clock::now();
    
    std::vector<std::string> filesToProcess = {
        "data1.txt",
        "data2.txt",
        "data3.txt"
    };
    
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        for (const auto& filename : filesToProcess) {
            taskQueue.push(filename);
        }
    }
    
    const int THREAD_POOL_SIZE = 2;
    std::vector<boost::thread> threadPool;
    
    std::cout << "Запуск пула из " << THREAD_POOL_SIZE << " потоков" << std::endl;
    std::cout << std::endl;
    
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        threadPool.emplace_back(workerThread, i + 1);
    }
    
    queueCV.notify_all();
    
    while (processedFilesCount.load() < filesToProcess.size()) {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
    
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        processedFilesCount.store(-1);
    }
    queueCV.notify_all();
    
    for (auto& thread : threadPool) {
        thread.join();
    }
    
    auto totalEndTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> totalTime = totalEndTime - totalStartTime;
    
    processedFilesCount.store(filesToProcess.size());
    
    printFinalStats(totalTime);
    
    return 0;
}