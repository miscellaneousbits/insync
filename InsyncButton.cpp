#include "stdafx.h"
#include "InsyncButton.h"

// CInSyncButton

IMPLEMENT_DYNAMIC(CInSyncButton, CMFCButton)

BEGIN_MESSAGE_MAP(CInSyncButton, CMFCButton)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

CInSyncButton::CInSyncButton()
{
    m_icon = NULL;
    EnableWindowsTheming();
}

void CInSyncButton::Init(CInsyncToolTipCtrl *tipCtl, CString tip)
{
    if (tipCtl->isOK()) {
        tipCtl->AddTool(this, tip);
    }
}

void CInSyncButton::Init(UINT id, CInsyncToolTipCtrl *tipCtl, CString tip)
{
    m_icon = (HICON)CMainDlg::Singleton().LoadResourceIcon(id, 16, 16);
    SetIcon(m_icon);
    Init(tipCtl, tip);
}

void CInSyncButton::OnDestroy()
{
    if (m_icon) {
        DestroyIcon(m_icon);
    }

    CMFCButton::OnDestroy();
}
