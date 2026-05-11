// TimeRewindGame1Dlg.cpp
#include "pch.h"
#include "framework.h"
#include "TimeRewindGame1.h"
#include "TimeRewindGame1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CTimeRewindGame1Dlg, CDialogEx)

BEGIN_MESSAGE_MAP(CTimeRewindGame1Dlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_KEYDOWN()
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()

// 构造函数
CTimeRewindGame1Dlg::CTimeRewindGame1Dlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_TIMEREWINDGAME1_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // 初始化变量
    m_playerX = 200;
    m_playerY = 200;
    m_historyHead = 0;
    m_historyCount = 0;
    m_bRewinding = FALSE;
}

// 析构函数
CTimeRewindGame1Dlg::~CTimeRewindGame1Dlg()
{
}

// 数据交换
void CTimeRewindGame1Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

// 初始化对话框
BOOL CTimeRewindGame1Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置图标
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // 设置窗口大小
    SetWindowPos(NULL, 0, 0, 550, 450, SWP_NOMOVE | SWP_NOZORDER);

    // 启动定时器（每50毫秒一帧）
    SetTimer(TIMER_GAME, 50, NULL);

    return TRUE;
}

// 绘制游戏画面
void CTimeRewindGame1Dlg::OnPaint()
{
    CPaintDC dc(this);

    // 获取客户区大小
    CRect rect;
    GetClientRect(&rect);

    // 双缓冲绘图
    CDC memDC;
    CBitmap memBitmap;
    memDC.CreateCompatibleDC(&dc);
    memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    memDC.SelectObject(&memBitmap);

    // 白色背景
    memDC.FillSolidRect(rect, RGB(255, 255, 255));

    // 绘制玩家（红色方块）
    CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
    CBrush brush(RGB(255, 0, 0));
    memDC.SelectObject(&pen);
    memDC.SelectObject(&brush);
    memDC.Rectangle(m_playerX - 10, m_playerY - 10, m_playerX + 10, m_playerY + 10);

    // 倒流时显示提示
    if (m_bRewinding)
    {
        CFont font;
        font.CreatePointFont(160, L"黑体");
        memDC.SelectObject(&font);
        memDC.SetTextColor(RGB(0, 0, 255));
        memDC.SetBkMode(TRANSPARENT);
        memDC.TextOutW(rect.Width() / 2 - 80, rect.Height() - 50, L"时间倒流中...");
    }

    // 复制到屏幕
    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
}

// 定时器：记录历史 或 执行倒流
void CTimeRewindGame1Dlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TIMER_GAME)
    {
        if (!m_bRewinding)
        {
            // 正常状态：记录当前位置
            m_historyX[m_historyHead] = m_playerX;
            m_historyY[m_historyHead] = m_playerY;
            m_historyHead = (m_historyHead + 1) % MAX_HISTORY;

            if (m_historyCount < MAX_HISTORY)
                m_historyCount++;
        }
        else
        {
            // 倒流状态：回退一步
            if (m_historyCount > 0)
            {
                int prevIndex = (m_historyHead - 1 + MAX_HISTORY) % MAX_HISTORY;
                m_playerX = m_historyX[prevIndex];
                m_playerY = m_historyY[prevIndex];
                m_historyHead = prevIndex;
                m_historyCount--;
                Invalidate(FALSE);
            }
            else
            {
                m_bRewinding = FALSE;
            }
        }
    }
    CDialogEx::OnTimer(nIDEvent);
}

// 键盘按下处理
void CTimeRewindGame1Dlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // 空格：开始倒流（只倒流一步）
    if (nChar == VK_SPACE)
    {
        if (!m_bRewinding && m_historyCount > 0)
        {
            m_bRewinding = TRUE;
        }
        return;
    }

    // 倒流时不能移动
    if (m_bRewinding) return;

    // 方向键移动
    int step = 10;
    if (nChar == VK_LEFT)  m_playerX -= step;
    if (nChar == VK_RIGHT) m_playerX += step;
    if (nChar == VK_UP)    m_playerY -= step;
    if (nChar == VK_DOWN)  m_playerY += step;

    // 边界限制
    if (m_playerX < 10)  m_playerX = 10;
    if (m_playerX > 540) m_playerX = 540;
    if (m_playerY < 10)  m_playerY = 10;
    if (m_playerY > 440) m_playerY = 440;

    // 重绘
    Invalidate(FALSE);

    CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

// 对话框激活时获得焦点
void CTimeRewindGame1Dlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CDialogEx::OnActivate(nState, pWndOther, bMinimized);
    if (nState != WA_INACTIVE)
    {
        SetFocus();
    }
}

// 拦截键盘消息
BOOL CTimeRewindGame1Dlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
        return TRUE;
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}