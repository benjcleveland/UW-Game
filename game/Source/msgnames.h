/* Copyright Steve Rabin, 2008. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2008"
 */

//These message names are processed inside msg.h

REGISTER_MESSAGE_NAME(MSG_NULL)							//Reserved message name
REGISTER_MESSAGE_NAME(MSG_GENERIC_TIMER)				//Reserved message name
REGISTER_MESSAGE_NAME(MSG_CHANGE_STATE_DELAYED)			//Reserved message name
REGISTER_MESSAGE_NAME(MSG_CHANGE_SUBSTATE_DELAYED)		//Reserved message name

//Add new messages here

// used for attack state machine
REGISTER_MESSAGE_NAME(MSG_Attack)

// used for projectile state machine
REGISTER_MESSAGE_NAME( MSG_ProjectileHit )
REGISTER_MESSAGE_NAME( MSG_ProjectileLaunch )
REGISTER_MESSAGE_NAME( MSG_ProjectileInit )
REGISTER_MESSAGE_NAME(MSG_CollisionPlayer)
REGISTER_MESSAGE_NAME(MSG_CollisionWall)

//Used for Zombie and Human demo state machines
REGISTER_MESSAGE_NAME(MSG_CheckTouch)
REGISTER_MESSAGE_NAME(MSG_Tagged)
REGISTER_MESSAGE_NAME(MSG_SetTargetPosition)
REGISTER_MESSAGE_NAME(MSG_Arrived)
REGISTER_MESSAGE_NAME(MSG_Reset)
REGISTER_MESSAGE_NAME(MSG_MouseClick)
REGISTER_MESSAGE_NAME(MSG_Collision)
REGISTER_MESSAGE_NAME(MSG_DoneSleeping)

REGISTER_MESSAGE_NAME(MSG_NPCHitByProjectile)

// used for A* debug enable/disable
REGISTER_MESSAGE_NAME(MSG_ASTAR_DEBUG)

// sound messages
REGISTER_MESSAGE_NAME(MSG_Death_Sound)
REGISTER_MESSAGE_NAME(MSG_Proj_Explosion)
REGISTER_MESSAGE_NAME(MSG_Game_End_Lost)
REGISTER_MESSAGE_NAME(MSG_Proj_Explosion_Col)
REGISTER_MESSAGE_NAME(MSG_Health_Pack)
REGISTER_MESSAGE_NAME(MSG_CountDown)
REGISTER_MESSAGE_NAME(MSG_Whistle)
REGISTER_MESSAGE_NAME(MSG_Winning)
REGISTER_MESSAGE_NAME(MSG_WallCollision)

// game message
REGISTER_MESSAGE_NAME(MSG_All_NPC_Dead)
REGISTER_MESSAGE_NAME(MSG_GameStart)
REGISTER_MESSAGE_NAME(MSG_PlayerHitByProjectile)
REGISTER_MESSAGE_NAME(MSG_PlayerGotHealthPack)

//Unit test messages
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage2)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage3)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage4)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage5)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage6)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage7)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage8)
REGISTER_MESSAGE_NAME(MSG_UnitTestMessage9)
REGISTER_MESSAGE_NAME(MSG_UnitTestBroken)
REGISTER_MESSAGE_NAME(MSG_UnitTestPing)
REGISTER_MESSAGE_NAME(MSG_UnitTestAck)
REGISTER_MESSAGE_NAME(MSG_UnitTestDone)
REGISTER_MESSAGE_NAME(MSG_UnitTestTimer)
