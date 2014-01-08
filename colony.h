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
	vector<Ant> ants_;
	array_3d pheromone_;
	Tup problem_instance_;
	double default_pheromone_;
public:
	Colony(const Tup& problem_instance, double default_pheromone);
	~Colony();
	void setProblemInstance(const Tup& problem_instance);

};

#endif // COLONY_H
