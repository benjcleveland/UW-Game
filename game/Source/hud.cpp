#include "DXUT.h"
#include "hud.h"
#include "DXUT\SDKmisc.h"

using namespace std;

#define MAX_SIZE 2

HUD::HUD() : m_pFont(NULL), m_pTextSprite(NULL), m_game_start(true), m_time_remaining(300),
    m_display_win(false),
    m_display_lose(false),
    m_game_reset(false),
    m_countdown(false),
    m_game_go(false)
{}

HUD::~HUD()
{
    Release();
}

void HUD::Release(void)
{
    SAFE_RELEASE(m_pFont);
    SAFE_RELEASE(m_pTextSprite);
}
void HUD::hudInit(IDirect3DDevice9* pd3dDevice)
{
    // create the font
    D3DXCreateFont( pd3dDevice, 18, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Arial", &m_pFont );

    // this will help with performance
    D3DXCreateSprite( pd3dDevice, &m_pTextSprite );

    m_window = DXUTGetWindowClientRect();   
}

void HUD::calcTimeString(WCHAR *str, int size)
{
    int minutes = m_time_remaining/60;
    float seconds = (float)((float)((float)(m_time_remaining/60.f) - minutes))*60.f + .1f;
    
    // create the string
    swprintf(str, size, L"Time Remaining - %d:%02.0f", minutes, seconds);
    return;
}

void HUD::Render()
{
    CDXUTTextHelper txtHelper( m_pFont, m_pTextSprite, 18 );
    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();

    m_window = DXUTGetWindowClientRect();
     // Output statistics
    txtHelper.Begin();
    if( m_game_start )
    {
        WCHAR *str = L"Press 'C' to start the game!\0";
        size_t size = wcslen(str);
        txtHelper.SetInsertionPos( m_window.bottom/2 - wcslen(str)/2,  m_window.bottom/2 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 10.0f, 10.0f, 10.0f, 1.0f ) );
        txtHelper.DrawTextLine( str );
    }
     if( m_game_reset )
    {
        WCHAR *str = L"Press 'R' to reset the game!\0";
        
        txtHelper.SetInsertionPos(  m_window.bottom/2 - wcslen(str)/2,  m_window.bottom/2  );
        txtHelper.SetForegroundColor( D3DXCOLOR( 10.0f, 10.0f, 10.0f, 1.0f ) );
        txtHelper.DrawTextLine( str );
    }

    if( m_countdown )
    {
        WCHAR str[100];

        swprintf(str, sizeof(str), L"The game will start in %d seconds...", m_countdown_val );

        txtHelper.SetInsertionPos(  m_window.bottom/2 - wcslen(str)/2,  m_window.bottom/2  );
        txtHelper.SetForegroundColor( D3DXCOLOR( 10.0f, 10.0f, 10.0f, 1.0f ) );
        txtHelper.DrawTextLine( str );
    }

    if(m_game_go)
    {
        WCHAR *str = L"GO!!!!!!\0";

        txtHelper.SetInsertionPos(  m_window.bottom/2 - wcslen(str)/2,  m_window.bottom/2  );
        txtHelper.SetForegroundColor( D3DXCOLOR( 10.0f, 10.0f, 10.0f, 1.0f ) );
        txtHelper.DrawTextLine( str );
    }

    WCHAR time_str[100];
    // display time remaining
    calcTimeString(time_str, sizeof(time_str));
    txtHelper.SetInsertionPos( m_window.right - 200, m_window.bottom - 50);
    txtHelper.DrawTextLine( time_str );

    // display health
    swprintf(time_str, sizeof(time_str), L"Health - %d%%", m_health);
    txtHelper.SetInsertionPos( m_window.right - 200, m_window.bottom - 30);
    txtHelper.DrawTextLine( time_str );

    if( m_display_win )
    {
        WCHAR *str = L"You Won the game!!\0";

        txtHelper.SetInsertionPos(  m_window.bottom/2 - wcslen(str)/2,  m_window.bottom/2  - 15 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 1.0f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( str );
    }
    if( m_display_lose )
    {
        WCHAR *str = L"You lost the game!\0";

        txtHelper.SetInsertionPos(  m_window.bottom/2 - wcslen(str)/2,  m_window.bottom/2 -15 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( str );
    }

    txtHelper.End();
}
