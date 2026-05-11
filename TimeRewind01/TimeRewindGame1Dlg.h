// TimeRewindGame1Dlg.h
#pragma once

#define MAX_HISTORY 300
#define TIMER_GAME 1

// CTimeRewindGame1Dlg 对话框
class CTimeRewindGame1Dlg : public CDialogEx
{
    DECLARE_DYNAMIC(CTimeRewindGame1Dlg)

public:
    CTimeRewindGame1Dlg(CWnd* pParent = nullptr);
    virtual ~CTimeRewindGame1Dlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TIMEREWINDGAME1_DIALOG };
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

protected:
    // 图标
    HICON m_hIcon;

private:
    // 玩家坐标
    int m_playerX;
    int m_playerY;

    // 历史记录（循环队列）
    int m_historyX[MAX_HISTORY];
    int m_historyY[MAX_HISTORY];
    int m_historyHead;   // 写入位置
    int m_historyCount;  // 当前存了多少帧

    // 倒流状态
    BOOL m_bRewinding;
};