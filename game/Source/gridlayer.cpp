#include "DXUT.h"
#include "gridlayer.h"
#include "database.h"
#include "gameobject.h"
#include "statemch.h"
#include "collision.h"

#pragma warning(disable : 4996)

////////////////////////////////////////////////////////////////////////////


GridLayer::GridLayer() : 
    m_cx(25),
    m_cy(25),
    m_pOccupGrid(NULL),
    m_pVisGrid(NULL),
    m_pCoverGrid(NULL), m_debug_render(0)
{
}


GridLayer::~GridLayer()
{
    //m_debugLine.Release();
    SAFE_DELETE_ARRAY(m_pOccupGrid);
    SAFE_DELETE_ARRAY(m_pVisGrid );
    SAFE_DELETE_ARRAY(m_pCoverGrid );
}

void GridLayer::Release()
{
    m_debugLine.Release();
}

bool GridLayer::initialize(IDirect3DDevice9* pd3dDevice)
{
    m_pOccupGrid = new float[m_cy * m_cx];
    if( m_pOccupGrid == NULL )
        return false;

    m_pVisGrid = new float[m_cy * m_cx];
    if( m_pVisGrid == NULL )
        return false;

    m_pCoverGrid = new float[m_cy * m_cx];
    if( m_pCoverGrid == NULL )
        return false;

    m_debugLine.lineInit(pd3dDevice);

    WCHAR str[MAX_PATH];
    // hack fix later
    ( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"level.grd" ) );
    // Load the world file
    g_terrain.Load(str);

    this->setVisibilityLayer();

    return true;
}



// this is resource intensive
// modify to only run every so often - on demand for each NPC?
void GridLayer::setCoverLayer()
{
    int x,y;
    dbCompositionList list;
	g_database.ComposeList( list, OBJECT_NPC );
	dbCompositionList::iterator i;
    // loop through all the NPCS
    for( i=list.begin(); i!=list.end(); ++i )
    {
        StateMachine* pStateMachine = (*i)->GetStateMachineManager()->GetStateMachine(STATE_MACHINE_QUEUE_0);
        if( pStateMachine )
        { 
            char* name = (*i)->GetName();
            GameObject *go = g_database.FindByName( name );
            if( go )    
            {
                D3DXVECTOR3 pos = go->getMovement()->getPosition();
                y = (int) pos.z;
                x = (int) pos.x;
                for( int i = x - 4; i < x + 5; ++i)
                {
                    if( i > 0 && i < m_cx )
                    {
                        for( int j = y - 4; j < y +5; ++j)
                        {
                            // can this point be 'seen'?
                            if( j > 0 && j < m_cy )
                            {
                                if( !g_terrain.IsWall(y,x))
                                {
                                    D3DXVECTOR3 new_pos(i +.5f, pos.y , j+.5f); 
                                    bool coll = runNPCSeePlayer(&pos, &new_pos);
                                    if( !coll )
                                        m_pCoverGrid[ m_cy * j + i] = 1.f;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// create the visibility layer
void GridLayer::setVisibilityLayer()
{
    int count = 0;
    int max = 0;

    for( int i = 0; i < m_cx; ++i )
    {
        for( int j = 0; j < m_cy; ++j )
        {
            // figure out how many squares this one can 'see'
            if(!g_terrain.IsWall(j,i))
            {
                // count until we run into a wall
                for( int x = i; x < m_cx; ++x )
                {
                    if(g_terrain.IsWall(j,x))
                        break;
                    for( int y = j; y < m_cy; ++y )
                    {
                        // if is wall break
                        if(g_terrain.IsWall(y,x) || g_terrain.IsWall(y,i) )
                            break;
                        ++count;
                    }

                    for( int y = j-1; y >= 0; --y )
                    {
                        // if is wall break  
                        if(g_terrain.IsWall(y,x) || g_terrain.IsWall(y,i) )
                            break;
                        ++count;
                    }
                }

                for( int x = i-1; x >= 0; --x )
                {
                    if(g_terrain.IsWall(j,x))
                        break;
                    for( int y = j; y < m_cy; ++y )
                    {
                        // if is wall break
                        if(g_terrain.IsWall(y,x)  || g_terrain.IsWall(y,i))
                            break;
                        ++count;
                    }

                    for( int y = j-1; y >= 0; --y )
                    {
                        // if is wall break
                        if(g_terrain.IsWall(y,x) || g_terrain.IsWall(y,i) )
                            break;
                        ++count;
                    }
                }
            }

            m_pVisGrid[ m_cy * j  + i ] = (float)count;
            if(count > max )
                max = count;
            
            count = 0;
        }  
    }

    // make it so the highest is 1.f
    for( int i = 0; i < m_cx; ++i )
    {
        for( int j = 0; j < m_cy; ++j )
        {
            m_pVisGrid[m_cy * j + i ] /= max;
        }
    }
}

/*
void GridLayer::setLineOfFireLayer()
{

}
*/


// create the occupancy grid
void GridLayer::setOccupencyLayer()
{
    int x, y;

    dbCompositionList list;
	g_database.ComposeList( list, OBJECT_NPC );
	dbCompositionList::iterator i;
    // loop through all the NPCS
	for( i=list.begin(); i!=list.end(); ++i )
	{
		StateMachine* pStateMachine = (*i)->GetStateMachineManager()->GetStateMachine(STATE_MACHINE_QUEUE_0);
		if( pStateMachine )
		{ 
			char* name = (*i)->GetName();
            GameObject *go = g_database.FindByName( name );
            if( go  )
            {
                
                D3DXVECTOR3 pos = go->getMovement()->getPosition();
                y = (int) pos.z;
                x = (int) pos.x;
                
                float distx;
                float disty;
                float dist;
                // set the value for the surrounding squares
                for(int i = x - 8; i < x + 9; ++i)
                { 
                    if( i > 0 && i < m_cx )
                    {
                        for( int j = y - 8; j < y + 9; ++j )
                        {
                            if( j > 0 && j < m_cy )
                            {
                                // figure out the distance to this point
                                distx = pow(float(x - i),2);
                                disty = pow(float(y - j),2);
                                dist = sqrt(distx + disty);

                                // set the value
                                m_pOccupGrid[m_cy * j + i] = (m_pOccupGrid[m_cy * j + i] + 1.f/dist >= 1) ? 1 : m_pOccupGrid[ m_cy * j + i] + 1.f/dist;//(float)((float)i / (float)x) + (float)((float)j / (float)y);
                            }
                        }
                    }
                }    
            }   
            // set the value for the square of the current position
            m_pOccupGrid[ m_cy * y  + x ] = 1.f;
        }
    }
    
    // do same for player?
}

// determine the color of the line to draw
D3DCOLOR GridLayer::determineColor(int y, int x)
{
    D3DCOLOR color = D3DCOLOR_COLORVALUE(0,0,0,1);
    switch(m_debug_render )
    {
    case 1: // render position (occupGrid)
        color = D3DCOLOR_COLORVALUE((m_pOccupGrid[ m_cy *y + x] ),0,0,1);
        break;
    case 2: // cover grid
        color = D3DCOLOR_COLORVALUE(0,(m_pCoverGrid[ m_cy *y + x]),0,1);
        break;

    case 3: // visablitiy
        color = D3DCOLOR_COLORVALUE(0,0,(m_pVisGrid[ m_cy *y + x]),1);
        break;
    case 4:     // combination
        color = D3DCOLOR_COLORVALUE((((1 - m_pOccupGrid[ m_cy *y + x]) * m_pVisGrid[ m_cy *y + x] + m_pCoverGrid[ m_cy *y + x] > 1) ?
            1 : (1 - m_pOccupGrid[ m_cy *y + x]) * m_pVisGrid[ m_cy *y + x] + m_pCoverGrid[ m_cy *y + x]), 0, 0,1); 
        break;
    default:
        // should never happen
        break;
    }
    return color;
}

// render the grid for debugging
void GridLayer::render( IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld )
{
    if( m_debug_render > 0 )
    {
        for(int x = 0; x < m_cx; ++x)
        {
            for( int y = 0; y < m_cy; ++y)
            {
                // only render if it is not a wall
                if( !g_terrain.IsWall(y,x) )
                {
                    // create line with color
                    m_debugLine.setColor(determineColor(y,x));
                    m_debugLine.setPositionQuad(y,x);
                    // set the value to 0

                   m_pCoverGrid[m_cy * y + x] = 0;
                }
            }
        }

        m_debugLine.RenderQuad(pd3dDevice,matWorld);
    }
}

void GridLayer::setDebugRender()
{
    m_debug_render = (m_debug_render >= 4 ) ? 0 : ++m_debug_render;
}

void GridLayer::renderText(CDXUTTextHelper *txtHelper)
{
    switch(m_debug_render )
    {
    case 1: // render position (occupGrid)
        txtHelper->DrawFormattedTextLine(L"\nTerrainLayer = Occupancy");
        break;
    case 2: // cover grid
        txtHelper->DrawFormattedTextLine(L"\nTerrainLayer = Dynamic cover");
        break;

    case 3: // visablitiy
        txtHelper->DrawFormattedTextLine(L"\nTerrainLayer = Static visibility");
        break;
    case 4:     // combination
        txtHelper->DrawFormattedTextLine(L"\nTerrainLayer = Combination");
        break;
    default:
        txtHelper->DrawFormattedTextLine(L"\nTerrainLayer = None");
        break;
    }
}

void GridLayer::UpdateLayers()
{
    memset(m_pOccupGrid,0,sizeof(float)*m_cy*m_cx);
    setOccupencyLayer();
    if( m_debug_render == 2 || m_debug_render == 4)
        setCoverLayer();
}