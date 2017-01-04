#ifndef __SPLASHWND_H_
#define __SPLASHWND_H_

class MEMORY_STREAM : IStream
{
    ULONG m_ReferenceCount;
    const PUCHAR m_Memory;
    ULARGE_INTEGER m_Offset;
    ULARGE_INTEGER m_Size;

public:
    MEMORY_STREAM(const void* Memory, DWORD Size);

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID InterfaceId, void** Object);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release(); 

    // ISequentialStream
    STDMETHODIMP Read(void* Buffer, ULONG ReadBytes, ULONG* BytesRead);
    STDMETHODIMP Write(const void* Buffer, ULONG WriteBytes, ULONG* BytesWritten);

    // IStream
    STDMETHODIMP Seek(LARGE_INTEGER Move, DWORD Origin, ULARGE_INTEGER* NewPosition);
    STDMETHODIMP SetSize(ULARGE_INTEGER NewSize);
    STDMETHODIMP CopyTo(IStream* Stream, ULARGE_INTEGER CopyBytes, ULARGE_INTEGER* BytesRead, ULARGE_INTEGER* BytesWritten);
    STDMETHODIMP Commit(DWORD Flags);
    STDMETHODIMP Revert();
    STDMETHODIMP LockRegion(ULARGE_INTEGER Offset, ULARGE_INTEGER Size, DWORD Type);
    STDMETHODIMP UnlockRegion(ULARGE_INTEGER Offset, ULARGE_INTEGER Size, DWORD Type);
    STDMETHODIMP Stat(STATSTG* Stats, DWORD Flag);
    STDMETHODIMP Clone(IStream** Stream);
};


class CSplashWnd
{
private:
	CSplashWnd(const CSplashWnd&) {};
	CSplashWnd& operator=(const CSplashWnd&) {};
protected:
	HANDLE				m_hThread;
	unsigned int		m_ThreadId;
	HANDLE				m_hEvent;

	Gdiplus::Image*		m_pImage;
	HWND				m_hSplashWnd;
	std::tstring		m_WindowName;
	HWND				m_hParentWnd;
	std::tstring		m_ProgressMsg;


public:
	CSplashWnd(int imgId, HWND hParent = NULL );
	~CSplashWnd();

	void SetImage(Gdiplus::Image* pImage);
    void SetImage(HMODULE hModule, UINT nResourceID);
	void SetWindowName(LPCWSTR windowName);
	void Show();
	void Hide();
    void SetString(const std::tstring& str);

	HWND GetWindowHwnd() const
	{
		return m_hSplashWnd;
	};

protected:
	static LRESULT CALLBACK SplashWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static unsigned int __stdcall SplashThreadProc(void* lpParameter);
};

#endif//__SPLASHWND_H_
