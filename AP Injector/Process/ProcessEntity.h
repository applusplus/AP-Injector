
#ifndef PROCESSENTITY_H
#define PROCESSENTITY_H

#include <windows.h>
#include <string>

namespace Process
{
	struct ProcessEntity
	{
	private:
		int PID;
		HICON Icon;
		std::wstring ProcessName;
		std::wstring ProcessFilePath;
	public:
		ProcessEntity();
		ProcessEntity(int pid, std::wstring processName, HICON icon = NULL, std::wstring processFilePath = L"");
		~ProcessEntity();

		void setPID(int pid);
		int getPID();
		void setIcon(HICON icon);
		HICON getIcon();
		void setProcessName(std::wstring processName);
		std::wstring getProcessName();
		void setProcessFilePath(std::wstring processFilePath);
		std::wstring getProcessFilePath();
	};
};

#endif