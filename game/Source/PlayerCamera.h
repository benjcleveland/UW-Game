/*
Debug Camera
Described in CP100A-Homework4.doc
*/

#include "DebugCamera.h"
#include "Tiny.h"
#include "Mesh.h"

static enum D3DUtil_PlayerCameraKeys
{
    PLAYER_CAM_ROTATE_LEFT = 0,
    PLAYER_CAM_ROTATE_RIGHT,
    PLAYER_CAM_MOVE_FORWARD,
    PLAYER_CAM_MOVE_BACKWARD,
    PLAYER_CAM_PITCH_UP,
    PLAYER_CAM_PITCH_DOWN,
    PLAYER_CAM_MOVE_UP,
    PLAYER_CAM_MOVE_DOWN,
    PLAYER_CAM_MAX_KEYS,
    PLAYER1_CAM_UNKNOWN = 0xFF
};



class CPlayerCamera : public CDebugCamera
{
public:
    CPlayerCamera();
    ~CPlayerCamera();

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

    void setPlayerPtr( );
    CTiny *p_Player;
    Mesh *p_Mesh;

protected:
    D3DXMATRIX m_mCameraWorld; // World matrix of the camera (inverse of the view matrix)

private:

    static D3DUtil_PlayerCameraKeys MapDebugCameraKey( UINT nKey );
    void GetDebugInput( void );

    float yaw;
    float pitch;
    float cam_dist;
    int prevMouse;
    int prevMouseY;
    D3DXVECTOR3 prevPosition;
};


