# Quanta Model

## Description
Simulates particle distribution in an indoor environment using quanta as a unit of measurement.

## Parameters
This is the list of cell paramters that you can change from the JSON file

| Parameter            | Usage                                                                                                        |
|----------------------|--------------------------------------------------------------------------------------------------------------|
| quanta_params        |                                                                                                              |
| ERq_resting          | The emission rate at rest (quanta/h)                                                                         |
| ERq_speaking         | The emission rate while speaking (quanta/h)                                                                  | 
| IR_resting           | The inhalation rate for an occupant at rest (1 breath every `IR_resting` seconds)                            |
| IR_speaking          | The inhalation rate for an occupant while speaking (1 breath every `IR_speaking` seconds)                    | 
| volume               | The volume of the room (cubic meters)                                                                        |
| n0                   | Initial concentration of infectious particles in the room                                                    |
| IVVR                 | Infectious viral removal rate (Air exchange rate + particle deposition + viral inactivation)                 |
| scenario params      |                                                                                                              |
| vent                 | Set to **true** for vents on and **false** for vents off                                                     |
| infected_occupants   | Array containing positions for infected occupants with their respective mask type
|                      | JSON format: `[[[<x_position, y_position>], <mask_type>], [[<x_position, y_position>], <mask_type>], ...]]`  
|                      | i.e., `[[[17,17], "N95"], [[17,46], "COTTON"], ...]]`                                                        |
| healthy_occupants    | Array containing positions for healthy occupants with their respective mask type
|                      | JSON format: `[[[<x_position, y_position>], <mask_type>], [[<x_position, y_position>], <mask_type>], ...]]`  
|                      | i.e., `[[[5,24], "SURGICAL"], [[17,46], "NO_MASK"], ...]]`                                                   |
| flow_weight          | The percentage of particles that travel  in the direction of the airflow                                     |
| infection_threshold  | The number of particles it takes before an occupant is at risk of being infected                             |
| mask_type            | The type of mask the receiver or spreader is wearing by default. Types include: NO_MASK, COTTON, SURGICAL, N95, N95_FIT |
| cell_size            | The size of the cell(in centimeters)                                                                         |
| resp_time            | The default response time for a cell state to update(in seconds)                                             |
| start_time           | Start time for occupants to enter building(in second)                                                        |
| masks                | Different mask types with their shedding rate(percentage of particles exhaled) and efficiency(percentage of particles that are blocked from being inhaled) |

## Running the model
You will need cmake to compile the program,

Linux installation:
- In your command prompt do `sudo apt install cmake`

Windows installation:
- Go to https://cmake.org/download/ and install the **Windows win64-x64 Installer**
- Run the installer and follow the instructions

Compiling and running the model:
- before compiling, in **CADMIUM/Cell-DEVS-Cadmium-Simulation-Environment/Cadmium-Cell-DEVS-Models/indoor_virus_spread/indoor_quanta_2D** create a new folder called results, this is where all the simulation results will be stored
- Navigate to **CADMIUM/Cell-DEVS-Cadmium-Simulation-Environment/Cadmium-Cell-DEVS-Models/indoor_virus_spread/indoor_quanta_2D** where CMakeLists is located and in your terminal run `cmake ./`
- A new Makefile will be created in your directory, in the terminal again run `make`
- A new executable file will be created in the same folder, it's caled vp (Linux) / vp.exe (Windows)
- To run a simulation, in your terminal run 
  - (Linux) `./vp ./config/<the scenario name>.json <number of timesteps to run simulation>`
    - i.e., `./vp ./config/restaurant_quanta1.json 500`
  - (Windows) `./vp.exe ./config/<the scenario name>.json <number of timesteps to run simulation>`
<<<<<<< HEAD:indoor_quanta_2D/README.md
    - i.e., `./vp.exe ./config/restaurant_quanta1.json 500`
- **NOTE:** The results will be generated in the results folder. Each time you run a new simulation the previous results will get overriden, so save them in another folder if you need them for future usage.
