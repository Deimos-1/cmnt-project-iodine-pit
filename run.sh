set -e                  # exits the script if an error is encountered
cd "$(dirname "$0")"    # cd to the directory of the script to access other files by relative path

# Execute the binaries for each scenario
mkdir -p results
for filename in ./data/scenarios/*.json; do
    echo "> Simulating: ${filename}" ; 
    ./bin/solve_dIdX_RK4 ./data/data_ding.json ${filename} ./results >> ./results/logs.txt; 
    done

# Analyse the results
# -batch combines -nodesktop, -nodisplay, and -nosplash
echo "> Plotting results with MATLAB... (~30s)"
matlab -sd ./src -batch analyse_results
echo "> Done !"