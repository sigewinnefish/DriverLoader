#include "DriverLoader.h"
#include <filesystem>

#pragma comment(lib, "ntdll.lib")

namespace fs = std::filesystem;
typedef NTSTATUS(NTAPI* pfnNtLoadDriver)(PUNICODE_STRING);
typedef NTSTATUS(NTAPI* pfnRtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);

HMODULE ntdll;

void initDriverLoader()
{
	ntdll = GetModuleHandle(L"ntdll.dll");
	if (!ntdll)
	{
		return;
	}

	pfnRtlAdjustPrivilege RtlAdjustPrivilege = (pfnRtlAdjustPrivilege)GetProcAddress(ntdll, "RtlAdjustPrivilege");
	if (!RtlAdjustPrivilege)
	{
		return;
	}

	LUID luid;
	LookupPrivilegeValue(NULL, SE_LOAD_DRIVER_NAME, &luid);

	BOOLEAN bEnabled;
	RtlAdjustPrivilege(luid.LowPart, TRUE, FALSE, &bEnabled);
}

void DriverInstall()
{
	fs::path driverPath = ::path;
	CHAR subKey[MAX_PATH] = "System\\CurrentControlSet\\Services\\";
	strcat_s(subKey, driverPath.stem().string().data());
	HKEY hKey = NULL;
	DWORD type = SERVICE_KERNEL_DRIVER;
	DWORD start = SERVICE_DEMAND_START;
	DWORD errorControl = SERVICE_ERROR_NORMAL;
	CHAR imagePath[MAX_PATH] = "\\??\\";
	strcat_s(imagePath, driverPath.string().data());
	RegCreateKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
	RegSetValueExA(hKey,"Type",NULL,REG_DWORD, (BYTE*)&type,sizeof(type));
	RegSetValueExA(hKey, "Start", NULL, REG_DWORD, (BYTE*)&start, sizeof(start));
	RegSetValueExA(hKey, "ErrorControl", NULL, REG_DWORD, (BYTE*)&errorControl, sizeof(errorControl));
	RegSetValueExA(hKey, "ImagePath", NULL, REG_EXPAND_SZ, (BYTE*)&imagePath, (DWORD)(sizeof(char)*(strlen(imagePath)+1)));
	RegCloseKey(hKey);
	return;
}

void DriverUninstall()
{
	fs::path driverPath = ::path;
	CHAR subKey[MAX_PATH] = "System\\CurrentControlSet\\Services\\";
	strcat_s(subKey, driverPath.stem().string().data());
	HKEY hKey = NULL;
	RegCreateKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
	RegDeleteKeyA(HKEY_LOCAL_MACHINE, subKey);
	RegCloseKey(hKey);
	return;
}

void DriverStart() 
{
	pfnNtLoadDriver NtLoadDriver = (pfnNtLoadDriver)GetProcAddress(ntdll, "NtLoadDriver"); //https://ntdoc.m417z.com/ntloaddriver
	if (!NtLoadDriver)
	{
		return;
	}
	WCHAR service[MAX_PATH] = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\";
	fs::path driverPath = ::path;
	wcscat_s(service, driverPath.stem().wstring().data());
	UNICODE_STRING ustr;
	RtlInitUnicodeString(&ustr, service);
	NtLoadDriver(&ustr);
}


void DriverStop()
{
	pfnNtLoadDriver NtUnloadDriver = (pfnNtLoadDriver)GetProcAddress(ntdll, "NtUnloadDriver"); //https://ntdoc.m417z.com/ntunloaddriver
	if (!NtUnloadDriver)
	{
		return;
	}
	WCHAR service[MAX_PATH] = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\";
	fs::path driverPath = ::path;
	wcscat_s(service, driverPath.stem().wstring().data());
	UNICODE_STRING ustr;
	RtlInitUnicodeString(&ustr, service);
	NtUnloadDriver(&ustr);
}