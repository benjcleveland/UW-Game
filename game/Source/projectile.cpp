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
#include "projectile.h"
#include "database.h"
#include "collision.h"
#include "Tiny.h"


// another hack
extern VecCollQuad vecCollQuad;
extern CTiny *g_pPlayer;

//Add new states here
enum StateName {
	STATE_Initialize,	//Note: the first enum is the starting state
	STATE_Idle,
	STATE_Launch,
    STATE_Explosion,
    STATE_ExplosionCollision,
	STATE_Finished
};

//Add new substates here
enum SubstateName {
	SUBSTATE_RenameThisSubstate1,
	SUBSTATE_RenameThisSubstate2,
	SUBSTATE_RenameThisSubstate3
};

bool ProjectileSM::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses go here
    OnMsg(MSG_Reset)        // reset the state machine
        ChangeState( STATE_Initialize );

    

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )
		OnEnter
            ChangeState( STATE_Idle );
            
	
	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Idle )
		OnEnter
        // wait for the message to launch
        OnMsg( MSG_ProjectileLaunch )
            // save the start position of the projectile from the msg
            if(  msg->IsVector3Data() )
            {
                D3DXVECTOR3 pos = msg->GetVector3Data();
                pos.y = .4f;
                // set the initial position to the NPC
                m_owner->getProjMovemement()->setPosition( pos );
                
                // set the target to the player
                // consider changing this to a fixed distance
                if( m_owner->GetType() == OBJECT_Projectile )
                    m_owner->getProjMovemement()->setTarget( g_pPlayer->GetPosition());
                else
                    m_facing = g_pPlayer->GetFacing(); // save the players facing direction

               
                 // change state to launch
                ChangeState( STATE_Launch );
            }
        OnExit
            // disable the partical system
            m_owner->getProjectile()->DisablePartSys();
            m_owner->getProjectile()->setRenderType( RENDER_NOTHING );
        
	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Launch )
		OnEnter
            float force = 0.075f;
            D3DXVECTOR3 velocity;
            if(m_owner->GetType() == OBJECT_Projectile )
                velocity = force * m_owner->getProjMovemement()->getFacingVec();
            else
                velocity = force * m_facing;

            velocity.y = .005f;

            // set the velocity of the projectile
            m_owner->getProjMovemement()->setVelocity( velocity );

            // render the projectile
            m_owner->getProjectile()->setRenderType( RENDER_BOMB );

        OnMsg( MSG_CollisionPlayer ) // hit the player
            ChangeState( STATE_ExplosionCollision );

        OnMsg( MSG_CollisionWall )
            if( msg->IsVector3Data() )
            {
                D3DXVECTOR3 velocity = m_owner->getProjMovemement()->getVelocity();
                D3DXVECTOR3 norm = msg->GetVector3Data();
                //m_owner->getProjMovemement()->getFacingVec();
                norm.y = 0;
                if( D3DXVec3Dot(&velocity, &norm) < 0 ) // make sure we are going into the wall
                {
                    if( norm.x != 0 )
                        velocity.x *= -.6f;
                    if( norm.z != 0 )
                        velocity.z *= -.6f;
                    m_owner->getProjMovemement()->setVelocity( velocity );
                    
                    // play sound
                    SendMsgBroadcastNow(MSG_WallCollision, OBJECT_Sounds, MSG_Data());
                }
            }
            //ChangeState( STATE_ExplosionCollision );

        OnMsg( MSG_ProjectileHit )
            // wait for the message that we hit the ground
            // change state to explosion
            ChangeState( STATE_Explosion );

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Explosion )
		OnEnter
            // set render type to partical system
            m_owner->getProjectile()->setRenderType( RENDER_PS_DECAL );
            
            // enable the partical system
            m_owner->getProjectile()->EnablePartSys( m_owner->getProjMovemement()->getPosition());

            // play explosion sound
            SendMsgBroadcastNow(MSG_Proj_Explosion, OBJECT_Sounds, MSG_Data());

            // for some amount of time
        OnTimeInState( 1 )
            ChangeState( STATE_Finished );
                       

    ///////////////////////////////////////////////////////////////
    DeclareState( STATE_ExplosionCollision )
        OnEnter
            // set render type to partical system
            m_owner->getProjectile()->setRenderType( RENDER_PARTICAL_SYS );
            
            // enable the partical system
            m_owner->getProjectile()->EnablePartSys( m_owner->getProjMovemement()->getPosition());

            // play explosion sound
            SendMsgBroadcastNow(MSG_Proj_Explosion_Col, OBJECT_Sounds, MSG_Data());

            // for some amount of time
        OnTimeInState( 1 )
            // change state to Init
            ChangeState( STATE_Initialize );
                       
    ///////////////////////////////////////////////////////////////
    DeclareState( STATE_Finished )
        OnEnter
            // set render to decal
            //m_owner->getProjectile()->setRenderType( RENDER_DECAL );

            // go to the idle state
            ChangeStateDelayed(.5f, STATE_Idle );

EndStateMachine
}


