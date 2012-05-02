//--------------------------------------------------------------------------------------
// Based upon MultiAnimation.cpp from the DirectX demos. 
//
// Starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "DXUT.h"
#pragma warning(disable: 4995)

#include "resource.h"
#include "DXUT\DXUTcamera.h"
#include "DebugCamera.h"
#include "DXUT\DXUTsettingsdlg.h"
#include "DXUT\SDKmisc.h"
#include "DXUT\SDKsound.h"
#include "Game.h"
#include "TeapotNode.h"
#include "WorldNode.h"
#include "World.h"
#include "Tiny.h"
#include "PlayerCamera.h"
#include "collision.h"
#include "minimap.h"
#include "AnimatedMesh.h"
#include "ParticalSystem.h"
#include "ShadowMap.h"
#include "database.h"
#include "gameobject.h"
#include "statemch.h"

using namespace std;

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
//CFirstPersonCamera      g_Camera;               // A model viewing camera
CBaseCamera             *g_pCamera;
CCameras                g_Cameras;
cMiniMap                g_MiniMap;
CDXUTDialogResourceManager g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg         g_SettingsDlg;          // Device settings dialog
CDXUTDialog             g_HUD;                  // dialog for standard controls
CDXUTDialog             g_SampleUI;             // dialog for sample specific controls
CSoundManager           g_Sound;                // direct sound class
CSound                  *g_pBackground;
CTiny*                  g_pPlayer;               //  tiny character object

Skybox                  g_skybox;

bool                    g_bShowHelp = true;     // If true, it renders the UI control text
bool                    g_bPlaySounds = true;   // whether to play sounds
bool                    g_bRenderDebugText = false; // whether to render debug text
int                     g_iRenderDebugAStar = -1;
double                  g_fLastAnimTime = 0.0;  // Time for the animations
float                   g_fSpeed = 1.f;           // speed to run the game
World					g_World;				// World for creating singletons and objects

D3DXVECTOR4             g_VSConstants[18];
bool                    g_bDirectionalLight = true;

CShadowMap              g_ShadowMap;

VecCollQuad vecCollQuad;
Node*					g_pScene = 0;


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_NEXTVIEW            6
#define IDC_PREVVIEW            7
#define IDC_RESETCAMERA         11
#define IDC_RESETTIME           12


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------

void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh );
void    RenderText();


//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
bool InitApp()
{
    // Initialize dialogs
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
    //g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    //g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    //g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;
    g_SampleUI.AddButton( IDC_NEXTVIEW, L"(N)ext View", 45, iY += 26, 120, 24, L'N' );
    g_SampleUI.AddButton( IDC_PREVVIEW, L"(P)revious View", 45, iY += 26, 120, 24, L'P' );
    g_SampleUI.AddButton( IDC_RESETCAMERA, L"(R)eset view", 45, iY += 26, 120, 24, L'R' );
    //g_SampleUI.AddButton( IDC_RESETTIME, L"Reset time", 45, iY += 26, 120, 24 );

    // Add mixed vp to the available vp choices in device settings dialog.
    DXUTGetD3D9Enumeration()->SetPossibleVertexProcessingList( true, false, false, true );

    // Setup the free view camera with view matrix
    D3DXVECTOR3 vEye(8.5f, 2.8f, -0.77f);
    D3DXVECTOR3 vAt(7.82f, 2.22f, -0.33f);

    g_Cameras.newDebugCam(vEye,vAt,0.05f,5.0f);
    
    g_Cameras.newPlayerCam(g_pPlayer);
    g_pCamera = g_Cameras.nextCam(); // set initial cam to player
    // add stationary debug camera
    D3DXVECTOR3 vEye1(12.5f, 22.8f, 11.57f);
    D3DXVECTOR3 vAt1(12.5f, 22.07f, 11.58f);
    g_Cameras.newDebugCam(vEye1,vAt1,0,0);

    //g_Cameras.newPlayerCam(&g_Mesh[0]);
    // set the main camera to debug
   
    g_World.InitializeSingletons();

    // initialize the sound manager
    
    //g_Sound.SetPrimaryBufferFormat(2, 44100, 16);
   
    return true;
}


//--------------------------------------------------------------------------------------
// Clean up the app
//--------------------------------------------------------------------------------------
void    CleanupApp()
{
    // Do any sort of app cleanup here 
}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                 D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
        AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
        D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // Need to support ps 1.1
    if( pCaps->PixelShaderVersion < D3DPS_VERSION( 1, 1 ) )
        return false;

    // Need to support A8R8G8B8 textures
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
        AdapterFormat, 0,
        D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8 ) ) )
        return false;

    // might want to change this
    if(!(pCaps->MaxSimultaneousTextures > 1)) //number of textures
 {
        return false;
 }

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the
// application to modify the device settings. The supplied pDeviceSettings parameter
// contains the settings that the framework has selected for the new device, and the
// application can make any desired changes directly to this structure.  Note however that
// DXUT will not correct invalid device settings so care must be taken
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW
    // then switch to SWVP.
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
        pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // If the hardware cannot do vertex blending, use software vertex processing.
    if( pCaps->MaxVertexBlendMatrices < 2 )
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    // If using hardware vertex processing, change to mixed vertex processing
    // so there is a fallback.
    if( pDeviceSettings->d3d9.BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;

    // Debugging vertex shaders requires either REF or software vertex processing
    // and debugging pixel shaders requires REF.
#ifdef DEBUG_VS
    if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning(  pDeviceSettings->ver  );
    }

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// created, which will happen during application initialization and windowed/full screen
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these
// resources need to be reloaded whenever the device is destroyed. Resources created
// here should be released in the OnDestroyDevice callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;
    

    V_RETURN( g_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnD3D9CreateDevice( pd3dDevice ) );
    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Arial", &g_pFont ) );

    // Create the default scene
    g_pScene = new WorldNode(pd3dDevice);


    //g_Cameras.resetPlayerCam(&g_Mesh[0]);
 
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// reset, which will happen after a lost device scenario. This is the best location to
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever
// the device is lost. Resources created here should be released in the OnLostDevice
// callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice,
                               const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;
    WCHAR str[MAX_PATH];
    V_RETURN( g_DialogResourceManager.OnD3D9ResetDevice() );
    V_RETURN( g_SettingsDlg.OnD3D9ResetDevice() );


     // Restore steps for tiny instances

    //    g_Player.RestoreDeviceObjects( pd3dDevice );
    // setup player
    g_pPlayer = new CTiny( pd3dDevice );
    g_pPlayer->Setup(0.f, D3DXVECTOR3(12.f, 0.f,11.f), &g_Sound );
    g_Cameras.resetPlayerCam(g_pPlayer);
    

    g_World.Initialize(pd3dDevice, &g_Sound);
    g_MiniMap.InitMiniMap(pd3dDevice);

    V_RETURN(g_skybox.Setup( pd3dDevice, L"skybox2.x" ));
    
    if(g_pScene)
        SAFE_DELETE(g_pScene);
    g_pScene = new WorldNode(pd3dDevice);

    // Use D3DX to create a texture from a file based image
    DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"Spooky.wav" );
    // load the sound
    hr = g_Sound.Create(&g_pBackground,str,DSBCAPS_CTRLVOLUME);
    g_pBackground->Play(0,DSBPLAY_LOOPING);

    // get device caps
    D3DCAPS9 caps;
    pd3dDevice->GetDeviceCaps( & caps );

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );

    // Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Cameras.ResetDevice(fAspectRatio);

    // Material sources
    pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    pd3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
    pd3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
    pd3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

    // Initialize default material
    D3DMATERIAL9		materialDefault;
    materialDefault.Ambient = D3DXCOLOR(1, 1, 1, 1.0f);		// White to ambient lighting
    materialDefault.Diffuse = D3DXCOLOR(1, 1, 1, 1.0f);		// White to diffuse lighting
    materialDefault.Emissive = D3DXCOLOR(0, 0, 0, 1.0);		// No emissive
    materialDefault.Power = 0;
    materialDefault.Specular = D3DXCOLOR(0, 0, 0, 1.0);		// No specular
    pd3dDevice->SetMaterial(&materialDefault);

    // Set a default light
    // White, directional, pointing downward
    D3DLIGHT9 light;
    memset(&light, 0, sizeof(light));
    light.Type			= D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r		= 1.0f;
    light.Diffuse.g		= 1.0f;
    light.Diffuse.b		= 1.0f;
    light.Ambient.r		= 0.15f;
    light.Ambient.g		= 0.15f;
    light.Ambient.b		= 0.15f;
    light.Direction.x	= -.75f;
    light.Direction.y	= -1.f;
    light.Direction.z	= 0.f;
    light.Attenuation0	= 1.0f;
    light.Range			= 10000.f;
    pd3dDevice->SetLight(0, &light);
    pd3dDevice->LightEnable(0, TRUE);
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);
    pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, false);
    pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, false);
    pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

    // ambient light color
    D3DXCOLOR ambient(.05f, .05f, .05f, .05f);
    pd3dDevice->SetRenderState( D3DRS_AMBIENT, ambient );
    *(D3DXCOLOR*)&g_VSConstants[11] = ambient;

    // setup the VSConstants (Directional light)
    *(D3DXCOLOR*  )&g_VSConstants[12] = light.Diffuse;
    *(D3DXVECTOR3*)&g_VSConstants[13] = light.Direction;

    // create a point light
    memset(&light, 0, sizeof(light));
    light.Type			= D3DLIGHT_POINT;
    light.Diffuse.r		= 1.0f;
    light.Diffuse.g		= 1.0f;
    light.Diffuse.b		= 1.0f;
    light.Ambient.r		= 0.15f;
    light.Ambient.g		= 0.15f;
    light.Ambient.b		= 0.15f;
    light.Direction.x	= -5.5f;
    light.Direction.y	= -0.5f;
    light.Direction.z	= -5.5f;
    light.Attenuation0	= 1.0f;
    light.Attenuation1 = 0;
    light.Attenuation2 = 1.0f/50;
    light.Range			= 2000.f;
    light.Position.x = 5.f;
    light.Position.y = 5.f;
    light.Position.z = 5.f;

    pd3dDevice->SetLight(1, &light);
    pd3dDevice->LightEnable(1, TRUE);

    *(D3DXCOLOR*  )&g_VSConstants[14] = light.Diffuse;
    *(D3DXVECTOR3*)&g_VSConstants[15] = light.Position;
    g_VSConstants[16].x = light.Attenuation0;
    g_VSConstants[16].y = light.Attenuation1;
    g_VSConstants[16].z = light.Attenuation2;
    g_VSConstants[16].w = light.Range;

    // reset the timer
    g_fLastAnimTime = DXUTGetGlobalTimer()->GetTime();

    // Adjust the dialog parameters.
    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width-170, pBackBufferSurfaceDesc->Height-270 );
    g_SampleUI.SetSize( 170, 220 );

     // create the shadow map
    V_RETURN(g_ShadowMap.Init( pd3dDevice ));
   
    // set world debug
    g_World.setDebug( g_bRenderDebugText );
   
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not
// intended to contain actual rendering calls, which should instead be placed in the
// OnFrameRender callback.
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    g_pScene->Update(fTime);

    g_pCamera->FrameMove(fElapsedTime);
    
    g_World.Update();
    g_World.Animate(fElapsedTime, fTime, g_pCamera->GetEyePt());

    // animate the character
    g_pPlayer->Animate( fElapsedTime,fTime - g_fLastAnimTime );

    g_fLastAnimTime = fTime;
    for(int i=0; i < 4; ++i)
    {
    //g_Mesh[i].determineSeekPosition(g_pPlayer->GetPosition(), &vecCollQuad, &g_DebugLine);
    //g_Mesh[i].Animate(fElapsedTime);
    }
    //collision detection
    runCollisionDetection((CollPerson *)g_pPlayer,0.15f,g_pPlayer->GetPosition(), g_pPlayer->GetFacing());
    /*D3DXVECTOR3 pos = g_Mesh.getPosition();
    pos.x += cos(g_Mesh.m_facing)*.07f;
    pos.z += sin(g_Mesh.m_facing)*.07f;
    runCollisionDetection((CollPerson *)&g_Mesh, 0.1f, pos, g_Mesh.getFacing());
    pos = g_Mesh.getPosition();
    pos.x -= cos(g_Mesh.m_facing)*.07f;
    pos.z -= sin(g_Mesh.m_facing)*.07f;
    runCollisionDetection((CollPerson *)&g_Mesh, 0.1f, pos, g_Mesh.getFacing());
    */
    //runPlayerNpcCollDetection((CollPerson *)g_pPlayer,(CollPerson *)&g_Mesh[0],.1f, g_pPlayer->GetPosition(), g_Mesh[0].getPosition(), g_pPlayer->GetFacing(), g_Mesh[0].getFacing());
    

    // collision detection for player against projectiles
    runPlayerProjCollDetection( g_pPlayer->GetPosition(), g_pPlayer->GetFacing());
    // run player health pack detection
    runPlayerHpCollDetection(g_pPlayer->GetPosition());
    // collision detection for player proj against NPC's
    runProjNpcCollDetection( );

    // collision detection for projectiles against walls
    runProjWallCollDetection();

    g_skybox.SetPosition(*g_pCamera->GetEyePt());

    g_MiniMap.SetPositionAndFacing( g_pPlayer->GetPosition(), g_pPlayer->GetMFacing()); 

    // update the location of the point light
    D3DXVECTOR3 pos = g_VSConstants[15];

    pos.x = (float)cos(fTime*2)*2.5f + 12.5f;
    pos.z = (float)sin(fTime*2)*2.5f + 12.5f;
    *(D3DXVECTOR3*)&g_VSConstants[15] = pos;
}


void RenderScene( IDirect3DDevice9* pd3dDevice, bool bRenderShadow, float fElapsedTime, const D3DXMATRIX* pmView,
                  const D3DXMATRIX* pmProj)
{

       // If the settings dialog is being shown, then
    // render it instead of rendering the app's scene

    if( g_SettingsDlg.IsActive() && !bRenderShadow )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

    HRESULT hr;

    pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        0x000000ff, 1.0f, 0L );

    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // set up the camera
        //D3DXMATRIXA16 mx, mxView, mxProj;
        D3DXVECTOR3 vEye;
        D3DXVECTOR3 vLightDir;

        // Light direction is same as camera front (reversed)
        // TODO - this needs to be fixed
        //vLightDir = -(*((CDebugCamera *)g_pCamera)->GetWorldAhead());

        
        V( pd3dDevice->SetTransform( D3DTS_VIEW,  pmView ) );
        V( pd3dDevice->SetTransform( D3DTS_PROJECTION,  pmProj ) );

        vEye = *g_pCamera->GetEyePt();

        // save the eye position
        *(D3DXVECTOR3*)&g_VSConstants[10] = vEye;
        
        pd3dDevice->SetVertexShaderConstantF(0, (float const*)g_VSConstants, 18);

        // Render the scene graph
        {
            D3DXMATRIX	matIdentity;		// identity matrix
            D3DXMatrixIdentity( &matIdentity );
         
            if( !bRenderShadow )
                g_skybox.Render( pd3dDevice, matIdentity );

            g_pScene->Render(pd3dDevice, matIdentity,bRenderShadow);

            g_World.AdvanceTimeAndRender(pd3dDevice,&matIdentity, fElapsedTime, &vEye, bRenderShadow);
        }

        // set view-proj
        D3DXMATRIXA16 mx;
        D3DXMatrixMultiply( &mx, pmView, pmProj );
      

        // draw the player
        if( bRenderShadow )
        {
            g_pPlayer->AdvanceTime(fElapsedTime/g_fSpeed, &vEye);
        }
        g_pPlayer->Draw( &mx, bRenderShadow );

        
        // draw the minimap
        if( !bRenderShadow )
            g_MiniMap.Render(pd3dDevice,mx);

        // Output text information - might want to move this if statement
        if( g_bRenderDebugText && !bRenderShadow  )
        {
            RenderText();
        }

        if( !bRenderShadow )
            V( g_HUD.OnRender( fElapsedTime ) );

        pd3dDevice->EndScene();
    }
}
//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the
// rendering calls for the scene, and it will also be called if the window needs to be
// repainted. After this function has returned, DXUT will call
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
 
    D3DXMATRIXA16 mx, mxView, mxProj;

    // this needs to be set the view and projection of the light
    D3DXMatrixIdentity( &mxView );
    D3DXMatrixIdentity( &mxProj );

    // render the shadow map
    g_ShadowMap.SetRT( pd3dDevice );
    RenderScene( pd3dDevice, true, fElapsedTime, &g_ShadowMap.m_mShadowView, &g_ShadowMap.m_mShadowProj );
    g_ShadowMap.UnSetRT( pd3dDevice );

    // get the view and projection
    // set static transforms
    mxView = *g_pCamera->GetViewMatrix();
    mxProj = *g_pCamera->GetProjMatrix();
    
    // render the scene
    RenderScene(pd3dDevice, false, fElapsedTime, &mxView, &mxProj);
}



//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work however the
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );
    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 135, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 10.0f, 10.0f, 10.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats(1) );
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );

    // Output debug camera info
    txtHelper.SetInsertionPos(5, 135);
    txtHelper.DrawFormattedTextLine(L"\nEye = (%.2f,%.2f,%.2f)", g_pCamera->GetEyePt()->x, g_pCamera->GetEyePt()->y, g_pCamera->GetEyePt()->z);
    txtHelper.DrawFormattedTextLine(L"\nLookAt = (%.2f,%.2f,%.2f)", g_pCamera->GetLookAtPt()->x, g_pCamera->GetLookAtPt()->y, g_pCamera->GetLookAtPt()->z);

    // output the player location
    txtHelper.DrawFormattedTextLine(L"\nPlayer = (%.2f,%.2f,%.2f)", g_pPlayer->GetPosition().x, g_pPlayer->GetPosition().y, g_pPlayer->GetPosition().z);
    
    // render any world debug info
    g_World.renderText(&txtHelper);

    // Dump out the FPS and device stats
    txtHelper.SetInsertionPos( 5, 150 );
    //txtHelper.DrawFormattedTextLine( L"  Time: %2.3f", DXUTGetGlobalTimer()->GetTime() );
    //txtHelper.DrawFormattedTextLine( L"  Number of models: %d", g_v_pCharacters.size() );

    //txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    txtHelper.SetInsertionPos( 5, 70 );

    // display the states of the stateMachine objects
    dbCompositionList list;
	g_database.ComposeList( list, OBJECT_Ignore_Type );
	int starttext = 250;
	int count = 0;
	dbCompositionList::iterator i;
	for( i=list.begin(); i!=list.end(); ++i )
	{
		StateMachine* pStateMachine = (*i)->GetStateMachineManager()->GetStateMachine(STATE_MACHINE_QUEUE_0);
		if( pStateMachine )
		{
            D3DXVECTOR3 pos = D3DXVECTOR3(0,0,0);
			char* name = (*i)->GetName();

			char* statename = pStateMachine->GetCurrentStateNameString();
			char* substatename = pStateMachine->GetCurrentSubstateNameString();
			TCHAR* unicode_name = new TCHAR[strlen(name)+1];
			TCHAR* unicode_statename = new TCHAR[strlen(statename)+1];
			TCHAR* unicode_substatename = new TCHAR[strlen(substatename)+1];
            mbstowcs(unicode_name, name, strlen(name)+1);
			mbstowcs(unicode_statename, statename, strlen(statename)+1);
			mbstowcs(unicode_substatename, substatename, strlen(substatename)+1);
            GameObject *go = g_database.FindByName( name );
            if( go && go->getMovement() != NULL )
            {
                pos = go->getMovement()->getPosition();
            }
            if( substatename[0] != 0 )
			{
				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 4, starttext-1+(12*count) );
                txtHelper.DrawFormattedTextLine( L"%s: H - %d (%.2f,%.2f,%.2f)  %s, %s", unicode_name, go->GetHealth(), pos.x, pos.y, pos.z, unicode_statename, unicode_substatename );
				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 5, starttext+(12*count++) );
				// TODO FIX - causes a crash
                //txtHelper.DrawFormattedTextLine( L"%s: (.2f,%.2f,%.2f)  %s, %s", unicode_name, pos.x, pos.y, pos.z, unicode_statename, unicode_substatename );
			}
			else
			{
				//txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 4, starttext-1+(12*count) );
				txtHelper.DrawFormattedTextLine( L"%s: H - %d (%.2f,%.2f,%.2f)  %s", unicode_name, go->GetHealth(), pos.x, pos.y, pos.z, unicode_statename );
				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 5, starttext+(12*count++) );
				//txtHelper.DrawFormattedTextLine( L"%s: H - %d (%.2f,%.2f,%.2f)  %s", unicode_name, go->GetHealth(), pos.x, pos.y, pos.z, unicode_statename );
			}
            
			delete unicode_name;
			delete unicode_statename;
			delete unicode_substatename;
		}
	}
/*
    vector <String> v_sReport;
    g_pPlayer->Report( v_sReport );
    txtHelper.SetInsertionPos( 5, pd3dsdBackBuffer->Height - 115 );
    for(int  i = 0; i < 6; ++i )
    {
        txtHelper.DrawTextLine( v_sReport[i].c_str() );
    }
    txtHelper.DrawTextLine( v_sReport[16].c_str() );

    txtHelper.SetInsertionPos( 210, pd3dsdBackBuffer->Height - 85 );
    for( int i = 6; i < 11; ++i )
    {
        txtHelper.DrawTextLine( v_sReport[i].c_str() );
    }

    txtHelper.SetInsertionPos( 370, pd3dsdBackBuffer->Height - 85 );
    for( int i = 11; i < 16; ++i )
    {
        txtHelper.DrawTextLine( v_sReport[i].c_str() );
    }
    if( g_bShowHelp )
    {
    }
*/
    txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows
// messages to the application through this callback function. If the application sets
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
    // Always allow dialog resource manager calls to handle global messages
    // so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to camera class for camera movement if the
    // global camera if active
    //if( -1 == g_dwFollow )
    if(g_pCamera != NULL )
    g_pCamera->HandleMessages( hWnd, uMsg, wParam, lParam );

    g_pPlayer->HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// As a convenience, DXUT inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
        case VK_F1:
            g_bShowHelp = !g_bShowHelp;
            break;
        // turn debug text on and off
        case VK_F4:
            g_bRenderDebugText = !g_bRenderDebugText;
            g_World.setDebug( g_bRenderDebugText );
            break;
        case VK_F5:
            // switch between point and directional light
            if( g_bDirectionalLight == false )
            {
                *(D3DXCOLOR*  )&g_VSConstants[12] = D3DXCOLOR(1.f,1.f,1.f,1.f);
                g_bDirectionalLight = true;
            }
            else
            {   
                *(D3DXCOLOR*  )&g_VSConstants[12] = D3DXCOLOR((DWORD)0);
                g_bDirectionalLight = false;
            }
            break;

        case VK_F7: // enable and disable the background music
            if(g_pBackground->IsSoundPlaying())
                g_pBackground->Stop();
            else
                g_pBackground->Play(0, DSBPLAY_LOOPING);

            break;
        case VK_F8:
            // A star debug
            if( g_iRenderDebugAStar != -1 ) // disable current
            {
                char name[10];
                sprintf( name, "NPC%d",g_iRenderDebugAStar);
                GameObject *go = g_database.FindByName(name );
                if( go  )
                {
                    // send message to disable
                    g_database.SendMsgFromSystem( go, MSG_ASTAR_DEBUG, MSG_Data(false));
                }
            }
            // increment
            if( g_iRenderDebugAStar == 3 )
                g_iRenderDebugAStar = -1;
            else
            {
                ++g_iRenderDebugAStar;
                char name[10];
                sprintf( name, "NPC%d",g_iRenderDebugAStar);
                GameObject *go = g_database.FindByName(name );
                if( go  )
                {
                    // send message to disable
                    g_database.SendMsgFromSystem( go, MSG_ASTAR_DEBUG, MSG_Data(true));
                }
            }

            break;
        case 'L':  // display grid 'layers'
            g_World.gridDebug();  // call this repeatedly will cycle through the grids
            break;

            // it might make sense to move this into the player
        case VK_SPACE:
            {
            GameObject *go = g_database.FindByName("PLAYPROJ0");
            if( go )
            {
                g_database.SendMsgFromSystem( go, MSG_ProjectileLaunch, MSG_Data(g_pPlayer->GetPosition()));
            }
            }
            break;

        case 'R':
            g_database.SendMsgFromSystem(MSG_Reset, MSG_Data());
            break;

        case 'C':
            // start the game
            g_database.SendMsgFromSystem(g_database.GetIDByName("Gameflow"), MSG_GameStart, MSG_Data());
            break;
        /*case 'S':
            g_fSpeed = 2.f;
            break;
        case 'P': // pause the game
            DXUTPause(!DXUTIsTimePaused(),0);
            break;*/
        }
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{


    switch( nControlID )
    {
    case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
    case IDC_TOGGLEREF:        DXUTToggleREF(); break;
    case IDC_CHANGEDEVICE:     g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;

    case IDC_NEXTVIEW:
        // go to the next debug camera
        g_pCamera = g_Cameras.nextCam();
        break;

    case IDC_PREVVIEW:
        g_pCamera = g_Cameras.prevCam();
        break;

    case IDC_RESETCAMERA:
        break;

    case IDC_RESETTIME:
        {
            DXUTGetGlobalTimer()->Reset();
            g_fLastAnimTime = DXUTGetGlobalTimer()->GetTime();
            break;
        }
    }
}


void cleanup()
{
    
    SAFE_DELETE( g_pBackground);

    g_World.Release();
    SAFE_RELEASE( g_pTextSprite );
    g_skybox.Release();

    g_MiniMap.Release();

    g_ShadowMap.Release();
    SAFE_DELETE( g_pScene );

    SAFE_DELETE( g_pPlayer);

}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9LostDevice();
    g_SettingsDlg.OnD3D9LostDevice();
    if( g_pFont )
        g_pFont->OnLostDevice();
//SAFE_RELEASE(g_pFont);
    
  
    g_pPlayer->InvalidateDeviceObjects();
    
    cleanup();
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// been destroyed, which generally happens as a result of application termination or
// windowed/full screen toggles. Resources created in the OnCreateDevice callback
// should be released here, which generally includes all D3DPOOL_MANAGED resources.
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9DestroyDevice();
    g_SettingsDlg.OnD3D9DestroyDevice();
    
    SAFE_RELEASE(g_pFont);
   
    cleanup();
}


