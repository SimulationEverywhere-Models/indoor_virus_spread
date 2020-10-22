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

#ifndef CADMIUM_CELLDEVS_vp_CELL_HPP
#define CADMIUM_CELLDEVS_vp_CELL_HPP

#include <cmath>
#include <nlohmann/json.hpp>
#include <cadmium/celldevs/cell/grid_cell.hpp>

using nlohmann::json;
using namespace cadmium::celldevs;
using namespace std;

/************************************/
/******COMPLEX STATE STRUCTURE*******/
/************************************/
enum CELL_TYPE {AIR=-100, vp_SOURCE=-200, IMPERMEABLE_STRUCTURE=-300, DOOR=-400, TABLE=-500, VENTILATION=-600, CHAIR=-700, vp_RECEIVER=-800};

struct vp_cell {
    int counter;
    CELL_TYPE prev_type;
    CELL_TYPE type;
    bool infected;
    int breathing_counter;
    int prev_num_particles;
    int num_particles;
    int neighbor_portion;
    int inhaled_particles;

    vp_cell() : counter(-1), prev_type(AIR), type(AIR), breathing_counter(20), infected(false), prev_num_particles(0), num_particles(0), neighbor_portion(0), inhaled_particles(0) {}  // a default constructor is required
    vp_cell(int i_counter, CELL_TYPE i_type, int i_breathing_counter, int i_num_particles) : counter(i_counter), type(i_type), breathing_counter(i_breathing_counter), num_particles(i_num_particles) {}
    
};

// Required for comparing states and detect any change
inline bool operator != (const vp_cell &x, const vp_cell &y) {
	//note that breathing_counter is not included here intentionally as it is onlu used to add the breathing factor
    return (x.counter != y.counter || x.prev_num_particles != y.prev_num_particles || x.num_particles != y.num_particles || x.prev_type != y.prev_type || x.type != y.type);
}

// Required if you want to use transport delay (priority queue has to sort messages somehow)
inline bool operator < (const vp_cell& lhs, const vp_cell& rhs){ return true; }

// Required for printing the state of the cell
std::ostream &operator << (std::ostream &os, const vp_cell &x) {
	//note that breathing_counter is not included here as it is not useful for visualization and it will only slow the simulation to include it. Keep it this way unless needed for another reason
    os << "<" << x.counter << "," << x.prev_num_particles << "," << x.num_particles << "," << x.prev_type << "," << x.type << ">";
    return os;
}

// Required for creating vp_cell objects from JSON file
void from_json(const json& j, vp_cell &s) {
    j.at("counter").get_to(s.counter);
    j.at("type").get_to(s.type);
    //j.at("breathing_counter").get_to(s.breathing_counter);
}

/************************************/
/******COMPLEX CONFIG STRUCTURE******/
/************************************/
struct source {
    int vp_production; //vp generated by one person
    float cell_size;
    int base; //vp base level
    int resp_time;
	int breathing_rate;
	int time_active; //amount of time in states spent at TABLE
	int start_time; //start state for TABLE occupation
    int infection_time;
    int infection_threshold;
    cell_position pos;

    // Each cell is 25cm x 25cm x 25cm = 15.626 Liters of air each
    // vp sources have their num_particles continually increased by default by 12.16 ppm every 5 seconds.
    source(): vp_production(11), cell_size(25), base(0), resp_time(1), breathing_rate(20), time_active(100), start_time(20), infection_time(100), infection_threshold(1000), pos({11,3}) {}
    source(float ci, float cs, int b, int wc, int vc, int r, int br, int ta, int st, int ns, int nb, int md, int it): vp_production(ci), cell_size(cs), base(b), resp_time(r), breathing_rate(br), time_active(ta), start_time(st), infection_threshold(it) {}
};
void from_json(const json& j, source &s) {
    j.at("vp_production").get_to(s.vp_production);
    j.at("cell_size").get_to(s.cell_size);
    j.at("resp_time").get_to(s.resp_time);
    j.at("breathing_rate").get_to(s.breathing_rate);
	j.at("time_active").get_to(s.time_active);
	j.at("start_time").get_to(s.start_time);
}

template <typename T>
class vp_res_cell : public grid_cell<T, vp_cell> {
public:
    using grid_cell<T, vp_cell, int>::simulation_clock;
    using grid_cell<T, vp_cell, int>::state;
    using grid_cell<T, vp_cell, int>::map;
    using grid_cell<T, vp_cell, int>::neighbors;

    using config_type = source;  // IMPORTANT FOR THE JSON   
    int num_particles_increase; //// vp sources have their num_particles continually increased
    int base; //vp base level 
    int resp_time; //Time used to calculate the num_particles inscrease /// set in JSON
	int breathing_rate; ///the interval between two consecutive breaths in seconds.//set intially from the JSON
	int time_active; ///time spent by the person at the TABLE ///set in JSON
	int start_time; ///start time for TABLE occupation ///set in JSON
    int infection_time;
    int infection_threshold;
    cell_position pos;

 
    vp_res_cell() : grid_cell<T, vp_cell, int>() {
    }

    vp_res_cell(cell_position const &cell_id, cell_unordered<int> const &neighborhood, vp_cell initial_state,
        cell_map<vp_cell, int> const &map_in, std::string const &delayer_id, source config) :
            grid_cell<T, vp_cell>(cell_id, neighborhood, initial_state, map_in, delayer_id) {
                num_particles_increase = config.vp_production;
                base = config.base;
                resp_time = config.resp_time;
				breathing_rate = config.breathing_rate;
				time_active = config.time_active;
				start_time = config.start_time;
                infection_time = config.infection_time;
                infection_threshold = config.infection_threshold;
                pos = cell_id;

    }

   vp_cell local_computation() const override {
        vp_cell new_state = state.current_state;
        switch(new_state.type) {
            case IMPERMEABLE_STRUCTURE:
                new_state.prev_num_particles = 0;
                new_state.prev_type = IMPERMEABLE_STRUCTURE;
                new_state.num_particles = 0;
                break;
            case TABLE:
                new_state.prev_num_particles = 0;
                new_state.prev_type = TABLE;
                new_state.num_particles = 0;
                break;
            case VENTILATION:
                new_state.prev_num_particles = 0;
                new_state.prev_type = VENTILATION;
                new_state.num_particles = 0;
                break;
            case AIR:{
                int num_neighbors = 0;
                int num_particles = 0;
                new_state.prev_type = AIR;   
                new_state.neighbor_portion = 0; 
                for(auto neighbors: state.neighbors_state) {
                    if( neighbors.second.num_particles < 0){
                        assert(false && "vp_cell num_particles cannot be negative");
                    }   
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                        num_particles += neighbors.second.neighbor_portion;
                        num_neighbors++;
                    }
                }
                new_state.prev_num_particles = new_state.num_particles;
                new_state.num_particles = new_state.neighbor_portion + (new_state.num_particles % num_neighbors);
                new_state.num_particles += num_particles;

                //check if the cell is a corner, wall or open cell
                if(num_neighbors == 4 || num_neighbors == 6 || num_neighbors == 9) {
                    new_state.neighbor_portion = new_state.num_particles - (new_state.num_particles % num_neighbors);
                } 
                //check if portion to be distributed can be evenly split
                if(new_state.neighbor_portion % num_neighbors == 0) {
                    new_state.neighbor_portion = new_state.num_particles/num_neighbors;
                }
                
                break;
            }
            case CHAIR:{  
                int num_neighbors = 0;
                int num_particles = 0;
                new_state.prev_type = CHAIR;
                new_state.neighbor_portion = 0; 
                for(auto neighbors: state.neighbors_state) {
                    if( neighbors.second.num_particles < 0){
                        assert(false && "vp_cell num_particles cannot be negative");
                    }   
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                        num_particles += neighbors.second.neighbor_portion;
                        num_neighbors++;
                    }
                }
                new_state.prev_num_particles = new_state.num_particles;
                new_state.num_particles = new_state.neighbor_portion + (new_state.num_particles % num_neighbors);
                new_state.num_particles += num_particles;

                if(num_neighbors == 4 || num_neighbors == 6 || num_neighbors == 9) {
                    new_state.neighbor_portion = new_state.num_particles - (new_state.num_particles % num_neighbors);
                } 
                if(new_state.neighbor_portion % num_neighbors == 0) {
                    new_state.neighbor_portion = new_state.num_particles/num_neighbors;
                }

                if (state.current_state.counter <= start_time) { //TODO parameterize //done
                    new_state.counter += 1;                   
                }

                if (state.current_state.counter == start_time){
                    vector<int> temp = {11,3};
                    if(pos == temp && new_state.infected == false) {
                        new_state.type = vp_SOURCE; 
                        new_state.infected = true;
                        new_state.breathing_counter = 0;
                    }
                    else {
                        new_state.type = vp_RECEIVER;
                        new_state.breathing_counter = 0;
                    }
				}
                break;
            }
            case vp_SOURCE:{
                int num_neighbors = 0;
                int num_particles = 0;
                new_state.prev_type = vp_SOURCE;
                new_state.neighbor_portion = 0; 
                for(auto neighbors: state.neighbors_state) {
                    if( neighbors.second.num_particles < 0) {
                        assert(false && "vp_cell num_particles cannot be negative");
                    }   
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                        num_particles += neighbors.second.neighbor_portion;
                        num_neighbors++;
                    }
                }
                new_state.prev_num_particles = new_state.num_particles;
                new_state.num_particles = new_state.neighbor_portion + (new_state.num_particles % num_neighbors);
                new_state.num_particles += num_particles;

                if(new_state.breathing_counter % breathing_rate == 0){
					new_state.num_particles += num_particles_increase;
                    new_state.neighbor_portion = num_particles_increase - (num_particles_increase % num_neighbors);
				}
                new_state.breathing_counter++;
                new_state.counter += 1;

                if(new_state.neighbor_portion % num_neighbors == 0) {
                    new_state.neighbor_portion = new_state.num_particles/num_neighbors;
                }
				//The num_particles increases every time an occupant breathes (the default breathing rate is every five seconds)
                break;
            }
            default:{
                assert(false && "should never happen");
            }
            case vp_RECEIVER: {
                int num_neighbors = 0;
                int num_particles = 0;
                new_state.prev_type = vp_RECEIVER;
                new_state.neighbor_portion = 0;                
                if(new_state.inhaled_particles >= infection_threshold) {
                    new_state.type = vp_SOURCE;
                    break;
                }
                for(auto neighbors: state.neighbors_state) {
                    if( neighbors.second.num_particles < 0){
                        assert(false && "vp_cell num_particles cannot be negative");
                    }   
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                        num_particles += neighbors.second.neighbor_portion;
                        num_neighbors++;
                    }
                }
                new_state.prev_num_particles = new_state.num_particles;
                new_state.num_particles = new_state.neighbor_portion + (new_state.num_particles % num_neighbors);
                new_state.num_particles += num_particles;

                if((new_state.breathing_counter % breathing_rate) != 0 && num_neighbors == 4 || num_neighbors == 6 || num_neighbors == 9) {
                    new_state.neighbor_portion = new_state.num_particles - (new_state.num_particles % num_neighbors);
                }
                if( (new_state.breathing_counter % breathing_rate) == 0){
					new_state.inhaled_particles += new_state.num_particles;
                    new_state.num_particles = 0;
				}
                new_state.breathing_counter++;
                new_state.counter += 1;

                if(new_state.neighbor_portion % num_neighbors == 0 && new_state.neighbor_portion > 0) {
                    new_state.neighbor_portion = new_state.num_particles/num_neighbors;
                }
                break;
            }
        }
        return new_state;
    }
    
    // It returns the delay to communicate cell's new state.
    T output_delay(vp_cell const &cell_state) const override {
		return resp_time;
    }

};

#endif //CADMIUM_CELLDEVS_vp_CELL_HPP