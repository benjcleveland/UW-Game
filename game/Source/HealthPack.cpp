/* Copyright Steve Rabin, 2008. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2008"
 */


#include "DXUT.h"
#include "HealthPack.h"
#include "database.h"


//Add new states here
enum StateName {
	STATE_Available,	//Note: the first enum is the starting state
    STATE_Used,
    STATE_UsedPS
};

//Add new substates here
enum SubstateName {
	SUBSTATE_RenameThisSubstate1,
	SUBSTATE_RenameThisSubstate2,
	SUBSTATE_RenameThisSubstate3
};

bool HealthPack::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses go here
    OnMsg( MSG_Reset )
        ChangeState( STATE_Available );

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Available )
		OnEnter
            m_owner->setHPRender(true);
        
        OnMsg( MSG_PlayerGotHealthPack )
            // play sound
            SendMsgBroadcastNow(MSG_Health_Pack,OBJECT_Sounds,MSG_Data());
            m_owner->getHPPartSys()->Enable(m_owner->getHealthPack()->getPosition());
            ChangeState( STATE_UsedPS );
	
	///////////////////////////////////////////////////////////////
    DeclareState( STATE_UsedPS )
        OnEnter
            // disable rendering
            m_owner->setHPRender(false);
        OnTimeInState(8)
            ChangeState( STATE_Used );

        OnExit
            // disable the PS
            m_owner->getHPPartSys()->Enable(D3DXVECTOR3(0,0,0));
            
    ///////////////////////////////////////////////////////////////
    DeclareState( STATE_Used )  
        OnEnter
            // do nothing

EndStateMachine
}


