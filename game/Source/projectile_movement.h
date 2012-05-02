/* Copyright Steve Rabin, 2010. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2010"
 */

#pragma once

#include "movement.h"

class GameObject;


class ProjectileMovement
{
public:
	ProjectileMovement( GameObject& owner );
	~ProjectileMovement( void );

	void setTarget( D3DXVECTOR3& target );
	inline D3DXVECTOR3& getTarget( void )					{ return( m_target ); }

    inline void setPosition( D3DXVECTOR3& position )			{ m_position = position; }
	inline D3DXVECTOR3& getPosition( void )					{ return( m_position ); }

    inline void setVelocity( D3DXVECTOR3 velocity ) { m_velocity = velocity; };
    inline D3DXVECTOR3 getVelocity() { return m_velocity; };

	void Animate( float fTimeDelta, const D3DXVECTOR3 *cameraFacing );

    void setRandomTarget(void);

	void SetIdleSpeed( void );
	void SetWalkSpeed( void );
	void SetJogSpeed( void );

    float getFacing() { return m_facing; };

    inline D3DXVECTOR3 getFacingVec() { D3DXVECTOR3 diff =  m_target - m_position; D3DXVec3Normalize( &diff, &diff); return diff; }
protected:

	GameObject* m_owner;

	D3DXVECTOR3 m_target;
    
    D3DXVECTOR3 m_position;
    D3DXVECTOR3 m_diffPosition;

    float m_facing;

    D3DXVECTOR3 m_velocity;

	float m_speedWalk;
	float m_speedJog;

};