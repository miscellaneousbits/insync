#pragma once

class CVssHelper
{
public:
    CVssHelper();
    bool Initialize();
    void Finalize();
    bool AddDriveToSnapshotSet(TCHAR);
    bool PrepareForBackup();
    bool DriveName(TCHAR, CString &name);

private:
    // Type definitions
    typedef struct volumeEntryType {
        VSS_ID id;
        CString deviceName;
    } volumeEntryType;

    typedef map<TCHAR, volumeEntryType> VolMap;
    typedef VolMap::iterator VolMapIterator;

    void ClearVolumeMap();
    bool Wait();
    void ReleaseComponents();

    IVssBackupComponents *m_components;
    IVssAsync *m_pAsync = nullptr;
    VSS_ID m_snapshotSetId;
    VolMap m_vMap;
};
