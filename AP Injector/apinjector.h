#ifndef APINJECTOR_H
#define APINJECTOR_H

#include <QtWidgets/QMainWindow>
#include <QSystemTrayIcon>
#include <QIntValidator>
#include <QMessageBox>
#include <QThread>
#include <QFileInfo>
#include <QFileDialog>
#include <QString>
#include <QAction>
#include <QMenu>
#include <QCloseEvent>
#include <QTextStream>

#include "ui_apinjector.h"

#include <Process/ProcessEntity.h>
#include <Process/ApplicationEntity.h>
#include "InjectionWorker.h"
#include "Injection/Injection.h"
#include "Settings/SettingsEntity.h"

#define PROGRAM_NAME "AP Injector"
#define SETTINGS_FILE_NAME "ap.applusplus"

class APInjector : public QMainWindow
{
	Q_OBJECT
public:
	APInjector(QWidget *parent = 0);
	~APInjector();

private:
	Ui::APInjectorClass ui;

	QThread InjectionThread;
	InjectionWorker* injectionWorker;
	QSystemTrayIcon *trayIcon;
	QMenu *trayMenu;
	Process::ProcessEntity ChosenProcess;
	Process::ApplicationEntity ChosenApplication;
	Process::ProcessEntity *TargetProcess;
	QString cfgFilePath;
	QStringList ProcessNameList;
	QStringList ApplicationNameList;

	void SetAddPidComboBoxItem(int pid);
	void SetAddPNameComboBoxItem(QString processName, QStringList* itemList);
	void SetPNameComboBoxItems(QStringList items);
	void SetAddDllPathComboBoxItem(QString dllFilePath);
	bool CheckForFieldsValidation();
	bool InjectionStateNotify(Injection::InjectionResult injectionResult);
	void SetEnabledForInjection(bool isEnabled);
	void SetEnabledForEjection(bool isEnabled);
	void SetInjectionStatus(QString statusText = "Standby", QColor color = Qt::GlobalColor::darkCyan);
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *e);
	void manageArguments(QStringList args);
	void switchSearchMode(bool isApplication = true);
	void saveSettings();
	void loadSettings();
private slots:
	void quit();
	void trayIconOnClick(QSystemTrayIcon::ActivationReason reason);
	void on_butAddNewProcesses_clicked();
	void on_butDllSearchPath_clicked();
	void on_butInjection_clicked();
	void on_butEject_clicked();
	void on_rbProcess_clicked();
	void on_rbApplication_clicked();
	void injectionComplete(int injectionResult);
	void ejectionComplete(int injectionResult);
signals:
	void injectDll(Process::ProcessEntity *targetProcess, int delayTime);
	void ejectDll(Process::ProcessEntity *targetProcess);
	void cancelInjection();
};

#endif
