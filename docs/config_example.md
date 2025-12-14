# Example data configuration file 

This is an example json configuration file for adding/editing reactor data to `./data/scenarios`.
```
{
    "configs" : 
    [ 
        {
            "time_start_s"           : <integer>, 
            "time_end_s"             : <integer>,
            "time_step_s"            : <integration time step, integer>,
            "neutron_flux_ratio"     : <ratio in [0, 1] of the nominal power>,
            "initial_iodine_per_cm3" : {
                "equilibrium" : <bool, wether to start from equilibrium at the given flux ratio>,
                "value"       : <double, if "equilibrium" is false>
            },
            "initial_xenon_per_cm3"  : {
                "equilibrium" : <bool>,
                "value"       : null
            }
        },{
            "time_start_s"           : <"time_end_s" of the previous segment>,
            "time_end_s"             : <int>,
            "time_step_s"            : <int>,
            "neutron_flux_ratio"     : <double>
        },{
            <etc.>   
        }
    ]
}
```