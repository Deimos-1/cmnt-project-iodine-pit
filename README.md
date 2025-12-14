# Xenon pit duration

## Project Description
This project aims to determine the duration of the Xenon pit in pressurized water reactors. It solves the system of ordinal differential equations governing the evolution of the xenon concentration into csv files. The results are then compiled into graphs with MATLAB to visualize the results. 

### Input files
Input files are located in `./data`, which consists of JSON configuration files that can easily be modified :
 * `data_ding.json`: Parameters related to the reactor core and physical constants: 
    - Halflives $[\text{hr}]$
    - Fission yields $[-]$
    - Microscopic absorption cross-sections $[\text{cm}^2]$.
    - Macroscopic cross-section of the core $[\text{cm}^2]$. 
    - Nominal value of the neutron flux $[\frac{n}{\text{cm}^2 \cdot \text{s}}]$.
 * `scenarios`: Contains scenarios to be computed. Every .json file in the folder is simulated when executing `./run.sh`. Each scenario consists of segments characterized by : 
    - start/end times $[\text{s}]$
    - integration step $[\text{s}]$
    - ratio of the nominal neutron flux for the segment $[-]$.
    - initial concentrations for Iodine and Xenon $[\frac{\text{n}}{\text{cm}^3}]$

#### Customization of the input files 
Example scenarios and data files are given in `./docs` : 
* `config_example.md` : Example scenario (boundary conditions, solver configuration)
* `data_example.md` : Example reactor data.   

### Output files 
Output files are located in `./results` and consists of: 
 * **.csv files** : Contains the time and concentrations for both Iodine and Xenon, from the numerical integration.
 * **.jpg images** : Graphs to visualize the results, generated with MATLAB.

### Report
Can be found with the necessary documentation under `./docs` as `report.pdf`. 

## Running the program

### Dependencies
Here are the programs used to run this project. 

| Program       | Version           | Remarks                                             |
| ------------- | ----------------- | --------------------------------------------------- |
| OS            | Any               |                                                     |
| GCC           | 15.2.0            | Rev8, Built by MSYS2 project                        |
| cJSON library | > 1.7.19          | Latest version automatically downloaded by build.sh |
| GNU bash      | 5.2.37(2)-release | Or any to run shell scripts                         |
| MATLAB        | R2025b            | At least R2022b for `dictionary()`                  |

This code was only tested on Windows due to the lack of Linux VM with the upper requirements.


### Build
The C programs are compiled automatically by a shell script `build.sh` using GCC. 
From the repository folder, it can be run as: `bash build.sh`.
The resulting binaries go in `./bin` and the necessary packages are located in `./include`. 


### Execute
After building the binaries and fetching the cJSON library, the program can be run with `run.sh`.


## Contributors
* Stefan Mignerey

## Acknowledgments

### Data sources
Data used in the .json configurations was mainly taken from :    
* _Solving Bateman Equation for Xenon Transient Analysis Using Numerical Methods, 
Zechuan  Ding, MATEC Web Conf. 186 01004 (2018), DOI: 10.1051/matecconf/201818601004_

**Except** for the neutron flux where a lower number of 4.42e13, based on the litterature cited in the report, was taken. This change is discussed in the report in more details. 


### Code
Credits to GaveGamble for the cJSON library ([github.com/DaveGamble/cJSON](https://github.com/DaveGamble/cJSON))
