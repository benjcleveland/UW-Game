/**************************** COLLISION.CPP *****************************/

/*
collision.cpp - collision data definitions and collision detection routines. 

Copyright (c) 2007 Dan Chang. All rights reserved. 

This software is provided "as is" without express or implied 
warranties. You may freely copy and compile this source into 
applications you distribute provided that the copyright text 
below is included in the resulting source code, for example: 
"Portions Copyright (c) 2007 Dan Chang" 
*/


#include "DXUT.h"
#include "collision.h"
#include "Mesh.h"
#include "database.h"
#include "gameobject.h"
#include "statemch.h"
#include "deathsm.h"

const float epsilon = 0.000005f;

CollOutput  gCollOutput;

static CollSphere aCollSphere[cMobyMax];
static int iMobyLast = 0;

extern VecCollQuad vecCollQuad;


/*
CollSphere::VsMobys - Tests for collisions against all mobys. 
Returns true if a collision was found or false if not. 
*/
bool CollSphere::VsMobys()
{
    bool  ret = false;

    gCollOutput.Reset();
    for (int i = 0; i < iMobyLast; ++i)
    {
        // Don't do collision test against self. 
        if (this != &aCollSphere[i] && VsSphere(&aCollSphere[i]))
            ret = true;
    }
    return ret;
} // CollSphere::VsMobys


bool CollSphere::VsQuad(const CollQuad& quad) const
/************************************************************************/
/* Collision test between sphere and a quadrangle.                      */
/* Returns true if they collide and false if not.                       */
/************************************************************************/
{
    // First find distance from center of sphere to plane of quad
    D3DXVECTOR3    v(center - quad.point[0]);
    float t = D3DXVec3Dot(&v, &quad.normal);

    // Added epsilon because otherwise it would sometimes return a very small 
    // push that wouldn't eliminate the collision. 
    // BUGBUG: is it better to put the epsilon in the collision test as 
    // implemented, or put the add the epsilon to the push value? 
    if ((t > 0.0f) && (t + epsilon < radius))
    {     // The plane is within radius of the sphere
        D3DXVECTOR3 p(center - t*quad.normal); // Get point in plane closest to center of sphere

        if (PointInQuad(p, quad))
        {   // If that point is inside the quadrangle - you're done
            t = radius - t;
            if (t > gCollOutput.length)
            {
                gCollOutput.length = t;
                gCollOutput.normal = quad.normal;
                gCollOutput.point = p;
                gCollOutput.push = t * quad.normal;
            }
            return true;
        }
        else
        {   // If that point is outside the triangle, then we have to check each line of the
            // tri for the closest point to the center of the sphere.
            float     radSq;
            float     min;
            D3DXVECTOR3        closest;

            radSq = min = radius * radius;
            for (int i = 0; i < 4; ++i)
            {
                // Make a line for each edge of the quadrangle
                int j = i + 1;
                if (j == 4)
                    j = 0;
                CollLine  line;
                line.Set(&quad.point[i], &quad.point[j]);
                // Get the point on that line closest to the sphere
                t = line.ClosestPoint(p, center);
                // Cap to within the bounds of the line
                if (t > line.length)
                    p = quad.point[j];
                else if (t < 0.0f)
                    p = quad.point[i];
                // How far from sphere is that point?
                v = center - p;
                t = D3DXVec3Dot(&v, &v);
                if (t < min)   // Keep track of closest point
                {
                    closest = p;
                    min = t;
                }
            }
            if (min < radSq)     // Was any point within radius of the sphere?
            {
                t = radius - sqrtf(min);
                if (t > gCollOutput.length)   // Update the collision data
                {
                    gCollOutput.length = t;
                    gCollOutput.normal = quad.normal;
                    gCollOutput.point = closest;
                    v = center - closest;
                    D3DXVec3Normalize(&v, &v);
                    gCollOutput.push = t * v;
                }
                return( TRUE );
            }
        }

        return( FALSE );
    }

    return( FALSE );
} // CollSphere::VsQuad


bool CollSphere::VsSphere(CollSphere *sphere)
/************************************************************************/
/* Collision test between sphere and another sphere.                    */
/* Returns TRUE if the 2 spheres collide and FALSE if not.              */
/************************************************************************/
{
    float d2, r2;
    D3DXVECTOR3    diff;

    diff = center - sphere->center;
    d2 = diff.x*diff.x + diff.y*diff.y + diff.z*diff.z;   // distance squared

    r2 = radius + sphere->radius;   // Sum of radii
    r2 = r2 * r2;                   // Sum of radii squared

    if( d2 < r2 )   // Collision has occurred
    {
        // Optimize this later - probably just save the 2 spheres and only calculate the other
        // info when asked for it.
        float length;
        length = sqrtf(r2) - sqrtf(d2);     // by how much?
        if( length > gCollOutput.length )   // Update the collision data
        {
            gCollOutput.type = COLL_SPHERE_VS_SPHERE;
            gCollOutput.length = length;
            D3DXVec3Normalize(&gCollOutput.normal, &diff);
            gCollOutput.push = gCollOutput.length * gCollOutput.normal;
            gCollOutput.point = sphere->center + sphere->radius*gCollOutput.normal;
        }
        return( TRUE );
    }

    return( FALSE );
}

void CollLine::Set(const D3DXVECTOR3* p1, const D3DXVECTOR3* p2)
/************************************************************************/
/* Set up a collision line.                                             */
/* p1 and p2 are pointers to the 2 end points.                          */
/************************************************************************/
{
    start = *p1;
    end = *p2;
    dir = end - start;
    length = D3DXVec3Length(&dir);
    dir.x /= length;    // Normalize -- since length is known this should be 
    dir.y /= length;    // faster than calling dir.Normalize()
    dir.z /= length;
}


bool CollLine::VsQuad(const CollQuad& quad) const
/************************************************************************/
/* Collision test between line and a quadrangle.                        */
/* Returns true if they collide and false if not.                       */
/************************************************************************/
{
    float denom = D3DXVec3Dot(&dir, &quad.normal);
    if (denom == 0.0f)   // The line and quadrangle are parallel
        return false;      // I'm considering parallel to be a miss

    // get the intersection of the line and the plane
    D3DXVECTOR3    diff(start - quad.point[0]);
    float t = -D3DXVec3Dot(&diff, &quad.normal);
    t = t / denom;

    if ((t > 0.0f) && (t < length))
    {   // The intesection lies between start and end on the line
        diff = t * dir;
        D3DXVECTOR3    point(start + diff);
        float len = length - t;

        if (PointInQuad(point, quad))
        {   // There is an intersection
            if (len > gCollOutput.length)   // Update the collision data
            {
                // Optimize this later - probably just save the line and quad and only calculate the other
                // info when asked for it.
                gCollOutput.type = COLL_LINE_VS_QUAD;
                gCollOutput.length = len;
                gCollOutput.point = point;
                gCollOutput.normal = quad.normal;
                gCollOutput.push = gCollOutput.point - end;
            }
            return true;
        }
    }
    return false;       // The plane is too far away from the line
} // CollLine::VsQuad


float CollLine::ClosestPoint(D3DXVECTOR3& p, const D3DXVECTOR3& c) const
/************************************************************************/
/* Sets p to the point on the line closest to point c.                  */
/* Returns how far along the line that point is with 0.0 being the      */
/* start point and length being the end point.                          */
/************************************************************************/
{
    D3DXVECTOR3      v(c - start);
    float   t = D3DXVec3Dot(&v, &dir);
    p = (t * dir) + start;

    return( t );
}


bool PointInQuad(const D3DXVECTOR3& p3, const CollQuad& quad)
/************************************************************************/
/* Check whether the point p3 is inside the quadrangle quad.            */
/* Returns true if the p3 is in quad or false if not.                   */
/* Does not check whether p3 lies within the plane of quad, only whether*/
/* the projection of p3 onto that plane is inside the quadrangle quad.  */
/************************************************************************/
{
    for (int i = 0; i < 4; ++i)
    {
        int j = i + 1;
        if (j == 4)
            j = 0;
        D3DXVECTOR3  diff(p3 - quad.point[i]);
        D3DXVECTOR3  diffTri(quad.point[j] - quad.point[i]);
        D3DXVec3Cross(&diff, &diffTri, &diff);
        float dot = D3DXVec3Dot(&diff, &quad.normal);
        if (dot < 0.0f)           // The point is outside the quadrangle 
            return false;
    }
    return true;
} // PointInQuad

/*
AppendMobyCollSphere - Copy data to Moby CollSphere array, return 
pointer to array entry where we've stored the data. 
*/ 
CollSphere* AppendMobyCollSphere(CollSphere& sphere)
{
    assert(iMobyLast < cMobyMax);         // trying to add too many CollSpheres
    aCollSphere[iMobyLast++] = sphere;
    return &aCollSphere[iMobyLast - 1];
} // AppendMobyCollSphere


// collision person
CollPerson::CollPerson() : m_xcollision ( false), m_zcollision(false), m_collperson(false){}

CollPerson::~CollPerson(){}


void CollPerson::setXColl(bool x)
{
    m_xcollision = x;
}
void CollPerson::setZColl(bool z)
{
    m_zcollision = z;
}

bool CollPerson::getXColl()
{
    return m_xcollision;
}

bool CollPerson::getZColl()
{
    return m_zcollision;
}

bool CollPerson::getCollPerson()
{
    return m_collperson;
}

void CollPerson::setCollPerson(bool pers)
{
    m_collperson = pers;
}

void runCollisionDetection( CollPerson *collPerson, float radius, D3DXVECTOR3 position, D3DXVECTOR3 facing)
{
    // collision detection
    CollSphere player_sphere;
    player_sphere.Set(&position, radius);

    for(unsigned int i = 0; i < vecCollQuad.size(); ++i )
    {
        if( player_sphere.VsQuad(vecCollQuad[i]) == true )
        {
            if( D3DXVec3Dot(&facing, &gCollOutput.normal) < 0)
            {
                // collision
                if(gCollOutput.normal.z != 0 )
                    collPerson->setZColl(true);
                if( gCollOutput.normal.x != 0 )
                    collPerson->setXColl(true);
            }
            gCollOutput.Reset();
        }
    }
}

void runPlayerNpcCollDetection(CollPerson *collPerson, CollPerson *collNpc, float radius, D3DXVECTOR3 position, D3DXVECTOR3 npc_pos, D3DXVECTOR3 facing, D3DXVECTOR3 npcfacing)
{
    // collision detection
    CollSphere player_sphere;
    CollSphere npc_sphere;
    player_sphere.Set(&position, radius);
    npc_sphere.Set(&npc_pos, radius);

    if( player_sphere.VsSphere(&npc_sphere) == true )
    {

        if(facing.z * npcfacing.z < -.2f )
        {
            collPerson->setZColl(true);
            collNpc->setZColl(true);
        }
        if( facing.x * npcfacing.x < -.2f )
        {
            collPerson->setXColl(true);
            collNpc->setXColl(true);
        }
        collPerson->setCollPerson(true);
    } 
}


// this function needs to be cleaned up
void runPlayerProjCollDetection(D3DXVECTOR3 position,D3DXVECTOR3 facing )
{
    dbCompositionList list;
	g_database.ComposeList( list, OBJECT_Projectile );
	dbCompositionList::iterator i;
	for( i=list.begin(); i!=list.end(); ++i )
	{
		StateMachine* pStateMachine = (*i)->GetStateMachineManager()->GetStateMachine(STATE_MACHINE_QUEUE_0);
		if( pStateMachine && pStateMachine->GetState() == 2) // we only care if its in the 'launched' state
		{
			char* name = (*i)->GetName();
            GameObject *go = g_database.FindByName( name );
            if( go  )
            {
                // collision detection
                CollSphere player_sphere;
                CollSphere npc_sphere;
                player_sphere.Set(&position, .1f);
                D3DXVECTOR3 npc_pos = go->getProjMovemement()->getPosition();
                npc_pos.y = 0; // make sure the spheres are in the same plane, the projectile should never be above the player
                npc_sphere.Set(&npc_pos, .1f);

                if( player_sphere.VsSphere(&npc_sphere) == true )
                {
                    // there was a collision so send a message to the state machine
                    g_database.SendMsgFromSystem( go, MSG_CollisionPlayer, MSG_Data());
                    
                    // tell the player he was hit
                    g_database.SendMsgFromSystem(g_database.FindByName("Gameflow"),MSG_PlayerHitByProjectile, MSG_Data());
                } 
            }
        }
    }
}

// projectile wall collisions
void runProjWallCollDetection()
{
    dbCompositionList list;
	g_database.ComposeList( list, OBJECT_Projectile | OBJECT_PlayerProjectile );
	dbCompositionList::iterator i;
	for( i=list.begin(); i!=list.end(); ++i )
	{
		StateMachine* pStateMachine = (*i)->GetStateMachineManager()->GetStateMachine(STATE_MACHINE_QUEUE_0);
		if( pStateMachine && pStateMachine->GetState() == 2) // we only care if its in the 'launched' state
		{
			char* name = (*i)->GetName();
            GameObject *go = g_database.FindByName( name );
            if( go )
            {
                D3DXVECTOR3 position = go->getProjMovemement()->getPosition() ;
                CollSphere proj_sphere;
                position.y = 0;
                proj_sphere.Set(&position, .1f);

                for(unsigned int i = 0; i < vecCollQuad.size(); ++i )
                {
                    if( proj_sphere.VsQuad(vecCollQuad[i]) == true )
                    {
                        // collision
                        g_database.SendMsgFromSystem( go, MSG_CollisionWall, MSG_Data(gCollOutput.normal));
                        gCollOutput.Reset();
                        //break;
                    }
                }
            }
        }
    }
}

void runProjNpcCollDetection( )
{
    bool coll = false;
    GameObject *proj_go = g_database.FindByName("PLAYPROJ0");
    if( proj_go && proj_go->getProjectile()->getRenderType() == RENDER_BOMB )
    {
        dbCompositionList list;
        g_database.ComposeList( list, OBJECT_NPC );
        dbCompositionList::iterator i;
        D3DXVECTOR3 position = proj_go->getProjMovemement()->getPosition() ;
        CollSphere proj_sphere;
        position.y = 0;
        proj_sphere.Set(&position, .1f);

        for( i=list.begin(); i!=list.end(); ++i )
        {
            StateMachine* pStateMachine = (*i)->GetStateMachineManager()->GetStateMachine(STATE_MACHINE_QUEUE_0);
            if( pStateMachine && strcmp(pStateMachine->GetCurrentStateNameString(),"STATE_Dead") != 0 ) 
            {
                char* name = (*i)->GetName();
                GameObject *go = g_database.FindByName( name );
                if( go  )
                {
                    // collision detection
                    CollSphere npc_sphere;
                    
                   
                    D3DXVECTOR3 npc_pos = go->getMovement()->getPosition();
                    npc_pos.y = 0; // make sure the spheres are in the same plane, the projectile should never be above the player
                    npc_sphere.Set(&npc_pos, .2f);

                    if( proj_sphere.VsSphere(&npc_sphere) == true )
                    {
                        coll = true;
                        // there was a collision so send a message to the state machine
                        g_database.SendMsgFromSystem( proj_go, MSG_CollisionPlayer, MSG_Data());
                        // tell the NPC it was hit
                        g_database.SendMsgFromSystem( go, MSG_NPCHitByProjectile, MSG_Data());
                    } 
                }
            }
        }
    }
}


bool runNPCSeePlayer(D3DXVECTOR3 *npc_pos, D3DXVECTOR3 *player_pos)
{
     // determine if we can see the player
    CollLine line;
    bool collision_detected = true;
    line.Set(npc_pos, player_pos);

    // check if we can see the player, ie there are no collisions with any walls
    for( unsigned int i =0; i < vecCollQuad.size(); ++i)
    {
        if( line.VsQuad((vecCollQuad)[i]) == true )
        {
            // there was a collision, so seek a random location
            collision_detected = false;
            break;
        }
    }
    // reset the collision output
    gCollOutput.Reset();
    return collision_detected;
}

bool runNPCSeePlayer(D3DXVECTOR3 *npc_pos, D3DXVECTOR3 *player_pos, D3DXVECTOR3 &point)
{
     // determine if we can see the player
    CollLine line;
    bool collision_detected = true;
    line.Set(npc_pos, player_pos  );

    // check if we can see the player, ie there are no collisions with any walls
    for( unsigned int i =0; i < vecCollQuad.size(); ++i)
    {
        if( line.VsQuad((vecCollQuad)[i]) == true )
        {
            // there was a collision, so seek a random location
            collision_detected = false;
           // break;
            if(gCollOutput.length < line.length)
                point = gCollOutput.point;
        }
    }

    point = gCollOutput.point;
    // reset the collision output
    gCollOutput.Reset();
    return collision_detected;
}


// player healthpack collision dection
void runPlayerHpCollDetection(D3DXVECTOR3 position )
{
    dbCompositionList list;
	g_database.ComposeList( list, OBJECT_Item);
	dbCompositionList::iterator i;
	for( i=list.begin(); i!=list.end(); ++i )
	{
		StateMachine* pStateMachine = (*i)->GetStateMachineManager()->GetStateMachine(STATE_MACHINE_QUEUE_0);
        if( pStateMachine && pStateMachine->GetState() == 0) // we only care if its in the 'availalbe' state
		{
			char* name = (*i)->GetName();
            GameObject *go = g_database.FindByName( name );
            if( go  )
            {
                // collision detection
                CollSphere player_sphere;
                CollSphere npc_sphere;
                player_sphere.Set(&position, .1f);
                D3DXVECTOR3 npc_pos = go->getHealthPack()->getPosition();
                npc_pos.y = 0; // make sure the spheres are in the same plane, the projectile should never be above the player
                npc_sphere.Set(&npc_pos, .1f);


                if( player_sphere.VsSphere(&npc_sphere) == true )
                {
                    // there was a collision so send a message to the state machine
                    g_database.SendMsgFromSystem( go, MSG_PlayerGotHealthPack, MSG_Data());
                    
                    // tell the player he was hit
                    g_database.SendMsgFromSystem(g_database.FindByName("Gameflow"),MSG_PlayerGotHealthPack, MSG_Data());
                } 
            }
        }
    }
}