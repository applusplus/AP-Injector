#ifndef APPLICATIONSELECTOR_H
#define APPLICATIONSELECTOR_H

#include <QDialog>
#include <QStandardItemModel>
#include <QString>

#include "ui_ApplicationSelector.h"

#include "Process/ApplicationEntity.h"

BOOL CALLBACK WindowCallback(HWND hwnd,LPARAM lParam);

class ApplicationSelector : public QDialog
{
	Q_OBJECT

public:
	ApplicationSelector(Process::ApplicationEntity *chosenApplication, QWidget *parent = 0);
	~ApplicationSelector();

private:
	Ui::ApplicationSelector ui;
	QStandardItemModel *standardItemModel;	
	Process::ApplicationEntity *ChosenApplication;
	void initTablewView();
	void initApplicationList();
	
private slots:
	void on_tableViewApplication_doubleClicked(QModelIndex modelIndex);
};

#endif // APPLICATIONSELECTOR_H
