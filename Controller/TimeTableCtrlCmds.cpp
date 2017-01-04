#include "stdafx.h"
#include "TimeTableCtrlCmds.h"
#include "TimeTableCtrl.h"
#include "TimeTableItemManager.h"
namespace _TIMETABLECTRLCMDS
{

	///////////////////////////////////////////////////////////////////////////////
	////timetable engine.

	bool TimeTableCmdEngine::bindTarget(CWnd *pTargetWnd)
	{
		ASSERT(pTargetWnd);
		__super::bindTarget(pTargetWnd);
		m_pFactory->bind(this);
		return true;
	}

	//
	//timetable factor
	//
	TimeTableCmdFactory::TimeTableCmdFactory()
	{
		m_pBuffCmd = NULL;
	}

	TimeTableCmdFactory::~TimeTableCmdFactory(){}
	_baseCmd * TimeTableCmdFactory::newCmdBegin(int nID)
	{
		if(m_pBuffCmd)
		{
			TRACE(_T("Bufferd Cmd is existing!\n"));
			return NULL;
		}
		this->m_pBuffCmd = newCmd(nID);
		return m_pBuffCmd;
	}

	_baseCmd * TimeTableCmdFactory::getBuffedCmd()
	{
		return (_baseCmd *)m_pBuffCmd;
	}

	_baseCmd * TimeTableCmdFactory::newCmdEnd()
	{
		_baseCmd *pCmd = m_pBuffCmd;
		m_pBuffCmd = NULL;
		return (_baseCmd *)pCmd;
	}

	_baseCmd * TimeTableCmdFactory::newCmd(int nID)
	{
		_baseCmd *pNewCmd = NULL;
		ASSERT(m_pOwner);
		CWnd *pTargetWnd = m_pOwner->getTargetWnd();

		ASSERT(pTargetWnd);
		switch(nID)
		{
		case CMD_TIEMEOBJ_CREATE:
			pNewCmd = new timeObjCreateCmd(nID,pTargetWnd);
			break;
		case CMD_TIEMEOBJ_DELETE:
			pNewCmd = new timeObjDeleteCmd(nID,pTargetWnd);
			break;
		case CMD_TIEMEOBJ_DRAG:
			pNewCmd = new timeObjDragCmd(nID,pTargetWnd);
			break;
		case CMD_TIEMEOBJ_DURATION_CHANGE:
			pNewCmd = new timeObjDurationChangeCmd(nID,pTargetWnd);
			break;
		case CMD_TIMEOBJ_PASTE:
			pNewCmd = new timeObjCreateCmd(nID,pTargetWnd);
			break;
		default: // unknown cmd id.
			ASSERT(0);
			return NULL;
		}
		return (_baseCmd *)pNewCmd;
	}

	bool TimeTableCmdFactory::delCmd(_baseCmd *pCmd)
	{
		__super::delCmd(pCmd);

		ASSERT(pCmd);
		switch(pCmd->getID())
		{
		case CMD_TIEMEOBJ_CREATE:
			delete (timeObjCreateCmd *)pCmd;
			break;
		case CMD_TIEMEOBJ_DELETE:
			delete (timeObjDeleteCmd *)pCmd;
			break;
		case CMD_TIEMEOBJ_DRAG:
			delete (timeObjDragCmd *)pCmd;
			break;
		case CMD_TIEMEOBJ_DURATION_CHANGE:
			delete (timeObjDurationChangeCmd*)pCmd;
			break;
		case CMD_TIMEOBJ_PASTE:
			delete (timeObjCreateCmd *)pCmd;
			break;
		default:
			TRACE(_T("Try to free unknown cmd class!"));
			this->FreeBaseCmd(pCmd);
			break;
		}
		return true;
	}

	LPCTSTR TimeTableCmdFactory::getCmdName(_baseCmd *)
	{
		return _T("");
	}
	/////////////////////////////////////////////////////////////////////////////////
	///cmds

	
	//
	//timeObjCreateCmd :create an time object or undo it.
	//
	timeObjCreateCmd::timeObjCreateCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){
	}

	timeObjCreateCmd::~timeObjCreateCmd(){
		//
		//Free items when Cmd in redo stack.
		//
		if(this->m_nOpt == OPT_REDO && _Appointments.GetCount())
		{
			//delete this->pAppointment;
			POSITION pos = _Appointments.GetHeadPosition();
			while(pos)
			{
				Appointment *pApp = _Appointments.GetAt(pos);
				delete pApp;
				_Appointments.GetNext(pos);
			}
		}
	}

	int timeObjCreateCmd::redo()
	{
		__super::redo();
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		//ASSERT(this->pAppointment);
		pTimeTable->ClearAppointmentSelection(0);
		POSITION pos = _Appointments.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _Appointments.GetAt(pos);
			pTimeTable->AddAppointment(pApp,1,0);
			pTimeTable->SelectItem(pApp,1,0);
			_Appointments.GetNext(pos);
		}
		pTimeTable->Invalidate();
		return SUCCESS;
	}

	int timeObjCreateCmd::undo()
	{
		__super::undo();
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		ASSERT(_Appointments.GetCount());
		
		TimeTableItemManager &Items = pTimeTable->GetItemManager();

		BOOL bVal = Items.m_bAutoDelete;
		Items.m_bAutoDelete = FALSE;
		
		pTimeTable->ClearAppointmentSelection(0);
		POSITION pos = _Appointments.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _Appointments.GetAt(pos);
			pTimeTable->RemoveAppointment(pApp);
			_Appointments.GetNext(pos);
		}
		Items.m_bAutoDelete = bVal;
		return SUCCESS;
	}

	int timeObjCreateCmd::ondo()
	{
		__super::ondo();
		if(this->_Appointments.GetCount() <= 0)
			return FAILURE;
		return SUCCESS;
	}

	//
	//timeObjDeleteCmd : delete an time object or restore it.
	//
	timeObjDeleteCmd::timeObjDeleteCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){
		if(this->m_nOpt == OPT_UNDO)
		{
			POSITION pos = _Appointments.GetHeadPosition();
			while(pos)
			{
			   Appointment *pApp = _Appointments.GetAt(pos);
			   delete pApp;
			   _Appointments.GetNext(pos);
			}
		}
	}

	timeObjDeleteCmd::~timeObjDeleteCmd(){}

	int timeObjDeleteCmd::redo()
	{
		__super::redo();
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		
		//save selected items.
		AppointmentList &selList =  _Appointments;
		if(selList.GetCount() <= 0)
			return FAILURE;
		pTimeTable->m_SelectedItemList.RemoveAll();
		
		TimeTableItemManager &Items = pTimeTable->GetItemManager();
		BOOL bVal = Items.m_bAutoDelete;
		Items.m_bAutoDelete = FALSE;
		POSITION pos = selList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = selList.GetAt(pos);
			pTimeTable->RemoveAppointment(pApp,0);
			selList.GetNext(pos);
		}
		Items.m_bAutoDelete = bVal;
		pTimeTable->Invalidate();
		return SUCCESS;
	}

	int timeObjDeleteCmd::undo()
	{
		__super::undo();

		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		
		//restore selected items.
		AppointmentList &selList =  pTimeTable->m_SelectedItemList;
		selList.RemoveAll();

		POSITION pos = _Appointments.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _Appointments.GetAt(pos);
			pTimeTable->SelectItem(pApp,1);
			_Appointments.GetNext(pos);
		}
		
		TimeTableItemManager &Items = pTimeTable->GetItemManager();
		Items.m_ItemList.RemoveAll();
		for( int i = 0;i < _bkAppInfos.size();i++)
		{
			APPOINTMENT_INFO &info = _bkAppInfos[i];
			info.pAppointment->SetEnd(info.dtEnd);
			info.pAppointment->SetStart(info.dtStart);
			//info.pAppointment->SetEnd(info.dtEnd);

			Items.m_ItemList.AddTail(info.pAppointment);
		}
	
		pTimeTable->Invalidate();
		return SUCCESS;
	}

	int timeObjDeleteCmd::ondo()
	{
		__super::ondo();
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		
		//save selected items.
		AppointmentList &selList =  pTimeTable->m_SelectedItemList;
		if(selList.GetCount() <= 0)
			return FAILURE;
	
		POSITION pos = selList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = selList.GetAt(pos);
			this->_Appointments.AddTail(pApp);
			selList.GetNext(pos);
		}

		TimeTableItemManager &Items = pTimeTable->GetItemManager();
		pos = Items.m_ItemList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = Items.m_ItemList.GetAt(pos);
			APPOINTMENT_INFO  info;
			info.pAppointment = pApp;
			info.dtStart = pApp->GetStart();
			info.dtEnd = pApp->GetFinish();

			_bkAppInfos.push_back(info);
			Items.m_ItemList.GetNext(pos);
		}

		BOOL bVal = Items.m_bAutoDelete;
		Items.m_bAutoDelete = FALSE;   
		pTimeTable->RemoveSelectedAppointments();
		Items.m_bAutoDelete = bVal;
		return SUCCESS;
	}

	//
	//Paste an time object or remove it.
	//
	timeObjPasteCmd::timeObjPasteCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){}
	timeObjPasteCmd::~timeObjPasteCmd(){}

	int timeObjPasteCmd::redo()
	{
		__super::redo();
		return SUCCESS;
	}
	int timeObjPasteCmd::undo()
	{
		__super::undo();
		return SUCCESS;
	}
	int timeObjPasteCmd::ondo()
	{
		__super::ondo();
		return SUCCESS;
	}
	

	//
	//Drag an object to another location.
	//
	timeObjDragCmd::timeObjDragCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){}
	timeObjDragCmd::~timeObjDragCmd(){}

	BOOL timeObjDragCmd::SaveAppointmentListInfos(AppointmentList &appList ,APPOINTMENT_VECTOR &infoArray)
	{
		POSITION pos = appList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = appList.GetAt(pos);
			APPOINTMENT_INFO info;
			info.pAppointment = pApp;
			info.dtStart = pApp->GetStart();
			info.dtEnd = pApp->GetFinish();
			infoArray.push_back(info);
			appList.GetNext(pos);
		}
		return TRUE;
	}

	BOOL timeObjDragCmd::RestoreAppointmentListInfos(AppointmentList &appList ,APPOINTMENT_VECTOR &infoArray)
	{
		if(infoArray.size() <= 0)
			return FALSE;
		appList.RemoveAll();
		for(int i = 0;i < infoArray.size();i++)
		{
			Appointment *pApp = infoArray[i].pAppointment;
			appList.AddTail(pApp);
			pApp->SetStart(infoArray[i].dtStart);
			pApp->SetEnd(infoArray[i].dtEnd);
		}
		return TRUE;
	}

	BOOL timeObjDragCmd::SaveStartDragInfos()
	{
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		TimeTableItemManager &Items = pTimeTable->GetItemManager();
		SaveAppointmentListInfos(Items.m_ItemList,this->_startDragBkInfos);
		return TRUE;
	}

	BOOL timeObjDragCmd::SaveEndDragInfos()
	{
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		TimeTableItemManager &Items = pTimeTable->GetItemManager();
		SaveAppointmentListInfos(Items.m_ItemList,this->_endDragBkInfos);
		return TRUE;
	}

	int timeObjDragCmd::redo()
	{
		__super::redo();
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
	
		TimeTableItemManager &Items = pTimeTable->GetItemManager();
		RestoreAppointmentListInfos(Items.m_ItemList,this->_endDragBkInfos);

		pTimeTable->ClearAppointmentSelection(0);
		POSITION pos = _selList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _selList.GetAt(pos);
			pTimeTable->SelectItem(pApp,1,0);
			_selList.GetNext(pos);
		}
		pTimeTable->Invalidate();
		return SUCCESS;
	}

	int timeObjDragCmd::undo()
	{
		__super::undo();
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		TimeTableItemManager &Items = pTimeTable->GetItemManager();
		RestoreAppointmentListInfos(Items.m_ItemList,this->_startDragBkInfos);
		
		pTimeTable->ClearAppointmentSelection(0);
		POSITION pos = _selList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _selList.GetAt(pos);
			pTimeTable->SelectItem(pApp,1,0);
			_selList.GetNext(pos);
		}
		pTimeTable->Invalidate();
		return SUCCESS;
	}

	int timeObjDragCmd::ondo()
	{
		__super::ondo();
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		
		//save selected items.
		AppointmentList &selList =  pTimeTable->m_SelectedItemList;
		if(selList.GetCount() <= 0)
			return FAILURE;
		POSITION pos = selList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = selList.GetAt(pos);
			this->_selList.AddTail(pApp);
			//pApp->SetSelected(TRUE);
			selList.GetNext(pos);
		}
		return SUCCESS;
	}
	//
	//Drag  side of time object to change duration.
	//
	timeObjDurationChangeCmd::timeObjDurationChangeCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){}
	timeObjDurationChangeCmd::~timeObjDurationChangeCmd(){}

	void  timeObjDurationChangeCmd::SaveStartInfo()
	{
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		ASSERT(pTimeTable->m_SelectedItemList.GetCount() >= 1);

		_appStartInfo.pAppointment = pTimeTable->m_SelectedItemList.GetHead();
                if(!_appStartInfo.pAppointment)
                    return;
		_appStartInfo.dtStart = _appStartInfo.pAppointment->GetStart();
		_appStartInfo.dtEnd = _appStartInfo.pAppointment->GetFinish();
	}

	void  timeObjDurationChangeCmd::SaveEndInfo()
	{
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
                //ASSERT(pTimeTable->m_SelectedItemList.GetCount() >= 1);

                //_appEndInfo.pAppointment = pTimeTable->m_SelectedItemList.GetHead();
		//ASSERT(_appStartInfo.pAppointment == _appEndInfo.pAppointment);
               if(!_appStartInfo.pAppointment)
                    return;
		_appEndInfo.dtStart = _appStartInfo.pAppointment->GetStart();
		_appEndInfo.dtEnd = _appStartInfo.pAppointment->GetFinish();
		
	}

	int timeObjDurationChangeCmd::redo()
	{
		__super::redo();
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		ASSERT(this->_appEndInfo.pAppointment);
		ASSERT(_appStartInfo.pAppointment == _appEndInfo.pAppointment);

		pTimeTable->ClearAppointmentSelection(0);
		pTimeTable->SelectItem(_appEndInfo.pAppointment,1,0);

		_appEndInfo.pAppointment->SetStart(_appEndInfo.dtStart);
		_appEndInfo.pAppointment->SetEnd(_appEndInfo.dtEnd);

		//pTimeTable->GetCurrentView()->ReArrangeAppointments();
		TimeTableItemManager &Items = pTimeTable->GetItemManager();
		SortAppointmentList(Items.m_ItemList);
		pTimeTable->Invalidate();
		return SUCCESS;
	}

	int timeObjDurationChangeCmd::undo()
	{
		__super::undo();
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		ASSERT(this->_appStartInfo.pAppointment);
		ASSERT(_appStartInfo.pAppointment == _appEndInfo.pAppointment);

		pTimeTable->ClearAppointmentSelection(0);
		pTimeTable->SelectItem(_appStartInfo.pAppointment,1,0);

		_appStartInfo.pAppointment->SetStart(_appStartInfo.dtStart);
		_appStartInfo.pAppointment->SetEnd(_appStartInfo.dtEnd);
		//pTimeTable->
		//pTimeTable->GetView(pTimeTable->m_Style)->AdjustLayout(1);
		//	pTimeTable->GetCurrentView()->ReArrangeAppointments();
		TimeTableItemManager &Items = pTimeTable->GetItemManager();
		SortAppointmentList(Items.m_ItemList);
		pTimeTable->Invalidate();
		return SUCCESS;
	}

	int timeObjDurationChangeCmd::ondo()
	{
		__super::ondo();
		CTimeTableCtrl *pTimeTable = (CTimeTableCtrl *)this->m_pTargetWnd;
		ASSERT(pTimeTable);
		if(NULL == this->_appEndInfo.pAppointment || NULL == _appStartInfo.pAppointment)
			return FAILURE;
		if(_appStartInfo.pAppointment != _appEndInfo.pAppointment)
			return FAILURE;
		//const  COleDateTime &curStartTime = this->_appStartInfo.pAppointment->GetStart();
		//const  COleDateTime &curEndTime = this->_appStartInfo.pAppointment->GetFinish();
		COleDateTimeSpan duration1 = this->_appStartInfo.dtEnd - this->_appStartInfo.dtStart;
		COleDateTimeSpan duration2 = this->_appEndInfo.dtEnd - this->_appEndInfo.dtStart;
		if(duration2 == duration1)
			return FAILURE;
		return SUCCESS;
	
	}
	

};
