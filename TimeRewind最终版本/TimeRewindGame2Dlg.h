// TimeRewindGame2Dlg.h
#pragma once

#include <afxtempl.h>

#define MAX_HISTORY 300
#define TIMER_GAME 1

struct CTrap
{
    int x, y;
    double vx, vy;
    int radius;
};

class CTimeRewindGame2Dlg : public CDialogEx
{
    DECLARE_DYNAMIC(CTimeRewindGame2Dlg)

public:
    CTimeRewindGame2Dlg(CWnd* pParent = nullptr);
    virtual ~CTimeRewindGame2Dlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TIMEREWINDGAME2_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

    DECLARE_MESSAGE_MAP()

private:
    void DrawPlayer(CDC* pDC, int x, int y);
    void DrawCoin(CDC* pDC, int x, int y);
    void DrawTrap(CDC* pDC, int x, int y);
    void DrawEnergyBar(CDC* pDC);
    void DrawHealthBar(CDC* pDC);
    void CheckCollision();
    void ResetPosition();
    void ResetGame();
    void NextLevel();
    void BeepSound(int freq);

private:
    int m_playerX, m_playerY;
    int m_playerRadius;
    int m_health;
    BOOL m_bInvincible;
    int m_invincibleTimer;

    int m_historyX[MAX_HISTORY];
    int m_historyY[MAX_HISTORY];
    int m_historyHead;
    int m_historyCount;
    int m_rewindEnergy;
    int m_rewindCost;

    struct Coin {
        int x, y;
        BOOL collected;
    };
    Coin m_coins[8];
    int m_totalCoins;
    int m_collectedCount;
    int m_score;

    CArray<CTrap, CTrap> m_traps;

    BOOL m_bGameRunning;
    BOOL m_bGameWin;
    int m_currentLevel;

    HICON m_hIcon;
};