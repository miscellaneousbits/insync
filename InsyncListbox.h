#pragma once
//****************************************************************************
//
// Copyright (c) 1998-2014 Dillobits Software Inc.
//
// This program is the proprietary software of Dillobits Software and/or its
// licensors, and may only be used, duplicated, modified or distributed
// pursuant to the terms and conditions of a separate, written license
// agreement executed between you and Dillobits Software (an "Authorized
// License"). Except as set forth in an Authorized License, Dillobits Software
// grants no license (express or implied), right to use, or waiver of any kind
// with respect to the Software, and Dillobits Software expressly reserves all
// rights in and to the Software and all intellectual property rights therein.
// IF YOU HAVE NOT AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
// SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY DILLOBITS SOFTWARE AND
// DISCONTINUE ALL USE OF THE SOFTWARE.
//
//****************************************************************************

class CInsyncListbox : public CListBox {
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


