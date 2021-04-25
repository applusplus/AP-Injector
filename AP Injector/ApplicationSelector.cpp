#include "ApplicationSelector.h"

Q_GUI_EXPORT QPixmap qt_pixmapFromWinHICON(HICON icon);

std::vector<Process::ApplicationEntity> applications;

ApplicationSelector::ApplicationSelector(Process::ApplicationEntity *chosenApplication, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ChosenApplication = chosenApplication;	
	initTablewView();	
	initApplicationList();
}

ApplicationSelector::~ApplicationSelector()
{
	if (applications.size() > 0)
	{
		for (Process::ApplicationEntity application : applications)
		{
			DestroyIcon(application.getIcon());
		}
	}
}

void ApplicationSelector::initTablewView()
{
	standardItemModel = new QStandardItemModel(1,2,this); //undefined Rows and 2 Columns
	standardItemModel->setHorizontalHeaderItem(0, new QStandardItem(QString("Icon")));
	standardItemModel->setHorizontalHeaderItem(1, new QStandardItem(QString("Handle")));
	standardItemModel->setHorizontalHeaderItem(2, new QStandardItem(QString("Name")));

	ui.tableViewApplication->setModel(standardItemModel);
	ui.tableViewApplication->setColumnWidth(0, 35);
	ui.tableViewApplication->setColumnWidth(1, 60);
	ui.tableViewApplication->setColumnWidth(2, 300);
}

void ApplicationSelector::initApplicationList()
{	
	applications.clear();
	EnumWindows(WindowCallback, NULL);
	standardItemModel->setRowCount(applications.size());
	for (int i = 0; i < applications.size(); i++)
	{
		QStandardItem *standartdItem = new QStandardItem(QIcon(qt_pixmapFromWinHICON(applications[i].getIcon())), tr(""));
		standartdItem->setTextAlignment(Qt::AlignCenter);
		standardItemModel->setItem(i, 0, standartdItem);
		standartdItem = new QStandardItem(QString("%1").arg((quintptr)applications[i].getHwnd(), QT_POINTER_SIZE, 16, QChar('0')));
		standartdItem->setTextAlignment(Qt::AlignCenter);
		standardItemModel->setItem(i ,1, standartdItem);
		standartdItem = new QStandardItem(QString::fromStdWString(applications[i].getApplicaitonName()));
		standartdItem->setTextAlignment(Qt::AlignLeft);
		standardItemModel->setItem(i, 2, standartdItem);
	}
}

void ApplicationSelector::on_tableViewApplication_doubleClicked(QModelIndex modelIndex)
{
	QString hwnd = standardItemModel->item(modelIndex.row(), 1)->text();
	if (!hwnd.isEmpty() && hwnd != "00000000")
	{
		for(Process::ApplicationEntity application : applications)
		{
			if (QString("%1").arg((quintptr)application.getHwnd(), QT_POINTER_SIZE, 16, QChar('0')) == hwnd)
			{
				*ChosenApplication = application;
				break;
			}
		}
	}
	this->accept();
	this->close();
}

BOOL CALLBACK WindowCallback(HWND hwnd,LPARAM lParam)
{
	wchar_t pcWinTitle[256]; 
	if(!GetWindow(hwnd, GW_OWNER) && IsWindowVisible(hwnd))
	{
		GetWindowText(hwnd, pcWinTitle, 255);
		QString winTitle = QString::fromStdWString(pcWinTitle);
		if (!winTitle.isEmpty())
		{
			HICON hIcon = (HICON)GetClassLong(hwnd,GCL_HICON);
			Process::ApplicationEntity applicationEntity(pcWinTitle, hwnd, hIcon);
			applications.push_back(applicationEntity);
		}
	}
	return TRUE;
}