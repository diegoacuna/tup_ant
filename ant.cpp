/**
 * Ant+TUP - IA Avazada
 * ant.cpp
 * Purpose: Implementation of Ant class.
 * @author dacuna <dacuna@alumnos.inf.utfsm.cl>
 * @version 1.0 10/10/2013
 */

#include <glog/logging.h>
#include <lemon/matching.h>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include "ant.h"

Ant::Ant(Tup& tup, int K) : actual_slot_(0), restrictions_graph_map_(restrictions_graph_), problem_instance_(tup),
 candidates_list_size_(K)
{
	//initialize some attributes
	prepare_restrictions_graph();
	schedule_.resize(extents[tup.number_of_umpires()][tup.number_of_slots()]);
	total_distance_ = 0.0;
	
	//sets the initial random startup, we take random elements from a bag of possible
	//matchs to assign to each umpire.
	vector<int> bag; 
	for(int i = 0; i < tup.number_of_umpires(); i++){
		distance_by_umpire_.push_back(0.0);
		bag.push_back(i);
	}
	int umpire = 0;
	for(int i = tup.number_of_umpires()-1; i >= 0; i--){
		rnd_.create_uniform_int_dis(0, i);
		int rand_value = rnd_.uniform_int_number();
		assign_match_to_umpire(tup.teams_schedule()[bag[rand_value]][0], umpire);
		//schedule_[umpire][actual_slot_] = tup.teams_schedule()[bag[rand_value]][0];
		bag.erase(bag.begin() + rand_value);
		umpire++;
	}
	actual_slot_++;
	
	move();
}

void Ant::prepare_restrictions_graph()
{
	DLOG(INFO) << "Creating restrictions graph (with r = " << problem_instance_.number_of_slots() << " slots)...";
	for(int i=0; i < problem_instance_.number_of_slots(); i++){
		slots_r1_.push_back(restrictions_graph_.addNode());
		restrictions_graph_map_[slots_r1_.back()] = i;
		slots_r2_.push_back(restrictions_graph_.addNode());
		restrictions_graph_map_[slots_r2_.back()] = i;
		DLOG(INFO) << "Assigning umpire nodes (= " << problem_instance_.number_of_umpires() << ") to slot node " << i;
		for(int j = 0; j < problem_instance_.number_of_umpires(); j++){
			ListDigraph::Node ump = restrictions_graph_.addNode();
			restrictions_graph_map_[ump] = j;
			restrictions_graph_.addArc(slots_r1_.back(), ump);
			//for slots_r2
			ListDigraph::Node ump2 = restrictions_graph_.addNode();
			restrictions_graph_map_[ump2] = j;
			restrictions_graph_.addArc(slots_r2_.back(), ump2);
		}
	}
}

void Ant::assign_match_to_umpire(Game game, int umpire)
{
	Game assigned = game;
	//assign the match to the umpire in the actual slot
	schedule_[umpire][actual_slot_] = game;
	DLOG(INFO) << "Assigned match (" << game.local_team() << ", " << game.visit_team() << 
		") to umpire " << umpire << " in the slot " << actual_slot_;
	//now we need to take care of the restrictions graph, if we are in the last slot, then continue
	if(actual_slot_ == problem_instance_.number_of_slots() -1)
		return;
	//case: the two last restrictions are not valid (i.e. are off)
	if(problem_instance_.home_restriction() == 0 && problem_instance_.venue_restriction() == 0)
		return;
	if(problem_instance_.home_restriction() == 1 && problem_instance_.venue_restriction() == 1)
		return;
	//process home restriction
	ListDigraph::Node hres = restrictions_graph_.addNode();
	restrictions_graph_map_[hres] = game.local_team();
	int start = actual_slot_ + 1;
	for(int i = start; i < start + problem_instance_.home_restriction() - 1; i++){
		//add arcs from the correponding slots and the corresponding umpire
		ListDigraph::Node umpire_slot = get_umpire_node_from_slot(i, umpire, slots_r1_);
		restrictions_graph_.addArc(umpire_slot, hres);
		DLOG(INFO) << "Adding home restriction for team " << game.local_team() << " in slot " << i << " to umpire " 
			<< restrictions_graph_map_[umpire_slot];
	}
	//process venue restriction
	ListDigraph::Node vres = restrictions_graph_.addNode();
	restrictions_graph_map_[vres] = game.visit_team();
	bool add_r2 = false;
	for(int i = start; i < start + problem_instance_.venue_restriction() - 1; i++){
		//add arcs from the correponding slots
		ListDigraph::Node umpire_slot = get_umpire_node_from_slot(i, umpire, slots_r2_);
		restrictions_graph_.addArc(umpire_slot, vres);
		restrictions_graph_.addArc(umpire_slot, hres);
		DLOG(INFO) << "Adding venue restriction for team " << game.local_team() << " in slot " << i << " to umpire " 
			<< restrictions_graph_map_[umpire_slot];;
		DLOG(INFO) << "Adding venue restriction for team " << game.visit_team() << " in slot " << i << " to umpire " 
			<< restrictions_graph_map_[umpire_slot];;
		add_r2 = true;
	}
	//erase not used nodes
	if(!add_r2) restrictions_graph_.erase(vres);
}

void Ant::update_distance(int umpire)
{
	//we're adding distance from game_(t-1).home to game_t.home
	Game last = schedule_[umpire][actual_slot_-1];
	Game actual = schedule_[umpire][actual_slot_];
	
	double distance = problem_instance_.distance_matrix()[last.local_team()-1][actual.local_team()-1];
	//distance is going to be added to umpire distance and to the entire global distance
	distance_by_umpire_[umpire] += distance;
	total_distance_ += distance;
}

void Ant::move()
{
	//get list of possible assignations
	vector<Game> potencial;
	for(int i = 0; i < problem_instance_.number_of_umpires(); i++)
		potencial.push_back(problem_instance_.teams_schedule()[i][actual_slot_]);
	//in the next vector we're going to store the list of candidates
	vector< vector<int> > candidates;
	
	//first the perfect weighted match
	vector<int> perfect_match;
	if(construct_perfect_match(potencial, perfect_match))
		candidates.push_back(std::move(perfect_match));
		
	//second, count possible assignations for every umpire, we need two ordered lists of umpires, one containing
	//the umpires ordered by the possible number of assignations and other ordered by the distance recorred
	//by each umpire until the present slot
	vector< pair<int, int> > number_assignations(problem_instance_.number_of_umpires(), make_pair(0, 0));
	vector< pair<int, int> > traveled_distance(problem_instance_.number_of_umpires(), make_pair(0,0));
	//in this vector we're going to store which games we can assign to every umpire
	vector< vector<int> > possible_assignations(problem_instance_.number_of_umpires());
	for(int umpire = 0; umpire < problem_instance_.number_of_umpires(); umpire++){
		number_assignations[umpire].first = traveled_distance[umpire].first = umpire;
		int previous_team = schedule_[umpire][actual_slot_-1].local_team() - 1;
		for(std::vector<Game>::size_type game = 0; game != potencial.size(); game++) {
			if(check_restriction(potencial[game], umpire)){
				number_assignations[umpire].second++;
				possible_assignations[umpire].push_back(game);
			}
		}
		traveled_distance[umpire].second = distance_by_umpire_[umpire];
	}
	//now order the vector number_assignations
	sort(number_assignations.begin(), number_assignations.end(), [](const pair<int, int>& a, 
	const pair<int, int>& b) -> bool { 
		return a.second < b.second; 
	});
	//and order the vector of traveled distance
	sort(traveled_distance.begin(), traveled_distance.end(), [](const pair<int, int>& a, 
	const pair<int, int>& b) -> bool { 
		return a.second < b.second; 
	});
	//now, traverse the number_assignations vector and watch which umpire has less possible assignations than permited
	for (std::vector<pair<int, int>>::size_type i = 0; i != number_assignations.size(); i++){
		int percentage = (number_assignations[i].second * 100) / problem_instance_.number_of_umpires();
		if(percentage > problem_instance_.gamma())
			break;
	}
	//in i we have the total number of umpires who breaks the gamma criterion, to that list of umpires we apply the criterion

	//finally move to the next round
	actual_slot_++;
}

bool Ant::construct_perfect_match(const vector<Game>& potencial, vector<int>& match)
{
	ListGraph bpMatch;
	ListGraph::EdgeMap<double> weight(bpMatch); //weights of the graph
	ListGraph::NodeMap<int> labels(bpMatch);
	vector<ListGraph::Node> bpA; //first partition (umpires)
	vector<ListGraph::Node> bpB; //second partition (games)
	
	for(int i=0; i < problem_instance_.number_of_umpires(); i++){
		bpA.push_back(bpMatch.addNode());
		labels[bpA.back()] = i;
	}
	for(std::vector<Game>::size_type i=0; i != potencial.size(); i++){
		bpB.push_back(bpMatch.addNode());
		labels[bpB.back()] = i;
	}
		
	//now, for each umpire check if we can join the umpire node with a node in bpB
	for(int umpire=0; umpire < problem_instance_.number_of_umpires(); umpire++){
		int previous_team = schedule_[umpire][actual_slot_-1].local_team() - 1;
		for(std::vector<Game>::size_type game = 0; game != potencial.size(); game++) {
			if(check_restriction(potencial[game], umpire)){
				ListGraph::Edge e = bpMatch.addEdge(bpA[umpire], bpB[game]);
				weight[e] = 1 / problem_instance_.distance_matrix()[previous_team]
					[potencial[game].local_team()-1];
			}
		}
	}
	
	//we have the full graph ready to search for a perfect matching...
	MaxWeightedPerfectMatching<ListGraph, ListGraph::EdgeMap<double> > matching(bpMatch, weight);
	if (!matching.run()){
		DLOG(WARNING) << "No perfect matching found in slot " << actual_slot_;
		return false;
	}
	DLOG(INFO) << "Perfect Matching found with weight " << matching.matchingWeight();
	//with the mate() function of MaxWeightedPerfectMatching we can obtain the matching
	//idea: iterate over bpA nodes and with mate() get the matching node in bpB
	for(ListGraph::Node umpire : bpA){
		ListGraph::Node game = matching.mate(umpire);
		match.push_back(labels[game]);
	} 
	
	return true;
}

bool Ant::construct_match_from_gamma_criterion(vector<Game> potencial, const vector< pair<int, int> >& 
		number_assignations, const vector< vector<int> >& possible_assignations, int until_umpire, vector<int>& match){
	//initialize the random generator
	rnd_.create_uniform_real_dis(0.0, 1.0);
	
	//to every umpire until 'until_umpire' we're going to assign a match a priori
	for(int i=0; i < until_umpire; i++){
		int umpire = number_assignations[i].first;
		//OBS: in potencial we've all the games that can be played in actual slot, in possible_assignations vector
		//we've all possible games that each umpire can play...
		//so, for the umpire number i we assign a priori match from its possible_assignations
		int total_assignations_to_actual_umpire = possible_assignations[umpire].size();
		//special case: the umpire only have one possible assignation
		if (total_assignations_to_actual_umpire == 1){
			if(potencial[possible_assignations[umpire][0]].local_team() != -1){
				match[umpire] = possible_assignations[umpire][0];
				Game dummy {-1,-1};
				potencial[possible_assignations[umpire][0]] = dummy;
			} else {
				DLOG(WARNING) << "Impossible to create a candidate for slot "<< actual_slot_ << " with gamma criterion"
				<< " to 'until_umpire' " << i << ". The problem was: empty possible_assignations vector, no more potencial games to play."; 
				return false;
			}
		} else {
			//we have more than one, first we need to check if we have a possible assignation for the umpire
			bool pass = false;
			for(int j=0; j < total_assignations_to_actual_umpire; j++)
				if(potencial[possible_assignations[umpire][j]].local_team() != -1)
					pass = true;
			if(!pass){
				DLOG(WARNING) << "Impossible to create a candidate for slot "<< actual_slot_ << " with gamma criterion"
				<< " to 'until_umpire' " << i << ". The problem was: empty possible_assignations vector, no more potencial games to play."; 
				return false;
			}
			//now we can continue...
			rnd_.create_uniform_int_dis(0, total_assignations_to_actual_umpire-1);
			int a_priori_match = rnd_.uniform_int_number();
			while(potencial[possible_assignations[umpire][a_priori_match]].local_team() == -1)
				a_priori_match = rnd_.uniform_int_number();
			//we found a possible a priori assignation
			match[umpire] = possible_assignations[umpire][a_priori_match];
			Game dummy {-1,-1};
			potencial[possible_assignations[umpire][a_priori_match]] = dummy;
		}
	}
	//at this point we have a candidate with 'until_umpire' assignations, we complete the rest with 
	//pi criterion and perfect weighted match, we need to count how many elements in the candidate we have
	int total = 0;
	for(int i=0; i < problem_instance_.number_of_umpires(); i++)
		if(match[i]!=-1)
			total++;
	int half = problem_instance_.number_of_umpires() / 2;
	//the other half not always is going to be the exact half (e.g. with 3 umpires)
	int other_half = problem_instance_.number_of_umpires() - half; 
	if(total < half){
		//we need to apply Pi criterion until half, with vector number_assignations we can know which umpires are missing
		int index_missings = until_umpire + (half-total);
		for(int i=until_umpire; i < index_missings; i++){
			double Pi = rnd_.uniform_real_number();
			if( Pi < 0.8 ){ //apply pheromone criterion
				
			} else {
				//apply minimum traveled distance criterion
			}
		}
	}
	
	//this code is probably wrong
	for(int i=0; i < until_umpire; i++){
		int umpire = number_assignations[i].first;
		int previous_team = schedule_[umpire][actual_slot_-1].local_team() - 1;
		//check which criterior of Pi we are going to apply
		double Pi = rnd_.uniform_real_number();
		if( Pi < 0.8){ //apply feromone criterion
			//TODO: implement code for selecting a move by the feromone
		} else { //apply minime traveled distance criterion
			pair<int, double> minimum {100, total_distance_}; //dummy initializators
			for(std::vector<Game>::size_type game = 0; game != potencial.size(); game++){
				if(check_restriction(potencial[game], umpire)){
					if(minimum.second > problem_instance_.distance_matrix()
						[previous_team][potencial[game].local_team()- 1]){
						minimum.first = game;
						minimum.second = problem_instance_.distance_matrix()
							[previous_team][potencial[game].local_team() - 1];
					}
				}
			}
			//so in minimum pair we have the assignation
			match[umpire] = minimum.first;
			//now we need to mark as deleted the potencial assignation recently assigned because is no available anymore.
			Game dummy {-1,-1};
			potencial[minimum.first] = dummy;
		}		
	}
	//at this point we have the gamma criterion satisfied, we know that we have until_umpire elements in our
	//match, if until_umpire is less than 50% of umpire, we need to full until 50% of match using the Pi criterion,
	//the other 50% is fulfilled using a perfect match with the remaining umpires.
}

//check assignation of game to actual umpire slot
bool Ant::check_restriction(Game game, int umpire)
{
	//for ghost assignations (check dummy game assignment in construct_match_from_gamma_criterion function)
	if(game.local_team() == -1) return false;
	//first check home restriction
	ListDigraph::Node umpire_node = get_umpire_node_from_slot(actual_slot_, umpire, slots_r1_);
	for (ListDigraph::OutArcIt a(restrictions_graph_, umpire_node); a != INVALID; ++a){
		//note that restrictions_graph_map_[restrictions_graph_.target(a)] represents a team
		if(restrictions_graph_map_[restrictions_graph_.target(a)] == game.local_team()){
			DLOG(WARNING) << "Can't assign game (" << game.local_team() << ", " << game.visit_team() <<
				") to umpire " << umpire << " in slot " << actual_slot_ << ", violates home restriction!";
			return false;
		}
	}
	
	//now check venue restriction
	ListDigraph::Node umpire_node2 = get_umpire_node_from_slot(actual_slot_, umpire, slots_r2_);
	for (ListDigraph::OutArcIt a(restrictions_graph_, umpire_node2); a != INVALID; ++a){
		//note that restrictions_graph_map_[restrictions_graph_.target(a)] represents a team
		int restricted_team = restrictions_graph_map_[restrictions_graph_.target(a)];
		if(restricted_team == game.local_team() || restricted_team == game.visit_team()){
			DLOG(WARNING) << "Can't assign game (" << game.local_team() << ", " << game.visit_team() <<
				") to umpire " << umpire << " in slot " << actual_slot_ << ", violates venue restriction!";
			return false;
		}
	}
	
	return true;
}

ListDigraph::Node Ant::get_umpire_node_from_slot(int slot, int umpire, const vector<ListDigraph::Node>& slot_vector)
{
	DLOG(INFO) << "Looking for an umpire node with restrictions for umpire " << umpire << " on slot " << slot; 
	ListDigraph::Node slot_node = slot_vector.at(slot);
	//now we need to find the umpire node
	for (ListDigraph::OutArcIt a(restrictions_graph_, slot_node); a != INVALID; ++a){
		if(restrictions_graph_map_[restrictions_graph_.target(a)] == umpire){
			DLOG(INFO) << "Founded umpire node " << umpire << " on slot " << slot << "!";
			return restrictions_graph_.target(a);
		}
	}
	DLOG(ERROR) << "Non Existent umpire node in slot " << slot << ". Maybe a misconstruction of restriction graph?";
	throw runtime_error("Non Existent umpire node in slot "+to_string(slot) +
		". Maybe a misconstruction of restriction graph?");
}

void Ant::set_random_generator(MersenneRandom& rnd)
{
	rnd_ = rnd;
}

const ListDigraph& Ant::restrictions_graph() const
{
	return restrictions_graph_;
}

const ListDigraph::NodeMap<int>& Ant::restrictions_graph_map() const
{
	return restrictions_graph_map_;
}

const multi_array<Game, 2>& Ant::schedule() const{
	return schedule_;
}

Ant::~Ant()
{
}
