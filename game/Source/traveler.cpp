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
#include "traveler.h"
#include "database.h"
#include "collision.h"
#include "Tiny.h"
#include "attacker.h"
#include "deathsm.h"

// another hack
extern CTiny *g_pPlayer;

//Add new states here
enum StateName {
	STATE_Initialize,	//Note: the first enum is the starting state
	STATE_Idle,
	STATE_SeekPlayer,
	STATE_SeekRandomLocation,
    STATE_ArrivedAtLocation
};

//Add new substates here
enum SubstateName {
	SUBSTATE_PickLocation,
	SUBSTATE_VerifyLocation,
	SUBSTATE_SeekLocation
};

bool Traveler::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses go here
    OnMsg( MSG_Arrived ) // arrived at the destination
            ChangeState( STATE_ArrivedAtLocation );

    OnMsg(MSG_ASTAR_DEBUG)
        // toggle a star debugging
        m_owner->setAStarDebug(msg->GetBoolData());

    OnMsg(MSG_NPCHitByProjectile)
        // npc has been hit - push the death state machine
        PushStateMachine( *new Death(*m_owner));

    OnMsg(MSG_Reset)
        m_owner->getMovement()->reset();
        ChangeState( STATE_Initialize );


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )
		OnEnter
        if( m_game_started )
            ChangeState( STATE_Idle );

        OnMsg(MSG_GameStart)
            m_game_started = true;
			ChangeState( STATE_Idle );
	
	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Idle )
		OnEnter
            D3DXVECTOR3 point;
            bool see_player = runNPCSeePlayer(&m_owner->getMovement()->getPosition(), &g_pPlayer->GetPosition());
            if( see_player == true )
            {
                // change the state to seeking the player
                ChangeState( STATE_SeekPlayer );
            }
            else
            {
                // change state to random target
                ChangeState( STATE_SeekRandomLocation );
            }
        OnMsg( MSG_Arrived )
            // do nothing, this shouldn't happen

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_SeekPlayer )

		OnEnter

            float dist = D3DXVec3Length( &(m_owner->getMovement()->getPosition() - g_pPlayer->GetPosition()));
            if( dist < 5 )  // if we are close enough attack!!
                PushStateMachine(*new Attacker( *m_owner ));

            // set the go to position to the player
            m_owner->getMovement()->setDestLocation(g_pPlayer->GetPosition());
         
            // change the color of the debug line
            m_owner->getDebugLine()->setColor(D3DCOLOR_COLORVALUE(1,0,0,1));


        OnPeriodicTimeInState(.1f)
            // make sure we can still see the player
            bool see_player = runNPCSeePlayer(&m_owner->getMovement()->getPosition(), &g_pPlayer->GetPosition());
            if( see_player == true )
            {
                float dist = D3DXVec3Length( &(m_owner->getMovement()->getPosition() - g_pPlayer->GetPosition()));
                if( dist < 5 )  // if we are close enough attack!!
                   PushStateMachine(*new Attacker( *m_owner ));

                // update the seek position
                if( m_owner->getMovement()->getTarget() != g_pPlayer->GetPosition() ) // only update if the player moved
                {
                    m_owner->getMovement()->setDestLocation(g_pPlayer->GetPosition());
                }
            }
            else // change the state to a random location
            {
               ChangeState( STATE_SeekRandomLocation );
            }


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_SeekRandomLocation )

		OnEnter
            // change the color of the debug line
            m_owner->getDebugLine()->setColor(D3DCOLOR_COLORVALUE(1,1,1,1));
           
           ChangeSubstate( SUBSTATE_PickLocation );
           DeclareSubstate( SUBSTATE_PickLocation ) 
               OnEnter
                    // select a random location
                    m_owner->getMovement()->setRandomTarget();
                    ChangeSubstate( SUBSTATE_SeekLocation );
/*
            DeclareSubstate( SUBSTATE_VerifyLocation )
                OnEnter
                    D3DXVECTOR3 coll_pos;
                    // Verify the location, ie, no collision
                    bool collision = runNPCSeePlayer(&m_owner->getMovement()->getPosition(), &m_owner->getMovement()->getTarget(), coll_pos);

                    // update the target if necessary, this kindof works....
                    if( collision == false )  // false means there was a collision
                    {
                        m_owner->getMovement()->setTarget( coll_pos );    
                    }

                    ChangeSubstate( SUBSTATE_SeekLocation );
  */

            DeclareSubstate( SUBSTATE_SeekLocation )
                OnEnter

                OnPeriodicTimeInState(.1f)
                    // make sure we still CANT see the player
                    bool see_player = runNPCSeePlayer(&m_owner->getMovement()->getPosition(), &g_pPlayer->GetPosition());
                    if( see_player == true )
                    {
                        // change state to random location
                        ChangeState( STATE_SeekPlayer );
                    }
    
    ///////////////////////////////////////////////////////////////        
    DeclareState( STATE_ArrivedAtLocation )
        OnEnter
            // change state to idle after a delay
            ChangeStateDelayed( RandDelay(1,2), STATE_Idle);
        OnMsg( MSG_Arrived )


EndStateMachine
}


