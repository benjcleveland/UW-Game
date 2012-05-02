#include "DXUT.h"
#include "DebugCamera.h"
#include "PlayerCamera.h"

CDebugCamera::CDebugCamera()
{
    // set the default for to non fixed camera

}

CDebugCamera::~CDebugCamera()
{

}

// Call these from client and use Get*Matrix() to read new matrices
LRESULT CDebugCamera::HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER( hWnd );
    //UNREFERENCED_PARAMETER( lParam );

    int iMouseX = (short)LOWORD(lParam);
    int iMouseY = (short)HIWORD(lParam);
    

    switch( uMsg )
    {
    case WM_KEYDOWN:
        {
            // Map this key to a D3DUtil_CameraKeys enum and update the
            // state of m_aKeys[] by adding the KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK mask
            // only if the key is not down
            D3DUtil_DebugCameraKeys mappedKey = MapDebugCameraKey( (UINT)wParam );
            if( mappedKey != CAM_UNKNOWN && (DWORD)mappedKey < CAM_MAX_KEYS )
            {
                if( FALSE == IsKeyDown(m_aKeys[mappedKey]) )
                {
                    m_aKeys[ mappedKey ] = KEY_WAS_DOWN_MASK | KEY_IS_DOWN_MASK;
                    ++m_cKeysDown;
                }
            }
            break;
        }

    case WM_KEYUP:
        {
            // Map this key to a D3DUtil_CameraKeys enum and update the
            // state of m_aKeys[] by removing the KEY_IS_DOWN_MASK mask.
            D3DUtil_DebugCameraKeys mappedKey = MapDebugCameraKey( (UINT)wParam );
            if( mappedKey != CAM_UNKNOWN && (DWORD)mappedKey < CAM_MAX_KEYS )
            {
                m_aKeys[ mappedKey ] &= ~KEY_IS_DOWN_MASK;
                --m_cKeysDown;
            }
            break;
        }
    default:
        break;
    }

    return FALSE;
}

D3DUtil_DebugCameraKeys CDebugCamera::MapDebugCameraKey( UINT nKey )
{
    // This could be upgraded to a method that's user-definable but for 
    // simplicity, we'll use a hardcoded mapping.
    switch( nKey )
    {
    case VK_UP:    return DEBUG_CAM_MOVE_FORWARD;
    case VK_DOWN:  return DEBUG_CAM_MOVE_BACKWARD;
    case VK_LEFT:  return DEBUG_CAM_ROTATE_LEFT;
    case VK_RIGHT: return DEBUG_CAM_ROTATE_RIGHT;

    case VK_PRIOR: return DEBUG_CAM_PITCH_UP;        // pgup
    case VK_NEXT:  return DEBUG_CAM_PITCH_DOWN;      // pgdn

    case 'Q':      return DEBUG_CAM_MOVE_DOWN;
    case 'Z':      return DEBUG_CAM_MOVE_UP;   
    default:       break;
    }

    return DEBUG_CAM_UNKNOWN;
}

void CDebugCamera::GetDebugInput( void )
{
    m_vKeyboardDirection = D3DXVECTOR3(0,0,0);
    m_vMouseDelta = D3DXVECTOR2(0,0);

    m_vGamePadLeftThumb = D3DXVECTOR3(0,0,0);
        m_vGamePadRightThumb = D3DXVECTOR3(0,0,0);

    // Update acceleration vector based on keyboard state
    if( IsKeyDown(m_aKeys[DEBUG_CAM_MOVE_FORWARD]) )
        m_vKeyboardDirection.z += 1.0f;
    if( IsKeyDown(m_aKeys[DEBUG_CAM_MOVE_BACKWARD]) )
        m_vKeyboardDirection.z -= 1.0f;

    if( IsKeyDown(m_aKeys[DEBUG_CAM_MOVE_UP]) )
        m_vKeyboardDirection.y -= 1.0f;
    if( IsKeyDown(m_aKeys[DEBUG_CAM_MOVE_DOWN]) )
        m_vKeyboardDirection.y += 1.0f;

    if( IsKeyDown(m_aKeys[DEBUG_CAM_ROTATE_LEFT]) )
        m_vMouseDelta.x -= 1.0f;
    if( IsKeyDown(m_aKeys[DEBUG_CAM_ROTATE_RIGHT]) )
        m_vMouseDelta.x += 1.0f;

    if( IsKeyDown(m_aKeys[DEBUG_CAM_PITCH_UP]) )
        m_vMouseDelta.y -= 1.0f;
    if( IsKeyDown(m_aKeys[DEBUG_CAM_PITCH_DOWN]) )
        m_vMouseDelta.y += 1.0f;
}

void CDebugCamera::FrameMove( FLOAT fElapsedTime )
{
    if( DXUTGetGlobalTimer()->IsStopped() )
        fElapsedTime = 1.0f / DXUTGetFPS();

    //  if( IsKeyDown(m_aKeys[CAM_RESET]) )
    //      Reset();

    // Get keyboard/mouse/gamepad input

        GetDebugInput( );

    // Get amount of velocity based on the keyboard input and drag (if any)
    UpdateVelocity( fElapsedTime );

    // Simple euler method to calculate position 
    D3DXVECTOR3 vPosDelta = m_vVelocity * fElapsedTime;

    // Make rotations to the camera
    // Modify to only do this sometimes?
    if(true)
    {
        // Update the pitch & yaw angle based on mouse movement
        float fYawDelta   = m_vRotVelocity.x;
        float fPitchDelta = m_vRotVelocity.y;

        // Invert pitch if requested
        if( m_bInvertPitch )
            fPitchDelta = -fPitchDelta;

        m_fCameraPitchAngle += fPitchDelta;
        m_fCameraYawAngle   += fYawDelta;

        // Limit pitch to straight up or straight down
        m_fCameraPitchAngle = __max( -D3DX_PI/2.0f + .01f,  m_fCameraPitchAngle );
        m_fCameraPitchAngle = __min( +D3DX_PI/2.0f - .01f,  m_fCameraPitchAngle );
    }

    // Make a rotation matrix based on the camera's yaw & pitch
    D3DXMATRIX mCameraRot;
    D3DXMatrixRotationYawPitchRoll( &mCameraRot, m_fCameraYawAngle, m_fCameraPitchAngle, 0 );

    // Transform vectors based on camera's rotation matrix
    D3DXVECTOR3 vWorldUp, vWorldAhead;
    D3DXVECTOR3 vLocalUp    = D3DXVECTOR3(0,1,0);
    D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0,0,1);
    D3DXVec3TransformCoord( &vWorldUp, &vLocalUp, &mCameraRot );
    D3DXVec3TransformCoord( &vWorldAhead, &vLocalAhead, &mCameraRot );

    // Transform the position delta by the camera's rotation 
    D3DXVECTOR3 vPosDeltaWorld;

    // Do not include pitch when transforming position delta vector.
    D3DXMatrixRotationYawPitchRoll( &mCameraRot, m_fCameraYawAngle, 0.0f, 0.0f );

    D3DXVec3TransformCoord( &vPosDeltaWorld, &vPosDelta, &mCameraRot );

    // Move the eye position 
    m_vEye += vPosDeltaWorld;
    if( m_bClipToBoundary )
        ConstrainToBoundary( &m_vEye );

    // Update the lookAt position based on the eye position 
    m_vLookAt = m_vEye + vWorldAhead;

    // Update the view matrix
    D3DXMatrixLookAtLH( &m_mView, &m_vEye, &m_vLookAt, &vWorldUp );

    D3DXMatrixInverse( &m_mCameraWorld, NULL, &m_mView );
}



CCameras::CCameras() : currCam(0){}
CCameras::~CCameras()
{
    for(unsigned int i = 0; i < m_vecCameras.size(); ++i)
    {
        if(m_vecCameras[i] != NULL )
        {
            delete m_vecCameras[i];
            m_vecCameras[i] = NULL;
        }
    }
    m_vecCameras.clear();
}

void CCameras::newPlayerCam(CTiny *player)
{
    CBaseCamera *p_cam = new CPlayerCamera();

    ((CPlayerCamera *)p_cam)->p_Player = player;

    playercam = m_vecCameras.size();
    m_vecCameras.push_back(p_cam);
}
void CCameras::newPlayerCam(Mesh *mesh)
{
    CBaseCamera *p_cam = new CPlayerCamera();

    ((CPlayerCamera *)p_cam)->p_Mesh = mesh;

    meshcam = m_vecCameras.size();
    m_vecCameras.push_back(p_cam);
}

void CCameras::newDebugCam(D3DXVECTOR3 vEye, D3DXVECTOR3 vAt, float rotation, float move )
{
    CBaseCamera *p_cam = new CDebugCamera();

    p_cam->SetViewParams( &vEye, &vAt );
    p_cam->SetScalers( rotation, move );  // Camera movement parameters

    // insert into the camera array
    m_vecCameras.push_back(p_cam);
}

CBaseCamera* CCameras::nextCam()
{
    if( currCam == m_vecCameras.size() - 1 )
        currCam = 0;
    else
        ++currCam;

    return m_vecCameras[currCam];
}

CBaseCamera* CCameras::prevCam()
{
    if( currCam == 0 )
        currCam = m_vecCameras.size() -1;
    else
        --currCam;

    return m_vecCameras[currCam]; 
}

void CCameras::ResetDevice(float fAspectRatio)
{
  // Setup the camera's projection parameters
    for(unsigned int i =0; i < m_vecCameras.size(); ++i)
    {
        m_vecCameras[i]->SetProjParams( D3DX_PI/3, fAspectRatio, 0.001f, 100.0f );
    }
    
}

void CCameras::resetPlayerCam(CTiny *player)
{
    ((CPlayerCamera *)m_vecCameras[playercam])->p_Player = player;
}
void CCameras::resetPlayerCam(Mesh *player)
{
    ((CPlayerCamera *)m_vecCameras[meshcam])->p_Mesh = player;
}