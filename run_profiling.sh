#!/bin/bash

# Array of targets to profile
targets=("core_only_caching" "core" "core_only_structure" "core_only_multi_threading")
output_files=("powermetrics_output_only_caching.txt" "powermetrics_output_core.txt" "powermetrics_output_only_structure.txt" "powermetrics_output_only_multi_threading.txt")

# Build all targets
make all

# Iterate over each target and run profiling.sh
for i in "${!targets[@]}"; do
    target="${targets[$i]}"
    output_file="${output_files[$i]}"
    echo "Profiling $target"
    make $target
    ./profiling.sh "$output_file"
done
