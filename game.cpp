/**
 * Ant+TUP - IA Avazada
 * game.cpp
 * Purpose: Implementation of class Game
 * @author dacuna <dacuna@alumnos.inf.utfsm.cl>
 * @version 1.0 10/10/2013
 */

#include "game.h"

Game::Game()
{
}

Game::Game(int local_team, int visit_team) : local_team_(local_team), visit_team_(visit_team)
{
}

int Game::local_team() const
{
	return local_team_;
}

int Game::visit_team() const 
{
	return visit_team_;
}

int Game::slot() const
{
	return slot_;
}

Game::~Game()
{
}
