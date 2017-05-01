// InsyncListbox.cpp : implementation file
//

#include "stdafx.h"
#include "InsyncListbox.h"


// CInsyncListbox

IMPLEMENT_DYNAMIC(CInsyncListbox, CListBox)

CInsyncListbox::CInsyncListbox() :
    m_cyItem(0),
    m_extra(0) {
    m_hTheme = OpenThemeData(m_hWnd, _T("Button"));
}

CInsyncListbox::~CInsyncListbox() {
    if (m_hTheme) {
        CloseThemeData(m_hTheme);
    }
    m_normalFont.DeleteObject();
    m_boldFont.DeleteObject();
    m_boldItalicFont.DeleteObject();
}

BEGIN_MESSAGE_MAP(CInsyncListbox, CListBox)
    ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()

void CInsyncListbox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
    CJobNode *jobNode = (CJobNode *)lpDrawItemStruct->itemData;
    CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    bool fSelected = ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0);
    DWORD xOffset = m_cySpace;

    //pDC->FillRect(&lpDrawItemStruct->rcItem, m_eraseBrush);

    if (m_hTheme) {
        xOffset += m_cyItem + m_cxSpace;
    }
    if (((LONG)(lpDrawItemStruct->itemID) >= 0) && (lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT))) {
        CString strText;
        bool fRunning;
        if (jobNode) {
            strText = jobNode->m_name;
            fRunning = GetRunning(lpDrawItemStruct->itemID);
        }
        else {
            GetText(lpDrawItemStruct->itemID, strText);
            fRunning = false;
        }

        int length = strText.GetLength();
        pDC->SelectObject(fRunning ? m_boldFont : m_normalFont);
        CSize size = pDC->GetOutputTextExtent(strText, length);
        LONG totalCx = size.cx;
        if (fRunning) {
            length += jobNode->m_runState.GetLength();
            pDC->SelectObject(m_boldItalicFont);
            totalCx += pDC->GetOutputTextExtent(jobNode->m_runState, jobNode->m_runState.GetLength()).cx;
        }
        if ((totalCx + m_cySpace) > lpDrawItemStruct->rcItem.right) {
            int extra = (totalCx + xOffset) - lpDrawItemStruct->rcItem.right;
            if (extra > m_extra) {
                m_extra = extra;
                if (m_extra) {
                    SetHorizontalExtent(m_extra + lpDrawItemStruct->rcItem.right + xOffset);
                }
            }
        }

        if (!m_hTheme) {
            if (fSelected) {
                pDC->SetTextColor(GetSysColor(IsWindowEnabled() ? COLOR_HIGHLIGHTTEXT : (fRunning ? COLOR_HIGHLIGHTTEXT :
                                              COLOR_GRAYTEXT)));
                pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
            }
            else {
                pDC->SetTextColor(GetSysColor(IsWindowEnabled() ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));
                pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
            }
        }

        pDC->SelectObject(fRunning ? m_boldFont : m_normalFont);

        RECT rect = lpDrawItemStruct->rcItem;
        rect.left += xOffset;
        rect.top += m_cySpace;
        rect.bottom = rect.top + m_cyItem;
        pDC->SetTextAlign(TA_TOP);

        pDC->ExtTextOut(rect.left, rect.top, ETO_OPAQUE, &rect,
                        strText,
                        strText.GetLength(),
                        NULL);
        if (fRunning) {
            pDC->SelectObject(m_boldItalicFont);
            rect.left += size.cx;
            pDC->ExtTextOut(rect.left, rect.top, ETO_OPAQUE, &rect,
                            jobNode->m_runState,
                            jobNode->m_runState.GetLength(),
                            NULL);
        }

        if (m_hTheme) {
            rect = lpDrawItemStruct->rcItem;
            rect.left += m_cxSpace;
            rect.right = rect.left + m_cyItem;
            rect.top += m_cySpace;
            rect.bottom = rect.top + m_cyItem;
            ::DrawThemeBackground(m_hTheme,
                                  lpDrawItemStruct->hDC,
                                  BP_CHECKBOX,
                                  fSelected ? CBS_CHECKEDNORMAL : CBS_UNCHECKEDNORMAL,
                                  &rect,
                                  NULL);
        }
    }

    if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0) {
        pDC->DrawFocusRect(&(lpDrawItemStruct->rcItem));
    }
}

bool CInsyncListbox::GetRunning(int nIndex) const {
    CJobNode *jobNode = (CJobNode *)GetItemDataPtr(nIndex);
    if (jobNode) {
        return jobNode->m_running;
    }
    return false;
}

void CInsyncListbox::SetRunning(int nIndex, bool bCheck, CString s) {
    CJobNode *jobNode = (CJobNode *)GetItemDataPtr(nIndex);
    if (jobNode) {
        jobNode->m_running = bCheck;
        if (bCheck) {
            jobNode->m_runState = _T(" (") + s + _T(")");
        }
        CRect rect;
        GetItemRect(nIndex, rect);
        InvalidateRect(rect, false);
    }
}

void CInsyncListbox::ResetContent() {
    m_extra = 0;
    SetHorizontalExtent(0);
    CListBox::ResetContent();
}

void CInsyncListbox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) {
    if (m_cyItem == 0) {
        LOGFONT logFont;
        CDC *pDC = GetDC();
        if (!m_normalFont.CreatePointFont(90, _T("Segoe UI"), pDC)) {
            GetFont()->GetLogFont(&logFont);
            m_normalFont.CreateFontIndirect(&logFont);
        }
        //  Get log font for the given window
        m_normalFont.GetLogFont(&logFont);
        //  Change the weight to bold
        logFont.lfWeight = FW_BOLD;
        //  Set the bold font for the given window
        if (!m_boldFont.CreateFontIndirect(&logFont)) {
            m_normalFont.GetLogFont(&logFont);
            m_boldFont.CreateFontIndirect(&logFont);
        }
        logFont.lfItalic = TRUE;
        if (!m_boldItalicFont.CreateFontIndirect(&logFont)) {
            m_normalFont.GetLogFont(&logFont);
            m_boldItalicFont.CreateFontIndirect(&logFont);
        }
        pDC->SelectObject(m_boldFont);
        TEXTMETRIC txm;
        pDC->GetTextMetrics(&txm);
        m_cyItem = txm.tmHeight;
        m_cySpace = m_cyItem / 8;
        if (m_cySpace < 1) {
            m_cySpace = 1;
        }
        m_cxSpace = m_cyItem / 4;
        if (m_cxSpace < 2) {
            m_cxSpace = 2;
        }
    }
    lpMeasureItemStruct->itemHeight = m_cyItem + m_cySpace + m_cySpace;
}
