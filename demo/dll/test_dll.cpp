#include "resource.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include <iostream>

using std::cout;
using std::cin;
using std::endl;

#include "udllman.h"
#include "umsg.h"

/*
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif

#if _WIN32_WINNT >= 0x0502

WINBASEAPI
BOOL
WINAPI
SetDllDirectoryA(
    LPCSTR lpPathName
    );
WINBASEAPI
BOOL
WINAPI
SetDllDirectoryW(
    LPCWSTR lpPathName
    );
#ifdef UNICODE
#define SetDllDirectory  SetDllDirectoryW
#else
#define SetDllDirectory  SetDllDirectoryA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetDllDirectoryA(
    DWORD nBufferLength,
    LPSTR lpBuffer
    );
WINBASEAPI
DWORD
WINAPI
GetDllDirectoryW(
    DWORD nBufferLength,
    LPWSTR lpBuffer
    );
#ifdef UNICODE
#define GetDllDirectory  GetDllDirectoryW
#else
#define GetDllDirectory  GetDllDirectoryA
#endif // !UNICODE

#endif // _WIN32_WINNT >= 0x0502
*/

int main(int argc, char *argv[])
{
    try {
        DEF_DLLFUNC(SetDllDirectory, BOOL, const TCHAR *);
        DEF_DLLFUNC(GetDllDirectory, DWORD, DWORD, TCHAR *);
        UDllMan udm;
        udm.load(_T("kernel32.dll"));

        //SetDllDirectory func = (SetDllDirectory)udm.find(_T("SetDllDirectoryA"));

        //func(_T("D:\\ulib-win\\bin"));

        //typedef DllFunc<BOOL, const TCHAR *>::type pfnType;;
        //pfnType pfn = (pfnType)udm.find(_T("SetDllDirectoryA"));
        //pfn(_T("D:\\ulib-win\\bin"));

       //UDllMan::DllFunc<BOOL, const TCHAR *>::callEx(udm, _T("SetDllDirectoryA"), _T("D:\\ulib-win\\bin"));
       udm.callFunc<BOOL, const TCHAR *>(_T("SetDllDirectoryA"), _T("D:\\ulib-win\\bin"));
        //GetDllDirectory foo = (GetDllDirectory)udm.find(_T("GetDllDirectoryA"));

        TCHAR buffer[5024];

        //int n = foo(5024, buffer);

       //int n = UDllMan::DllFunc2<DWORD, DWORD, TCHAR*>::callEx(udm, _T("GetDllDirectoryA"), 5024, buffer);
        int n = udm.callFunc<DWORD, DWORD, TCHAR*>(_T("GetDllDirectoryA"), 5024, buffer);
        cout << n <<endl;
        //cout << GetLastError() << endl;
        //showErrorByNum(GetLastError());
        cout << buffer << endl;


        udm.free();
    }
    catch(UDllMan::UDllException *e)
    {
        cout << e->what() << endl;
    }

    try {
        UDllMan udm;
        udm.load("libuwin.dll");

        //typedef void (*GetVolSerialNum)(char *, char *);
        DEF_DLLFUNC(GetVolSerialNum, void, char *, char *);

        GetVolSerialNum get_vol_sn;
        get_vol_sn= (GetVolSerialNum)udm.find("getVolSerialNum@8");
        char ss[256];
        get_vol_sn(ss, "C:\\");
        udm.free();
        cout << ss << endl;
    }
    catch(UDllMan::UDllException *e)
    {
        cout << e->what() << endl;
    }

    cin.ignore();
   return 0;
}


