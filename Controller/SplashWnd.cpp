#include "stdafx.h"
#include <Windows.h>
#include <CommCtrl.h>
#include <GdiPlus.h>
#include <string>
#include <process.h>

#include "SplashWnd.h"
#include "resource.h"


////////////////////////////////////////////////////////////////////////////////
//
// MEMORY_STREAM Implementation
//
////////////////////////////////////////////////////////////////////////////////


STDMETHODCALLTYPE MEMORY_STREAM::MEMORY_STREAM(const void* Image, DWORD Size)
    : m_ReferenceCount(0),
      m_Memory((PUCHAR)Image)
{
    m_Offset.QuadPart = 0;
    m_Size.QuadPart = Size;
}

STDMETHODIMP_(ULONG) MEMORY_STREAM::AddRef()
{
    return ++m_ReferenceCount;
}

STDMETHODIMP_(ULONG) MEMORY_STREAM::Release()
{
    ULONG ReferenceCount(--m_ReferenceCount);

    if (m_ReferenceCount == 0)
    {
        delete this;
    }

    return ReferenceCount;
}


STDMETHODIMP MEMORY_STREAM::QueryInterface(REFIID InterfaceId, void** Object)
{
    while (true)
    {
        if (IsEqualGUID(InterfaceId, __uuidof(IUnknown)))
        {
            *Object = static_cast<IUnknown*>(this);
            break;
        }

        if (IsEqualGUID(InterfaceId, __uuidof(IStream)))
        {
            *Object = static_cast<IStream*>(this);
            break;
        }

        *Object = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


STDMETHODIMP MEMORY_STREAM::Read(void* Buffer, ULONG ReadBytes, ULONG* BytesRead)
{
    ULONG Length;
    HRESULT Result;

    if (m_Offset.QuadPart + ReadBytes > m_Size.QuadPart)
    {
        Length = (ULONG)(m_Size.QuadPart - m_Offset.QuadPart);
        Result = S_FALSE;
    }
    else
    {
        Length = ReadBytes;
        Result = S_OK;
    }

    CopyMemory(Buffer, m_Memory + (ULONG_PTR)m_Offset.QuadPart, Length);
    m_Offset.QuadPart += Length;

    if (BytesRead)
    {
        *BytesRead = Length;
    }

    return Result;
}


STDMETHODIMP MEMORY_STREAM::Write(const void* Buffer, ULONG WriteBytes, ULONG* BytesWritten)
{
    return E_NOTIMPL;
}


STDMETHODIMP MEMORY_STREAM::Seek(LARGE_INTEGER Move, DWORD Origin, ULARGE_INTEGER* NewPosition)
{
    switch (Origin)
    {
    case STREAM_SEEK_SET:
        m_Offset.QuadPart = Move.QuadPart;
        break;

    case STREAM_SEEK_CUR:
        if (m_Offset.QuadPart + Move.QuadPart > m_Size.QuadPart)
        {
            return STG_E_INVALIDFUNCTION;
        }

        m_Offset.QuadPart += Move.QuadPart;
        break;

    case STREAM_SEEK_END:
        if ((ULONGLONG)Move.QuadPart > m_Offset.QuadPart)
        {
            return STG_E_INVALIDFUNCTION;
        }

        m_Offset.QuadPart = m_Size.QuadPart - Move.QuadPart;
        break;

    default:
        return STG_E_INVALIDFUNCTION;
    }

    if (NewPosition)
    {
        *NewPosition = m_Offset;
    }

    return S_OK;
}


STDMETHODIMP MEMORY_STREAM::SetSize(ULARGE_INTEGER NewSize)
{
    return E_NOTIMPL;
}

STDMETHODIMP MEMORY_STREAM::CopyTo(IStream* Stream, ULARGE_INTEGER CopyBytes, ULARGE_INTEGER* BytesRead, ULARGE_INTEGER* BytesWritten)
{
    return E_NOTIMPL;
}

STDMETHODIMP MEMORY_STREAM::Commit(DWORD Flags)
{
    return E_NOTIMPL;
}

STDMETHODIMP MEMORY_STREAM::Revert()
{
    return E_NOTIMPL;
}


STDMETHODIMP MEMORY_STREAM::LockRegion(ULARGE_INTEGER Offset, ULARGE_INTEGER Size, DWORD Type)
{
    return E_NOTIMPL;
}


STDMETHODIMP MEMORY_STREAM::UnlockRegion(ULARGE_INTEGER Offset, ULARGE_INTEGER Size, DWORD Type)
{
    return E_NOTIMPL;
}

STDMETHODIMP MEMORY_STREAM::Stat(STATSTG* Stats, DWORD Flags)
{
    ZeroMemory(Stats, sizeof(*Stats));

    if ((Flags & STATFLAG_NONAME) != STATFLAG_NONAME)
    {
        Stats->pwcsName = (LPOLESTR)CoTaskMemAlloc(sizeof(OLECHAR));

        if (Stats->pwcsName)
        {
            *Stats->pwcsName = 0;
        }
    }

    Stats->type = STGTY_STREAM;
    Stats->cbSize = m_Size;
    return S_OK;
}


STDMETHODIMP MEMORY_STREAM::Clone(IStream** Stream)
{
    return E_NOTIMPL;
}


HRESULT CreateStreamOnResource(HMODULE Instance, PCTSTR Type, PCTSTR Name, IStream** Stream)
{
    HRSRC ResourceHandle = FindResource(Instance, Name, Type);

    if (ResourceHandle)
    {
        DWORD Size = ::SizeofResource(Instance, ResourceHandle);

        if (Size)
        {
            HGLOBAL Resource = LoadResource(Instance, ResourceHandle);

            if (Resource)
            {
                const void* Data = LockResource(Resource);

                MEMORY_STREAM* MemoryStream = new MEMORY_STREAM(Data, Size);

                if (!MemoryStream)
                {
                    return E_OUTOFMEMORY;
                }

                return MemoryStream->QueryInterface
                    (
                    __uuidof(IStream),
                    (void**)Stream
                    );
            }
        }
    }

    return HRESULT_FROM_WIN32(GetLastError());
}

//////////////////////////////////////////////////////////////////////////

CSplashWnd::CSplashWnd(int imgId, HWND hParent )
{
    IStream* stream;
    CreateStreamOnResource(NULL, _T("PNG"), MAKEINTRESOURCE(imgId), &stream);
    m_hThread = NULL;
    m_pImage = Gdiplus::Image::FromStream(stream);
    stream->Release();
    m_hSplashWnd = NULL;
    m_ThreadId = 0;
    m_hEvent = NULL;
    m_hParentWnd = hParent;
}

CSplashWnd::~CSplashWnd()
{
    Hide();
    if (m_pImage) delete m_pImage;
}

void CSplashWnd::SetImage( Gdiplus::Image* pImage )
{
    if (m_pImage == NULL && pImage != NULL)
        m_pImage = pImage->Clone();
}


void CSplashWnd::Show()
{
    if (m_hThread == NULL)
    {
        m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        m_hThread = (HANDLE)_beginthreadex(NULL, 0, SplashThreadProc, static_cast<LPVOID>(this), 0, &m_ThreadId);
        if (WaitForSingleObject(m_hEvent, 5000) == WAIT_TIMEOUT)
        {
            OutputDebugString(_T("Error starting SplashThread\n"));
        }
    }
    else
    {
        PostThreadMessage( m_ThreadId, WM_ACTIVATE, WA_CLICKACTIVE, 0 );
    }
}

void CSplashWnd::Hide()
{
    if (m_hThread)
    {
        PostThreadMessage(m_ThreadId, WM_QUIT, 0, 0);
        if ( WaitForSingleObject(m_hThread, 9000) == WAIT_TIMEOUT )
        {
            ::TerminateThread( m_hThread, 2222 );
        }
        CloseHandle(m_hThread);
        CloseHandle(m_hEvent);
    }
    m_hThread = NULL;
}

unsigned int __stdcall CSplashWnd::SplashThreadProc( void* lpParameter )
{
    CSplashWnd* pThis = static_cast<CSplashWnd*>(lpParameter);
    if (pThis->m_pImage == NULL) return 0;

    // Register your unique class name
    WNDCLASS wndcls = {0};

    wndcls.style = CS_HREDRAW | CS_VREDRAW;
    wndcls.lpfnWndProc = SplashWndProc;
    wndcls.hInstance = GetModuleHandle(NULL);
    wndcls.hCursor = LoadCursor(NULL, IDC_APPSTARTING);
    wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndcls.lpszClassName = _T("SplashWnd");
    wndcls.hIcon = LoadIcon(wndcls.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClass(&wndcls))
    {
        if (GetLastError() != 0x00000582) // already registered)
        {
            OutputDebugString(_T("Unable to register class SplashWnd\n"));
            return 0;
        }
    }

    // try to find monitor where mouse was last time
    POINT point = { 0 };
    MONITORINFO mi = { sizeof(MONITORINFO), 0 };
    HMONITOR hMonitor = 0;
    RECT rcArea = { 0 };

    ::GetCursorPos( &point );
    hMonitor = ::MonitorFromPoint( point, MONITOR_DEFAULTTONEAREST );
    if ( ::GetMonitorInfo( hMonitor, &mi ) )
    {
        rcArea.left = ( mi.rcMonitor.right + mi.rcMonitor.left - static_cast<long>(pThis->m_pImage->GetWidth()) ) /2;
        rcArea.top = ( mi.rcMonitor.top + mi.rcMonitor.bottom - static_cast<long>(pThis->m_pImage->GetHeight()) ) /2;
    }
    else
    {
        SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
        rcArea.left = (rcArea.right + rcArea.left - pThis->m_pImage->GetWidth())/2;
        rcArea.top = (rcArea.top + rcArea.bottom - pThis->m_pImage->GetHeight())/2;
    }

    //
    pThis->m_hSplashWnd = CreateWindowEx(pThis->m_WindowName.length()?0:WS_EX_TOOLWINDOW, _T("SplashWnd"), pThis->m_WindowName.c_str(),
        WS_CLIPCHILDREN|WS_POPUP, rcArea.left, rcArea.top, pThis->m_pImage->GetWidth(), pThis->m_pImage->GetHeight(),
        pThis->m_hParentWnd, NULL, wndcls.hInstance, NULL);
    if (!pThis->m_hSplashWnd)
    {
        OutputDebugString(_T("Unable to create SplashWnd\n"));
        return 0;
    }


    SetWindowLongPtr(pThis->m_hSplashWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis) );
    ShowWindow(pThis->m_hSplashWnd, SW_SHOWNOACTIVATE);
    UpdateWindow(pThis->m_hSplashWnd);


    MSG msg;
    BOOL bRet;
    LONG timerCount = 0;

    PeekMessage(&msg, NULL, 0, 0, 0); // invoke creating message queue
    SetEvent(pThis->m_hEvent);

    while ((bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
    {
        if (msg.message == WM_QUIT) break;


        if (bRet == -1)
        {
            // handle the error and possibly exit
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    DestroyWindow(pThis->m_hSplashWnd);

    return 0;
}

LRESULT CALLBACK CSplashWnd::SplashWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    CSplashWnd* pInstance = reinterpret_cast<CSplashWnd*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (pInstance == NULL)
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
    case WM_PAINT:
        {
            if (pInstance->m_pImage)
            {
                POINT point = { 0 };
                MONITORINFO mi = { sizeof(MONITORINFO), 0 };
                HMONITOR hMonitor = 0;
                RECT rcArea = { 0 };

                ::GetCursorPos( &point );
                hMonitor = ::MonitorFromPoint( point, MONITOR_DEFAULTTONEAREST );
                if ( ::GetMonitorInfo( hMonitor, &mi ) )
                {
                    rcArea.left = ( mi.rcMonitor.right + mi.rcMonitor.left - static_cast<long>(pInstance->m_pImage->GetWidth()) ) /2;
                    rcArea.top = ( mi.rcMonitor.top + mi.rcMonitor.bottom - static_cast<long>(pInstance->m_pImage->GetHeight()) ) /2;
                }
                else
                {
                    SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
                    rcArea.left = (rcArea.right + rcArea.left - pInstance->m_pImage->GetWidth())/2;
                    rcArea.top = (rcArea.top + rcArea.bottom - pInstance->m_pImage->GetHeight())/2;
                }

                HDC hdcScreen = ::GetDC(pInstance->m_hSplashWnd);
                HDC hdcMemory = ::CreateCompatibleDC(hdcScreen);

                HBITMAP hBitMap = ::CreateCompatibleBitmap(hdcScreen, pInstance->m_pImage->GetWidth(), pInstance->m_pImage->GetHeight());
                SelectObject(hdcMemory,hBitMap);

                RECT rct;
                ::GetWindowRect(pInstance->m_hSplashWnd, &rct);
                POINT ptWinPos={rct.left,rct.top};
                Gdiplus::Graphics graph(hdcMemory);

                SIZE s;
                s.cx = pInstance->m_pImage->GetWidth();
                s.cy = pInstance->m_pImage->GetHeight();

                BLENDFUNCTION blend;
                blend.BlendOp = 0;
                blend.BlendFlags = 0;
                blend.AlphaFormat = 1;
                blend.SourceConstantAlpha = 255;

                DWORD dwExStyle = GetWindowLong(pInstance->m_hSplashWnd, GWL_EXSTYLE);
                if((dwExStyle&0x80000)!=0x80000)
                    SetWindowLong(pInstance->m_hSplashWnd, GWL_EXSTYLE, dwExStyle^0x80000);

//                 Gdiplus::Graphics gdip(hwnd);
                graph.DrawImage(pInstance->m_pImage, 0, 0, pInstance->m_pImage->GetWidth(), pInstance->m_pImage->GetHeight());

                if ( pInstance->m_ProgressMsg.size() > 0 )
                {
                    Gdiplus::Font msgFont( _T("Tahoma"), 12, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel );
                    Gdiplus::SolidBrush msgBrush( Gdiplus::Color(0xBD, 0xCA, 0xD8) );
                    int len = pInstance->m_ProgressMsg.size();
                    Gdiplus::StringFormat stringFormat;
                    stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);

                    graph.DrawString( pInstance->m_ProgressMsg.c_str(), -1,
                        &msgFont, Gdiplus::RectF(0, 145.0f, 475.0f, 64),
                        &stringFormat, &msgBrush );
                }
                SIZE sizeWindow={pInstance->m_pImage->GetWidth(), pInstance->m_pImage->GetHeight()};
                POINT ptSrc={0,0};

                BOOL bRet=FALSE;
                bRet= UpdateLayeredWindow(pInstance->m_hSplashWnd, hdcScreen,&ptWinPos,
                    &sizeWindow, hdcMemory, &ptSrc, 0, &blend, 2);
                graph.ReleaseHDC(hdcMemory);
                ::ReleaseDC(pInstance->m_hSplashWnd, hdcScreen);
                hdcScreen=NULL;
                DeleteObject(hBitMap);
                DeleteDC(hdcMemory);
                hdcMemory=NULL;
            }
            ValidateRect(hwnd, NULL);
            return 0;
        } break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CSplashWnd::SetWindowName( const wchar_t* windowName )
{
    m_WindowName = windowName;
}

void CSplashWnd::SetString(const std::tstring& str)
{
    m_ProgressMsg = str;
}
