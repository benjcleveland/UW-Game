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
#include "patroller.h"
#include "database.h"
#include "collision.h"
#include "Tiny.h"
#include "attacker.h"
#include "deathsm.h"

// another hack
extern CTiny *g_pPlayer;

//Add new states here
enum StateName {
	STATE_PatrolInitialize,	//Note: the first enum is the starting state
	STATE_PatrolIdle,
    STATE_PatrolRotate,
	STATE_PatrolSeekLocation
};

//Add new substates here
enum SubstateName {
	SUBSTATE_None
};

bool Patroller::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses go here
    OnMsg(MSG_ASTAR_DEBUG)
        // toggle a star debugging
        m_owner->setAStarDebug(msg->GetBoolData());

    OnMsg(MSG_NPCHitByProjectile)
        // npc has been hit - push the death state machine
        PushStateMachine( *new Death(*m_owner));

    
	///////////////////////////////////////////////////////////////
	DeclareState( STATE_PatrolInitialize )
		OnEnter
            // change the color of the debug line
            m_owner->getDebugLine()->setColor(D3DCOLOR_COLORVALUE(1,1,1,1));
			ChangeState( STATE_PatrolIdle );
            
	
	///////////////////////////////////////////////////////////////
	DeclareState( STATE_PatrolIdle )
		OnEnter            
            ChangeState( STATE_PatrolSeekLocation );
            

    ///////////////////////////////////////////////////////////////
  /*  DelacareState( STATE_PatrolRotate )
        OnEnter
            // tell the NPC to rotate
            m_owner->getMovement()->setMovementModeRotate();

        OnMsg( MSG_Arrived )
            ChangeState( STATE_PatrolSeekLocation );
*/
	///////////////////////////////////////////////////////////////
	DeclareState( STATE_PatrolSeekLocation )
		OnEnter
            // set the target location
            m_owner->getMovement()->setDestLocation(m_seek_position);
            
            // rotate first
            //m_owner->getMovement()->setMovementModeRotate();
           /*
        OnPeriodicTimeInState(10.1f)
            bool see_player = runNPCSeePlayer(&m_owner->getMovement()->getPosition(), &g_pPlayer->GetPosition());
            if( see_player == true )
            {
                  float dist = D3DXVec3Length( &(m_owner->getMovement()->getPosition() - g_pPlayer->GetPosition()));
                  if( dist < 5 )  // if we are close enough attack!!
                    PushStateMachine(*new Attacker( *m_owner ));
                // we can see the player so push the attacker state machine
            }
*/
        OnMsg( MSG_Arrived ) // arrived at the destination
            RequeueStateMachine();


EndStateMachine
}


