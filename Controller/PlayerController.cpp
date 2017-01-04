#include "StdAfx.h"
#include "PlayerController.h"
#include "Controller.h"
#include "Utilities/StringUtility.h"

PlayerController PlayerController::s_Instance;

void CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    m_CS.Lock();

    PlayerController* pPlayerController = (PlayerController*)lpParam;

    if (lpParam == NULL)
    {
        TRACE0("No palyer controller class send to timer.\n");
        m_CS.Unlock();
        return;
    }

    if(pPlayerController->m_bConnected && pPlayerController->m_hPipe != INVALID_HANDLE_VALUE)
    {
        DWORD ReadNumber = 0;
        DWORD ErrorCode = 0;

        if(pPlayerController->m_bInitalized)
        {
            if(GetOverlappedResult(pPlayerController->m_hPipe, &pPlayerController->m_AsycStructure, &ReadNumber, FALSE) == FALSE)
            {
                ErrorCode = GetLastError();
                TRACE1("Get Overlapped Result failed (%d).\n", ErrorCode);
                if(ErrorCode != ERROR_IO_PENDING && ErrorCode != ERROR_IO_INCOMPLETE)
                {
                    pPlayerController->CloseConnection();
                    m_CS.Unlock();
                    return;
                }

                m_CS.Unlock();
                return;
            }

            if(ReadNumber > 0)
            {
                pPlayerController->m_szReadBuffer[ReadNumber * sizeof(TCHAR)] = '\0';
                TRACE1("Receive Command : %s.\n", pPlayerController->m_szReadBuffer);
                if (_tcscmp(pPlayerController->m_szReadBuffer, _T("0010")) == 0)
                {
                    pPlayerController->m_bSucceed = TRUE;
                }
            }
        }
        if (ReadFile(pPlayerController->m_hPipe, pPlayerController->m_szReadBuffer, sizeof(pPlayerController->m_szReadBuffer),
            &pPlayerController->m_nReadNumber, &pPlayerController->m_AsycStructure) == FALSE)
        {
            ErrorCode = GetLastError();
            TRACE1("Read player pipe failed (%d).\n", ErrorCode);
            if(ErrorCode != ERROR_IO_PENDING && ErrorCode != ERROR_IO_INCOMPLETE)
            {
                pPlayerController->CloseConnection();
                m_CS.Unlock();
                return;
            }
        }
        pPlayerController->m_bInitalized = TRUE;
    }

    m_CS.Unlock();
}


PlayerController::PlayerController()
{
    m_hPlayerProcess    = NULL;
    m_hPipe             = NULL;
    m_bConnected        = FALSE;
    m_bInitalized       = FALSE;
    m_szCommand         = _T("");
    m_hTimer            = NULL;
    m_hTimerQueue       = NULL;
    m_bSucceed          = FALSE;

}

PlayerController::~PlayerController()
{
    Terminate();

    if (m_hPlayerProcess != NULL)
    {
        TerminateProcess(m_hPlayerProcess, 1);
    }

}

HRESULT PlayerController::StartPlayer(const int nWidth, const int nHeight)
{
    HRESULT hr = S_OK;
    CString szCmdLine;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    TString MediaLocation = GetMediaLibraryInterface->GetMediaLibraryRoot();

#ifndef SERVER_EDITION
    MediaLocation = g_MediaServer().GetServerHomeDir();
#endif
	 CControllerSettings controllerSetting(GetConfig);
	 map<CString,CString> & LanMap = GetLanguageNameMap();
	 CString lanLocName = _T("English");
	 if(LanMap.size())
	 {
		 //lanLocName = ChangeEngToLocLanName(controllerSetting.m_LanguageSetting.m_SelLanguage);
		 lanLocName = controllerSetting.m_LanguageSetting.m_SelLanguage;
	 }
	 szCmdLine.Format(_T("Player.exe --playertype=playerw --fitscreen --previewwidth=%d --previewheight=%d --pipename=\\\\.\\Pipe\\S3SignagePlayer%s --medialibrary=%s --curlanguage=%s --logpath=%s"), nWidth, nHeight, m_szPipePostfix.GetString(),
        MediaLocation.c_str(), lanLocName, m_szLogPath.GetString());

    if (m_hTimerQueue != NULL)
    {
        StopPlayer();
    }

    if (CreateProcess(NULL, (LPWSTR)(LPCWSTR)szCmdLine, NULL, NULL,
        FALSE, 0, NULL, NULL, &si, &pi))
    {
        m_hPlayerProcess = pi.hProcess;
    }
    else
    {
        TRACE1("CreateProcess failed (%d).\n", GetLastError());
        return E_FAIL;
    }

    if (FAILED(StartListenPipe()))
    {
        TRACE1("Start listen player pipe failed (%d).\n", GetLastError());
        TerminateProcess(m_hPlayerProcess, 1);
        m_hPlayerProcess = NULL;
        return E_FAIL;
    }

    return hr;
}

HRESULT PlayerController::StopPlayer()
{
    HRESULT hr = S_OK;

    if (NULL == m_hPlayerProcess)
    {
        return S_OK;
    }

    Terminate();

    int Counter = 0;
    while(IsPlayerRunning() && Counter < 10)
    {
        Sleep(100);
        Counter ++;
    }

    if(IsPlayerRunning())
    {
        if (!TerminateProcess(m_hPlayerProcess, 1))
        {
            hr = E_FAIL;
        }
    }

    CloseHandle(m_hPlayerProcess);
    m_hPlayerProcess = NULL;

    return hr;
}

BOOL PlayerController::IsPlayerRunning()
{
    BOOL bRunning = FALSE;
    DWORD exitCode;
    if (m_hPlayerProcess != NULL)
    {
        if (GetExitCodeProcess(m_hPlayerProcess, &exitCode))
        {
            if (exitCode == STILL_ACTIVE)
            {
                bRunning = TRUE;
            }
        }
    }

    return bRunning;
}

HRESULT PlayerController::Load(CString szFileName)
{
    if (!m_bConnected)
    {
        return E_UNEXPECTED;
    }

    m_bSucceed = FALSE;
    CString CommandString;
    CommandString.Format(_T("Load \'%s\'"), szFileName);
    WriteCommand(CommandString);

    while (!m_bSucceed)
    {
        Sleep(500);
    }

    return S_OK;
}

HRESULT PlayerController::Snapshot(CString szFileName)
{
    if (!m_bConnected)
    {
        return E_UNEXPECTED;
    }

    m_bSucceed = FALSE;
    CString CommandString;
    CommandString.Format(_T("Snapshot \'%s\' %d"), szFileName, 64);
    WriteCommand(CommandString);

    while (!m_bSucceed)
    {
        Sleep(500);
    }

    return S_OK;
}

HRESULT PlayerController::Quit()
{
    if (!m_bConnected)
    {
        return E_UNEXPECTED;
    }

    CString CommandString;
    CommandString.Format(_T("Quit"));
    WriteCommand(CommandString);

    return S_OK;
}

HRESULT PlayerController::StartListenPipe()
{
    HRESULT hr = S_OK;
    UINT nWaitCount = 0;

    if (!m_bConnected)
    {
        // Try wait 10s to connect player, if can not connect to player within 60s, return fail
        do
        {
            Sleep(500);
            nWaitCount++;
            hr = Connect();
        } while (FAILED(hr) && nWaitCount <= 120);

        if (FAILED(hr))
        {
            TRACE1("Connect to player failed (%d).\n", hr);
            return hr;
        }
    }

    m_hTimerQueue = CreateTimerQueue();
    if (NULL == m_hTimerQueue)
    {
        TRACE1("CreateTimerQueue failed (%d).\n", GetLastError());
        return E_FAIL;
    }

    if (!CreateTimerQueueTimer(&m_hTimer, m_hTimerQueue, TimerRoutine, this, 0, 100, 0))
    {
        TRACE1("CreateTimerQueueTimer failed (%d).\n", GetLastError());
        return E_FAIL;
    }

    return S_OK;
}

HRESULT PlayerController::Connect()
{
    if(m_bConnected)
    {
        CloseConnection();
    }

    if(WaitNamedPipe(_T("\\\\.\\Pipe\\S3SignagePlayer") + m_szPipePostfix, NMPWAIT_WAIT_FOREVER) == FALSE)
    {
        TRACE0("Can not connect to player.\n");
        return E_UNEXPECTED;
    }

    m_hPipe = CreateFile(_T("\\\\.\\Pipe\\S3SignagePlayer") + m_szPipePostfix, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL);

    if(m_hPipe == INVALID_HANDLE_VALUE)
    {
        TRACE0("Can not connect to player.\n");
        return E_UNEXPECTED;
    }

    m_bConnected = TRUE;
    m_bInitalized = FALSE;
    memset(&m_AsycStructure, 0, sizeof(m_AsycStructure));

    return S_OK;
}

HRESULT PlayerController::Terminate()
{
    WriteCommand(_T("Quit"));
    CloseConnection();

    if (NULL != m_hTimerQueue)
    {
        if (!DeleteTimerQueueTimer(m_hTimerQueue, m_hTimer, NULL))
        {
            TRACE1("DeleteTimerQueueTimer failed (%d).\n", GetLastError());
            return E_FAIL;
        }

        //if (!DeleteTimerQueueEx(m_hTimerQueue, NULL));
        //{
        //    TRACE1("DeleteTimerQueueEx failed (%d).\n", GetLastError());
        //    return E_FAIL;
        //}

        m_hTimer = NULL;
        m_hTimerQueue = NULL;
    }

    return S_OK;
}

void PlayerController::CloseConnection()
{
    m_bConnected = FALSE;

    if(INVALID_HANDLE_VALUE != m_hPipe)
    {
        CloseHandle(m_hPipe);
        m_hPipe = INVALID_HANDLE_VALUE;
    }
}

HRESULT PlayerController::WriteCommand(CString CommandString)
{
    if(m_bConnected && m_hPipe != INVALID_HANDLE_VALUE)
    {
        if(CommandString.GetLength()!= 0)
        {
            DWORD WriteNum;
            if(WriteFile(m_hPipe, CommandString, CommandString.GetLength() * sizeof(TCHAR) + sizeof(TCHAR), &WriteNum, NULL) == FALSE)
            {
                TRACE0("Can not send command to player.\n");
                CloseConnection();
                return E_UNEXPECTED;
            }
        }
    }

    return S_OK;
}

void PlayerController::SetPipePostfix(const CString& p)
{
    m_szPipePostfix = p;
}

void PlayerController::SetLogPath(const CString& p)
{
    m_szLogPath = StringUtility::StripEndSlash(p.GetString()).c_str();
}
