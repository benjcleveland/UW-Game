#pragma once
#include "Node.h"
#include "WorldFile.h"
#include "Mesh.h"

////////////////////////////////////////////////////////////////////////////


class WorldNode : public Node
{

public:
    WorldNode();
	WorldNode(IDirect3DDevice9* pd3dDevice);
	~WorldNode();

    // initailization function
    void Init(IDirect3DDevice9* pd3dDevice);

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

						// Update traversal for physics, AI, etc.
	virtual void		Update(double fTime);

						// Render traversal for drawing objects
	virtual void		Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld,bool shadowPass);

private:

    // initialize the pixel and vertex shaders for the world
    void InitShaders( IDirect3DDevice9* pDevice);

    IDirect3DVertexShader9 *m_pVertexShader;
    IDirect3DPixelShader9 *m_pPixelShader;
    IDirect3DPixelShader9 *m_pPixelShaderWall;
	// A structure for our custom vertex type
	struct CUSTOMVERTEX
	{
		FLOAT x, y, z;      // The untransformed, 3D position for the vertex
       FLOAT nx, ny, nz;
		DWORD color;        // The vertex color
        DWORD color1; 
        FLOAT tu, tv;       // The texture coordinates
	};
    
    int num_floor_vertices;   // number of vertices
    int num_vertices; 

	LPDIRECT3DVERTEXBUFFER9 m_pVB; // Buffer to hold Vertices
    LPDIRECT3DVERTEXBUFFER9 m_pFloorVB; // Buffer to hold Vertices
    LPDIRECT3DTEXTURE9      m_pFloorTexture;
    LPDIRECT3DTEXTURE9      m_pWallTexture;

    IDirect3DVertexDeclaration9* pVD;

    IDirect3DStateBlock9* m_pStateBlock;

 //   Mesh                    m_skybox;
};

struct VertexData
{
    // This structure contains all that "extra" data attached to a vertex or pixel.
    // This data gets copied, transformed or interpolated as appropriate in the pipeline,
    // and is ultimately consumed by the pixel shading function.

    D3DCOLOR    diffuse;
    D3DCOLOR    specular;
    D3DXVECTOR2 texCoord;
};

struct WorldVertex
{
    // This structure contains a vertex in world (or model) coordinates,
    // prior to calculating the light observed on the vertex.

    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    //D3DXVECTOR3 tangent;
    VertexData  data;

 //   static IDirect3DVertexDeclaration9* pVD;
    static D3DVERTEXELEMENT9 const declaration[];
};

#if WorldNode_cpp
D3DVERTEXELEMENT9 const WorldVertex::declaration[] =
{
    { 0, offsetof(WorldVertex, position     ), D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION, 0 },
    { 0, offsetof(WorldVertex, normal       ), D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_NORMAL  , 0 },
    //{ 0, offsetof(WorldVertex, tangent      ), D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_TANGENT , 0 },
    { 0, offsetof(WorldVertex, data.diffuse ), D3DDECLTYPE_D3DCOLOR, 0, D3DDECLUSAGE_COLOR   , 0 },
    { 0, offsetof(WorldVertex, data.specular), D3DDECLTYPE_D3DCOLOR, 0, D3DDECLUSAGE_COLOR   , 1 },
    { 0, offsetof(WorldVertex, data.texCoord), D3DDECLTYPE_FLOAT2  , 0, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END(),
};
#endif

////////////////////////////////////////////////////////////////////////////
