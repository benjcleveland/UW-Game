#include "DXUT.h"
#include "PlayerCamera.h"

CPlayerCamera::CPlayerCamera() : p_Player(NULL), p_Mesh(NULL), yaw(0), prevMouse(0), prevMouseY(0), cam_dist(1), pitch(D3DX_PI/2-.25f)
{
    prevPosition = D3DXVECTOR3(0,0,0);
}

CPlayerCamera::~CPlayerCamera()
{

}

// Call these from client and use Get*Matrix() to read new matrices
LRESULT CPlayerCamera::HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER( hWnd );
    //UNREFERENCED_PARAMETER( lParam );

    int iMouseX = (short)LOWORD(lParam);
    int iMouseY = (short)HIWORD(lParam);
    int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    
    switch( uMsg )
    {
    case WM_LBUTTONDOWN:
        // case WM_LBUTTONDBLCLK:
        m_vMouseDelta.x  = 0.0f;
        m_vMouseDelta.y = .0f;
       // yaw = 0;
        prevMouse = iMouseX;
        break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        m_vMouseDelta.x = 0.f;
        m_vMouseDelta.y = 0.f;
        break;
    case WM_RBUTTONDOWN:
        // case WM_LBUTTONDBLCLK:
        m_vMouseDelta.x  = -1.0f;

        //yaw = 0;
        break;

    case WM_MOUSEMOVE:
            if( MK_LBUTTON&wParam )
            {
                if( iMouseX - prevMouse > 0 )
                {
                    m_vMouseDelta.x = 2.5f;
                }
                else if(iMouseX - prevMouse < 0 )
                    m_vMouseDelta.x = -2.5f;
                else
                    m_vMouseDelta.x = 0.f;

                prevMouse = iMouseX;
                if( iMouseY - prevMouseY > 0)
                    m_vMouseDelta.y = -1.5f;
                else if (iMouseY - prevMouseY < 0 )
                    m_vMouseDelta.y = 1.5f;
                else
                    m_vMouseDelta.y = 0.f;
                prevMouseY = iMouseY;
            }


        break;
    case WM_MOUSEWHEEL:
        if(zDelta > 0 )
        {
            if( cam_dist > .35f )
            {
                cam_dist -= .025f;
            }
            else
                cam_dist = .35f;
        }
        else
        {
            if( cam_dist < 1.f )
            {
                cam_dist += .025f;
            }
            else 
                cam_dist = 1.f;
        }
           break; 
    default:
        break;
    }

    return FALSE;
}

D3DUtil_PlayerCameraKeys CPlayerCamera::MapDebugCameraKey( UINT nKey )
{
    // This could be upgraded to a method that's user-definable but for 
    // simplicity, we'll use a hardcoded mapping.

    return PLAYER1_CAM_UNKNOWN;
}

void CPlayerCamera::GetDebugInput( void )
{
    m_vKeyboardDirection = D3DXVECTOR3(0,0,0);
    m_vMouseDelta = D3DXVECTOR2(0,0);
}



void CPlayerCamera::FrameMove( FLOAT fElapsedTime)
{
    D3DXVECTOR3 player;
    D3DXVECTOR3 distPos;
    D3DXVECTOR3 vfacing;
    // clean this up

    if(p_Player != NULL )
    {
        vfacing = p_Player->GetFacing();
        player = p_Player->GetPosition();
    }
    else
    {
        vfacing = p_Mesh->getFacing();
        player = p_Mesh->getPosition();
    }
    
    m_vEye.y = 0;
    distPos = ((player-vfacing) - m_vEye);
   if( D3DXVec3Length(&distPos) > .05f)

    {
        prevPosition = (distPos) * 6.5f * fElapsedTime;//*.2f; //* 4.5f * fElapsedTime;
        m_vEye += prevPosition;
        m_vLookAt += prevPosition;
    }
/*
    else
    {
        //prevPosition = player;
        m_vEye = player;
        m_vLookAt = player;
        m_vEye -= vfacing;
    }
*/

    // update the mouse input
    D3DXVECTOR2 vDeltaRotVelocity = m_vMouseDelta * m_fRotationScaler;
    m_vMouseDelta.x = 0.0f;
    m_vRotVelocity = vDeltaRotVelocity;

    

    m_vEye.y = 1.1f;
    m_vLookAt.y = 0.f;


    //m_vEye -= vfacing;
    m_vLookAt = player;
    m_vLookAt += vfacing;

    m_vLookAt.y = .1f;
    this->SetViewParams(&m_vEye,&m_vLookAt);
    float yawx = acosf( vfacing.x);
    float yawy = asinf(vfacing.z);
  
#if 0
    pitch += m_vRotVelocity.y;

    if( pitch < D3DX_PI/2 -1.1f )
        pitch = D3DX_PI/2 -1.1f;
    if(pitch > D3DX_PI/2  -.05f)
        pitch = D3DX_PI/2 -.05f;
    //float yawf = atan2f(vfacing.x, vfacing.y);
    //float cos
    //if( m_vRotVelocity.x != 0 )
    {
        yaw   += m_vRotVelocity.x;
        // rotate the camera
        if(vfacing.x >= 0 && vfacing.z >= 0)
        {
            m_vEye.x = -cos(yawx-yaw)*cam_dist*sin(pitch)+player.x;
            m_vEye.z = -sin(yawy-yaw)*cam_dist*sin(pitch)+player.z;
            m_vLookAt.x = cos(yawx-yaw)*cam_dist/*(-sin(pitch))*/ + player.x;
            m_vLookAt.z = sin(yawy-yaw)*cam_dist/*(-sin(pitch))*/ + player.z;
            //m_vLookAt.y = .11f + cam_dist*(cos(pitch));
        }
        else if( vfacing.x >= 0 && vfacing.z < 0 )
        {
            if( vfacing.z < 0)
                yawy = -yawy;
            m_vEye.x = -cos(yawx+yaw)*cam_dist*sin(pitch)+player.x;
            m_vEye.z = sin(yawy+yaw)*cam_dist*sin(pitch)+player.z;
            m_vLookAt.x = cos(yawx+yaw)*cam_dist + player.x;
            m_vLookAt.z = -sin(yawy+yaw)*cam_dist+player.z;   
        }
        else if((vfacing.x < 0 && vfacing.z >=0)  )
        {

            if(vfacing.x < 0)
                yawx = -yawx;
            m_vEye.x = -cos(yawx+yaw)*cam_dist*sin(pitch)+player.x;
            m_vEye.z = -sin(yawy+yaw)*cam_dist*sin(pitch)+player.z;
            m_vLookAt.x = cos(yawx+yaw)*cam_dist + player.x;
            m_vLookAt.z = sin(yawy+yaw)*cam_dist+player.z;
        }

        else 
        {
            m_vEye.x = -cos(yawx+yaw)*cam_dist*sin(pitch)+player.x;
            m_vEye.z = -sin(yawy+yaw)*cam_dist*sin(pitch)+player.z;
            m_vLookAt.z = sin(yawy+yaw)*cam_dist+player.z;
            m_vLookAt.x = cos(yawx+yaw)*cam_dist + player.x;
        }
    }
#endif
    m_vEye.y = .5f + cos(pitch)*cam_dist;
    m_vLookAt.y = .1f - cos(pitch)*cam_dist;

    /*float yawf = atan2f(vfacing.x, vfacing.y);
    m_vLookAt.x += sin(yawf)*cam_dist;
    m_vLookAt.z += cos(yawf)*cam_dist;*/
    D3DXVECTOR3 vUp(0,1,0);
    D3DXMatrixLookAtLH( &m_mView, &m_vEye, &m_vLookAt, &vUp );

    D3DXMATRIX mInvView;
    D3DXMatrixInverse( &mInvView, NULL, &m_mView );
}
/*
void CPlayerCamera::playerFrameMove( FLOAT fElapsedTime, D3DXVECTOR3 player, D3DXVECTOR3 vfacing)
{
// update the mouse input
m_vRotVelocity = m_vMouseDelta * m_fRotationScaler;


float yawf   = atan2f( -vfacing.x, -vfacing.z );
if( m_vRotVelocity.x > 0 )
{
//m_vVelocity.x = 1.0f;
D3DXVECTOR3 vPosDelta = m_vVelocity * fElapsedTime;
yaw   += m_vRotVelocity.x;
//m_fCameraYawAngle += yaw;
// Make a rotation matrix based on the camera's yaw & pitch
D3DXMATRIX mCameraRot;
D3DXMatrixRotationYawPitchRoll( &mCameraRot, m_fCameraYawAngle+yaw, 0, 0 );

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

// Update the lookAt position based on the eye position 
m_vLookAt = m_vEye + vWorldAhead;

// Update the view matrix
D3DXMatrixLookAtLH( &m_mView, &m_vEye, &m_vLookAt, &vWorldUp );

D3DXMatrixInverse( &m_mCameraWorld, NULL, &m_mView );

}
else
{

m_vEye = player;
m_vLookAt = player;

m_vEye.y = 0.75f;
m_vLookAt.y = 0.1f;

//m_vEye -= vfacing;
m_vLookAt += vfacing;
m_vEye -= vfacing;

D3DXVECTOR3 vUp(0,1,0);
D3DXMatrixLookAtLH( &m_mView, &m_vEye, &m_vLookAt, &vUp );

D3DXMATRIX mInvView;
D3DXMatrixInverse( &mInvView, NULL, &m_mView );

}
}

*/

/*
void CPlayerCamera::playerFrameMove( FLOAT fElapsedTime, D3DXVECTOR3 player, D3DXVECTOR3 vfacing)
{
// update the mouse input
m_vRotVelocity = m_vMouseDelta * m_fRotationScaler;

m_vEye = player;
m_vLookAt = player;

m_vEye.y = 0.75f;
m_vLookAt.y = 0.1f;

//m_vEye -= vfacing;
m_vLookAt += vfacing;
float yawf   = atan2f( -vfacing.x, -vfacing.z );
if( m_vRotVelocity.x > 0 )
{
D3DXMATRIX  mx;
//  yaw   += m_vRotVelocity.x;
// rotate the player
D3DXMatrixRotationYawPitchRoll( &mx, m_fCameraYawAngle + yaw, 0, 0 );

D3DXVECTOR3 vWorldUp, vWorldAhead;
D3DXVECTOR3 vLocalUp    = D3DXVECTOR3(0,1,0);
D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0,0,1);
D3DXVec3TransformCoord( &vWorldUp, &vLocalUp, &mx );
D3DXVec3TransformCoord( &vWorldAhead, &vLocalAhead, &mx );

D3DXVECTOR3 vPosDeltaWorld;
D3DXVec3TransformCoord( &vPosDeltaWorld, &m_vEye, &mx);
m_vEye += vPosDeltaWorld;
//   yaw   += m_vRotVelocity.x;
// m_vEye.z += cos(yawf + yaw)*m_vEye.x + sin(yawf + yaw)*m_vEye.z;// + player.z;
//  m_vEye.x += -sin(yawf + yaw)*m_vEye.x + cos(yawf +yaw)*m_vEye.z;// + player.x;

//m_vEye.x = cos(yawf)*(-vfacing.x) + player.x;
//m_vEye.z = sin(yawf)*(-vfacing.z)+ player.z;
//m_vLookAt.z = -(cos(yawf + yaw) + sin(yawf + yaw));// + player.x;
//m_vLookAt.x = -(cos(yawf + yaw) - sin(yawf + yaw));// + player.z;

m_vEye -= vfacing;
// m_vLookAt += vfacing;

//  m_vEye.y = 0.75f;
//  m_vLookAt.y = 0.1f;

}
else
{


m_vEye -= vfacing;
}
D3DXVECTOR3 vUp(0,1,0);
D3DXMatrixLookAtLH( &m_mView, &m_vEye, &m_vLookAt, &vUp );

D3DXMATRIX mInvView;
D3DXMatrixInverse( &mInvView, NULL, &m_mView );
}

*/

