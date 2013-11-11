// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of Rem Con target selector plugin.
// 
//

/**
 @file
 @internalComponent
*/

#include <remcon/remcontargetselectorplugin.h>
#include <ecom/ecom.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

EXPORT_C CRemConTargetSelectorPlugin::~CRemConTargetSelectorPlugin()
	{
	LOG_STATIC_FUNC
	REComSession::DestroyedImplementation(iInstanceId);
	}

EXPORT_C CRemConTargetSelectorPlugin::CRemConTargetSelectorPlugin(MRemConTargetSelectorPluginObserver& aObserver)
:	iObserver(aObserver)
	{
	LOG_FUNC
	}

EXPORT_C CRemConTargetSelectorPlugin* CRemConTargetSelectorPlugin::NewL(TUid aImplementationUid, 
																		MRemConTargetSelectorPluginObserver& aObserver)
	{
	LOG_STATIC_FUNC
	return reinterpret_cast<CRemConTargetSelectorPlugin*>(
		REComSession::CreateImplementationL(
			aImplementationUid, 
			_FOFF(CRemConTargetSelectorPlugin, iInstanceId),
			(TAny*)&aObserver)
		);
	}

EXPORT_C MRemConTargetSelectorPluginObserver& CRemConTargetSelectorPlugin::Observer()
	{
	return iObserver;
	}
