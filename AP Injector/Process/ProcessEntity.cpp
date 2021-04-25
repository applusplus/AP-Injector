#include "ProcessEntity.h"

namespace Process
{
	ProcessEntity::ProcessEntity()
	{

	}

	ProcessEntity::ProcessEntity(int pid, std::wstring processName, HICON icon, std::wstring processFilePath)
	{
		PID = pid;
		Icon = icon;
		ProcessName = processName;
		ProcessFilePath = processFilePath;
	}

	ProcessEntity::~ProcessEntity()
	{

	}

	void ProcessEntity::setPID(int pid) 
	{
		PID = pid;
	}

	int ProcessEntity::getPID() 
	{
		return PID;
	}

	void ProcessEntity::setIcon(HICON icon)
	{
		Icon = icon;
	}

	HICON ProcessEntity::getIcon()
	{
		return Icon;
	}

	void ProcessEntity::setProcessName(std::wstring processName) 
	{
		ProcessName = processName;
	}

	std::wstring ProcessEntity::getProcessName() 
	{
		return ProcessName;
	}

	void ProcessEntity::setProcessFilePath( std::wstring processFilePath )
	{
		ProcessFilePath = processFilePath;
	}

	std::wstring ProcessEntity::getProcessFilePath()
	{
		return ProcessFilePath;
	}
};
