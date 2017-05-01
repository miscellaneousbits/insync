#pragma once


// CInSyncButton

class CInsyncToolTipCtrl;

class CInSyncButton : public CMFCButton {
    DECLARE_DYNAMIC(CInSyncButton)

public:
    CInSyncButton();
    void CInSyncButton::Init(CInsyncToolTipCtrl *tipCtl, CString tip);
    void CInSyncButton::Init(UINT id, CInsyncToolTipCtrl *tipCtl, CString tip);

protected:
    DECLARE_MESSAGE_MAP()

private:
    HICON m_icon;
public:
    afx_msg void OnDestroy();
};


