#include "DXUT.h"
#include "DXUT\SDKmisc.h"
#include "ShadowMap.h"

// include the shaders
#include "ShadowVertexShade.vfxobj"
#include "ShadowPixelShade.pfxobj"

#define ShadowMap_SIZE 2048

CShadowMap::CShadowMap() : m_pShadowMap(NULL), m_pDSShadow(NULL), m_pOldRT(NULL), m_pOldDS(NULL), m_pStateBlock(NULL),
    m_pVertexShader(NULL), m_pPixelShader(NULL)
{
}

CShadowMap::~CShadowMap()
{
    Release();
}

void CShadowMap::Release()
{
    SAFE_RELEASE( m_pShadowMap );
    SAFE_RELEASE( m_pDSShadow );
    SAFE_RELEASE(m_pStateBlock );
    SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pPixelShader );
}

HRESULT CShadowMap::Init(IDirect3DDevice9 *pd3dDevice)
{
    HRESULT hr;

    // create the shadow map texture
    V_RETURN( pd3dDevice->CreateTexture( ShadowMap_SIZE, ShadowMap_SIZE,
            1, D3DUSAGE_RENDERTARGET,
            D3DFMT_R32F,
            D3DPOOL_DEFAULT,
            &m_pShadowMap,
            NULL ) );

    // create the depth stencile texture
    DXUTDeviceSettings d3dSettings = DXUTGetDeviceSettings();
    V_RETURN( pd3dDevice->CreateDepthStencilSurface( ShadowMap_SIZE,
            ShadowMap_SIZE,
            d3dSettings.d3d9.pp.AutoDepthStencilFormat,
            D3DMULTISAMPLE_NONE,
            0,
            TRUE,
            &m_pDSShadow,
            NULL ) );


    // create the view matrix
    // Calc the view matrix
    D3DXVECTOR3 vUp( 0,1,0 );
    //D3DXVECTOR3 vEyePt(27.89f,11.60f,26.54f);
    //D3DXVECTOR3 vLookAt(27.f, 10.86f, 25.55f);
    //D3DXVECTOR3 vEyePt(15.89f,6.60f,6.54f);
    //D3DXVECTOR3 vLookAt(14.f, 5.86f, 5.55f);
    // side view from last quarter
    //D3DXVECTOR3 vEyePt(26.f, 30.8f, 12.57f);
    //D3DXVECTOR3 vLookAt(17.5f, 6.07f, 11.58f);
    // directly above
    //D3DXVECTOR3 vEyePt(12.5f, 30.8f, 12.57f);
    //D3DXVECTOR3 vLookAt(12.5f, 6.07f, 12.58f);

    D3DXVECTOR3 vEyePt(28.5f, 30.8f, 12.57f);
    D3DXVECTOR3 vLookAt(17.f, 6.07f, 12.58f);
    D3DXMatrixLookAtLH( &m_mShadowView, &vEyePt, &vLookAt, &vUp );
    
    // Initialize the shadow projection matrix
    D3DXMatrixPerspectiveFovLH( &m_mShadowProj, D3DX_PI/4, 1, 25.1f, 40.0f );

   // D3DXMatrixIdentity(&m_mShadowView );
   // D3DXMatrixIdentity(&m_mShadowProj);

        // create the state block
     pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pStateBlock );


         // load the vertex shader
    if( FAILED(pd3dDevice->CreateVertexShader((DWORD const*)VFX_ShadowVertexShade, &m_pVertexShader )))
    {   
        MessageBox(NULL, L"Failed to load vertex shader!!!", L"Game.exe", MB_OK );
    }

    // load the pixel shader
    if( FAILED(pd3dDevice->CreatePixelShader((DWORD const*)PFX_ShadowPixelShade, &m_pPixelShader )))
    {
        MessageBox(NULL, L"Failed to load pixel shader!!!", L"Game.exe", MB_OK );
    }

    return hr;
}

void CShadowMap::SetRT(IDirect3DDevice9 *pd3dDevice)
{

    // set the render target to the shadow map 
    m_pStateBlock->Capture();

    // save the current render target
    ( pd3dDevice->GetRenderTarget( 0, &m_pOldRT ) );
    LPDIRECT3DSURFACE9 pShadowSurf;
    if( SUCCEEDED( m_pShadowMap->GetSurfaceLevel( 0, &pShadowSurf ) ) )
    {
        pd3dDevice->SetRenderTarget( 0, pShadowSurf );
        SAFE_RELEASE( pShadowSurf );
    }

    // save the current depth stencil
    if( SUCCEEDED( pd3dDevice->GetDepthStencilSurface( &m_pOldDS ) ) ) 
        pd3dDevice->SetDepthStencilSurface( m_pDSShadow );

    //colour buffer OFF
   // pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
   // pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
  //  pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
    //lighting OFF
  //  pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    //depth buffer ON (write+test)
    pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    //stencil buffer OFF
//    pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );

    // set the vertex and pixel shaders
    pd3dDevice->SetVertexShader( m_pVertexShader );
    pd3dDevice->SetPixelShader( m_pPixelShader );


}

void CShadowMap::UnSetRT(IDirect3DDevice9 *pd3dDevice)
{
    if( m_pOldDS )
    {
        pd3dDevice->SetDepthStencilSurface( m_pOldDS );
        m_pOldDS->Release();
    }
    pd3dDevice->SetRenderTarget( 0, m_pOldRT );
    SAFE_RELEASE( m_pOldRT );

    m_pStateBlock->Apply();
}

void CShadowMap::SetShadowTexture(IDirect3DDevice9 *pd3dDevice, DWORD stage)
{
    pd3dDevice->SetTexture( stage, m_pShadowMap );
    
}