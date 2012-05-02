
#include "DXUT.h"
#include "DXUT\SDKmisc.h"
#include <time.h>
#include "Tiny.h"

#define ParticalSystem_cpp 1
#include "ParticalSystem.h"

// include the shaders
//#include "AnimatedMeshVertexShade.vfxobj"
#include "VertexShade.vfxobj"
#include "PixelShadeDecal.pfxobj"

using namespace std;

#define NUM_PARTICALS 250

CParticalSystem::CParticalSystem() : pVD(NULL), m_pPixelShader(NULL), m_pVertexShader(NULL), m_particals(NULL)
{
    enabled = false;
}

CParticalSystem::~CParticalSystem()
{
    Release();
    SAFE_DELETE_ARRAY(m_particals);
}

void CParticalSystem::Release(void)
{
    for( int i = 0; i < NUM_PARTICALS; ++i )
    {
        m_particals[i].m_billboard.Release();
    }
    SAFE_RELEASE(m_pStateBlock );
    SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pPixelShader );
    SAFE_RELEASE( pVD );
}

void CParticalSystem::Init(IDirect3DDevice9* pd3dDevice)
{
   
    // create the billboards
    m_particals = new Partical[NUM_PARTICALS];

    srand((unsigned int)time(NULL));

    InitParticals(pd3dDevice);

    // create the state block
     pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pStateBlock ); 
    
     // create the vertex decleation
    pd3dDevice->CreateVertexDeclaration( BillboardVertex::declaration, &pVD);
}

void CParticalSystem::InitParticals(IDirect3DDevice9* pd3dDevice)
{
    D3DXVECTOR3 pos;
     // init all the billboards
    for( int i = 0; i < NUM_PARTICALS; ++i)
    {
        pos = InitParticalPosition(i,D3DXVECTOR3(0,0,0));
        m_particals[i].m_billboard.Init(pd3dDevice);
        m_particals[i].m_billboard.SetPosition(pos);
    }
}


D3DXVECTOR3 CParticalSystem::InitParticalPosition(int i, D3DXVECTOR3 initPosition)
{
    float y_pos = (initPosition.y < 0 ? 0 : (initPosition.y));
    D3DXVECTOR3 pos((float)(rand() % 100) / 1000 + initPosition.x, (float)(rand() % 100) / 1000 + y_pos, (float)(rand() % 100) / 1000 + initPosition.z);
    // set the initial velocity
    float veloy = (float)(rand() % 100) / 2500;
    float velox = (float)(rand() % 100) / 5000* (((float)((rand()%10 / 5))-.5f)*2);
    float veloz = (float)(rand() % 100) / 5000 * (((float)((rand()%10 / 5))-.5f)*2);

    m_particals[i].velocity = m_particals[i].initvelocity=D3DXVECTOR3(velox, veloy, veloz);
    // set the render state to true
    m_particals[i].RenderState = true;
    return pos;
}

void CParticalSystem::Update( float fElapsedTime, const D3DXVECTOR3 *cameraFacing)
{
    
    D3DXVECTOR3 force(0,-.04f,0);
    D3DXVECTOR3 velocity(0,0,0);
    D3DXVECTOR3 pos;

    if( enabled )
    {
        for(int i = 0; i < NUM_PARTICALS; ++i)
        {
            if( m_particals[i].RenderState == TRUE )
            {
                // update the position of the billboard
                m_particals[i].velocity += force * fElapsedTime;
                pos = m_particals[i].m_billboard.GetPosition();
                pos = pos + m_particals[i].velocity;
                // handle what happens when the y pos = y
                // had a colision with the ground
                if( pos.y <= 0 )
                {
                    // calculate new velocity
                    m_particals[i].velocity -= 2*D3DXVec3Dot(&m_particals[i].velocity, &D3DXVECTOR3(0,1,0))*D3DXVECTOR3(0,1,0)/1.25f;
                    
                    if( m_particals[i].velocity.y < .001f)
                    {
                        // set the initial velocity and position
                        // pos = InitParticalPosition(i);
                        m_particals[i].RenderState = false;
                    }
                }
                m_particals[i].m_billboard.SetPosition(pos);

                // update the orientation of the billboards
                m_particals[i].m_billboard.Update(fElapsedTime, cameraFacing);
            }
        }
    }
}


// render the animated mesh
void CParticalSystem::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
    if( enabled )
    {
        m_pStateBlock->Capture();

        pd3dDevice->SetTexture(0, NULL);

        // disable culling 
        pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);

        // Turn off D3D lighting, since we are providing our own vertex colors
        pd3dDevice->SetRenderState( D3DRS_LIGHTING, false );
        // render each billboard
        for(int i = 0; i < NUM_PARTICALS; ++i )
        {
            if( m_particals[i].RenderState == TRUE )
            {
                m_particals[i].m_billboard.Render(pd3dDevice,matWorld);
            }
        }

        m_pStateBlock->Apply();
    }
}


void CParticalSystem::Enable( D3DXVECTOR3 initPosition )
{
    // reset everything
    D3DXVECTOR3 pos;
    // init all the billboards
    for( int i = 0; i < NUM_PARTICALS; ++i)
    {
        pos = InitParticalPosition(i, initPosition);
        m_particals[i].m_billboard.SetPosition(pos);
    }
    enabled = true;
}


bool CParticalSystem::isRunning(void)
{
    return enabled;
}

// billboard class
CBillboard::CBillboard() : pVD(NULL), m_pPixelShader(NULL), m_pVertexShader(NULL)
{

    BillboardVertex verts[4]  = 
    {
        {D3DXVECTOR3(-.005f,-.005f,0),D3DXCOLOR(1.f,0,0,1.f)},// -.125f, .125f, 0, 0, 0,1,0,1   },
        {D3DXVECTOR3(-.005f,.005f,0),D3DXCOLOR(1.f,0,0,1.f)},// .125f, .125f, 1,0 , 1, 1,1, 1},
        {D3DXVECTOR3(.005f,-.005f,0),D3DXCOLOR(1.f,0,0,1.f)},// -.125f, -.125f, 0, 1, 0, 0,0,0},
        {D3DXVECTOR3(.005f,.005f,0),D3DXCOLOR(1.f,0,0,1.f)}// .125f, -.125f,1,1, 1,0,1,0} 
    };

    m_vertices = new BillboardVertex[4];
    memcpy(m_vertices, &verts, sizeof(verts));

    D3DXMatrixIdentity(&m_Orientation);

    m_position = D3DXVECTOR3(0,0,0);
}

CBillboard::~CBillboard()
{
    SAFE_DELETE_ARRAY(m_vertices);
    //Release();
}

void CBillboard::Release(void)
{
    //SAFE_RELEASE( m_pVertexShader );
    //SAFE_RELEASE( m_pPixelShader );
    SAFE_RELEASE( pVD );
}

void CBillboard::Init(IDirect3DDevice9* pd3dDevice)
{
     // create the vertex decleation
    pd3dDevice->CreateVertexDeclaration( BillboardVertex::declaration, &pVD);
}

void CBillboard::SetPosition( D3DXVECTOR3 position )
{
    m_position = position; // save the value of the position for later use
}

D3DXVECTOR3 CBillboard::GetPosition( void )
{
    return m_position;
}

void CBillboard::Update( float fElapsedTime, const D3DXVECTOR3 *cameraFacing)
{
    D3DXVECTOR3 diff = m_position - *cameraFacing;
    float xRot = atan2(-diff.y, sqrt(diff.x*diff.x + diff.z*diff.z));
    float yRot = atan2(diff.x, diff.z);

    D3DXMATRIX mx;
    D3DXMatrixIdentity(&m_World);
    D3DXMatrixRotationX(&m_World, xRot );
    D3DXMatrixRotationY(&mx, yRot );
    D3DXMatrixMultiply(&m_World, &m_World, &mx );
    D3DXMatrixTranslation(&mx, m_position.x, m_position.y, m_position.z);
    D3DXMatrixMultiply(&m_World, &m_World, &mx );
    //D3DXMatrixMultiply(&m_World, &m_World, &m_Orientation);
}


// render the Billboard
void CBillboard::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
    // capture the current state
   // m_pStateBlock->Capture();

    pd3dDevice->SetTransform( D3DTS_WORLD, &m_World );

    // set the vertex declaration
    pd3dDevice->SetVertexDeclaration(pVD);

    pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, m_vertices, sizeof(m_vertices[0]));
    // apply previous state
    //m_pStateBlock->Apply();

}



// projectile
CProjectile::CProjectile()
{
    m_renderState = RENDER_NOTHING;
}

CProjectile::~CProjectile()
{
    Release();
}

void CProjectile::Release(void)
{
    m_particalsys.Release();
    m_bomb.Release();
    m_decal.Release();
}

void CProjectile::Init(IDirect3DDevice9* pd3dDevice )
{

    // initialize the partical system
    m_particalsys.Init(pd3dDevice);

    // initialize the bomb mesh
    m_bomb.Setup( pd3dDevice, L"ammo.x" );

    // init the decal
    m_decal.Init(pd3dDevice);

}


// update the partical system
void CProjectile::UpdatePartSys( float fElapsedTime, const D3DXVECTOR3 *cameraFacing)
{
    m_particalsys.Update( fElapsedTime, cameraFacing );
}

// enable the partical system
void CProjectile::EnablePartSys( D3DXVECTOR3 pos )
{
    m_particalsys.Enable( pos );
}

void CProjectile::DisablePartSys()
{
    m_particalsys.Disable();
}
// render the animated mesh
void CProjectile::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld, bool bRenderShadow)
{

    if( m_renderState == RENDER_BOMB )
    {
        m_bomb.Render(pd3dDevice, matWorld, bRenderShadow);
    }
    else if( m_renderState == RENDER_PS_DECAL && !bRenderShadow )
    {
        m_particalsys.Render(pd3dDevice, matWorld);
        m_decal.Render(pd3dDevice, matWorld );
    }
    else if( m_renderState == RENDER_PARTICAL_SYS && !bRenderShadow)
    {
        m_particalsys.Render(pd3dDevice, matWorld);
    }
    else if( ( m_renderState == RENDER_DECAL) && !bRenderShadow )
    {
        m_decal.Render(pd3dDevice, matWorld );
    }
}

void CProjectile::setPosition( D3DXVECTOR3 pos )
{
    m_bomb.SetPosition(pos);
    // set the position of the decal
    m_decal.SetPosition(pos);
}




// decal
CDecal::CDecal() : pVD(NULL), m_pPixelShader(NULL), m_pVertexShader(NULL), m_pExplosionTexture( NULL)
{

    DecalVertex verts[4]  = 
    {
        {D3DXVECTOR3(-.25f, 0,-.25f), D3DXVECTOR2(0,0)}, // -.125f, .125f, 0, 0, 0,1,0,1   },
        {D3DXVECTOR3(-.25f,0,.25f), D3DXVECTOR2(1,0)},// .125f, .125f, 1,0 , 1, 1,1, 1},
        {D3DXVECTOR3(.25f,0,-.25f), D3DXVECTOR2(0,1)},// -.125f, -.125f, 0, 1, 0, 0,0,0},
        {D3DXVECTOR3(.25f,0,.25f),D3DXVECTOR2(1,1)}// .125f, -.125f,1,1, 1,0,1,0} 
    };

    m_vertices = new DecalVertex[4];
    memcpy(m_vertices, &verts, sizeof(verts));

    D3DXMatrixIdentity(&m_Orientation);

    m_position = D3DXVECTOR3(1,0,1);
     D3DXMATRIX mx;
    D3DXMatrixIdentity(&m_World);
    D3DXMatrixTranslation(&mx, m_position.x, m_position.y, m_position.z);
    D3DXMatrixMultiply(&m_World, &m_World, &mx );
}

CDecal::~CDecal()
{
    SAFE_DELETE_ARRAY(m_vertices);
    Release();
}

void CDecal::Release(void)
{
    SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pPixelShader );
    SAFE_RELEASE( m_pExplosionTexture );
    SAFE_RELEASE( m_pStateBlock );
    SAFE_RELEASE( pVD );
}

void CDecal::Init(IDirect3DDevice9* pd3dDevice)
{
    WCHAR str[MAX_PATH];

     // create the vertex decleation
    pd3dDevice->CreateVertexDeclaration( DecalVertex::declaration, &pVD);

       // Use D3DX to create a texture from a file based image
    DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"explosion_res.jpg" );
    if( FAILED( D3DXCreateTextureFromFile( pd3dDevice, str, &m_pExplosionTexture ) ) )
    {
            MessageBox( NULL, L"Could not find explosion_res.bmp", L"Textures.exe", MB_OK );
           // return E_FAIL;  
    }

       // load the pixel shader
    if( FAILED(pd3dDevice->CreatePixelShader((DWORD const*)PFX_PixelShadeDecal, &m_pPixelShader )))
    {
        MessageBox(NULL, L"Failed to load pixel shader!!!", L"Game.exe", MB_OK );
    }

       // load the pixel shader
    /*if( FAILED(pd3dDevice->CreateVertexShader((DWORD const*)VFX_VertexShade, &m_pVertexShader )))
    {
        MessageBox(NULL, L"Failed to load vertex shader!!!", L"Game.exe", MB_OK );
    }*/
    // create the state block
     pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pStateBlock ); 
}

void CDecal::SetPosition( D3DXVECTOR3 position )
{
    m_position = position; // save the value of the position for later use
    D3DXMATRIX mx;
    D3DXMatrixIdentity(&m_World);
    D3DXMatrixTranslation(&mx, m_position.x, 0, m_position.z);
    D3DXMatrixMultiply(&m_World, &m_World, &mx );
    
}

D3DXVECTOR3 CDecal::GetPosition( void )
{
    return m_position;
}

void CDecal::Update( float fElapsedTime)
{

    D3DXMATRIX mx;
    D3DXMatrixIdentity(&m_World);
    D3DXMatrixTranslation(&mx, m_position.x, m_position.y, m_position.z);
    D3DXMatrixMultiply(&m_World, &m_World, &mx );
    //D3DXMatrixMultiply(&m_World, &m_World, &m_Orientation);
}


// render the Decal
void CDecal::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
    // capture the current state
    m_pStateBlock->Capture();

        // enable alpha-blending
    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

    // set the depth bias
    float bias = -.00001f;
    pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)(&bias));

    pd3dDevice->SetTransform( D3DTS_WORLD, &m_World );

    pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );
    pd3dDevice->SetTexture(0, m_pExplosionTexture );

    // set the vertex declaration
    pd3dDevice->SetVertexDeclaration(pVD);

   // pd3dDevice->SetVertexShader( m_pVertexShader );
    pd3dDevice->SetPixelShader( m_pPixelShader );

    pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, m_vertices, sizeof(m_vertices[0]));
    
    // apply previous state
    m_pStateBlock->Apply();

}
