/* Copyright Steve Rabin, 2010. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2010"
 */

#include <list>
#include "WorldFile.h"

#pragma once

class GameObject;

typedef std::list<D3DXVECTOR3> WaypointList;

enum MovementMode
{
	MOVEMENT_NULL,
	MOVEMENT_SEEK_TARGET,
    MOVEMENT_COMPUTE_A_STAR,
    MOVEMENT_ROTATE,
    MOVEMENT_SCALE,
	MOVEMENT_WAYPOINT_LIST
};

struct gameNode
{
    int Row;
    int Col;

    int parentRow;
    int parentCol;
    //Node *parentPtr;

    float cost;

    float total;
};

class Movement
{
public:
    Movement(){};
	Movement( GameObject& owner );
	~Movement( void );

	void setTarget( D3DXVECTOR3& target );
	inline D3DXVECTOR3& getTarget( void )					{ return( m_target ); }

    void setStartPosition( D3DXVECTOR3& position );
	inline D3DXVECTOR3& getPosition( void )					{ return( m_position ); }

	void Animate( double dTimeDelta );

    void setRandomTarget(void);

	void SetIdleSpeed( void );
	void SetWalkSpeed( void );
	void SetJogSpeed( void );

    inline float getFacing() { return m_facing; };

    inline D3DXVECTOR3 getFacingVec() { D3DXVECTOR3 diff =  m_target - m_position; D3DXVec3Normalize( &diff, &diff); return diff; }
    
    // set movement mode to rotate
    void setMovementModeRotate() { m_movementMode = MOVEMENT_ROTATE; };

    void setMovementDead();

    // debug line drawing function
    void DrawDebugVisualization( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj );
    void DrawAstarDebug(IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj );

    // a* functions
    void setDestLocation( D3DXVECTOR3 pos);

    // reset - start a new game
    void reset();
    
protected:

    void ComputePathAStar();
    void ComputeRubberbandPath();
    void SmoothCurrentWaypoints( void );

	GameObject* m_owner;

	D3DXVECTOR3 m_target;
    
    D3DXVECTOR3 m_position;
    D3DXVECTOR3 m_diffPosition;

    D3DXVECTOR3 m_orig_position;    // position to reset to

    WaypointList m_waypointList;

    MovementMode m_movementMode;

    float m_facing;

	float m_speedWalk;
	float m_speedJog;

    WorldFile g_terrain;

        // lists
    // open list
    std::list<gameNode> OpenList;

    // closed list
    std::list<gameNode> ClosedList;

    int m_row;
    int m_col;

    float m_size;
};
