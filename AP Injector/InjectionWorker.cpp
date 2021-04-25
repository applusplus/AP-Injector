#include "InjectionWorker.h"


InjectionWorker::InjectionWorker()
{
	IsNotCanceled = true;
}


InjectionWorker::~InjectionWorker()
{
	
}

void InjectionWorker::injectionExecutor(Process::ProcessEntity *targetProcess, int delayTime)
{
	DWORD pid = targetProcess->getPID();
	DWORD tmpPid = pid;
	std::wstring processName = targetProcess->getProcessName();
	if ((int)pid < 1 || !Injection::IsProcessExists(pid))
	{
		if ((int)pid == INJECTION_LOOK_FOR_WINDOW_PID)
		{
			for (;IsNotCanceled;)
			{
				tmpPid = Injection::GetProcessIDbyWindow(processName);
				if ((int)tmpPid  > 0)
				{
					break;
				}
				QThread::msleep(100);
			}
		}
		else
		{
			for (;IsNotCanceled;)
			{
				tmpPid = Injection::GetProcessIDbyName(processName);
				if ((int)tmpPid  > 0)
				{
					break;
				}
				QThread::msleep(100);
			}
		}
	}
	if ((int)tmpPid > 1)
	{
		targetProcess->setPID(tmpPid);
		inject(targetProcess, delayTime);
	}
	if (!IsNotCanceled)
	{
		IsNotCanceled = true;
	}
}

void InjectionWorker::injectionCancel()
{
	IsNotCanceled = false;
}

void InjectionWorker::inject(Process::ProcessEntity *targetProcess, int delayTime)
{
	if (delayTime < 300)
	{
		delayTime *= 1000;
	}	
	QThread::msleep(delayTime);
	Injection::InjectionResult injectionResult = Injection::InjectDll(targetProcess->getPID(), targetProcess->getProcessName(), targetProcess->getProcessFilePath());
	injectionDone(injectionResult);
}

void InjectionWorker::ejectionExecutor(Process::ProcessEntity *targetProcess)
{
	DWORD pid = targetProcess->getPID();
	DWORD tmpPid = pid;
	std::wstring processName = targetProcess->getProcessName();
	if ((int)pid < 1 || !Injection::IsProcessExists(pid))
	{
		if ((int)pid == INJECTION_LOOK_FOR_WINDOW_PID)
		{
			tmpPid = Injection::GetProcessIDbyWindow(processName);
		}
		else
		{
			tmpPid = Injection::GetProcessIDbyName(processName);
		}
	}
	if ((int)tmpPid > 1)
	{
		targetProcess->setPID(tmpPid);
		eject(targetProcess);
	}
}

void InjectionWorker::eject(Process::ProcessEntity *targetProcess)
{
	Injection::InjectionResult injectionResult = Injection::EjectDll(targetProcess->getPID(), targetProcess->getProcessName(), targetProcess->getProcessFilePath());
	ejectionDone(injectionResult);
}