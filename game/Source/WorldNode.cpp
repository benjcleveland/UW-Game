#include "DXUT.h"
//#include "Game.h"
#include "DXUT/SDKmisc.h"
#define WorldNode_cpp 1
#include "WorldNode.h"
#undef WorldNode_cpp 
#include "collision.h"
#include "ShadowMap.h"

// include the shaders
#include "VertexShade.vfxobj"
#include "PixelShadeTexture.pfxobj"

using namespace std;
////////////////////////////////////////////////////////////////////////////



extern VecCollQuad vecCollQuad;
extern CShadowMap  g_ShadowMap;

WorldNode::WorldNode() : Node(NULL),	m_pVB(0), m_pVertexShader(NULL), m_pPixelShader(NULL)
{
    return;	// S_OK;
}

WorldNode::WorldNode(IDirect3DDevice9* pd3dDevice) : Node(pd3dDevice),	m_pVB(0), m_pVertexShader(NULL), m_pPixelShader(NULL)
{
    Init(pd3dDevice);

    return;	// S_OK;
}


WorldNode::~WorldNode()
{
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pFloorVB );
    SAFE_RELEASE( m_pFloorTexture );
    SAFE_RELEASE( m_pWallTexture );
    SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pPixelShader );
    SAFE_RELEASE( pVD );
    SAFE_RELEASE( m_pStateBlock );
}

// Initialize the WorldNode
void WorldNode::Init(IDirect3DDevice9* pd3dDevice)
{
    WorldFile worldfile;
    int height, width;
    int x;
    WCHAR str[MAX_PATH];
    WorldVertex *floor_vertices = new WorldVertex[18750];
    WorldVertex *vertices = new WorldVertex[18750];

    // set the initial number of vertices to 0
    num_vertices = 0;
    num_floor_vertices = 0;

    ( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"level.grd" ) );
    // Load the world file
    worldfile.Load(str);

    // get height and width of the world
    height = worldfile.GetHeight();
    width = worldfile.GetWidth();

    WorldVertex square[] =
    {
        // back
        {  D3DXVECTOR3(0.0f, 0.0f, 0.0f),D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DCOLOR(0xffffffff),D3DXCOLOR(.5f,.5f,.5f,1.f), D3DXVECTOR2(0.0f, 1.0f) },
        {  D3DXVECTOR3(0.0f, 0.0f, 1.0f),D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DCOLOR(0xffffffff), D3DXCOLOR(.5f,.5f,.5f,1.f),D3DXVECTOR2(0.0f, 0.0f) },
        {  D3DXVECTOR3(1.0f, 0.0f, 1.0f),D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DCOLOR(0xffffffff), D3DXCOLOR(.5f,.5f,.5f,1.f),D3DXVECTOR2(1.0f, 0.0f) },
        {  D3DXVECTOR3(1.0f, 0.0f, 1.0f),D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DCOLOR(0xffffffff), D3DXCOLOR(.5f,.5f,.5f,1.f),D3DXVECTOR2(1.0f, 0.0f) },
        {  D3DXVECTOR3(1.0f, 0.0f, 0.0f),D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DCOLOR(0xffffffff), D3DXCOLOR(.5f,.5f,.5f,1.f),D3DXVECTOR2(1.0f, 1.0f) },
        {  D3DXVECTOR3(0.0f, 0.0f, 0.0f),D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DCOLOR(0xffffffff), D3DXCOLOR(.5f,.5f,.5f,1.f),D3DXVECTOR2(0.0f, 1.0f) }
    };

    CUSTOMVERTEX front[] =
    {
        {  0.0f,1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 0.0f, 1.0f  },
        {  1.0f,1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 0.0f },
        {  1.0f,1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 1.0f},
        {  0.0f,1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 0.0f, 1.0f},
        {  0.0f,1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 0.0f, 0.0f },
        {  1.0f,1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 0.0f}
    };

    CUSTOMVERTEX top[] = 
    {
        {  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 0.0f, 1.0f  },
        {  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 1.0f},
        {  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 0.0f},
        {  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 0.0f},
        {  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 0.0f, 0.0f},
        {  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0xffffffff, D3DXCOLOR(.5f,.5f,.5f,1.f),0.0f, 1.0f},
    };

    CUSTOMVERTEX bottom [] =
    {
        {  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f),0.0f, 1.0f },
        {  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 0.0f, 0.0f},
        {  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 1.0f},
        {  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 0.0f, 0.0f},
        {  1.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 0.0f},
        {  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,0xffffffff, D3DXCOLOR(.5f,.5f,.5f,1.f),1.0f, 1.0f}
    };

    CUSTOMVERTEX left[] =
    {
        {  0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f),0.0f, 1.0f },
        {  0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 0.0f, 0.0f},
        {  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 1.0f},
        {  0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 0.0f, 0.0f},
        {  0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 0.0f},
        {  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 1.0f}
    };

    CUSTOMVERTEX right[] = 
    {
        {  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 1.0f},
        {  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 0.0f, 1.0f},
        {  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 1.0f, 0.0f},
        {  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,0xffffffff,D3DXCOLOR(.5f,.5f,.5f,1.f), 0.0f, 1.0f},
        {  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,0xffffffff, D3DXCOLOR(.5f,.5f,.5f,1.f),0.0f, 0.0f},
        {  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,0xffffffff, D3DXCOLOR(.5f,.5f,.5f,1.f),1.0f, 0.0f}
    };



    // loop through each point and convert to flat square or a cube
    for( int i = height ; i >= 0; --i )
    {
        for( int j = 0; j < width; ++j )
        {
            // add a flat square
            if( worldfile(i,j) == worldfile.EMPTY_CELL )
            {
                x = num_floor_vertices;
                memcpy(&floor_vertices[num_floor_vertices], square, sizeof(square));
                num_floor_vertices += 6;
                
                 for(x; x < num_floor_vertices; ++x)
                {
                    floor_vertices[x].position.x += j;
                    floor_vertices[x].position.z += i;
                }
            }
            else if( worldfile(i,j) == worldfile.OCCUPIED_CELL )
            {

                x = num_vertices;

                // add the top
                memcpy(&vertices[num_vertices], front, sizeof(front));
                num_vertices += 6;
                
                // check to see if we need to add more, z direction first
                if( worldfile(i+1,j) != worldfile.OCCUPIED_CELL  )
                {
                    memcpy(&vertices[num_vertices], top, sizeof(top));
                    num_vertices += 6;
                    CollQuad quad;
   
                    D3DVECTOR v4 = {0.0f + j, 0.0f, 1.0f + i};
                    D3DVECTOR v3 = {0.0f + j, 1.0f, 1.0f + i};
                    D3DVECTOR v2 = {1.0f + j, 1.0f, 1.0f + i};
                    D3DVECTOR v1 = {1.0f + j, 0.0f, 1.0f + i};
                  

                    quad.Set(v1,v2,v3,v4);
                    // add this to the array
                    vecCollQuad.push_back(quad);
                }
                // bottom
                if( worldfile(i-1,j) != worldfile.OCCUPIED_CELL )
                {
                    memcpy(&vertices[num_vertices], bottom, sizeof(bottom));
                    num_vertices += 6;
                    
                    CollQuad quad;
                                        
                    D3DVECTOR v1 = {0.0f + j, 0.0f, 0.0f + i};
                    D3DVECTOR v2 = {0.0f + j, 1.0f, 0.0f + i};
                    D3DVECTOR v3 = {1.0f + j, 1.0f, 0.0f + i};
                    D3DVECTOR v4 = {1.0f + j, 0.0f, 0.0f + i};
                    
                    quad.Set(v1,v2,v3,v4);
                    // add this to the array
                    vecCollQuad.push_back(quad);
                }
                //left
                if( worldfile(i, j-1) != worldfile.OCCUPIED_CELL )
                {
                    memcpy(&vertices[num_vertices], left, sizeof(left));
                    num_vertices += 6;

                    CollQuad quad;

                    D3DVECTOR v1 = {0.0f + j, 0.0f, 1.0f + i};
                    D3DVECTOR v2 = {0.0f + j, 1.0f, 1.0f + i};
                    D3DVECTOR v3 = {0.0f + j, 1.0f, 0.0f + i};
                    D3DVECTOR v4 = {0.0f + j, 0.0f, 0.0f + i};
                    
                    
                    quad.Set(v1,v2,v3,v4);
                    // add this to the array
                    vecCollQuad.push_back(quad);
                }
                // right
                if( worldfile(i, j+1) != worldfile.OCCUPIED_CELL )
                {
                    memcpy(&vertices[num_vertices], right, sizeof(right));
                    num_vertices += 6;

                    CollQuad quad;

                    D3DVECTOR v1 = {1.0f + j, 0.0f, 0.0f + i};
                    D3DVECTOR v2 = {1.0f + j, 1.0f, 0.0f + i};
                    D3DVECTOR v3 = {1.0f + j, 1.0f, 1.0f + i};
                    D3DVECTOR v4 = {1.0f + j, 0.0f, 1.0f + i};
               
                    D3DVECTOR n = {1.0f,0,0};
                    quad.Set(v1,v2,v3,v4);
                    // add this to the array
                    vecCollQuad.push_back(quad);
                }

                for(x; x < num_vertices; ++x)
                {
                    vertices[x].position.x += j;
                    vertices[x].position.z += i;
                }
            }
            else
            {
                // invalid cell fail?
                break;
            }
        }
    }
  

    // Create the vertex buffer.
    if( FAILED( pd3dDevice->CreateVertexBuffer( num_vertices*sizeof(WorldVertex),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        return;	// E_FAIL;
    }

    // Fill the vertex buffer.
    VOID* pVertices;
    if( FAILED( m_pVB->Lock( 0, sizeof(WorldVertex)*num_vertices, (void**)&pVertices, 0 ) ) )
        return;	// E_FAIL;
    memcpy( pVertices, vertices, sizeof(WorldVertex)*num_vertices );
    m_pVB->Unlock();


     // Create the vertex buffer for the floor
    if( FAILED( pd3dDevice->CreateVertexBuffer( num_floor_vertices*sizeof(WorldVertex),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &m_pFloorVB, NULL ) ) )
    {
        return;	// E_FAIL;
    }

    // Fill the vertex buffer.
    if( FAILED( m_pFloorVB->Lock( 0, sizeof(WorldVertex)*num_floor_vertices, (void**)&pVertices, 0 ) ) )
        return;	// E_FAIL;
    memcpy( pVertices, floor_vertices, sizeof(WorldVertex)*num_floor_vertices );
    m_pFloorVB->Unlock();

    // Use D3DX to create a texture from a file based image
    DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"floortiles01.jpg" );
    if( FAILED( D3DXCreateTextureFromFile( pd3dDevice, str, &m_pFloorTexture ) ) )
    {
            MessageBox( NULL, L"Could not find banana.bmp", L"Textures.exe", MB_OK );
           // return E_FAIL;  
    }

     // Use D3DX to create a texture from a file based image
    DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"wall.jpg" );
    if( FAILED( D3DXCreateTextureFromFile( pd3dDevice, str, &m_pWallTexture ) ) )
    {
       
            MessageBox( NULL, L"Could not find banana.bmp", L"Textures.exe", MB_OK );
           // return E_FAIL;
        
    }

    // setup the skybox
    //m_skybox.Setup(pd3dDevice, L"skybox2.x");
    //m_skybox.InitSkybox();
 
    // initialize the shaders for the world map
    InitShaders(pd3dDevice);

    // capture the state
    pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pStateBlock );

    // cleanup
    SAFE_DELETE_ARRAY(vertices);
    SAFE_DELETE_ARRAY(floor_vertices);
}

// initialize the shaders
void WorldNode::InitShaders(IDirect3DDevice9 *pDevice)
{
    // load the vertex shader
    if( FAILED(pDevice->CreateVertexShader((DWORD const*)VFX_VertexShade, &m_pVertexShader )))
    {   
        MessageBox(NULL, L"Failed to load vertex shader!!!", L"Game.exe", MB_OK );
    }

    // create the vertex decleation
    pDevice->CreateVertexDeclaration( WorldVertex::declaration, &pVD);

    // load the pixel shader
    if( FAILED(pDevice->CreatePixelShader((DWORD const*)PFX_PixelShadeTexture, &m_pPixelShader )))
    {
        MessageBox(NULL, L"Failed to load pixel shader!!!", L"Game.exe", MB_OK );
    }
}

void WorldNode::Update(double /* fTime */)
{
}


void WorldNode::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld, bool shadowPass)
{
    // clean this up
    D3DXMATRIX matView;
    D3DXMATRIX matProj;
    D3DXMATRIX res;
    D3DXMATRIX light;
    pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
    pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

   
    D3DXMatrixTranspose(&res, &(matWorld * matView  *matProj));
    D3DXMatrixTranspose(&light, &matWorld);
    
    // save the state 
    m_pStateBlock->Capture();

      D3DXMATRIXA16 mViewToLightProj;
    mViewToLightProj = matWorld*matView*matProj;
    D3DXMatrixInverse( &mViewToLightProj, NULL, &mViewToLightProj );
   // D3DXMatrixIdentity( &mViewToLightProj );
    D3DXMatrixMultiply( &mViewToLightProj, &mViewToLightProj, &g_ShadowMap.m_mShadowView );
    D3DXMatrixMultiply( &mViewToLightProj, &mViewToLightProj, &g_ShadowMap.m_mShadowProj );
    //mViewToLightProj = mViewToLightProj *g_ShadowMap.m_mShadowView * g_ShadowMap.m_mShadowProj  ;
    // D3DXMatrixTranspose( &mViewToLightProj, &mViewToLightProj);
     pd3dDevice->SetVertexShaderConstantF(18, (float const*)mViewToLightProj, sizeof(mViewToLightProj));


    // Set the world space transform
    pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    // Set the texture
    if( !shadowPass )
    {
        pd3dDevice->SetTexture(0, m_pFloorTexture);
        g_ShadowMap.SetShadowTexture(pd3dDevice, 1);
    }

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

    pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

    pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
    
    // Turn on culling
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    // Turn off D3D lighting, since we are providing our own vertex colors
   // pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );

    // Draw the triangles in the vertex buffer. This is broken into a few
    // steps. We are passing the Vertices down a "stream", so first we need
    // to specify the source of that stream, which is our vertex buffer. Then
    // we need to let D3D know what vertex shader to use. Full, custom vertex
    // shaders are an advanced topic, but in most cases the vertex shader is
    // just the FVF, so that D3D knows what type of Vertices we are dealing
    // with. Finally, we call DrawPrimitive() which does the actual rendering
    // of our geometry (in this case, just one triangle).
    
    if( !shadowPass )
    {
        pd3dDevice->SetVertexShader(m_pVertexShader);
        pd3dDevice->SetPixelShader(m_pPixelShader);
    }

    // set vertex shader constants
    pd3dDevice->SetVertexShaderConstantF(0, (float const*)res, 4);
    pd3dDevice->SetVertexShaderConstantF(4, (float const*)light, 3);

    float power = 32.f;
    pd3dDevice->SetVertexShaderConstantF(17, (float const*)&power,1);

    pd3dDevice->SetVertexDeclaration(pVD);
    pd3dDevice->SetStreamSource( 0, m_pFloorVB, 0, sizeof(WorldVertex) );
    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, num_floor_vertices/3 );
    
    // Set the texture
    if(!shadowPass )
        pd3dDevice->SetTexture(0, m_pWallTexture);

    // walls
    pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(WorldVertex) );
    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, num_vertices/3 );

    // Call base class
   // Node::Render(pd3dDevice, matWorld);
    
    m_pStateBlock->Apply();
}
