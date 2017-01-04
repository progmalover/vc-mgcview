#include "StdAfx.h"
#include "Register.h"


//
// SetVar
//

SetVar::SetVar()
{
    dwVal = 0;
    sVal = _T("");
    nType = SETVAR_DWORD;
}

SetVar::~SetVar()
{
}

int SetVar::SetName(TCHAR* name)
{
    szName = name;
    return 0;
}

TCHAR* SetVar::GetName()
{
    return (TCHAR*)szName.c_str();
}

int SetVar::SetType(int type)
{
    nType = type;
    return 0;
}

int SetVar::GetType()
{
    return nType;
}

SetVar& SetVar::operator= (DWORD dw)
{
    dwVal = dw;
    return *this;
}

SetVar& SetVar::operator= (TCHAR* str)
{
    sVal = str;
    return *this;
}

SetVar::operator DWORD ()
{
    return dwVal;
}

SetVar::operator TCHAR* ()
{
    return (TCHAR*)sVal.c_str();
}

///////////////////////////////////////////////////////////////

Register::Register(void)
{
}


Register::~Register(void)
{
}

bool Register::LoadSetting(SetVar& var, std::tstring path)
{
	HKEY hKey = NULL;

	CString rootKey;
	rootKey.Format(path.c_str());

	RegOpenKeyEx(HKEY_CURRENT_USER, rootKey, 0, KEY_ALL_ACCESS, &hKey);

	if (!hKey)
	{
		return false;
	}

    if (LoadVar(var, hKey) < 0)
    {
        RegCloseKey(hKey);
        return false;
    }

	RegCloseKey(hKey);

	return true;
}

bool Register::SaveSetting(SetVar& var, std::tstring path)
{
	HKEY hKey = NULL;

	CString rootKey;
	rootKey.Format(path.c_str());

	RegCreateKeyEx(HKEY_CURRENT_USER, rootKey, 0, NULL, 0, 
		KEY_ALL_ACCESS, NULL, &hKey, NULL);

	if (!hKey)
	{
		return false;
	}

    SaveVar(var, hKey);

	RegCloseKey(hKey);

	return true;
}

int Register::LoadVar(SetVar& var, HKEY hKey)
{
    switch (var.GetType())
    {
    case SETVAR_DWORD:
        {
            DWORD dwNum;
            DWORD dwType = REG_DWORD;
            DWORD cbData = sizeof(dwNum);

            if (RegQueryValueEx(hKey, var.GetName(), NULL, 
                &dwType, (LPBYTE)&dwNum, &cbData) != ERROR_SUCCESS)
                return -1;

            var = dwNum;
        }
        break;

    case SETVAR_STR:
        {
            TCHAR buf[512];
            DWORD dwType = REG_SZ;
            DWORD cbData = sizeof(buf);

            // read the TString
            if (RegQueryValueEx(hKey, var.GetName(), NULL, 
                &dwType, (LPBYTE)buf, &cbData) != ERROR_SUCCESS)
                return -1;

            var = buf;
        }
        break;
    }

    return 0;
}

int Register::SaveVar(SetVar& var, HKEY hKey)
{
    switch (var.GetType())
    {
    case SETVAR_DWORD:
        {
            DWORD dwNum = var;
            RegSetValueEx(hKey, var.GetName(),	0, REG_DWORD, (LPBYTE)&dwNum, sizeof(dwNum));
        }
        break;

    case SETVAR_STR:
        {
            TCHAR* str = var;
            RegSetValueEx(hKey, var.GetName(),	0, REG_SZ, (LPBYTE)str, (DWORD)(_tcslen(str)+1)*sizeof(TCHAR));
        }
        break;
    }

    return 0;
}
