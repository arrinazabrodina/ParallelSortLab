#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

vector<int> generateArray(int size) {
    vector<int> a(size);
    for (int i = 0; i < size; i++)
        a[i] = rand() % 10000;
    return a;
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
        mergeSort(a, l, m);
        mergeSort(a, m + 1, r);
        merge(a, l, m, r);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<int> sizes = {10000, 50000, 100000};
    if (rank == 0) {
        cout << fixed << setprecision(4);
        cout << "Результати для MPI" << endl;
        cout << left << setw(15) << "Розмір масиву"
             << setw(20) << "Merge Sort (c)" << endl;
    }

    for (int n : sizes) {
        vector<int> fullArray;
        int localSize = n / size;
        vector<int> localArray(localSize);

        if (rank == 0) {
            fullArray = generateArray(n);
        }

        MPI_Scatter(fullArray.data(), localSize, MPI_INT,
                    localArray.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);

        double start = MPI_Wtime();
        mergeSort(localArray, 0, localSize - 1);
        double localTime = MPI_Wtime() - start;

        vector<int> gathered(n);
        MPI_Gather(localArray.data(), localSize, MPI_INT,
                   gathered.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);

        double maxTime;
        MPI_Reduce(&localTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            for (int i = 1; i < size; ++i) {
                int l = 0;
                int m = i * localSize - 1;
                int r = (i + 1) * localSize - 1;
                merge(gathered, l, m, r);
            }

            cout << left << setw(15) << n
                 << setw(20) << maxTime << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
