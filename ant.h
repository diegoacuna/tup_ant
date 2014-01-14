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
#include <lemon/list_graph.h>
#include "array_3d.h"
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
	int id_;
	int actual_slot_;                   /**<In classic ant it should be actual_city but in ant+tup we travel through slots */
	double total_distance_;             /**<Total distance of the schedule */
	vector<double> distance_by_umpire_; /**<Distance walked by each umpire */
	double distance_actual_slot;        /**<Distance walked in the actual slot by each umpire */
	multi_array<Game, 2> schedule_;   /**<Schedule for an entire tournament build by the ant */
	ListDigraph restrictions_graph_;    /**<Graph representing restrictions in each round of the schedule */
	ListDigraph::NodeMap<int> restrictions_graph_map_; /**<Map of restrictions_graph_ */
	vector<ListDigraph::Node> slots_r1_; /**<Nodes representing each slot in the schedule for home restriction */
	vector<ListDigraph::Node> slots_r2_; /**<Nodes representing each slot in the schedule for venue restriction */
	Tup problem_instance_;              /**<Instance of TUP o solve */
	MersenneRandom rnd_;				/**<Instance of a random generator */
	int seed_;
	int candidates_list_size_;          /**<Parameter K = size of the candidates list **/
	array_3d* colony_pheromone_;
	double alpha_;
	double beta_;
public:
	/**
	 * Class constructor.
	 *
	 * @param Tup reference to a problem instance to solve
	 */
	Ant(int id, Tup& tup, int K, int seed);

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

	/**
	 * Update the distance traveled by an umpire at the end of the actual slot.
	 *
	 * @param int which umpire gets his distance updated
	 */
	void update_distance(int umpire);

	void move();

	/**
	 * Creates a perfect weighted match for the actual slot of the tournament.
	 *
	 * @param vector<Game> reference to a vector with all the games played in the actual slot
	 * @param vector<int> vector<int> reference to a vector where the resultant match is going to be stored.
	 *
	 * @return bool true if was possible to construct a perfect match, false otherwise
	 */
	bool construct_perfect_match(const vector<Game>& potencial, vector<int>& match);

	/**
	 * Construct a match using the gamma criterion for umpires in the vector number_assignations until
	 * the umpired indexed by the index until_umpire. The created match (if possible) gets assigned to
	 * vector match.
	 *
	 * @param vector<Game> vector with the potencial assignations of games for the actual slot
	 * @param vector< pair<int, int> > reference to a ordered vector of possible number of assignation by umpire
	 * @param vector< pair<int, int> > reference to a ordered vector of total traveled distance by umpire
	 * @param vector< vector<int> > vector with possible games assignations to umpires
	 * @param int index indicating until which umpire we need to assign a priori matchs
	 * @param vector<int> reference to a vector where the resultant match is going to be stored.
	 *
	 * @return bool true if was possible to construct a match, false otherwise
	 */
	bool construct_match_from_gamma_criterion(vector<Game> potencial, const vector< pair<int, int> >&
	        number_assignations, const vector< vector<int> >& possible_assignations, int until_umpire, vector<int>& match);

	/**
	 * Check if the umpire indexed by 'umpire' can arbitrate the game 'game' in the actual slot according
	 * to his actual restrictions.
	 *
	 * @param Game Game to check
	 * @param int umpire to check
	 *
	 * @return bool true if the umpire is able to arbitrate, false otherwise
	 */
	bool check_restriction(Game game, int umpire);

	ListDigraph::Node get_umpire_node_from_slot(int slot, int umpire, const vector<ListDigraph::Node>& slot_vector);
	
	int calculatePheromoneForCandidateList(vector< vector<int> >& candidateList);
		 
	int calculatePheromoneForGameList(int umpire, const vector<int>& potencial_games, 
		const vector<Game>& games);

	/**
	 * Sets the random generator instance.
	 *
	 * @param MersenneRandom reference to a random generator instance
	 */
	void set_random_generator(MersenneRandom& rnd);

	const ListDigraph& restrictions_graph() const;

	const ListDigraph::NodeMap<int>& restrictions_graph_map() const;

	const multi_array<Game, 2>& schedule() const;

	void setColonyPheromone(array_3d* colony_pheromone_) {
		this->colony_pheromone_ = colony_pheromone_;
	}
	
	void setAlphaBeta(double alpha, double beta);
	
	double get_distance_actual_slot();
	
	double get_total_distance();
	
	int id();

	/**
	 * Class destructor.
	 */
	~Ant();

};

#endif // ANT_H
