#ifndef COLONY_H
#define COLONY_H

#include "array_3d.h"
#include "ant.h"

using namespace std;

class Colony
{
private:
	int number_of_ants_;
	int number_of_iterations_;
	int cycle_counter_;
	double best_tour_length_;
	vector<Ant*> ants_;
	array_3d pheromone_;
	Tup problem_instance_;
	double default_pheromone_;
	double Q_;
	double pheromone_persistence_;
	double best_distance_;
	multi_array<Game, 2> best_schedule_;
public:
	Colony(int number_of_ants, int number_of_iterations, const Tup& 
problem_instance, double default_pheromone, double Q, double pheromone_persistence, 
int seed, double alpha, double beta);
	~Colony();
	void setProblemInstance(const Tup& problem_instance);

};

#endif // COLONY_H
