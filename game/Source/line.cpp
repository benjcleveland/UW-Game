#include "DXUT.h"
#include "line.h"

using namespace std;

#define MAX_SIZE 2

CLine::CLine() : m_linevb(NULL), m_linevb_array(NULL), color(D3DCOLOR_COLORVALUE( 1, 0.5, 0, 1.0 )), m_num_lines(0){}
CLine::~CLine()
{
    Release();
}

void CLine::Release(void)
{
    SAFE_RELEASE(m_linevb);
    SAFE_RELEASE(m_linevb_array);
}
void CLine::lineInit(IDirect3DDevice9* pd3dDevice)
{
    // Create the vertex buffer.
    if( FAILED( pd3dDevice->CreateVertexBuffer( MAX_SIZE * sizeof(LINEVERTEX),
        0, D3DFVF_LINE_VERTEX,
        D3DPOOL_DEFAULT, &m_linevb, NULL ) ) )
    {
        return;	// E_FAIL;
    }

    if( FAILED( pd3dDevice->CreateVertexBuffer( MAX_NUM_LINES * sizeof(LINEVERTEX),
        0, D3DFVF_LINE_VERTEX,
        D3DPOOL_DEFAULT, &m_linevb_array, NULL ) ) )
    {
        return;	// E_FAIL;
    }
    D3DXVECTOR3 zero(0,0,0);
    setPosition(zero, zero);
}


void CLine::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{

    pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
    // Set the texture
    pd3dDevice->SetTexture(0, NULL);
    // Turn off D3D lighting, since we are providing our own vertex colors
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, false );
    pd3dDevice->SetStreamSource( 0, m_linevb, 0, sizeof(LINEVERTEX) );
    pd3dDevice->SetFVF( D3DFVF_LINE_VERTEX );
    pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, MAX_SIZE/2 );
    // enable lighting
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );
}

void CLine::RenderArray(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
    // Fill the vertex buffer.
    VOID* pVertices;
    if( FAILED( m_linevb_array->Lock( 0, sizeof(LINEVERTEX)*m_num_lines, (void**)&pVertices, 0 ) ) )
        return;	// E_FAIL;
    memcpy( pVertices, m_lines, sizeof(LINEVERTEX)*m_num_lines );
    m_linevb_array->Unlock();

    pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
    // Set the texture
    pd3dDevice->SetTexture(0, NULL);
    // Turn off D3D lighting, since we are providing our own vertex colors
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, false );
    pd3dDevice->SetStreamSource( 0, m_linevb_array, 0, sizeof(LINEVERTEX) );
    pd3dDevice->SetFVF( D3DFVF_LINE_VERTEX );
    pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, m_num_lines/2 );
    // enable lighting
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );

    // set number of lines to 0 so we can start over
    m_num_lines = 0;
}

void CLine::RenderQuad( IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld )
{
     // Fill the vertex buffer.
    VOID* pVertices;
    if( FAILED( m_linevb_array->Lock( 0, sizeof(LINEVERTEX)*m_num_lines, (void**)&pVertices, 0 ) ) )
        return;	// E_FAIL;
    memcpy( pVertices, m_lines, sizeof(LINEVERTEX)*m_num_lines );
    m_linevb_array->Unlock();

    pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
    // Set the texture
    pd3dDevice->SetTexture(0, NULL);

    // set the depth bias
    float bias = -.000001f;
    pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)(&bias));

    // Turn off D3D lighting, since we are providing our own vertex colors
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, false );
    pd3dDevice->SetStreamSource( 0, m_linevb_array, 0, sizeof(LINEVERTEX) );
    pd3dDevice->SetFVF( D3DFVF_LINE_VERTEX );
    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_num_lines/3 );
    // enable lighting
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );

    // reset depth bias to 0
    bias = 0.f;
    pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)(&bias));
    // set number of lines to 0 so we can start over
    m_num_lines = 0;
}
void CLine::setPosition( D3DXVECTOR3 start, D3DXVECTOR3 end)
{

    LINEVERTEX linev[] = 
    {
        {start.x, start.y, start.z, color},
        {end.x, end.y, end.z, color}
    };
    // Fill the vertex buffer.
    VOID* pVertices;
    if( FAILED( m_linevb->Lock( 0, sizeof(LINEVERTEX)*MAX_SIZE, (void**)&pVertices, 0 ) ) )
        return;	// E_FAIL;
    memcpy( pVertices, linev, sizeof(LINEVERTEX)*MAX_SIZE );
    m_linevb->Unlock();
}

void CLine::setPositionArray(D3DXVECTOR3 start, D3DXVECTOR3 end)
{
    LINEVERTEX linev[] = 
    {
        {start.x, start.y, start.z, color},
        {end.x, end.y, end.z, color}
    };

    if( m_num_lines < MAX_NUM_LINES)
    {
        memcpy(&m_lines[m_num_lines], linev, sizeof(LINEVERTEX)*MAX_SIZE);
        m_num_lines += 2;
    }
}

// this function will render a square using 4 lines
void CLine::setPositionSquare( int r, int c )
{
    D3DXVECTOR3 point,point1;

    point.x = (float)c;
    point.z = (float)r;
    point.y =0.1f;
    point1.y =0.1f;
    point1.x = (float)c + .991f;
    point1.z = (float)r;

    setPositionArray(point, point1);
    point1.x = (float)c;
    point1.z = (float)r + .991f;
    setPositionArray(point, point1);
    point.x = (float)c + .991f;
    point.z = (float)r + .991f;
    setPositionArray(point1, point);
    point1.x = (float)c + .991f;
    point1.z = (float)r;
    setPositionArray(point1, point); 
}

void CLine::setPositionQuad( int r, int c)
{
     LINEVERTEX linev[] =
    {
        // back
        {  0.0f +c, 0, 0.0f +r,color },
        {  0.0f+ c, 0, 1.0f + r,color },
        {  1.0f+ c, 0, 1.0f+ r,color },
        {  1.0f+ c, 0, 1.0f+ r,color },
        {  1.0f+ c, 0, 0.0f+ r,color },
        {  0.0f+ c, 0, 0.0f+ r, color }
    };    

     if( m_num_lines < MAX_NUM_LINES)
    {
        memcpy(&m_lines[m_num_lines], linev, sizeof(LINEVERTEX)* 6);
        m_num_lines += 6;
    }
}

void CLine::setColor( D3DCOLOR new_color)
{
    color = new_color;
}
