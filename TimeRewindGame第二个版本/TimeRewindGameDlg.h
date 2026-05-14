// TimeRewindGameDlg.h
#pragma once

#define MAX_HISTORY 300
#define TIMER_GAME 1

struct CTrap
{
    int x, y;
    double vx, vy;
    int radius;
};

class CTimeRewindGameDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CTimeRewindGameDlg)

public:
    CTimeRewindGameDlg(CWnd* pParent = nullptr);
    virtual ~CTimeRewindGameDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TIMEREWINDGAME_DIALOG };
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
    void ResetPosition();      // 重置位置（保留生命）
    void ResetGame();          // 完全重置游戏
    void NextLevel();          // 进入下一关
    void BeepSound(int freq);  // 简单蜂鸣

private:
    HICON m_hIcon; // 修复：添加成员变量用于存储窗口图标

    // 玩家
    int m_playerX, m_playerY;
    int m_playerRadius;
    int m_health;              // 生命值 0-3
    BOOL m_bInvincible;        // 无敌状态
    int m_invincibleTimer;     // 无敌计时（帧数）

    // 倒流系统
    int m_historyX[MAX_HISTORY];
    int m_historyY[MAX_HISTORY];
    int m_historyHead;
    int m_historyCount;
    BOOL m_bRewinding;
    int m_rewindEnergy;
    int m_rewindCost;

    // 金币
    struct Coin {
        int x, y;
        BOOL collected;
    };
    Coin m_coins[8];
    int m_totalCoins;
    int m_collectedCount;
    int m_score;               // 总分

    // 陷阱
    CTrap m_traps[3];
    int m_trapCount;
    int m_trapSpeedLevel;      // 陷阱速度等级（每关增加）

    // 游戏状态
    BOOL m_bGameRunning;
    BOOL m_bGameWin;
    int m_currentLevel;        // 当前关卡
};