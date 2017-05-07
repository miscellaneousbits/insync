#include "stdafx.h"

IMPLEMENT_DYNAMIC(CInSyncCheck, CButton)

BEGIN_MESSAGE_MAP(CInSyncCheck, CButton)
END_MESSAGE_MAP()

void CInSyncCheck::Init(CInsyncToolTipCtrl *tipCtl, CString tip)
{
    if (tipCtl->isOK()) {
        tipCtl->AddTool(this, tip);
    }
}

