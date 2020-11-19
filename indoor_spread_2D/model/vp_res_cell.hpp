/**
 * Copyright (c) 2020, Cristina Ruiz Martin
 * ARSLab - Carleton University
 * All rights reserved.
 *
 * Redistribution and use in conf and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of conf code must retain the above copyright notice,
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
* Model developed Ryan Carriere in Cell-DEVS 
* Implemented in Cadmium-cell-DEVS by Cristina Ruiz Martin
*/

#ifndef CADMIUM_CELLDEVS_vp_CELL_HPP
#define CADMIUM_CELLDEVS_vp_CELL_HPP

#include <cmath>
#include <stdlib.h>
#include <nlohmann/json.hpp>
#include <cadmium/celldevs/cell/grid_cell.hpp>

using nlohmann::json;
using namespace cadmium::celldevs;
using namespace std;

/************************************/
/******COMPLEX STATE STRUCTURE*******/
/************************************/

enum CELL_TYPE {AIR=-100, vp_SOURCE=-200, IMPERMEABLE_STRUCTURE=-300, DOOR=-400, TABLE=-500, VENTILATION=-600, CHAIR=-700, vp_RECEIVER=-800, INFECTED=-900};

struct vp_cell {
    int counter;
    bool edge;
    CELL_TYPE type;
    CELL_TYPE prev_type;
    string mask;
    int prev_num_particles;
    int num_particles;
    int neighbor_portion;
    int remainder;
    int flow_portion;
    int infection_threshold;
    int prev_inhaled_particles;
    int inhaled_particles;
    vector<int> direction;
    int time_stayed;


    vp_cell() : counter(-1), edge(false), prev_type(AIR), mask("NO_MASK"), type(AIR), prev_num_particles(0), num_particles(0), neighbor_portion(0), remainder(0), flow_portion(0), prev_inhaled_particles(0), inhaled_particles(0), infection_threshold(1000), direction({0,0}), time_stayed(1800) {}  // a default constructor is required
    vp_cell(int i_counter, CELL_TYPE i_type, int i_inhaled) : counter(i_counter), type(i_type), inhaled_particles(i_inhaled){}
    
};

// Required for comparing states and detect any change
inline bool operator != (const vp_cell &x, const vp_cell &y) {
	//note that breathing_counter is not included here intentionally as it is only used to add the breathing factor
    return (x.counter != y.counter || x.num_particles != y.num_particles || x.direction != y.direction || x.type != y.type || x.inhaled_particles != y.inhaled_particles);
}

// Required if you want to use transport delay (priority queue has to sort messages somehow)
inline bool operator < (const vp_cell& lhs, const vp_cell& rhs) { return true; }

// Required for printing the state of the cell
std::ostream &operator << (std::ostream &os, const vp_cell &x) {
	//note that breathing_counter is not included here as it is not useful for visualization and it will only slow the simulation to include it. Keep it this way unless needed for another reason
    os << "<" << x.inhaled_particles << "," << x.num_particles << "," << x.type << ">";
    //os << "<" << x.counter << "," << x.prev_inhaled_particles << "," << x.inhaled_particles << "," << x.prev_num_particles << "," << x.num_particles << "," << x.prev_type << "," << x.type << ">";
    return os;
}

// Required for creating vp_cell objects from JSON file
void from_json(const json& j, vp_cell &s) {
    //j.at("inhaled").get_to(s.inhaled_particles);
    j.at("counter").get_to(s.counter);
    j.at("type").get_to(s.type);
}

/************************************/
/******COMPLEX CONFIG STRUCTURE******/
/************************************/
struct conf {
    string spreader_mask; //default mask for spreaders
    string receiver_mask; //default mask for receivers

    vector<pair<vector<int>, string>> infected_occupants; //positions in scenario for infected occupants
    vector<pair<vector<int>, string>> healthy_occupants; //positions in scenario for healthy occupants

    bool vent; //condition to check if vent is on or off
    int breathing_production; //viral particle production from breathing
    int speaking_production; //viral particle production from speaking
    int coughing_production; //viral particle production from coughing
    float cell_size;
    int resp_time;
	int breathing_rate; //rate at which an occupant breathes
    int speaking_rate; //rate at which an occupant speaks
    int coughing_rate; // rate at which and occupant coughs
	int start_time; //start state for TABLE occupation
    int infection_threshold; 
    float flow_weight;

    float no_mask_shed;
    float no_mask_efficiency;
    float cotton_shed;
    float cotton_efficiency;
    float surgical_shed;
    float surgical_efficiency;
    float n95_shed;
    float n95_efficiency;
    float n95_fit_shed;
    float n95_fit_efficiency;
    
    conf(): spreader_mask("NO_MASK"), receiver_mask("NO_MASK"), vent(true), breathing_production(11), speaking_production(22), coughing_production(33), cell_size(25), resp_time(1), breathing_rate(3), start_time(20), infection_threshold(10), flow_weight(1) {}
    conf(string sm, string rm, vector<pair<vector<int>, string>> io, vector<pair<vector<int>, string>> ho, bool v, int bp, int sp, int cp, float cs, int b, int wc, int vc, int r, int br, int sr, int cr, int st, int ns, int nb, int md, int it, float fw, float sw, float me, float nms, float nme, float cos, float ce, float ss, float se, float nns, float nne, float nnfs, float nnfe): spreader_mask(sm), receiver_mask(rm), infected_occupants(io), healthy_occupants(ho), vent(v), breathing_production(bp), speaking_production(sp), coughing_production(33), cell_size(cs), resp_time(r), breathing_rate(br), speaking_rate(sr), coughing_rate(cr), start_time(st), infection_threshold(it), flow_weight(fw), no_mask_shed(nms), no_mask_efficiency(nme), cotton_shed(cos), cotton_efficiency(ce), surgical_shed(ss), surgical_efficiency(se), n95_shed(nns), n95_efficiency(nne), n95_fit_shed(nnfs), n95_fit_efficiency(nnfe) {}
};
void from_json(const json& j, conf &s) {
    j.at("infected_occupants").get_to(s.infected_occupants);
    j.at("healthy_occupants").get_to(s.healthy_occupants);
    j.at("vent").get_to(s.vent);
    j.at("breathing_production").get_to(s.breathing_production);
    j.at("speaking_production").get_to(s.speaking_production);
    j.at("coughing_production").get_to(s.coughing_production);
    j.at("cell_size").get_to(s.cell_size);
    j.at("resp_time").get_to(s.resp_time);
    j.at("breathing_rate").get_to(s.breathing_rate);
    j.at("speaking_rate").get_to(s.speaking_rate);
    j.at("coughing_rate").get_to(s.coughing_rate);
	j.at("start_time").get_to(s.start_time);
    j.at("infection_threshold").get_to(s.infection_threshold);
    j.at("flow_weight").get_to(s.flow_weight);
    j.at("receiver").at("mask_type").get_to(s.receiver_mask); 
    j.at("spreader").at("mask_type").get_to(s.spreader_mask);
    j.at("masks").at("NO_MASK").at("shed").get_to(s.no_mask_shed);
    j.at("masks").at("NO_MASK").at("efficiency").get_to(s.no_mask_efficiency);
    j.at("masks").at("COTTON").at("shed").get_to(s.cotton_shed);
    j.at("masks").at("COTTON").at("efficiency").get_to(s.cotton_efficiency);
    j.at("masks").at("SURGICAL").at("shed").get_to(s.surgical_shed);
    j.at("masks").at("SURGICAL").at("efficiency").get_to(s.surgical_efficiency);
    j.at("masks").at("N95").at("shed").get_to(s.n95_shed);
    j.at("masks").at("N95").at("efficiency").get_to(s.n95_efficiency);
    j.at("masks").at("N95_FIT").at("shed").get_to(s.n95_fit_shed);
    j.at("masks").at("N95_FIT").at("efficiency").get_to(s.n95_fit_efficiency);

}

template <typename T>
class vp_res_cell : public grid_cell<T, vp_cell> {
public:
    using grid_cell<T, vp_cell, int>::simulation_clock;
    using grid_cell<T, vp_cell, int>::state;
    using grid_cell<T, vp_cell, int>::map;
    using grid_cell<T, vp_cell, int>::neighbors;

    using config_type = conf;  // IMPORTANT FOR THE JSON   
    string spreader_mask; //default mask for spreaders
    string receiver_mask; //default mask for receivers

    vector<pair<vector<int>, string>> infected_occupants; //positions in scenario for infected occupants
    vector<pair<vector<int>, string>> healthy_occupants; //positions in scenario for healthy occupants

    bool vent; //condition to check if vent is on or off
    int breathing_production; //viral particle production from breathing
    int speaking_production; //viral particle production from speaking
    int coughing_production; //viral particle production from coughing
    float cell_size;
    int resp_time; //time to calculate particle increase distribution in each cell
	int breathing_rate; //rate at which an occupant breathes
    int speaking_rate; //rate at which an occupant speaks
    int coughing_rate; // rate at which and occupant coughs
	int start_time; //start state for CHAIR occupation
    int infection_threshold; //number of particles needed to infect a person
    float flow_weight; //percentage of particles that move in the direction of the airflow scale is [0-1]
    cell_position pos;

    float no_mask_shed;
    float no_mask_efficiency;
    float cotton_shed;
    float cotton_efficiency;
    float surgical_shed;
    float surgical_efficiency;
    float n95_shed;
    float n95_efficiency;
    float n95_fit_shed;
    float n95_fit_efficiency;
 
    vp_res_cell() : grid_cell<T, vp_cell, int>() {
    }

    vp_res_cell(cell_position const &cell_id, cell_unordered<int> const &neighborhood, vp_cell initial_state,
        cell_map<vp_cell, int> const &map_in, std::string const &delayer_id, conf config) :
            grid_cell<T, vp_cell>(cell_id, neighborhood, initial_state, map_in, delayer_id) {
                vent = config.vent;
                infected_occupants = config.infected_occupants;
                healthy_occupants = config.healthy_occupants;
                breathing_production = config.breathing_production;
                speaking_production = config.speaking_production;
                coughing_production = config.coughing_production;
                resp_time = config.resp_time;
				breathing_rate = config.breathing_rate;
                speaking_rate = config.speaking_rate;
                coughing_rate = config.coughing_rate;
				start_time = config.start_time;
                infection_threshold = config.infection_threshold;
                flow_weight = config.flow_weight;
                receiver_mask = config.receiver_mask;
                spreader_mask = config.spreader_mask;
                pos = cell_id;
                no_mask_shed = config.no_mask_shed;
                no_mask_efficiency = config.no_mask_efficiency;
                cotton_shed = config.cotton_shed;
                cotton_efficiency = config.cotton_efficiency;
                surgical_shed = config.surgical_shed;
                surgical_efficiency = config.surgical_efficiency;
                n95_shed = config.n95_shed;
                n95_efficiency = config.n95_efficiency;
                n95_fit_shed = config.n95_fit_shed;
                n95_fit_efficiency = config.n95_fit_efficiency;
    }

   vp_cell local_computation() const override {
        vp_cell new_state = state.current_state;
        switch(new_state.type) {
            case IMPERMEABLE_STRUCTURE: 
                new_state.num_particles = 0;
                new_state.prev_type = IMPERMEABLE_STRUCTURE;
                break;
            case DOOR: 
                new_state.num_particles = 0;
                new_state.prev_type = DOOR;
                break;
            case TABLE: {
                new_state.prev_type = TABLE;
                int num_neighbors = 0;
                int num_particles = 0;

                for(auto neighbors: state.neighbors_state) {
                    if(neighbors.second.num_particles < 0){
                        assert(false && "vp_cell num_particles cannot be negative");
                    } 
                    setDirection(new_state, neighbors);
                    loopNeighbors(num_neighbors, num_particles, new_state, neighbors);
                }
                computeParticles(new_state, num_neighbors, num_particles);
                break;
            }
            case VENTILATION:{
                if(vent) {
                    new_state.num_particles = 0;
                    new_state.prev_type = VENTILATION;
                    for(auto neighbors: state.neighbors_state) {
                        if(neighbors.second.num_particles < 0) {
                            assert(false && "vp_cell num_particles cannot be negative");
                        } 
                        setDirection(new_state, neighbors);
                    }
                }
                else {
                    new_state.prev_type = VENTILATION;
                    new_state.num_particles = 0;
                }
                break;
            }
            case AIR:{
                new_state.prev_type = AIR;
                int num_neighbors = 0;
                int num_particles = 0;

                for(auto neighbors: state.neighbors_state) {
                    if(neighbors.second.num_particles < 0){
                        assert(false && "vp_cell num_particles cannot be negative");
                    } 
                    setDirection(new_state, neighbors);
                    loopNeighbors(num_neighbors, num_particles, new_state, neighbors);
                }
                computeParticles(new_state, num_neighbors, num_particles);

                new_state.counter++;
                break;
            }
            case CHAIR:{  
                new_state.prev_type = CHAIR;
                int num_neighbors = 0;
                int num_particles = 0;

                for(auto neighbors: state.neighbors_state) {
                    if(neighbors.second.num_particles < 0){
                        assert(false && "vp_cell num_particles cannot be negative");
                    } 
                    setDirection(new_state, neighbors);
                    loopNeighbors(num_neighbors, num_particles, new_state, neighbors);
                }
                computeParticles(new_state, num_neighbors, num_particles);

                if (new_state.counter == start_time) {
                    for(auto infected: infected_occupants) {
                        if(pos == infected.first) {
                            new_state.type = vp_SOURCE; 
                            new_state.mask = infected.second;
                            new_state.counter = 0; 
                            break;
                        }
                        // else {
                        //     int random = rand() % 5 + 1;
                        //     if(random == 3) {
                        //         //int stayed = rand() % 3600 + 900;
                        //         new_state.type = vp_RECEIVER;
                        //         new_state.mask = receiver_mask;
                        //         new_state.counter = 0;
                        //         //new_state.time_stayed = stayed;
                        //         break;
                        //     }
                        // }
                    }
                    for(auto healthy: healthy_occupants) {
                        if(pos == healthy.first) {
                            new_state.type = vp_RECEIVER; 
                            new_state.mask = healthy.second;
                            new_state.counter = 0; 
                            break;
                        }
                        // else {
                        //     int random = rand() % 5 + 1;
                        //     if(random == 3) {
                        //         //int stayed = rand() % 3600 + 900;
                        //         new_state.type = vp_RECEIVER;
                        //         new_state.mask = receiver_mask;
                        //         new_state.counter = 0;
                        //         //new_state.time_stayed = stayed;
                        //         break;
                        //     }
                        // }
                    }
				}
                new_state.counter++;
                break;
            }
            case vp_RECEIVER: {
                new_state.prev_type = vp_RECEIVER;
                int num_neighbors = 0;
                int num_particles = 0;
                /*
                if(new_state.counter >= new_state.time_stayed) {
                    new_state.type = CHAIR;
                }
                */
                if(new_state.inhaled_particles >= infection_threshold) {
                    new_state.type = INFECTED;
                    break;
                }
                for(auto neighbors: state.neighbors_state) { 
                    if(neighbors.second.num_particles < 0){
                        assert(false && "vp_cell num_particles cannot be negative");
                    } 
                    setDirection(new_state, neighbors);
                    loopNeighbors(num_neighbors, num_particles, new_state, neighbors);
                }
                computeParticles(new_state, num_neighbors, num_particles);
                if(new_state.counter % breathing_rate == 0) {
                    computeInhalation(new_state);
                }
                new_state.counter++;
                break;
            }
            case vp_SOURCE:{
                new_state.prev_type = vp_SOURCE;
                int num_neighbors = 0;
                int num_particles = 0;

                for(auto neighbors: state.neighbors_state) {
                    if(neighbors.second.num_particles < 0){
                        assert(false && "vp_cell num_particles cannot be negative");
                    } 
                    setDirection(new_state, neighbors);
                    loopNeighbors(num_neighbors, num_particles, new_state, neighbors);
                }
                if(new_state.counter % breathing_rate == 0 && new_state.counter >= breathing_rate) {
                    computeEmission(new_state, breathing_production, num_particles);
                }
                if(new_state.counter % speaking_rate == 0 && new_state.counter >= speaking_rate) {
                    computeEmission(new_state, speaking_production, num_particles);
                }
                if(new_state.counter % coughing_rate == 0 && new_state.counter >= coughing_rate) {
                    computeEmission(new_state, coughing_production, num_particles);
                }
                new_state.counter++;

                computeParticles(new_state, num_neighbors, num_particles);
    
                break;
            }
            case INFECTED: {
                new_state.prev_type = INFECTED;
                int num_neighbors = 0;
                int num_particles = 0;

                for(auto neighbors: state.neighbors_state) {
                    if(neighbors.second.num_particles < 0){
                        assert(false && "vp_cell num_particles cannot be negative");
                    } 
                    setDirection(new_state, neighbors);
                    loopNeighbors(num_neighbors, num_particles, new_state, neighbors);
                }
                computeParticles(new_state, num_neighbors, num_particles);
                if(new_state.counter % breathing_rate == 0) {
                    computeInhalation(new_state);
                }
                new_state.counter++;
                break;
            }
            default:{
                assert(false && "should never happen");
            }
        }
        return new_state;
    }

    void computeParticles(vp_cell& curr, int const& num_neighbors, int const& num_particles) const {
        curr.neighbor_portion = 0;
        vector<int> temp = {0,0};
        if(curr.direction != temp && !curr.edge) {
            curr.prev_num_particles = curr.num_particles;
            curr.num_particles = curr.remainder;
            curr.num_particles += num_particles;

            int flow = (curr.num_particles - curr.remainder) * flow_weight;
            curr.flow_portion = flow;
            curr.remainder += (curr.num_particles - flow - curr.remainder);

            if(curr.remainder % num_neighbors >= 0 && curr.remainder >= num_neighbors) {
                curr.neighbor_portion = curr.remainder/num_neighbors;
                curr.remainder = curr.remainder % num_neighbors;
            } 
        }  
        /*
        else if(curr.direction != temp && curr.edge) {
            curr.num_particles += num_particles;
            return;
        }
        */
        else {
            curr.prev_num_particles = curr.num_particles;
            curr.num_particles = curr.num_particles % num_neighbors;
            curr.num_particles += num_particles;

            if(curr.num_particles % num_neighbors >= 0 && curr.num_particles >= num_neighbors) {
                curr.neighbor_portion = curr.num_particles/num_neighbors;
            } 
        }  
    }

    void setDirection(vp_cell& curr, pair<cell_position, vp_cell> const& nb) const {
        vector<int> N = {0,-1};
        vector<int> E = {1,0};
        vector<int> S = {0,1};
        vector<int> W = {-1,0};
        vector<int> Z = {0,0};
        
        vector<int> diff = {0,0};
        vector<int> shape = this->map.shape;
        vector<int> currPos = pos;

        //cout << this->map.shape << endl;
        transform(shape.begin(), shape.end(), currPos.begin(), diff.begin(), minus<int>());
        if(curr.type == VENTILATION && nb.second.type != IMPERMEABLE_STRUCTURE) {
            if(curr.direction != Z) {
                return;
            }
            if(diff.at(1) == 0) {
                curr.direction = S;
            }
            else if(diff.at(0) == 1) {
                curr.direction = W;
            }
            else if(diff.at(1) == 1) {
                curr.direction = N;
            }
            else if(diff.at(0) == 0) {
                curr.direction = E;
            }
        }
        else if(curr.type == VENTILATION && nb.second.type == IMPERMEABLE_STRUCTURE) {
            if(curr.direction != Z) {
                return;
            }
            if(this->map.relative(nb.first) == N) {
                curr.direction = S;
            }
            else if(this->map.relative(nb.first) == E) {
                curr.direction = W;
            }
            else if(this->map.relative(nb.first) == S) {
                curr.direction = N;
            }
            else if(this->map.relative(nb.first) == W) {
                curr.direction = E;
            }
        }
        else {
            if(curr.direction != Z) {
                return;
            }
            else if(this->map.relative(nb.first) == N && nb.second.direction == S) {
                curr.direction = S;
            }
            else if(this->map.relative(nb.first) == E && nb.second.direction == W) {
                curr.direction = W;
            }
            else if(this->map.relative(nb.first) == S && nb.second.direction == N) {
                curr.direction = N;
            }
            else if(this->map.relative(nb.first) == W && nb.second.direction == E) {
                curr.direction = E;
            }
        }
    }

    void loopNeighbors(int& num_neighbors, int& num_particles, vp_cell& curr, pair<cell_position, vp_cell> const& nb) const {
        vector<int> N = {0,-1};
        vector<int> E = {1,0};
        vector<int> S = {0,1};
        vector<int> W = {-1,0};

        if(nb.second.type != IMPERMEABLE_STRUCTURE){
            vector<int> temp = {0,0};
            vector<int> sum = {NULL, NULL};
            transform(curr.direction.begin(), curr.direction.end(), this->map.relative(nb.first).begin(), sum.begin(), plus<int>());
            if(curr.direction != temp && sum == temp) {
                num_particles += nb.second.flow_portion;
            }
            else {
                num_particles += nb.second.neighbor_portion;
            }
            num_neighbors++;
        }
        else {
            if((this->map.relative(nb.first) == N && curr.direction == S) || (this->map.relative(nb.first) == E && curr.direction == E) || (this->map.relative(nb.first) == S && curr.direction == N) || (this->map.relative(nb.first) == W && curr.direction == W)) {
                curr.edge = true;
            }
        }
        return;
    }

    void computeEmission(vp_cell& curr, int const& production, int& num_particles) const {
        if(curr.mask == "NO_MASK") {
            num_particles += production;
        }
        else if(curr.mask == "COTTON") {
            num_particles += (production * cotton_shed);
        }
        else if(curr.mask == "SURGICAL") {
            num_particles += (production * surgical_shed);
        }
        else if(curr.mask == "N95") {
            num_particles += (production * n95_shed);
        }
        else if(curr.mask == "N95_FIT") {
            num_particles += (production * n95_fit_shed);
        }
    }

    void computeInhalation(vp_cell& curr) const {
        if(curr.mask == "NO_MASK") {
            curr.inhaled_particles += (curr.num_particles * (1 - no_mask_efficiency));
            curr.num_particles -= (curr.num_particles * (1 - no_mask_efficiency));
        }
        else if(curr.mask == "COTTON") {
            curr.inhaled_particles += (curr.num_particles * (1 - cotton_efficiency));
            curr.num_particles -= (curr.num_particles * (1 - cotton_efficiency));
        }
        else if(curr.mask == "SURGICAL") {
            curr.inhaled_particles += (curr.num_particles * (1 - surgical_efficiency));
            curr.num_particles -= (curr.num_particles * (1 - surgical_efficiency));
        }
        else if(curr.mask == "N95") {
            curr.inhaled_particles += (curr.num_particles * (1 - n95_efficiency));
            curr.num_particles -= (curr.num_particles * (1 - n95_efficiency));
        }
        else if(curr.mask == "N95_FIT") {
            curr.inhaled_particles += (curr.num_particles * (1 - n95_fit_efficiency));
            curr.num_particles -= (curr.num_particles * (1 - n95_fit_efficiency));
        }
    }

    // It returns the delay to communicate cell's new state.
    T output_delay(vp_cell const &cell_state) const override {
		return resp_time;
    }

};

#endif //CADMIUM_CELLDEVS_vp_CELL_HPP