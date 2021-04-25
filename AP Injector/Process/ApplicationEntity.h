#ifndef APPLICATIONENTITY_H
#define APPLICATIONENTITY_H

#include <windows.h>
#include <string>

namespace Process
{
	class ApplicationEntity
	{
	public:
		ApplicationEntity();
		ApplicationEntity(std::wstring applicationName, HWND hwnd = 0, HICON icon = NULL);
		~ApplicationEntity();

		void setIcon(HICON icon);
		HICON getIcon();
		void setHwnd(HWND _hwnd);
		HWND getHwnd();
		void setApplicaitonName(std::wstring applicaitonName);
		std::wstring getApplicaitonName();
	private:
		HICON Icon;
		std::wstring ApplicationName;
		HWND _HWND;
	};
};

#endif

