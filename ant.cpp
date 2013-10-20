/**
 * Ant+TUP - IA Avazada
 * ant.cpp
 * Purpose: Implementation of Ant class.
 * @author dacuna <dacuna@alumnos.inf.utfsm.cl>
 * @version 1.0 10/10/2013
 */

#include <glog/logging.h>
#include <stdexcept>
#include "ant.h"

Ant::Ant(Tup& tup) : actual_slot_(0), restrictions_graph_map_(restrictions_graph_), problem_instance_(tup)
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
			restrictions_graph_.addArc(slots_r2_.back(), ump);
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
	//in the next vector we're going to store the lista of candidates
	vector< vector<int> > candidates;
	
	construct_perfect_match(potencial);
	
	//finally move to the next round
	actual_slot_++;
}

void Ant::construct_perfect_match(const vector<Game>& potencial)
{
	ListDigraph bpMatch;
	vector<ListDigraph::Node> bpA; //first partition (umpires)
	vector<ListDigraph::Node> bpB; //second partition (games)
	for(int i=0; i < problem_instance_.number_of_umpires(); i++)
		bpA.push_back(bpMatch.addNode());
	for(int i=0; i < potencial.size(); i++)
		bpB.push_back(bpMatch.addNode());
		
	//now, for each umpire check if we can join the umpire node with a node in bpB
	for(int i=0; i < problem_instance_.number_of_umpires(); i++){
		for(const Game& game : potencial){
			check_restriction(game, i);
		}
	}
	
}

//check assignation of game to actual umpire slot
bool Ant::check_restriction(Game game, int umpire)
{
	//first check home restriction
	ListDigraph::Node umpire_node = get_umpire_node_from_slot(actual_slot_, umpire, slots_r1_);
	for (ListDigraph::OutArcIt a(restrictions_graph_, umpire_node); a != INVALID; ++a){
		//note that restrictions_graph_map_[restrictions_graph_.target(a)] represents a team
		if(restrictions_graph_map_[restrictions_graph_.target(a)] == game.local_team()){
			DLOG(WARNING) << "Can't assign game (" << game.local_team() << ", " << game.visit_team() <<
				" to umpire " << umpire << " in slot " << actual_slot_ << ", violates home restriction!";
			return false;
		}
	}
	
	//now check venue restriction
	umpire_node = get_umpire_node_from_slot(actual_slot_, umpire, slots_r2_);
	for (ListDigraph::OutArcIt a(restrictions_graph_, umpire_node); a != INVALID; ++a){
		//note that restrictions_graph_map_[restrictions_graph_.target(a)] represents a team
		int restricted_team = restrictions_graph_map_[restrictions_graph_.target(a)];
		if(restricted_team == game.local_team() || restricted_team == game.visit_team()){
			DLOG(WARNING) << "Can't assign game (" << game.local_team() << ", " << game.visit_team() <<
				" to umpire " << umpire << " in slot " << actual_slot_ << ", violates venue restriction!";
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
