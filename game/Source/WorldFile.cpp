#include "DXUT.h"
#include "WorldFile.h"

#pragma warning(disable : 4996)

////////////////////////////////////////////////////////////////////////////


WorldFile::WorldFile() : 
    m_cx(0)
,   m_cy(0)
,	m_pGrid(0),
    m_debugList(NULL)
{
}


WorldFile::~WorldFile()
{
    delete [] m_pGrid;

    SAFE_DELETE_ARRAY(m_debugList);
}


bool WorldFile::Load(const LPCWSTR szFilename)
{
    SAFE_DELETE_ARRAY(m_pGrid);
    
    SAFE_DELETE_ARRAY(m_debugList);

	FILE*   fp = _wfopen(szFilename, L"rt");
    if (fp)
    {
        char sz[32768];
        fgets (sz, sizeof(sz), fp);
        sscanf(sz, "%d,%d", &m_cx, &m_cy);
        m_pGrid = new ECell[m_cy * m_cx];
        m_debugList = new DebugNode[m_cy * m_cx];

        for (int y = m_cy -1; y >= 0; --y)
        {
            fgets (sz, sizeof(sz), fp);
            char *pc = sz;
            for (int x = 0; x < m_cx; x++)
            {
                if (*pc == '#')
                {
                    m_pGrid[y * m_cx + x] = OCCUPIED_CELL;
                }
                else if (*pc == '.')
                {
                    m_pGrid[y * m_cx + x] = EMPTY_CELL;
                }
                else
                {
                    m_pGrid[y * m_cx + x] = INVALID_CELL;
                }
                ++pc;
                m_debugList[y * m_cx +x ] = NOT_USED;
            }
        }
        fclose (fp);
        return true;
    }
    return false;
}

WorldFile::ECell WorldFile::operator () ( int row, int col ) const
{
    if (m_pGrid)
    {
        if (0 <= row && row < m_cy && 0 <= col && col < m_cx)
        {
            return m_pGrid[row * m_cx + col];
        }
    }
    return INVALID_CELL;
}


D3DXVECTOR3 WorldFile::GetCoordinates( int r, int c )
{
    D3DXVECTOR3 pos;
    // add .5 to be at the middle of the square
    pos.x = ((float)c ) + 0.5f;
	pos.y = 0.0f;
	pos.z = ((float)r) + 0.5f;
	
    return pos;
}

bool WorldFile::GetRowColumn( D3DXVECTOR3* pos, int* r, int* c )
{
    if( pos->x >= 0.0f && pos->x <= 25.0f && pos->z >= 0.0f && pos->z <= 25.0f )
	{
		*r = (int)(pos->z);// * (float)m_cy);
		*c = (int)(pos->x);// * (float)m_cx);
		return true;
	}
	else
	{
		return false;
	}
}


bool WorldFile::IsWall( int r, int c )
{
    if(m_pGrid)
    {
        if( 0 <= r && r < m_cy && 0 <= c && c < m_cx)
        {
            bool ret = m_pGrid[ r * m_cy + c ] == OCCUPIED_CELL;
            return ret;       
        }
    }
    return 0;
}

void WorldFile::setOpenList( int r, int c)
{
    if( m_debugList )
    {
        if( 0 <= r && r < m_cy && 0 <= c && c < m_cx)
        {
            m_debugList[ r * m_cy + c ] = OPEN_LIST;
        }
    }
}

void WorldFile::setClosedList( int r, int c)
{
    if( m_debugList )
    {
        if( 0 <= r && r < m_cy && 0 <= c && c < m_cx)
        {
            m_debugList[ r * m_cy + c ] = CLOSED_LIST;
        }
    }
}


WorldFile::DebugNode WorldFile::getList( int r, int c)
{
    if( m_debugList )
    {
        if( 0 <= r && r < m_cy && 0 <= c && c < m_cx)
        {
            return m_debugList[ r * m_cy + c ];
        }
    }
    return NOT_USED;
}

void WorldFile::resetDebugList()
{
    if( m_debugList )
    {
        for(int x = 0; x < m_cx; ++x)
        {
            for( int y = 0; y < m_cy; ++y)
            {
                m_debugList[ y * m_cy + x ] = NOT_USED;
            }
        }
    }
}