#include "stdafx.h"

IMPLEMENT_DYNAMIC(CInsyncToolTipCtrl, CToolTipCtrl)

CInsyncToolTipCtrl::CInsyncToolTipCtrl(CInsyncDlg *dlg)
{
    m_isOK = false;

    if (Create(dlg)) {
        m_isOK = true;
        SetDelayTime(1500);
    }
}

BEGIN_MESSAGE_MAP(CInsyncToolTipCtrl, CToolTipCtrl)
END_MESSAGE_MAP()

void CInsyncToolTipCtrl::RelayEvent(LPMSG lpMsg)
{
    if (isOK() && !CMainDlg::Singleton().m_tipsOff) {
        CToolTipCtrl::RelayEvent(lpMsg);
    }
}

bool CInsyncToolTipCtrl::isOK() const
{
    return m_isOK;
}
