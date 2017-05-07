#pragma once

class CInSyncCheck : public CButton
{
    DECLARE_DYNAMIC(CInSyncCheck)

public:
    void Init(CInsyncToolTipCtrl *tipCtl, CString tip);

protected:
    DECLARE_MESSAGE_MAP()

};


