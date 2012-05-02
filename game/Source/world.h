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

class Time;
class Database;
class MsgRoute;
class DebugLog;


#include <vector>
#include "DXUT/SDKsound.h"
#include "gridlayer.h"
#include "DXUT\SDKmisc.h"

class World
{
public:
	World();
	~World();

	void InitializeSingletons( void );
	void Initialize( IDirect3DDevice9* pd3dDevice, CSoundManager *p_sound);
	void Update();

    void Animate( double dTimeDelta, double dTime, const D3DXVECTOR3 *cameraFacing );
	void AdvanceTimeAndRender( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj, double dTimeDelta, D3DXVECTOR3 *pvEye, bool shadowPass );

    void Release();

    // debug enable/disable
    void setDebug(bool debug);

    void gridDebug();
    GridLayer* getGridLayer(){ return &m_grid_layer; }

    void renderText(CDXUTTextHelper *txtHelper);
    
protected:

	bool m_initialized;
	Time* m_time;
	Database* m_database;
	MsgRoute* m_msgroute;
	DebugLog* m_debuglog;

    // grid layers
    GridLayer m_grid_layer;
};

