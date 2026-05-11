// TimeRewindGame2.cpp
#include "pch.h"
#include "framework.h"
#include "TimeRewindGame2.h"
#include "TimeRewindGame2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTimeRewindGame2App, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CTimeRewindGame2App::CTimeRewindGame2App() {}

CTimeRewindGame2App theApp;

BOOL CTimeRewindGame2App::InitInstance()
{
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();
    AfxEnableControlContainer();

    CTimeRewindGame2Dlg dlg;
    m_pMainWnd = &dlg;
    dlg.DoModal();

    return FALSE;
}