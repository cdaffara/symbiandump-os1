// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file CReconnectRequest.cpp
*/

#include "CReconnectRequest.h"
#include <comms-infras/nifagt.h>
#include "CNetworkController.h"
#include "NetConPanic.h"
#include "NetConLog.h"

CReconnectRequest* CReconnectRequest::NewL(MNetConEnv* aController, MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent, CStoreableOverrideSettings* aOverrides)
/**
static newL function allocates memory to an object of class CReconnectRequest

@param aController, Used to inform the CNetworkController of state changes such as a request or a bearer availability check completing.
@param aObserver, request the selection
@param aAgent, the agent who's connection failed
@param aOverrides, store and retrieve override sets to and from both streams and buffers.
@return a  pointer to class CReconnectRequest
@exception leaves if memory allocation fails.
*/
	{
	CReconnectRequest* self = new(ELeave) CReconnectRequest(aController, aObserver, aAgent, aOverrides);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CReconnectRequest::CReconnectRequest(MNetConEnv* aController, MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent, CStoreableOverrideSettings* aOverrides)
: CNetConRequestBase(aController, aObserver, aOverrides), iAgent(aAgent)
/**
Constructor
*/
	{ }

void CReconnectRequest::ConstructL()
	{
	CNetConRequestBase::ConstructL();
	}

CReconnectRequest::~CReconnectRequest()
/**
Destuctor
*/
	{ }

void CReconnectRequest::StartRequest()
/**
start processing this request
*/
	{
	LOG( NetConLog::Printf(_L("\tRequest type = Reconnect")); )

	// set overrides for this request in the databse
	TRAPD(err, iDatabase->SetOverridesL(iOverrides);
               iDatabase->RequestNotificationOfServiceChangeL(ipServiceChangeObserver));

	if(err!=KErrNone)
		{
		RequestComplete(err);
		return;
		}

	TRAP(err, StartReconnectL());
	if(err!=KErrNone)
		{
		RequestComplete(err);
		}
	}

void CReconnectRequest::RequestComplete(TInt aError)
/**
Force premature completion of a request with a given error
*/
	{

	LOG( NetConLog::Printf(_L("\tRequest Complete with error %d"), aError); )

	iController->RequestComplete(this, aError);
	}

void CReconnectRequest::StartReconnectL()
/**
Read current connection preferences and settings from the database then start the reconnection process. 

@exception leaves if database access leaves
*/
	{

	iController->CheckBearerAvailability(ETrue);
	}

void CReconnectRequest::SetAvailableBearers(TUint32 aBearerSet)
/**
Making bearer availability with a set of bearers.

@param aBearerSet the set of available bearers
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CReconnectRequest::SetAvailableBearers(aBearerSet)")); )

	// cancel bearer availability check, since we're not
	// going to do any second phase (i.e. signal strength)
	// bearer availability checking
	iController->CancelBearerAvailabilityCheck();

	// retrieve connection settings from the agent
	TRAPD(err, iSettings = iAgent->ConnectionSettingsL());

	LOG (
		NetConLog::Printf(_L("\tSelected Service Type = '%S'"), &iSettings.iServiceType);
		NetConLog::Printf(_L("\tAvailable Bearer Set = %d"), aBearerSet);
		)

	if(err==KErrNone && SelectedServiceAvailable(aBearerSet))
		{
		// if service is available then display the reconnect dialog box
		Reconnect();
		}
	else
		{
		// otherwise complete the request with KErrCancel
		// Note that Nifman will propagate the original error
		// code (e.g. KErrCommsLineFail) to the application
		RequestComplete(KErrCancel);
		}
	}


void CReconnectRequest::MDPOReconnectComplete(TInt aError)
/**
Implementation of the MDialogProcessorObserver interface:
Callback from the dialog processor when the reconnect dialog box has completed

@param aError KErrCancel if the user selected "Cancel"
@param aReponse ETrue if the user selected "OK" otherwise EFalse
*/
	{

	RequestComplete(aError);
	}

