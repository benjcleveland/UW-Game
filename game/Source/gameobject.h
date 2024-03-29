/* Copyright Steve Rabin, 2008. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2008"
 */

#pragma once

#include <list>
#include "global.h"

#include "Mesh.h"
#include "Movement.h"
#include "line.h"
#include "ParticalSystem.h"
#include "projectile_movement.h"
#include "hud.h"
#include "AnimatedMesh.h"

//Add new object types here (bitfield mask - objects can be combinations of types)
#define OBJECT_Ignore_Type  (0)
#define OBJECT_Gameflow     (1<<1)
#define OBJECT_Character    (1<<2)
#define OBJECT_NPC          (1<<3)
#define OBJECT_Player       (1<<4)
#define OBJECT_Enemy        (1<<5)
#define OBJECT_Weapon       (1<<6)
#define OBJECT_Item         (1<<7)
#define OBJECT_Projectile   (1<<8)
#define OBJECT_PlayerProjectile (1<<9)
#define OBJECT_Sounds       (1<<10)

#define GAME_OBJECT_MAX_NAME_SIZE 64


//Forward declarations
class StateMachineManager;
class MSG_Object;



class GameObject
{
public:

	GameObject( objectID id, unsigned int type, char* name, CSoundManager *p_sound );
	~GameObject( void );

	inline objectID GetID( void )					{ return( m_id ); }
	inline unsigned int GetType( void )				{ return( m_type ); }
	inline char* GetName( void )					{ return( m_name ); }
	
	void Update( void );
    void Render( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj, bool shadowMap );
    void Animate( float fElapsedTime, double dTime, const D3DXVECTOR3 *cameraFacing );

    void Release();

	//State machine related
	void CreateStateMachineManager( void );
	inline StateMachineManager* GetStateMachineManager( void )	{ ASSERTMSG(m_stateMachineManager, "GameObject::GetStateMachineManager - m_stateMachineManager not set"); return( m_stateMachineManager ); }

	//Scheduled deletion
	inline void MarkForDeletion( void )				{ m_markedForDeletion = true; }
	inline bool IsMarkedForDeletion( void )			{ return( m_markedForDeletion ); }

	int GetHealth(void)								{ return( m_health ); }
	void SetHealth(int health)						{ m_health = health; }

    // createNPC
    void createNPC( IDirect3DDevice9* pd3dDevice, LPCWSTR filename );
    Mesh* getNPC(){ return m_npc_mesh; };

    // debug stuff
    CLine* getDebugLine(){ return m_debug_line; };
    void setDebug( bool debug) { m_debug = debug; };
    void setAStarDebug( bool debug ) { m_debugAStar = debug; };

    // movement
    void createMovement() { m_movement = new Movement(*this); };
    Movement* getMovement() { return m_movement; };

    // projectile
    void createProjectile( IDirect3DDevice9* pd3dDevice);
    CProjectile* getProjectile() {return m_projectile; };

    // projectile movement
    void createProjMovement() { m_proj_movement = new ProjectileMovement(*this); }
    ProjectileMovement* getProjMovemement() { return m_proj_movement; }

    // hud
    void createHUD( IDirect3DDevice9 *pd3dDevice ) { m_hud = new HUD(); m_hud->hudInit( pd3dDevice ); }
    HUD* getHUD() { return m_hud; }

    // get sound manager
    CSoundManager* getSoundManager(){ return m_soundManager; };

    // health pack
    void createHealthPack( IDirect3DDevice9 *pd3dDevice ) { m_health_pack = new CAnimMesh(); m_health_pack->Init( pd3dDevice ); m_hp_part_sys = new CParticalSystem(); m_hp_part_sys->Init( pd3dDevice); }
    CAnimMesh* getHealthPack(){ return m_health_pack; }
    CParticalSystem* getHPPartSys() { return m_hp_part_sys; }

    void setHPRender(bool val) {m_render_hp = val;}

private:

	objectID m_id;									//Unique id of object (safer than a pointer).
	unsigned int m_type;							//Type of object (can be combination).
	bool m_markedForDeletion;						//Flag to delete this object (when it is safe to do so).
	char m_name[GAME_OBJECT_MAX_NAME_SIZE];			//String name of object.

	StateMachineManager* m_stateMachineManager;
	
    CSoundManager *m_soundManager;
    // mesh
	Mesh *m_npc_mesh;
    Movement *m_movement;
    CLine *m_debug_line;

    // animated mesh
    CAnimMesh *m_health_pack;
    CParticalSystem *m_hp_part_sys;

    // HUD
    HUD *m_hud;

    // projectile
    CProjectile *m_projectile;
    ProjectileMovement *m_proj_movement;

	int m_health;
    bool m_debug;
    bool m_debugAStar;
    bool m_render_hp;
};
