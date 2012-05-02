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
#include "attacker.h"
#include "database.h"
#include "collision.h"
#include "Tiny.h"
#include "deathsm.h"


// another hack
extern VecCollQuad vecCollQuad;
extern CTiny *g_pPlayer;

//Add new states here
enum StateName {
	STATE_Initialize,	//Note: the first enum is the starting state
	STATE_Idle,
	STATE_AttackPlayer,
	STATE_Runaway,
    STATE_ArrivedAtLocation
};

//Add new substates here
enum SubstateName {
	SUBSTATE_RenameThisSubstate1,
	SUBSTATE_RenameThisSubstate2,
	SUBSTATE_RenameThisSubstate3
};

bool Attacker::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses go here
/*  Not currently used  */
    OnMsg( MSG_CollisionPlayer )
        // stop moving
        m_owner->getMovement()->setTarget(m_owner->getMovement()->getPosition());
        //ChangeStateDelayed(10, STATE_Runaway );

    OnMsg(MSG_ASTAR_DEBUG)
        // toggle a star debugging
        m_owner->setAStarDebug(msg->GetBoolData());

    OnMsg(MSG_NPCHitByProjectile)
        // npc has been hit - replace this sm with death state machine
        ReplaceStateMachine( *new Death(*m_owner));

    OnMsg(MSG_Reset)
        m_owner->getMovement()->reset();
        //PopStateMachine();

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )
		OnEnter
            ChangeState( STATE_Idle );
	
	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Idle )

		OnEnter
            // make sure we can see the player and we are within the appropriate distance
            bool see_player = runNPCSeePlayer(&m_owner->getMovement()->getPosition(), &g_pPlayer->GetPosition());
            float dist = D3DXVec3Length( &(m_owner->getMovement()->getPosition() - g_pPlayer->GetPosition()));
            if( see_player == true && dist < 5)
            {
                // change the state to seeking the player
                ChangeState( STATE_AttackPlayer );
            }
            else
            {
                // we can't see the player so go back to wandering
                // do this by removing the attacker statemachine
                PopStateMachine();
            }

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_AttackPlayer )

		OnEnter
            // change the color of the debug line
            m_owner->getDebugLine()->setColor(D3DCOLOR_COLORVALUE(1,0,0,1));
            
            // make sure we are facing the player 
            m_owner->getMovement()->setDestLocation( g_pPlayer->GetPosition());

            // attack the player!, after some delay...
            SendMsgDelayedToState(RandDelay(1,2),MSG_Attack,MSG_Data());

        OnMsg( MSG_Attack )
            // create the name of the projectile
            char name[10] = "PROJ";
            sprintf(name, "%s%s", name, &m_owner->GetName()[3]);

            // we have attacked the player so now run away
            SendMsg( MSG_ProjectileLaunch, g_database.GetIDByName( name), MSG_Data( m_owner->getMovement()->getPosition()));
            
            // runaway from the player
            ChangeState( STATE_Runaway );

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Runaway )

		OnEnter
            // determine a location behind the NPC to run to 
            D3DXVECTOR3 diff  = m_owner->getNPC()->getFacing();

            D3DXVECTOR3 move_pos = D3DXVECTOR3(5,0,5);
            move_pos.x *= diff.x;
            move_pos.z *= diff.z;
            move_pos = m_owner->getMovement()->getPosition() - move_pos;
            
            if( move_pos.x <= 2.f )
                move_pos.x = 2.5f;
            if( move_pos.z <= 2.f )
                move_pos.z = 2.5f;

            // verify the location
            D3DXVECTOR3 coll_pos;
            // Verify the location, ie, no collision
            bool collision = runNPCSeePlayer(&m_owner->getMovement()->getPosition(), &move_pos, coll_pos);

            // update the target if necessary, this kindof works....
            if( collision == false )  // false means there was a collision
            {
                //if( (int)move_pos.x != (int)coll_pos.x )
                {       
                    coll_pos.x += .5f*((coll_pos.x > move_pos.x) ? 1 : -1);
                }
                //if( (int) move_pos.z != (int)coll_pos.z )
                    coll_pos.z += .5f*((coll_pos.z > move_pos.z) ? 1 : -1);
              
                    // hack
                if( coll_pos.z <= 2 )
                    coll_pos.z = 2.f;
                if( coll_pos.x <= 2 )
                    coll_pos.x = 2.f;
                
                // update the seek location
                m_owner->getMovement()->setDestLocation( coll_pos );    
            }
            else
            // change the location
                m_owner->getMovement()->setDestLocation( move_pos);

            // change the color of the debug line
            m_owner->getDebugLine()->setColor(D3DCOLOR_COLORVALUE(0,1,0,1));

        OnMsg( MSG_Arrived ) 
            // arrived at the destination, change state to arrived
            ChangeState( STATE_ArrivedAtLocation );

    ///////////////////////////////////////////////////////////////
    DeclareState( STATE_ArrivedAtLocation )
        OnEnter
        SendMsgDelayedToState( RandDelay(1,2), MSG_DoneSleeping, MSG_Data());

        OnMsg( MSG_DoneSleeping )
            // pop the state machine
            PopStateMachine();
        

EndStateMachine
}


