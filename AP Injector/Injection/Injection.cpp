
#include "Injection.h"

namespace Injection
{
	InjectionResult Injection::InjectDll(DWORD processID, std::wstring processName, std::wstring dllPath)
	{
		return executer(processID, processName, dllPath);
	}

	InjectionResult Injection::EjectDll(DWORD processID, std::wstring processName, std::wstring dllPath )
	{
		return executer(processID, processName, dllPath, EJECT);
	}

	InjectionResult Injection::executer(DWORD processID, std::wstring processName, std::wstring dllPath, InjectionModule injModule) 
	{
		InjectionResult result = UNKNOWN;
		DWORD PID = processID;
		if(PID < 1)
		{
			if (processName.find(L".exe") != std::wstring::npos)
			{
				PID = GetProcessIDbyName(processName);
			}
			else if(PID = INJECTION_LOOK_FOR_WINDOW_PID)
			{
				PID = GetProcessIDbyWindow(processName);
			}
			else
			{
				result = BAD_PROCESS_NAME;
			}
		}
		if(result != BAD_PROCESS_NAME) 
		{
			if(_waccess(dllPath.c_str(), _A_NORMAL) == -1)
			{
				result = BAD_DLL_PATH;
			}
			else
			{
				MODULEENTRY32 mModule;
				if(GetModuleByPath(PID, dllPath, mModule) && injModule != EJECT) 
				{
					result = ALREADY_LOADED;
				}
				else
				{
					HANDLE hToken;
					TOKEN_PRIVILEGES tokenPrivileges;
					if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
					{
						result = MISSING_PRIVILEGES;
					}
					else
					{
						LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tokenPrivileges.Privileges[0].Luid);
						tokenPrivileges.PrivilegeCount = 1;
						tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
						if(!AdjustTokenPrivileges(hToken, 0, &tokenPrivileges, sizeof(tokenPrivileges), NULL, NULL))
						{
							result = MISSING_PRIVILEGES;
						}
						else
						{
							HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
							if (hProcess == NULL)
							{
								result = MISSING_PROCESS;
							}
							else
							{
								switch (injModule)
								{
								case Injection::INJECT:
									result = execInject(hProcess, dllPath);
									break;
								case Injection::EJECT:
									result = execEject(hProcess, mModule);
									break;
								default:
									break;
								}
								CloseHandle(hProcess);
							}
						}
					}
				}
			}
		}
		
		return result;
	}

	InjectionResult Injection::execInject(HANDLE& hProcess, std::wstring dllPath) 
	{
		InjectionResult result = UNKNOWN;
		
		HMODULE hLocKernel32 = GetModuleHandleA("Kernel32");
		FARPROC hLocLoadLibrary = GetProcAddress(hLocKernel32, "LoadLibraryW");

		dllPath += L'\0';
		int pathSize = dllPath.size() * sizeof(std::wstring::value_type);
		LPVOID hRemoteMem = VirtualAllocEx(hProcess, NULL, pathSize, MEM_COMMIT, PAGE_READWRITE);

		DWORD numBytesWritten;
		if(WriteProcessMemory(hProcess, hRemoteMem, dllPath.c_str(), pathSize, &numBytesWritten))
		{
			HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)hLocLoadLibrary, hRemoteMem, 0, NULL);
			if (!hRemoteThread)
			{
				result = REMOTE_THREAD_NULL;
			}
			else
			{
				if(WaitForSingleObject(hRemoteThread, MAXWAIT) == WAIT_TIMEOUT)
				{
					result = REMOTE_THREAD_TIMEOUT;
				}
				else
				{
					result = SUCCESS;
				}
			}
			VirtualFreeEx(hProcess, hRemoteMem, pathSize, MEM_RELEASE);
		}

		return result;
	}

	InjectionResult Injection::execEject(HANDLE& hProcess, MODULEENTRY32& module) 
	{
		InjectionResult result = UNKNOWN;

		HMODULE hLocKernel32 = GetModuleHandleA("Kernel32");
		FARPROC hLocLoadLibrary = GetProcAddress(hLocKernel32, "FreeLibrary");

		HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)hLocLoadLibrary, module.hModule, 0, NULL);
		if (!hRemoteThread)
		{
			result = REMOTE_THREAD_NULL;
		}
		else
		{
			if(WaitForSingleObject(hRemoteThread, MAXWAIT) == WAIT_TIMEOUT)
			{
				result = REMOTE_THREAD_TIMEOUT;
			}
			else
			{
				result = SUCCESS;
			}
		}	

		return result;
	}

	DWORD Injection::GetProcessIDbyName(std::wstring processName)
	{
		HANDLE hSnapshot;
		PROCESSENTRY32 peProcess;
		DWORD dwResult = 0;

		if((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0))!= INVALID_HANDLE_VALUE)
		{
			peProcess.dwSize = sizeof(PROCESSENTRY32);
			if(Process32First(hSnapshot, &peProcess))
			{
				while(Process32Next(hSnapshot, &peProcess))
				{
					if(processName == peProcess.szExeFile)
					{
						dwResult =  peProcess.th32ProcessID;
						break;
					}
				}
			}
	   }
	   CloseHandle(hSnapshot);

	   return dwResult;
	}

	DWORD Injection::GetProcessIDbyWindow(std::wstring windowName)
	{
		DWORD pid = 0;
		HWND windowHwnd = FindWindow(NULL, windowName.c_str());		
		GetWindowThreadProcessId(windowHwnd, &pid);
		return pid;
	}

	bool IsProcessExists(DWORD processID)
	{
		bool isExists = false;
		HANDLE hSnapshot;
		PROCESSENTRY32 peProcess;
		
		if((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0)) != INVALID_HANDLE_VALUE)
		{
			peProcess.dwSize = sizeof(PROCESSENTRY32);
			if(Process32First(hSnapshot, &peProcess))
			{
				while(Process32Next(hSnapshot, &peProcess))
				{
					if(peProcess.th32ProcessID == processID)
					{
						isExists = true;
						break;
					}
				}
			}
		}
		CloseHandle(hSnapshot);

		return isExists;
	}

	bool Injection::GetModuleByPath(DWORD processID, std::wstring exePath, MODULEENTRY32& mResult)
	{
		bool result = false;
		HANDLE hSnapshot;
		MODULEENTRY32 mModule;

		if((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID)) != INVALID_HANDLE_VALUE)
		{
			mModule.dwSize = sizeof(MODULEENTRY32);
			if(Module32First(hSnapshot, &mModule))
			{
				while(Module32Next(hSnapshot, &mModule))
				{
					if(mModule.szExePath == exePath)
					{
						result = true;
						mResult = mModule;
						break;
					}
				}
			}
		}
		CloseHandle(hSnapshot);

		return result;
	}

	std::wstring Injection::StringToWiString(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	} 
};