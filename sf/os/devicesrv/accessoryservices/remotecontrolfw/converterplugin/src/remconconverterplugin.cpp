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
// Implementation of Rem Con bearer plugin.
// 
//

/**
 @file
 @internalComponent
*/

#include <remcon/remconconverterplugin.h>
#include <ecom/ecom.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

EXPORT_C CRemConConverterPlugin::~CRemConConverterPlugin()
	{
	LOG_FUNC
	REComSession::DestroyedImplementation(iInstanceId);
	}

EXPORT_C CRemConConverterPlugin::CRemConConverterPlugin()
	{
	LOG_FUNC
	}

EXPORT_C CRemConConverterPlugin* CRemConConverterPlugin::NewL(TUid aImplementationId)
	{
	return reinterpret_cast<CRemConConverterPlugin*>(
		REComSession::CreateImplementationL(
			aImplementationId, 
			_FOFF(CRemConConverterPlugin, iInstanceId))
		);
	}
