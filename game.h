/**
 * Ant+TUP - IA Avazada
 * game.h
 * Purpose: Class representing an instance of a game in a tournament.
 * @author dacuna <dacuna@alumnos.inf.utfsm.cl>
 * @version 1.0 10/10/2013
 */

#ifndef GAME_H
#define GAME_H

/** 
 * Represents an instance of game in a sports tournament.
 *
 * A game is constituted by a local team and a visitor team. For the sake of simplicity
 * a team is not molded by a different class, this is because by knowing the id (a int number)
 * of a team we inmediately know the venue of the team (because localities of each team 
 * corresponds to the same team id). So, a team can be fully represented only by a number, hence,
 * that being the argument of the non existence of a specific Team class.
 * 
 * @author dacuna <dacuna@alumnos.inf.utfsm.cl>
 * @date 10/10/2013
 */
class Game
{
private:
	int local_team_; /**<The team who plays in his venue */
	int visit_team_; /**<The team who plays in a venue != his venue */
	int slot_;       /**<Round in wich the game is going to get played */
public:
	/**
	 * Default class constructor.
	 */
	Game();
	
	/**
	 * Class constructor based on pair of teams.
	 * 
	 * @param local_team id of the local team
	 * @param visit_team id of the visit team
	 */
	Game(int local_team, int visit_team);
	
	/**
	 * Returns the id of the team who plays in his venue.
	 * 
	 * @return id of local team
	 */
	int local_team() const;
	
	/**
	 * Returns the id of the team who plays in a venue != his venue.
	 * 
	 * @return id of visit team
	 */
	int visit_team() const;
	
	/**
	 * Returns the slot in wich the game is going to be played
	 * 
	 * @return slot of the game
	 */
	 int slot() const;
	
	/**
	 * Class destructor
	 */
	~Game();

};

#endif // GAME_H
