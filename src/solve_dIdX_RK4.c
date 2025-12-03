// latest version of cJSON is downloaded when building the project
#include "../include/cJSON.h"
#include <stdio.h>  // working with files
#include <stdlib.h> // for malloc()
#include <string.h> // for memset() 
#include <math.h>   


// DEFINE STRUCTURES
// ================= //

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
double decayConst(double halfLife_hr) {
    return log(2)/(halfLife_hr*60*60); // given in [/s]
}
  
// 1st order ODE for the Xenon concentration
double dX(
    double neutronFlux, double I, double X, 
    struct Parameters * P
) {
    return P->yield_X * P->fissionSection * neutronFlux + P->decayConst_I * I 
    - P->decayConst_X * X - P->absSection_X * X * neutronFlux ;
}

// 1st order ODE for the Iodine concentration
double dI(
    double neutronFlux, double I,
    struct Parameters * P
) {
    return P->yield_I * P-> fissionSection * neutronFlux - P->decayConst_I * I ;
}


// RK4 solver to a .csv
// solving on [t0, tf[ (tf not included)
double * solve2csv(
    FILE * fptr, 
    double (*dI)(double, double, struct Parameters *), 
    double (*dX)(double, double, double, struct Parameters *), 
    double I0, double X0, double neutronFlux,
    double t0, double tf, double dt, 
    struct Parameters * P
) {
    // few checks on the times
    if ((tf < t0) || (dt > (tf - t0))){printf("Inconsistant timings."); return NULL;}

    fprintf(fptr, "t; I; X\n") ; // header 
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
    // at the last iteration we return the final values of X and I for the next segment of integration
    double * latestConcArr = (double *)malloc(2*sizeof(double));
    latestConcArr[0] = I; latestConcArr[1] = X;
    return latestConcArr;
}






int main(int varc, char * argv[]) {

    // Get the parameters from the json file
    // ===================================== //

    // the filename is the second argument after the binary's path
    if (varc < 2) {printf("Missing argument: filename\n"); return 1;}
    char * filename = argv[1]; 

    FILE * fp = fopen(filename, "r") ;
    if (fp == NULL) {
        printf("Invalid file: %s\n", filename);
        return 1;
    }

    // copy contents into a string 
    char buffer[2048]; // should be large enough 
    int length = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    
    struct Parameters P;
    // Read parameters with cJSON: (used ChatGPT GPT-5 Mini to get them quickly)
    cJSON * tree = cJSON_Parse(buffer) ;
    if (!tree) { printf("JSON parse error\n"); return 1; }

    cJSON *tempCat = cJSON_GetObjectItemCaseSensitive(tree, "halfLife_hr");
    if (!tempCat) { printf("Missing halfLife_hr\n"); return 1; }

    cJSON *tempItem = cJSON_GetObjectItemCaseSensitive(tempCat, "Xe135");
    if (!cJSON_IsNumber(tempItem)) { printf("Missing Xe135 in halfLife_hr\n"); return 1; }
    P.halfLife_X_hr = tempItem->valuedouble;
    P.decayConst_X = decayConst(P.halfLife_X_hr);

    tempItem = cJSON_GetObjectItemCaseSensitive(tempCat, "I135");
    if (!cJSON_IsNumber(tempItem)) { printf("Missing I135 in halfLife_hr\n"); return 1; }
    P.halfLife_I_hr = tempItem->valuedouble;
    P.decayConst_I = decayConst(P.halfLife_I_hr);

    tempCat = cJSON_GetObjectItemCaseSensitive(tree, "fissionYield");
    if (!tempCat) { printf("Missing fissionYield\n"); return 1; }

    tempItem = cJSON_GetObjectItemCaseSensitive(tempCat, "Xe135");
    if (!cJSON_IsNumber(tempItem)) { printf("Missing Xe135 in fissionYield\n"); return 1; }
    P.yield_X = tempItem->valuedouble;

    tempItem = cJSON_GetObjectItemCaseSensitive(tempCat, "I135");
    if (!cJSON_IsNumber(tempItem)) { printf("Missing I135 in fissionYield\n"); return 1; }
    P.yield_I = tempItem->valuedouble;

    tempCat = cJSON_GetObjectItemCaseSensitive(tree, "absobtionSection");
    if (!tempCat) { printf("Missing absobtionSection\n"); return 1; }

    tempItem = cJSON_GetObjectItemCaseSensitive(tempCat, "Xe135");
    if (!cJSON_IsNumber(tempItem)) { printf("Missing Xe135 in absobtionSection\n"); return 1; }
    P.absSection_X = tempItem->valuedouble;

    tempItem = cJSON_GetObjectItemCaseSensitive(tree, "macroFissionSection");
    if (!cJSON_IsNumber(tempItem)) { printf("Missing macroFissionSection\n"); return 1; }
    P.fissionSection = tempItem->valuedouble;

    tempItem = cJSON_GetObjectItemCaseSensitive(tree, "nominalNeutronFlux");
    if (!cJSON_IsNumber(tempItem)) { printf("Missing nominalNeutronFlux\n"); return 1; }
    P.nominalNeutronFlux = tempItem->valuedouble;
    

    // LOAD SCENARIO 
    // ============= //
    // scenario is loaded as a vector of timespans. Each timespan is a SolverConfig struct
    // containing the necessary information on how to integrate the functions, it also 
    // contains the initial conditions for integration. 

    // load the config file and get the number of elements 
    // NEED TO PASS SCENARIO AS INPUT ARGUMENT LATER !!!
    fp = fopen("./data/scenarios/power_drop.json", "r") ;
    if (fp == NULL) {
        printf("Invalid file: ./data/scenarios/power_drop.json \n");
        return 1;
    }

    // copy contents into a string 
    memset(buffer, 0, sizeof(buffer)); // clear the buffer for this new file content 
    length = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    tree = cJSON_Parse(buffer) ;
    if (!tree) { printf("JSON parse error\n"); return 1; }

    tempCat = cJSON_GetObjectItemCaseSensitive(tree, "configs");
    if (!tempCat) { printf("Missing configs\n"); return 1; }

    int nConfigs = cJSON_GetArraySize(tempCat);
    struct SolverConfig C[nConfigs];

    for (int i = 0; i < nConfigs; i++) {
        cJSON * tempSubCat = cJSON_GetArrayItem(tempCat, i);
        if (!tempSubCat) { printf("Empty configuration\n"); return 1; }

        tempItem = cJSON_GetObjectItemCaseSensitive(tempSubCat, "time_start_s");
        if (!tempItem) { printf("Missing time_start_s\n"); return 1; }
        C[i].t0 = tempItem->valueint ;

        tempItem = cJSON_GetObjectItemCaseSensitive(tempSubCat, "time_end_s");
        if (!tempItem) { printf("Missing time_end_s\n"); return 1; }
        C[i].tf = tempItem->valueint ;

        tempItem = cJSON_GetObjectItemCaseSensitive(tempSubCat, "time_step_s");
        if (!tempItem) { printf("Missing time_step_s\n"); return 1; }
        C[i].dt = tempItem->valueint ;

        tempItem = cJSON_GetObjectItemCaseSensitive(tempSubCat, "neutron_flux_ratio");
        if (!tempItem) { printf("Missing neutron_flux_ratio\n"); return 1; }
        C[i].fluxPrctg = tempItem->valueint ;

        if (i == 0) { // it's not ideal to check at each iteration but nConfigs is small
            tempItem = cJSON_GetObjectItemCaseSensitive(tempSubCat, "initial_iodine_per_cm3");
            if (!tempItem) { printf("Missing initial_iodine_per_cm3\n"); return 1; }
            cJSON * tempSubItem = cJSON_GetObjectItemCaseSensitive(tempItem, "equilibrium");
            if (!tempSubItem) { printf("Missing field 'equilibrium'\n"); return 1; }
            else if (tempSubItem->valueint == 1) {
                // analytically solved dI = 0 (equilibrium)
                C[i].I0 = P.yield_I * P.fissionSection * P.nominalNeutronFlux / P.decayConst_I ;  
            }
            else { 
                tempSubItem = cJSON_GetObjectItemCaseSensitive(tempItem, "value");
                if (!tempSubItem) { printf("Missing field 'equilibrium'\n"); return 1; }
                C[i].I0 = tempSubItem->valuedouble;
            }

            tempItem = cJSON_GetObjectItemCaseSensitive(tempSubCat, "initial_xenon_per_cm3");
            if (!tempItem) { printf("Missing initial_xenon_per_cm3\n"); return 1; }
            tempSubItem = cJSON_GetObjectItemCaseSensitive(tempItem, "equilibrium");
            if (!tempSubItem) { printf("Missing field 'equilibrium'\n"); return 1; }
            else if (tempSubItem->valueint == 1) {
                // analytically solved dX = 0
                C[i].X0 = ( P.yield_X * P.fissionSection * C[i].fluxPrctg * P.nominalNeutronFlux + P.decayConst_I * C[i].I0 ) 
                        / ( P.decayConst_X + P.absSection_X * C[i].fluxPrctg * P.nominalNeutronFlux );  
            }
            else { 
                tempSubItem = cJSON_GetObjectItemCaseSensitive(tempItem, "value");
                if (!tempSubItem) { printf("Missing field 'equilibrium'\n"); return 1; }
                C[i].I0 = tempSubItem->valuedouble;
            }
        }
    }

    // inactivate temporary JSON object pointers, prevents from calling them later
    cJSON_Delete(tree);  
    tree = NULL; tempCat = NULL; tempItem = NULL;
    
    // Solve with RK4, print each step to a CSV
    FILE * fptr = fopen(argv[2], "w");
    if (fptr == NULL) {printf("Can't write to file: %s", argv[2]); return 1;}

    // OK | if (C[0].I0 != 1753809341066586.75){printf("Incompatible result"); return 1;};
    // OK | if (C[0].X0 != 178421563086872.593750){printf("X0 doesn't match with equilibrium \n| Expected: 178421563086872.62\n| Got: %f", C[0].X0); return 1;};

    for (int i = 0; i < nConfigs; i++) {
        double * latestConcPtr = solve2csv(fptr, &dI, &dX, C[i].I0, C[i].X0, P.nominalNeutronFlux*C[i].fluxPrctg, C[i].t0, C[i].tf, C[i].dt, &P);
        if (latestConcPtr == NULL) { printf("The integration of segment %i failed\n", i); return 1; };

        // use the final values as initial conditions on the next integration
        if (i < nConfigs - 1) {
            C[i+1].I0 = latestConcPtr[0]; 
            C[i+1].X0 = latestConcPtr[1];
        }
    }
    fclose(fptr);

    // It's better to write to a csv from the program than 
    // to use redirection, for performance

    // then read the CSV with MATLAB to analyse results
    return 0;
}