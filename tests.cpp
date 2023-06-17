#include "fftlib.h"
#include <vector>
#include <cmath>

#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>

std::vector<Complex> create_example_time_series() {
    // const int N = pow(2, 8);
    const int N = pow(2, 22);
    // const double PI = 3;
    const double PI = std::acos(-1);
    std::vector<Complex> a(N);

    for(int i = 0; i < N; i++) {
        double t = (double)i / N; // time
        // First signal 15 Hz
        double s1 = std::sin(2 * PI * 15 * t);
        // Second signal 50 Hz
        double s2 = std::sin(2 * PI * 50 * t);
        // Third signal 100 Hz
        double s3 = std::sin(2 * PI * 100 * t);

        a[i] = Complex(s1 + s2 + s3, 0);
    }

    return a;
}

std::vector<Complex> read_stock_csv_file(const std::string& filename) {
    std::vector<Complex> a;
    std::ifstream file(filename);
    std::string line;

    // Skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string data;

        // Skip date, open, high, low columns
        for (int i = 0; i < 4; ++i)
            std::getline(ss, data, ',');

        // Read close column
        std::getline(ss, data, ',');
        double close = std::stod(data);

        // Add to vector
        a.push_back(Complex(close, 0));
    }

    return a;
}

std::vector<Complex> read_weather_csv_file(const std::string& filename) {
    std::vector<Complex> a;
    std::ifstream file(filename);
    std::string line;

    // Skip header
    std::getline(file, line);

    int line_count = 0;

    while (std::getline(file, line) && line_count < 1024) {
        std::stringstream ss(line);
        std::string data;

        // skip date
        for (int i = 0; i < 1; ++i)
            std::getline(ss, data, ',');

        // Read close column
        std::getline(ss, data, ',');
        double close = std::stod(data);
        a.push_back(Complex(close, 0));
        line_count++;
    }

    return a;
}


int main() {
    // std::vector<Complex> a = read_stock_csv_file("data/apple.csv");
    // std::vector<Complex> a = read_weather_csv_file("data/delhi_temperatures.csv");
    std::vector<Complex> a = create_example_time_series();
    std::vector<Complex> a1 = a;
    std::vector<Complex> a2 = a;
    std::vector<Complex> a3 = a;
    std::vector<Complex> a4 = a;

    const int N = a.size();

    // fft_cooley_tukey
    auto start = std::chrono::high_resolution_clock::now();
    fft_cooley_tukey(a1);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "fft_cooley_tucky " << elapsed.count() << " seconds." << std::endl;

    // fft_iterative
    start = std::chrono::high_resolution_clock::now();
    fft_iterative(a2);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "fft_iterative took " << elapsed.count() << " seconds." << std::endl;
    
    // fft_parallel_butterfly
    start = std::chrono::high_resolution_clock::now();
    ThreadHive pool(4);
    fft_parallel_butterfly(a3, pool);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "fft_parallel_butterfly took " << elapsed.count() << " seconds." << std::endl;

    // fft_parallel_chuncking
    start = std::chrono::high_resolution_clock::now();
    fft_parallel_chunking(a4, pool);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "fft_parallel_chunking took " << elapsed.count() << " seconds." << std::endl;

    // Print the magnitudes of the first N/2 frequencies
    for(int i = 0; i < std::min(128, N / 2); i++) {
        std::cout << i << ": " << std::sqrt(a1[i].real * a1[i].real + a1[i].imag * a1[i].imag) << std::endl;
    }

    return 0;
}
