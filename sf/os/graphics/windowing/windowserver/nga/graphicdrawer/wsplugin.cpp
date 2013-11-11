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
// The base class for window server plugins
// Interface ID : 0x10285A29
// DLL ID       : 0x10281920
// 
//

#include "graphics/WSPLUGIN.H"
#include <s32mem.h>

_LIT(KPluginName, "unnamed");

// CWsPlugin::CPimpl \\\\\\\\\\\\\\\\\\\\\\\\

/** @internalComponent
	@released
*/
NONSHARABLE_CLASS(CWsPlugin::CPimpl): public CBase
	{
public:
	CPimpl(CWsPlugin& aGraphic,MWsGraphicDrawerEnvironment& aEnv);
	CWsPlugin& iPlugin;
	MWsGraphicDrawerEnvironment& iEnv;
	};

CWsPlugin::CPimpl::CPimpl(CWsPlugin& aPlugin,MWsGraphicDrawerEnvironment& aEnv):
	iPlugin(aPlugin), iEnv(aEnv)
	{
	}

// CWsPlugin \\\\\\\\\\\\\\\\\\\\\\\\

/** Constructor
*/
EXPORT_C CWsPlugin::CWsPlugin()
	{
	}

/** Destructor
*/
EXPORT_C CWsPlugin::~CWsPlugin()
	{
	delete iPimpl;

	if(KNullUid != iDtor_ID_Key)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key);
		}
	}

EXPORT_C void CWsPlugin::BaseConstructL(MWsGraphicDrawerEnvironment& aEnv)
	{
	iPimpl = new(ELeave) CPimpl(*this,aEnv);
	}

EXPORT_C MWsGraphicDrawerEnvironment& CWsPlugin::Env()
	{
	return iPimpl->iEnv;
	}

EXPORT_C const MWsGraphicDrawerEnvironment& CWsPlugin::Env() const
	{
	return iPimpl->iEnv;
	}

EXPORT_C const TDesC & CWsPlugin::PluginName() const
	{
	return (KPluginName);
	}
