#include "DXUT.h"
#include "Mesh.h"
#include "DXUT\SDKmisc.h"

using namespace std;

// include the shaders
#include "MeshVertexShade.vfxobj"
#include "MeshPixelShadeTexture.pfxobj"
#include "AnimatedPixelShade.pfxobj"
#include "AnimatedMeshVertexShade.vfxobj"

Mesh::Mesh() : pMesh( NULL ), pMeshMaterials( NULL ), pMeshTextures(NULL), numMaterials(0), m_state(IDLE), m_ftimewaiting(.5f)
{
    D3DXMatrixIdentity( &m_Orientation );
    D3DXMatrixIdentity( &m_World );
    
}

Mesh::~Mesh()
{
    // cleanup!!
    Release();
}

HRESULT Mesh::Setup(IDirect3DDevice9* pd3dDevice, LPCWSTR filename)
{
    HRESULT hr;
    WCHAR str[MAX_PATH];
    WCHAR wszTexName[ MAX_PATH ];
    LPD3DXBUFFER pMaterialBuffer;
    D3DXMATERIAL *pMaterials;
    bool material = false;

    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, filename ) );

    // load the mesh from the specified file
    hr = D3DXLoadMeshFromX(str, D3DXMESH_SYSTEMMEM, pd3dDevice, NULL, &pMaterialBuffer, NULL, &numMaterials, &pMesh);

    pMaterials = (D3DXMATERIAL*) pMaterialBuffer->GetBufferPointer();

    pMeshMaterials = new D3DMATERIAL9[numMaterials];
    pMeshTextures = new LPDIRECT3DTEXTURE9[numMaterials];

    // load Materials
    for( DWORD i = 0; i < numMaterials; ++i )
    {
        pMeshMaterials[i] = pMaterials[i].MatD3D;
        pMeshMaterials[i].Ambient = pMeshMaterials[i].Diffuse;
        pMeshTextures[i] = NULL;
        
        // see if this material has a texture file associated with it
        if(pMaterials[i].pTextureFilename != NULL )
        {
            material = true;
            // convert the filename
            if( MultiByteToWideChar( CP_ACP, 0, pMaterials[ i ].pTextureFilename, -1, wszTexName, MAX_PATH ) == 0 )
            {
                pMeshTextures[ i ] = NULL;
            }
            else
            {
                // find the location of the file
                if( SUCCEEDED (DXUTFindDXSDKMediaFileCch( str, MAX_PATH, wszTexName ) ) )
                {
                    // load the texture
                    if( FAILED( D3DXCreateTextureFromFile(pd3dDevice, str, &pMeshTextures[i] )))
                    {
                        // there was an error inform the user, probably should exit
                        MessageBox(NULL, L"Could not find texture", L"Error", MB_OK);
                    }   
                }
            }
        }

    }
    // release material buffer
    SAFE_RELEASE(pMaterialBuffer);


    currPosition.x = 10;//2.3414159f;
    currPosition.y = 0;
    currPosition.z = 0;//7.2354894f;

    goToPosition.x = 4.6393018f;//24.0f * rand() / RAND_MAX;
    goToPosition.z = 19.409773f;//24.0f * rand() / RAND_MAX;
    goToPosition.y = 0;
    diffPosition = goToPosition - currPosition;


    m_facing = 0;
    
    // set initial facing vector
    setFacingVec(D3DXVECTOR3(0,0,1.f));

    // set scale
    setScale( .5f );

    // create shaders
    InitShaders(pd3dDevice, material);

    // capture the state
    pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pStateBlock );

    return hr;
}

// set the scale of the mesh
void Mesh::setScale( float scale )
{
    D3DXMATRIX mx;
    // scale
    D3DXMatrixScaling(&m_Orientation, scale, scale, scale);
    D3DXMatrixRotationY( &mx, D3DX_PI );
    //D3DXMatrixTranslation(&mx, currPosition.x, currPosition.y, currPosition.z);
    D3DXMatrixMultiply( &m_Orientation, &m_Orientation, &mx );

    D3DXMatrixRotationY(&m_World, m_facing );
    D3DXMatrixTranslation(&mx, currPosition.x, currPosition.y, currPosition.z);
    D3DXMatrixMultiply(&m_World, &m_World, &mx );
    D3DXMatrixMultiply(&m_World,&m_Orientation, &m_World);
}

// initialize the shaders
void Mesh::InitShaders(IDirect3DDevice9 *pDevice, bool material)
{
    // load the vertex shader
    if( FAILED(pDevice->CreateVertexShader((DWORD const*)VFX_MeshVertexShade, &m_pVertexShader )))
    {   
        MessageBox(NULL, L"Failed to load vertex shader!!!", L"Game.exe", MB_OK );
    }

    // load the pixel shader
    if( material == true )
    {
        if(FAILED(pDevice->CreatePixelShader((DWORD const*)PFX_MeshPixelShadeTexture, &m_pPixelShader )))
        {
            MessageBox(NULL, L"Failed to load pixel shader!!!", L"Game.exe", MB_OK );
        }
    }
    else
    {
        if( FAILED( pDevice->CreatePixelShader( (DWORD const *) PFX_AnimatedPixelShade, &m_pPixelShader )))
        {
            MessageBox(NULL, L"Failed to load pixel shader!!!", L"Game.exe", MB_OK );
        }
    }
}

void Mesh::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld, bool shadowPass)
{

     // clean this up
    D3DXMATRIX matView;
    D3DXMATRIX matProj;
    D3DXMATRIX res;
    D3DXMATRIX light;
    pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
    pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

    D3DXMatrixTranspose(&res, &(m_World * matView  *matProj));
    D3DXMatrixTranspose(&light, &m_World);

     // save the state 
   m_pStateBlock->Capture();

    if( !shadowPass )
    {
        pd3dDevice->SetVertexShader(m_pVertexShader);
        pd3dDevice->SetPixelShader(m_pPixelShader);
    }

    // set vertex shader constants
    pd3dDevice->SetVertexShaderConstantF(0, (float const*)res, 4);
    pd3dDevice->SetVertexShaderConstantF(4, (float const*)light, 3);

    // Set the world space transform
    pd3dDevice->SetTransform(D3DTS_WORLD, &m_World);
    pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );

    // loop through all the materials
    for( DWORD i = 0; i < numMaterials; ++i )
    {
        D3DXVECTOR4 PSConstants[2];
        // set the material and texture
        if( !shadowPass )
        {
            pd3dDevice->SetMaterial(&pMeshMaterials[i]);
            pd3dDevice->SetTexture(0, pMeshTextures[i]);
        }

        *(D3DCOLORVALUE*)&PSConstants[0] = pMeshMaterials->Diffuse;
        *(D3DCOLORVALUE*)&PSConstants[1] = pMeshMaterials->Specular;
        pd3dDevice->SetPixelShaderConstantF(0, (float *)PSConstants, 2);
 //       pd3dDevice->SetVertexShaderConstantF(17, (float*)&power,1);

        // draw the mesh subset
        pMesh->DrawSubset( i );
    }

    m_pStateBlock->Apply();
}

void Mesh::Release()
{
    // clean everything up if needed
    SAFE_DELETE_ARRAY(pMeshMaterials);

    if( pMeshTextures != NULL )
    {
        for( DWORD i = 0; i < numMaterials; ++i )
        {
            SAFE_RELEASE(pMeshTextures[i]);
        }
        SAFE_DELETE_ARRAY(pMeshTextures);
    }

    SAFE_RELEASE(pMesh);
    SAFE_RELEASE(m_pStateBlock);
    SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pPixelShader );
}



D3DXVECTOR3 Mesh::getPosition()
{
    return currPosition;
}


D3DXVECTOR3 Mesh::getFacing()
{
    /*
    D3DXVECTOR3 vfacing;
    D3DXVec3Subtract(&vfacing, &goToPosition, &currPosition);
    D3DXVec3Normalize( &vfacing, &vfacing);

    D3DXMATRIX m;
    D3DXVECTOR3 pV( 1.0f, 0.0f, 0.0f );
    D3DXVec3TransformCoord( &pV, &pV, D3DXMatrixRotationY( &m, m_facing  ) );
    */
    return m_facing_vec;
}

D3DXVECTOR3 Mesh::getSeekingPosition()
{
    return goToPosition;
}

void Mesh::setSeekPosition(D3DXVECTOR3 seekPosition)
{
    goToPosition = seekPosition;
    diffPosition = goToPosition - currPosition;
    D3DXVec3Normalize(&diffPosition,&diffPosition);
}



void Mesh::SetPosition( D3DXVECTOR3 pos)
{
    D3DXMATRIX mx;
    currPosition = pos;

    D3DXMatrixRotationY(&m_World, m_facing );
    D3DXMatrixTranslation(&mx, currPosition.x, currPosition.y, currPosition.z);
    D3DXMatrixMultiply(&m_World, &m_World, &mx );
    D3DXMatrixMultiply(&m_World,&m_Orientation, &m_World);
    
}

D3DXVECTOR3 Mesh::getDiffPosDest()
{
    return diffPosition;
}


/*     Skybox stuff */
void Skybox::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{

    // Set the world space transform
    pd3dDevice->SetTransform(D3DTS_WORLD, &m_World);
    pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );

    pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    // Disable the z buffer
    pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

    // Disable lighting
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Clamp texture co-ordinates to get rid of horrible seam
    pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
    pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );
    // loop through all the materials
    for( DWORD i = 0; i < numMaterials; ++i )
    {
        // set the material and texture
        pd3dDevice->SetMaterial(&pMeshMaterials[i]);
        pd3dDevice->SetTexture(0, pMeshTextures[i]);

        // draw the mesh subset
        pMesh->DrawSubset( i );
    }

    // Disable the z buffer
    pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

    // Disable lighting
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}


void Skybox::InitSkybox()
{
        D3DXMATRIX mx;
    // scale
    //D3DXMatrixScaling(&m_Orientation, .416666666f, .416666666f, .40f);
    D3DXMatrixRotationY( &mx, D3DX_PI );
    D3DXMatrixScaling(&m_Orientation, 1, 1, 1);
    D3DXMatrixTranslation(&mx, 12.5f, 3, 12.f);

    D3DXMatrixMultiply( &m_Orientation, &m_Orientation, &mx );
    D3DXMatrixMultiply(&m_World,&m_Orientation, &m_World);
}

void Skybox::SetPosition(D3DXVECTOR3 pos)
{
    D3DXMATRIX mx;
    D3DXMatrixIdentity( &m_World );
   D3DXMatrixScaling(&m_World, 1.8f, 1.8f, 1.8f);
    D3DXMatrixTranslation(&mx, pos.x, pos.y+3, pos.z);

    //D3DXMatrixMultiply( &m_Orientation, &m_Orientation, &mx );
    D3DXMatrixMultiply(&m_World,&m_World,&mx);   
}

void CBomb::SetPosition( D3DXVECTOR3 pos)
{
    D3DXMATRIX mx;
    D3DXMatrixIdentity( &m_World );
    //D3DXMatrixRotationY(&m_World, m_facing );
    D3DXMatrixScaling(&m_World, .05f, .05f, .05f);
    D3DXMatrixTranslation(&mx, pos.x, pos.y, pos.z);
    D3DXMatrixMultiply(&m_World, &m_World, &mx );
   // D3DXMatrixMultiply(&m_World,&m_Orientation, &m_World); 

    currPosition = pos;
}