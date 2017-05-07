#include "stdafx.h"

void CInsyncComboBox::SetSelectedFromItemData(DWORD_PTR data)
{
    int ix, ixe;
    ixe = GetCount();

    for (ix = 0; ix < ixe; ix++) {
        if (data == GetItemData(ix)) {
            SetCurSel(ix);
            return;
        }
    }

    SetCurSel(0);
}

DWORD_PTR CInsyncComboBox::GetSelectedItemData()
{
    int ix = GetCurSel();

    if (ix != CB_ERR) {
        return GetItemData(ix);
    }

    return 0;
}
