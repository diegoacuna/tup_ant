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

Ant::Ant(int id, Tup& tup, int K, int seed) : 
id_(id),
actual_slot_(0), 
restrictions_graph_map_(restrictions_graph_), 
problem_instance_(tup),
seed_(seed),
candidates_list_size_(K)
{
	//create the random generator
	rnd_.initialize_generator((unsigned int) (seed*id));
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
	
	//move();
}

void Ant::prepare_restrictions_graph()
{
	DLOG(INFO) << "Creating restrictions graph (with r = " << problem_instance_.number_of_slots() << " slots)...";
	for(int i=0; i < problem_instance_.number_of_slots(); i++){
		slots_r1_.push_back(restrictions_graph_.addNode());
		restrictions_graph_map_[slots_r1_.back()] = i;
		slots_r2_.push_back(restrictions_graph_.addNode());
		restrictions_graph_map_[slots_r2_.back()] = i;
		//DLOG(INFO) << "Assigning umpire nodes (= " << problem_instance_.number_of_umpires() << ") to slot node " << i;
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
	//DLOG(INFO) << "Assigned match (" << game.local_team() << ", " << game.visit_team() << 
	//	") to umpire " << umpire << " in the slot " << actual_slot_;
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
	//maybe start + problem_instance_.home_restriction() (who represents future 
	//slots where we need to apply the restriction) is greater than the maximum
	//slot available. Ej: home_restriction()= 3 and we're in slot 8, then start
	//is goint to be first 9, then 10 and in this case the algorithm is going
	//to break because the maximun slot is 9 (0-9 for 10 slots). So, summarizing
	//we need to check that i is going to be less than the maximum slot
	int max_slot = start + problem_instance_.home_restriction() - 1;
	//choose the min from start, maximum slot
	max_slot = (max_slot > problem_instance_.number_of_slots()-1) 
				? problem_instance_.number_of_slots()  
				: max_slot;
	for(int i = start; i < max_slot; i++){
		//add arcs from the correponding slots and the corresponding umpire
		ListDigraph::Node umpire_slot = get_umpire_node_from_slot(i, umpire, slots_r1_);
		restrictions_graph_.addArc(umpire_slot, hres);
		//DLOG(INFO) << "Adding home restriction for team " << game.local_team() << " in slot " << i << " to umpire " 
		//	<< restrictions_graph_map_[umpire_slot];
	}
	//process venue restriction
	ListDigraph::Node vres = restrictions_graph_.addNode();
	restrictions_graph_map_[vres] = game.visit_team();
	bool add_r2 = false;
	max_slot = start + problem_instance_.venue_restriction() - 1;
	//choose the min from start, maximum slot
	max_slot = (max_slot > problem_instance_.number_of_slots()-1) 
				? problem_instance_.number_of_slots()  
				: max_slot;
	for(int i = start; i < max_slot; i++){
		//add arcs from the correponding slots
		ListDigraph::Node umpire_slot = get_umpire_node_from_slot(i, umpire, slots_r2_);
		restrictions_graph_.addArc(umpire_slot, vres);
		restrictions_graph_.addArc(umpire_slot, hres);
		//DLOG(INFO) << "Adding venue restriction for team " << game.local_team() << " in slot " << i << " to umpire " 
		//	<< restrictions_graph_map_[umpire_slot];;
		//DLOG(INFO) << "Adding venue restriction for team " << game.visit_team() << " in slot " << i << " to umpire " 
		//	<< restrictions_graph_map_[umpire_slot];;
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
	distance_actual_slot += distance;
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
		//int previous_team = schedule_[umpire][actual_slot_-1].local_team() - 1;
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
	//START THE GAMMA CRITERION
	DLOG(INFO) << "STARTING THE GAMMA CRITERION FOR SLOT " << actual_slot_;
	//now, traverse the number_assignations vector and watch which umpire has less possible assignations than permited
	int umpires_gamma_criterion = -1;
	for (std::vector<pair<int, int>>::size_type i = 0; i != number_assignations.size(); i++){
		int percentage = (number_assignations[i].second * 100) / problem_instance_.number_of_umpires();
		if(percentage > problem_instance_.gamma()){
			umpires_gamma_criterion = i;
			break;
		}
	}
	//in umpires_gamma_criterion we have the total number of umpires who breaks 
	//the gamma criterion, to that list of umpires we apply the criterion
	//TODO: bug, if umpires_gamma_criterion == -1 then all umpires breaks gamma
	//criterion, at this time we're doing nothing with this, if umpires_gamma_criterion
	//==-1 then we pass (and not apply gamma criterion to any umpires) which is
	//clearly wrong, in that case we need to apply gamma criterion to all umpires
	if(umpires_gamma_criterion != -1){
		//we can create umpires_gamma_criterion candidates...
		for(int i=0; i < umpires_gamma_criterion; i++){
			vector<int> candidate(problem_instance_.number_of_umpires(), -1);
			if(construct_match_from_gamma_criterion(potencial, number_assignations, 
				possible_assignations, i, candidate)){
				//we have a match, need to copy to the candidate list
				candidates.push_back(candidate);
			}
		}
	} 
	DLOG(INFO) << "END THE GAMMA CRITERION FOR SLOT " << actual_slot_;
	
	//we need to complete the candidate list
	//TODO: mean while K is going to be equal to the number of teams... probably 
	//its better to let it be a parameter
	DLOG(INFO) << "STARTING THE MIXED CRITERION FOR SLOT " << actual_slot_;
	if(candidates.size() < (size_t)problem_instance_.number_of_teams()){
		//we are going to complete a list of size K = number of teams using the 
		//Pi criterion + perfect weigthed match
		rnd_.create_uniform_real_dis(0.0, 1.0);
		for(auto i=candidates.size();i<(size_t)problem_instance_.number_of_teams()+1;i++){
			//sometimes we cannot create the candidate so we need a mechanism to know 
			//when no to go on on trying to create this candidate
			bool preserve_candidate = true; //a priori of course we can create a candidate
			//CONSTRUCT A COMPLETE CANDIDATE, 50% WITH PI CRITERION AND 50% WITH PERFECT WEIGHTED MATCH
			vector<Game> potencial_pi = potencial;
			vector< pair<int, int> > traveled_distance_pi = traveled_distance;
			vector<int> match(problem_instance_.number_of_umpires(), -1);
			//First, construct the initial 50% of the candidate
			for(int j=0;j<problem_instance_.number_of_umpires()/2;j++){
				double Pi = rnd_.uniform_real_number();
				if( Pi < 0.8 ){
					//first, choose a good distance umpire (from the first half 
					//of traveled_distance vector
					int choosed_umpire = -1;
					//in the worst case the index is going to be the first umpire
					int index_umpire = 0; 
					//the second comparision is needed because we need to know
					//if the umpire is available to use it
					if(traveled_distance_pi.size() == 1 && traveled_distance_pi
						[index_umpire].first != -1) 
						choosed_umpire = traveled_distance[0].first;
					else {
						int half = traveled_distance_pi.size() / 2; //doesn't matter where the half start...
						rnd_.create_uniform_int_dis(0, half-1);
						index_umpire = rnd_.uniform_int_number();
						while(traveled_distance_pi[index_umpire].first == -1){
							index_umpire = rnd_.uniform_int_number();
							//DLOG(INFO) << "Looking for an index_umpire in traveled_distance vector...";
						}
						choosed_umpire = traveled_distance_pi[index_umpire].first;
					}
					//we need to create a list of actual possible game candidates for choosed umpire
					vector<int> actual_possible_games;
					for(int assignation : possible_assignations[choosed_umpire]){
						if(potencial_pi[assignation].local_team() != -1)
							actual_possible_games.push_back(assignation);
					}
					if(actual_possible_games.size() == 0){
						//this is a case where we can't go on constructing this
						//candidate, so preserve_candidate should go false
						preserve_candidate = false;
					} else {
						//calculate pheromones and determine which game we play
						int which_game = calculatePheromoneForGameList(choosed_umpire, 
							actual_possible_games, potencial_pi);
						match[choosed_umpire] = actual_possible_games[which_game];
						Game dummy {-1,-1};
						potencial_pi[actual_possible_games[which_game]] = dummy;
						traveled_distance_pi[index_umpire].first = -1;						
					}
				} else {
					//apply minimum traveled distance criterion
					//first, choose a bad distance umpire (from the second half of traveled_distance vector
					int choosed_umpire = -1;
					int index_umpire = -1;
					if(traveled_distance_pi.size() == 1){
						choosed_umpire = traveled_distance_pi[0].first;
						index_umpire = 0;
					} else {
						int half = traveled_distance_pi.size() / 2; //doesn't matter where the half start...
						rnd_.create_uniform_int_dis(half-1, traveled_distance_pi.size()-1);
						index_umpire = rnd_.uniform_int_number();
						while(traveled_distance_pi[index_umpire].first == -1){
							index_umpire = rnd_.uniform_int_number();
							//DLOG(INFO) << "Looking for an index_umpire in traveled_distance vector...";
						}
						choosed_umpire = traveled_distance_pi[index_umpire].first;
					}
					//We have an umpire...
					//now select a match who minimices traveled distance
					int minimum_assignation = -1;
					double min_distance = distance_by_umpire_[choosed_umpire]*1000; //dummy
					int previous_team = schedule_[choosed_umpire][actual_slot_-1].local_team() - 1;
					for(int assignation : possible_assignations[choosed_umpire]){
						if(potencial_pi[assignation].local_team() != -1){
							if(problem_instance_.distance_matrix()[previous_team][potencial_pi[assignation].local_team() - 1] < min_distance)
								minimum_assignation = assignation;
						}
					}//END FIND THE MINIMAL DISTANCE MATCH
					if(minimum_assignation == -1){
						DLOG(WARNING) << "Impossible to create a candidate for slot "<< actual_slot_ << " with gamma criterion"
						<< " in phase Pi criterion (phase to create until K candidates) with umpire " << choosed_umpire << ". The problem was: empty possible_assignations vector, no more potencial games to play."; 
						continue;
					}
					//make the assignation, minimum assignations contains the 
					//element in vector potencial_pi to use (its index)
					match[choosed_umpire] = minimum_assignation;
					Game dummy {-1,-1};
					potencial_pi[minimum_assignation] = dummy;
					traveled_distance_pi[index_umpire].first = -1;
				} //END PI CRITERION WITH PI > 0.8
			}
			//check the preserve_candidate status
			if(!preserve_candidate) continue; //we can't go on...
			
			//now we need to create the rest of the candidate with a perfect 
			//weighted match to know which umpires to bound we look the match 
			//vector (umpires with match -1 are not assigned)
			//START PERFECT WEIGHTED MATCH FOR THE 50% REST
			ListGraph bpMatch;
			ListGraph::EdgeMap<double> weight(bpMatch); //weights of the graph
			ListGraph::NodeMap<int> labels(bpMatch);
			vector<ListGraph::Node> bpA; //first partition (umpires)
			vector<ListGraph::Node> bpB; //second partition (games)
			
			//add umpires
			for(int i=0; i < problem_instance_.number_of_umpires(); i++){
				bpA.push_back(bpMatch.addNode());
				labels[bpA.back()] = i;
			}
			//add matches
			for(std::vector<Game>::size_type i=0; i != potencial_pi.size(); i++){
				bpB.push_back(bpMatch.addNode());
				labels[bpB.back()] = i;
			}	
			//now, for each umpire check if we can join the umpire node with a node in bpB
			for(int umpire=0; umpire < problem_instance_.number_of_umpires(); umpire++){
				if(match[umpire]!=-1){
					//TODO: add the edge with the already instanciated match
					int assignation = match[umpire];
					ListGraph::Edge e = bpMatch.addEdge(bpA[umpire], bpB[assignation]);
					weight[e] = 1;
					continue;
				}
				//join possible assignations (games) with umpires
				int previous_team = schedule_[umpire][actual_slot_-1].local_team() - 1;
				for(int assignation : possible_assignations[umpire]){
					if(potencial_pi[assignation].local_team() != -1){
						ListGraph::Edge e = bpMatch.addEdge(bpA[umpire], bpB[assignation]);
						weight[e] = 1 / problem_instance_.distance_matrix()[previous_team]
							[potencial_pi[assignation].local_team()-1];
					}
				}
			} //END JOINING NODES
			
			//we have the full graph ready to search for a perfect matching...
			MaxWeightedPerfectMatching<ListGraph, ListGraph::EdgeMap<double> > 
				matching(bpMatch, weight);
			if (!matching.run()){
				DLOG(WARNING) << "No perfect matching found for 50% in " << 
				  "construction of candidate list in slot " << actual_slot_;
				continue;
			}
			DLOG(INFO) << "Perfect Matching found with weight " << matching.matchingWeight();
			//with the mate() function of MaxWeightedPerfectMatching we can obtain the matching
			for(ListGraph::Node umpire : bpA){
				ListGraph::Node game = matching.mate(umpire);
				int which_umpire = labels[umpire];
				if(match[which_umpire] != -1) continue; //pass...
				//make the assignment
				match[which_umpire] = labels[game];
				Game dummy {-1,-1};
				potencial_pi[labels[game]] = dummy;
			}
			if(find(candidates.begin(), candidates.end(), match) == candidates.end())
				candidates.push_back(match);
			//END APPLY PERFECT WEIGHTED MATCH FOR THE 50% REST OF THE CANDIDATE LIST
		}//END FOR PI CRITERION + PERFECT WEIGHTED MATCH TO CREATE UNTIL K CANDIDATES
	}
	DLOG(INFO) << "END THE MIXED CRITERION FOR SLOT " << actual_slot_;
	
	//now we need to select which candidate choose and fill the schedule
	if(candidates.size() == 0){
		this->id_ = -1;
		return;
	}
	rnd_.create_uniform_int_dis(0, candidates.size()-1);
	int which_candidate = rnd_.uniform_int_number();
	//int which_candidate = calculatePheromoneForCandidateList(candidates);
	//DEBUG
	if(which_candidate == 0)
		DLOG(INFO) << "Choosed perfect match in slot " << actual_slot_;
	//so we select candidate 'wich_candidate', now fill the schedule and update
	//the distance of each umpire. First get each game and assign, then update distance
	int index_umpire = 0;
	distance_actual_slot = 0.0; //also we calculate the distance of actual slot
	for(int game : candidates[which_candidate]){
		Game instance_game = potencial[game];
		assign_match_to_umpire(instance_game, index_umpire);
		//update distance
		update_distance(index_umpire);
		index_umpire++;
	}
		
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
	
	//all the umpires are going to participate in the match
	for(int i=0; i < problem_instance_.number_of_umpires(); i++){
		bpA.push_back(bpMatch.addNode());
		labels[bpA.back()] = i;
	}
	//also all games will fall into the match
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
	//so the match vector contains in each row the value of wich game is going 
	//to play the umpire with key i, i.e. match[i] = j means the umpire i is
	//going to play the game j (in vector potencial)
	
	return true;
}

bool Ant::construct_match_from_gamma_criterion(vector<Game> potencial, const vector< pair<int, int> >& 
		number_assignations, const vector< vector<int> >& possible_assignations, int until_umpire, vector<int>& match){
	if(until_umpire == 0) //ERROR CHECKING
		return false;
	
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
	//int other_half = problem_instance_.number_of_umpires() - half; 
	//we need to apply Pi criterion until half, with vector number_assignations we can know which umpires are missing
	int index_missings = until_umpire + (half-total);
	
	//TODO: THIS BLOCK HAS BEEN REMOVED
	
	//START PERFECT WEIGHTED MATCH FOR THE 50% REST
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
		if(match[umpire]!=-1){
			int assignation = match[umpire];
			ListGraph::Edge e = bpMatch.addEdge(bpA[umpire], bpB[assignation]);
			weight[e] = 1;
			continue;
		}
		//join possible assignations (games) with umpires
		int previous_team = schedule_[umpire][actual_slot_-1].local_team() - 1;
		for(int assignation : possible_assignations[umpire]){
			if(potencial[assignation].local_team() != -1){
				ListGraph::Edge e = bpMatch.addEdge(bpA[umpire], bpB[assignation]);
				weight[e] = 1 / problem_instance_.distance_matrix()[previous_team]
					[potencial[assignation].local_team()-1];
			}
		}
	} //END JOINING NODES
	
	//we have the full graph ready to search for a perfect matching...
	MaxWeightedPerfectMatching<ListGraph, ListGraph::EdgeMap<double> > matching(bpMatch, weight);
	if (!matching.run()){
		DLOG(WARNING) << "No perfect matching found for 50% in construction of candidate list for until umpire " 
		<< until_umpire << " in slot " << actual_slot_;
		return false;
	}
	DLOG(INFO) << "Perfect Matching found with weight " << matching.matchingWeight();
	//with the mate() function of MaxWeightedPerfectMatching we can obtain the matching
	for(ListGraph::Node umpire : bpA){
		ListGraph::Node game = matching.mate(umpire);
		int which_umpire = labels[umpire];
		//make the assignment
		match[which_umpire] = labels[game];
		Game dummy {-1,-1};
		potencial[labels[game]] = dummy;
	}
	//END APPLY PERFECT WEIGHTED MATCH FOR THE 50% REST OF THE CANDIDATE LIST
	
	//At this point we have a complete candidate to travel, good news :D
	return true;
	
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
			//DLOG(WARNING) << "Can't assign game (" << game.local_team() << ", " << game.visit_team() <<
			//	") to umpire " << umpire << " in slot " << actual_slot_ << ", violates home restriction!";
			return false;
		}
	}
	
	//now check venue restriction
	ListDigraph::Node umpire_node2 = get_umpire_node_from_slot(actual_slot_, umpire, slots_r2_);
	for (ListDigraph::OutArcIt a(restrictions_graph_, umpire_node2); a != INVALID; ++a){
		//note that restrictions_graph_map_[restrictions_graph_.target(a)] represents a team
		int restricted_team = restrictions_graph_map_[restrictions_graph_.target(a)];
		if(restricted_team == game.local_team() || restricted_team == game.visit_team()){
			//DLOG(WARNING) << "Can't assign game (" << game.local_team() << ", " << game.visit_team() <<
			//	") to umpire " << umpire << " in slot " << actual_slot_ << ", violates venue restriction!";
			return false;
		}
	}
	
	return true;
}

ListDigraph::Node Ant::get_umpire_node_from_slot(int slot, int umpire, const vector<ListDigraph::Node>& slot_vector)
{
	//DLOG(INFO) << "Looking for an umpire node with restrictions for umpire " << umpire << " on slot " << slot; 
	ListDigraph::Node slot_node = slot_vector.at(slot);
	//now we need to find the umpire node
	for (ListDigraph::OutArcIt a(restrictions_graph_, slot_node); a != INVALID; ++a){
		if(restrictions_graph_map_[restrictions_graph_.target(a)] == umpire){
			//DLOG(INFO) << "Founded umpire node " << umpire << " on slot " << slot << "!";
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

int Ant::calculatePheromoneForCandidateList(vector<vector<int> >& candidateList)
{
	//first we need to sum up all the pheromone for the candidate list and de visibility
	double totalPheromone = 0.0;
	//at the same time we are going to calculate pheromone for each candidate
	vector<double> pheromone_candidate;
	//also get total visibility and each candidate visibility
	double total_visibility = 0.0;
	vector<double> visibility_candidate;
	for(auto candidate : candidateList){
		double pheromone_actual_candidate = 0.0;
		double visibility_actual_candidate = 0.0;
		for(int umpire = 0; umpire < problem_instance_.number_of_umpires(); umpire++){
			int game = candidate[umpire];
			//we have the i,j,k index => umpire, slot, game
			totalPheromone += (*colony_pheromone_)[umpire][actual_slot_][game];
			pheromone_actual_candidate += (*colony_pheromone_)[umpire][actual_slot_][game];
			visibility_actual_candidate += problem_instance_.
				inverted_distance_matrix()
				[schedule_[umpire][actual_slot_-1].local_team()-1]
				[problem_instance_.teams_schedule()[game][actual_slot_].local_team()-1];
			total_visibility += visibility_actual_candidate;
		}
		pheromone_candidate.push_back(pheromone_actual_candidate);
		visibility_candidate.push_back(visibility_actual_candidate);
	}
	
	//ok, now we can calculate the P_{IJ} for each candidate
	vector<double> pheromone_relative;
	for(vector< vector<int> >::size_type i=0; i < candidateList.size(); i++){
		double calculation = (pow(pheromone_candidate[i], alpha_) *
		                     pow(visibility_candidate[i], beta_) ) /
							 (pow(totalPheromone, alpha_) * 
							 pow(total_visibility, beta_));
		pheromone_relative.push_back(calculation);
	}
	
	//in pheromone_relative we have relative probabilities, now calculate cumulatives
	vector<double> pheromone_cumulative;
	int size_list = 0;
	for(vector<int>::size_type i = 0; i < candidateList.size(); i++){
		if(i==0)
			pheromone_cumulative.push_back(pheromone_relative[0]);
		else
			pheromone_cumulative.push_back(pheromone_cumulative[i-1]
				+ pheromone_relative[i]);
		size_list++;
	}
	
	//now, spin the roulette and choose the game, we return the number of the
	//game which got elected
	rnd_.create_uniform_real_dis(0.0, 1.0);
	double p=rnd_.uniform_real_number();
	if(p < pheromone_cumulative[0])
		return 0;
	else
	{
		for(int j=0; j < size_list-1;j++)
		{
			if(p >= pheromone_cumulative[j] && p < pheromone_cumulative[j+1])
				return j+1;
		}
		if(p >= pheromone_cumulative[size_list-1])
			return size_list-1;
	}
	
	//never is going to ocurre but to avoid C++ warnings
	return 0;
}

int Ant::calculatePheromoneForGameList(int umpire, const vector<int>& potencial_games, 
	const vector<Game>& games)
{
	//the idea of this function is to calculate pheromone to know which game to
	//select for the umpire, so at the end we're going to have a list with 
	//pheromone value to each of the potencial games to play (for the umpire)...
	//this is like the real approach to pheromone in Dorigo ant system

	//first we need to sum up all the pheromone for the candidate list and the visibility
	double total_pheromone = 0.0;
	//also get total visibility and each candidate visibility
	double total_visibility = 0.0;
	for(auto candidate : potencial_games){
		//the umpire is 'umpire', the slot is 'slot' the game is 'candidate', but
		total_pheromone += (*colony_pheromone_)[umpire][actual_slot_][candidate];
		total_visibility += problem_instance_.
				inverted_distance_matrix()
				[schedule_[umpire][actual_slot_-1].local_team()-1]
				[problem_instance_.teams_schedule()[candidate][actual_slot_].local_team()-1];
	}
	
	//ok, now we can calculate the P_{IJ} for each candidate
	vector<double> pheromone_relative;
	for(auto candidate : potencial_games){
		//the visibility for the actual umpire, slot, team (candidate)
		double visibility = problem_instance_.
				inverted_distance_matrix()
				[schedule_[umpire][actual_slot_-1].local_team()-1]
				[problem_instance_.teams_schedule()[candidate][actual_slot_].local_team()-1];
				
		double calculation = (pow((*colony_pheromone_)[umpire][actual_slot_][candidate], alpha_) *
		                     pow(visibility, beta_) ) /
							 (pow(total_pheromone, alpha_) * 
							 pow(total_visibility, beta_));
		pheromone_relative.push_back(calculation);
	}
	
	//in pheromone_relative we have relative probabilities, now calculate cumulatives
	vector<double> pheromone_cumulative;
	int size_list = 0;
	for(vector<int>::size_type i = 0; i < potencial_games.size(); i++){
		if(i==0)
			pheromone_cumulative.push_back(pheromone_relative[0]);
		else
			pheromone_cumulative.push_back(pheromone_cumulative[i-1]
				+ pheromone_relative[i]);
		size_list++;
	}
	
	//now, spin the roulette and choose the game, we return the number of the
	//game which got elected
	rnd_.create_uniform_real_dis(0.0, 1.0);
	double p=rnd_.uniform_real_number();
	if(p < pheromone_cumulative[0])
		return 0;
	else
	{
		for(int j=0; j < size_list-1;j++)
		{
			if(p >= pheromone_cumulative[j] && p < pheromone_cumulative[j+1])
				return j+1;
		}
		if(p >= pheromone_cumulative[size_list-1])
			return size_list-1;
	}
	//never is going to ocurre but to avoid C++ warnings
	return 0;
}

void Ant::setAlphaBeta(double alpha, double beta)
{
	this->alpha_ = alpha;
	this->beta_ = beta;
}

double Ant::get_distance_actual_slot()
{
	return this->distance_actual_slot;
}

double Ant::get_total_distance()
{
	return this->total_distance_;
}

int Ant::id()
{
	return id_;
}
