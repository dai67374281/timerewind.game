// TimeRewindGame1.cpp
#include "pch.h"
#include "framework.h"
#include "TimeRewindGame1.h"
#include "TimeRewindGame1Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTimeRewindGame1App, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CTimeRewindGame1App::CTimeRewindGame1App() {}

CTimeRewindGame1App theApp;

BOOL CTimeRewindGame1App::InitInstance()
{
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();
    AfxEnableControlContainer();

    CTimeRewindGame1Dlg dlg;
    m_pMainWnd = &dlg;
    dlg.DoModal();

    return FALSE;
}