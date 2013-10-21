/**
 * Ant+TUP - IA Avazada
 * ant.h
 * Purpose: Class representing an ant entity.
 * @author dacuna <dacuna@alumnos.inf.utfsm.cl>
 * @version 1.0 10/10/2013
 */

#ifndef ANT_H
#define ANT_H

#include <vector>
#include <boost/multi_array.hpp>
#include <lemon/list_graph.h>
#include "tup.h"
#include "mersenne_random.h"
#include "game.h"

using namespace std;
using namespace lemon;
using namespace boost;

/** 
 * Ant entity used in the ant colony algorithm.
 *
 * Contains useful methods abstracting the work made by each ant in the
 * overall ant colony algorithm. Also, this entity handles the logic inside
 * the traveling umpire problem. With this remark its clear that this class
 * should not be used, in its raw format, in a different problem or context.
 * 
 * @author dacuna <dacuna@alumnos.inf.utfsm.cl>
 * @date 10/10/2013
 */
class Ant
{
private:
	int actual_slot_;                   /**<In classic ant it should be actual_city but in ant+tup we travel through slots */
	double total_distance_;             /**<Total distance of the schedule */
	vector<double> distance_by_umpire_; /**<Distance walked by each umpire */
	multi_array<Game, 2> schedule_;   /**<Schedule for an entire tournament build by the ant */ 
	ListDigraph restrictions_graph_;    /**<Graph representing restrictions in each round of the schedule */
	ListDigraph::NodeMap<int> restrictions_graph_map_; /**<Map of restrictions_graph_ */
	vector<ListDigraph::Node> slots_r1_; /**<Nodes representing each slot in the schedule for home restriction */
	vector<ListDigraph::Node> slots_r2_; /**<Nodes representing each slot in the schedule for venue restriction */
	Tup problem_instance_;              /**<Instance of TUP o solve */
	MersenneRandom rnd_;				/**<Instance of a random generator */
public:
	/**
	 * Class constructor.
	 *
	 * @param Tup reference to a problem instance to solve
	 */
	Ant(Tup& tup);
	
	/**
	 * Create and populates the graph of restrictions (filling the slots and the map).
	 */
	void prepare_restrictions_graph();
	
	/**
	 * Assigns a game to a umpire in the round defined by actual_slot_. This
	 * method takes care of populate the restrictions graph according to the
	 * recently made assignation.
	 *
	 * @param Game reference to the game assigned
	 * @param int which umpire gets a game assigned
	 */
	void assign_match_to_umpire(Game game, int umpire);
	
	void update_distance(int umpire);
	
	void move();
	
	bool construct_perfect_match(const vector<Game>& potencial, vector<int>& match);
	
	bool check_restriction(Game game, int umpire);
	
	ListDigraph::Node get_umpire_node_from_slot(int slot, int umpire, const vector<ListDigraph::Node>& slot_vector);
	
	/**
	 * Sets the random generator instance.
	 *
	 * @param MersenneRandom reference to a random generator instance
	 */
	void set_random_generator(MersenneRandom& rnd);
	
	const ListDigraph& restrictions_graph() const;
	
	const ListDigraph::NodeMap<int>& restrictions_graph_map() const;
	
	const multi_array<Game, 2>& schedule() const;
	
	/**
	 * Class destructor.
	 */
	~Ant();

};

#endif // ANT_H
