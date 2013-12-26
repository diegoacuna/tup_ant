/** @mainpage TUP+Ant: Solver of traveling umpire problem using an ant based algorithm.
*   @par Description:
*   - This program solves the traveling umpire problem, a relative new combinatorial
* 	  problem wich arise in the context of sports scheduling. As far as we know there's
*     no an existing approach based on an ant algorithm to solve this problem.
*	@par Remarks:
*    - For detailed instruction to build and execute the application, refer to README document.
*    - External libraries used: Boost (for matrix_array and tokenizers), Lemon (for graph data
* 	   structures and matching algorithms), Google Glog (for logging purposes) and TCLAP (for 
* 	   command line argument parsing).
*	@par Author:
*    - Diego Acuna R. <dacuna@alumnos.inf.utfsm.cl>
*    - Date: October XX, 2013
*/

/**
 * Ant+TUP - IA Avazada
 * main.cpp
 * Purpose: Frontend file for the entire algorithm.
 * @author dacuna <dacuna@alumnos.inf.utfsm.cl>
 * @version 1.0 12/10/2013
 */
#include <stdio.h>
#include <string>
#include <glog/logging.h>
#include <lemon/list_graph.h>
#include <tclap/CmdLine.h>
#include "tup.h"
#include "mersenne_random.h"
#include "ant.h"

using namespace lemon;
using namespace std;

int main(int argc, char **argv)
{
	FLAGS_logtostderr = 1;
	google::InitGoogleLogging(argv[0]);
	
	try {
		TCLAP::CmdLine cmd("Ant System TSP: Traveling salesman problem solved by AS", ' ', "1.1");
		//params for ant algorithm
		TCLAP::ValueArg<double> p_arg("p","persistence","Trail persistence of ant's pheromone",false,0.6,"double",cmd);
		TCLAP::ValueArg<double> alpha_arg("a","alpha","Relative importance of trail",false,0.1,"double",cmd);
		TCLAP::ValueArg<double> beta_arg("b","beta","Relative importance of visibility",false,0.1,"double",cmd);
		TCLAP::ValueArg<double> q_arg("q","q","Quantity of trail laid by ants",false,0.1,"double",cmd);
		TCLAP::ValueArg<int> seed_arg("s","seed","Random seed for random number generation",false,100,"int",cmd);
		//params for tup
		TCLAP::ValueArg<string> instance_arg("i","instance","File with TSPLIB instance (only euclidean chords)",
			true,"","string",cmd);
		TCLAP::ValueArg<int> home_arg("o","home","Home restriction (n-d1)",false,0,"int",cmd);
		TCLAP::ValueArg<int> venue_arg("e","venue","Any venue restriction (n/2 - d2)",false,0,"int",cmd);
		//params for my proposal of algorithm
		TCLAP::ValueArg<int> gamma_arg("g","gamma","Percentage of possible assignations for an umpire to consider him blocked",false,20,"int",cmd);
		TCLAP::ValueArg<int> k_arg("k","candidates-list","Size of candidates list",false,5,"int",cmd);
		//general params
		TCLAP::SwitchArg debug_arg("d","debug","Show debug information", cmd, false);
		
		// Parse the argv array.
		cmd.parse(argc, argv);
		bool debug = debug_arg.getValue();
		double p = p_arg.getValue();
		double alpha = alpha_arg.getValue();
		double beta = beta_arg.getValue();
		double q = q_arg.getValue();
		//End get command line parameters
		
		Tup problem {instance_arg.getValue(), home_arg.getValue(), venue_arg.getValue(), gamma_arg.getValue()};
		MersenneRandom rnd {(unsigned int) seed_arg.getValue()};
		Ant test {problem, k_arg.getValue()};
		
	} catch (TCLAP::ArgException &e) { 
		LOG(FATAL) << "error: " << e.error() << " for arg " << e.argId();
	} catch(const char* msg) {
		LOG(FATAL) << "The application has ended with the following errors: " << msg;
	}

}
