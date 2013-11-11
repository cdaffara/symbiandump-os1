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

#include <bluetooth/hci/hcicmdqueuedecisionplugin.h>
#include <bluetooth/hci/hcicmdqueuedecisioninterface.h>
#include <bluetooth/logger.h>
#include <bluetooth/hci/hciutil.h>

#include <bluetooth/hci/qdpbase.hrh>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_QDP_BASE);
#endif

/** 
Static factory method for the HCI Command Queue Decision plugin

@param aImplementationUid The implementation UID.
@return Ownership of a new QDP plugin.
*/
EXPORT_C /*static*/ CHCICmdQueueDecisionPlugin* CHCICmdQueueDecisionPlugin::NewL(TUid aImplementationUid)
	{
	LOG_STATIC_FUNC

	CHCICmdQueueDecisionPlugin* self = reinterpret_cast<CHCICmdQueueDecisionPlugin*> (
		CHciUtil::LoadImplementationL(TUid::Uid(KHCICmdQueueDecisionEcomInterfaceUid), 
							aImplementationUid, 
							_FOFF(CHCICmdQueueDecisionPlugin, iInstanceId))
		);	
	return self;
	}

/** 
Static factory method for the HCI Command Queue Decision plugin

This method will attempt to load a single instance of an implementation for
the correct interface.

@internalComponent
@return Ownership of a new core HCI plugin.
*/
EXPORT_C CHCICmdQueueDecisionPlugin* CHCICmdQueueDecisionPlugin::NewL()
	{
	LOG_STATIC_FUNC

	CHCICmdQueueDecisionPlugin* self = reinterpret_cast<CHCICmdQueueDecisionPlugin*> (
		CHciUtil::LoadImplementationL(TUid::Uid(KHCICmdQueueDecisionEcomInterfaceUid), 
							_FOFF(CHCICmdQueueDecisionPlugin, iInstanceId))
		);
	return self;
	}

/** 
Destructor. 
All ECom interfaces obtained through the Interface will be deleted.
*/
EXPORT_C CHCICmdQueueDecisionPlugin::~CHCICmdQueueDecisionPlugin()
	{
	LOG_FUNC
 	
 	CHciUtil::DestroyedImplementation(iInstanceId);
 	CLOSE_LOGGER
	}
	
/** 
Private Constructor
*/
EXPORT_C CHCICmdQueueDecisionPlugin::CHCICmdQueueDecisionPlugin()
	{
	CONNECT_LOGGER
	LOG_FUNC	
	}
