// cJSON should be downloaded when building the project (latest version)
#include "../include/cJSON.h"
#include <stdio.h>
#include <math.h>


// Define dX_dt and dI_dt with the parameters
// ==========================================

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
};


double decayConst(double halfLife_hr) {
    return log(2)/(halfLife_hr*60*60); // given in [/s]
}
  

double dX(
    double neutronFlux, double I, double X, 
    struct Parameters * P
) {
    return P->yield_X * P->fissionSection * neutronFlux + P->decayConst_I * I 
    - P->decayConst_X * X - P->absSection_X * X * neutronFlux ;
}


double dI(
    double neutronFlux, double I,
    struct Parameters * P
) {
    return P->yield_I * P-> fissionSection * neutronFlux - P->decayConst_I * I ;
}


// RK4 solver to a .csv
// solving on [t0, tf[ (tf not included)
int solve2csv(
    FILE * fptr, 
    double (*dI)(double, double, struct Parameters *), 
    double (*dX)(double, double, double, struct Parameters *), 
    double I0, double X0, double neutronFlux,
    double t0, double tf, double dt, 
    struct Parameters * P
) {
    // few checks on the times
    if ((tf < t0) || (dt > (tf - t0))){printf("Inconsistant timings."); return 1;}

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
        // I did not know what values of I to pass to estimate dX, 
        // according to ChatGPT, it should be I, I1, etc.
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
    return 0;
}






int main(int varc, char * argv[]) {

    // Get the parameters from the json file
    // =====================================

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
    
    
    // inactivate temporary JSON object pointers, prevents from calling them later
    cJSON_Delete(tree);  
    tree = NULL; tempCat = NULL; tempItem = NULL;

    // Flux should be given as a time series ??
    
    // Solve with RK4, print each step to a CSV
    FILE * fptr = fopen(argv[2], "w");
    if (fptr == NULL) {printf("Can't write to file: %s", argv[2]); return 1;}

    double neutronFlux = 1e14;
    double I0 = P.yield_I * P.fissionSection * neutronFlux / P.decayConst_I ;
    if (I0 != 1753809341066586.75){printf("Incompatible result"); return 1;};
    double X0 = 178421563086872.62 ; // from MATLAB script
    solve2csv(fptr, &dI, &dX, I0, X0, neutronFlux*0, 0, 100*60*60, 10, &P);
    fclose(fptr);

    // It's better to write to a csv from the program than 
    // to use redirection, for performance

    // read the CSV with MATLAB to analyse results
    return 0;
}