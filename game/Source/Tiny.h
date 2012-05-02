//-----------------------------------------------------------------------------
// File: Tiny.h
//
// Desc: Header file for the CTiny class.  Its declaraction is found here.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------


#pragma once 

#include "DXUT\SDKsound.h"

#include "MultiAnimation.h"
#include "collision.h"

#define IDLE_TRANSITION_TIME 0.4f
#define MOVE_TRANSITION_TIME 0.25f


typedef std::basic_string <TCHAR>   String;


static enum D3DUtil_PlayerKeys
{
    PLAYER_ROTATE_LEFT = 0,
    PLAYER_ROTATE_RIGHT,
    PLAYER_WALK_FORWARD,
    PLAYER_RUN_FORWARD,
    PLAYER_MOVE_BACKWARD,
    PLAYER_SLOW_STOP,
    PLAYER_MAX_KEYS,
    PLAYER_CAM_UNKNOWN = 0xFF
};

#define ANIMINDEX_FAIL      0xffffffff

#define KEY_WAS_DOWN_MASK 0x80
#define KEY_IS_DOWN_MASK  0x01

//-----------------------------------------------------------------------------
// Name: class CTiny
// Desc: This is the character class. It handles character behaviors and the
//       the associated animations.
//-----------------------------------------------------------------------------
class CTiny : public CollPerson
{

protected:

    // -- data structuring
    CMultiAnim* m_pMA;               // pointer to mesh-type-specific object
    DWORD m_dwMultiAnimIdx;    // index identifying which CAnimInstance this object uses
    CAnimInstance* m_pAI;               // pointer to CAnimInstance specific to this object

    DWORD m_dwAnimIdxLoiter,   // Indexes of various animation sets
        m_dwAnimIdxWalk,
        m_dwAnimIdxJog;

    // operational status
    double m_dTimePrev;         // global time value before last update
    double m_dTimeCurrent;      // current global time value
    bool m_bUserControl;      // true == user is controling character with the keyboard
    DWORD m_dwCurrentTrack;    // current animation track for primary animation

    // character traits
    float m_fSpeed;            // character's movement speed -- in units/second
    float m_fSpeedTurn;        // character's turning speed -- in radians/second

    D3DXMATRIX m_mxOrientation;     // transform that gets the mesh into a common world space
    float m_fPersonalRadius;   // personal space radius -- things can't get closer than this
    // (note that no height information is given--not necessary for this sample)
    // character status
    
    float m_fFacing;           // current direction the character is facing -- in our sample, it's 2D only
    D3DXVECTOR3 m_vPosTarget;        // This indicates where we are moving to
    float m_fFacingTarget;     // The direction from Tiny's current position to the final destination
    float m_fSpeedWalk;
    float m_fSpeedJog;
    bool m_bIdle;             // set when Tiny is idling -- not turning toward a target
    bool m_bWaiting;          // set when Tiny is not idle, but cannot move
    double m_dTimeIdling;       // countdown - Tiny Idles til this goes < 0

    // controls
    float m_fheading;            // heading in the xz plane in degrees.  0 == (1,0,0), 90 -- (0,0,1)
    D3DXVECTOR3 m_vPos;         // current position in the map -- in our sample, y is always == 0
    D3DXVECTOR3 m_vvel;           // velocity of player in the world
    D3DXVECTOR3 m_vacc;           // accleration of player in the world
    D3DXVECTOR2 m_rot;                  // rotation  
    BYTE        m_aKeys[PLAYER_MAX_KEYS];  // State of input - KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK
   
    char                m_szASName[64];      // Current track's animation set name (for preserving across device reset)


    D3DUtil_PlayerKeys MapPlayerKey( UINT nKey );
    CSound *m_pSound;
   
public:

                        CTiny();
                        CTiny(IDirect3DDevice9* pd3dDevice);
    virtual             ~CTiny();
    virtual HRESULT     Setup( double dTimeCurrent, D3DXVECTOR3 pInitialPosition, CSoundManager *p_sound );
    virtual void        Cleanup();
 
    D3DXVECTOR3         GetPosition( void );
    D3DXVECTOR3         GetFacing( void );
    float               GetMFacing( void );
    virtual void        Animate( float fElapsedTime, double dTimeDelta );
    virtual HRESULT     ResetTime();
    virtual HRESULT     AdvanceTime( double dTimeDelta, D3DXVECTOR3* pvEye );
    virtual HRESULT     Draw(D3DXMATRIXA16 *mxViewProj, bool bRenderShadow);
    virtual void        Report( std::vector <String>& v_sReport );

    LRESULT             HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HRESULT             RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT             InvalidateDeviceObjects();

protected:      // ************ The following are not callable by the app -- internal stuff


    DWORD               GetAnimIndex( char sString[] );

    bool IsKeyDown( BYTE key ) const { return( (key & KEY_IS_DOWN_MASK) == KEY_IS_DOWN_MASK ); }
    bool WasKeyDown( BYTE key ) const { return( (key & KEY_WAS_DOWN_MASK) == KEY_WAS_DOWN_MASK ); }

    void                GetPlayerInput( void );

    void                SmoothLoiter();
    double              GetSpeedScale();
    void                SetMoveKey();
    void                SetIdleKey( bool bResetPosition );
};
