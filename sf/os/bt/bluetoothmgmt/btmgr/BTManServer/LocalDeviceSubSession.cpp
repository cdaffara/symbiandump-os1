// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Provides the implementation of the subsession that refers to the local device
// 
//

#include "BTManServer.h"
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_MANAGER_SERVER);
#endif

// declare statics

CBTLocalDeviceSubSession* CBTLocalDeviceSubSession::NewL(CBTManSession& aSession, CBTRegistry& aRegistry)
	{
	LOG_STATIC_FUNC
	CBTLocalDeviceSubSession* self = new(ELeave) CBTLocalDeviceSubSession(aSession, aRegistry);
	//Since its a CObject derived class so we should use CleanupClosePushL
	CleanupClosePushL(*self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;	
	}

CBTLocalDeviceSubSession::~CBTLocalDeviceSubSession()
	{
	LOG_FUNC
	}

void CBTLocalDeviceSubSession::ConstructL()
	{
	LOG_FUNC
	}
	
CBTLocalDeviceSubSession::CBTLocalDeviceSubSession(CBTManSession& aSession, CBTRegistry& aRegistry)
: CBTManSubSession(aSession,aRegistry)
	{
	LOG_FUNC
	}

void CBTLocalDeviceSubSession::GetL(const RMessage2& aMessage)
/**
	Retrieve the settings of the local device and return to client

	@param	aMessage The client message to complete
**/
	{
	LOG_FUNC
	// ask registry for row from the localdevice table
	TBTLocalDevice* locDevice = iRegistry.GetLocalDeviceLC();
	TPckg<TBTLocalDevice> pckg(*locDevice);
	
	aMessage.WriteL(0, pckg); // write client descriptor
	iSession.CompleteMessage(aMessage, KErrNone);

	CleanupStack::PopAndDestroy(locDevice);
	}

void CBTLocalDeviceSubSession::UpdateL(const TBTLocalDevice& aLocalDevice, const RMessage2& aMessage)
/**
	Update the local device settings in the registry

	@param	aLocalDevice the new settings
	@param	aMessage	the client message to complete
**/
	{
	LOG_FUNC
	iRegistry.UpdateLocalDeviceL(aLocalDevice);
	iSession.CompleteMessage(aMessage, KErrNone);
	//P&S
	NotifyChange(KRegistryChangeLocalTable);
	}

void CBTLocalDeviceSubSession::Cleanup(TInt /*aError*/)
	{
	LOG_FUNC
	// do nothing
	}
