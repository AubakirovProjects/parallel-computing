#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <semaphore>
#include <atomic>
#include <memory>

struct FileChunk {
    int chunk_id;
    int file_id;
    size_t size;
    
    void download() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100 + (size % 100)));
    }
};

class FileDownload {
private:
    int file_id;
    std::vector<FileChunk> chunks;
    std::atomic<int> downloaded_chunks;

public:
    FileDownload(int id, int num_chunks) : file_id(id), downloaded_chunks(0) {
        for (int i = 0; i < num_chunks; i++) {
            chunks.push_back({i, id, static_cast<size_t>(100 + i * 10)});
        }
    }
    
    FileDownload(const FileDownload&) = delete;
    FileDownload& operator=(const FileDownload&) = delete;
    
    FileDownload(FileDownload&& other) noexcept 
        : file_id(other.file_id), 
          chunks(std::move(other.chunks)),
          downloaded_chunks(other.downloaded_chunks.load()) {}
    
    FileDownload& operator=(FileDownload&& other) noexcept {
        if (this != &other) {
            file_id = other.file_id;
            chunks = std::move(other.chunks);
            downloaded_chunks = other.downloaded_chunks.load();
        }
        return *this;
    }
    
    bool is_complete() const {
        return downloaded_chunks.load() == static_cast<int>(chunks.size());
    }
    
    void mark_chunk_downloaded(int chunk_id) {
        downloaded_chunks++;
        std::cout << "Файл " << file_id << " часть " << chunk_id 
                  << " загружена. Прогресс: " << downloaded_chunks.load() 
                  << "/" << chunks.size() << "\n";
    }
    
    int get_file_id() const { return file_id; }
    
    const std::vector<FileChunk>& get_chunks() const { return chunks; }
};

class DownloadManager {
private:
    std::queue<FileChunk> download_queue;
    std::counting_semaphore<> active_downloads;
    std::counting_semaphore<> chunk_downloads;
    std::mutex queue_mutex;
    std::mutex files_mutex;
    std::atomic<int> completed_files;
    std::vector<std::unique_ptr<FileDownload>> files;
    bool running;

public:
    DownloadManager(int max_files, int max_chunks) 
        : active_downloads(max_files), chunk_downloads(max_chunks), completed_files(0), running(true) {}

    void add_file(std::unique_ptr<FileDownload> file) {
        {
            std::lock_guard<std::mutex> lock(files_mutex);
            files.push_back(std::move(file));
        }
        
        const auto& chunks_ref = files.back()->get_chunks();
        for (const auto& chunk : chunks_ref) {
            std::lock_guard<std::mutex> lock(queue_mutex);
            download_queue.push(chunk);
            std::cout << "Добавлена часть " << chunk.chunk_id 
                      << " файла " << chunk.file_id << " в очередь\n";
        }
    }
    
    inline void process_chunk(FileChunk chunk) {
        std::cout << "Поток " << std::this_thread::get_id() 
                  << " загружает файл " << chunk.file_id 
                  << " часть " << chunk.chunk_id 
                  << " (размер: " << chunk.size << ")\n";
        
        chunk.download();
        
        std::cout << "Поток " << std::this_thread::get_id() 
                  << " завершил загрузку файла " << chunk.file_id 
                  << " часть " << chunk.chunk_id << "\n";
        
        std::lock_guard<std::mutex> lock(files_mutex);
        for (auto& file_ptr : files) {
            if (file_ptr->get_file_id() == chunk.file_id) {
                file_ptr->mark_chunk_downloaded(chunk.chunk_id);
                
                if (file_ptr->is_complete()) {
                    completed_files++;
                    std::cout << "Файл " << chunk.file_id << " полностью загружен! "
                              << "Всего завершено файлов: " << completed_files << "\n";
                }
                break;
            }
        }
    }
    
    void download_worker() {
        while (running) {
            std::this_thread::yield();
            
            FileChunk chunk;
            bool has_chunk = false;
            
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (!download_queue.empty()) {
                    chunk = download_queue.front();
                    download_queue.pop();
                    has_chunk = true;
                }
            }
            
            if (!has_chunk) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }
            
            if (!active_downloads.try_acquire()) {
                std::lock_guard<std::mutex> lock(queue_mutex);
                download_queue.push(chunk);
                continue;
            }
            
            if (!chunk_downloads.try_acquire()) {
                active_downloads.release();
                std::lock_guard<std::mutex> lock(queue_mutex);
                download_queue.push(chunk);
                continue;
            }
            
            process_chunk(chunk);
            
            chunk_downloads.release();
            active_downloads.release();
        }
    }
    
    void stop() {
        running = false;
    }
    
    int get_completed_files() const {
        return completed_files.load();
    }
    
    void print_statistics() {
        std::cout << "\n=== Статистика загрузок ===\n";
        std::cout << "Завершено файлов: " << completed_files.load() << "\n";
        
        std::lock_guard<std::mutex> lock(files_mutex);
        for (const auto& file_ptr : files) {
            std::cout << "Файл " << file_ptr->get_file_id() << ": " 
                      << (file_ptr->is_complete() ? "завершен" : "не завершен") << "\n";
        }
    }
};

int main() {
    DownloadManager manager(2, 3);
    
    std::vector<std::thread> workers;
    
    for (int i = 0; i < 4; i++) {
        workers.emplace_back([&manager]() {
            manager.download_worker();
        });
    }
    
    auto file1 = std::make_unique<FileDownload>(1, 4);
    auto file2 = std::make_unique<FileDownload>(2, 3);
    auto file3 = std::make_unique<FileDownload>(3, 5);
    
    manager.add_file(std::move(file1));
    manager.add_file(std::move(file2));
    manager.add_file(std::move(file3));
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    manager.stop();
    
    for (auto& t : workers) {
        t.detach();
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    manager.print_statistics();
    
    return 0;
}