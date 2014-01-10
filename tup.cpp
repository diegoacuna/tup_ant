/**
 * Ant+TUP - IA Avazada
 * tup.cpp
 * Purpose: Implementation of Tup class.
 * @author dacuna <dacuna@alumnos.inf.utfsm.cl>
 * @version 1.0 10/10/2013
 */

#include <glog/logging.h>
#include <boost/tokenizer.hpp>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <stdlib.h> 
#include "tup.h"

Tup::Tup(string filename, int home, int venue, int gamma) : home_restriction_(home), venue_restriction_(venue), gamma_(gamma)
{
	//read the config file and create parameters
	ifstream infile(filename);
	string line;
	int rowCounter = 0; //for counting the rows of distance and schedule matrix
	while (getline(infile, line)) {
		if(line[0]  == 'n') {
			//reading the number of teams
			try {
				number_of_teams_ = stoi(line.substr(7,1), nullptr);
				number_of_umpires_ = number_of_teams_ / 2;
				number_of_slots_ = 2*number_of_teams_ - 2;
				//resize matrix of distances and schedule
				teams_schedule_.resize(extents[number_of_umpires_][number_of_slots_]);
				distance_matrix_.resize(extents[number_of_teams_][number_of_teams_]);
				inverted_distance_matrix_.resize(extents[number_of_teams_][number_of_teams_]);
			} catch (const invalid_argument& ia) {
				infile.close();
				throw "Error reading instance file: the number of teams is not a number";
			}
		}
			if(line[0] == '[') {
				//reading a row from the distance matrix
				tokenizer<> tok(line);
				int columnCounter = 0;
				for(auto elem = tok.begin(); elem != tok.end(); ++elem) {
					try {
						distance_matrix_[rowCounter][columnCounter] = stod(*elem, nullptr);
						inverted_distance_matrix_[rowCounter][columnCounter] =
						    1/distance_matrix_[rowCounter][columnCounter];
						columnCounter++;
					} catch (const invalid_argument& ia) {
						throw "Error reading distance matrix: the number in row " + to_string(rowCounter) +
						          " and column " + to_string(columnCounter) + " is not a number";
					}
				}
				rowCounter = (rowCounter == number_of_teams_ -1) ? 0 : rowCounter + 1;
			} if(line[3] == '[') {
				//reading a row from opponents matrix
				int columnCounter = 0; //OBS: in this time column gets inverted with row!
				string buf;
				stringstream ss(line);
				vector<string> tok;
				while (ss >> buf)
					tok.push_back(buf);
				vector<int> lecture;
				vector<bool> readed;
				for(auto elem = tok.begin(); elem != tok.end(); ++elem) {
					try {
						if((*elem).compare(tok.front()) == 0)
							lecture.push_back(stoi((*elem).substr(1), nullptr));
						else
							lecture.push_back(stoi(*elem, nullptr));
						readed.push_back(false);
					} catch (const invalid_argument& ia) {
						throw "Error reading opponents matrix: the number in row " + to_string(columnCounter) +
						          " and column " + to_string(rowCounter) + " is not a number";
					}
				}
				//insert into the opponents matrix
				for(std::vector<int>::size_type i = 0; i != lecture.size(); i++) {
					int local = (lecture[i] > 0) ? i+1 : abs(lecture[i]);
					int visit = (lecture[i] < 0) ? i+1 : lecture[i];
					if(!readed[visit-1] && !readed[local-1]){					
						Game game {local, visit, columnCounter};
						teams_schedule_[columnCounter][rowCounter] = game;
						readed[visit-1] = readed[local-1] = true;
						columnCounter++;
					}
				}
				rowCounter++;
		}
	}
}

void Tup::set_problem_name(const string& problem_name_) {
	this->problem_name_ = problem_name_;
}

const string& Tup::problem_name() const {
	return problem_name_;
}

void Tup::set_number_of_umpires(int number_of_umpires_) {
	this->number_of_umpires_ = number_of_umpires_;
}

int Tup::number_of_umpires() const {
	return number_of_umpires_;
}

void Tup::number_of_slots(int number_of_slots_) {
	this->number_of_slots_ = number_of_slots_;
}

int Tup::number_of_slots() const {
	return number_of_slots_;
}

void Tup::set_number_of_teams(int number_of_teams_) {
	this->number_of_teams_ = number_of_teams_;
}

int Tup::number_of_teams() const {
	return number_of_teams_;
}

void Tup::set_home_restriction(int home_restriction){
	this->home_restriction_ = home_restriction;
}
	
int Tup::home_restriction() const{
	return home_restriction_;
}

void Tup::set_venue_restriction(int venue_restriction){
	this->venue_restriction_ = venue_restriction;
}

int Tup::venue_restriction() const{
	return venue_restriction_;
}

void Tup::set_gamma(int gamma){
	this->gamma_ = gamma;
}

int Tup::gamma() const{
	return gamma_;
}

const multi_array<double, 2>& Tup::distance_matrix() const{
	return distance_matrix_;
}

const multi_array<double, 2>& Tup::inverted_distance_matrix() const{
	return inverted_distance_matrix_;
}
	
const multi_array<Game, 2>& Tup::teams_schedule() const{
	return teams_schedule_;
}

Tup::~Tup()
{
}
