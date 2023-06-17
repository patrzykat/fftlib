# fftlib: Parallelized FFT Library

A high-performance C++ library implementing a parallelized Fast Fourier Transform using my own custom-built thread pool.

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
- [Interpreting Results](#interpreting-results)
- [Performance and Optimization](#performance-and-optimization)
- [Math Background](#math-background)
- [Contributing](#contributing)
- [License](#license)

## Features

- Efficient FFT calculations: Implementation of the Cooley-Tukey FFT algorithm for efficient conversion of time-domain signals to frequency-domain signals.
- Parallel computing: Use of a custom thread pool library to parallelize computations for improved performance.
- Interactive testing: An interactive test suite to verify the functionality and measure the performance of the library.
- Easy integration: Designed for easy integration into other C++ projects.

## Getting Started

### How to use in your project

[ThreadHive](https://github.com/patrzykat/ThreadHive.git) is easy to install. You just need to clone the repository and compile the source code:

```bash
# clone the repository
git clone https://github.com/patrzykat/fftlib.git

# add and compile ThreadHive dependency
cd fftlib
git clone https://github.com/patrzykat/ThreadHive.git
cd ThreadHive
make

# compile FFT
cd ..
make
```

Then in your files you can

```cpp
#include "fft.h"
```

### How to run interactive tests

```bash
cd fftlib
./fft_tests
```

## Interpreting Results

A FFT (Fast Fourier Transform) gives you the frequency components of the signal. The output of the FFT is a list of complex numbers, which represent the amplitude and phase of the frequency components. Here's how to interpret these results:

### The Output Array

The output of the FFT is an array of complex numbers. The length of this array is equal to the number of samples you gave as the input to the FFT. Each complex number represents a certain frequency component in the input signal.

Here's what each part of the array means:

- Index `0`: This is the DC component of the signal, which is equivalent to the average value of the signal. The frequency of this component is 0.

- Indices `1` to `N/2`: These indices represent the positive frequencies in the signal. The frequency corresponding to index `k` is `k*(Fs/N)` where `Fs` is the sampling rate and `N` is the number of samples.

- Indices `N/2+1` to `N-1`: These indices represent the negative frequencies in the signal. They are mirror images of the positive frequencies, and are usually not needed for most analyses. For real-valued signals, these will be complex conjugates of the corresponding positive frequencies.

### Amplitude and Phase

Each value in the FFT output array is a complex number. Here's what each part of the complex number means:

- The **magnitude** of the complex number corresponds to the **amplitude** of the frequency component.

- The **angle** of the complex number (when represented in polar form) corresponds to the **phase** of the frequency component.

The amplitude and phase give you information about how much of a certain frequency is present in the signal. The units of the FFT results are in terms of your original signal units per cycle, not per second.

### Interpreting the FFT Plot

Here's what each part of the plot means:

- The x-axis represents the frequency. It starts from zero and goes up to half the sampling rate.
- The y-axis represents the amplitude of the corresponding frequency component. This tells you how much of each frequency is present in the signal.

A common strategy is to look for peaks in the y-axis. These peaks represent the presence and strength of certain frequencies in your time series.

### Example interpretation (daily weather data)

There is a dataset that contains 1024 days of weather data. The results show a spike around 3. This makes sense because there are approximately 3 cycles of 365 days in the sample of 1024 days. 

## Performance and Optimization

### Cooley-Tukey Algorithm (Recursive)

The Cooley-Tukey algorithm is a Fast Fourier Transform (FFT) algorithm that recursively breaks down a DFT into many smaller DFTs, along with O(N) multiplications by complex roots of unity traditionally called "twiddle factors".

This "divide and conquer" approach changes the order of computation from O(N²) in the case of direct computation, to O(N log N) for the FFT, a significant reduction in complexity and increase in speed for large N.

### Iterative Algorithm

An iterative approach to the Cooley-Tukey FFT was found to be approximately 3 times faster. This is primarily due to avoiding the overhead of recursion.

Iterative algorithm implementation follows a bottom-up construction where the input data is treated as N DFTs of size 1, and each stage merges pairs of smaller DFTs into larger ones until the final DFT is obtained.

Reference: [Kumar, Rohit (2023). Iterative Fast Fourier Transformation for polynomial multiplication](https://www.geeksforgeeks.org/iterative-fast-fourier-transformation-polynomial-multiplication/)

### Butterfly Parallelization

The original attempt to parallelize the iterative algorithm led to a significantly slower implementation. This was due to the high synchronization overhead for low s values, where s is the stride at each stage of the algorithm.

### Chunked Parallelization

To overcome the limitations of the original parallelized iterative algorithm, a chunked parallelization approach was adopted. Here, the data is divided into chunks that are processed in parallel. The chunk size is chosen to balance the load across threads, while minimizing synchronization overhead.

This method achieved much better performance and scalability than the original parallelized iterative algorithm.

### Thread Pool Library

For the parallelization, I utilized my own thread pool library, `ThreadHive`. This library provides an interface to efficiently manage and utilize a pool of threads, with support for task queuing and dynamic resizing of the thread pool.

This allowed for efficient use of system resources, and the ability to finely tune the parallelization for optimal performance.

## Math Background

### Discrete Fourier Transform (DFT)

The Discrete Fourier Transform (DFT) turns a series of numbers into its frequency components. The DFT is given by the formula:

```math
X_k = \sum_{n=0}^{N-1} x_n e^{-i 2\pi k \frac{n}{N}}
```

where $x_n$ are the time domain samples, $X_k$ are the frequency domain samples, $N$ is the number of samples, and $i$ is the imaginary unit.

Computing the DFT involves adding, subtracting, and multiplying complex numbers, each represented by a pair of real numbers in your code.

### Fast Fourier Transform (FFT)

The Fast Fourier Transform (FFT) is an efficient algorithm for computing the DFT. The key insight of the FFT is to recursively divide the DFT computation into two smaller DFT computations. The most common FFT algorithm is the the Cooley-Tukey algorithm.

### Cooley-Tukey FFT

The Cooley-Tukey Fast Fourier Transform (FFT) algorithm is a way to compute the DFT faster by splitting the problem into smaller parts, solving each part, and then combining the solutions. This approach, known as divide and conquer, reduces the amount of work needed to get the final result.

In coding terms, this can be achieved by:

1. Decimation: Splitting the input array into even-indexed and odd-indexed parts.
2. Recursive FFTs: Recursively applying FFTs to these two halves.
3. Combination: Combining these two results to produce the final FFT result. This is the most complex part and involves maintaining "twiddle factors" (w), which represents an angle (or a point on the unit circle in a complex plane).

This approach reduces the time complexity from $O(N^2)$ of the DFT to $O(N \log N)$ of the FFT.

## Contributing

We welcome contributions! Feel free submit a PR!

## License

fftlib is licensed under the MIT License.
