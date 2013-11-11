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

#ifndef __WSPLUGINMANAGER_H__
#define __WSPLUGINMANAGER_H__

#include <e32base.h>
#include "graphics/WSPLUGIN.H"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>

/**
The window server has a single plugin manager object, which maintains the set
of loaded plugins.
*/
class CWsPluginManager : public CBase, public MWsPluginManager
	{
private:
	class CPluginInfo : public CBase
		{
	public:
		CPluginInfo();
		~CPluginInfo();
		CWsPlugin * iPlugin;
		};
public:
	static CWsPluginManager* NewL();
	~CWsPluginManager();
	
	void InitializePluginsL(MWsGraphicDrawerEnvironment& aEnvironment);	
	
	template <class T> T * FindImplementation(TInt & aStart);
	template <class T> T * FindNamedImplementation(const TDesC& aName);
	
public: // from MWsObjectProvider:
	TAny * ResolveObjectInterface(TUint aId);
	
public: // from MWsPluginManager
	TAny * ResolvePluginInterface(TUint aId);

private:
	CWsPluginManager();	
	void ConstructL();
	CWsPlugin * LoadPluginL(const TDesC& aSection);
	
private:
	RPointerArray<CPluginInfo> iPlugins;
	REComSession& iSession;
	};

/**
This searches for any plugin which implements a specified interface.
Calling this function modifies the value of aStart, and passing it back in
allows the next implementation to be found.
*/
template <class T> T * CWsPluginManager::FindImplementation(TInt & aStart)
	{
	while (aStart < iPlugins.Count())
		{
		T * impl = iPlugins[aStart]->iPlugin->ObjectInterface<T>();
		++aStart;
		if (impl)
			{
			return impl;
			}
		}
	return NULL;
	}

/**
This searches for any plugin which implements a specified interface and
has the specified name returned from PluginName.

Maybe plugin name should be a function on an MWsNamed class instead of CWsPlugin?
*/
template <class T> T * CWsPluginManager::FindNamedImplementation(const TDesC& aName)
	{
	TInt pos = 0;
	while (pos < iPlugins.Count())
		{
		T * impl = iPlugins[pos]->iPlugin->ObjectInterface<T>();
		if (impl && iPlugins[pos]->iPlugin->PluginName() == aName)
			{
			return impl;
			}
		++pos;
		}
	return NULL;
	}

#endif // __WSPLUGINMANAGER_H__
