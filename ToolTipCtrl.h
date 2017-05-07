#pragma once

class CInsyncDlg;

class CInsyncToolTipCtrl : public CToolTipCtrl
{
    DECLARE_DYNAMIC(CInsyncToolTipCtrl)

public:
    CInsyncToolTipCtrl(CInsyncDlg *);
    bool isOK() const;
    virtual void RelayEvent(LPMSG lpMsg);

protected:
    DECLARE_MESSAGE_MAP()

private:
    bool m_isOK;
};


