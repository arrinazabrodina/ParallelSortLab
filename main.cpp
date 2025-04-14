#include <iostream>
#include <omp.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;


void bitonicMerge(vector<int>& a, int low, int cnt, bool dir) {
    if (cnt > 1) {
        int k = cnt / 2;
        for (int i = low; i < low + k; i++) {
            if (dir == (a[i] > a[i + k]))
                swap(a[i], a[i + k]);
        }
#pragma omp parallel sections
        {
#pragma omp section
            bitonicMerge(a, low, k, dir);
#pragma omp section
            bitonicMerge(a, low + k, k, dir);
        }
    }
}

void bitonicSort(vector<int>& a, int low, int cnt, bool dir) {
    if (cnt > 1) {
        int k = cnt / 2;
#pragma omp parallel sections
        {
#pragma omp section
            bitonicSort(a, low, k, true);
#pragma omp section
            bitonicSort(a, low + k, k, false);
        }
        bitonicMerge(a, low, cnt, dir);
    }
}

void merge(vector<int>& a, int l, int m, int r) {
    vector<int> left(a.begin() + l, a.begin() + m + 1);
    vector<int> right(a.begin() + m + 1, a.begin() + r + 1);
    int i = 0, j = 0, k = l;
    while (i < left.size() && j < right.size()) {
        a[k++] = (left[i] < right[j]) ? left[i++] : right[j++];
    }
    while (i < left.size()) a[k++] = left[i++];
    while (j < right.size()) a[k++] = right[j++];
}

void mergeSort(vector<int>& a, int l, int r) {
    if (l < r) {
        int m = (l + r) / 2;
#pragma omp parallel sections
        {
#pragma omp section
            mergeSort(a, l, m);
#pragma omp section
            mergeSort(a, m + 1, r);
        }
        merge(a, l, m, r);
    }
}

void bubbleSort(vector<int>& a) {
    int n = a.size();
    for (int i = 0; i < n - 1; i++) {
#pragma omp parallel for
        for (int j = 0; j < n - i - 1; j++) {
            if (a[j] > a[j + 1])
                swap(a[j], a[j + 1]);
        }
    }
}


double runBitonic(vector<int> a) {
    double start = omp_get_wtime();
    bitonicSort(a, 0, a.size(), true);
    return omp_get_wtime() - start;
}

double runMerge(vector<int> a) {
    double start = omp_get_wtime();
    mergeSort(a, 0, a.size() - 1);
    return omp_get_wtime() - start;
}

double runBubble(vector<int> a) {
    double start = omp_get_wtime();
    bubbleSort(a);
    return omp_get_wtime() - start;
}


vector<int> generateArray(int size) {
    vector<int> a(size);
    for (int i = 0; i < size; i++)
        a[i] = rand() % 10000;
    return a;
}


int main() {
    srand(time(0));
    vector<int> sizes = {10000, 50000, 100000};

    cout << fixed << setprecision(4);
    cout << "Результати для OpenMP" << endl;
    cout << left << setw(15) << "Розмір масиву"
         << setw(20) << "Bitonic Sort (c)"
         << setw(20) << "Merge Sort (c)"
         << setw(20) << "Bubble Sort (c)" << endl;

    for (int size : sizes) {
        vector<int> data = generateArray(size);
        double t_b = runBitonic(data);
        double t_m = runMerge(data);
        double t_bb = runBubble(data);
        cout << left << setw(15) << size
             << setw(20) << t_b
             << setw(20) << t_m
             << setw(20) << t_bb << endl;
    }

    return 0;
}
