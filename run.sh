set -e                  # exits the script if an error is encountered
cd "$(dirname "$0")"    # cd to the directory of the script to access other files by relative path

# Execute the binaries for each scenario
for filename in ./data/scenarios/*.json; do
    echo "> Simulating: ${filename}" ; 
    ./bin/solve_dIdX_RK4 ./data/data_ding.json ${filename} ./results >> ./results/logs.txt; 
    done

# Analyse the results
# -batch combines -nodesktop, -nodisplay, and -nosplash
echo "> Plotting results with MATLAB... (~30s)"
matlab -sd ./src -batch analyse_results
echo "> Done !"


## referencing ?              - small stuff -> no need
## keeping old MATLAB stuff ? - Nope, use gitignore for that
## logging ?                  - Yeah might be useful so we know programm isn't dead
## size of the project ?      - If proposal is fullfilled, not to worry too much