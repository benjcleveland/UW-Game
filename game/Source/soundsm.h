/* Copyright Steve Rabin, 2008. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2008"
 */

#pragma once

#include "statemch.h"
#include "DXUT\SDKsound.h"

class Sound : public StateMachine
{
public:

	Sound( GameObject & object ) : StateMachine( object ) 
    {
        WCHAR str[MAX_PATH];
        // Use D3DX to create a texture from a file based image
        DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"am-i-totally-screwed-or.wav" );
        // load the sound
        m_owner->getSoundManager()->Create(&m_pDeathSound,str,DSBCAPS_CTRLVOLUME);

        // Use D3DX to create a texture from a file based image
        DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"explosion_feu.wav" );
        // load the sound
        m_owner->getSoundManager()->Create(&m_pExplosionSound,str,DSBCAPS_CTRLVOLUME);

        // Use D3DX to create a texture from a file based image
        DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"clong_2.wav" );
        // load the sound
        m_owner->getSoundManager()->Create(&m_pExplosionColSound,str,DSBCAPS_CTRLVOLUME);

        // Use D3DX to create a texture from a file based image
        DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"buzzer.wav" );
        // load the sound
        m_owner->getSoundManager()->Create(&m_pGameEndSound,str,DSBCAPS_CTRLVOLUME);

        // Use D3DX to create a texture from a file based image
        DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"yipee.wav" );
        // load the sound
        m_owner->getSoundManager()->Create(&m_pHealthPackSound,str,DSBCAPS_CTRLVOLUME);

        // Use D3DX to create a texture from a file based image
        DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"cowbell.wav" );
        // load the sound
        m_owner->getSoundManager()->Create(&m_pCountDownSound,str,DSBCAPS_CTRLVOLUME);

        // Use D3DX to create a texture from a file based image
        DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"whistle_2.wav" );
        // load the sound
        m_owner->getSoundManager()->Create(&m_pWhistleSound,str,DSBCAPS_CTRLVOLUME);

        // Use D3DX to create a texture from a file based image
        DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"yaahooo.wav" );
        // load the sound
        m_owner->getSoundManager()->Create(&m_pWinningSound,str,DSBCAPS_CTRLVOLUME);

        // Use D3DX to create a texture from a file based image
        DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"metalplate.wav" );
        // load the sound
        m_owner->getSoundManager()->Create(&m_pWallCollSound,str,DSBCAPS_CTRLVOLUME);
    }
	~Sound( void ) { 
        SAFE_DELETE(m_pDeathSound);
        SAFE_DELETE(m_pExplosionSound);
        SAFE_DELETE(m_pGameEndSound);
        SAFE_DELETE(m_pExplosionColSound);
        SAFE_DELETE( m_pHealthPackSound );
        SAFE_DELETE( m_pCountDownSound );
        SAFE_DELETE( m_pWhistleSound );
        SAFE_DELETE( m_pWinningSound );
        SAFE_DELETE( m_pWallCollSound );
    }


private:

	virtual bool States( State_Machine_Event event, MSG_Object * msg, int state, int substate );

    CSound *m_pDeathSound;
    CSound *m_pExplosionSound;
    CSound *m_pExplosionColSound;
    CSound *m_pGameEndSound;
    CSound *m_pHealthPackSound;
    CSound *m_pCountDownSound;
    CSound *m_pWhistleSound;
    CSound *m_pWinningSound;
    CSound *m_pWallCollSound;
};
