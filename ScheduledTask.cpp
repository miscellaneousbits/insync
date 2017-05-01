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

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Convenience macros

#define DDS_CST_EVERYMONTH \
    (TASK_JANUARY  | TASK_FEBRUARY | TASK_MARCH     | \
    TASK_APRIL    | TASK_MAY      | TASK_JUNE      | \
    TASK_JULY     | TASK_AUGUST   | TASK_SEPTEMBER | \
    TASK_OCTOBER  | TASK_NOVEMBER | TASK_DECEMBER)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CScheduledTask::CScheduledTask() :
    m_eFreq(freqWeekly),
    m_autoDelete(true),
    m_hasEndDate(false) {
    memset(&m_timeStart, 0, sizeof(SYSTEMTIME));
    memset(&m_timeEnd, 0, sizeof(SYSTEMTIME));
}

//////////////////////////////////////////////////////////////////////
// Data accessor functions

bool CScheduledTask::GetStartDateTime(CTime &time) const {
    bool bRet = FALSE;
    if (m_timeStart.wYear) {
        time = m_timeStart;
        bRet = TRUE;
    }
    return bRet;
}

void CScheduledTask::SetStartDateTime(const CTime &time) {
    time.GetAsSystemTime(m_timeStart);
    m_timeStart.wSecond = 0;
    m_timeStart.wMilliseconds = 0;
}

bool CScheduledTask::GetEndDate(CTime &time) const {
    bool bRet = FALSE;
    if (m_timeEnd.wYear) {
        time = m_timeEnd;
        bRet = TRUE;
    }
    return bRet;
}

void CScheduledTask::SetEndDate(const CTime &time) {
    ZeroMemory(&m_timeEnd, sizeof(SYSTEMTIME));
    m_timeEnd.wYear = (WORD)time.GetYear();
    m_timeEnd.wMonth = (WORD)time.GetMonth();
    m_timeEnd.wDay = (WORD)time.GetDay();
}

//////////////////////////////////////////////////////////////////////////
// Other functions

HRESULT CScheduledTask::SaveTask(LPCTSTR szTaskName, bool bFailIfExists) const {
    HRESULT hr;
    ITaskScheduler *pISched = NULL;
    IUnknown *pIUnk = NULL;
    IPersistFile *pIFile = NULL;
    ITask *pITask = NULL;
    ITaskTrigger *pITaskTrig = NULL;
    TASK_TRIGGER rTrigger;
    DWORD dwTaskFlags;
    WORD wTrigNumber;

    USES_CONVERSION;

    do {
        hr = ::CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER,
                                IID_ITaskScheduler, (LPVOID *)&pISched);
        if (HRFAILED(hr)) {
            break;
        }
        if (pISched == NULL) {
            hr = E_UNEXPECTED;
            break;
        }

        hr = pISched->NewWorkItem(T2COLE(szTaskName), CLSID_CTask, IID_ITask, &pIUnk);
        if (HRFAILED(hr)) {
            if (HRESULT_CODE(hr) == ERROR_FILE_EXISTS) {
                if (bFailIfExists) {
                    hr = HRESULT_FROM_WIN32(ERROR_FILE_EXISTS);
                    break;
                }
                else {
                    hr = CScheduledTask::DeleteTask(szTaskName);
                    if (HRFAILED(hr)) {
                        break;
                    }
                    hr = pISched->NewWorkItem(T2COLE(szTaskName), CLSID_CTask, IID_ITask, &pIUnk);
                    if (HRFAILED(hr)) {
                        break;
                    }
                }
            }
            else {
                break;
            }
        }
        if (pIUnk == NULL) {
            hr = E_UNEXPECTED;
            break;
        }
        hr = pIUnk->QueryInterface(IID_ITask, (LPVOID *)&pITask);
        if (HRFAILED(hr)) {
            break;
        }
        if (pITask == NULL) {
            hr = E_UNEXPECTED;
            break;
        }
        hr = pITask->SetApplicationName(T2COLE((LPCTSTR)m_sProgramPath));
        if (HRFAILED(hr)) {
            break;
        }
        if (m_sParameters.GetLength() > 0) {
            hr = pITask->SetParameters(T2COLE((LPCTSTR)m_sParameters));
            if (HRFAILED(hr)) {
                break;
            }
        }
        hr = pITask->SetAccountInformation(T2COLE((LPCTSTR)m_sAccount), T2COLE((LPCTSTR)m_sPassword));
        if (HRFAILED(hr)) {
            break;
        }
        if (m_sComment.GetLength() > 0) {
            hr = pITask->SetComment(T2COLE((LPCTSTR)m_sComment));
            if (HRFAILED(hr)) {
                break;
            }
        }
        hr = pITask->SetWorkItemData((WORD)((m_parameters.GetLength() + 1)
                                            * sizeof(TCHAR)), (BYTE *)(LPCTSTR)m_parameters);
        if (HRFAILED(hr)) {
            break;
        }
        dwTaskFlags = 0;
        if (m_autoDelete) {
            dwTaskFlags |= TASK_FLAG_DELETE_WHEN_DONE;
        }
        hr = pITask->SetFlags(dwTaskFlags);
        if (HRFAILED(hr)) {
            break;
        }
        hr = pITask->CreateTrigger(&wTrigNumber, &pITaskTrig);
        if (HRFAILED(hr)) {
            break;
        }
        if (pITaskTrig == NULL) {
            hr = E_UNEXPECTED;
            break;
        }
        ZeroMemory(&rTrigger, sizeof(TASK_TRIGGER));
        rTrigger.cbTriggerSize = sizeof(TASK_TRIGGER);
        rTrigger.wBeginYear = m_timeStart.wYear;
        rTrigger.wBeginMonth = m_timeStart.wMonth;
        rTrigger.wBeginDay = m_timeStart.wDay;
        rTrigger.wStartHour = m_timeStart.wHour;
        rTrigger.wStartMinute = m_timeStart.wMinute;
        if (m_hasEndDate) {
            rTrigger.rgFlags = TASK_TRIGGER_FLAG_HAS_END_DATE;
            rTrigger.wEndYear = m_timeEnd.wYear;
            rTrigger.wEndMonth = m_timeEnd.wMonth;
            rTrigger.wEndDay = m_timeEnd.wDay;
        }
        switch (m_eFreq) {
        case freqOnce:
            rTrigger.TriggerType = TASK_TIME_TRIGGER_ONCE;
            break;

        case freqStartup:
            rTrigger.TriggerType = TASK_EVENT_TRIGGER_AT_SYSTEMSTART;
            break;

        case freqDaily:
            rTrigger.TriggerType = TASK_TIME_TRIGGER_DAILY;
            rTrigger.Type.Daily.DaysInterval = 1;
            break;

        case freqWeekly:
            rTrigger.TriggerType = TASK_TIME_TRIGGER_WEEKLY;
            rTrigger.Type.Weekly.rgfDaysOfTheWeek = GetDayOfWeekFlag(m_timeStart);
            rTrigger.Type.Weekly.WeeksInterval = 1;
            break;

        case freqMonthly:
            rTrigger.TriggerType = TASK_TIME_TRIGGER_MONTHLYDATE;
            rTrigger.Type.MonthlyDate.rgfDays = 1 << (m_timeStart.wDay - 1);
            rTrigger.Type.MonthlyDate.rgfMonths = DDS_CST_EVERYMONTH;
            break;
        }
        hr = pITaskTrig->SetTrigger(&rTrigger);
        if (HRFAILED(hr)) {
            break;
        }
        hr = pITask->QueryInterface(IID_IPersistFile, (LPVOID *)&pIFile);
        if (HRFAILED(hr)) {
            break;
        }
        if (pIFile == NULL) {
            hr = E_UNEXPECTED;
            break;
        }
        hr = pIFile->Save(NULL, FALSE);
    }
    while (false);
    if (pIUnk) {
        pIUnk->Release();
    }
    if (pIFile) {
        pIFile->Release();
    }
    if (pITaskTrig) {
        pITaskTrig->Release();
    }
    if (pITask) {
        pITask->Release();
    }
    if (pISched) {
        pISched->Release();
    }
    return hr;
}

HRESULT CScheduledTask::DeleteTask(LPCTSTR szTaskName) {
    HRESULT hr;
    ITaskScheduler *pISched = NULL;

    USES_CONVERSION;

    if (0 == lstrlen(szTaskName)) {
        return E_FAIL;
    }
    hr = ::CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskScheduler, (LPVOID *)&pISched);
    if (HRFAILED(hr)) {
        return hr;
    }
    if (pISched == NULL) {
        return E_UNEXPECTED;
    }
    hr = pISched->Delete(T2COLE(szTaskName));
    if (pISched) {
        pISched->Release();
    }
    return hr;
}

WORD CScheduledTask::GetDayOfWeekFlag(const CTime &time) const {
    static WORD s_wDayFlags[] = { 0, TASK_SUNDAY, TASK_MONDAY, TASK_TUESDAY, TASK_WEDNESDAY, TASK_THURSDAY,
                                  TASK_FRIDAY, TASK_SATURDAY
                                };
    WORD wRet = 0;
    int nDayOfWeek = time.GetDayOfWeek();
    wRet = s_wDayFlags[nDayOfWeek];
    return wRet;
}

HRESULT CScheduledTask::LoadTask(LPCTSTR name) {
    HRESULT hr;
    ITaskScheduler *pISched = NULL;
    ITaskScheduler *pITS = NULL;
    ITask *pITask = NULL;
    ITaskTrigger *pITaskTrig = NULL;
    TASK_TRIGGER rTrigger;

    USES_CONVERSION;

    do {
        hr = CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskScheduler, (LPVOID *)&pITS);
        if (HRFAILED(hr)) {
            break;
        }
        if (pITS == NULL) {
            hr = E_UNEXPECTED;
            break;
        }
        hr = pITS->Activate(T2COLE(name), IID_ITask, (IUnknown **)&pITask);
        if (HRFAILED(hr)) {
            break;
        }
        if (pITask == NULL) {
            hr = E_UNEXPECTED;
            break;
        }

        LPWSTR lpwsz;
        hr = pITask->GetApplicationName(&lpwsz);
        if (HRFAILED(hr)) {
            break;
        }
        m_sProgramPath = OLE2CT(lpwsz);
        CoTaskMemFree(lpwsz);
        hr = pITask->GetParameters(&lpwsz);
        if (HRFAILED(hr)) {
            break;
        }
        m_sParameters = OLE2CT(lpwsz);
        CoTaskMemFree(lpwsz);
        hr = pITask->GetAccountInformation(&lpwsz);
        if (HRFAILED(hr)) {
            m_sAccount = "";
        }
        else {
            m_sAccount = OLE2CT(lpwsz);
            CoTaskMemFree(lpwsz);
        }
        m_sPassword = kNullString;
        hr = pITask->GetComment(&lpwsz);
        if (HRFAILED(hr)) {
            break;
        }
        m_sComment = OLE2CT(lpwsz);
        CoTaskMemFree(lpwsz);

        WORD nBytes;
        BYTE *bytes;
        hr = pITask->GetWorkItemData(&nBytes, &bytes);
        if (HRFAILED(hr)) {
            break;
        }
        if (nBytes) {
            m_parameters = (LPCTSTR)bytes;
            CoTaskMemFree(bytes);
        }

        DWORD dwTaskFlags;
        hr = pITask->GetFlags(&dwTaskFlags);
        if (HRFAILED(hr)) {
            break;
        }
        m_autoDelete = (dwTaskFlags & TASK_FLAG_DELETE_WHEN_DONE) != 0;
        hr = pITask->GetTrigger(0, &pITaskTrig);
        if (HRFAILED(hr)) {
            break;
        }
        if (pITaskTrig == NULL) {
            hr = E_UNEXPECTED;
            break;
        }
        ZeroMemory(&rTrigger, sizeof(TASK_TRIGGER));
        rTrigger.cbTriggerSize = sizeof(TASK_TRIGGER);
        hr = pITaskTrig->GetTrigger(&rTrigger);
        if (HRFAILED(hr)) {
            break;
        }
        m_timeStart.wYear = rTrigger.wBeginYear;
        m_timeStart.wMonth = rTrigger.wBeginMonth;
        m_timeStart.wDay = rTrigger.wBeginDay;
        m_timeStart.wHour = rTrigger.wStartHour;
        m_timeStart.wMinute = rTrigger.wStartMinute;
        m_hasEndDate = (rTrigger.rgFlags & TASK_TRIGGER_FLAG_HAS_END_DATE) != 0;
        if (m_hasEndDate) {
            m_timeEnd.wYear = rTrigger.wEndYear;
            m_timeEnd.wMonth = rTrigger.wEndMonth;
            m_timeEnd.wDay = rTrigger.wEndDay;
        }
        else {
            m_timeEnd.wYear = 0;
            m_timeEnd.wMonth = 0;
            m_timeEnd.wDay = 0;
        }
        switch (rTrigger.TriggerType) {
        case TASK_TIME_TRIGGER_ONCE:
            m_eFreq = freqOnce;
            break;
        case TASK_TIME_TRIGGER_DAILY:
            m_eFreq = freqDaily;
            break;
        case TASK_TIME_TRIGGER_WEEKLY:
            m_eFreq = freqWeekly;
            break;
        case TASK_TIME_TRIGGER_MONTHLYDATE:
            m_eFreq = freqMonthly;
            break;
        case TASK_EVENT_TRIGGER_AT_SYSTEMSTART:
            m_eFreq = freqStartup;
            break;
        default:
            m_eFreq = freqOnce;
            break;
        }
    }
    while (false);

    // Clean up all the interfaces.
    if (pITS) {
        pITS->Release();
    }
    if (pITaskTrig) {
        pITaskTrig->Release();
    }
    if (pITask) {
        pITask->Release();
    }
    if (pISched) {
        pISched->Release();
    }

    return hr;
}

HRESULT CScheduledTask::EnumerateTasks(WORD &eventCount, CStringArray &names) {
    HRESULT hr = S_OK;
    eventCount = 0;
    ITaskScheduler *pITS = NULL;
    hr = CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskScheduler, (LPVOID *)&pITS);
    if (HRFAILED(hr)) {
        return hr;
    }
    if (pITS == NULL) {
        return E_UNEXPECTED;
    }
    IEnumWorkItems *pIEnum;
    hr = pITS->Enum(&pIEnum);
    pITS->Release();
    if (HRFAILED(hr)) {
        return hr;
    }
    if (pIEnum == NULL) {
        return E_UNEXPECTED;
    }
    LPWSTR *lpwszNames;
    hr = pIEnum->Next(1, &lpwszNames, NULL);
    while (HRSUCCEEDED(hr)) {
        eventCount++;
        hr = pIEnum->Next(1, &lpwszNames, NULL);
    }
    hr = pIEnum->Reset();
    if (HRFAILED(hr)) {
        pIEnum->Release();
        return hr;
    }
    names.SetSize(eventCount);
    int n = 0;
    hr = pIEnum->Next(1, &lpwszNames, NULL);
    while (HRSUCCEEDED(hr) && (n < eventCount)) {
        CString name(lpwszNames[0]);
        names.SetAt(n, name);
        n++;
        CoTaskMemFree(lpwszNames[0]);
        CoTaskMemFree(lpwszNames);
        hr = pIEnum->Next(1, &lpwszNames, NULL);
    }
    pIEnum->Release();
    return S_OK;
}

bool CScheduledTask::IsInsyncTask() const {
    return m_sProgramPath.Right(10).CompareNoCase(_T("Insync.exe")) == 0;
}
