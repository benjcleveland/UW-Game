
#pragma once

#include <stddef.h>
#include "Mesh.h"
#include "DXUT\SDKsound.h"
#include "Tiny.h"

struct BillboardVertex
{
    D3DXVECTOR3 position;
    D3DXCOLOR diffuse;
    D3DXVECTOR2 texCoord;
   // D3DXVECTOR3 normal;
    
    static D3DVERTEXELEMENT9 const declaration[];
};

#if ParticalSystem_cpp
D3DVERTEXELEMENT9 const BillboardVertex::declaration[] =
{
    { 0, offsetof(BillboardVertex, position     ), D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION, 0 },
    //{ 0, offsetof(BillboardVertex, normal       ), D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_NORMAL  , 0 },
    { 0, offsetof(BillboardVertex, diffuse ), D3DDECLTYPE_D3DCOLOR, 0, D3DDECLUSAGE_COLOR   , 0 },
    D3DDECL_END(),
};
#endif

struct DecalVertex
{
    D3DXVECTOR3 position;
    D3DXVECTOR4 texCoord;
    
    static D3DVERTEXELEMENT9 const declaration[];
};

#if ParticalSystem_cpp
D3DVERTEXELEMENT9 const DecalVertex::declaration[] =
{
    { 0, offsetof(DecalVertex, position     ), D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION, 0 },
    { 0, offsetof(DecalVertex, texCoord), D3DDECLTYPE_FLOAT4  , 0, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END(),
};
#endif


class CBillboard
{
public:
    CBillboard();
    ~CBillboard();

    void Release( void );
    void Init(IDirect3DDevice9* pd3dDevice);

    void SetPosition( D3DXVECTOR3 pos );
    D3DXVECTOR3 GetPosition( void );

    void Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);
    void Update( float fElapsedTime, const D3DXVECTOR3 *cameraFacing );

    

private:

    IDirect3DVertexShader9 *m_pVertexShader;
    IDirect3DPixelShader9 *m_pPixelShader;
    
    IDirect3DVertexDeclaration9* pVD;
    
    LPDIRECT3DVERTEXBUFFER9 m_pVB;

    BillboardVertex *m_vertices;

    D3DXMATRIX m_Orientation;
    D3DXMATRIX m_World;
    D3DXVECTOR3 m_position;
};

struct Partical
{
    D3DXVECTOR3 velocity;
    D3DXVECTOR3 initvelocity;
    bool RenderState;           // whether or not to render this partical

    CBillboard m_billboard;
};

class CParticalSystem
{
public:
    CParticalSystem();
    ~CParticalSystem();

    void Release( void );
    void Init(IDirect3DDevice9* pd3dDevice);

    void Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);
    void Update( float fElapsedTime, const D3DXVECTOR3 *cameraFacing );

     void Enable( D3DXVECTOR3 initPosition );
     void Disable() { enabled = false; }
     bool isRunning(void);

private:
    
    void InitParticals(IDirect3DDevice9* pd3dDevice);
    D3DXVECTOR3 InitParticalPosition(int i, D3DXVECTOR3 initPosition);
    IDirect3DStateBlock9* m_pStateBlock;

    IDirect3DVertexShader9 *m_pVertexShader;
    IDirect3DPixelShader9 *m_pPixelShader;
    
    IDirect3DVertexDeclaration9* pVD;
    
    LPDIRECT3DVERTEXBUFFER9 m_pVB;
    
    Partical *m_particals;
    bool enabled;
};

class CDecal
{
public:
    CDecal();
    ~CDecal();

    void Release( void );
    void Init(IDirect3DDevice9* pd3dDevice);

    void SetPosition( D3DXVECTOR3 pos );
    D3DXVECTOR3 GetPosition( void );

    void Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);
    void Update( float fElapsedTime );

    

private:

    IDirect3DVertexShader9 *m_pVertexShader;
    IDirect3DPixelShader9 *m_pPixelShader;
    LPDIRECT3DTEXTURE9 m_pExplosionTexture;
    IDirect3DVertexDeclaration9* pVD;
    IDirect3DStateBlock9* m_pStateBlock;

    LPDIRECT3DVERTEXBUFFER9 m_pVB;

    DecalVertex *m_vertices;

    D3DXMATRIX m_Orientation;
    D3DXMATRIX m_World;
    D3DXVECTOR3 m_position;
};

enum RenderType
{
    RENDER_BOMB,
    RENDER_PARTICAL_SYS,
    RENDER_DECAL,
    RENDER_PS_DECAL,
    RENDER_NOTHING
};

class CProjectile
{
    public:
    CProjectile();
    ~CProjectile();

    void Release( void );
    void Init(IDirect3DDevice9* pd3dDevice);

    void Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld, bool bRenderShadow);

    void setPosition( D3DXVECTOR3 pos );

    void setRenderType( RenderType new_render_type ){ m_renderState = new_render_type; };
    RenderType getRenderType( ){ return m_renderState; };

    void UpdatePartSys( float fElapsedTime, const D3DXVECTOR3 *cameraFacing );
    void EnablePartSys( D3DXVECTOR3 pos );
    void DisablePartSys( );
private:

    void EnableDisable( void );
    IDirect3DVertexShader9 *m_pVertexShader;
    IDirect3DPixelShader9 *m_pPixelShader;
   
    RenderType m_renderState;

    D3DXVECTOR3 m_pos;

    CBomb m_bomb;
    CParticalSystem m_particalsys;
    CDecal m_decal;

};


