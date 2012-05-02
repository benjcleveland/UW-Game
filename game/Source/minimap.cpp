#include "DXUT.h"
#include "minimap.h"
#include "D3D9.h"
#include "database.h"
#include "statemch.h"
#include "gameobject.h"


// constructor
cMiniMap::cMiniMap() : pStateBlock(NULL)
{

    MINIMAPVERTEX verts[4]  = 
    {
       {15,15,0,1, -.125f, .125f, 0, 0, 0,1   },
        {125,15,0,1, .125f, .125f, 1,0 , 1, 1},
        {15,125,0,1, -.125f, -.125f, 0, 1, 0, 0},
        {125,125,0,1, .125f, -.125f,1,1, 1,0} 
    };

    vertices = new MINIMAPVERTEX[4];
    memcpy(vertices, &verts, sizeof(verts));
    D3DXMatrixIdentity( &m_World );
}

// destructor
cMiniMap::~cMiniMap()
{
    this->Release();
    SAFE_DELETE_ARRAY(vertices);
}

void cMiniMap::Release()
{
    SAFE_RELEASE(m_pMapAlphaTexture);
    SAFE_RELEASE(m_pMapTexture);
    SAFE_RELEASE(m_pPlayerMark);
    SAFE_RELEASE(pStateBlock);
}


// init the minimap (read in all textures)
void cMiniMap::InitMiniMap(IDirect3DDevice9* pd3dDevice)
{

    WCHAR str[MAX_PATH];

    // read in all our textures

    // Use D3DX to create a texture from a file based image
    DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"minimapframe.dds" );
    if( FAILED( D3DXCreateTextureFromFile( pd3dDevice, str, &m_pMapAlphaTexture ) ) )
    {
        MessageBox( NULL, L"Could not find banana.bmp", L"Textures.exe", MB_OK );
        // return E_FAIL;  
    }
    // Use D3DX to create a texture from a file based image
    DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"minimap2.jpg" );
    if( FAILED( D3DXCreateTextureFromFile( pd3dDevice, str, &m_pMapTexture ) ) )
    {
        MessageBox( NULL, L"Could not find banana.bmp", L"Textures.exe", MB_OK );
        // return E_FAIL;  
    }
    // Use D3DX to create a texture from a file based image
    DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"playercircle1.dds" );
    if( FAILED( D3DXCreateTextureFromFile( pd3dDevice, str, &m_pPlayerMark ) ) )
    {
        MessageBox( NULL, L"Could not find banana.bmp", L"Textures.exe", MB_OK );
        // return E_FAIL;  
    }

    // capture the state
    pd3dDevice->CreateStateBlock(D3DSBT_ALL, &pStateBlock );
}
void cMiniMap::SetPositionAndFacing(D3DXVECTOR3 pos, float facing)
{
    D3DXMATRIX mx;
    D3DXMatrixIdentity( &m_World );

    MINIMAPVERTEX verts[4]  = 
    {
        {15,15,0,1, -.125f, .125f, 0, 0, 0,1  },
        {125,15,0,1, .125f, .125f, 1,0 , 1, 1 },
        {15,125,0,1, -.125f, -.125f, 0, 1, 0,0 },
        {125,125,0,1, .125f, -.125f,1,1, 1,0 } 
    };

    // save the player position
    m_player_pos = pos;
    m_facing = facing- D3DX_PI/2;

    // update the vertices coords
    for(int i = 0; i < 4; ++i )
    {
        float tu = verts[i].tu;
        float tv = verts[i].tv;
        vertices[i].tu = (tu * cos( m_facing ) - tv *sin(m_facing)+(pos.x)/25.f);
        vertices[i].tv = (tu*sin(m_facing) + tv*cos(m_facing)+(pos.z)/25.f);

    }



}

// render the minimap
void cMiniMap::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld)
{
    // save the state 
    pStateBlock->Capture();

    // Set the world space transform
    pd3dDevice->SetTransform(D3DTS_TEXTURE0, &m_World);

    // disable z-buffering
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

    // enable alpha-blending
    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

    //pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DPCMPCAPS_GREATEREQUAL);
    //pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0);
    //pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);

    // disable pixel shader
    pd3dDevice->SetPixelShader(NULL);

    pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // disable culling 
    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);

    // set the texture
    pd3dDevice->SetTexture(0, m_pMapTexture);
    pd3dDevice->SetTexture(1, m_pMapAlphaTexture);   
    pd3dDevice->SetTexture(2, m_pPlayerMark);
    pd3dDevice->SetTexture(3, m_pPlayerMark);

    // mini map
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    //pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
    //pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
    pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    // alpha map
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

    // player marker
    pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_ADDSMOOTH );
    pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG2,D3DTA_CURRENT);

    // npc marker
    // pd3dDevice->SetTextureStageState( 3, D3DTSS_TEXCOORDINDEX, 2);
    //pd3dDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_ADDSMOOTH );
    //pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    //pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG2,D3DTA_CURRENT);

    pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

    // render the minimap
    pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(0) );
    pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(vertices[0]));
    

    // render the NPC positions
    pd3dDevice->SetTexture(0, m_pPlayerMark);
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2,D3DTA_CURRENT);
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0) );

    dbCompositionList list;
	g_database.ComposeList( list, OBJECT_NPC );
	dbCompositionList::iterator i;
	for( i=list.begin(); i!=list.end(); ++i )
	{
		StateMachine* pStateMachine = (*i)->GetStateMachineManager()->GetStateMachine(STATE_MACHINE_QUEUE_0);
		if( pStateMachine ) // we only care if its in the 'launched' state
		{
			char* name = (*i)->GetName();
            GameObject *go = g_database.FindByName( name );
            if( go )
            {
                D3DXVECTOR3 npc_pos = go->getMovement()->getPosition();
                D3DXVECTOR3 diff_pos = npc_pos - m_player_pos;
                D3DXVECTOR3 norm;
                D3DXVec3Normalize(&norm,&diff_pos);
                float dist = D3DXVec3Dot(&norm, &diff_pos);

                // only render if within distance
                if( dist < 2.75f )
                {
                    // lame hack here to get it to work
                    vertices[0].tu = 0;
                    vertices[0].tv = 1;
                    vertices[1].tu = 1;
                    vertices[1].tv = 1;
                    vertices[2].tu = 0;
                    vertices[2].tv = 0;
                    vertices[3].tu = 1;
                    vertices[3].tv = 0;

                    for( int i = 0; i < 4; ++i )
                    {
                       vertices[i].tu -= cos(-m_facing)*diff_pos.x/6.5f -sin(-m_facing)*diff_pos.z/6.5f;
                       vertices[i].tv -= sin(-m_facing)*diff_pos.x/6.5f + cos(-m_facing)*diff_pos.z/6.5f;
                    }
                    
                    // draw
                    pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(vertices[0]));
                }
            }
        }
    }
    // replace the state and release
    pStateBlock->Apply();
}
