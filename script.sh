set -e                  # exits the script if an error is encountered
cd "$(dirname "$0")"    # cd to the directory of the script to access other files by relative path

# fetch the latest version of cJSON
# curl -o include/cJSON.c "https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.c"
# curl -o include/cJSON.h "https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.h"

# Compile C files
gcc -Wall ./src/solve_dIdX_RK4.c ./include/cJSON.c -o ./bin/solve_dIdX_RK4  -lm

# Execute the binaries 
# https://stackoverflow.com/questions/8512462/looping-through-all-files-in-a-directory
for filename in ./data/scenarios/*.json; do
    echo "Simulating: ${filename}" ; 
    ./bin/solve_dIdX_RK4 ./data/data_ding.json ${filename} ./results/results.csv ; 
    done

# Analyse the results
# -batch combines -nodesktop, -nodisplay, and -nosplash
# https://stackoverflow.com/questions/65200329/matlab-r2020a-run-m-script-from-terminal
echo "Plotting results with MATLAB..."
matlab -sd ./src -batch analyse_results
echo "Done !"


## referencing ?              - small stuff -> no need
## keeping old MATLAB stuff ? - Nope, use gitignore for that
## logging ?                  - Yeah might be useful so we know programm isn't dead
## size of the project ?      - If proposal is fullfilled, not to worry too much