# c++ compiler
CXX = g++

# flags
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -I.

# output program name for fft and tests
# FFT_OUTPUT = fft
TEST_OUTPUT = fft_tests

# path to thread pool library
TH_PATH = ThreadHive

# default rule
all: $(TEST_OUTPUT)

# rule for building fft program
# $(FFT_OUTPUT): fft.cpp
# 	$(CXX) $(CXXFLAGS) -I$(TH_PATH) -L$(TH_PATH) fft.cpp -lThreadHive -o $(FFT_OUTPUT)

# rule for building test program
$(TEST_OUTPUT): tests.cpp fftlib.cpp
	$(CXX) $(CXXFLAGS) -I$(TH_PATH) -L$(TH_PATH) tests.cpp fftlib.cpp -lThreadHive -o $(TEST_OUTPUT)

# clean rule
clean:
	$(RM) $(FFT_OUTPUT) $(TEST_OUTPUT)


# # c++ compiler
# CXX = g++

# # flags
# CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -I.

# # output program name
# OUTPUT = fft

# # path to thread pool library
# TH_PATH = ThreadHive

# # default rule
# all: $(OUTPUT)

# # rule for building fft program
# $(OUTPUT): fft.cpp
# 	$(CXX) $(CXXFLAGS) -I$(TH_PATH) -L$(TH_PATH) fft.cpp -lThreadHive -o $(OUTPUT)

# # clean rule
# clean:
# 	$(RM) $(OUTPUT)
