#pragma once



class HUD
{
public:
    HUD();
    ~HUD();

    void hudInit(IDirect3DDevice9* pd3dDevice);

    void Release(void);
    void Render();

    void displayGameStart(bool val) { m_game_start = val; }
    void displayGameReset(bool val) { m_game_reset = val; }
    void displayCountDown( bool val, int start ){ m_countdown = val; m_countdown_val = start; }
    void displayGo(bool val) {m_game_go = val;}
    void setCountdownVal( int val ) { m_countdown_val = val; }
    int getCountdownVal(){ return m_countdown_val; }

    void setHealth(int val) { m_health = val; }
    int getHealth() { return m_health;}

    // get and set the time remaining
    void setTimeRemaining( int time){ m_time_remaining = time; };
    int getTimeRemaining() { return m_time_remaining; }
    
    void displayWinToggle() { m_display_win = !m_display_win; }
    void displayLoseToggle() { m_display_lose = !m_display_lose; }
private:

    void calcTimeString(WCHAR *str, int size);

    ID3DXFont *m_pFont;
    ID3DXSprite* m_pTextSprite;

    bool m_game_start;
    bool m_display_win;
    bool m_display_lose;
    bool m_game_reset;
    bool m_countdown;
    bool m_game_go;
    int m_countdown_val;
    RECT m_window;
    int m_time_remaining;
    int m_health;
};