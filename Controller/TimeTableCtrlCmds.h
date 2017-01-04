#pragma once
#include"stdio.h"
#include "Appointment.h"
#include "cmds.h"
#include <vector>
using namespace std;
using namespace _CMDS;
namespace _TIMETABLECTRLCMDS
{
#define DECLARE_TIMETABLE_FRIEND_CMDS()\
	DECLARE_FRIEND_CLASS(timeObjCreateCmd)\
	DECLARE_FRIEND_CLASS(timeObjDeleteCmd)\
	DECLARE_FRIEND_CLASS(timeObjPasteCmd)\
	DECLARE_FRIEND_CLASS(timeObjDragCmd)\
	DECLARE_FRIEND_CLASS(timeObjDurationChangeCmd)

	enum cmds{
		CMD_TIEMEOBJ_CREATE = 2,
		CMD_TIEMEOBJ_DELETE,
		CMD_TIMEOBJ_PASTE,
		CMD_TIEMEOBJ_DRAG,		//keep duration
		CMD_TIEMEOBJ_DURATION_CHANGE //duration changed.
	};

	class TimeTableCmdEngine;
	class TimeTableCmdFactory :public _baseCmdFactory<TimeTableCmdEngine>
	{
	public:
		TimeTableCmdFactory();
		~TimeTableCmdFactory();
	public:
		_baseCmd * newCmdBegin(int nID);
		_baseCmd * getBuffedCmd();
		_baseCmd * newCmdEnd();
		_baseCmd * newCmd(int nID);
		bool       delCmd(_baseCmd *pCmd);
		LPCTSTR    getCmdName(_baseCmd *);
	private:
		_baseCmd * m_pBuffCmd;
	};

	class TimeTableCmdEngine :public CmdsEngine<TimeTableCmdFactory>
	{
		friend class TimeTableCmdFactory;
	public:
		TimeTableCmdEngine(){};
		~TimeTableCmdEngine(){};
	public:
		virtual bool bindTarget(CWnd *pTargetWnd);
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef struct AppointInfo
	{
		AppointInfo()
		{
			pAppointment = NULL;
		}
		Appointment *pAppointment;
		COleDateTime dtStart;
		COleDateTime dtEnd;
	} APPOINTMENT_INFO;
	typedef vector<APPOINTMENT_INFO> APPOINTMENT_VECTOR;

	class timeObjCreateCmd :public _baseCmd
	{
		friend class TimeTableCmdFactory;
	public:  
		timeObjCreateCmd(int nID,CWnd *pTarget);
		~timeObjCreateCmd();
	public:
		int redo();
		int undo();
		int ondo();
		AppointmentList _Appointments;
	};

	class timeObjDeleteCmd :public _baseCmd
	{  
		friend class TimeTableCmdFactory;
	public:
		timeObjDeleteCmd(int nID,CWnd *pTarget);
		~timeObjDeleteCmd();
	public:
		int redo();
		int undo();
		int ondo();
		AppointmentList    _Appointments;
		APPOINTMENT_VECTOR _bkAppInfos;
	};

	class timeObjPasteCmd :public _baseCmd
	{
		friend class TimeTableCmdFactory;
	public:
		timeObjPasteCmd(int nID,CWnd *pTarget);
		~timeObjPasteCmd();
	public:
		int redo();
		int undo();
		int ondo();
	};

	class timeObjDragCmd :public _baseCmd
	{
		friend class TimeTableCmdFactory;
	public:
		timeObjDragCmd(int nID,CWnd *pTarget);
		~timeObjDragCmd();
	public:
		int  redo();
		int  undo();
		int  ondo();
	public:
		BOOL SaveStartDragInfos();
		BOOL SaveEndDragInfos();
	private:
		BOOL SaveAppointmentListInfos(AppointmentList &appList ,APPOINTMENT_VECTOR &infoArray);
		BOOL RestoreAppointmentListInfos(AppointmentList &appList ,APPOINTMENT_VECTOR &infoArray);
	private:
		APPOINTMENT_VECTOR _startDragBkInfos;
		APPOINTMENT_VECTOR _endDragBkInfos;
		AppointmentList	   _selList;
	};

	class timeObjDurationChangeCmd :public _baseCmd
	{
		friend class TimeTableCmdFactory;
	public:
		timeObjDurationChangeCmd(int nID,CWnd *pTarget);
		~timeObjDurationChangeCmd();
	public:
		int redo();
		int undo();
		int ondo();

		void SaveStartInfo();
		void SaveEndInfo();
	private:
		APPOINTMENT_INFO _appStartInfo;
		APPOINTMENT_INFO _appEndInfo;
	};
};