
#pragma once

#include <stddef.h>


struct AnimatedVertex
{
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    D3DXVECTOR3 weights;
    D3DXVECTOR4 indices;
    static D3DVERTEXELEMENT9 const declaration[];
};

#if AnimatedMesh_cpp
D3DVERTEXELEMENT9 const AnimatedVertex::declaration[] =
{
    { 0, offsetof(AnimatedVertex, position     ), D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION, 0 },
    { 0, offsetof(AnimatedVertex, normal       ), D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_NORMAL  , 0 },
    { 0, offsetof(AnimatedVertex, weights      ), D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_BLENDWEIGHT , 0 },
    { 0, offsetof(AnimatedVertex, indices      ), D3DDECLTYPE_FLOAT4  , 0, D3DDECLUSAGE_BLENDINDICES , 0 },
    D3DDECL_END(),
};
#endif

class CBone
{
public:
    CBone();
    ~CBone();

    void initBone( CBone *parent, float offset );
    
    void update( D3DXMATRIX *rotation );

    D3DXMATRIX getSkinMatrix(D3DXMATRIX *world);
    D3DXMATRIX getMatrix();

private:
    D3DXMATRIX m_matrix;
    D3DXMATRIX m_invMatrix;
    D3DXMATRIX m_matrixRot;
    D3DXMATRIX m_matrixSkin;
};


class CAnimMesh
{
public:
    CAnimMesh();
    ~CAnimMesh();

    void Release( void );
    void Init(IDirect3DDevice9* pd3dDevice);

    void Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld, bool bRenderShadow);
    void Update( float fElapsedTime );

    void setLocation(D3DXVECTOR3 location);
    D3DXVECTOR3 getPosition(){ return m_position; }

private:

    
    LPD3DXMESH m_pmesh;
    
    IDirect3DStateBlock9* m_pStateBlock;

    IDirect3DVertexShader9 *m_pVertexShader;
    IDirect3DPixelShader9 *m_pPixelShader;
    IDirect3DVertexDeclaration9* pVD;

    D3DXMATRIX m_Orientation;
    D3DXMATRIX m_World;
    D3DXVECTOR3 m_position;

    CBone *bones;
    int num_bones;
};



