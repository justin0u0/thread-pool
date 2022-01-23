CXX = g++
CXXFLAGS = -std=c++11 -O3 -fsanitize=address -g
LDFLAGS = -pthread
TARGETS = pthread/thread_pool_test

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	rm -f $(TARGETS)

%: %.cpp
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $^
