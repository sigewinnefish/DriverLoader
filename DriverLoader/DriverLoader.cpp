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

void DriverStart() 
{
	pfnNtLoadDriver NtLoadDriver = (pfnNtLoadDriver)GetProcAddress(ntdll, "NtLoadDriver"); //https://ntdoc.m417z.com/ntloaddriver
	if (!NtLoadDriver)
	{
		return;
	}
	WCHAR service[MAX_PATH] = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\";
	fs::path driverPath = ::path;
	wcscat(service, driverPath.stem().wstring().data());
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
	wcscat(service, driverPath.stem().wstring().data());
	UNICODE_STRING ustr;
	RtlInitUnicodeString(&ustr, service);
	NtUnloadDriver(&ustr);
}