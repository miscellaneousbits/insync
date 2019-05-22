#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CDirListBox

BEGIN_MESSAGE_MAP(CDirListBox, CListBox)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirListBox message handlers

int CDirListBox::AddString(LPCTSTR lpszItem)
{
    int rc = CListBox::AddString(lpszItem);
    SetHScroll();
    return rc;
}

int CDirListBox::DeleteString(UINT nIndex)
{
    int rc = CListBox::DeleteString(nIndex);
    SetHScroll();
    return rc;
}

void CDirListBox::ResetContent()
{
    CListBox::ResetContent();
    SetHScroll();
}

void CDirListBox::SetHScroll()
{
    CDC *pDC = GetDC();
    int dx = 0;
    int i = GetCount();

    while (--i >= 0) {
        CString str;
        GetText(i, str);
        CSize sz = pDC->GetTextExtent(str);

        if (sz.cx > dx) {
            dx = sz.cx;
        }
    }

    ReleaseDC(pDC);
    SetHorizontalExtent(dx);
}

void CDirListBox::OnDropFiles(HDROP hDropInfo)
{
    UINT nFiles = DragQueryFile(hDropInfo, 0xffffffff, NULL, 0);

    for (UINT fileIndex = 0; fileIndex < nFiles; fileIndex++) {
        UINT fSize = DragQueryFile(hDropInfo, fileIndex, NULL, 0);
        LPTSTR fName = new TCHAR[size_t(fSize) + 1];

        if (fName) {
            fSize = DragQueryFile(hDropInfo, fileIndex, fName, fSize + 1);
            bool deleteName = false;

            if (fSize) {
                if (PathIsDirectory(fName)) {
                    GetParent()->PostMessage(WM_USER_ADD_FOLDER, (WPARAM)this, (LPARAM)fName);
                } else {
                    CString msg(fName);
                    msg += _T(" is not a folder");
                    CMainDlg::Singleton().InSyncMessageBox(true, msg, MB_ICONERROR | MB_OK);
                    deleteName = true;
                }
            }

            if (deleteName) {
                delete [] fName;
            }
        }
    }

    CListBox::OnDropFiles(hDropInfo);
}
