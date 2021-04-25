#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <vector>
#include <iostream>
#include <shellapi.h>

#include "Process/ProcessEntity.h"

namespace Process
{
	class ProcessManager
	{
	private:
	public:
		ProcessManager();
		~ProcessManager();

		void getRunningProcesses(std::vector<ProcessEntity> &processes);
	};
};

#endif
