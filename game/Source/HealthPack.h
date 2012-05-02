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

class HealthPack : public StateMachine
{
public:

    HealthPack( GameObject & object ) : StateMachine( object ) {}

	~HealthPack( void ) { 
    }


private:

	virtual bool States( State_Machine_Event event, MSG_Object * msg, int state, int substate );

};
