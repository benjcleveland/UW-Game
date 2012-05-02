/*
Debug Camera
Described in CP100A-Homework4.doc
*/
#include <vector>
#include "DXUT/DXUTcamera.h"
#include "Tiny.h"
#include "Mesh.h"

#pragma once
static enum D3DUtil_DebugCameraKeys
{
    DEBUG_CAM_ROTATE_LEFT = 0,
    DEBUG_CAM_ROTATE_RIGHT,
    DEBUG_CAM_MOVE_FORWARD,
    DEBUG_CAM_MOVE_BACKWARD,
    DEBUG_CAM_PITCH_UP,
    DEBUG_CAM_PITCH_DOWN,
    DEBUG_CAM_MOVE_UP,
    DEBUG_CAM_MOVE_DOWN,
    DEBUG_CAM_MAX_KEYS,
    DEBUG_CAM_UNKNOWN = 0xFF
};


class CDebugCamera : public CBaseCamera
{
public:
    CDebugCamera();
    ~CDebugCamera();

    // Call these from client and use Get*Matrix() to read new matrices
    virtual LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual void FrameMove( FLOAT fElapsedTime );

    // Functions to change behavior
   // void SetRotateButtons( bool bLeft, bool bMiddle, bool bRight, 
    //    boolbRotateWithoutButtonDown = false );

    // Functions to get state
    D3DXMATRIX* GetWorldMatrix() { return &m_mCameraWorld; }

    const D3DXVECTOR3* GetWorldRight() const { return (D3DXVECTOR3*)&m_mCameraWorld._11; }
    const D3DXVECTOR3* GetWorldUp() const { return (D3DXVECTOR3*)&m_mCameraWorld._21; }
    const D3DXVECTOR3* GetWorldAhead() const { return (D3DXVECTOR3*)&m_mCameraWorld._31; }
    const D3DXVECTOR3* GetEyePt() const { return (D3DXVECTOR3*)&m_mCameraWorld._41; }
    


protected:
    D3DXMATRIX m_mCameraWorld; // World matrix of the camera (inverse of the view matrix)

private:

    static D3DUtil_DebugCameraKeys MapDebugCameraKey( UINT nKey );
    void GetDebugInput( void );
};

// typedef for the camera vector
typedef std::vector<CBaseCamera *> VecCamera;
// class to keep track of all the cameras
class CCameras
{
public:
    CCameras();
    ~CCameras();

    void newPlayerCam(CTiny *player);
    void newPlayerCam( Mesh *mesh);
    void newDebugCam(D3DXVECTOR3 vEye, D3DXVECTOR3 vAt, float rotation, float move );

    void ResetDevice(float fAspectRatio);
    CBaseCamera* nextCam();
    CBaseCamera* prevCam();

    void resetPlayerCam(CTiny *player);
    void resetPlayerCam(Mesh *player);
private:
    VecCamera m_vecCameras;
    int currCam;
    int playercam;
    int meshcam;
};


