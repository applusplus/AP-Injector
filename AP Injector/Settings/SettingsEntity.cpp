#include "SettingsEntity.h"

namespace Settings
{
	SettingsEntity::SettingsEntity()
	{
	}

	SettingsEntity::SettingsEntity(/*int chosenPid, QList<int> processIds,*/ QString chosenProcessName, QStringList processNames, QStringList applicationNames,
		QString chosenDllPath, QStringList dllPaths, bool autoInject, bool isApplication, bool minimiteToTray,	bool closeAfterInjection)
	{
		/*ChosenPid = chosenPid;
		ProcessIds = processIds;*/
		ChosenProcessName = chosenProcessName;
		ProcessNames = processNames;
		ApplicationNames = applicationNames;
		ChosenDllPath = chosenDllPath;
		DllPaths = dllPaths;
		AutoInject = autoInject;
		IsApplication = isApplication;
		MinimiteToTray = minimiteToTray;
		CloseAfterInjection = closeAfterInjection;
	}

	SettingsEntity::~SettingsEntity()
	{
		
	}

	QString SettingsEntity::toJson()
	{
		Json::StyledWriter writer;
		Json::Value root;
		//Json::Value pids_node;
		Json::Value processNames_node;
		Json::Value applicationNames_node;
		Json::Value dllPaths_node;
		//root["ChosenPid"] = ChosenPid;
		root["Copyright"] = COPYRIGHT;
		root["ChosenProcessName"] = ChosenProcessName.toStdString();
		root["ChosenDllPath"] = ChosenDllPath.toStdString();
		root["AutoInject"] = AutoInject;
		root["IsApplication"] = IsApplication;
		root["MinimiteToTray"] = MinimiteToTray;
		root["CloseAfterInjection"] = CloseAfterInjection;
		/*for(int i = 0; i < ProcessIds.size(); i++) {
			pids_node.append(ProcessIds[i]);
		}
		root["ProcessIds"] = pids_node;*/
		for(int i = 0; i < ProcessNames.size(); i++) {
			processNames_node.append(ProcessNames[i].toStdString());
		}
		root["ProcessNames"] = processNames_node;
		for(int i = 0; i < ApplicationNames.size(); i++) {
			applicationNames_node.append(ApplicationNames[i].toStdString());
		}
		root["ApplicationNames"] = applicationNames_node;
		for(int i = 0; i < DllPaths.size(); i++) {
			dllPaths_node.append(DllPaths[i].toStdString());
		}
		root["DllPaths"] = dllPaths_node;
		
		return QString::fromStdString(writer.write(root));
	}

	void SettingsEntity::fromJson(QString json)
	{
		Json::Reader reader;
		Json::Value root;
		bool parsingSuccessful = reader.parse( json.toStdString(), root );
		if ( !parsingSuccessful )
			return;

		//ChosenPid = root.get("ChosenPid", 0).asInt();
		ChosenProcessName = QString::fromStdString(root.get("ChosenProcessName", "").asString());
		ChosenDllPath = QString::fromStdString(root.get("ChosenDllPath", "").asString());
		AutoInject = root.get("AutoInject", true).asBool();
		IsApplication = root.get("IsApplication", false).asBool();
		MinimiteToTray = root.get("MinimiteToTray", false).asBool();
		CloseAfterInjection = root.get("CloseAfterInjection", false).asBool();

		//Json::Value pids_node = root["ProcessIds"];
		Json::Value processNames_node = root["ProcessNames"];
		Json::Value applicationNames_node = root["ApplicationNames"];
		Json::Value dllPaths_node = root["DllPaths"];
		/*for ( int i = 0; i < pids_node.size(); ++i )
			ProcessIds.push_back(pids_node[i].asInt());
		*/
		for ( int i = 0; i < processNames_node.size(); ++i )
			ProcessNames.push_back(QString::fromStdString(processNames_node[i].asString()));

		for ( int i = 0; i < applicationNames_node.size(); ++i )
			ApplicationNames.push_back(QString::fromStdString(applicationNames_node[i].asString()));

		for ( int i = 0; i < dllPaths_node.size(); ++i )
			DllPaths.push_back(QString::fromStdString(dllPaths_node[i].asString()));
	}
};
