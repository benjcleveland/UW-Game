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
#include "deathsm.h"
#include "database.h"
#include "collision.h"


//Add new states here
enum StateName {
	STATE_Initialize,	//Note: the first enum is the starting state
	STATE_Idle,
	STATE_Dead
};

//Add new substates here
enum SubstateName {
	SUBSTATE_RenameThisSubstate1,
	SUBSTATE_RenameThisSubstate2,
	SUBSTATE_RenameThisSubstate3
};

bool Death::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses go here
    OnMsg(MSG_ASTAR_DEBUG)
        // toggle a star debugging
        m_owner->setAStarDebug(msg->GetBoolData());

    OnMsg(MSG_Reset)
        m_owner->getMovement()->reset();
       // PopStateMachine();

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )
		OnEnter
        
            ChangeState( STATE_Idle );
	
	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Idle )
		OnEnter
        m_owner->SetHealth(m_owner->GetHealth() - 1 );
        if( m_owner->GetHealth() < 0 )
            ChangeState( STATE_Dead );
        else
            PopStateMachine();
            

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Dead )
		OnEnter
            // play death sound
            SendMsgBroadcastNow(MSG_Death_Sound,OBJECT_Sounds,MSG_Data());

            // make the NPC stop moving
            m_owner->getMovement()->setMovementDead();

            // check to see if there are any other NPC's that are alive
            dbCompositionList list;
            g_database.ComposeList( list, OBJECT_NPC );
            dbCompositionList::iterator i;
            bool still_alive = false;
            for( i=list.begin(); i!=list.end(); ++i )
            {
                if((*i)->GetHealth() > -1 )
                {
                    still_alive = true;
                    break;
                }
            }
            if(still_alive == false ) // all the NPC's are dead, game is over
            {
                SendMsgBroadcastNow(MSG_All_NPC_Dead, OBJECT_Gameflow, MSG_Data());
            }
        

EndStateMachine
}


