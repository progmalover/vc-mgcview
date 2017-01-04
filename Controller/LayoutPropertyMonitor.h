#pragma once
#include "Singleton.h"
#include <map>
using namespace std;

extern UINT WM_ON_MEDIA_SEL_CHANGE;
extern UINT WM_ON_SIZE_POS_CHANGE2;
extern UINT WM_PRE_SIZE_POS_CHANGE2;
extern UINT WM_ON_LAYOUT_SIZE_CHANGE;
extern UINT WM_ON_LAYOUT_SIZE_PRE_CHANGE;
extern UINT WM_ON_LAYER_SEL_CHANGE;
extern UINT WM_ON_LAYER_LOOP_CHANGE;
extern UINT WM_ON_MEDIA_NUM_CHANGE;
//extern UINT WM_ON_MEDIA_ITEM_CHANGE;

namespace DESIGNERCMDS
{
 //designed for "snapshot mode" undo/redo system.
	typedef map<CString ,CString> Context_Map;
	class StatusContext
	{
	public:
		StatusContext();
		~StatusContext();
	public:
		void AttachContext(Context_Map &);
		void SetContext(CString strName,CString val);
		CString  GetContextVal(CString strName);
		Context_Map  GetContext(){return m_mapContext;};
	private:
		Context_Map m_mapContext;
	};

	typedef struct context_data
	{
		WNDPROC orgProc;
		StatusContext context;
	}CONTEXT_DATA;
	class CLayoutPropertyMonitor
	{
		DECLARE_SINGLETON(CLayoutPropertyMonitor);
	public:
		CLayoutPropertyMonitor(void);
		~CLayoutPropertyMonitor(void);

		typedef pair<CWnd *,CONTEXT_DATA>  WND_HANDLE;
		typedef map<CWnd *,CONTEXT_DATA> WND_HANDLES;
	protected:	
		WND_HANDLES m_Wnds;
	public :
		LRESULT HandleMsg(CWnd *,DWORD,WPARAM,LPARAM);
		WND_HANDLES &GetWnds(){return m_Wnds;};

		void DetectUpdateBegin(CWnd *,UINT,WPARAM,LPARAM);
		int  DetectUpdateEnd();
		void MakeUpdateCmd();

		void AddPropertyWnd( CWnd *);
	private:
		BOOL m_bUpdated;
	};

}

