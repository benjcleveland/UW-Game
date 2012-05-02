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
#include "gamesm.h"
#include "database.h"
#include "collision.h"
#include "traveler.h"
#include "hud.h"


//Add new states here
enum StateName {
	STATE_Initialize,	//Note: the first enum is the starting state
	STATE_Idle,
    STATE_Playing,
    STATE_Countdown,
    STATE_Win,
    STATE_Lose
};

//Add new substates here
enum SubstateName {
	SUBSTATE_RenameThisSubstate1,
	SUBSTATE_RenameThisSubstate2,
	SUBSTATE_RenameThisSubstate3
};

bool GameSM::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message responses go here
    OnMsg(MSG_Reset)
        dbCompositionList list;
        g_database.ComposeList( list, OBJECT_NPC );
        dbCompositionList::iterator i;
        bool still_alive = false;
        for( i=list.begin(); i!=list.end(); ++i )
        {
            (*i)->GetStateMachineManager()->DeleteStateMachineQueue(STATE_MACHINE_QUEUE_0);
            (*i)->GetStateMachineManager()->PushStateMachine( *new Traveler( *(*i) ), STATE_MACHINE_QUEUE_0, TRUE );
        }

        // make sure all the NPC's have been reset
        SendMsgBroadcastNow(MSG_Reset, OBJECT_NPC, MSG_Data());
        
        ChangeState(STATE_Initialize );    

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )
		OnEnter
            // set the health for the player
            m_owner->getHUD()->setHealth(100);
            ChangeState( STATE_Idle );
	
	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Idle )
		OnEnter
            // display text
            // wait for player input to start the game
            m_owner->getHUD()->displayGameStart(true);
        OnMsg( MSG_GameStart )
            m_owner->getHUD()->displayGameStart(false);     // disable the start message
            // set the time
            m_owner->getHUD()->setTimeRemaining(300);

            ChangeState( STATE_Countdown );

    ///////////////////////////////////////////////////////////////
    DeclareState( STATE_Countdown )
        OnEnter
            // start countdown
            m_owner->getHUD()->displayCountDown(true, 3);
        OnPeriodicTimeInState(1)
            // update countdown
            int time = m_owner->getHUD()->getCountdownVal() - 1;
            m_owner->getHUD()->setCountdownVal(time);
            
            // play a sound
            if( time >= 0)
                SendMsgBroadcastNow(MSG_CountDown, OBJECT_Sounds, MSG_Data());
            else
                ChangeState(STATE_Playing);
        OnExit
            // play whistle
            SendMsgBroadcastNow(MSG_Whistle, OBJECT_Sounds, MSG_Data());

            // disable countdown
            m_owner->getHUD()->displayCountDown(false,0);
            // enable go!
            m_owner->getHUD()->displayGo(true);

    ///////////////////////////////////////////////////////////////
	DeclareState( STATE_Playing )
		OnEnter        
            // tell all the NPC's to start
            SendMsgBroadcastNow(MSG_GameStart, OBJECT_NPC, MSG_Data());
        OnTimeInState(1)
            // disable go message
            m_owner->getHUD()->displayGo(false);

        OnPeriodicTimeInState(1)
            // update the game time
            int time = m_owner->getHUD()->getTimeRemaining() - 1;
            m_owner->getHUD()->setTimeRemaining(time);
            if( time == 0 )
                ChangeState( STATE_Lose );  // the player ran out of time!

        OnMsg(MSG_All_NPC_Dead)
            //the player has won
            ChangeState(STATE_Win);

          // handle player hits
        OnMsg( MSG_PlayerHitByProjectile )
            // player was hit
            int health = m_owner->getHUD()->getHealth() - 10;
            m_owner->getHUD()->setHealth(health);
            
            if( health <= 0 ) // the player has died
            {
                ChangeState( STATE_Lose );
            }

        OnMsg( MSG_PlayerGotHealthPack )
            int health = m_owner->getHUD()->getHealth() + 10;
            health = (health > 100 ) ? 100 : health;
            m_owner->getHUD()->setHealth( health );

        OnExit
            // ensure the go isn't displayed
            m_owner->getHUD()->displayGo(false);

    ///////////////////////////////////////////////////////////////
	DeclareState( STATE_Win )
		OnEnter
            // display winning text
            m_owner->getHUD()->displayWinToggle();

            // play winning sound
            SendMsgBroadcastNow(MSG_Winning, OBJECT_Sounds, MSG_Data());

            // wait for player input to start the game
            m_owner->getHUD()->displayGameReset(true);

        OnExit
            m_owner->getHUD()->displayGameReset(false);
            m_owner->getHUD()->displayWinToggle();

    ///////////////////////////////////////////////////////////////
	DeclareState( STATE_Lose )
		OnEnter
            // display losing text
            m_owner->getHUD()->displayLoseToggle();

            // wait for player input to start the game
            m_owner->getHUD()->displayGameReset(true);

            // Game over sound
            SendMsgBroadcastNow(MSG_Game_End_Lost, OBJECT_Sounds, MSG_Data());
            
            // tell everything to stop moving
            
        OnExit
            m_owner->getHUD()->displayGameReset(false);
            m_owner->getHUD()->displayLoseToggle();


EndStateMachine
}


