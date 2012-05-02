/* Copyright Steve Rabin, 2008. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2008"
 */

#pragma warning(disable: 4995)

#include "DXUT.h"
#include "world.h"
#include "time.h"
#include "database.h"
#include "msgroute.h"
#include "gameobject.h"
#include "debuglog.h"
#include "DebugCamera.h"



// State machines
#include "traveler.h"
#include "projectile.h"
#include "patroller.h"
#include "soundsm.h"
#include "gamesm.h"
#include "HealthPack.h"

// Unit test state machines
#include "unittest1.h"
#include "unittest2a.h"
#include "unittest2b.h"
#include "unittest2c.h"
#include "unittest3a.h"
#include "unittest3b.h"
#include "unittest4.h"
#include "unittest5.h"
#include "unittest6.h"


extern CCameras g_Cameras;
//#define UNIT_TESTING



World::World(void)
: m_initialized(false)
{

}

World::~World(void)
{
	delete m_time;
	delete m_database;
	delete m_msgroute;
	delete m_debuglog;
}

void World::InitializeSingletons( void )
{
	//Create Singletons
	m_time = new Time();
	m_database = new Database();
	m_msgroute = new MsgRoute();
	m_debuglog = new DebugLog();
}

void World::Initialize( IDirect3DDevice9* pd3dDevice, CSoundManager* p_sound )
{
    // start positions of the NPC's
    D3DXVECTOR3 start_positions[4] = 
    {
        D3DXVECTOR3(3.f, 0, 22.f ),
        D3DXVECTOR3(22.f, 0, 22.f),
        D3DXVECTOR3(22.f, 0, 3.f),
        D3DXVECTOR3(6.f, 0, 7.f)
    };

	if(!m_initialized)
	{
		m_initialized = true;
	

#ifdef UNIT_TESTING

		//Create unit test game objects
		GameObject* unittest1 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest1" );
		GameObject* unittest2 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest2" );
		GameObject* unittest3a = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest3a" );
		GameObject* unittest3b = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest3b" );
		GameObject* unittest4 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest4" );
		GameObject* unittest5 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest5" );
		GameObject* unittest6 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest6" );
		
		unittest1->CreateStateMachineManager();
		unittest2->CreateStateMachineManager();
		unittest3a->CreateStateMachineManager();
		unittest3b->CreateStateMachineManager();
		unittest4->CreateStateMachineManager();
		unittest5->CreateStateMachineManager();
		unittest6->CreateStateMachineManager();
		
		g_database.Store( *unittest1 );
		g_database.Store( *unittest2 );
		g_database.Store( *unittest3a );
		g_database.Store( *unittest3b );
		g_database.Store( *unittest4 );
		g_database.Store( *unittest5 );
		g_database.Store( *unittest6 );

		//Give the unit test game objects a state machine
		unittest1->GetStateMachineManager()->PushStateMachine( *new UnitTest1( *unittest1 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest2->GetStateMachineManager()->PushStateMachine( *new UnitTest2a( *unittest2 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest3a->GetStateMachineManager()->PushStateMachine( *new UnitTest3a( *unittest3a ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest3b->GetStateMachineManager()->PushStateMachine( *new UnitTest3b( *unittest3b ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest4->GetStateMachineManager()->PushStateMachine( *new UnitTest4( *unittest4 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest5->GetStateMachineManager()->PushStateMachine( *new UnitTest5( *unittest5 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest6->GetStateMachineManager()->PushStateMachine( *new UnitTest6( *unittest6 ), STATE_MACHINE_QUEUE_0, TRUE );

#else


    // create the NPC's
	for( int i = 0; i < 4; i++ )
	{
		//Create game objects
		char name[10] = "NPC";
		sprintf( name, "%s%d", name, i );
		GameObject* npc = new GameObject( g_database.GetNewObjectID(), OBJECT_NPC, name, p_sound );
		npc->CreateStateMachineManager();
		g_database.Store( *npc );
        
        npc->createMovement();
        //npc->createNPC(pd3dDevice, L"Dwarf.x");
        // replace with archive dude
        npc->createNPC( pd3dDevice, L"Rex_v2.x" );

        // create a camera for the npc, disabled for now
        g_Cameras.newPlayerCam(npc->getNPC());  

        // change to 3 to use patroller
        if( i < 4 )
        {
            // setup the travelers
            npc->getMovement()->setStartPosition(start_positions[i]);
            //Give the game object a state machine
            npc->GetStateMachineManager()->PushStateMachine( *new Traveler( *npc ), STATE_MACHINE_QUEUE_0, TRUE );
        }
        else
        {
            // create the patroller
            npc->getMovement()->setStartPosition(D3DXVECTOR3(3,0,19));
            npc->GetStateMachineManager()->PushStateMachine( *new Patroller( *npc, D3DXVECTOR3(22, 0, 22 )), STATE_MACHINE_QUEUE_0, TRUE );
            npc->GetStateMachineManager()->PushStateMachine( *new Patroller( *npc, D3DXVECTOR3(19, 0, 17)), STATE_MACHINE_QUEUE_0, TRUE);
            npc->GetStateMachineManager()->PushStateMachine( *new Patroller( *npc, D3DXVECTOR3(7, 0, 21)), STATE_MACHINE_QUEUE_0, TRUE);
            npc->GetStateMachineManager()->PushStateMachine( *new Patroller( *npc, D3DXVECTOR3(10, 0, 7)), STATE_MACHINE_QUEUE_0, TRUE);
            npc->GetStateMachineManager()->PushStateMachine( *new Patroller( *npc, D3DXVECTOR3(17, 0, 2 )), STATE_MACHINE_QUEUE_0, TRUE );
        }
           
	}

    // create the projectiles
    for( int i = 0; i < 4; ++i )
    {
        char name[10] = "PROJ";
        sprintf( name, "%s%d", name, i);
        GameObject *projectile = new GameObject( g_database.GetNewObjectID(), OBJECT_Projectile, name, p_sound );
        projectile->CreateStateMachineManager();

        g_database.Store( *projectile );

        // create the projectile
        projectile->createProjectile( pd3dDevice);

        // crate movement class
        projectile->createProjMovement( );

        // create the state machine
        projectile->GetStateMachineManager()->PushStateMachine( *new ProjectileSM( *projectile ), STATE_MACHINE_QUEUE_0, TRUE );
    }

    // create the health packs
    for( int i =0; i < 4; ++i)
    {
        char name[10] = "ITEM";
        sprintf( name, "%s%d", name, i);
        GameObject *health_pack = new GameObject( g_database.GetNewObjectID(), OBJECT_Item, name, p_sound );
        health_pack->CreateStateMachineManager();

        g_database.Store( *health_pack );

        // create the item
        health_pack->createHealthPack( pd3dDevice);

        // set the location
        health_pack->getHealthPack()->setLocation(start_positions[i]);

        // create the state machine
        health_pack->GetStateMachineManager()->PushStateMachine( *new HealthPack( *health_pack ), STATE_MACHINE_QUEUE_0, TRUE );
    }

    // create the player projectile
    char name[10] = "PLAYPROJ0";
    GameObject *projectile = new GameObject( g_database.GetNewObjectID(), OBJECT_PlayerProjectile, name, p_sound );
    projectile->CreateStateMachineManager();

    g_database.Store( *projectile );

    // create the projectile
    projectile->createProjectile( pd3dDevice);

    // crate movement class
    projectile->createProjMovement( );

    // create the state machine
    projectile->GetStateMachineManager()->PushStateMachine( *new ProjectileSM( *projectile ), STATE_MACHINE_QUEUE_0, TRUE );

    // create a sound sm
    GameObject *sounds = new GameObject( g_database.GetNewObjectID(), OBJECT_Sounds, "Sounds0", p_sound );
    sounds->CreateStateMachineManager();

    g_database.Store( *sounds );

    // create the state machine
    sounds->GetStateMachineManager()->PushStateMachine( *new Sound( *sounds ), STATE_MACHINE_QUEUE_0, TRUE );


    // create the game state machine
    GameObject *gameflow = new GameObject( g_database.GetNewObjectID(), OBJECT_Gameflow, "Gameflow", p_sound );
    gameflow->CreateStateMachineManager();
    g_database.Store( *gameflow );

    // create hud for the gameflow
    gameflow->createHUD( pd3dDevice );

    // create the sm
    gameflow->GetStateMachineManager()->PushStateMachine( *new GameSM( *gameflow), STATE_MACHINE_QUEUE_0, TRUE );

    // initalize the grids
    m_grid_layer.initialize(pd3dDevice);

#endif

	}
}

void World::Update()
{
	g_time.MarkTimeThisTick();
	g_database.Update();
}

void World::Animate( double dTimeDelta, double dTime, const D3DXVECTOR3 *cameraFacing )
{
    g_database.Animate( dTimeDelta,dTime, cameraFacing );
 
    // update the grids
    m_grid_layer.UpdateLayers();
}

void World::AdvanceTimeAndRender(IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj, double dTimeDelta, D3DXVECTOR3 *pvEye, bool shadowPass )
{
    g_database.AdvanceTimeAndRender(pd3dDevice,pViewProj,dTimeDelta,pvEye,shadowPass);
    if(!shadowPass)
        m_grid_layer.render(pd3dDevice, *pViewProj);
        
}

void World::Release()
{
    // little bit of a hack - this is so the NPC's get recreated
    m_initialized = false;
    g_database.Release();
    m_grid_layer.Release();
}

void World::setDebug( bool debug )
{
    g_database.setDebug( debug );
}

void World::gridDebug()
{
    m_grid_layer.setDebugRender();
}

void World::renderText(CDXUTTextHelper *txtHelper)
{
    m_grid_layer.renderText(txtHelper);
}