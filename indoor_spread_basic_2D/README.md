# Basic model

## Description
First iteration of the model, showing a uniform distribution of particles.

## Parameters
This is the list of cell paramters that you can change from the JSON file

| Parameter            | Usage                                                                                                        |
|----------------------|--------------------------------------------------------------------------------------------------------------|
| vp_production        | Number of particles generated every time the occupant breathes                                               |
| breathing_rate       | Rate at which the occupant breathes(in seconds)                                                              |
| cell_size            | The size of the cell(in centimeters)                                                                         |
| resp_time            | The default response time for a cell state to update(in seconds)                                             |
| start_time           | Start time for occupants to enter building(in second)                                                        |
| infection_threshold  | number of particles needed to be inhaled before becoming infected                                            |

## Running the model
You will need cmake to compile the program,

Linux installation:
- In your command prompt do `sudo apt install cmake`

Windows installation:
- Go to https://cmake.org/download/ and install the **Windows win64-x64 Installer**
- Run the installer and follow the instructions

Compiling and running the model:
- Navigate to **CADMIUM/Cell-DEVS-Cadmium-Simulation-Environment/Cadmium-Cell-DEVS-Models/indoor_virus_spread/indoor_spread_basic_2D** where CMakeLists is located and in your terminal run `cmake ./`
- A new Makefile will be created in your directory, in the terminal again run `make`
- A new executable file will be created in the same folder, it's caled vp (Linux) / vp.exe (Windows)
- To run a simulation, in your terminal run 
  - (Linux) `./vp ./config/<the scenario name>.json <number of timesteps to run simulation>`
    - i.e., `./vp ./config/restaurant_2d.json 500`
  - (Windows) `./vp.exe ./config/<the scenario name>.json <number of timesteps to run simulation>`
    - i.e., `./vp.exe ./config/restaurant_2d.json 500`
- **NOTE:** The results will be generated in the results folder. Each time you run a new simulation the previous results will get overriden, so save them in another folder if you need them for future usage.

