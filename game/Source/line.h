#pragma once

#define D3DFVF_LINE_VERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE )

#define MAX_NUM_LINES 10000

class CLine
{
public:
    CLine();
    ~CLine();

    void lineInit(IDirect3DDevice9* pd3dDevice);

    void Release(void);
    void Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);
    
    void RenderArray(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);
    void RenderQuad( IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld );

    void setPosition( D3DXVECTOR3 start, D3DXVECTOR3 end);
    void setPositionArray(D3DXVECTOR3 start, D3DXVECTOR3 end);
    void setPositionSquare( int r, int c );
    void setPositionQuad( int r, int c);

    void setColor( D3DCOLOR new_color);
    D3DCOLOR getColor(){ return color; };

private:
    	// A structure for our custom vertex type
	struct LINEVERTEX
	{
		FLOAT x, y, z;      // The untransformed, 3D position for the vertex
		DWORD color;        // The vertex color
	};

    LINEVERTEX m_lines[MAX_NUM_LINES];
    
    int m_num_lines;

    LPDIRECT3DVERTEXBUFFER9 m_linevb;
    LPDIRECT3DVERTEXBUFFER9 m_linevb_array;

    D3DCOLOR color;

};