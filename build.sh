set -e                  # exits the script if an error is encountered
cd "$(dirname "$0")"    # cd to the directory of the script to access other files by relative path

## fetch the latest version of cJSON
echo "> Fetching the latest version of cJSON on GitHub..."
mkdir -p include # -p flag: only if it doesnt exist
curl -o include/cJSON.c "https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.c"
curl -o include/cJSON.h "https://raw.githubusercontent.com/DaveGamble/cJSON/refs/heads/master/cJSON.h"

## Compile C files
echo "> Compiling the binaries..." 
mkdir -p bin
gcc -Wall ./src/solve_dIdX_RK4.c ./include/cJSON.c -o ./bin/solve_dIdX_RK4  -lm
echo "> Done !"