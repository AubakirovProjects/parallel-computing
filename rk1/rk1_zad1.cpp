#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <chrono>
#include <random>

const int StationsAmount = 3;
const int WorkersAmount = 5;

std::mutex mtx;
std::condition_variable cv;

bool stations[StationsAmount] = {false};

std::atomic<int> completedTasks(0);

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> workTimeDist(1, 3); 

int findFreeStation() {
    for (int i = 0; i < StationsAmount; ++i) {
        if (!stations[i]) {
            return i;
        }
    }
    return -1;
}

void worker(int workerId) {
    int workTime = workTimeDist(gen);

    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, [] {
        return findFreeStation() != -1;
    });

    int stationId = findFreeStation();
    stations[stationId] = true;

    std::cout << "Рабочий " << workerId  << " начал работу на станке " << stationId + 1  << " на " << workTime << " секунд(ы)." << std::endl;

    lock.unlock();

    std::this_thread::sleep_for(std::chrono::seconds(workTime));

    lock.lock();
    stations[stationId] = false;
    completedTasks++;

    std::cout << "Рабочий " << workerId   << " закончил работу на станке " << stationId + 1  << ". Всего выполнено задач: " << completedTasks.load() << std::endl; 

    cv.notify_one();
}

int main() {
    std::vector<std::thread> workers;

    for (int i = 1; i <= WorkersAmount; ++i) {
        workers.emplace_back(worker, i);
    }

    for (auto& t : workers) {
        t.join();
    }

    std::cout << "\nВсе рабочие завершили работу. Всего выполнено задач: "   << completedTasks.load() << std::endl;

    return 0;
}