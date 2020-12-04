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
        string mask;
        int num_particles;
        int neighbor_portion;
        int remainder;
        int flow_portion;
        int infection_threshold;
        int inhaled_particles;
        vector<int> direction;
        int time_stayed;
        float conc;
        float total_quanta;

        vp_cell() : total_quanta(0), counter(-1), edge(false), mask("NO_MASK"), type(AIR), num_particles(0), neighbor_portion(0), remainder(0), flow_portion(0), infection_threshold(1000), inhaled_particles(0), direction({0,0}), time_stayed(1800), conc(0) {}  // a default constructor is required
        vp_cell(int i_counter, CELL_TYPE i_type, int i_inhaled) : counter(i_counter), type(i_type), inhaled_particles(i_inhaled) {}
        
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
        os << "<" << x.inhaled_particles << "," << x.num_particles << "," << x.type << ">";
        return os;
    }

    // Required for creating vp_cell objects from JSON file
    void from_json(const json& j, vp_cell &s) {
        j.at("counter").get_to(s.counter);
        j.at("type").get_to(s.type);
    }

    /************************************/
    /******COMPLEX CONFIG STRUCTURE******/
    /************************************/
    struct conf {
        vector<pair<vector<int>, string>> infected_occupants; //positions in scenario for infected occupants
        vector<pair<vector<int>, string>> healthy_occupants; //positions in scenario for healthy occupants

        bool vent;
        float cell_size;
        int resp_time;
        int start_time;
        float flow_weight;
        int infection_threshold;

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

        float ERq_resting;
        float ERq_speaking;
        int IR_resting;
        int IR_speaking;
        float volume;
        float n0;
        float IVVR;

        conf(): vent(true), cell_size(25), resp_time(1), start_time(20), flow_weight(1) {}
        conf(string sm, string rm, vector<pair<vector<int>, string>> io, vector<pair<vector<int>, string>> ho, bool v, float cs, int b, int wc, int vc, int r, int st, int ns, int nb, int md, float fw, int it, float sw, float me, float nms, float nme, float cos, float ce, float ss, float se, float nns, float nne, float nnfs, float nnfe, float err, float ers, float irr, float irs, float vol, float nz, float ni, float ivvr): infected_occupants(io), healthy_occupants(ho), vent(v), cell_size(cs), resp_time(r), start_time(st), flow_weight(fw), infection_threshold(it), no_mask_shed(nms), no_mask_efficiency(nme), cotton_shed(cos), cotton_efficiency(ce), surgical_shed(ss), surgical_efficiency(se), n95_shed(nns), n95_efficiency(nne), n95_fit_shed(nnfs), n95_fit_efficiency(nnfe), ERq_resting(err), ERq_speaking(ers), IR_resting(irr), IR_speaking(irs), volume(vol), n0(nz), IVVR(ivvr) {}
    };
    void from_json(const json& j, conf &s) {
        j.at("infected_occupants").get_to(s.infected_occupants);
        j.at("healthy_occupants").get_to(s.healthy_occupants);
        j.at("vent").get_to(s.vent);
        j.at("cell_size").get_to(s.cell_size);
        j.at("resp_time").get_to(s.resp_time);
        j.at("start_time").get_to(s.start_time);
        j.at("flow_weight").get_to(s.flow_weight);
        j.at("infection_threshold").get_to(s.infection_threshold);
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
        j.at("quanta_params").at("ERq_resting").get_to(s.ERq_resting);
        j.at("quanta_params").at("ERq_speaking").get_to(s.ERq_speaking);
        j.at("quanta_params").at("IR_resting").get_to(s.IR_resting);
        j.at("quanta_params").at("IR_speaking").get_to(s.IR_speaking);
        j.at("quanta_params").at("volume").get_to(s.volume);
        j.at("quanta_params").at("n0").get_to(s.n0);
        j.at("quanta_params").at("IVVR").get_to(s.IVVR);
    }

    template <typename T>
    class vp_res_cell : public grid_cell<T, vp_cell> {
    public:
        using grid_cell<T, vp_cell, int>::simulation_clock;
        using grid_cell<T, vp_cell, int>::state;
        using grid_cell<T, vp_cell, int>::map;
        using grid_cell<T, vp_cell, int>::neighbors;

        using config_type = conf;  // IMPORTANT FOR THE JSON   

        vector<pair<vector<int>, string>> infected_occupants; //positions in scenario for infected occupants
        vector<pair<vector<int>, string>> healthy_occupants; //positions in scenario for healthy occupants

        /**
         *  scenario params
         */
        bool vent; //condition to check if vent is on or off
        float cell_size; // size of the cell in centimeters
        int resp_time; //time to calculate particle increase distribution in each cell
        int start_time; //start state for CHAIR occupation
        int infection_threshold; //number of particles needed to put a person at risk
        float flow_weight; //percentage of particles that move in the direction of the airflow scale is [0-1]
        cell_position pos; // position of the cell

        /**
         *  mask params
         */
        float no_mask_shed; //percentage of particles that shed with no mask
        float no_mask_efficiency; //percentage of particles that are blocked with no mask
        float cotton_shed; //percentage of particles that shed with a cotton mask
        float cotton_efficiency; //percentage of particles that are blocked with a cotton mask
        float surgical_shed; //percentage of particles that shed with a surgical mask
        float surgical_efficiency; //percentage of particles that are blocked with a surgical mask
        float n95_shed; //percentage of particles that shed with a n95 mask
        float n95_efficiency; //percentage of particles that are blocked with a n95 mask
        float n95_fit_shed; //percentage of particles that shed with a n95_fit mask
        float n95_fit_efficiency; //percentage of particles that are blocked with a n95_fit mask

        /**
         *  quanta params
         */
        float ERq_resting; //the emission rate while resting(in quanta)
        float ERq_speaking; //the emission rate while speaking(in quanta)
        int IR_resting; //the inhalation rate while resting(in quanta)
        int IR_speaking; //the inhalation rate while speaking(in quanta)
        float volume; //the volume of the room
        float n0; //the initial concentration in a room
        float IVVR; //the infectiuous viral removal rate (air eschange rate + deposition rate + inactivation rate)
    
        vp_res_cell() : grid_cell<T, vp_cell, int>() {
        }

        vp_res_cell(cell_position const &cell_id, cell_unordered<int> const &neighborhood, vp_cell initial_state,
            cell_map<vp_cell, int> const &map_in, std::string const &delayer_id, conf config) :
                grid_cell<T, vp_cell>(cell_id, neighborhood, initial_state, map_in, delayer_id) {
                    vent = config.vent;
                    infected_occupants = config.infected_occupants;
                    healthy_occupants = config.healthy_occupants;
                    resp_time = config.resp_time;
                    start_time = config.start_time;
                    infection_threshold = config.infection_threshold;
                    flow_weight = config.flow_weight;
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
                    ERq_resting = config.ERq_resting;
                    ERq_speaking = config.ERq_speaking;
                    IR_resting = config.IR_resting;
                    IR_speaking = config.IR_speaking;
                    volume = config.volume;
                    n0 = config.n0;
                    IVVR = config.IVVR;
        }

    vp_cell local_computation() const override {
            vp_cell new_state = state.current_state;
            switch(new_state.type) {
                case IMPERMEABLE_STRUCTURE: 
                    new_state.num_particles = 0;
                    break;
                case DOOR: 
                    new_state.num_particles = 0;
                    break;
                case TABLE: {
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
                        for(auto neighbors: state.neighbors_state) {
                            if(neighbors.second.num_particles < 0) {
                                assert(false && "vp_cell num_particles cannot be negative");
                            } 
                            setDirection(new_state, neighbors);
                        }
                    }
                    else {
                        new_state.num_particles = 0;
                    }
                    break;
                }
                case AIR:{
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
                    int num_neighbors = 0;
                    int num_particles = 0;

                    for(auto neighbors: state.neighbors_state) {
                        if(neighbors.second.num_particles < 0) {
                            assert(false && "vp_cell num_particles cannot be negative");
                        } 
                        setDirection(new_state, neighbors);
                        loopNeighbors(num_neighbors, num_particles, new_state, neighbors);
                    }
                    computeParticles(new_state, num_neighbors, num_particles);

                    if (new_state.counter == start_time) {
                        for(auto infected: infected_occupants) {
                            // set location of infected occupants
                            if(pos == infected.first) {
                                new_state.type = vp_SOURCE; 
                                new_state.mask = infected.second;
                                new_state.counter = 0; 
                                break;
                            }
                            else {
                                // // randomize location of infected occupants
                                // int random = rand() % 5 + 1;
                                // if(random == 3) {
                                //     new_state.type = vp_RECEIVER;
                                //     new_state.mask = receiver_mask;
                                //     new_state.counter = 0;
                                //     break;
                                // }
                            }
                        }
                        for(auto healthy: healthy_occupants) {
                            // set location of healthy occupants
                            if(pos == healthy.first) {
                                new_state.type = vp_RECEIVER; 
                                new_state.mask = healthy.second;
                                new_state.counter = 0; 
                                break;
                            }
                            else {
                                // // randomize location of healthy occupants
                                // int random = rand() % 5 + 1;
                                // if(random == 3) {
                                //     new_state.type = vp_RECEIVER;
                                //     new_state.mask = receiver_mask;
                                //     new_state.counter = 0;
                                //     break;
                                // }
                            }
                        }
                    }
                    new_state.counter++;
                    break;
                }
                case vp_RECEIVER: {
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
                    if(new_state.counter % IR_resting == 0) {
                        computeInhalation(new_state);
                    }
                    new_state.counter++;
                    break;
                }
                case vp_SOURCE:{
                    int num_neighbors = 0;
                    int num_particles = 0;

                    for(auto neighbors: state.neighbors_state) {
                        if(neighbors.second.num_particles < 0){
                            assert(false && "vp_cell num_particles cannot be negative");
                        } 
                        setDirection(new_state, neighbors);
                        loopNeighbors(num_neighbors, num_particles, new_state, neighbors);
                    }
                    computeEmission(new_state, num_particles);
                    new_state.counter++;

                    computeParticles(new_state, num_neighbors, num_particles);
        
                    break;
                }
                case INFECTED: {
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
                    if(new_state.counter % IR_resting == 0) {
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

        /**
         * Calculates the number of particles to distribute to the neighbouring cells
         * 
         * @param curr reference to the cell
         * @param num_neighbors reference to the number of neighbours
         * @param num_particles reference to the number of particles
         */
        void computeParticles(vp_cell& curr, int const& num_neighbors, int const& num_particles) const {
            curr.neighbor_portion = 0;
            vector<int> temp = {0,0};
            if(curr.direction != temp && !curr.edge) {
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
                curr.num_particles = curr.num_particles % num_neighbors;
                curr.num_particles += num_particles;

                if(curr.num_particles % num_neighbors >= 0 && curr.num_particles >= num_neighbors) {
                    curr.neighbor_portion = curr.num_particles/num_neighbors;
                } 
            }  
        }

        /**
         * Sets the direction of the airflow in a given cell
         * 
         * @param curr reference to the cell
         * @param nb reference to the neighbouring cell
         */
        void setDirection(vp_cell& curr, pair<cell_position, vp_cell> const& nb) const {
            vector<int> N = {0,-1};
            vector<int> E = {1,0};
            vector<int> S = {0,1};
            vector<int> W = {-1,0};
            vector<int> Z = {0,0};
            
            vector<int> diff = {0,0};
            vector<int> shape = this->map.shape;
            vector<int> currPos = pos;

            transform(shape.begin(), shape.end(), currPos.begin(), diff.begin(), minus<int>());
            if(curr.type == VENTILATION && nb.second.type != IMPERMEABLE_STRUCTURE) {
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

        /**
         * loops through the neighbouring cells, and collects the particles that need to be distributed
         * 
         * @param num_neighbors reference to the number of neighbouring cells
         * @param num_particles reference to the number of particles to increase by
         * @param curr reference to the cell
         * @param nb reference to the neighbouring cell
         */
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

        /**
         * Calculates the increase in viral particles from a cell which has an infected occupant
         * 
         * @param curr a reference to the cell 
         * @param num_particles a reference to the number of particles to increase by
         */
        void computeEmission(vp_cell& curr, int& num_particles) const {
            concentration(curr, ERq_speaking, infected_occupants.size(), IVVR, volume, n0);

            float increase = curr.conc;
            int scale = 10000;
            int scaled = increase * scaled;
            // cout << scaled << " for cell" << pos << " at timestep - " << curr.counter << endl;
            // cout << "total quanta for cell" << pos << curr.total_quanta << endl;
            if(curr.mask == "NO_MASK") {
                num_particles += scaled;
            }
            else if(curr.mask == "COTTON") {
                num_particles += (scaled * cotton_shed);
            }
            else if(curr.mask == "SURGICAL") {
                num_particles += (scaled * surgical_shed);
            }
            else if(curr.mask == "N95") {
                num_particles += (scaled * n95_shed);
            }
            else if(curr.mask == "N95_FIT") {
                num_particles += (scaled * n95_fit_shed);
            }
        }

        void computeInhalation(vp_cell& curr) const {
            if(curr.inhaled_particles >= infection_threshold && curr.type != INFECTED) {
                float IOS = infected_occupants.size();
                float HOS = healthy_occupants.size();
                float PI = IOS/HOS;
                //cout << "PI: " << PI << endl;
                float R = PI * 0.145;
                //cout << "R: " << R << endl;
                int odds = 100/(R * 100);
                //cout << "odds: " << odds << endl;
                int random = rand() % odds + 1;
                cout << random << endl;
                if(random == 2) {
                    curr.type = INFECTED;
                    curr.counter = 0;

                }
            }
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

        /**
         * Calculates the concentration in one cell at a given time
         * 
         * @param curr a reference to the cell 
         * @param ERq the Emission Rate in quanta
         * @param I the number of infectious people
         * @param IVVR the viral removal rate, (virus inactivation + air exchange rate + deposition rate)
         * @param V the volume of the room
         * @param n0 the concentration at time 0
         */
        void concentration(vp_cell& curr, float ERq, int I, float IVVR, float V, float n0) const {
            float time = curr.counter;
            time /= 3600;
            float conc;
            float total;
            float increase;
            conc = ( (ERq * I) / (IVVR * V) ) + ( (n0 - ( (ERq * I) / IVVR) ) * (pow(M_E, - (IVVR * time) ) / V) );
            total = conc * V;
            increase = total - curr.total_quanta;
            curr.total_quanta = total;
            curr.conc = increase;
        }

        // It returns the delay to communicate cell's new state.
        T output_delay(vp_cell const &cell_state) const override {
            return resp_time;
        }

    };

    #endif //CADMIUM_CELLDEVS_vp_CELL_HPP