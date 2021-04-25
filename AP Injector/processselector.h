#ifndef PROCESSELECTOR_H
#define PROCESSELECTOR_H

#include <QDialog>
#include <QStandardItemModel>
#include <QString>

#include <qt_windows.h>
#include <vector>

#include "ui_ProcessSelector.h"

#include "Process/ProcessManager.h"
#include "Process/ProcessEntity.h"

class ProcessSelector : public QDialog
{
	Q_OBJECT

public:
	ProcessSelector(Process::ProcessEntity *chosenProcess, QWidget *parent = 0);
	~ProcessSelector();

private:
	Ui::FormProcessSelector ui;

	QStandardItemModel *standardItemModel;
	std::vector<Process::ProcessEntity> processes;
	Process::ProcessEntity *ChosenProcess;
	void initTablewView();
	void initProcessList();
private slots:
	void on_tableViewProcesses_doubleClicked(QModelIndex modelIndex);
};

#endif // PROCESSELECTOR_H