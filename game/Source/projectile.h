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
#include "DXUT\SDKsound.h"

class ProjectileSM : public StateMachine
{
public:

    ProjectileSM( GameObject & object ) : StateMachine( object ) {}
	~ProjectileSM( void ) { }


private:

	virtual bool States( State_Machine_Event event, MSG_Object * msg, int state, int substate );

    D3DXVECTOR3 m_facing;
};
