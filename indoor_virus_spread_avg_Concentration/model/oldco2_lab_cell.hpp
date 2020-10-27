/**
 * Copyright (c) 2020, Cristina Ruiz Martin
 * ARSLab - Carleton University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/**
* Model developed by Hoda Khalil in Cell-DEVS CD++
* Implemented in Cadmium-cell-DEVS by Cristina Ruiz Martin
*/

#ifndef CADMIUM_CELLDEVS_CO2_CELL_HPP
#define CADMIUM_CELLDEVS_CO2_CELL_HPP

#include <cmath>
#include <nlohmann/json.hpp>
#include <cadmium/celldevs/cell/grid_cell.hpp>

using nlohmann::json;
using namespace cadmium::celldevs;
using namespace std;

/************************************/
/******COMPLEX STATE STRUCTURE*******/
/************************************/
enum CELL_TYPE {AIR=-100, CO2_SOURCE=-200, IMPERMEABLE_STRUCTURE=-300, DOOR=-400, WINDOW=-500, VENTILATION=-600, WORKSTATION=-700, EXHAUST=-800};
struct co2 {
    int counter;
    int concentration;
    CELL_TYPE type;
	int breathing_counter=5; //number of cycles since this cell became a CO2 source. Used to know when to add the concentration increase for co2 sources. The default is every five seconds

    co2() : counter(-1), concentration(500), type(AIR), breathing_counter(0) {}  // a default constructor is required
    co2(int i_counter, int i_concentration, CELL_TYPE i_type, int i_breathing_counter) : counter(i_counter), concentration(i_concentration), type(i_type), breathing_counter(i_breathing_counter){}
};
// Required for comparing states and detect any change
inline bool operator != (const co2 &x, const co2 &y) {
	//note that breathing_counter is not included here intentionally as it is onlu used to add the breathing factor
    return (x.counter != y.counter || x.concentration != y.concentration || x.type != y.type );
}
// Required if you want to use transport delay (priority queue has to sort messages somehow)
inline bool operator < (const co2& lhs, const co2& rhs){ return true; }

// Required for printing the state of the cell
std::ostream &operator << (std::ostream &os, const co2 &x) {
	//note that breathing_counter is not included here as it is not useful for visualization and it will only slow the simulation to include it. Keep it this way unless needed for another reason
    os << "<" << x.counter << "," << x.concentration << "," << x.type << ">";
    return os;
}

// Required for creating co2 objects from JSON file
void from_json(const json& j, co2 &s) {
    j.at("counter").get_to(s.counter);
    j.at("concentration").get_to(s.concentration);
    j.at("type").get_to(s.type);
    //j.at("breathing_counter").get_to(s.breathing_counter);
}

/************************************/
/******COMPLEX CONFIG STRUCTURE******/
/************************************/
struct conc {
    float conc_increase; //CO2 generated by one person
    int base; //CO2 base level
    int window_conc; //CO2 level at window
    int vent_conc; //CO2 level at vent
    int resp_time;
    int exhaust_conc;
	int breathing_rate;

    // Each cell is 25cm x 25cm x 25cm = 15.626 Liters of air each
      // CO2 sources have their concentration continually increased by default by 12.16 ppm every 5 seconds.
    conc(): conc_increase(10), base(500), resp_time(1), window_conc(400), vent_conc(400), exhaust_conc(-1050), breathing_rate(5) {}
    conc(float ci, int b, int wc, int vc, int r, int ec, int br): conc_increase(ci), base(b), resp_time(r), window_conc(wc), vent_conc(vc), exhaust_conc(ec), breathing_rate(br) {}
};
void from_json(const json& j, conc &c) {
    j.at("conc_increase").get_to(c.conc_increase);
    j.at("base").get_to(c.base);
    j.at("resp_time").get_to(c.resp_time);
    j.at("window_conc").get_to(c.window_conc);
    j.at("vent_conc").get_to(c.vent_conc);
    j.at("exhaust_conc").get_to(c.exhaust_conc);
    j.at("breathing_rate").get_to(c.breathing_rate);
}


template <typename T>
class co2_lab_cell : public grid_cell<T, co2> {
public:
    using grid_cell<T, co2, int>::simulation_clock;
    using grid_cell<T, co2, int>::state;
    using grid_cell<T, co2, int>::map;
    using grid_cell<T, co2, int>::neighbors;

    using config_type = conc;  // IMPORTANT FOR THE JSON   
    float  concentration_increase; //// CO2 sources have their concentration continually increased
    int base; //CO2 base level
    int resp_time; //Time used to calculate the concentration inscrease
    int window_conc; //CO2 level at window
    int vent_conc; //CO2 level at cent
    int exhaust_conc;
	int breathing_rate; ///the interval between two consecutive breaths in seconds.//set intially from the JSON

 
    co2_lab_cell() : grid_cell<T, co2, int>() {
    }

    co2_lab_cell(cell_position const &cell_id, cell_unordered<int> const &neighborhood, co2 initial_state,
        cell_map<co2, int> const &map_in, std::string const &delayer_id, conc config) :
            grid_cell<T, co2>(cell_id, neighborhood, initial_state, map_in, delayer_id) {
                concentration_increase = config.conc_increase;
                base = config.base;
                resp_time = config.resp_time;
                window_conc = config.window_conc;
                vent_conc = config.vent_conc;
                exhaust_conc = config.exhaust_conc;
				breathing_rate = config.breathing_rate;
    }

    co2 local_computation() const override {
        co2 new_state = state.current_state;
        switch(new_state.type){
            case IMPERMEABLE_STRUCTURE: 
                new_state.concentration = 0;
                break;
            case DOOR:    
                new_state.concentration = base;
                break;
            case WINDOW:
                new_state.concentration = window_conc;
                break;
            case VENTILATION:
                new_state.concentration = vent_conc;
                break;
            case EXHAUST:
              //  new_state.concentration = exhaust_conc;
                //break;
            case AIR:{
                int concentration = 0;
                int num_neighbors = 0;                
                for(auto neighbors: state.neighbors_state) {
//                    if( neighbors.second.concentration < 0){
//                        assert(false && "co2 concentration cannot be negative");
//                    }
                    if((neighbors.second.type != IMPERMEABLE_STRUCTURE) ){
                        concentration += neighbors.second.concentration;
                        num_neighbors +=1;
                    }
                }
                new_state.concentration = concentration/num_neighbors;
                break;             
            }
            case WORKSTATION:{
                int concentration = 0;
                int num_neighbors = 0;                
                for(auto neighbors: state.neighbors_state) {
//                    if( neighbors.second.concentration < 0){
//                        assert(false && "co2 concentration cannot be negative");
//                    }
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                        concentration += neighbors.second.concentration;
                        num_neighbors +=1;
                    }
                }
                new_state.concentration = concentration/num_neighbors;
                
                    
                if (state.current_state.counter <= 30) { //TODO parameterize
                    new_state.counter += 1;                   
                }

                if (state.current_state.counter == 30){
                    new_state.type = CO2_SOURCE; 
					new_state.breathing_counter = 0;
				}
                break;
            }
            case CO2_SOURCE:{
                int concentration = 0;
                int num_neighbors = 0;
                for(auto neighbors: state.neighbors_state) {
//                  if( neighbors.second.concentration < 0){
//                        assert(false 	& "co2 concentration cannot be negative");
//                    }
                      if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                        concentration += neighbors.second.concentration;
                        num_neighbors +=1;
                    }               
                }
                
                new_state.concentration = (concentration/num_neighbors);
				//The concentration increases every time an occupant breathes (the default breathing rate is every five seconds)
				if( (new_state.breathing_counter % breathing_rate) == 0){
					new_state.concentration += (concentration_increase);
				}
				new_state.breathing_counter++;
                new_state.counter += 1;
                if (state.current_state.counter == 250) { //TODO parameterize
                    new_state.type = WORKSTATION; //The student left. The place is free.
                }
                break;
            }
            default:{
                assert(false && "should never happen");
            }
        }
      
        return new_state;

    }

    
    // It returns the delay to communicate cell's new state.
    T output_delay(co2 const &cell_state) const override {
			return resp_time;
    }

};

#endif //CADMIUM_CELLDEVS_CO2_CELL_HPP













