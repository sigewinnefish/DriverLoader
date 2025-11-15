#include "pch.h"
#include "loader.h"
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

	#define CHECK_STATUS(status) do { if (status) { ::s = DriverInstallFailed; return; } } while(0)
	CHECK_STATUS(RegCreateKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));
	CHECK_STATUS(RegSetValueExA(hKey,"Type",NULL,REG_DWORD, (BYTE*)&type,sizeof(type)));
	CHECK_STATUS(RegSetValueExA(hKey, "Start", NULL, REG_DWORD, (BYTE*)&start, sizeof(start)));
	CHECK_STATUS(RegSetValueExA(hKey, "ErrorControl", NULL, REG_DWORD, (BYTE*)&errorControl, sizeof(errorControl)));
	CHECK_STATUS(RegSetValueExA(hKey, "ImagePath", NULL, REG_EXPAND_SZ, (BYTE*)&imagePath, (DWORD)(sizeof(char)*(strlen(imagePath)+1))));
	CHECK_STATUS(RegCloseKey(hKey));
	::s = DriverInstallOK;
}

void DriverUninstall()
{
	fs::path driverPath = ::path;
	CHAR subKey[MAX_PATH] = "System\\CurrentControlSet\\Services\\";
	strcat_s(subKey, driverPath.stem().string().data());
	HKEY hKey = NULL;
	#undef CHECK_STATUS
	#define CHECK_STATUS(status) do { if (status) { ::s = DriverUninstallFailed; return; } } while(0)
	CHECK_STATUS(RegCreateKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));
	CHECK_STATUS(RegDeleteKeyA(HKEY_LOCAL_MACHINE, subKey));
	CHECK_STATUS(RegCloseKey(hKey));
	::s = DriverInstallOK;
}

void DriverStart() 
{
	pfnNtLoadDriver NtLoadDriver = (pfnNtLoadDriver)GetProcAddress(ntdll, "NtLoadDriver"); //https://ntdoc.m417z.com/ntloaddriver
	if (!NtLoadDriver)
	{
		::s = DriverStartFailed;
		return;
	}
	WCHAR service[MAX_PATH] = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\";
	fs::path driverPath = ::path;
	wcscat_s(service, driverPath.stem().wstring().data());
	UNICODE_STRING ustr;
	RtlInitUnicodeString(&ustr, service);
	#undef CHECK_STATUS
	#define CHECK_STATUS(status) do { if (status) { ::s = DriverStartFailed; return; } } while(0)
	CHECK_STATUS(NtLoadDriver(&ustr));
	::s = DriverStartOK;
}


void DriverStop()
{
	pfnNtLoadDriver NtUnloadDriver = (pfnNtLoadDriver)GetProcAddress(ntdll, "NtUnloadDriver"); //https://ntdoc.m417z.com/ntunloaddriver
	if (!NtUnloadDriver)
	{
		::s = DriverStopFailed;
		return;
	}
	WCHAR service[MAX_PATH] = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\";
	fs::path driverPath = ::path;
	wcscat_s(service, driverPath.stem().wstring().data());
	UNICODE_STRING ustr;
	RtlInitUnicodeString(&ustr, service);
	#undef CHECK_STATUS
	#define CHECK_STATUS(status) do { if (status) { ::s = DriverStopFailed; return; } } while(0)
	CHECK_STATUS(NtUnloadDriver(&ustr));
}