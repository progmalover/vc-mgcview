#pragma once
#include "ControllerClient/CritSection.h"

static MagicView::CommonLib::ThreadSection m_CS;

class PlayerController
{
public:
    PlayerController();
    ~PlayerController();

    static PlayerController *GetInstance() { return &s_Instance; };

protected:
    static PlayerController     s_Instance;

public:
    void SetPipePostfix(const CString& p);
    void SetLogPath(const CString& p);
    HRESULT StartPlayer(const int nWidth = 800, const int nHeight = 600);
    HRESULT StopPlayer();
    BOOL IsPlayerRunning();

public:
    HRESULT Load(CString szFileName);
    HRESULT Snapshot(CString szFileName);
    HRESULT Quit();

protected:
    HRESULT StartListenPipe();
    HRESULT Terminate();

    void CloseConnection();
    HRESULT Connect();

    void Run();

    HRESULT WriteCommand(CString CommandString);

    friend void CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired);

private:
    HANDLE          m_hPlayerProcess;
    HANDLE          m_hPipe;
    HANDLE          m_hTimer;
    HANDLE          m_hTimerQueue;
    BOOL            m_bConnected;
    BOOL            m_bInitalized;
    OVERLAPPED      m_AsycStructure;
    TCHAR           m_szReadBuffer[1024];
    DWORD           m_nReadNumber;
    CString         m_szCommand;
    BOOL            m_bSucceed;
    CString         m_szPipePostfix;
    CString         m_szLogPath;
};