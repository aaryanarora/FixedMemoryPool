CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -I./include
TARGET = memory_pool_example
PERF_TARGET = performance_test
SOURCES = src/TestMemoryPool.cpp
PERF_SOURCES = src/TestPerformance.cpp 

.PHONY: all clean run perf run-perf

all: $(TARGET) $(PERF_TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

$(PERF_TARGET): $(PERF_SOURCES)
	$(CXX) $(CXXFLAGS) -o $(PERF_TARGET) $(PERF_SOURCES)

run: $(TARGET)
	./$(TARGET)

perf: $(PERF_TARGET)

run-perf: $(PERF_TARGET)
	./$(PERF_TARGET)

clean:
	rm -f $(TARGET) $(PERF_TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET) $(PERF_TARGET)

# Release build
release: CXXFLAGS += -DNDEBUG
release: $(TARGET) $(PERF_TARGET)
