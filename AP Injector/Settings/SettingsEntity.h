#ifndef SETTINGS_ENTITY_H
#define SETTINGS_ENTITY_H

#include <QString>
#include <QStringList>
#include "JsonCpp/json.h"

#define COPYRIGHT "Copyright (c) 2013-2014 applusplus"

namespace Settings
{
	class SettingsEntity
	{
	public:
		SettingsEntity();
		SettingsEntity(/*int chosenPid, QList<int> processIds,*/ QString chosenProcessName, QStringList processNames, QStringList applicationNames,
			QString chosenDllPath, QStringList dllPaths, bool autoInject, bool isApplication, bool minimiteToTray,	bool closeAfterInjection);
		~SettingsEntity();

		int ChosenPid;
		QList<int> ProcessIds;
		QString ChosenProcessName;
		QStringList ProcessNames;
		QStringList ApplicationNames;
		QString ChosenDllPath;
		QStringList DllPaths;
		bool AutoInject;
		bool IsApplication;
		bool MinimiteToTray;
		bool CloseAfterInjection;

		QString toJson();
		void fromJson(QString json);
	};
};

#endif