#include "colony.h"

Colony::Colony(const Tup& problem_instance, double default_pheromone) : problem_instance_(problem_instance), 
default_pheromone_(default_pheromone)
{
	//create pheromone matrix with dimensions umpires x slot x games, the idea is to store the next concept
	//the umpire x in the slot y refereed the game z, so we sum up a pheromone (or substract if is bad)
	//OBS: the number of games per slot is the same number of umpires
	pheromone_.resize(boost::extents
		[problem_instance_.number_of_umpires()]
		[problem_instance_.number_of_slots()]
		[problem_instance_.number_of_umpires()]);
	fill(pheromone_.data(), pheromone_.data() + pheromone_.num_elements(), default_pheromone_); 
}

Colony::~Colony()
{
}

void Colony::setProblemInstance(const Tup& problem_instance)
{
	this->problem_instance_ = problem_instance_;
}
