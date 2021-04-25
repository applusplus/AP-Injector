#include "ProcessManager.h"

namespace Process
{
	ProcessManager::ProcessManager()
	{
	}


	ProcessManager::~ProcessManager()
	{
	}

	void ProcessManager::getRunningProcesses(std::vector<ProcessEntity> &processes)
	{
		HANDLE hSnapshot;
		PROCESSENTRY32 peProcess;

		if((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0))!= INVALID_HANDLE_VALUE)
		{
			peProcess.dwSize = sizeof(PROCESSENTRY32);
			if(Process32First(hSnapshot, &peProcess))
			{
				while(Process32Next(hSnapshot, &peProcess))
				{
					std::wstring filePath = peProcess.szExeFile;
					if (filePath.find(L".exe") != std::string::npos)
					{
						DWORD size = MAX_PATH + 100;
						wchar_t *buffer = new wchar_t[size];
						HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | SYNCHRONIZE, false, peProcess.th32ProcessID);
						QueryFullProcessImageName(processHandle, 0, buffer, &size);
						SHFILEINFO FileInfo = { 0 };
						SHGetFileInfo(buffer, 0, &FileInfo, sizeof(FileInfo), SHGFI_ICON | SHGFI_LARGEICON);
						if(FileInfo.hIcon > 0) {
							ProcessEntity processEntity((int)peProcess.th32ProcessID, filePath, FileInfo.hIcon, buffer);
							processes.push_back(processEntity);
						}
						else
						{
							DestroyIcon(FileInfo.hIcon);
						}
						CloseHandle(processHandle);
					}
				}
			}
		}
		CloseHandle(hSnapshot);
	}
};
