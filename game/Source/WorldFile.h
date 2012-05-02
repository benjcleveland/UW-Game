#pragma once

////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////


class WorldFile
{
public:
    WorldFile();
    ~WorldFile();

    enum ECell
    {
        INVALID_CELL = -1, 
        EMPTY_CELL = 0, 
        OCCUPIED_CELL = 1, 
        CELL_MAX
    };
    
    enum DebugNode
    {
        OPEN_LIST,
        CLOSED_LIST,
        NOT_USED
    };

    bool        Load(const LPCWSTR szFilename);

    ECell       operator () ( int row, int col ) const;
	int         GetWidth()  { return m_cx; }
	int         GetHeight() { return m_cy; }

    D3DXVECTOR3 GetCoordinates( int r, int c );
	bool GetRowColumn( D3DXVECTOR3* pos, int* r, int* c );
	bool IsWall( int r, int c );

    // for debug purposes
    void setOpenList( int r, int c);
    void setClosedList( int r, int c);
    DebugNode getList( int r, int c);
    void resetDebugList();


private:
    int m_cx, m_cy;
    ECell* m_pGrid;

    // this should be merged with ECell
    DebugNode *m_debugList;
};
