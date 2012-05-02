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
#include "gameobject.h"
#include "msgroute.h"
#include "statemch.h"


#include "Mesh.h"
#include "movement.h"


GameObject::GameObject( objectID id, unsigned int type, char* name, CSoundManager *p_sound )
: m_markedForDeletion(false),
  m_stateMachineManager(0),
  m_health(2),
  m_npc_mesh(NULL),
  m_movement(NULL),
  m_debug_line(NULL),
  m_projectile(NULL),
  m_proj_movement(NULL),
  m_debugAStar(false),
  m_soundManager(p_sound),
  m_hud(NULL),
  m_health_pack(NULL),
  m_hp_part_sys(NULL),
  m_debug(false),
  m_render_hp(true)
{
	m_id = id;
	m_type = type;
	
	if( strlen(name) < GAME_OBJECT_MAX_NAME_SIZE ) {
		strcpy( m_name, name );
	}
	else {
		strcpy( m_name, "invalid_name" );
		ASSERTMSG(0, "GameObject::GameObject - name is too long" );
	}
}

GameObject::~GameObject( void )
{
	if(m_stateMachineManager)
	{
		delete m_stateMachineManager;
	}

    SAFE_DELETE( m_npc_mesh );
    SAFE_DELETE( m_movement );
    SAFE_DELETE( m_debug_line );
    SAFE_DELETE( m_proj_movement );
    SAFE_DELETE( m_projectile );
    SAFE_DELETE( m_hud );
    SAFE_DELETE( m_health_pack );
    SAFE_DELETE( m_hp_part_sys );
}

void GameObject::CreateStateMachineManager( void )
{
	m_stateMachineManager = new StateMachineManager( *this );
}

/*---------------------------------------------------------------------------*
  Name:         Update

  Description:  Calls the update function of the currect state machine.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void GameObject::Update( void )
{
	if(m_stateMachineManager)
	{
		m_stateMachineManager->Update();
	}
}

void GameObject::Animate(float fElapsedTime, double dTime, const D3DXVECTOR3 *cameraFacing)
{
    if( m_movement )
    {
        m_movement->Animate( fElapsedTime );
    }
    else if( m_proj_movement )
    {
        m_proj_movement->Animate( fElapsedTime, cameraFacing );
    }
    else if( m_health_pack && m_render_hp)
    {
        m_health_pack->Update((float)dTime);
    }
    else if( m_health_pack && m_hp_part_sys && !m_render_hp)
    {
        m_hp_part_sys->Update( fElapsedTime, cameraFacing );
    }
}

void GameObject::createNPC(IDirect3DDevice9 *pd3dDevice, LPCWSTR filename)
{
    m_npc_mesh = new Mesh();
    m_npc_mesh->Setup(pd3dDevice, filename);

    // create a debug line
    m_debug_line = new CLine();
    m_debug_line->lineInit(pd3dDevice);
}

void GameObject::createProjectile( IDirect3DDevice9 *pd3dDevice )
{
    m_projectile = new CProjectile();
    m_projectile->Init( pd3dDevice );
    
}

void GameObject::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX *pMatViewProj, bool shadowPass)
{
    // render the mesh
    if( m_npc_mesh )
    {
        m_npc_mesh->Render(pd3dDevice, *pMatViewProj, shadowPass );
    }

    // render the projectile
    if( m_projectile )
    {
        m_projectile->Render(pd3dDevice, *pMatViewProj, shadowPass );
    }

    // render debug line
    if( m_debug_line && !shadowPass &&  m_movement && m_debug)
    {
        //m_debug_line->Render(pd3dDevice, *pMatViewProj);
        m_movement->DrawDebugVisualization(pd3dDevice,pMatViewProj);
     
        if( m_debugAStar ) // a star debugging
            m_movement->DrawAstarDebug(pd3dDevice,pMatViewProj);
    }

    // render the hud
    if( m_hud && !shadowPass )
    {
        m_hud->Render(); 
    }

    // render health pack
    if( m_health_pack && m_render_hp)
    {
        m_health_pack->Render(pd3dDevice, *pMatViewProj, shadowPass);
    }
    else if( m_health_pack && m_hp_part_sys && !shadowPass)
    {
        m_hp_part_sys->Render(pd3dDevice, *pMatViewProj );
    }
}

void GameObject::Release()
{
    // release the mesh
    if( m_npc_mesh )
    {
        m_npc_mesh->Release();
        SAFE_DELETE( m_npc_mesh );
    }

    // release the line
    if( m_debug_line )
    {
        m_debug_line->Release();
        SAFE_DELETE(m_debug_line);
    }

    // release the projecile
    if(m_projectile)
    {
        m_projectile->Release();
        SAFE_DELETE(m_projectile);
    }

    if( m_hud )
    {
        m_hud->Release();
        SAFE_DELETE(m_hud);
    }

    if( m_health_pack )
    {
        m_health_pack->Release();
        SAFE_DELETE(m_health_pack);
    }
    if( m_hp_part_sys )
    {
        m_hp_part_sys->Release();
        SAFE_DELETE(m_hp_part_sys);
    }

    SAFE_DELETE( m_movement );

    SAFE_DELETE( m_proj_movement );
}