#pragma once

class CDirListBox : public CListBox
{
public:
    int AddString(LPCTSTR);
    int DeleteString(UINT);
    void ResetContent();

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnDropFiles(HDROP hDropInfo);

private:
    void SetHScroll();
};
