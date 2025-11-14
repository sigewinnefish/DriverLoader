#pragma once
#include <Windows.h>
#include <winternl.h>
inline char path[MAX_PATH] = { 0 };
void initDriverLoader();
void DriverInstall();
void DriverStart();
void DriverStop();
void DriverUnInstall();