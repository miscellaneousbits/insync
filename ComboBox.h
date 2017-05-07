#pragma once

class CInsyncComboBox : public CComboBox
{
public:
    void SetSelectedFromItemData(DWORD_PTR data);
    DWORD_PTR GetSelectedItemData();
};
