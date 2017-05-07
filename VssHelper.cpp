#include "StdAfx.h"

CVssHelper::CVssHelper()
{
    m_components = NULL;
    m_snapshotSetId = GUID_NULL;
}

// Local helper functions
bool CVssHelper::Wait()
{
    HRESULT hr;
    m_pAsync->Wait(INFINITE);
    int reserved = 0;

    if (!SUCCEEDED(m_pAsync->QueryStatus(&hr, &reserved))) {
        return false;
    }

    return hr == (HRESULT)VSS_S_ASYNC_FINISHED;
}

void CVssHelper::ClearVolumeMap()
{
    m_vMap.clear();
}

static boolean ValidDriveLetter(TCHAR &dev)
{
    return ((dev >= _T('A')) && (dev <= _T('Z')));
}

void CVssHelper::ReleaseComponents()
{
    if (m_components) {
        m_components->Release();
        m_components = NULL;
    }
}

bool CVssHelper::Initialize()
{
    m_snapshotSetId = GUID_NULL;
    m_components = NULL;

    do {
        if (FAILED(CoInitialize(NULL))) {
            break;
        }

        if (FAILED(CreateVssBackupComponents(&m_components))) {
            break;
        }

        if (FAILED(m_components->InitializeForBackup())) {
            break;
        }

        if (FAILED(m_components->SetBackupState(false, false, VSS_BT_FULL, false))) {
            break;
        }

        if (FAILED(m_components->StartSnapshotSet(&m_snapshotSetId))) {
            break;
        }

        ClearVolumeMap();
        return true;
    } while (false);

    ReleaseComponents();
    return false;
}

void CVssHelper::Finalize()
{
    if (!m_components) {
        return;
    }

    if (SUCCEEDED(m_components->BackupComplete(&m_pAsync))) {
        Wait();
    }

    if (m_snapshotSetId != GUID_NULL) {
        VSS_ID nonDeleted = GUID_NULL;
        LONG n = 0;
        m_components->DeleteSnapshots(m_snapshotSetId, VSS_OBJECT_SNAPSHOT_SET, true, &n, &nonDeleted);
        m_snapshotSetId = GUID_NULL;
    }

    ReleaseComponents();
    ClearVolumeMap();
}

bool CVssHelper::AddDriveToSnapshotSet(TCHAR device)
{
    if (!(m_components && ValidDriveLetter(device))) {
        return false;
    }

    if (m_vMap.find(device) != m_vMap.end()) {
        return true;
    }

    volumeEntryType v;
    TCHAR d[4] = { device, _T(':'), _T('\\'), 0 };

    if (FAILED(m_components->AddToSnapshotSet(d, GUID_NULL, &v.id))) {
        return false;
    }

    m_vMap[device] = v;
    return true;
}

bool CVssHelper::DriveName(TCHAR device, CString &name)
{
    if (!(m_components && ValidDriveLetter(device))) {
        return false;
    }

    VolMapIterator ix = m_vMap.find(_totupper(device));

    if (ix != m_vMap.end()) {
        name = ix->second.deviceName;
        return true;
    }

    return false;
}

bool CVssHelper::PrepareForBackup()
{
    if (m_components) {
        if (FAILED(m_components->PrepareForBackup(&m_pAsync))) {
            return false;
        }

        if (!Wait()) {
            return false;
        }

        if (FAILED(m_components->DoSnapshotSet(&m_pAsync))) {
            return false;
        }

        if (!Wait()) {
            return false;
        }

        VolMapIterator ix;

        for (ix = m_vMap.begin(); ix != m_vMap.end(); ++ix) {
            volumeEntryType *vp = &(ix->second);

            if (vp->id != GUID_NULL) {
                VSS_SNAPSHOT_PROP prop;

                if (SUCCEEDED(m_components->GetSnapshotProperties(vp->id, &prop))) {
                    vp->deviceName = prop.m_pwszSnapshotDeviceObject;
                    vp->id = GUID_NULL;
                }
            }
        }

        return true;
    }

    return false;
}
