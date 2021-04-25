#include "processselector.h"

Q_GUI_EXPORT QPixmap qt_pixmapFromWinHICON(HICON icon);

ProcessSelector::ProcessSelector(Process::ProcessEntity *chosenProcess, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ChosenProcess = chosenProcess;
	initTablewView();
	initProcessList();
}

ProcessSelector::~ProcessSelector()
{
	if (processes.size() > 0)
	{
		for (Process::ProcessEntity process : processes)
		{
			DestroyIcon(process.getIcon());
		}
	}
}

void ProcessSelector::initTablewView()
{
	standardItemModel = new QStandardItemModel(1,3,this); //undefined Rows and 3 Columns
	standardItemModel->setHorizontalHeaderItem(0, new QStandardItem(QString("Icon")));
	standardItemModel->setHorizontalHeaderItem(1, new QStandardItem(QString("PID")));
	standardItemModel->setHorizontalHeaderItem(2, new QStandardItem(QString("Name")));

	ui.tableViewProcesses->setModel(standardItemModel);
	ui.tableViewProcesses->setColumnWidth(0, 40);
	ui.tableViewProcesses->setColumnWidth(1, 80);
	ui.tableViewProcesses->setColumnWidth(2, 140);
}

void ProcessSelector::initProcessList()
{
	Process::ProcessManager processManager;	
	processManager.getRunningProcesses(processes);
	standardItemModel->setRowCount(processes.size());
	for (int i = 0; i < processes.size(); i++)
	{
		QStandardItem *standartdItem = new QStandardItem(QIcon(qt_pixmapFromWinHICON(processes[i].getIcon())), tr(""));
		standartdItem->setTextAlignment(Qt::AlignCenter);
		standardItemModel->setItem(i, 0, standartdItem);
		standartdItem = new QStandardItem(tr("%1").arg(processes[i].getPID()));
		standartdItem->setTextAlignment(Qt::AlignCenter);
		standardItemModel->setItem(i ,1, standartdItem);
		standartdItem = new QStandardItem(QString::fromStdWString(processes[i].getProcessName()));
		standartdItem->setTextAlignment(Qt::AlignCenter);
		standardItemModel->setItem(i, 2, standartdItem);
	}
}

void ProcessSelector::on_tableViewProcesses_doubleClicked(QModelIndex modelIndex)
{
	int pid = standardItemModel->item(modelIndex.row(), 1)->text().toInt();
	if (pid > 0)
	{
		for(Process::ProcessEntity process : processes)
		{
			if (process.getPID() == pid)
			{
				*ChosenProcess = process;
				break;
			}
		}
	}	
	this->accept();
	this->close();
}