#include "fftlib.h"
#include <vector>
#include <cmath>

#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>

Complex::Complex(double r, double i) : real(r), imag(i) {}

Complex Complex::operator+(const Complex& b) const {
    return Complex(real + b.real, imag + b.imag);
}

Complex Complex::operator-(const Complex& b) const {
    return Complex(real - b.real, imag - b.imag);
}

Complex Complex::operator*(const Complex& b) const {
    return Complex(real*b.real - imag*b.imag, real*b.imag + imag*b.real);
}

void dft(std::vector<Complex>& a) {
    int N = a.size();
    std::vector<Complex> a_output(N); // Final output array

    for (int k = 0; k < N; ++k) {
        Complex sum(0, 0);
        for (int n = 0; n < N; ++n) {
            double angle = -2.0 * M_PI * k * n / N;
            Complex w(cos(angle), sin(angle));
            sum = sum + a[n] * w;
        }
        a_output[k] = sum;
    }

    a = a_output;
}


void fft_cooley_tukey(std::vector<Complex>& a) {
    int n = a.size();
    if (n <= 1) return;

    // split
    std::vector<Complex> a0(n / 2), a1(n / 2);
    for (int i = 0; 2 * i < n; i++) {
        a0[i] = a[2*i];
        a1[i] = a[2*i+1];
    }

    // recurse
    fft_cooley_tukey(a0);
    fft_cooley_tukey(a1);

    // combine
    double angle = 2 * M_PI / n;
    Complex w(1), wn(cos(angle), sin(angle));
    for (int i = 0; 2 * i < n; i++) {
        a[i] = a0[i] + w * a1[i];
        a[i + n/2] = a0[i] - w * a1[i];
        if (i + n/2 < n) w = w * wn;
    }
}

void fft_iterative(std::vector<Complex>& a) {
    int n = a.size();
    if (n <= 1) return;

    // bit-reverse array elements
    for (int i = 0, j = 0; i < n; ++i) {
        if (j > i) {
            std::swap(a[i], a[j]);
        }
        int m = n / 2;
        while (m >= 2 && j >= m) {
            j -= m;
            m /= 2;
        }
        j += m;
    }

    // iterative FFT
    for (int s = 2; s <= n; s *= 2) {
        Complex wn(cos(2.0 * M_PI / s), sin(2.0 * M_PI / s));
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i += s) {
            Complex w(1);
            for (int j = 0; j < s/2; ++j) {
                Complex t = w * a[i + j + s/2];
                Complex u = a[i + j];
                a[i + j] = u + t;
                a[i + j + s/2] = u - t;
                w = w * wn;
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << "Time: " << s << " " << diff.count() << " seconds\n";
    }
}

void fft_parallel_butterfly(std::vector<Complex>& a, ThreadHive& pool) {
    int n = a.size();
    if (n <= 1) return;

    // bit-reverse array elements
    for (int i = 0, j = 0; i < n; ++i) {
        if (j > i) {
            std::swap(a[i], a[j]);
        }
        int m = n / 2;
        while (m >= 2 && j >= m) {
            j -= m;
            m /= 2;
        }
        j += m;
    }

    // iterative FFT
    for (int s = 2; s <= n; s *= 2) {
        Complex wn(cos(2.0 * M_PI / s), sin(2.0 * M_PI / s));
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i += s) {
            // Schedule a task to perform a group of butterfly operations
            pool.enqueue([&, s, wn, i]() {
                Complex w(1);
                for (int j = 0; j < s/2; ++j) {
                    Complex t = w * a[i + j + s/2];
                    Complex u = a[i + j];
                    a[i + j] = u + t;
                    a[i + j + s/2] = u - t;
                    w = w * wn;
                }
            });
        }

        pool.wait_all();
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << "Time: " << s << " " << diff.count() << " seconds\n";
    }
}

void fft_parallel_chunking(std::vector<Complex>& a, ThreadHive& pool) {
    int n = a.size();
    if (n <= 1) return;

    // bit-reverse array elements
    for (int i = 0, j = 0; i < n; ++i) {
        if (j > i) {
            std::swap(a[i], a[j]);
        }
        int m = n / 2;
        while (m >= 2 && j >= m) {
            j -= m;
            m /= 2;
        }
        j += m;
    }

    // iterative FFT
    for (int s = 2; s <= n; s *= 2) {
        Complex wn(cos(2.0 * M_PI / s), sin(2.0 * M_PI / s));

        auto start = std::chrono::high_resolution_clock::now();
        
        // Try to keep the work done by each thread roughly equal
        int chunkSize = n / 4;
        for (int chunk = 0; chunk < 4; ++chunk) {
            pool.enqueue([&, s, wn, chunk, chunkSize]() {
                int start = chunk * chunkSize;
                int end = (chunk + 1) * chunkSize;
                for (int i = start; i < end; i += s) {
                    Complex w(1);
                    for (int j = 0; j < s/2; ++j) {
                        Complex t = w * a[i + j + s/2];
                        Complex u = a[i + j];
                        a[i + j] = u + t;
                        a[i + j + s/2] = u - t;
                        w = w * wn;
                    }
                }
            });
        }

        pool.wait_all();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << "Time: " << s << " " << diff.count() << " seconds\n";
    }
}

void fft(std::vector<Complex>& a) {
    // parallized is not faster for small inputs (synchronization overhead)
    size_t THRESHOLD = pow(2, 11);
    if (a.size() <= THRESHOLD) {
        fft_iterative(a);
    } else {
        ThreadHive pool(4);
        fft_parallel_chunking(a, pool);
    }
}