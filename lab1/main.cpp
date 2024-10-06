#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <iomanip>

void appendResultsToFile(double timeSeq, double timePar, int N, int threads_num) {
    std::ofstream outfile;
    outfile.open("results.txt", std::ios_base::app);
    outfile << "N: " << N << ", Threads: " << threads_num << ", Sequential Time: " << timeSeq << " seconds, Parallel Time: " << timePar << " seconds" << std::endl;
    outfile.close();
}

void fillMatrix(int** a, const int N, bool random = true) {
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            a[i][j] = random ? rand() % 100 : 0;
        }
    }
}

void transposeMatrix(const int** b, int** b_transposed, const int N) {
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            b_transposed[j][i] = b[i][j];
        }
    }
}

std::string printMatrix(const int** a, const int N){
    std::string result = "";
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            result += std::to_string(a[i][j]) + " ";
        }
        result += "\n";
    }
    return result;
}

void multiplyMatrixSeq(const int** a, const int** b, int** c, const int N) {
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            c[i][j] = 0;
            for(int k = 0; k < N; ++k) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void multiplyMatrixPar(const int** a, const int** b, int** c, const int N, int num_threads) {
    std::thread threads[num_threads];
    int chunk_size = N / num_threads;
    
    for(int t = 0; t < num_threads; ++t) {
        threads[t] = std::thread([&, t](){
            int start = t * chunk_size;
            int end = (t + 1) * chunk_size;
            for(int i = start; i < end; ++i) {
                for(int j = 0; j < N; ++j) {
                    for(int k = 0; k < N; ++k) {
                        c[i][j] += a[i][k] * b[k][j];
                    }
                }
            }
        });
    }
    
    for(int t = 0; t < num_threads; ++t) {
        threads[t].join();
    }
}

void resetMatrix(int** a, const int N) {
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            a[i][j] = 0;
        }
    }
}

int main(int, char**){
    srand(time(NULL));
    double time1, time2;

    // wymiar macierzy NxN
    const int N = 500;
    // liczba watkow
    const int threads_num = 8;

    //inicjalizacja tablic 2 wymiarowych a, b, c
    int **a = new int*[N];
    int **b = new int*[N];
    int **c = new int*[N];
    int **b_transposed = new int*[N];

    for(int i = 0; i < N; ++i) {
        a[i] = new int[N];
        b[i] = new int[N];
        c[i] = new int[N];
        b_transposed[i] = new int[N];
    }

    //wypelnienie macierzy a, b - losowymi wartosciami
    fillMatrix(a, N);
    fillMatrix(b, N);
    //wypelnienie macierzy c - zerami
    fillMatrix(c, N, false);

    // transpozycja macierzy b
    transposeMatrix((const int**)b, b_transposed, N);

    // std::cout << "Matrix A: \n" << printMatrix((const int**)a, N) << std::endl;
    // std::cout << "Matrix B: \n" << printMatrix((const int**)b, N) << std::endl;
    // std::cout << "Matrix C: \n" << printMatrix((const int**)c, N) << std::endl;

    // mnozenie macierzy sekwencyjne
    auto start = std::chrono::high_resolution_clock::now();
    multiplyMatrixSeq((const int**)a, (const int**)b_transposed, c, N);
    auto end = std::chrono::high_resolution_clock::now();
    time1 = std::chrono::duration<double>(end - start).count();

    // wyzerowanie macierzy c
    resetMatrix(c, N);

    // mnozenie macierzy rownolegle
    start = std::chrono::high_resolution_clock::now();
    multiplyMatrixPar((const int**)a, (const int**)b_transposed, c, N, threads_num);
    end = std::chrono::high_resolution_clock::now();
    time2 = std::chrono::duration<double>(end - start).count();

    // wyniki
    std::cout << std::fixed << std::setprecision(3); // Set fixed-point notation and precision
    std::cout << "Matrix multiplication time: " << time1 << " seconds\n";
    std::cout << "Matrix multiplication time (parallel): " << time2 << " seconds\n";

    // zapis wynikow do pliku
    appendResultsToFile(time1, time2, N, threads_num);

    // zwolnienie pamieci
    for(int i = 0; i < N; ++i) {
        delete[] a[i];
        delete[] b[i];
        delete[] c[i];
        delete[] b_transposed[i];
    }
    delete[] a;
    delete[] b;
    delete[] c;
    delete[] b_transposed;

    // koniec
    return 0;
}