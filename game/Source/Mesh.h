#pragma once

#include "DXUT\SDKmisc.h"
#include "collision.h"
#include "line.h"


enum npc_states
{
    SEEK_PLAYER,
    IDLE,
    SEEK_RANDOM_LOCATION

};

class Mesh : public CollPerson
{
public:
    Mesh();
    ~Mesh();

    HRESULT Setup(IDirect3DDevice9* pd3dDevice, LPCWSTR filename);

    void Animate( float fElapsedTime );

    void Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld, bool shadowPass);

    void Release();

    D3DXVECTOR3 getPosition();
    D3DXVECTOR3 getFacing();
    D3DXVECTOR3 getSeekingPosition();

    void setSeekPosition(D3DXVECTOR3 seekPosition);

    void SetPosition(D3DXVECTOR3 pos);
    void setFacingVec(D3DXVECTOR3 facing){ m_facing_vec = facing;};
    D3DXVECTOR3 getDiffPosDest();

    void setScale( float scale );

    
//private:

    void randomLocation();

    void InitShaders(IDirect3DDevice9* pDevice, bool material);

    D3DXMATRIX m_Orientation;
    D3DXMATRIX m_World;
    LPD3DXMESH pMesh; // mesh object

    D3DMATERIAL9* pMeshMaterials;
    LPDIRECT3DTEXTURE9* pMeshTextures;

    DWORD numMaterials;

    D3DXVECTOR3 goToPosition;
    D3DXVECTOR3 diffPosition;
    D3DXVECTOR3 currPosition;
    float       m_facing;
    D3DXVECTOR3 m_facing_vec;
    float       m_ftimewaiting;
    npc_states  m_state;

    IDirect3DStateBlock9* m_pStateBlock;

    IDirect3DVertexShader9 *m_pVertexShader;
    IDirect3DPixelShader9 *m_pPixelShader;
};

class Skybox : public Mesh
{
public:
    //Skybox();
    
    void Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);
    void InitSkybox();
    void SetPosition(D3DXVECTOR3 pos);
};

class CBomb : public Mesh
{
    public:
    void SetPosition(D3DXVECTOR3 pos);
    D3DXVECTOR3 velocity;
};