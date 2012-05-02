#include "DXUT.h"
#include "DXUT\SDKmisc.h"

#define AnimatedMesh_cpp 1
#include "AnimatedMesh.h"

// include the shaders
#include "AnimatedMeshVertexShade.vfxobj"
#include "AnimatedPixelShade.pfxobj"

using namespace std;

#define AnimatedMesh_cpp 1

#define MAX_NUM_BONES 8
#define CYLINDER_LENGTH 1.5f

CAnimMesh::CAnimMesh() : m_pmesh(NULL),  pVD(NULL), m_pPixelShader(NULL), m_pVertexShader(NULL)
{
    num_bones = MAX_NUM_BONES;
    bones = new CBone[MAX_NUM_BONES];
}

CAnimMesh::~CAnimMesh()
{
    Release();
    SAFE_DELETE_ARRAY(bones);
}

void CAnimMesh::Release(void)
{
    SAFE_RELEASE(m_pmesh);
    SAFE_RELEASE(m_pStateBlock );
    SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pPixelShader );
    SAFE_RELEASE( pVD );
    //SAFE_DELETE_ARRAY(bones);
}

void CAnimMesh::Init(IDirect3DDevice9* pd3dDevice)
{
    LPDIRECT3DVERTEXBUFFER9 pVB;
    void* pVertices;
    LPD3DXMESH lamemesh;
    AnimatedVertex *test;

    // Create the vertex buffer.
    if( FAILED( D3DXCreateCylinder( pd3dDevice, .1f, .0125f, CYLINDER_LENGTH, 10, 10, &lamemesh, NULL )))
    {
         MessageBox(NULL, L"Could not create cylinder", L"Error", MB_OK);
    }

    // create a clone of the mesh
    lamemesh->CloneMesh(0, AnimatedVertex::declaration, pd3dDevice, &m_pmesh);
    
    // release the lamemesh
    SAFE_RELEASE( lamemesh );

    // get the vertex buffer
    m_pmesh->GetVertexBuffer(&pVB);

    // get all the vertices
    if(FAILED(pVB->Lock(0, m_pmesh->GetNumBytesPerVertex() * m_pmesh->GetNumVertices(), (void**)&pVertices, 0 ) ) ) 
        return;

    // loop through all the vertices
    for( unsigned int i = 0; i < m_pmesh->GetNumVertices(); ++i )
    {  
        test = (AnimatedVertex*)pVertices;

        // modify the position of the cylinder
        test->position.z += CYLINDER_LENGTH/2;

        test->indices.x = floor(test->position.z / (CYLINDER_LENGTH/this->num_bones));
        if( test->indices.x >=this->num_bones )
            --test->indices.x;      // little bit of a hack

        // figure out the weight        
        test->weights.y = 0;//test->position.z/ (CYLINDER_LENGTH/this->num_bones) - test->indices.x;
        test->weights.x = 1;// - test->weights.y;

        pVertices = (char *)pVertices + m_pmesh->GetNumBytesPerVertex();
    }

    pVB->Unlock();

    // release the vertex buffer 
    SAFE_RELEASE(pVB);

     // load the vertex shader
    if( FAILED(pd3dDevice->CreateVertexShader((DWORD const*)VFX_AnimatedMeshVertexShade, &m_pVertexShader )))
    {   
        MessageBox(NULL, L"Failed to load vertex shader!!!", L"Game.exe", MB_OK );
    }

     // load the vertex shader
    if( FAILED(pd3dDevice->CreatePixelShader((DWORD const*)PFX_AnimatedPixelShade, &m_pPixelShader )))
    {   
        MessageBox(NULL, L"Failed to load vertex shader!!!", L"Game.exe", MB_OK );
    }

    // create the vertex decleation
    pd3dDevice->CreateVertexDeclaration( AnimatedVertex::declaration, &pVD);

    // create the state block
     pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pStateBlock );

     // initialize the matrices
     setLocation( D3DXVECTOR3(0,0,0));
}


void CAnimMesh::setLocation(D3DXVECTOR3 location)
{
    D3DXMATRIX aInv;
    D3DXMATRIX bInv;
    D3DXMATRIX mx;

    m_position = location;
    for( int i = 0; i < num_bones; ++i )
    {
        if( i == 0 )
            bones[i].initBone(NULL, 0);
        else
            bones[i].initBone(&bones[i-1], CYLINDER_LENGTH/this->num_bones);
    }
    // init orientation matrix

    D3DXMatrixIdentity(&m_Orientation);
    D3DXMatrixRotationX( &mx, -D3DX_PI/2 );
    D3DXMatrixMultiply( &m_Orientation, &m_Orientation, &mx );
    D3DXMatrixTranslation(&mx, location.x, location.y, location.z);
    D3DXMatrixMultiply( &m_Orientation, &m_Orientation, &mx );
  
}

void CAnimMesh::Update( float fElapsedTime)
{
    D3DXMATRIX mx;
    //D3DXMatrixRotationX(&m_matrixArot,cos(fElapsedTime)*0);
    //D3DXMatrixRotationX(&mx, cos(fElapsedTime)/2);
    D3DXMatrixIdentity(&mx);
    bones[0].update(&mx);

    // decide what to do here
    D3DXMatrixRotationX(&mx,cos(fElapsedTime)/4);
    for( int i = 1; i < this->num_bones; ++i)
    {
        bones[i].update(&mx);
    }
    /*D3DXMatrixRotationX(&mx, sin(fElapsedTime)/2);
    for( int i = 2; i < this->num_bones; i+=2 )
    {
        bones[i].update(&mx);
    }*/

    //D3DXMatrixTranslation(&mx, 0,0, 1.25f);
    //D3DXMatrixMultiply( &m_matrixBrot, &mx, &m_matrixBrot );

    //D3DXMATRIX mx;
    //D3DXMatrixRotationY(&m_World, m_facing );
    //D3DXMatrixTranslation(&mx, currPosition.x, currPosition.y, currPosition.z);
    //D3DXMatrixMultiply(&m_World, &m_World, &mx );
    D3DXMatrixIdentity(&m_World);
    D3DXMatrixMultiply(&m_World,&m_Orientation, &m_World);
}


// render the animated mesh
void CAnimMesh::Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld, bool bRenderShadow)
{

       // clean this up
    D3DXMATRIX matView;
    D3DXMATRIX matProj;
    D3DXMATRIX res;
    D3DXMATRIX light;
    D3DXMATRIX m_trans;
    D3DXMATRIX skinned[MAX_NUM_BONES];
    
    pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
    pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

    D3DXMatrixTranspose(&res, &( m_World * matView  *matProj));
    D3DXMatrixTranspose(&m_trans,&matWorld);
    D3DXMatrixTranspose(&light, &m_World);

    // calculate the matrices for all the bones
    for( int i = 0; i < this->num_bones; ++i)
    {
        if( i == 0)
            skinned[i] = bones[i].getSkinMatrix(&m_trans);
        else
            skinned[i] = bones[i].getSkinMatrix(&skinned[i-1]);
    }

    // capture the current state
    m_pStateBlock->Capture();

    // set the vertex shader constants
    pd3dDevice->SetVertexShaderConstantF(18, (float const*)skinned, 4*MAX_NUM_BONES);

    pd3dDevice->SetTransform(D3DTS_WORLD, &m_World);
    // Set the texture
    pd3dDevice->SetTexture(0, NULL);
    
    if( !bRenderShadow )
    {
        // Turn on D3D lighting
        pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );
        
        pd3dDevice->SetPixelShader( m_pPixelShader );
    }
    pd3dDevice->SetVertexShader( m_pVertexShader );
    pd3dDevice->SetVertexDeclaration(pVD);

    // set vertex shader constants
    pd3dDevice->SetVertexShaderConstantF(0, (float const*)res, 4);
    pd3dDevice->SetVertexShaderConstantF(4, (float const*)light, 3);
    //pd3dDevice->SetVertexShaderConstantF(18, (float const*)m_matrixBskin, 4);
    //pd3dDevice->SetVertexShaderConstantF(22, (float const*)m_matrixAskin, 4);
  

    m_pmesh->DrawSubset( 0 );
    
    // apply previous state
    m_pStateBlock->Apply();
}


// bone class

CBone::CBone(){}
CBone::~CBone(){}

D3DXMATRIX CBone::getMatrix()
{
    return m_matrix;
}

void CBone::initBone( CBone *parent, float offset )
{
    D3DXMATRIX aInv;
    D3DXMATRIX bInv;
    D3DXMATRIX mx;

    D3DXMatrixIdentity( &m_matrix );

    if( parent )
    {
        D3DXMatrixTranslation(&mx, 0,0, offset);
        D3DXMatrixMultiply( &m_matrix, &m_matrix, &mx );
    
        m_matrix = m_matrix * parent->getMatrix();
    }
    
    D3DXMatrixInverse( &m_invMatrix, NULL,&m_matrix);
}

void CBone::update(D3DXMATRIX *rotation )
{
    m_matrixSkin = m_invMatrix * (*rotation) * m_matrix;   
}

D3DXMATRIX CBone::getSkinMatrix(D3DXMATRIX *world)
{
    return m_matrixSkin * (*world);
}