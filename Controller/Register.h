#pragma once

// how to use this 

//  SetVar  RegSettingDW;
// 	INIT_SET(RegSettingDW, _T("KeySettingName"), SETVAR_DWORD);
//  RegSettingDW = 2000;
//  Register::SaveSetting(RegSettingDW, _T("Software\\S3\\S3Signage\\S3 Signage Controller"));

//  SetVar  RegSettingStr;
// 	INIT_SET(RegSettingStr, _T("KeySettingName"), SETVAR_STR);
//  RegSettingStr = _T("Value");
//  Register::SaveSetting(RegSettingStr, _T("Software\\S3\\S3Signage\\S3 Signage Controller"));

//  SetVar  RegSettingDW;
// 	INIT_SET(RegSettingDW, _T("KeySettingName"), SETVAR_DWORD);
//  Register::LoadSetting(RegSettingDW, _T("Software\\S3\\S3Signage\\S3 Signage Controller"));

//  SetVar  RegSettingStr;
// 	INIT_SET(RegSettingStr, _T("KeySettingName"), SETVAR_STR);
//  Register::LoadSetting(RegSettingStr, _T("Software\\S3\\S3Signage\\S3 Signage Controller"));

//
// SetVar
//

#define SETVAR_DWORD 1
#define SETVAR_STR   2

//
#define INIT_SET(var_name, name, type)\
    var_name.SetType(type); var_name.SetName(name);

class SetVar
{
public:
    SetVar();
    ~SetVar();

    int SetName(TCHAR* name);
    TCHAR* GetName();

    int SetType(int type);
    int GetType();

    SetVar& operator= (DWORD dw);
    SetVar& operator= (TCHAR* str);
    operator DWORD ();
    operator TCHAR* ();

private:
    int nType;
    std::tstring szName;
    DWORD dwVal;
    std::tstring sVal;
};

class Register
{
public:
	Register(void);
	~Register(void);

public:
	static bool LoadSetting(SetVar& var, std::tstring path = _T("Software\\S3\\S3Signage\\"));
	static bool SaveSetting(SetVar& var, std::tstring path = _T("Software\\S3\\S3Signage\\"));

private:

    static int LoadVar(SetVar& var, HKEY hKey);
    static int SaveVar(SetVar& var, HKEY hKey);
};

