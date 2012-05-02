/* Copyright Steve Rabin, 2008. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2008"
 */

#pragma once

#include "statemch.h"


class Traveler : public StateMachine
{
public:

	Traveler( GameObject & object ) : StateMachine( object ), m_game_started(false) {}
	~Traveler( void ) {}


private:

	virtual bool States( State_Machine_Event event, MSG_Object * msg, int state, int substate );
    bool m_game_started;
};
