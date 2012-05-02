//-----------------------------------------------------------------------------
// File: Tiny.cpp
//
// Desc: Defines the character class, CTiny, for the MultipleAnimation sample.
//       The class does some basic things to make the character aware of
//       its surroundings, as well as implements all actions and movements.
//       CTiny shows a full-featured example of this.  These
//       classes use the MultiAnimation class library to control the
//       animations.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUT/SDKmisc.h"
#pragma warning(disable: 4995)
#include "MultiAnimation.h"
#include "Tiny.h"
#pragma warning(default: 4995)

using namespace std;

//-----------------------------------------------------------------------------
// Name: CTiny::CTiny
// Desc: Constructor for CTiny
//-----------------------------------------------------------------------------
CTiny::CTiny() : m_pMA( NULL ),
m_dwMultiAnimIdx( 0 ),
m_pAI( NULL ),

m_dTimePrev( 0.0 ),
m_dTimeCurrent( 0.0 ),
m_bUserControl( false ),
m_dwCurrentTrack( 0 ),

m_fSpeed( 0.f ),
m_fSpeedTurn( 0.f ),
m_fPersonalRadius( 0.f ),

m_fSpeedWalk( 1.f / 4.f ),
m_fSpeedJog( 1.f / 2.f ),

m_bIdle( false ),
m_bWaiting( false ),
m_dTimeIdling( 0.0 )
{
    D3DXMatrixIdentity( &m_mxOrientation );

    m_fSpeedTurn = 1.3f;
    m_fPersonalRadius = .035f;

    m_szASName[0] = '\0';

    ZeroMemory( m_aKeys, sizeof(BYTE)*PLAYER_MAX_KEYS );
}

//-----------------------------------------------------------------------------
// Name: CTiny::CTiny( IDirect3DDevice9* pd3dDevice )
// Desc: Constructor for CTiny
//-----------------------------------------------------------------------------
CTiny::CTiny(IDirect3DDevice9* pd3dDevice) : m_pMA( NULL ),
m_dwMultiAnimIdx( 0 ),
m_pAI( NULL ),

m_dTimePrev( 0.0 ),
m_dTimeCurrent( 0.0 ),
m_bUserControl( false ),
m_dwCurrentTrack( 0 ),

m_fSpeed( 0.f ),
m_fSpeedTurn( 0.f ),
m_fPersonalRadius( 0.f ),

m_fSpeedWalk( 1.f / 2.f ),
m_fSpeedJog( 1.f / 1.f ),

m_bIdle( true ),
m_bWaiting( false ),
m_dTimeIdling( 0.0 ),
m_pSound(NULL)
{
    WCHAR sXFile[MAX_PATH];
    WCHAR str[MAX_PATH];
    D3DXMatrixIdentity( &m_mxOrientation );

    m_fSpeedTurn = 1.3f;
    m_fPersonalRadius = .035f;

    m_szASName[0] = '\0';

    ZeroMemory( m_aKeys, sizeof(BYTE)*PLAYER_MAX_KEYS );

    // setup the animation libarary
    m_pMA = new CMultiAnim;

    // Read the D3DX effect file
    // TODO - HANDLE ERRORS IF THESE FILES DO NOT EXIST
    ( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"MultiAnimation.fx" ) );
    ( DXUTFindDXSDKMediaFileCch( sXFile, MAX_PATH, L"tiny_4anim.x" ) );

    CMultiAnimAllocateHierarchy AH;
    AH.SetMA( m_pMA );

    ( m_pMA->Setup( pd3dDevice, sXFile, str, &AH ) );

    // Select the technique that fits the shader version.
    // We could have used ValidateTechnique()/GetNextValidTechnique() to find the
    // best one, but this is convenient for our purposes.
    m_pMA->SetTechnique( "Skinning20" );


}


//-----------------------------------------------------------------------------
// Name: CTiny::~CTiny
// Desc: Destructor for CTiny
//-----------------------------------------------------------------------------
CTiny::~CTiny()
{
    Cleanup();
}




//-----------------------------------------------------------------------------
// Name: CTiny::Setup
// Desc: Initializes the class and readies it for animation
//-----------------------------------------------------------------------------
HRESULT CTiny::Setup(double dTimeCurrent, D3DXVECTOR3 pInitialPosition, CSoundManager *p_sound )
{
    HRESULT hr;
    WCHAR str[MAX_PATH];

    // set the current and prev time
    m_dTimeCurrent = m_dTimePrev = dTimeCurrent;

    hr = m_pMA->CreateNewInstance( &m_dwMultiAnimIdx );
    if( FAILED( hr ) )
        return E_OUTOFMEMORY;

    m_pAI = m_pMA->GetInstance( m_dwMultiAnimIdx );

    // set initial position
    m_vPos = pInitialPosition;

    m_fFacing = 0.0f;

    // set up anim indices
    m_dwAnimIdxLoiter = GetAnimIndex( "Loiter" );
    m_dwAnimIdxWalk = GetAnimIndex( "Walk" );
    m_dwAnimIdxJog = GetAnimIndex( "Jog" );
    if( m_dwAnimIdxLoiter == ANIMINDEX_FAIL ||
        m_dwAnimIdxWalk == ANIMINDEX_FAIL ||
        m_dwAnimIdxJog == ANIMINDEX_FAIL )
        return E_FAIL;

    // compute reorientation matrix based on default orientation and bounding radius
    D3DXMATRIX mx;
    float fScale = 1.f / m_pMA->GetBoundingRadius() / 2.5f;
    D3DXMatrixScaling( &mx, fScale, fScale, fScale );
    m_mxOrientation = mx;
    D3DXMatrixRotationX( &mx, -D3DX_PI / 2.0f );
    D3DXMatrixMultiply( &m_mxOrientation, &m_mxOrientation, &mx );
    D3DXMatrixRotationY( &mx, D3DX_PI / 2.0f );
    D3DXMatrixMultiply( &m_mxOrientation, &m_mxOrientation, &mx );

    // set track to idle
    SetIdleKey(false);

    LPD3DXANIMATIONCONTROLLER pAC;
    m_pAI->GetAnimController( &pAC );
    pAC->AdvanceTime( m_dTimeCurrent, NULL );
    pAC->Release();

    // Use D3DX to create a texture from a file based image
    DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"metalplate.wav" );
    // load the sound
    hr = p_sound->Create(&m_pSound,str,DSBCAPS_CTRLVOLUME);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CTiny::Cleanup()
// Desc: Performs cleanup tasks for CTiny
//-----------------------------------------------------------------------------
void CTiny::Cleanup()
{
    SAFE_DELETE(m_pMA);
    SAFE_DELETE(m_pSound);
}



//-----------------------------------------------------------------------------
// Name: CTiny::GetPosition()
// Desc: Returns the position of this instance.
//-----------------------------------------------------------------------------
D3DXVECTOR3 CTiny::GetPosition( void )
{
    return m_vPos;
}




//-----------------------------------------------------------------------------
// Name: CTiny::GetFacing()
// Desc: Returns a unit vector representing the direction this CTiny
//       instance is facing.
//-----------------------------------------------------------------------------
D3DXVECTOR3 CTiny::GetFacing( void )
{
    D3DXMATRIX m;
    D3DXVECTOR3 pV( 1.0f, 0.0f, 0.0f );
    D3DXVec3TransformCoord( &pV, &pV, D3DXMatrixRotationY( &m, -m_fFacing ) );
    return pV;
}


float CTiny::GetMFacing(void)
{
    return m_fFacing;
}

//-----------------------------------------------------------------------------
// Name: CTiny::Animate()
// Desc: Advances the local time by dTimeDelta. Determine an action for Tiny,
//       then update the animation controller's tracks to reflect the action.
//-----------------------------------------------------------------------------
void CTiny::Animate( float fElapsedTime, double dTimeDelta )
{

    // determine which keys are down
    GetPlayerInput();
    
    m_vvel = m_vacc;

    // position delta
   D3DXVECTOR3 vPosDelta = m_vvel * fElapsedTime;

    // add rotation delta to facing
    m_fFacing -= m_rot.x;

    // loop the loiter animation back on itself to avoid the end-to-end jerk
    SmoothLoiter();

    D3DXMATRIX mxWorld, mx;

    // rotate the player
    D3DXMatrixRotationY( &mxWorld, -m_fFacing );
    
    // determine the postion of the player based on the rotation
    D3DXVECTOR3 vPosDeltaWorld;
    D3DXVec3TransformCoord( &vPosDeltaWorld, &vPosDelta, &mxWorld );

    if( getXColl() == true )
    {   
        vPosDeltaWorld.x = 0;
        setXColl(false);
    }
    if( getZColl() == true )
    {
        vPosDeltaWorld.z = 0;
        setZColl(false);
    }
    if( getCollPerson() == true )
    {
        // not sure if this is the right place to do this
        m_pSound->Play();
        setCollPerson(false);
    }

    m_vPos += vPosDeltaWorld;

    // translate player
    // hack to make shadows look better (adding .1f to the y pos - gets removed in the shader)
    D3DXMatrixTranslation( &mx, m_vPos.x, m_vPos.y + .1f, m_vPos.z );
    D3DXMatrixMultiply( &mxWorld, &mxWorld, &mx );
    D3DXMatrixMultiply( &mxWorld, &m_mxOrientation, &mxWorld );
    m_pAI->SetWorldTransform( &mxWorld );
    
}




//-----------------------------------------------------------------------------
// Name: CTiny::ResetTime()
// Desc: Resets the local time for this CTiny instance.
//-----------------------------------------------------------------------------
HRESULT CTiny::ResetTime()
{
    m_dTimeCurrent = m_dTimePrev = 0.0;
    return m_pAI->ResetTime();
}




//-----------------------------------------------------------------------------
// Name: CTiny::AdvanceTime()
// Desc: Advances the local animation time by dTimeDelta, and call
//       CAnimInstance to set up its frames to reflect the time advancement.
//-----------------------------------------------------------------------------
HRESULT CTiny::AdvanceTime( double dTimeDelta, D3DXVECTOR3* pvEye )
{

    m_dTimePrev = m_dTimeCurrent;
    m_dTimeCurrent += dTimeDelta;
    return m_pAI->AdvanceTime( dTimeDelta, NULL );
}




//-----------------------------------------------------------------------------
// Name: CTiny::Draw()
// Desc: Renders this CTiny instace using the current animation frames.
//-----------------------------------------------------------------------------
HRESULT CTiny::Draw(D3DXMATRIXA16 *mxViewProj, bool bRenderShadow)
{
    LPDIRECT3DDEVICE9 pDevice = this->m_pMA->GetDevice();
    D3DLIGHT9 light;
    pDevice->GetLight(0, &light);
    // update the ViewProjection matric for the animation
    ID3DXEffect* pMAEffect = m_pMA->GetEffect();
    if( pMAEffect )
    {
        pMAEffect->SetMatrix( "g_mViewProj", mxViewProj );
        pMAEffect->SetVector( "lhtDir", (D3DXVECTOR4 *)&(light.Direction));
    }

    SAFE_RELEASE( pMAEffect );
    SAFE_RELEASE( pDevice);
   


    if(bRenderShadow)
    {
        m_pMA->SetTechnique( "ShadowPass" );
    }
    else
    {

        m_pMA->SetTechnique( "Skinning20" );
            // translate player

       
    }

    return m_pAI->Draw();
}




//-----------------------------------------------------------------------------
// Name: CTiny::Report()
// Desc: Add to the vector of strings, v_sReport, with useful information
//       about this instance of CTiny.
//-----------------------------------------------------------------------------
void CTiny::Report( vector <String>& v_sReport )
{
    WCHAR s[ 256 ];

    try
    {
        swprintf_s( s, 256, L"Pos: %f, %f", m_vPos.x, m_vPos.z );
        v_sReport.push_back( String( s ) );
        swprintf_s( s, 256, L"Facing: %f", m_fFacing );
        v_sReport.push_back( String( s ) );
        swprintf_s( s, 256, L"Local time: %f", m_dTimeCurrent );
        v_sReport.push_back( String( s ) );
        swprintf_s( s, 256, L"Pos Target: %f, %f", m_vPosTarget.x, m_vPosTarget.z );
        v_sReport.push_back( String( s ) );
        swprintf_s( s, 256, L"Facing Target: %f", m_fFacingTarget );
        v_sReport.push_back( String( s ) );
        swprintf_s( s, 256, L"Status: %s", m_bIdle ? L"Idle" : ( m_bWaiting ? L"Waiting" : L"Moving" ) );
        v_sReport.push_back( String( s ) );

        // report track data
        LPD3DXANIMATIONCONTROLLER pAC;
        LPD3DXANIMATIONSET pAS;
        D3DXTRACK_DESC td;
        m_pAI->GetAnimController( &pAC );

        pAC->GetTrackAnimationSet( 0, &pAS );
        WCHAR wstr[256];
        MultiByteToWideChar( CP_ACP, 0, pAS->GetName(), -1, wstr, 256 );
        swprintf_s( s, 256, L"Track 0: %s%s", wstr, m_dwCurrentTrack == 0 ? L" (current)" : L"" );
        v_sReport.push_back( String( s ) );
        pAS->Release();

        pAC->GetTrackDesc( 0, &td );
        swprintf_s( s, 256, L"  Weight: %f", td.Weight );
        v_sReport.push_back( String( s ) );
        swprintf_s( s, 256, L"  Speed: %f", td.Speed );
        v_sReport.push_back( String( s ) );
        swprintf_s( s, 256, L"  Position: %f", td.Position );
        v_sReport.push_back( String( s ) );
        swprintf_s( s, 256, L"  Enable: %s", td.Enable ? L"true" : L"false" );
        v_sReport.push_back( String( s ) );

        pAC->GetTrackAnimationSet( 1, &pAS );
        if( pAS )
        {
            MultiByteToWideChar( CP_ACP, 0, pAS->GetName(), -1, wstr, 256 );
            pAS->Release();
        }
        else
        {
            swprintf_s( wstr, 256, L"n/a" );
        }
        swprintf_s( s, 256, L"Track 1: %s%s", wstr, m_dwCurrentTrack == 1 ? L" (current)" : L"" );
        v_sReport.push_back( String( s ) );

        pAC->GetTrackDesc( 1, &td );
        swprintf_s( s, 256, L"  Weight: %f", td.Weight );
        v_sReport.push_back( String( s ) );
        swprintf_s( s, 256, L"  Speed: %f", td.Speed );
        v_sReport.push_back( String( s ) );
        swprintf_s( s, 256, L"  Position: %f", td.Position );
        v_sReport.push_back( String( s ) );
        swprintf_s( s, 256, L"  Enable: %s", td.Enable ? L"true" : L"false" );
        v_sReport.push_back( String( s ) );

        if( m_bUserControl )
        {
            swprintf_s( s, 256, L"Control: USER" );
            v_sReport.push_back( String( s ) );
        }
        else
        {
            swprintf_s( s, 256, L"Control: AUTO" );
            v_sReport.push_back( String( s ) );
        }

        pAC->Release();
    }
    catch( ... )
    {
    }
}


//-----------------------------------------------------------------------------
// Name: CTiny::GetAnimIndex()
// Desc: Returns the index of an animation set within this animation instance's
//       animation controller given an animation set name.
//-----------------------------------------------------------------------------
DWORD CTiny::GetAnimIndex( char sString[] )
{
    HRESULT hr;
    LPD3DXANIMATIONCONTROLLER pAC;
    LPD3DXANIMATIONSET pAS;
    DWORD dwRet = ANIMINDEX_FAIL;

    m_pAI->GetAnimController( &pAC );

    for( DWORD i = 0; i < pAC->GetNumAnimationSets(); ++ i )
    {
        hr = pAC->GetAnimationSet( i, &pAS );
        if( FAILED( hr ) )
            continue;

        if( pAS->GetName() &&
            !strncmp( pAS->GetName(), sString, min( strlen( pAS->GetName() ), strlen( sString ) ) ) )
        {
            dwRet = i;
            pAS->Release();
            break;
        }

        pAS->Release();
    }

    pAC->Release();

    return dwRet;
}

//-----------------------------------------------------------------------------
// Name: CTiny::SmoothLoiter()
// Desc: If Tiny is loitering, check if we have reached the end of animation.
//       If so, set up a new track to play Loiter animation from the start and
//       smoothly transition to the track, so that Tiny can loiter more.
//-----------------------------------------------------------------------------
void CTiny::SmoothLoiter()
{
    LPD3DXANIMATIONCONTROLLER pAC;
    LPD3DXANIMATIONSET pASTrack, pASLoiter;
    m_pAI->GetAnimController( &pAC );

    // check if we're loitering
    pAC->GetTrackAnimationSet( m_dwCurrentTrack, &pASTrack );
    pAC->GetAnimationSet( m_dwAnimIdxLoiter, &pASLoiter );
    if( pASTrack && pASTrack == pASLoiter )
    {
        D3DXTRACK_DESC td;
        pAC->GetTrackDesc( m_dwCurrentTrack, &td );
        if( td.Position > pASTrack->GetPeriod() - IDLE_TRANSITION_TIME )  // come within the change delta of the end
            SetIdleKey( true );
    }

    SAFE_RELEASE( pASTrack );
    SAFE_RELEASE( pASLoiter );
    SAFE_RELEASE( pAC );
}




//-----------------------------------------------------------------------------
// Name: CTiny::GetSpeedScale()
// Desc: Returns the speed of the current track.
//-----------------------------------------------------------------------------
double CTiny::GetSpeedScale()
{
    LPD3DXANIMATIONCONTROLLER pAC;
    D3DXTRACK_DESC td;

    if( m_bIdle )
        return 1.0;
    else
    {
        m_pAI->GetAnimController( &pAC );
        pAC->GetTrackDesc( m_dwCurrentTrack, &td );
        pAC->Release();

        return td.Speed;
    }
}

//-----------------------------------------------------------------------------
// Name: CTiny::SetMoveKey()
// Desc: Initialize a new track in the animation controller for the movement
//       animation (run or walk), and set up the smooth transition from the idle
//       animation (current track) to it (new track).
//-----------------------------------------------------------------------------
void CTiny::SetMoveKey()
{
    DWORD dwNewTrack = ( m_dwCurrentTrack == 0 ? 1 : 0 );
    LPD3DXANIMATIONCONTROLLER pAC;
    LPD3DXANIMATIONSET pAS;
    m_pAI->GetAnimController( &pAC );

    if( m_fSpeed == m_fSpeedWalk || m_fSpeed == -m_fSpeedWalk )
        pAC->GetAnimationSet( m_dwAnimIdxWalk, &pAS );
    else
        pAC->GetAnimationSet( m_dwAnimIdxJog, &pAS );

    pAC->SetTrackAnimationSet( dwNewTrack, pAS );
    pAS->Release();

    pAC->UnkeyAllTrackEvents( m_dwCurrentTrack );
    pAC->UnkeyAllTrackEvents( dwNewTrack );

    pAC->KeyTrackEnable( m_dwCurrentTrack, FALSE, m_dTimeCurrent + MOVE_TRANSITION_TIME );
    pAC->KeyTrackSpeed( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->KeyTrackWeight( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->SetTrackEnable( dwNewTrack, TRUE );
    pAC->KeyTrackSpeed( dwNewTrack, 1.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->KeyTrackWeight( dwNewTrack, 1.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );

    m_dwCurrentTrack = dwNewTrack;

    pAC->Release();
}




//-----------------------------------------------------------------------------
// Name: CTiny::SetIdleKey()
// Desc: Initialize a new track in the animation controller for the idle
//       (loiter ) animation, and set up the smooth transition from the
//       movement animation (current track) to it (new track).
//
//       bResetPosition controls whether we start the Loiter animation from
//       its beginning or current position.
//-----------------------------------------------------------------------------
void CTiny::SetIdleKey( bool bResetPosition )
{
    DWORD dwNewTrack = ( m_dwCurrentTrack == 0 ? 1 : 0 );
    LPD3DXANIMATIONCONTROLLER pAC;
    LPD3DXANIMATIONSET pAS;
    m_pAI->GetAnimController( &pAC );

    pAC->GetAnimationSet( m_dwAnimIdxLoiter, &pAS );
    pAC->SetTrackAnimationSet( dwNewTrack, pAS );
    pAS->Release();

    pAC->UnkeyAllTrackEvents( m_dwCurrentTrack );
    pAC->UnkeyAllTrackEvents( dwNewTrack );

    pAC->KeyTrackEnable( m_dwCurrentTrack, FALSE, m_dTimeCurrent + IDLE_TRANSITION_TIME );
    pAC->KeyTrackSpeed( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->KeyTrackWeight( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->SetTrackEnable( dwNewTrack, TRUE );
    pAC->KeyTrackSpeed( dwNewTrack, 1.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->KeyTrackWeight( dwNewTrack, 1.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    if( bResetPosition )
        pAC->SetTrackPosition( dwNewTrack, 0.0 );

    m_dwCurrentTrack = dwNewTrack;

    pAC->Release();
}




//-----------------------------------------------------------------------------
// Name: CTiny::RestoreDeviceObjects()
// Desc: Reinitialize necessary objects
//-----------------------------------------------------------------------------
HRESULT CTiny::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
    // Compress the animation sets in the new animation controller
    //    SetupCallbacksAndCompression();

    LPD3DXANIMATIONCONTROLLER pAC;
    m_pAI->GetAnimController( &pAC );
    pAC->ResetTime();
    pAC->AdvanceTime( m_dTimeCurrent, NULL );

    // Initialize current track
    if( m_szASName[0] != '\0' )
    {
        DWORD dwActiveSet = GetAnimIndex( m_szASName );
        LPD3DXANIMATIONSET pAS = NULL;
        pAC->GetAnimationSet( dwActiveSet, &pAS );
        pAC->SetTrackAnimationSet( m_dwCurrentTrack, pAS );
        SAFE_RELEASE( pAS );
    }

    pAC->SetTrackEnable( m_dwCurrentTrack, TRUE );
    pAC->SetTrackWeight( m_dwCurrentTrack, 1.0f );
    pAC->SetTrackSpeed( m_dwCurrentTrack, 1.0f );

    SAFE_RELEASE( pAC );

    // Call animate to initialize the tracks.
    Animate( 0.0, 0.0 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CTiny::RestoreDeviceObjects()
// Desc: Free D3D objects so that the device can be reset.
//-----------------------------------------------------------------------------
HRESULT CTiny::InvalidateDeviceObjects()
{
    // Save the current track's animation set name
    // so we can reset it again in RestoreDeviceObjects later.
    LPD3DXANIMATIONCONTROLLER pAC = NULL;
    m_pAI->GetAnimController( &pAC );
    if( pAC )
    {
        LPD3DXANIMATIONSET pAS = NULL;
        pAC->GetTrackAnimationSet( m_dwCurrentTrack, &pAS );
        if( pAS )
        {
            if( pAS->GetName() )
                strcpy_s( m_szASName, 64, pAS->GetName() );
            SAFE_RELEASE( pAS );
        }
        SAFE_RELEASE( pAC );
    }

    CMultiAnimAllocateHierarchy AH;
    AH.SetMA( m_pMA );
    m_pMA->Cleanup( &AH );

    return S_OK;
}


// Call these from client and use Get*Matrix() to read new matrices
LRESULT CTiny::HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER( hWnd );
    UNREFERENCED_PARAMETER( lParam );

    switch( uMsg )
    {
    case WM_KEYDOWN:
        {
            // Map this key to a D3DUtil_CameraKeys enum and update the
            // state of m_aKeys[] by adding the KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK mask
            // only if the key is not down
            D3DUtil_PlayerKeys mappedKey = MapPlayerKey( (UINT)wParam );
            if( mappedKey != PLAYER_CAM_UNKNOWN && (DWORD)mappedKey < PLAYER_MAX_KEYS )
            {
                if( FALSE == IsKeyDown(m_aKeys[mappedKey]) )
                {
                    m_aKeys[ mappedKey ] = KEY_WAS_DOWN_MASK | KEY_IS_DOWN_MASK;
                  //  ++m_cKeysDown;
                }
            }
            break;
        }

    case WM_KEYUP:
        {
            // Map this key to a D3DUtil_CameraKeys enum and update the
            // state of m_aKeys[] by removing the KEY_IS_DOWN_MASK mask.
            D3DUtil_PlayerKeys mappedKey = MapPlayerKey( (UINT)wParam );
            if( mappedKey != PLAYER_CAM_UNKNOWN && (DWORD)mappedKey < PLAYER_MAX_KEYS )
            {
                m_aKeys[ mappedKey ] &= ~KEY_IS_DOWN_MASK;
            //    --m_cKeysDown;
            }
            break;
        }
    default:
        break;
    }

    return FALSE;
}

D3DUtil_PlayerKeys CTiny::MapPlayerKey( UINT nKey )
{
    // This could be upgraded to a method that's user-definable but for 
    // simplicity, we'll use a hardcoded mapping.
    switch( nKey )
    {
    case 'W':   return PLAYER_WALK_FORWARD;
    case 'D':   return PLAYER_ROTATE_RIGHT;
    case 'A':   return PLAYER_ROTATE_LEFT;
    case 'S':   return PLAYER_SLOW_STOP;
    case VK_SHIFT: return PLAYER_RUN_FORWARD;

    default:       break;
    }

    return PLAYER_CAM_UNKNOWN;
}

void CTiny::GetPlayerInput( void )
{
     m_vacc = D3DXVECTOR3(0,0,0);
     m_rot = D3DXVECTOR2(0,0);
    
     //m_bIdle = true;
     if( IsKeyDown( m_aKeys[PLAYER_WALK_FORWARD]) )
     {
         // check if shift is down
         if( IsKeyDown( m_aKeys[PLAYER_RUN_FORWARD]))
         {
             // set the speed to running
             m_vacc.x += m_fSpeedJog;
             if(m_fSpeed != m_fSpeedJog)
             {
                 m_fSpeed = m_fSpeedJog;
                 m_bIdle = false;
                 SetMoveKey();
             }
         }
         else
         {
             // set the speed to walking
             m_vacc.x += m_fSpeedWalk;
             if( m_fSpeed != m_fSpeedWalk)
             {
                 m_fSpeed = m_fSpeedWalk;
                 m_bIdle = false;
                 SetMoveKey();
             }
         }
     }
     else if( WasKeyDown( m_aKeys[PLAYER_WALK_FORWARD]) || WasKeyDown( m_aKeys[PLAYER_SLOW_STOP])  )
     {
         // set Idle to true
         if( m_bIdle != true )
         {
             m_bIdle = true;
             m_fSpeed = 0;  // set speed to 0 since we are no longer walking
             SetIdleKey(true);
         }
     }
     if( IsKeyDown( m_aKeys[PLAYER_SLOW_STOP] ) )
     {
         m_vacc.x -= m_fSpeedWalk;
         // set Idle to true
          if( m_fSpeed != -m_fSpeedWalk)
          {
                 m_fSpeed = -m_fSpeedWalk;
                 m_bIdle = false;
                 SetMoveKey();
           }
     }
     
     if( IsKeyDown( m_aKeys[PLAYER_ROTATE_RIGHT]) )
     {
         // multiply by rotation scaler
         m_rot.x += 1.0f * .04f;
     }
     if( IsKeyDown( m_aKeys[PLAYER_ROTATE_LEFT]) )
     {
         // multiply by rotation scaler
         m_rot.x -= 1.0f * .04f;
     }


}