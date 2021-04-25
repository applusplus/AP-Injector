#pragma once

#include <QObject>
#include <QString>
#include <QThread>
#include "Injection/Injection.h"
#include "Process/ProcessEntity.h"

class InjectionWorker : public QObject
{
	Q_OBJECT
public:
	InjectionWorker();
	~InjectionWorker();
		
public slots:
	void injectionExecutor(Process::ProcessEntity *targetProcess, int delayTime);
	void ejectionExecutor(Process::ProcessEntity *targetProcess);
	void injectionCancel();
signals:
	void injectionDone(int injectionResult);
	void ejectionDone(int injectionResult);
private:
	bool IsNotCanceled;	
	void inject(Process::ProcessEntity *targetProcess, int delayTime);
	void eject(Process::ProcessEntity *targetProcess);
};

