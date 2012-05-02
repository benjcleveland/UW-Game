#pragma once

#include "line.h"
#include "WorldFile.h"
#include "DXUT\SDKmisc.h"

////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////


class GridLayer
{
public:
    GridLayer();
    ~GridLayer();


    bool initialize(IDirect3DDevice9* pd3dDevice);
    
    // update grid layers
    void UpdateLayers();

    // debug render settings
    void setDebugRender();

    void render( IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld );

    void Release();

    // debug text
    void renderText(CDXUTTextHelper *txtHelper);

    float getValue( int r, int c){ return m_pOccupGrid[ r *m_cy + c];}
    float getVisValue( int r, int c) { return m_pVisGrid[ r* m_cy + c];}

private:
    
    // creates the occupency layer
    void setOccupencyLayer();
    void setCoverLayer();
    void setVisibilityLayer();
    D3DCOLOR GridLayer::determineColor(int y, int x); // helper when rendering
    
    int m_cx, m_cy;
    
    // occupancy grid values
    float* m_pOccupGrid;
    float* m_pVisGrid;
    float* m_pCoverGrid;

    CLine m_debugLine;

    WorldFile g_terrain;

    int m_debug_render;
};
