/* Copyright Steve Rabin, 2010. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2010"
 */

#include "DXUT.h"
#include "database.h"
#include "projectile_movement.h"
#include "gameobject.h"
//#include "body.h"
#include "tiny.h"
#include "statemch.h"

// constructor
ProjectileMovement::ProjectileMovement( GameObject& owner )
: m_owner( &owner ),
  m_speedWalk( 1.f / 5.7f ),
  m_speedJog( 1.f / 2.3f ),
  m_facing(0)
{
	m_target.x = m_target.y = m_target.z = 0.0f;
}

// destrctor
ProjectileMovement::~ProjectileMovement( void )
{

}

// animate function that gets called every frame
void ProjectileMovement::Animate( float fTimeDelta, const D3DXVECTOR3 *cameraFacing )
{
    
    if( m_owner->getProjectile()->getRenderType() == RENDER_BOMB )
    {
        D3DXVECTOR3 force(0,-.02f,0);
        // determine the position of the bomb
        // update the position of the billboard
        m_velocity += force * fTimeDelta;
        // update the postion
        m_position = m_position + m_velocity;
        // set the position of the projectile
        m_owner->getProjectile()->setPosition( m_position );

        // check to see if the projectile hit the ground
        if( m_position.y <= 0 )
        {
            g_database.SendMsgFromSystem( m_owner, MSG_ProjectileHit);
        }
    }
    else if( m_owner->getProjectile()->getRenderType() == RENDER_PARTICAL_SYS || m_owner->getProjectile()->getRenderType() == RENDER_PS_DECAL )
    {
        // update the partical system
        m_owner->getProjectile()->UpdatePartSys( fTimeDelta, cameraFacing );
    }
}


void ProjectileMovement::SetIdleSpeed( void )
{
//	m_owner->GetBody().SetSpeed( 0.0f );
	//m_owner->GetTiny().SetIdleKey( true );
}

void ProjectileMovement::SetWalkSpeed( void )
{
//	m_owner->GetBody().SetSpeed( m_speedWalk );
	//m_owner->GetTiny().SetMoveKey();
}

void ProjectileMovement::SetJogSpeed( void )
{
//	m_owner->GetBody().SetSpeed( m_speedJog );
	//m_owner->GetTiny().SetMoveKey();
}

// set the target
void ProjectileMovement::setTarget( D3DXVECTOR3 &target)
{
    m_target = target;
    m_diffPosition = m_target - m_position;
    // normalize the diff position so the NPC will always move the same speed
    D3DXVec3Normalize(&m_diffPosition,&m_diffPosition);
}

void ProjectileMovement::setRandomTarget()
{
    D3DXVECTOR3 goToPosition;

    // set to random location between 0 and 25
    goToPosition.x = 24.0f * rand() / RAND_MAX;
    goToPosition.z = 24.0f * rand() / RAND_MAX;
    goToPosition.y = 0;

    // update the target
    setTarget( goToPosition );
}