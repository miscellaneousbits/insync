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
    int m_cxSpace;
    int m_cySpace;
    int m_cyItem;
    int m_extra;
    HTHEME m_hTheme;
    CFont m_boldFont;
    CFont m_boldItalicFont;
    CFont m_normalFont;
};


