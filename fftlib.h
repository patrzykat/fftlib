#ifndef FFT_H
#define FFT_H

#include "ThreadHive.h"
#include <vector>

// Complex class declaration
class Complex {
public:
    double real, imag;

    Complex(double r = 0, double i = 0);
    Complex operator+(const Complex& b) const;
    Complex operator-(const Complex& b) const;
    Complex operator*(const Complex& b) const;
};

// Function declarations
void fft(std::vector<Complex>& a);
void dft(std::vector<Complex>& a);
void fft_cooley_tukey(std::vector<Complex>& a);
void fft_iterative(std::vector<Complex>& a);
void fft_parallel_butterfly(std::vector<Complex>& a, ThreadHive& pool);
void fft_parallel_chunking(std::vector<Complex>& a, ThreadHive& pool);

#endif // FFT_H
