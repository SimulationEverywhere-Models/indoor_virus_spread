# Advanced Model

## Description


## Parameters
This is the list of cell paramters that you can change from the JSON file

| Parameter            | Usage                                                                                                        |
|----------------------|--------------------------------------------------------------------------------------------------------------|
| vent                 | Set to **true** for vents on and **false** for vents off                                                     |
| infected_occupants   | Array containing positions for infected occupants with their respective mask type
|                      | JSON format: `[[[<x_position, y_position>], <mask_type>], [[<x_position, y_position>], <mask_type>], ...]]`  
|                      | i.e., `[[[17,17], "N95"], [[17,46], "COTTON"], ...]]`                                                        |
| healthy_occupants    | Array containing positions for healthy occupants with their respective mask type
|                      | JSON format: `[[[<x_position, y_position>], <mask_type>], [[<x_position, y_position>], <mask_type>], ...]]`  
|                      | i.e., `[[[5,24], "SURGICAL"], [[17,46], "NO_MASK"], ...]]`                                                   |
| breathing_production | Number of particles generated from breathing                                                                 |
| speaking_production  | Number of particles generated from speaking                                                                  |
| coughing_production  | Number of particles generated from coughing                                                                  |
| breathing_rate       | Rate at which the occupant breathes(in seconds)                                                              |
| speaking_rate        | Rate at which the occupant speaks(in seconds)                                                                |
| coughing_rate        | Rate at which the occupant coughs(in seconds)                                                                |
| flow_weight          | The percentage of particles that travel  in the direction of the airflow                                     |
| infection_threshold  | The number of particles it takes to infect an occupant                                                       |
| mask_type            | The type of mask the receiver or spreader is wearing by default. Types include: NO_MASK, COTTON, SURGICAL, N95, N95_FIT |
| cell_size            | The size of the cell(in centimeters)                                                                         |
| resp_time            | The default response time for a cell state to update(in seconds)                                             |
| start_time           | Start time for occupants to enter building(in second)                                                        |
| masks           | Different mask types with their shedding rate(percentage of particles exhaled) and efficiency(percentage of particles that are blocked from being inhaled)                                                       |

## Running the model
You will need cmake to compile the program,

Linux installation:
- In your command prompt do `sudo apt install cmake`

Windows installation:
- Go to https://cmake.org/download/ and install the **Windows win64-x64 Installer**
- Run the installer and follow the instructions

Compiling and running the model:
- before compiling, in **CADMIUM/Cell-DEVS-Cadmium-Simulation-Environment/Cadmium-Cell-DEVS-Models/indoor_virus_spread/indoor_spread_2D** create a new folder called results, this is where all the simulation results will be stored
- Navigate to **CADMIUM/Cell-DEVS-Cadmium-Simulation-Environment/Cadmium-Cell-DEVS-Models/indoor_virus_spread/indoor_spread_2D** where CMakeLists is located and in your terminal run `cmake ./`
- A new Makefile will be created in your directory, in the terminal again run `make`
- A new executable file will be created in the same folder, it's caled vp (Linux) / vp.exe (Windows)
- To run a simulation, in your terminal run 
  - (Linux) `./vp ./config/<the scenario name>.json <number of timesteps to run simulation>`
    - i.e., `./vp ./config/Restaurant-scenario1.json 500`
  - (Windows) `./vp.exe ./config/<the scenario name>.json <number of timesteps to run simulation>`
    - i.e., `./vp.exe ./config/Restaurant-scenario1.json 500`
- The results will be generated in the results folder. Each time you run a new simulation the previous results will get overriden, so save them in another folder if you need them.

## Roadmap
Make a connection between RNA/Viral titer and Quanta and implement relation in model
- [RNA/Viral titer with masks](https://msphere.asm.org/content/5/5/e00637-20)
- [Definition of quanta and relation with RNA/Viral titer](https://www.medrxiv.org/content/10.1101/2020.06.01.20118984v1)

