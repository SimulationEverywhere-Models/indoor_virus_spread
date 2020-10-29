# Basic model

## Parameters
This is the list of cell paramters that you can change from the JSON file

| Parameter            | Usage                                                                                                        |
|----------------------|--------------------------------------------------------------------------------------------------------------|
| vent                 | set to **true** for vents on and **false** for vents off                                                              |
| breathing_production | number of particles generated from breathing                                                                 |
| speaking_production  | number of particles generated from speaking                                                                  |
| coughing_production  | number of particles generated from coughing                                                                  |
| breathing_rate       | rate at which the occupant breathes(in seconds)                                                              |
| speaking_rate        | rate at which the occupant speaks(in seconds)                                                                |
| coughing_rate        | rate at which the occupant coughs(in seconds)                                                                |
| flow_weight          | the percentage of particles that travel  in the direction of the airflow                                     |
| infection_threshold  | the number of particles it takes to infect an occupant                                                       |
| mask_type            | the type of mask the receiver or spreader is wearing. Types include: NO_MASK, COTTON, SURGICAL, N95, N95_FIT |
| cell_size            | the size of the cell(in centimeters)                                                                         |
| resp_time            | the default response time for a cell state to update(in seconds)                                             |
| start_time           | start time for occupants to enter building(in second)                                                        |

## Running the model



