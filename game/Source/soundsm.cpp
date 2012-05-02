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
#include "soundsm.h"
#include "database.h"
#include "collision.h"


//Add new states here
enum StateName {
	STATE_Initialize,	//Note: the first enum is the starting state

};

//Add new substates here
enum SubstateName {
	SUBSTATE_RenameThisSubstate1,
	SUBSTATE_RenameThisSubstate2,
	SUBSTATE_RenameThisSubstate3
};

bool Sound::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses go here
    OnMsg(MSG_Death_Sound)
        m_pDeathSound->Play();

    OnMsg(MSG_Proj_Explosion)
        m_pExplosionSound->Reset();
        m_pExplosionSound->Play();

    OnMsg( MSG_Game_End_Lost)
        m_pGameEndSound->Play();

    OnMsg(MSG_Proj_Explosion_Col)
        m_pExplosionColSound->Reset();
        m_pExplosionColSound->Play();

    OnMsg( MSG_Health_Pack )
        m_pHealthPackSound->Play();

    OnMsg( MSG_CountDown )
        m_pCountDownSound->Play();

    OnMsg( MSG_Whistle )
        m_pWhistleSound->Play();

    OnMsg( MSG_Winning )
        m_pWinningSound->Play();

    OnMsg( MSG_WallCollision )
        m_pWallCollSound->Reset();
        m_pWallCollSound->Play();

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )
		OnEnter
   
	
	///////////////////////////////////////////////////////////////

            
       
EndStateMachine
}


