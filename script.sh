#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"

# fetch the latest version of cJSON
# curl -o include/cJSON.c "https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.c"
# curl -o include/cJSON.h "https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.h"

# Compile C files
gcc -Wall ./src/solve_dIdX_RK4.c ./include/cJSON.c -o ./bin/solve_dIdX_RK4  -lm

# Execute the binaries 
./bin/solve_dIdX_RK4 ./data/data_ding.json ./results/results.csv

# Analyse the results
# -batch combines -nodesktop, -nodisplay, and -nosplash
# https://stackoverflow.com/questions/65200329/matlab-r2020a-run-m-script-from-terminal
matlab -sd ./src -batch analyse_results