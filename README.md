# CreateService


### 服务模板代码
```
#include <windows.h>
#include <tchar.h>

//LPSERVICE_MAIN_FUNCTIONA LpserviceMainFunctiona;
TCHAR szServiceName[MAX_PATH] = "ServiceTest.exe";
SERVICE_STATUS_HANDLE g_ServiceStatusHandle = 0;

void MyFunc() {
	HANDLE hFile = NULL;
	TCHAR szText[] = "If you see this, it means service is created successfully : D";
	DWORD dwSize = 0;
	hFile = CreateFile("C:\\Users\\Administrator\\Desktop\\ttt.txt", GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, szText, _countof(szText), &dwSize, NULL);
	CloseHandle(hFile);
}

BOOL SetServiceStatus(DWORD dwServiceCode) {
	SERVICE_STATUS ss = { 0 };

	ss.dwCurrentState = dwServiceCode;
	ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ss.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN |
		SERVICE_ACCEPT_PAUSE_CONTINUE;
	ss.dwWin32ExitCode = 0;
	ss.dwWaitHint = 3000;

	return(SetServiceStatus(g_ServiceStatusHandle, &ss));
}

void __stdcall LphandlerFunction(DWORD dwControl) {
	switch (dwControl) {
	case SERVICE_CONTROL_STOP:
		SetServiceStatus(SERVICE_STOP_PENDING);
		SetServiceStatus(SERVICE_STOPPED);
		break;
	case SERVICE_CONTROL_PAUSE:
		SetServiceStatus(SERVICE_PAUSE_PENDING);
		SetServiceStatus(SERVICE_PAUSED);
		break;
	case SERVICE_CONTROL_CONTINUE:
		SetServiceStatus(SERVICE_CONTINUE_PENDING);
		SetServiceStatus(SERVICE_RUNNING);
		break;
	default:
		break;
	}
}

void __stdcall LpserviceMainFunctiona(
	DWORD dwNumServicesArgs,
	LPSTR* lpServiceArgVectors
) {
	g_ServiceStatusHandle = RegisterServiceCtrlHandler(szServiceName, LphandlerFunction);
	if (0 == g_ServiceStatusHandle)
		return;
	SetServiceStatus(SERVICE_START_PENDING);
	SetServiceStatus(SERVICE_RUNNING);

	while (TRUE) {
		Sleep(5000);
		MyFunc();
	}
}

int _tmain() {
	SERVICE_TABLE_ENTRY stDispatchTable[] = {
		{ szServiceName, LpserviceMainFunctiona },
		{ NULL, NULL }
	};
	StartServiceCtrlDispatcher(stDispatchTable);

	return(0);
}
```
