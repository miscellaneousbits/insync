#pragma once

class CInsyncListbox : public CListBox
{
    DECLARE_DYNAMIC(CInsyncListbox)

public:
    CInsyncListbox();
    virtual ~CInsyncListbox();
    virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    bool GetRunning(int nIndex) const;
    void SetRunning(int nIndex, bool bCheck, CString s);
    void ResetContent();
    USHORT GetCheckboxHeight();

protected:
    DECLARE_MESSAGE_MAP()

private:
    int m_cxSpace = 0;
    int m_cySpace = 0;
    int m_cyItem = 0;
    int m_extra = 0;
    HTHEME m_hTheme;
    CFont m_boldFont;
    CFont m_boldItalicFont;
    CFont m_normalFont;
};


