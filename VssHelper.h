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

class CVssHelper {
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
    IVssAsync *m_pAsync;
    VSS_ID m_snapshotSetId;
    VolMap m_vMap;
};
