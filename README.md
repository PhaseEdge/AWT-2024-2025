We as a group (Furkan, Efe, Utku) tried to:

Implement core API functions (StartQuery(), EndQuery(), MatchDocument(), GetNextAvailRes()) and create a test driver to validate them using sample data files.

Optimize the initial implementation for higher throughput using techniques like multithreading, caching, and data structures.

Develop a data-parallel version using a framework like Apache Spark, Flink, or Dask, and compare its performance with previous implementations.

Deliverables include the source code, test results, and reports detailing implementation, optimizations, and performance comparisons.

// IMPORTANT NOTE
core.cpp or powermetrics_output_core === multi threading + caching + better code structure for C++.
Base output is inside python branch

## Testing Instructions

To test the implementation, follow these steps:

```bash
make clean
make
chmod +x run_profiling.sh            // You might need to be in the run_profiling.sh directory
./run_profiling.sh
```
