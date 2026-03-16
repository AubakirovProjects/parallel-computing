#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

template<typename T>
class MatrixProcessor {
private:
    std::vector<std::vector<T>> matrix;
    size_t rows, cols;
    int threads_count;
    
    std::mutex mtx;
    std::condition_variable cv;
    int finished_threads;
    
public:
    MatrixProcessor(const std::vector<std::vector<T>>& mat, int threads) : matrix(mat), threads_count(threads), finished_threads(0) {
        
        rows = matrix.size();
        
        if (rows > 0) {
            cols = matrix[0].size();
        } else {
            cols = 0;
        }
    }
    
    void apply(T (*func)(T)) {  
        int total = rows * cols;
        int chunk = total / threads_count;
        
        for (int i = 0; i < threads_count; i++) {
            int start = i * chunk;
            
            int end;
            if (i == threads_count - 1) {
                end = total;
            } else {
                end = start + chunk;
            }
            
            std::thread([this, func, start, end]() {
                for (int idx = start; idx < end; idx++) {
                    int row = idx / cols;
                    int col = idx % cols;
                    matrix[row][col] = func(matrix[row][col]);
                }
                
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    finished_threads++;
                }
                cv.notify_one();
            }).detach();
        }
        
        std::unique_lock<std::mutex> lock(mtx);
        while (finished_threads < threads_count) {
            cv.wait(lock);
        }
    }
    
    void print() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                std::cout << matrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    std::vector<std::vector<int>> mat = {{1,2,3}, {4,5,6}, {7,8,9}};
    
    MatrixProcessor<int> mp(mat, 3);
    
    std::cout << "Исходная матрица:\n";
    mp.print();

    mp.apply([](int x) {return x + x; });
    
    std::cout << "\nРезультат:\n";
    mp.print();
    
    return 0;
}