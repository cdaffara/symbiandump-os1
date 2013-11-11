// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/controllerinitialisationplugin.h>
#include <bluetooth/hci/hciutil.h>
#include <bluetooth/hci/initialisorbase.hrh>

EXPORT_C /*static*/ CControllerInitialisationPlugin* CControllerInitialisationPlugin::NewL(TUid aImplementationUid)
	{
	CControllerInitialisationPlugin* self = reinterpret_cast<CControllerInitialisationPlugin*> (
		CHciUtil::LoadImplementationL(TUid::Uid(KControllerInitialisationEcomInterfaceUid), 
							aImplementationUid, 
							_FOFF(CControllerInitialisationPlugin, iInstanceId)));	
	return self;
	}

EXPORT_C /*static*/ CControllerInitialisationPlugin* CControllerInitialisationPlugin::NewL()
	{
	CControllerInitialisationPlugin* self = reinterpret_cast<CControllerInitialisationPlugin*> (
		CHciUtil::LoadImplementationL(TUid::Uid(KControllerInitialisationEcomInterfaceUid), 
							_FOFF(CControllerInitialisationPlugin, iInstanceId)));	
	return self;
	}

EXPORT_C CControllerInitialisationPlugin::~CControllerInitialisationPlugin()
	{
	CHciUtil::DestroyedImplementation(iInstanceId);
	}

// Protected Constructor.
EXPORT_C CControllerInitialisationPlugin::CControllerInitialisationPlugin()
	{
	}

