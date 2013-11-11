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

#include <bluetooth/hci/corehciplugin.h>
#include <bluetooth/hci/hciutil.h>
#include <bluetooth/hci/hcibase.hrh>

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COREHCI_BASE);
#endif

/** 
@param aImplementationUid is used by ECom to load the implementation.
@return Ownership of a new core HCI plugin.
*/
EXPORT_C CCoreHCIPlugin* CCoreHCIPlugin::NewL(TUid aImplementationUid)
	{
	LOG_STATIC_FUNC

	return reinterpret_cast<CCoreHCIPlugin*> (CHciUtil::LoadImplementationL(TUid::Uid(KCoreHCIEcomInterfaceUid), 
																			aImplementationUid, 
																			_FOFF(CCoreHCIPlugin, iInstanceId)));
	}

/** 
This method will attempt to load a single instance of an implementation for
the correct interface.
@return Ownership of a new core HCI plugin.
*/
EXPORT_C CCoreHCIPlugin* CCoreHCIPlugin::NewL()
	{
	LOG_STATIC_FUNC

	return reinterpret_cast<CCoreHCIPlugin*> (CHciUtil::LoadImplementationL(TUid::Uid(KCoreHCIEcomInterfaceUid),
																			_FOFF(CCoreHCIPlugin, iInstanceId)));
	}

/**
Cleanup Ecom stuff
*/
EXPORT_C CCoreHCIPlugin::~CCoreHCIPlugin()
    {
	LOG_FUNC

	CHciUtil::DestroyedImplementation(iInstanceId);
	CLOSE_LOGGER
    }

EXPORT_C CCoreHCIPlugin::CCoreHCIPlugin()
    {
	CONNECT_LOGGER
	LOG_FUNC
    }
