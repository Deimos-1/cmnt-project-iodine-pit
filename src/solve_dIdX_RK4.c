
/* Solve the ODE system on [t0, tf). 
   Writes the final (t, I, X) values to a CSV. */

// latest version of cJSON is downloaded when building the project
#include "../include/cJSON.h"
#include <stdio.h>  // working with files
#include <stdlib.h> // for malloc()
#include <string.h> // for memset()
#include <time.h>   // for logging
#include <math.h>   


#define SECONDS_PER_HOUR 3600.0


// Structure for reactor parameters
struct Parameters
{
    double halfLife_X_hr;
    double halfLife_I_hr;
    double decayConst_I;
    double decayConst_X; 
    double yield_I;
    double yield_X; 
    double absSection_X;
    double fissionSection;
    double nominalNeutronFlux;
};

// Structure for solver's parameters
struct SolverConfig
{
    double t0;
    double tf; 
    double dt; 
    double fluxPrctg; 
    double I0;
    double X0;
}; 


// DEFINE FUNCTIONS 
// ================ //

// Gets the decay constant [/s] from the halflife [hr]
double decayConst (double halfLife_hr) 
{
    return log(2)/(halfLife_hr * SECONDS_PER_HOUR); // given in [/s]
}
  
// 1st order ODE for the Xenon concentration
double dX (double neutronFlux, double I, double X, struct Parameters * P) 
{
    return P->yield_X * P->fissionSection * neutronFlux + P->decayConst_I * I 
    - P->decayConst_X * X - P->absSection_X * X * neutronFlux ;
}

// 1st order ODE for the Iodine concentration
double dI (double neutronFlux, double I, struct Parameters * P) 
{
    return P->yield_I * P-> fissionSection * neutronFlux - P->decayConst_I * I;
}

double doubleFromParentJSON (cJSON * parent, char * key)
{
    cJSON * item = cJSON_GetObjectItemCaseSensitive(parent, key);
    if (!cJSON_IsNumber(item)) {
        printf("> [ERROR] Missing JSON field '%s' from '%s'\n", key, 
            cJSON_GetStringValue(parent));
        exit(EXIT_FAILURE); /* break the program entirely as we can no longer 
                               solve */
    }
    return item->valuedouble;
}

double intFromParentJSON (cJSON * parent, char * key)
{
    cJSON * item = cJSON_GetObjectItemCaseSensitive(parent, key);
    if (!cJSON_IsNumber(item)) {
        printf("> [ERROR] Missing JSON field '%s' from '%s'\n", key, 
            cJSON_GetStringValue(parent));
        exit(EXIT_FAILURE); /* break the program entirely as we can no longer 
                               solve */
    }
    return item->valueint;
}


// RK4 solver to a .csv
// solving on [t0, tf[ (tf not included)
double * solve2csv (FILE * fptr, 
    double (*dI)(double, double, struct Parameters *), 
    double (*dX)(double, double, double, struct Parameters *), 
    double I0, double X0, double neutronFlux, double t0, double tf, double dt, 
    struct Parameters * P) 
{
    // few checks on the times
    if ((tf < t0) || (dt > (tf - t0))){
        printf("> [ERROR]: Inconsistant timings."); return NULL;
    }

    double I=I0; double X=X0; // init

    // we solve on [t0, tf[ ! 
    for (int i=0; i < (tf-t0)/dt; i++) {
        // write current iterate
        fprintf(fptr, "%f; %f; %f\n", t0 + i*dt, I, X);

        // then compute the next iterate
        // starting with Iodine : 
        double k1 = dI(neutronFlux, I, P) ;           
        double I1 = I + k1*dt/2;     
        double k2 = dI(neutronFlux, I1, P);                
        double I2 = I + k2*dt/2;     
        double k3 = dI(neutronFlux, I2, P); 
        double I3 = I + k3*dt/2;
        double k4 = dI(neutronFlux, I3, P);
        
        double Inext = I + (k1 + 2*k2 + 2*k3 + k4)*dt/6;

        // same for Xenon: 
        k1 = dX(neutronFlux, I, X, P);
        double X1 = X + k1*dt/2;    
        k2 = dX(neutronFlux, I1, X1, P);                
        double X2 = X + k2*dt/2;     
        k3 = dX(neutronFlux, I2, X2, P); 
        double X3 = X + k3*dt/2;
        k4 = dX(neutronFlux, I3, X3, P);
        
        double Xnext = X + (k1 + 2*k2 + 2*k3 + k4)*dt/6;
        I = Inext; X = Xnext;
    }
    // at the last iteration we return the final values of 
    // X and I for the next segment of integration
    double * latestConcArr = (double *)malloc(2*sizeof(double));
    latestConcArr[0] = I; latestConcArr[1] = X;
    return latestConcArr;
}

// converts file path string to the file name string 
char * getNameFromFile (char * string) 
{
    // look for the last occurence of '/'
    char * slashPtr = strrchr(string, '/') ;
    if (!slashPtr) {
        // look for '\' instead, just in case. 
        slashPtr = strrchr(string, '\\') ; 
        if (!slashPtr) {
            printf("> [ERROR]: Invalid string: %s\n", string); return NULL; 
        };
    }
    // look for the location of the last '.'
    char * pointPtr = strrchr(string, '.') ;
    if (!pointPtr) {
        printf("> [ERROR]: Invalid string: %s\n", string); return NULL; 
    };
    // get indices for start/stop of the name
    int startIndex = slashPtr - string + 1; 
    int endIndex = pointPtr - string;
    // allocate memory for the name (+1 for null-termination)
    char * filename = malloc((endIndex - startIndex + 1)*sizeof(char));  
    // populate the string array
    strncpy(filename, string + startIndex, endIndex - startIndex) ;     
    filename[endIndex - startIndex] = '\0';
    return filename;
}






int main (int varc, char * argv[]) 
{
    printf("\nLogs for %s", asctime(localtime(&(time_t){time(NULL)})));

    // Get the parameters from the json file
    // ===================================== //

    // the filename is the second argument after the binary's path
    if (varc < 2) {
        printf("> [ERROR]: Missing argument: filename\n"); return 1;
    }
    char * filename = argv[1]; 

    FILE * fptr = fopen(filename, "r") ;
    if (fptr == NULL) {
        printf("> [ERROR]: Invalid file: %s\n", filename);
        return 1;
    }

    // copy contents into a string 
    char buffer[2048]; // should be large enough 
    int length = fread(buffer, 1, sizeof(buffer), fptr);
    buffer[length] = '\0';
    fclose(fptr);

    
    struct Parameters P;
    // Read parameters with cJSON 
    cJSON * tree = cJSON_Parse(buffer) ;
    if (!tree) { printf("> [ERROR]: JSON parse error\n"); return 1; }

    // Halflife of I-135 and Xe-135 [hr]
    cJSON *tempCat = cJSON_GetObjectItemCaseSensitive(tree, "halfLife_hr");
    if (!tempCat) { printf("> [ERROR]: Missing halfLife_hr\n"); return 1; }
    P.halfLife_X_hr = doubleFromParentJSON(tempCat, "Xe135");
    P.decayConst_X = decayConst(P.halfLife_X_hr);
    P.halfLife_I_hr = doubleFromParentJSON(tempCat, "I135");
    P.decayConst_I = decayConst(P.halfLife_I_hr);

    // fission yield of I-135 and Xe-135
    tempCat = cJSON_GetObjectItemCaseSensitive(tree, "fissionYield");
    if (!tempCat) {printf("> [ERROR]: Missing fissionYield\n"); return 1;}
    P.yield_X = doubleFromParentJSON(tempCat, "Xe135");
    P.yield_I = doubleFromParentJSON(tempCat, "I135");

    // Microscopic absorption cross-section for Xe-135 
    tempCat = cJSON_GetObjectItemCaseSensitive(tree, "absorptionSection");
    if (!tempCat) {printf("> [ERROR]: Missing absorptionSection\n"); return 1;}
    P.absSection_X = doubleFromParentJSON(tempCat, "Xe135");

    // Macroscopic fission cross-section of the reactor
    P.fissionSection = doubleFromParentJSON(tree, "macroFissionSection");

    // Neutron flux at 100% of rated power
    P.nominalNeutronFlux = doubleFromParentJSON(tree, "nominalNeutronFlux");
    

    // LOAD SCENARIO 
    // ============= //
    // Each scenario is loaded as an array of timespans. Each timespan is a 
    // SolverConfig struct containing the necessary information on how to 
    // integrate the functions, it also contains the initial conditions for 
    // integration. 

    // load the config file and get the number of elements 
    fptr = fopen(argv[2], "r") ;
    if (fptr == NULL) {
        printf("> [ERROR]: Invalid file: %s \n", argv[2]);
        return 1;
    }

    // copy contents into a string 
    // clear the buffer for this new file content
    memset(buffer, 0, sizeof(buffer));  
    length = fread(buffer, 1, sizeof(buffer), fptr);
    buffer[length] = '\0';
    fclose(fptr);

    tree = cJSON_Parse(buffer) ;
    if (!tree) { printf("> [ERROR]: JSON parse error\n"); return 1; }

    tempCat = cJSON_GetObjectItemCaseSensitive(tree, "configs");
    if (!tempCat) { printf("> [ERROR]: Missing configs\n"); return 1; }

    int nConfigs = cJSON_GetArraySize(tempCat);
    struct SolverConfig C[nConfigs];

    // Build each integration span C[i]
    for (int i = 0; i < nConfigs; i++) {
        cJSON * tempSubCat = cJSON_GetArrayItem(tempCat, i);
        if (!tempSubCat) {printf("> [ERROR]: Empty configuration\n");return 1;}

        C[i].t0 = intFromParentJSON(tempSubCat, "time_start_s");
        C[i].tf = intFromParentJSON(tempSubCat, "time_end_s");
        C[i].dt = intFromParentJSON(tempSubCat, "time_step_s");
        C[i].fluxPrctg = doubleFromParentJSON(tempSubCat, "neutron_flux_ratio");

        if (i == 0) { 
            // it's not ideal to check at each iteration but nConfigs is small

            // Iodine
            cJSON * tempItem = cJSON_GetObjectItemCaseSensitive(
                                    tempSubCat, "initial_iodine_per_cm3"
                                );
            if (!tempItem) {printf("> [ERROR]: Missing initial_iodine_per_cm3\n"); return 1; }
            cJSON * tempSubItem = cJSON_GetObjectItemCaseSensitive(tempItem, "equilibrium");
            if (!tempSubItem) { printf("> [ERROR]: Missing field 'equilibrium'\n"); return 1; }
            else if (tempSubItem->valueint == 1) {
                // Equilibrium value from solving dI = 0 
                C[i].I0 = P.yield_I * P.fissionSection * C[i].fluxPrctg
                            * P.nominalNeutronFlux / P.decayConst_I ;  
            }
            else { // not starting from equilibrium concentration
                tempSubItem = cJSON_GetObjectItemCaseSensitive(tempItem, "value");
                if (!tempSubItem) { printf("> [ERROR]: Missing field 'equilibrium'\n"); return 1; }
                C[i].I0 = tempSubItem->valuedouble;
            }

            // Xenon
            tempItem = cJSON_GetObjectItemCaseSensitive(tempSubCat, "initial_xenon_per_cm3");
            if (!tempItem) { printf("> [ERROR]: Missing initial_xenon_per_cm3\n"); return 1; }
            tempSubItem = cJSON_GetObjectItemCaseSensitive(tempItem, "equilibrium");
            if (!tempSubItem) { printf("> [ERROR]: Missing field 'equilibrium'\n"); return 1; }
            else if (tempSubItem->valueint == 1) {
                // Equilibrium value from solving dX = 0
                C[i].X0 = ( P.yield_X * P.fissionSection * C[i].fluxPrctg 
                            * P.nominalNeutronFlux + P.decayConst_I * C[i].I0 ) 
                        / ( P.decayConst_X + P.absSection_X 
                            * C[i].fluxPrctg * P.nominalNeutronFlux );  
            }
            else { // not starting from equilibrium concentration
                tempSubItem = cJSON_GetObjectItemCaseSensitive(tempItem, "value");
                if (!tempSubItem) { printf("> [ERROR]: Missing field 'equilibrium'\n"); return 1; }
                C[i].X0 = tempSubItem->valuedouble;
            }
        }
    }

    // Inactivate temporary JSON object pointers, 
    // prevents from calling them later
    cJSON_Delete(tree);  
    tree = NULL; tempCat = NULL; 
    // tempSub* variables can't be called anyway after deleting tree
    

    // Scenario name
    char * scenarioName = getNameFromFile(argv[2]);
    printf("> Scenario file name: %s\n", scenarioName);


    // SOLVING
    // ======= // 
    // Solve with RK4, print each step to a CSV
    char * outcsvPath = malloc(
        ( 6 + strlen(argv[3]) + strlen(scenarioName) ) * sizeof(char)
    ); // ^-- extra '/', '.csv' & '\0'
    sprintf(outcsvPath, "%s/%s.csv", argv[3], scenarioName);
    fptr = fopen(outcsvPath, "w");
    if (fptr == NULL) {
        printf("> [ERROR]: Can't write to file: %s\n", outcsvPath); return 1;
    }

    // Loop through C for the numerical integration
    for (int i = 0; i < nConfigs; i++) {
        if (i==0) {fprintf(fptr, "t; I; X\n") ;} // header 

        double * latestConcPtr = solve2csv(
            fptr, &dI, &dX, C[i].I0, C[i].X0, 
            P.nominalNeutronFlux*C[i].fluxPrctg, 
            C[i].t0, C[i].tf, C[i].dt, &P
        );
        if (latestConcPtr == NULL) { 
            printf("> [ERROR]: The integration of segment %i failed\n", i); 
            return 1; 
        };

        // use the final values as initial conditions on the next integration
        if (i < nConfigs - 1) {
            C[i+1].I0 = latestConcPtr[0]; 
            C[i+1].X0 = latestConcPtr[1];
        }
        free(latestConcPtr); 
    }
    fclose(fptr); free(outcsvPath); free(scenarioName);
    printf("> Scenario ran successfully.\n"); return 0;
    // then read the CSV with MATLAB to analyse results
}