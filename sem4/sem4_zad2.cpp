#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

template<typename T>
class Account {
private:
    T balance;
    std::mutex mtx;
    std::condition_variable cv;
    
public:
    Account(T init) : balance(init) {}
    
    T get_balance() {
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
    }
    
    void deposit(T amount) {
        std::lock_guard<std::mutex> lock(mtx);
        balance += amount;
        cv.notify_all();
    }
    
    bool withdraw(T amount) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this, amount]() { return balance >= amount; });
        balance -= amount;
        return true;
    }
    
    std::mutex& get_mutex() { return mtx; }
    std::condition_variable& get_cv() { return cv; }
};

template<typename T>
class Bank {
private:
    std::vector<Account<T>> accounts;
    static std::mutex cout_mtx;
    
public:
    Bank(std::vector<T> inits) {
        for (T val : inits) accounts.emplace_back(val);
    }
    
    inline void do_transfer(int from, int to, T amount) {
        accounts[from].withdraw(amount);  
        accounts[to].deposit(amount); 
    }
    
    void transfer(int from, int to, T amount) {
        auto id = std::this_thread::get_id();
        
        if (from < to) {
            std::lock_guard<std::mutex> lock1(accounts[from].get_mutex());
            std::lock_guard<std::mutex> lock2(accounts[to].get_mutex());
            do_transfer(from, to, amount);
        } else {
            std::lock_guard<std::mutex> lock1(accounts[to].get_mutex());
            std::lock_guard<std::mutex> lock2(accounts[from].get_mutex());
            do_transfer(from, to, amount);
        }
        
        {
            std::lock_guard<std::mutex> lock(cout_mtx);
            std::cout << "Поток " << id << ": " << from << " → " << to  << " (" << amount << ")\n";
        }
    }
    
    T total_balance() {
        T sum = 0;
        for (auto& acc : accounts) sum += acc.get_balance();
        return sum;
    }
};

template<typename T>
std::mutex Bank<T>::cout_mtx;

int main() {
    Bank<double> bank({1000, 1000, 1000, 1000, 1000});
    double start = bank.total_balance();
    
    std::vector<std::thread> clients;
    clients.emplace_back([&bank]() {
        bank.transfer(0, 1, 100); 
        bank.transfer(1, 2, 50); 
        bank.transfer(2, 3, 30);
    });
    
    clients.emplace_back([&bank]() {
        bank.transfer(3, 4, 200);
        bank.transfer(4, 0, 150);
        bank.transfer(1, 3, 80); 
    });
    
    clients.emplace_back([&bank]() {
        bank.transfer(2, 4, 120);
        bank.transfer(0, 2, 90);
        bank.transfer(4, 1, 70); 
    });
    
    for (auto& t : clients) t.join();
    
    std::cout << "Начало: " << start << ", Конец: " << bank.total_balance() << std::endl;
    return 0;
}