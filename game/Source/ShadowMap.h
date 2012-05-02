
#pragma once

class CShadowMap
{
public:
    CShadowMap();
    ~CShadowMap();

    void Release();
    HRESULT Init( IDirect3DDevice9* pd3dDevice );

    void SetRT( IDirect3DDevice9* pd3dDevice );

    void UnSetRT( IDirect3DDevice9* pd3dDevice );

    void SetShadowTexture( IDirect3DDevice9 *pd3dDevice, DWORD stage );

    D3DXMATRIXA16   m_mShadowView;
    D3DXMATRIXA16   m_mShadowProj;
private:

    LPDIRECT3DTEXTURE9              m_pShadowMap;    // Texture to which the shadow map is rendered
    LPDIRECT3DSURFACE9              m_pDSShadow;     // Depth-stencil buffer for rendering to shadow map
    LPDIRECT3DSURFACE9 m_pOldRT;
    LPDIRECT3DSURFACE9 m_pOldDS;

    IDirect3DStateBlock9* m_pStateBlock;
    IDirect3DVertexShader9 *m_pVertexShader;
    IDirect3DPixelShader9 *m_pPixelShader;

};