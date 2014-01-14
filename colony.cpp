#include "colony.h"
#include <string>
#include <limits>
#include <iostream>
#include <glog/logging.h>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

Colony::Colony(int number_of_ants, int number_of_iterations, const Tup& 
problem_instance, double default_pheromone, double Q, double pheromone_persistence, 
int seed, double alpha, double beta) : 
number_of_ants_(number_of_ants), 
number_of_iterations_(number_of_iterations),
problem_instance_(problem_instance), 
default_pheromone_(default_pheromone),
Q_(Q),
pheromone_persistence_(pheromone_persistence)
{
	//create pheromone matrix with dimensions umpires x slot x games, the idea is to store the next concept
	//the umpire x in the slot y refereed the game z, so we sum up a pheromone (or substract if is bad)
	//OBS: the number of games per slot is the same number of umpires
	pheromone_.resize(boost::extents
		[problem_instance_.number_of_umpires()]
		[problem_instance_.number_of_slots()]
		[problem_instance_.number_of_umpires()]);
	//also initialize the best distance and best schedule
	best_distance_ = std::numeric_limits<double>::max();
	best_schedule_.resize(extents[problem_instance.number_of_umpires()]
								 [problem_instance.number_of_slots()]);
		
	fill(pheromone_.data(), pheromone_.data() + pheromone_.num_elements(), default_pheromone_);
	//create the random generator
	MersenneRandom rnd {(unsigned int) seed};
	//walk through iterations
	for(int cycle=0; cycle < number_of_iterations_; cycle++){
		DLOG(INFO) << "STARTING ITERATION NUMBER = " << cycle;
		//start the colony, the idea is to run a complete construction of a 
		//schedule through each iteration and update the pheromone
		//create vector of ants, first ensure is empty
		ants_.clear();
		for(int i=0; i < number_of_ants_; i++){
			//TODO: set K parameter dinamically
			Ant* new_ant = new Ant(problem_instance_, problem_instance_.number_of_umpires(), 
						rnd);
			new_ant->setColonyPheromone(&pheromone_);
			new_ant->setAlphaBeta(alpha, beta);
			ants_.push_back(new_ant);
		}
		//now construct each schedule for each ant, slot starts in 1 because in
		//the constructor of each ant it moves randomly in the first slot
		for(int slot=1; slot < problem_instance.number_of_slots(); slot++){
			DLOG(INFO) << "CONSTRUCTING SLOT NUMBER = " << slot;
			for(Ant* ant : ants_)
				ant->move();
			//after the end of the slot we update the pheromone
			//clever way to save some space
			DLOG(INFO) << "STARTING UPDATE PHEROMONE PROCESS...";
			vector<string> to_update;
			vector<double> to_update_value;
			for(Ant* ant : ants_){
				for(int umpire=0; umpire < problem_instance.number_of_umpires(); umpire++){
					Game game = ant->schedule()[umpire][slot];
					//IDEA: create a key from umpire-slot-game and insert it into
					//the vector to_update, if the key exists already in the vector
					//the sum up the value and acumulate it into the pheromone for
					//the key umpire-slot-game
					string keys = to_string(umpire) + "-" + to_string(slot) + "-" +
						to_string(game.index_slot());
					auto it = find(to_update.begin(), to_update.end(), keys);
					if (it == to_update.end()) {
					  to_update.push_back(keys);
					  to_update_value.push_back(Q / ant->get_distance_actual_slot()); 
					} else {
					  auto index = std::distance(to_update.begin(), it);
					  to_update_value[index] += Q / ant->get_distance_actual_slot();
					}
				}
			}
			//now update the pheromone matrix: traverse to_update vector, split
			//each string using '-' as delimiter and transform each token into a
			//integer, use that integer as index for pheromone_ matrix, the index
			//of the update_vector element is used to get the value from to_update_value
			for(vector<string>::size_type i=0; i < to_update.size(); i++){
				vector<string> tokens;
				split(to_update[i], '-', tokens);
				int umpire = stoi(tokens[0]);
				int slot = stoi(tokens[1]);
				int game = stoi(tokens[2]);
				double previous_pheromone = pheromone_[umpire][slot][game];
				pheromone_[umpire][slot][game] = pheromone_persistence*previous_pheromone +
					to_update_value[i];
			}
			DLOG(INFO) << "END UPDATE PHEROMONE PROCESS...";
		}
		
		//now that all ants have their schedules completed we find for the best
		int best_ant = -1;
		for(vector<Ant*>::size_type ant = 0; ant < ants_.size(); ant++){
			if( ants_[ant]->get_total_distance() < best_distance_){
				best_ant = ant; //this is to avoid too many copies of schedule matrix
				best_distance_ = ants_[ant]->get_total_distance();
			}
		}
		//now we copy the schedule matrix if there's a good ant
		if(best_ant != -1)
			best_schedule_ = ants_[best_ant]->schedule();
		
		//destroy all ants
		for(Ant* ant : ants_)
			delete ant;
			
		DLOG(INFO) << "Best distance founded until iteration " << cycle << " is:";
		DLOG(INFO) << "DISTANCE = " << best_distance_;
	}
	
	DLOG(INFO) << "Best distance founded = " << best_distance_;
	//usually in the sintonizing process we need to print only the fitness
	cout << best_distance_;
}

Colony::~Colony()
{
}

void Colony::setProblemInstance(const Tup& problem_instance)
{
	this->problem_instance_ = problem_instance_;
}