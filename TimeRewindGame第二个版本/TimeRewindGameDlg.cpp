// TimeRewindGameDlg.cpp
#include "pch.h"
#include "framework.h"
#include "TimeRewindGame.h"
#include "TimeRewindGameDlg.h"
#include "afxdialogex.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CTimeRewindGameDlg, CDialogEx)

BEGIN_MESSAGE_MAP(CTimeRewindGameDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_KEYDOWN()
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()

// 构造函数
CTimeRewindGameDlg::CTimeRewindGameDlg(CWnd* pParent)
    : CDialogEx(IDD_TIMEREWINDGAME_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    ResetGame();
}

CTimeRewindGameDlg::~CTimeRewindGameDlg() {}

void CTimeRewindGameDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

// 简单蜂鸣
void CTimeRewindGameDlg::BeepSound(int freq)
{
    ::Beep(freq, 80);
}

// 重置位置（保留生命）
void CTimeRewindGameDlg::ResetPosition()
{
    m_playerX = 250;
    m_playerY = 200;
    m_bInvincible = TRUE;
    m_invincibleTimer = 30;  // 1.5秒无敌（30帧 * 50ms）
    BeepSound(300);
}

// 完全重置游戏
void CTimeRewindGameDlg::ResetGame()
{
    m_playerX = 250;
    m_playerY = 200;
    m_playerRadius = 15;
    m_health = 3;
    m_bInvincible = FALSE;
    m_invincibleTimer = 0;

    m_historyHead = 0;
    m_historyCount = 0;
    m_bRewinding = FALSE;
    m_rewindEnergy = 100;
    m_rewindCost = 10;

    m_bGameRunning = TRUE;
    m_bGameWin = FALSE;
    m_collectedCount = 0;
    m_totalCoins = 8;
    m_score = 0;
    m_currentLevel = 1;
    m_trapSpeedLevel = 1;

    // 金币位置
    Coin coinPos[] = {
        {100, 100, FALSE}, {400, 100, FALSE}, {250, 50, FALSE},
        {100, 300, FALSE}, {400, 300, FALSE}, {150, 200, FALSE},
        {350, 200, FALSE}, {250, 350, FALSE}
    };
    for (int i = 0; i < 8; i++)
        m_coins[i] = coinPos[i];

    // 陷阱
    m_trapCount = 3;
    double baseSpeed = 2.0 + (m_currentLevel - 1) * 0.5;
    m_traps[0] = { 100, 150, baseSpeed, baseSpeed, 12 };
    m_traps[1] = { 400, 250, -baseSpeed, baseSpeed * 1.5, 12 };
    m_traps[2] = { 250, 350, baseSpeed * 1.2, -baseSpeed, 12 };
}

// 下一关
void CTimeRewindGameDlg::NextLevel()
{
    m_currentLevel++;
    m_collectedCount = 0;

    // 重置金币
    for (int i = 0; i < m_totalCoins; i++)
        m_coins[i].collected = FALSE;

    // 陷阱加速
    m_trapSpeedLevel = m_currentLevel;
    double baseSpeed = 2.0 + (m_currentLevel - 1) * 0.8;
    m_traps[0] = { 100, 150, baseSpeed, baseSpeed, 12 };
    m_traps[1] = { 400, 250, -baseSpeed, baseSpeed * 1.5, 12 };
    m_traps[2] = { 250, 350, baseSpeed * 1.2, -baseSpeed, 12 };

    // 恢复能量
    m_rewindEnergy = 100;
    ResetPosition();
    BeepSound(800);
}

// 初始化对话框
BOOL CTimeRewindGameDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);
    SetWindowPos(NULL, 0, 0, 550, 450, SWP_NOMOVE | SWP_NOZORDER);
    SetTimer(TIMER_GAME, 50, NULL);
    return TRUE;
}
void CTimeRewindGameDlg::DrawPlayer(CDC* pDC, int x, int y)
{
    // 无敌时闪烁（每5帧显示/隐藏）
    if (m_bInvincible && (m_invincibleTimer / 3) % 2 == 0)
    {
        CPen pen(PS_SOLID, 2, RGB(200, 200, 200));
        pDC->SelectObject(&pen);
        pDC->SelectObject(GetStockObject(WHITE_BRUSH));
        pDC->Ellipse(x - m_playerRadius, y - m_playerRadius,
            x + m_playerRadius, y + m_playerRadius);
        return;
    }

    CBrush brush(RGB(0, 100, 255));
    CBrush* pOldBrush = pDC->SelectObject(&brush);
    CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
    CPen* pOldPen = pDC->SelectObject(&pen);

    pDC->Ellipse(x - m_playerRadius, y - m_playerRadius,
        x + m_playerRadius, y + m_playerRadius);

    pDC->SelectObject(GetStockObject(BLACK_BRUSH));
    pDC->Ellipse(x - 6, y - 5, x - 2, y - 1);
    pDC->Ellipse(x + 2, y - 5, x + 6, y - 1);

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
}

void CTimeRewindGameDlg::DrawCoin(CDC* pDC, int x, int y)
{
    CBrush brush(RGB(255, 215, 0));
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    POINT points[4] = { {x, y - 8}, {x + 8, y}, {x, y + 8}, {x - 8, y} };
    pDC->Polygon(points, 4);

    pDC->SelectObject(GetStockObject(WHITE_BRUSH));
    pDC->Ellipse(x - 2, y - 2, x + 2, y + 2);
    pDC->SelectObject(pOldBrush);
}

void CTimeRewindGameDlg::DrawTrap(CDC* pDC, int x, int y)
{
    CBrush brush(RGB(255, 50, 50));
    CBrush* pOldBrush = pDC->SelectObject(&brush);
    CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
    CPen* pOldPen = pDC->SelectObject(&pen);

    pDC->Ellipse(x - 12, y - 12, x + 12, y + 12);
    pDC->MoveTo(x - 8, y - 8);
    pDC->LineTo(x + 8, y + 8);
    pDC->MoveTo(x + 8, y - 8);
    pDC->LineTo(x - 8, y + 8);

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
}

void CTimeRewindGameDlg::DrawEnergyBar(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);

    // 圆角效果
    int barX = rect.Width() - 130;
    pDC->FillSolidRect(barX, 20, 110, 15, RGB(200, 200, 200));
    int width = (int)(110 * m_rewindEnergy / 100.0);
    pDC->FillSolidRect(barX, 20, width, 15, RGB(0, 150, 255));

    CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
    pDC->SelectObject(&pen);
    pDC->SelectObject(GetStockObject(NULL_BRUSH));
    pDC->Rectangle(barX, 20, barX + 110, 35);

    pDC->SetBkMode(TRANSPARENT);
    pDC->TextOutW(barX, 5, L"能量");
}

void CTimeRewindGameDlg::DrawHealthBar(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);
    CString str;
    str.Format(L"❤️ x %d", m_health);
    pDC->SetBkMode(TRANSPARENT);
    pDC->TextOutW(20, 5, str);
}
void CTimeRewindGameDlg::CheckCollision()
{
    // 金币碰撞
    for (int i = 0; i < m_totalCoins; i++)
    {
        if (!m_coins[i].collected)
        {
            int dx = m_playerX - m_coins[i].x;
            int dy = m_playerY - m_coins[i].y;
            int dist = (int)sqrt((double)(dx * dx + dy * dy));
            if (dist < m_playerRadius + 8)
            {
                m_coins[i].collected = TRUE;
                m_collectedCount++;
                m_score += 10;
                m_rewindEnergy += 15;
                if (m_rewindEnergy > 100) m_rewindEnergy = 100;
                BeepSound(1000);

                // 每收集2个金币，陷阱加速
                if (m_collectedCount % 2 == 0)
                {
                    for (int j = 0; j < m_trapCount; j++)
                    {
                        double factor = 1.2;
                        m_traps[j].vx *= factor;
                        m_traps[j].vy *= factor;
                    }
                }
            }
        }
    }

    // 陷阱碰撞（无敌时不检测）
    if (!m_bInvincible)
    {
        for (int i = 0; i < m_trapCount; i++)
        {
            int dx = m_playerX - m_traps[i].x;
            int dy = m_playerY - m_traps[i].y;
            int dist = (int)sqrt((double)(dx * dx + dy * dy));
            if (dist < m_playerRadius + m_traps[i].radius)
            {
                m_health--;
                BeepSound(200);
                if (m_health <= 0)
                {
                    ResetGame();
                }
                else
                {
                    ResetPosition();
                }
                return;
            }
        }
    }

    // 关卡完成
    if (m_collectedCount >= m_totalCoins)
    {
        if (m_currentLevel >= 5)
        {
            m_bGameWin = TRUE;
            m_bGameRunning = FALSE;
            BeepSound(1200);
        }
        else
        {
            NextLevel();
        }
        Invalidate(FALSE);
    }
}
void CTimeRewindGameDlg::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);

    CDC memDC;
    CBitmap memBitmap;
    memDC.CreateCompatibleDC(&dc);
    memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    memDC.SelectObject(&memBitmap);

    // 渐变背景
    for (int i = 0; i < rect.Height(); i++)
    {
        int r = 200 - i / 3;
        int g = 220 - i / 4;
        int b = 255;
        if (r < 100) r = 100;
        if (g < 150) g = 150;
        memDC.FillSolidRect(0, i, rect.Width(), 1, RGB(r, g, b));
    }

    // 金币
    for (int i = 0; i < m_totalCoins; i++)
        if (!m_coins[i].collected)
            DrawCoin(&memDC, m_coins[i].x, m_coins[i].y);

    // 陷阱
    for (int i = 0; i < m_trapCount; i++)
        DrawTrap(&memDC, m_traps[i].x, m_traps[i].y);

    // 玩家
    DrawPlayer(&memDC, m_playerX, m_playerY);
    DrawEnergyBar(&memDC);
    DrawHealthBar(&memDC);

    // UI文字
    CString str;
    str.Format(L"得分: %d  关卡: %d  金币: %d/%d",
        m_score, m_currentLevel, m_collectedCount, m_totalCoins);
    memDC.SetBkMode(TRANSPARENT);
    memDC.TextOutW(rect.Width() / 2 - 120, 8, str);

    // 倒流提示
    if (m_bRewinding)
    {
        CFont font;
        font.CreatePointFont(200, L"黑体");
        memDC.SelectObject(&font);
        memDC.SetTextColor(RGB(0, 0, 255));
        memDC.TextOutW(rect.Width() / 2 - 100, rect.Height() / 2 - 30, L"⏪ 时间倒流中...");
    }

    // 无敌提示
    if (m_bInvincible)
    {
        memDC.SetTextColor(RGB(255, 100, 0));
        memDC.TextOutW(rect.Width() / 2 - 80, rect.Height() - 60, L"✨ 无敌状态 ✨");
    }

    // 胜利画面
    if (m_bGameWin)
    {
        CFont font;
        font.CreatePointFont(240, L"黑体");
        memDC.SelectObject(&font);
        memDC.SetTextColor(RGB(0, 150, 0));
        memDC.TextOutW(rect.Width() / 2 - 80, rect.Height() / 2 - 20, L"通关！");
        font.DeleteObject();

        CFont font2;
        font2.CreatePointFont(120, L"黑体");
        memDC.SelectObject(&font2);
        memDC.TextOutW(rect.Width() / 2 - 100, rect.Height() / 2 + 30, L"按 R 重新开始");
    }

    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
}
void CTimeRewindGameDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TIMER_GAME)
    {
        if (m_bGameWin)
        {
            CDialogEx::OnTimer(nIDEvent);
            return;
        }

        // 无敌计时
        if (m_bInvincible)
        {
            m_invincibleTimer--;
            if (m_invincibleTimer <= 0)
            {
                m_bInvincible = FALSE;
            }
        }

        if (!m_bRewinding)
        {
            // 移动陷阱
            for (int i = 0; i < m_trapCount; i++)
            {
                m_traps[i].x += (int)m_traps[i].vx;
                m_traps[i].y += (int)m_traps[i].vy;

                if (m_traps[i].x < 20 || m_traps[i].x > 530)
                    m_traps[i].vx = -m_traps[i].vx;
                if (m_traps[i].y < 20 || m_traps[i].y > 430)
                    m_traps[i].vy = -m_traps[i].vy;
            }

            // 记录历史
            m_historyX[m_historyHead] = m_playerX;
            m_historyY[m_historyHead] = m_playerY;
            m_historyHead = (m_historyHead + 1) % MAX_HISTORY;
            if (m_historyCount < MAX_HISTORY) m_historyCount++;

            CheckCollision();
            Invalidate(FALSE);
        }
    }
    CDialogEx::OnTimer(nIDEvent);
}

void CTimeRewindGameDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == 'R' || nChar == 'r')
    {
        ResetGame();
        Invalidate(FALSE);
        return;
    }

    if (nChar == VK_SPACE)
    {
        if (!m_bRewinding && m_historyCount > 0 && m_rewindEnergy >= m_rewindCost)
        {
            m_bRewinding = TRUE;
            int steps = min(20, m_historyCount);
            for (int i = 0; i < steps; i++)
            {
                int prevIndex = (m_historyHead - 1 + MAX_HISTORY) % MAX_HISTORY;
                m_playerX = m_historyX[prevIndex];
                m_playerY = m_historyY[prevIndex];
                m_historyHead = prevIndex;
                m_historyCount--;
            }
            m_rewindEnergy -= m_rewindCost;
            m_bRewinding = FALSE;
            Invalidate(FALSE);
            BeepSound(600);
        }
        return;
    }

    if (m_bGameWin) return;

    int step = 8;
    if (nChar == VK_LEFT)  m_playerX -= step;
    if (nChar == VK_RIGHT) m_playerX += step;
    if (nChar == VK_UP)    m_playerY -= step;
    if (nChar == VK_DOWN)  m_playerY += step;

    if (m_playerX < 20)  m_playerX = 20;
    if (m_playerX > 530) m_playerX = 530;
    if (m_playerY < 20)  m_playerY = 20;
    if (m_playerY > 430) m_playerY = 430;

    Invalidate(FALSE);
    CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTimeRewindGameDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CDialogEx::OnActivate(nState, pWndOther, bMinimized);
    if (nState != WA_INACTIVE) SetFocus();
}

BOOL CTimeRewindGameDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
        return TRUE;
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}