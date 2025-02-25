We as a group (Furkan, Efe, Utku) tried to:

Implement core API functions (StartQuery(), EndQuery(), MatchDocument(), GetNextAvailRes()) and create a test driver to validate them using sample data files.

Optimize the initial implementation for higher throughput using techniques like multithreading, caching, and data structures.

Develop a data-parallel version using a framework like Apache Spark, Flink, or Dask, and compare its performance with previous implementations.

Deliverables include the source code, test results, and reports detailing implementation, optimizations, and performance comparisons.

There might be some errors on the terminal. Not important ones.

// IMPORTANT NOTE
core.cpp or powermetrics_output_core === multi threading + caching + better code structure for C++.

Base output is inside python branch

For the main branch, speeds stated in the terminal are wrong. If you need exact runtimes switch to branches such as only_structure, only_caching etc.

## Minimum Requirements

XCode for MacOS

GCC/G++ (Version supporting C++17)

Python 3 (Installed and accessible as python3)

Pip (For dependency installation)

## Testing Instructions

To test the implementation, follow these steps:

\*\* MAIN BRANCH -- C++ IMPLEMENTATIONS ALL TOGETHER

```bash
make clean
make
chmod +x run_profiling.sh            // You might need to be in the run_profiling.sh directory
./run_profiling.sh
```

\*\* PYTHON_IMPLEMENTATION BRANCH -- PYTHON IMPLEMENTATIONS + BASE PROJECT
To test the implementation, follow these steps:

```bash
make
```

Then, setup the python environment for the required libraries:

```bash
make setup_python
```

Make sure to update your python interpreter to the .venv path in your workspace:
".../AWT-2024-2025/.venv/bin/python"

For Visual Studio Code:

Press `Ctrl+Shift+P`(Windows) or `Cmd+Shift+P`(Mac) to open the command palette.
Search for "Python: Select Interpreter" and click on "Enter interpreter path...". And enter .venv path.

Close and Reopen Visual Studio Code if necessary

Start the test using:

```bash
python benchmark.py
```

\*\* C BRANCHES -- SINGLE C++ BRANCHES SUCH AS C_ONLY_STRUCTURE, C_ONLY_CACHING ...

```bash
make clean
make
chmod +x profiling.sh            // You might need to be in the profiling.sh directory
./profiling.sh
```

\*\* Results Docs
To see results check "powermetrics_output" files on the project.

For MacOS search for "Combined Power". If there are more than one. Sum up all the values for the total.

For Intel search for "derived package power". If there are more than one. Sum up all the values for the total.
