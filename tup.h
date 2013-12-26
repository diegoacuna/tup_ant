/**
 * Ant+TUP - IA Avazada
 * tup.h
 * Purpose: Class representing an instance of the traveling umpire problem.
 * @author dacuna <dacuna@alumnos.inf.utfsm.cl>
 * @version 1.0 10/10/2013
 */

#ifndef TUP_H
#define TUP_H

#include <string>
#include <fstream>
#include <boost/multi_array.hpp>
#include "game.h"

using namespace std;
using namespace boost;

/**
 * Represents an instance of the traveling umpire problem.
 *
 * Allows others entities in the application to access information about
 * the tup's instance used in the ejecution of the algorithm.
 *
 * @author dacuna <dacuna@alumnos.inf.utfsm.cl>
 * @date 10/10/2013
 */
class Tup
{
private:
	string problem_name_;							   /**<Name of the tup instance to solve */
	int number_of_umpires_;                            /**<Number of umpires in the tournament */
	int number_of_slots_;                              /**<Number of slots in the tournament */
	int number_of_teams_;                              /**<Number of teams in the tournament */
	int home_restriction_;							   /**<Restriction for home venues (n - d1) */
	int venue_restriction_;							   /**<Restriction for any venue restriction (n/2 - d2) */
	int gamma_;            /**<Percentage of possible assignations for an umpire to consider him blocked */
	multi_array<Game, 2> teams_schedule_;              /**<Schedule of the entire tournaments grouped by slot */
	multi_array<double, 2> distance_matrix_;           /**<Distances from between all pairs of cities in the tournament */
	multi_array<double, 2> inverted_distance_matrix_;  /**<Distance_matrix_[i,j]^(-1) for all i,j in the tournament */							   /**<File stream to read the instance file */
public:
	/**
	 * Class constructor.
	 *
	 * @param filename string with the route to the tup instance file
	 * @param home Restriction for home venues (n - d1)
	 * @param venue Restriction for any venue restriction (n/2 - d2)
	 */
	Tup(string filename, int home, int venue, int gamma);
	
	void set_problem_name(const string& problem_name_);
	
	const string& problem_name() const;
	
	void set_number_of_umpires(int number_of_umpires_);
	
	int number_of_umpires() const;
	
	void number_of_slots(int number_of_slots_);
	
	int number_of_slots() const;
	
	void set_number_of_teams(int number_of_teams_);
	
	int number_of_teams() const;
	
	void set_home_restriction(int home_restriction);
	
	int home_restriction() const;
	
	void set_venue_restriction(int venue_restriction);
	
	int venue_restriction() const;
	
	void set_gamma(int gamma);
	
	int gamma() const;
	
	const multi_array<double, 2>& distance_matrix() const;
	
	const multi_array<double, 2>& inverted_distance_matrix() const;
	
	const multi_array<Game, 2>& teams_schedule() const;

	/**
	 * Class constructor.
	 */
	~Tup();

};

#endif // TUP_H
