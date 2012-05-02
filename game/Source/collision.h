/**************************** COLLISION.H *******************************/

/*
    Copyright (c) 2007 Dan Chang. All rights reserved. 

    This software is provided "as is" without express or implied 
    warranties. You may freely copy and compile this source into 
    applications you distribute provided that the copyright text 
    below is included in the resulting source code, for example: 
    "Portions Copyright (c) 2007 Dan Chang" 
 */

#pragma once

#include <vector>


/************************************************************************/
/* CONSTANTS                                                            */
/************************************************************************/

enum COLL_ENUM
{
  COLL_NONE,                // 0
  COLL_SPHERE_VS_SPHERE,
  COLL_SPHERE_VS_TRI,
  COLL_LINE_VS_QUAD, 
  COLL_LINE_VS_SPHERE,
  COLL_LINE_VS_TRI,

  COLL_TOTAL
};

const int cMobyMax  = 64;               // never more than cMobyMax active

/************************************************************************/
/* CLASSES & STRUCTURES                                                 */
/************************************************************************/


class CollOutput
{
public:
    char        type;
    float       length;
    D3DXVECTOR3 normal;
    D3DXVECTOR3 push;
    D3DXVECTOR3 point;

    void Reset()
    {
        type = COLL_NONE;
        length = -1.0f;
    };
};

struct _CollSphere
{
    D3DVECTOR center;
    float radius;
};

class CollSphere
{
public:
    D3DXVECTOR3 center;
    float radius;

    void Set(const D3DVECTOR *p, const float r)    
    { 
        center = *p; 
        radius = r;
    };
    void Set(const float cx, const float cy, const float cz, const float r)
    {
        center.x = cx;
        center.y = cy;
        center.z = cz;
        radius = r;
    };

    bool  VsEnv();
    bool  VsMobys();

    bool  VsQuad(const class CollQuad& quad) const;
    bool  VsSphere(CollSphere *sphere);
    bool  VsTri(const class CollTri& tri) const;
};

class CollLine
{
public:
    D3DXVECTOR3 start;
    D3DXVECTOR3 end;
    D3DXVECTOR3 dir;
    float length;

    void  Set(const D3DXVECTOR3* s, const D3DXVECTOR3* e);
    void  Set(D3DXVECTOR3 *p, D3DXVECTOR3 *v, float l);

    bool  VsEnv();

    bool  VsQuad(const class CollQuad& quad) const;
    char  VsSphere(CollSphere *sphere);
    bool  VsTri(const class CollTri& tri) const;
    float ClosestPoint(D3DXVECTOR3& p, const D3DXVECTOR3& c) const;
};

struct _CollQuad
{
    D3DVECTOR point[4];
    D3DVECTOR normal;
};

class CollQuad
{
public:
    D3DXVECTOR3 point[4];
    D3DXVECTOR3 normal;

    CollQuad() { }
    CollQuad(const D3DVECTOR& p1, const D3DVECTOR& p2, const D3DVECTOR& D3DXVECTOR3, const D3DVECTOR& p4);
    void  Set(const D3DVECTOR& p1, const D3DVECTOR& p2, const D3DVECTOR& D3DXVECTOR3, const D3DVECTOR& p4)
    {
        point[0] = p1;  point[1] = p2;  point[2] = D3DXVECTOR3;  point[3] = p4; D3DXVec3Cross(&normal, &point[1], &point[2]);// D3DXVec3Normalize(&normal, &normal);
    };
    void  Set(const D3DVECTOR& p1, const D3DVECTOR& p2, const D3DVECTOR& D3DXVECTOR3, const D3DVECTOR& p4, const D3DVECTOR& n)
    {
        point[0] = p1;  point[1] = p2;  point[2] = D3DXVECTOR3;  point[3] = p4;  normal = n;
    };
};

class CollPerson
{
public:
    CollPerson();
    ~CollPerson();


    void setXColl(bool x);
    void setZColl(bool z);
    void setCollPerson(bool pers);

    bool getXColl();
    bool getZColl();
    bool getCollPerson();

private:
    bool m_xcollision;
    bool m_zcollision;
    bool m_collperson;
};


typedef std::vector<CollQuad> VecCollQuad;

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

bool PointInQuad(const D3DXVECTOR3& D3DXVECTOR3, const CollQuad& quad);

CollSphere* AppendMobyCollSphere(CollSphere& sphere);

void runCollisionDetection( CollPerson *collPerson, float radius, D3DXVECTOR3 position, D3DXVECTOR3 facing);
void runPlayerNpcCollDetection(CollPerson *collPerson, CollPerson *collNpc, float radius, D3DXVECTOR3 position, D3DXVECTOR3 npc_pos, D3DXVECTOR3 facing,D3DXVECTOR3 npcfacing);

// this function will determine if the NPC can 'see' the player and return true if it does
bool runNPCSeePlayer( D3DXVECTOR3 *npc_pos, D3DXVECTOR3 *player_pos);
bool runNPCSeePlayer( D3DXVECTOR3 *npc_pos, D3DXVECTOR3 *player_pos, D3DXVECTOR3 &point);

void runPlayerProjCollDetection(D3DXVECTOR3 position,D3DXVECTOR3 facing );
void runProjNpcCollDetection(  );
void runPlayerHpCollDetection(D3DXVECTOR3 position );
void runProjWallCollDetection();
/************************************************************************/
/* EXTERNS                                                              */
/************************************************************************/

extern CollOutput gCollOutput;
