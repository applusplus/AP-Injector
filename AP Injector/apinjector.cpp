#include "apinjector.h"
#include "processselector.h"
#include "ApplicationSelector.h"


APInjector::APInjector(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	this->setWindowTitle("AP Injector 1.5 :: by applusplus");

	QAction *actionExit = new QAction("Exit", this);
	connect(actionExit, SIGNAL(triggered()), this, SLOT(quit()));
	trayMenu = new QMenu("Menu", this);
	trayMenu->addAction(actionExit);
	trayIcon = new QSystemTrayIcon(QIcon(":/injection.ico"), this);
	trayIcon->setToolTip(PROGRAM_NAME); 
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconOnClick(QSystemTrayIcon::ActivationReason)));
	trayIcon->setContextMenu(trayMenu);
	trayIcon->show();

	ui.comboBoxPID->setValidator(new QIntValidator(0,99999, this));
	SetInjectionStatus();

	injectionWorker = new InjectionWorker();
	injectionWorker->moveToThread(&InjectionThread);
	connect(this, SIGNAL(injectDll(Process::ProcessEntity*,int)), injectionWorker, SLOT(injectionExecutor(Process::ProcessEntity*,int)));
	connect(this, SIGNAL(ejectDll(Process::ProcessEntity*)), injectionWorker, SLOT(ejectionExecutor(Process::ProcessEntity*)));
	connect(this, SIGNAL(cancelInjection()), injectionWorker, SLOT(injectionCancel()), Qt::DirectConnection);
	connect(injectionWorker, SIGNAL(injectionDone(int)), this, SLOT(injectionComplete(int)));
	connect(injectionWorker, SIGNAL(ejectionDone(int)), this, SLOT(ejectionComplete(int)));
	InjectionThread.start();
	manageArguments(QCoreApplication::arguments());

	cfgFilePath = QCoreApplication::applicationDirPath() + "/" + SETTINGS_FILE_NAME;
	loadSettings();
}

APInjector::~APInjector()
{
	delete injectionWorker;
}

void APInjector::quit()
{
	this->close();
}

void APInjector::closeEvent(QCloseEvent *e)
{
	saveSettings();
	e->accept();
}


void APInjector::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::WindowStateChange && ui.actionTray_on_minimize->isChecked() && isMinimized())
	{
		this->hide();
	}
}

void APInjector::trayIconOnClick(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::ActivationReason::Trigger)
	{
		this->showNormal();
		this->activateWindow();
	}
}

void APInjector::manageArguments(QStringList args)
{
	if (args.count() > 1)
	{
		if (args.count() > 2)
		{
			ui.actionClose_after_injection->setChecked(true);
			TargetProcess = new Process::ProcessEntity(0, args.at(1).toStdWString(), NULL, args.at(2).toStdWString());
			int delayTime = args.count() > 3 ? args.at(3).toInt() : 1;
			injectDll(TargetProcess, delayTime);
		}
		else
		{
			QMessageBox::warning(NULL, "AP Injector", "You have to give me three parameters!\npPocess name, dll path and delay time(sec)!");
		}
	}	
}

void APInjector::on_butAddNewProcesses_clicked()
{
	bool isAccepted = false;
	if (ui.rbProcess->isChecked())
	{
		ProcessSelector ps(&ChosenProcess, this);
		ps.setModal(true);
		isAccepted = (ps.exec() == QDialog::DialogCode::Accepted);
		if(isAccepted)
		{
			SetAddPidComboBoxItem(ChosenProcess.getPID());
			SetAddPNameComboBoxItem(QString::fromStdWString(ChosenProcess.getProcessName()), &ProcessNameList);
		}
	} 
	else
	{
		ApplicationSelector applicationSelector(&ChosenApplication, this);
		applicationSelector.setModal(true);
		isAccepted = (applicationSelector.exec() == QDialog::DialogCode::Accepted);
		if(isAccepted)
		{
			ui.comboBoxPID->setEditText("");
			SetAddPNameComboBoxItem(QString::fromStdWString(ChosenApplication.getApplicaitonName()), &ApplicationNameList);
		}
	}
}

void APInjector::SetAddPidComboBoxItem(int pid)
{
	QString sPid = tr("%1").arg(pid);
	if(ui.comboBoxPID->findText(sPid) < 0) 
	{
		ui.comboBoxPID->addItem(sPid);
	}
	ui.comboBoxPID->setEditText(sPid);
}

void APInjector::SetAddPNameComboBoxItem(QString processName, QStringList* itemList)
{
	if(ui.comboBoxPName->findText(processName) < 0) 
	{
		ui.comboBoxPName->addItem(processName);
		if (itemList->indexOf(processName) < 0)
		{
			itemList->push_back(processName);
		}
	}
	ui.comboBoxPName->setEditText(processName);	
}

void APInjector::SetPNameComboBoxItems(QStringList items)
{
	ui.comboBoxPName->clear();	
	ui.comboBoxPName->addItems(items);
}

void APInjector::SetAddDllPathComboBoxItem(QString dllFilePath)
{
	dllFilePath = dllFilePath.replace("/","\\");
	if(ui.comboBoxDllPath->findText(dllFilePath) < 0) 
	{
		ui.comboBoxDllPath->addItem(dllFilePath);
	}
	ui.comboBoxDllPath->setCurrentText(dllFilePath);
}


void APInjector::on_butDllSearchPath_clicked()
{
	QString dllFileName = QFileDialog::getOpenFileName(this, tr("Get DLL path"), QFileInfo(ui.comboBoxDllPath->currentText()).absoluteFilePath(), tr("DLL (*.dll)"));
	if (!dllFileName.isEmpty())
	{
		SetAddDllPathComboBoxItem(dllFileName);
	}
}

void APInjector::on_butInjection_clicked()
{	
	if (!ui.butInjection->text().contains("Cancel") && CheckForFieldsValidation())
	{
		ui.butInjection->setText("Cancel");
		int pid = ui.comboBoxPID->currentText().toInt();
		long DelayTime = 1;
		SetInjectionStatus("Injecting, please wait...", Qt::GlobalColor::blue);
		trayIcon->showMessage("", "Injecting...");
		SetEnabledForInjection(false);
		if (ui.rbApplication->isChecked())
		{
			pid = INJECTION_LOOK_FOR_WINDOW_PID;
		}
		TargetProcess = new Process::ProcessEntity(pid, ui.comboBoxPName->currentText().toStdWString(), NULL, ui.comboBoxDllPath->currentText().toStdWString());
		if (ui.rbInjectionMethodAuto->isChecked())
		{
			DelayTime = ui.dsbInjectionDelay->value();
		}
		injectDll(TargetProcess, DelayTime);
	}
	else if(ui.butInjection->text().contains("Cancel"))
	{
		cancelInjection();
		ui.butInjection->setText("Start Injection");
		SetInjectionStatus("Canceled", Qt::GlobalColor::darkYellow);
		SetEnabledForInjection(true);
	}
}

void APInjector::on_butEject_clicked()
{	
	if (CheckForFieldsValidation())
	{
		int pid = ui.comboBoxPID->currentText().toInt();
		SetInjectionStatus("Ejecting, please wait...", Qt::GlobalColor::blue);
		trayIcon->showMessage("", "Ejecting...");
		SetEnabledForInjection(false);
		if (ui.rbApplication->isChecked())
		{
			pid = INJECTION_LOOK_FOR_WINDOW_PID;
		}
		TargetProcess = new Process::ProcessEntity(pid, ui.comboBoxPName->currentText().toStdWString(), NULL, ui.comboBoxDllPath->currentText().toStdWString());
		ejectDll(TargetProcess);
	}
}

void APInjector::on_rbProcess_clicked()
{
	if (ui.rbProcess->isChecked())
	{
		switchSearchMode(false);
	}	
}

void APInjector::on_rbApplication_clicked()
{
	if (ui.rbApplication->isChecked())
	{
		switchSearchMode();
	}
}

void APInjector::switchSearchMode(bool isApplication)
{
	if (isApplication)
	{
		ui.comboBoxPID->setEnabled(false);
		ui.butAddNewProcesses->setText("A");
		SetPNameComboBoxItems(ApplicationNameList);
	}
	else
	{
		ui.comboBoxPID->setEnabled(true);
		ui.butAddNewProcesses->setText("P");
		SetPNameComboBoxItems(ProcessNameList);
	}
}

void APInjector::injectionComplete(int injectionResult)
{
	bool isSuccsess = InjectionStateNotify((Injection::InjectionResult)injectionResult);
	ui.butInjection->setText("Start Injection");
	SetEnabledForInjection(true);
	delete TargetProcess;
	if (ui.actionClose_after_injection->isChecked() && isSuccsess)
	{
		close();
	}
}

void APInjector::ejectionComplete(int injectionResult)
{
	// bool isSuccsess = InjectionStateNotify((Injection::InjectionResult)injectionResult);
	SetEnabledForEjection(true);
	delete TargetProcess;
}

void APInjector::SetEnabledForInjection(bool isEnabled)
{
	ui.comboBoxPID->setEnabled(ui.rbProcess->isChecked() && isEnabled);
	ui.comboBoxPName->setEnabled(isEnabled);
	ui.comboBoxDllPath->setEnabled(isEnabled);
	ui.dsbInjectionDelay->setEnabled(isEnabled);
	ui.butAddNewProcesses->setEnabled(isEnabled);
	ui.butDllSearchPath->setEnabled(isEnabled);
	//ui.butInjection->setEnabled(isEnabled);
	ui.rbInjectionMethodAuto->setEnabled(isEnabled);
	ui.rbInjectionMethodInstant->setEnabled(isEnabled);
	ui.rbApplication->setEnabled(isEnabled);
	ui.rbProcess->setEnabled(isEnabled);
}

void APInjector::SetEnabledForEjection(bool isEnabled)
{
	SetEnabledForInjection(isEnabled);
	ui.butInjection->setEnabled(isEnabled);
}

bool APInjector::CheckForFieldsValidation()
{
	bool result = true;
	bool okPid;
	QString pid = ui.comboBoxPID->currentText();
	QString pName = ui.comboBoxPName->currentText();
	QString dllPath = ui.comboBoxDllPath->currentText();
	int convertedPid = pid.toInt(&okPid);
	if(!pid.isEmpty() && !okPid)
	{
		result = false;
		QMessageBox::warning(this, "Invalid Process ID!", "Process ID must be a number!");
	}
	if (pid.isEmpty() && pName.isEmpty())
	{
		result = false;
		QMessageBox::warning(this, "No process informations!", "You have to fill out the Process ID or Process Name!");
	}
	if (dllPath.isEmpty())
	{
		result = false;
		QMessageBox::warning(this, "Which DLL should be injected!?", "You should set the DLL path!");
	}
	if (!QFileInfo(dllPath).exists())
	{
		result = false;
		QMessageBox::warning(this, "Which DLL should be injected!?", "Could not found the DLL on the given path!");
	}
	return result;
}

void APInjector::SetInjectionStatus(QString statusText, QColor color)
{
	ui.labInjectionStatus->setText(statusText);	
	QPalette* palette = new QPalette();
	palette->setColor(QPalette::WindowText, color);
	ui.labInjectionStatus->setPalette(*palette);
}

bool APInjector::InjectionStateNotify(Injection::InjectionResult injectionResult)
{
	bool isSuccsess = false;
	QString text = "";
	QColor color = Qt::GlobalColor::darkCyan;
	switch (injectionResult)
	{
	case Injection::InjectionResult::SUCCESS:
		text = "Injection complete!";
		color = Qt::GlobalColor::darkGreen;
		isSuccsess = true;
		break;
	case Injection::InjectionResult::BAD_DLL_PATH:
		text = "Could not found the DLL file on the path.";
		color = Qt::GlobalColor::darkYellow;
		break;
	case Injection::InjectionResult::MISSING_PRIVILEGES:
		text = "Missing rights!";
		color = Qt::GlobalColor::darkRed;
		QMessageBox::warning(this, "Missing rights!", "Try to rerun this program as administrator!");
		break;
	case Injection::InjectionResult::BAD_PROCESS_NAME:
		text = "Could not found the chosen process!";
		color = Qt::GlobalColor::darkRed;
		break;
	case Injection::InjectionResult::REMOTE_THREAD_TIMEOUT:
		text = "Injection Failed. Is the process still running?";
		color = Qt::GlobalColor::darkRed;
		break;
	case Injection::InjectionResult::MISSING_PROCESS:
		text = "Could not open the process. Is it still running?";
		color = Qt::GlobalColor::darkRed;
		break;
	};
	SetInjectionStatus(text, color);
	trayIcon->showMessage("", text);
	return isSuccsess;
}

void APInjector::saveSettings()
{		
	QList<int> pids;
	QList<QString> dllPaths;
	int chosenProcessId = ui.comboBoxPID->currentText().toInt();
	QString chosenProcessName = ui.comboBoxPName->currentText();
	QString chosenDllPath = ui.comboBoxDllPath->currentText();
	bool autoInject = ui.rbInjectionMethodAuto->isChecked();
	bool isApplication = ui.rbApplication->isChecked();
	bool minimiteToTray = ui.actionTray_on_minimize->isChecked();
	bool closeAfterInjection = ui.actionClose_after_injection->isChecked();
	/*
	for (int i = 0; i < ui.comboBoxPID->count();i++)
	{
		pids.push_back(ui.comboBoxPID->itemText(i).toInt());
	}*/
	/*for (int i = 0; i < ProcessNameList.size();i++)
	{
		if (!ui.comboBoxPName->itemText(i).isEmpty())
		{
			ProcessNameList.push_back(ui.comboBoxPName->itemText(i));
		}		
	}*/
	for (int i = 0; i < ui.comboBoxDllPath->count();i++)
	{
		dllPaths.push_back(ui.comboBoxDllPath->itemText(i));
	}

	Settings::SettingsEntity* settingsEntity = new Settings::SettingsEntity(/*chosenProcessId, pids,*/ chosenProcessName, ProcessNameList, ApplicationNameList, 
																			chosenDllPath, dllPaths, autoInject, isApplication, minimiteToTray, closeAfterInjection);
	QString settingsJson = settingsEntity->toJson();
	
	QFile cfgJson(cfgFilePath);
	if (!cfgJson.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&cfgJson);
	out << settingsJson;
	delete settingsEntity;
}

void APInjector::loadSettings()
{
	QString cfgJson;
	QFile cfgFile(cfgFilePath);
	if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	QTextStream in(&cfgFile);
	while (!in.atEnd()) {
		cfgJson += in.readLine();
	}
	Settings::SettingsEntity* settingsEntity = new Settings::SettingsEntity();
	settingsEntity->fromJson(cfgJson);
	/*
	for (int i = 0; i < settingsEntity->ProcessIds.size(); i++)
	{
		ui.comboBoxPID->addItem(QString("%1").arg(settingsEntity->ProcessIds[i]));
	}*/
	ProcessNameList = settingsEntity->ProcessNames;
	ApplicationNameList = settingsEntity->ApplicationNames;

	for (int i = 0; i < settingsEntity->DllPaths.size(); i++)
	{
		ui.comboBoxDllPath->addItem(settingsEntity->DllPaths[i]);
	}
	/*
	if (settingsEntity->ChosenPid > 0)
	{
		ui.comboBoxPID->setEditText(QString("%1").arg(settingsEntity->ChosenPid));
	}*/
	ui.comboBoxPName->setEditText(settingsEntity->ChosenProcessName);
	ui.comboBoxDllPath->setEditText(settingsEntity->ChosenDllPath);
	if (settingsEntity->AutoInject)
	{
		ui.rbInjectionMethodAuto->setChecked(true);
	}
	else
	{
		ui.rbInjectionMethodInstant->setChecked(true);
	}
	if (settingsEntity->IsApplication)
	{
		ui.rbApplication->setChecked(true);
		switchSearchMode(true);
	}
	else
	{
		ui.rbProcess->setChecked(true);
		switchSearchMode(false);
	}
	ui.actionTray_on_minimize->setChecked(settingsEntity->MinimiteToTray);
	ui.actionClose_after_injection->setChecked(settingsEntity->CloseAfterInjection);

	delete settingsEntity;
}