#include "ApplicationEntity.h"

namespace Process
{
	ApplicationEntity::ApplicationEntity()
	{
	}

	ApplicationEntity::ApplicationEntity(std::wstring applicationName, HWND _hwnd, HICON icon)
	{
		ApplicationName = applicationName;
		_HWND = _hwnd;
		Icon = icon;
	}

	ApplicationEntity::~ApplicationEntity()
	{
	}

	void ApplicationEntity::setIcon(HICON icon)
	{
		Icon = icon;
	}

	HICON ApplicationEntity::getIcon()
	{
		return Icon;
	}

	void ApplicationEntity::setHwnd(HWND _hwnd)
	{
		_HWND = _hwnd;
	}

	HWND ApplicationEntity::getHwnd()
	{
		return _HWND;
	}

	void ApplicationEntity::setApplicaitonName(std::wstring applicaitonName)
	{
		ApplicationName = applicaitonName;
	}

	std::wstring ApplicationEntity::getApplicaitonName()
	{
		return ApplicationName;
	}
};
