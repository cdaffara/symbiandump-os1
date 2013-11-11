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
// Provides the implementation of the subsession that refers to virtual serial ports
// 
//

#include "BTManServer.h"
#include "btmanserverutil.h"
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_MANAGER_SERVER);
#endif

CBTCommPortSettingsSubSession* CBTCommPortSettingsSubSession::NewL(CBTManSession& aSession, CBTRegistry& aRegistry)
	{
	LOG_STATIC_FUNC
	CBTCommPortSettingsSubSession* self = new(ELeave) CBTCommPortSettingsSubSession(aSession, aRegistry);
	//Since its a CObject derived class so we should use CleanupClosePushL
	CleanupClosePushL(*self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;	
	}

CBTCommPortSettingsSubSession::~CBTCommPortSettingsSubSession()
	{
	LOG_FUNC
	}

void CBTCommPortSettingsSubSession::ConstructL()
	{
	LOG_FUNC
	}
	
CBTCommPortSettingsSubSession::CBTCommPortSettingsSubSession(CBTManSession& aSession, CBTRegistry& aRegistry)
: CBTManSubSession(aSession,aRegistry)
	{
	LOG_FUNC
	}

void CBTCommPortSettingsSubSession::GetL(const TBTCommPortSettings& aSettings, const RMessage2& aMessage)
/**
	Find the virtual serial port specified by aSettings, and write the settings to client

	@param	aSettings	the key to find the virtual serial port
	@param	aMessage	the client message to complete
**/
	{
	LOG_FUNC
	// ask registry for row from the localdevice table
	const TBTCommPortSettings* portSettings = iRegistry.GetCommPortSettingsLC(aSettings);

	TPckg<TBTCommPortSettings> pckg(*portSettings);
	
	aMessage.WriteL(0, pckg); // write client descriptor
	iSession.CompleteMessage(aMessage, KErrNone);

	CleanupStack::PopAndDestroy(1); //portSettings;
	}

void CBTCommPortSettingsSubSession::UpdateL(const TBTCommPortSettings& aPortSettings, const RMessage2& aMessage)
/**
	Find the virtual serial port specified by aSettings, and update

	@param	aPortSettings	the key to find the virtual serial port
	@param	aMessage	the client message to complete
**/
	{
	LOG_FUNC
	RDbView* view = NULL;

	TRAPD(err, view = iRegistry.OpenCommPortL(aPortSettings));
	CleanupCloseDeletePushL(view); // we take ownsership

	switch(err)
		{
		case KErrNone:
			{
			iRegistry.UpdateCommPortSettingsL(*view, aPortSettings);
			break;
			}
		case KErrNotFound:
			{
			
			//In EKA2 we can verify that the client uid is correct
			iRegistry.AddCommPortSettingsL(aPortSettings, aMessage.SecureId());
			
			break;
			}
		default:
			User::Leave(err);
		}

	// Got here OK, so complete message and tidy up
	iSession.CompleteMessage(aMessage, KErrNone);
	CleanupStack::PopAndDestroy(view);
	NotifyChange(KRegistryChangeCSYTable);
	}

void CBTCommPortSettingsSubSession::DeleteL(const TBTCommPortSettings& aPortSettings, const RMessage2& aMessage)
/**
	Find the virtual serial port specified by aSettings, and delete

	@param	aPortSettings	the key to find the virtual serial port
	@param	aMessage	the client message to complete
**/
	{
	LOG_FUNC
	RDbView* view = NULL;

	TRAPD(err, view = iRegistry.OpenCommPortL(aPortSettings));
	CleanupCloseDeletePushL(view);
	switch(err)
		{
		case KErrNone:
			{
			iRegistry.DeleteCommPortSettingsL(*view);
			break;
			}
		default:
			User::Leave(err);
		}
	// Got here OK, so complete message and tidy up
	iSession.CompleteMessage(aMessage, KErrNone);
	CleanupStack::PopAndDestroy(view);
	NotifyChange(KRegistryChangeCSYTable);
	}

void CBTCommPortSettingsSubSession::Cleanup(TInt /*aError*/)
	{
	LOG_FUNC
	// do nothing
	}
