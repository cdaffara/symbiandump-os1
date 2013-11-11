// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "wspluginmanager.h"
#include "graphics/WSPLUGIN.H"
#include "inifile.h"

GLREF_D CDebugLogBase* wsDebugLog;

const TInt KPluginInterfaceId = 0x10285A29;

/**********************************************************************
Plugin Info
**********************************************************************/
CWsPluginManager::CPluginInfo::CPluginInfo()
	{
	}

CWsPluginManager::CPluginInfo::~CPluginInfo()
	{
	delete iPlugin;
	}

/**********************************************************************
Plugin Manager
**********************************************************************/
CWsPluginManager* CWsPluginManager::NewL()
	{
	CWsPluginManager* self = new (ELeave) CWsPluginManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CWsPluginManager::CWsPluginManager() :
    iSession(REComSession::OpenL())
	{
	}
	
CWsPluginManager::~CWsPluginManager()
	{
	iPlugins.ResetAndDestroy();
	iSession.Close();
	REComSession::FinalClose();
	}

void CWsPluginManager::ConstructL()
	{
	_LIT(KPlugins,"PLUGINS");
	
	TPtrC pluginString;
	TBool havePlugins=WsIniFile->FindVar(KPlugins,pluginString);
	const TDesC * plugins;
	_LIT(KDefaultPlugins, "FLICKERBUFFER DISPLAY");	
	if (havePlugins)
		plugins = &pluginString;
	else
		plugins = &KDefaultPlugins;
	TLex lex(*plugins);
	while(true)
		{
		TPtrC ptr = lex.NextToken();
		if (ptr.Length() > 0)
			{
			CWsPlugin * plugin = 0;
			
			TRAPD(err, plugin = LoadPluginL(ptr));

			if (wsDebugLog)
				{
				TBuf<80> buf;
				if (err == KErrNone)
					{
					_LIT(KLoadedPlugin,"Loaded plugin: ");
					_LIT(KPluginName, " calling itself: ");
					buf.Append(KLoadedPlugin);
					buf.Append(ptr);
					buf.Append(KPluginName);
					buf.Append(plugin->PluginName());
					wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,buf);
					}
				else
					{
					_LIT(KMissingPlugin,"Failed to load plugin (%d): ");
					buf.Append(KMissingPlugin);
					buf.Append(ptr);
					wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,buf,err);
					}
				}

			if (err<KErrNone)
				{	//Additionally log to RDebug if error
				_LIT(KMissingPlugin,"WServ: Failed to load plugin %S (error %d)");
				RDebug::Print(KMissingPlugin,&ptr,err);
				}
			}
		else
			{
			break;
			}
		}
	}

CWsPlugin * CWsPluginManager::LoadPluginL(const TDesC& aSection)
	{
	_LIT(KId, "ID");
	_LIT(KData, "DATA");
	_LIT(KType, "TYPE");
	TInt id;
	TPtrC data;
	TPtrC type;
	TAny * dataPtr;
	CWsPlugin * plugin = 0;
	TBool hasId=WsIniFile->FindVar(aSection,KId,id);
	TBool hasType=WsIniFile->FindVar(aSection,KType,type);
	if ((!hasId) && (!hasType))
		{
		_LIT(KCscStage, "TCSC");
		_LIT(KFlickerBufferStage, "FLICKERBUFFER");
		_LIT(KDisplayStage, "DISPLAY");
		if (!aSection.CompareF(KFlickerBufferStage))
			{
			hasId = ETrue;
			id = 0x1028637B;
			}
		else if (!aSection.CompareF(KDisplayStage))
			{
			hasId = ETrue;
			id = 0x1028637A;
			}
		else if (!aSection.CompareF(KCscStage))
			{
			hasId = ETrue;
			id = 0x10286507;
			}	
		else
			{
			hasType = ETrue;
			type.Set(aSection);
			}
		}	
	TBool hasData = WsIniFile->FindVar(aSection,KData,data);
	
	if (hasData)
		dataPtr = &data;
	else
		dataPtr = NULL;

	CPluginInfo* info = new (ELeave) CPluginInfo;
	CleanupStack::PushL(info);

	if (hasId)
		{
		TUid uid = TUid::Uid(id);
		plugin = reinterpret_cast<CWsPlugin*>(iSession.CreateImplementationL(uid, CWsPlugin::DtorIDKeyOffset(), dataPtr));
		}
	else
		{
		TEComResolverParams params;
		RBuf8 buf8;
		buf8.CreateL(type.Length());
		CleanupClosePushL(buf8);
		buf8.Copy(type);
		params.SetDataType(buf8);
		plugin = reinterpret_cast<CWsPlugin*>(iSession.CreateImplementationL(TUid::Uid(KPluginInterfaceId),CWsPlugin::DtorIDKeyOffset(),dataPtr,params));
		CleanupStack::PopAndDestroy(&buf8);
		}
	
	User::LeaveIfNull(plugin);
	info->iPlugin = plugin;
	User::LeaveIfError(iPlugins.Append(info));
	CleanupStack::Pop(info);
	return plugin;
	}

void CWsPluginManager::InitializePluginsL(MWsGraphicDrawerEnvironment& aEnvironment)
	{
	TInt nbPlugins=iPlugins.Count();
	RBuf8 empty;
	for (TInt k=0;k<nbPlugins;k++)
		{
		iPlugins[k]->iPlugin->ConstructL(aEnvironment,empty);
		}
	}

TAny * CWsPluginManager::ResolveObjectInterface(TUint aId)
	{
	if (aId == MWsPluginManager::EWsObjectInterfaceId)
		return static_cast<MWsPluginManager*>(this);
	return NULL;
	}

TAny * CWsPluginManager::ResolvePluginInterface(TUint aId)
	{
	for (TInt p = 0; p < iPlugins.Count(); ++p)
		{
		TAny * interface = iPlugins[p]->iPlugin->ResolveObjectInterface(aId);
		if (interface)
			return interface;
		}
	return NULL;
	}

