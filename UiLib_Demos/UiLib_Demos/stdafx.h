// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"
#include "resource.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlstr.h>
#include <limits>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�


#include "../../Common/Include/UiLib/UIlib.h"

//������ڴ����øú��������ڴ�й¶���
//�����˳�ǰ����_CrtDumpMemoryLeaks();�����ɲ鿴�ڴ�й¶��ID�ţ���ID�ſ���ΪBreakpoint�Ĳ���������ʱ��������Ӧλ���Զ��ϵ�
inline  void  EnableMemLeakCheck(int Breakpoint = 0){
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)  |  _CRTDBG_LEAK_CHECK_DF);
	if(Breakpoint)
		_CrtSetBreakAlloc ( 1912 );
}

#ifdef _DEBUG
#define  new   new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#endif 
#define _CRT_SECURE_NO_DEPRECATE


#if _MSC_VER == 1500
#ifdef _DEBUG
#ifndef UILIB_EXPORTS
#pragma comment(lib, "../../Common/Static_Debug_2008/UiLib_d.lib")
#else
#pragma comment(lib, "../../Common/DLL_Debug_2008/UiLib_d.lib")
#endif
#else
#ifndef UILIB_EXPORTS
#pragma comment(lib, "../../Common/Static_Release_2008/UiLib.lib")
#else
#pragma comment(lib, "../../Common/DLL_Release_2008/UiLib.lib")
#endif
#endif
#endif

#if _MSC_VER == 1600
#ifdef _DEBUG
#ifndef UILIB_EXPORTS
#pragma comment(lib, "../../Common/Static_Debug_2010/UiLib_d.lib")
#else
#pragma comment(lib, "../../Common/DLL_Debug_2010/UiLib_d.lib")
#endif
#else
#ifndef UILIB_EXPORTS
#pragma comment(lib, "../../Common/Static_Release_2010/UiLib.lib")
#else
#pragma comment(lib, "../../Common/DLL_Release_2010/UiLib.lib")
#endif
#endif
#endif

#if _MSC_VER == 1700
#ifdef _DEBUG
#ifndef UILIB_EXPORTS
#pragma comment(lib, "../../Common/Static_Debug_2012/UiLib_d.lib")
#else
#pragma comment(lib, "../../Common/DLL_Debug_2012/UiLib_d.lib")
#endif
#else
#ifndef UILIB_EXPORTS
#pragma comment(lib, "../../Common/Static_Release_2012/UiLib.lib")
#else
#pragma comment(lib, "../../Common/DLL_Release_2012/UiLib.lib")
#endif
#endif
#endif
