# Example scenario configuration 

This is an example json configuration file for adding/editing data to `./data`.   
Don't forget to edit `run.sh` with the correct input data as argument to `./bin/solve_dIdX_RK4`
```
{
  "halfLife_hr": {
	"I135": <double>,
    "Xe135": <double>  
  },
  "fissionYield": {
	"I135": <double>,
    "Xe135": <double>
  },
  "absorptionSection": {
    "Xe135": <double>
  },
  "macroFissionSection" : <double>,
  "nominalNeutronFlux" : <double>
}
```