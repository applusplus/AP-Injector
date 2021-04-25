
#ifndef INJECTION_H
#define INJECTION_H

#include <windows.h>
#include <string>
#include <stdlib.h>
#include <cstdio>
#include <direct.h>
#include <iostream>
#include <tlhelp32.h>
#include <io.h>

#define MAXWAIT 10000
#define INJECTION_LOOK_FOR_WINDOW_PID -16

namespace Injection
{
	enum InjectionResult {SUCCESS, BAD_PROCESS_NAME, BAD_DLL_PATH, MISSING_PROCESS, MISSING_PRIVILEGES, REMOTE_THREAD_NULL, REMOTE_THREAD_TIMEOUT, ALREADY_LOADED, UNKNOWN};
	enum InjectionModule {INJECT, EJECT};

	InjectionResult InjectDll(DWORD processID, std::wstring processName, std::wstring dllPath);
	InjectionResult EjectDll(DWORD processID, std::wstring processName, std::wstring dllPath);
	DWORD GetProcessIDbyName(std::wstring processName);
	bool IsProcessExists(DWORD processID);
	std::wstring StringToWiString(const std::string& s);
	DWORD GetProcessIDbyWindow(std::wstring windowName);
	bool GetModuleByPath(DWORD processID, std::wstring exePath, MODULEENTRY32& mResult);
	InjectionResult executer(DWORD processID, std::wstring processName, std::wstring dllPath, InjectionModule injModule = INJECT);
	InjectionResult execInject(HANDLE& hProcess, std::wstring dllPath);
	InjectionResult execEject(HANDLE& hProcess, MODULEENTRY32& module);
};

#endif

