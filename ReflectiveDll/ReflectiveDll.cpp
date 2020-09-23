//===============================================================================================//
// This is a stub for the actuall functionality of the DLL.
//===============================================================================================//
#include "ReflectiveLoader.h"
#include <Windows.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <shellapi.h>
#pragma comment(lib, "Shlwapi.lib")
static  char* szName;
using namespace std;
std::string szargs;
std::wstring wszargs;
std::wstring wsHostFile;
int argc = 0;
LPSTR* argv = NULL;
// Note: REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR and REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN are
// defined in the project properties (Properties->C++->Preprocessor) so as we can specify our own 
// DllMain and use the LoadRemoteLibraryR() API to inject this DLL.

// You can use this value as a pseudo hinstDLL value (defined and set via ReflectiveLoader.c)


// 0 加载服务    1 启动服务    2 停止服务    3 删除服务
BOOL SystemServiceOperate(char* lpszDriverPath, int iOperateType)
{
	BOOL bRet = TRUE;
	//char szName[MAX_PATH] = { 0 };

	//lstrcpy(szName, lpszDriverPath);
	// 过滤掉文件目录，获取文件名
	//PathStripPath(szName);

	SC_HANDLE shSCManager = NULL, shService = NULL;
	SERVICE_STATUS sStatus;
	DWORD dwErrorCode = 0;

	// 打开服务控制管理器数据库
	shSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (0 != iOperateType)
	{
		// 打开一个已经存在的服务
		shService = OpenService(shSCManager, szName, SERVICE_ALL_ACCESS);
		if (!shService)
		{
			CloseServiceHandle(shSCManager);
			shSCManager = NULL;
			return FALSE;
		}
	}

	switch (iOperateType)
	{
	case 0:
	{
		// 创建服务
		// SERVICE_AUTO_START   随系统自动启动
		// SERVICE_DEMAND_START 手动启动
		shService = CreateService(shSCManager, szName, szName,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			lpszDriverPath, NULL, NULL, NULL, NULL, NULL);
		break;
	}
	case 1:
	{
		// 启动服务
		StartService(shService, 0, NULL);
		break;
	}
	case 2:
	{
		// 停止服务
		ControlService(shService, SERVICE_CONTROL_STOP, &sStatus);
		break;
	}
	case 3:
	{
		// 删除服务
		DeleteService(shService);
		break;
	}
	default:
		break;
	}
	// 关闭句柄

	CloseServiceHandle(shService);
	CloseServiceHandle(shSCManager);

	return TRUE;
}

std::wstring StringToWString(const std::string& str)
{
	int num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wide = new wchar_t[num];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, num);
	std::wstring w_str(wide);
	delete[] wide;
	return w_str;
}

extern HINSTANCE hAppInstance;
//===============================================================================================//
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	BOOL bReturnValue = TRUE;
	switch (dwReason)
	{
	case DLL_QUERY_HMODULE:
		if (lpReserved != NULL)
			*(HMODULE*)lpReserved = hAppInstance;
		break;
	case DLL_PROCESS_ATTACH:
	{
		hAppInstance = hinstDLL;
		/* print some output to the operator */
		if (lpReserved != NULL) {
			szargs = (PCHAR)lpReserved;
			wszargs = StringToWString(szargs);
			argv = CommandLineToArgvW(wszargs.data(), &argc);
		}
		BOOL bRet = FALSE;
		char* szFileName = argv[0];
		szName = argv[1];
		printf("[*] CreateService by Uknow\n");
		if (argc != 3)
		{
			printf("   [+] usage: CreateService BinaryPathName ServiceName start/stop\n");
			printf("   [+] eg: CreateService \"c:\\evil.exe\" EvilService start/stop\n");
			return -1;
		}
		else if (strcmp(argv[2], "start") == 0) {
			bRet = SystemServiceOperate(szFileName, 0);
			if (FALSE == bRet)
			{
				printf("   [-] Create Error!\n");
				return -1;
			}
			bRet = SystemServiceOperate(szFileName, 1);
			if (FALSE == bRet)
			{
				printf("   [-] Start Error!\n");
				return -1;
			}
			printf("    [+] ServiceName: %s\n", szName);
			printf("    [+] BinaryPathName: %s\n", szFileName);
			printf("    [+] Success! Service successfully Create and Start.\n");
			return 0;
		}
		else if (strcmp(argv[2], "stop") == 0)
		{
			bRet = SystemServiceOperate(szFileName, 2);
			if (FALSE == bRet)
			{
				printf("   [-] Stop Error!\n");
				return -1;
			}
			bRet = SystemServiceOperate(szFileName, 3);
			if (FALSE == bRet)
			{
				printf("   [-] Delete Error!\n");
				return -1;
			}
			printf("    [+] ServiceName: %s\n", szName);
			printf("    [+] BinaryPathName: %s\n", szFileName);
			printf("    [+] Success! Service successfully Stop and Delete.\n");
			return 0;
		}
		break;
	}
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return bReturnValue;
}
