

#pragma once

#include "DXUT\SDKmisc.h"

class cMiniMap
{
public:

    cMiniMap();
    ~cMiniMap();

    void Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);
    void InitMiniMap(IDirect3DDevice9* pd3dDevice);
    void SetPositionAndFacing(D3DXVECTOR3 pos, float facing);
    void Release(void);

private:

    // A structure for our custom vertex type
    struct MINIMAPVERTEX
    {
        FLOAT x, y, z, rwh;      // The untransformed, 3D position for the vertex
        FLOAT tu, tv;       // The texture coordinates
        FLOAT tu2, tv2; 
        FLOAT tu3, tv3;
    };

    float m_facing;
    D3DXVECTOR3 m_player_pos;

    LPDIRECT3DTEXTURE9      m_pMapAlphaTexture;
    LPDIRECT3DTEXTURE9      m_pMapTexture;
    LPDIRECT3DTEXTURE9      m_pPlayerMark;

    D3DXMATRIX m_World;
    MINIMAPVERTEX *vertices;
    MINIMAPVERTEX *npc_vertices;
    IDirect3DStateBlock9* pStateBlock;
};
