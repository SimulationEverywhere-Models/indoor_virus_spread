# Advanced Model

## Description


## Parameters
This is the list of cell paramters that you can change from the JSON file

| Parameter            | Usage                                                                                                        |
|----------------------|--------------------------------------------------------------------------------------------------------------|
| vent                 | Set to **true** for vents on and **false** for vents off                                                     |
| infected_occupants   | Array containing positions for infected occupants.  
|                      | JSON format: `[[[<x_position, y_position>], <mask_type>], [[<x_position, y_position>], <mask_type>], ...]]`  
|                      | i.e., `[[[17,17], "N95"], [[17,46], "COTTON"], ...]]`                                                        |
| healthy_occupants    | Array containing positions for healthy occupants.  
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


## Roadmap
Make a connection between RNA/Viral titer and Quanta and implement relation in model
- [RNA/Viral titer with masks](https://msphere.asm.org/content/5/5/e00637-20)
- [Definition of quanta and relation with RNA/Viral titer](https://www.medrxiv.org/content/10.1101/2020.06.01.20118984v1)

