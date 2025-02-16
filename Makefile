# Build targets (your implementation targets should go in IMPL_O)
TEST_O=test_driver/test.o 
IMPL_O=implementation/core.o implementation/core_only_structure.o implementation/core_only_multi_threading.o implementation/core_only_caching.o implementation/queries.o implementation/cache.o implementation/distance.o

# Compiler flags
CC  = gcc
CXX = g++ -std=c++11
CFLAGS=-O3 -fPIC -Wall -g -I. -I./include
CXXFLAGS=$(CFLAGS)
LDFLAGS=-lpthread

# The programs that will be built
PROGRAMS=testdriver

# The name of the library that will be built
LIBRARY=core

# Build all programs
all: $(PROGRAMS)

lib: $(IMPL_O)
	$(CXX) $(CXXFLAGS) -shared -o lib$(LIBRARY).so $(IMPL_O)

testdriver: lib $(TEST_O)
	$(CXX) $(CXXFLAGS) -o testdriver $(TEST_O) ./lib$(LIBRARY).so

core_only_caching: implementation/core_only_caching.o $(filter-out implementation/core.o, $(IMPL_O))
	$(CXX) $(CXXFLAGS) -o core_only_caching implementation/core_only_caching.o $(filter-out implementation/core.o, $(IMPL_O)) ./lib$(LIBRARY).so

core_only_structure: implementation/core_only_structure.o $(filter-out implementation/core.o, $(IMPL_O))
	$(CXX) $(CXXFLAGS) -o core_only_structure implementation/core_only_structure.o $(filter-out implementation/core.o, $(IMPL_O)) ./lib$(LIBRARY).so

core_only_multi_threading: implementation/core_only_multi_threading.o $(filter-out implementation/core.o, $(IMPL_O))
	$(CXX) $(CXXFLAGS) -o core_only_multi_threading implementation/core_only_multi_threading.o $(filter-out implementation/core.o, $(IMPL_O)) ./lib$(LIBRARY).so

clean:
	rm -f $(PROGRAMS) lib$(LIBRARY).so core_only_caching core_only_structure core_only_multi_threading
	find . -name '*.o' -print | xargs rm -f