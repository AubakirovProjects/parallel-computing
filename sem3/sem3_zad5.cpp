#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std::chrono;

std::vector<int> BubbleSort(const std::vector<int>& input) {
    std::vector<int> arr = input;
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
    return arr;
}

std::vector<int> InsertionSort(const std::vector<int>& input) {
    std::vector<int> arr = input;
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
    return arr;
}

void Merge(std::vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(n1);
    std::vector<int> R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int i = 0; i < n2; i++) R[i] = arr[mid + i + 1];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void MergeSortImp(std::vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = (left + right) / 2;

        MergeSortImp(arr, left, mid);
        MergeSortImp(arr, mid + 1, right);

        Merge(arr, left, mid, right);
    }
}

std::vector<int> MergeSort(const std::vector<int>& input) {
    std::vector<int> arr = input;
    MergeSortImp(arr, 0, arr.size() - 1);
    return arr;
}

int partition(std::vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }

    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

void QuickSortImp(std::vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        QuickSortImp(arr, low, pi - 1);
        QuickSortImp(arr, pi + 1, high);
    }
}

std::vector<int> QuickSort(const std::vector<int>& input) {
    std::vector<int> arr = input;
    QuickSortImp(arr, 0, arr.size() - 1);
    return arr;
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 1000000);
    std::vector<int> vec(10000);

    for (int& el : vec) el = dist(gen);

    auto start1 = high_resolution_clock::now();
    auto vec1 = BubbleSort(vec);
    auto end1 = high_resolution_clock::now();
    std::cout << "Сортировка пузырьком заняла " << duration_cast<milliseconds>(end1 - start1).count() << " мс" << std::endl;

    auto start2 = high_resolution_clock::now();
    auto vec2 = InsertionSort(vec);
    auto end2 = high_resolution_clock::now();
    std::cout << "Сортировка вставками заняла " << duration_cast<milliseconds>(end2 - start2).count() << " мс" << std::endl;

    auto start3 = high_resolution_clock::now();
    auto vec3 = MergeSort(vec);
    auto end3 = high_resolution_clock::now();
    std::cout << "Сортировка слиянием заняла " << duration_cast<milliseconds>(end3 - start3).count() << " мс" << std::endl;

    auto start4 = high_resolution_clock::now();
    auto vec4 = QuickSort(vec);
    auto end4 = high_resolution_clock::now();
    std::cout << "Быстрая сортировка заняла " << duration_cast<milliseconds>(end4 - start4).count() << " мс" << std::endl;

    auto vec5 = vec;
    auto start5 = high_resolution_clock::now();
    std::sort(vec5.begin(), vec5.end(), [](int a, int b) { return a < b; });
    auto end5 = high_resolution_clock::now();
    std::cout << "Сортировка std::sort заняла " << duration_cast<milliseconds>(end5 - start5).count() << " мс" << std::endl;

    return 0;
}