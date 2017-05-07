#pragma once

class CInsyncDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CInsyncDlg)

public:
    CInsyncDlg(DWORD idd, DWORD helpId, CWnd *pParent = NULL);   // standard constructor
    virtual ~CInsyncDlg();
    afx_msg void OnIdhelp();
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual BOOL OnInitDialog();
    HICON LoadResourceIcon(const UINT, const WORD, const WORD) const;

protected:
    virtual void DoDataExchange(CDataExchange *pDX) = 0;
    CInsyncToolTipCtrl *m_pToolTip;
    bool BrowseForFolder(CString &, CWnd *, LPCTSTR);

    DECLARE_MESSAGE_MAP()

private:
    DWORD m_helpId;
    int m_idd;
};

class CDlgOccManager : public COccManager
{
public:
    const DLGTEMPLATE *CDlgOccManager::PreCreateDialog(_AFX_OCC_DIALOG_INFO *, const DLGTEMPLATE *);
};

