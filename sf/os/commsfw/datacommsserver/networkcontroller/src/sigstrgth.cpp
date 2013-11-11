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
 @file SIGSTRGTH.CPP
*/

#include "sigstrgth.h"

CAgentSignalStrengthWatcher* CAgentSignalStrengthWatcher::NewL(RMobilePhone& aPhone)
/**
Allocates memory to class CAgentSignalStrengthWatcher

@param aPhone,encapsulates access to a mobile phone.
@return a pointer to class CAgentSignalStrengthWatcher.
@exception if memory allocation fails.
*/
	{
	CAgentSignalStrengthWatcher* self = new(ELeave)CAgentSignalStrengthWatcher(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAgentSignalStrengthWatcher::~CAgentSignalStrengthWatcher()
/**
Destructor
*/
	{
	Cancel();
	}

TInt CAgentSignalStrengthWatcher::CurrentSignalStrength(TInt32& aSigStrength)
/**
Retrieve signal strength  of the current Agent.

@param aSigStrength, denotes the signal strength.
@return KErrUnknown if the signal strength has not yet been retrieved else KErrNone
*/
	{
	if(iState == EInitialising)
		{
		// The signal strength has not yet been retrieved
		return KErrUnknown;
		}
	aSigStrength=iSigStrengthInDBm;
	return KErrNone;
	}

CAgentSignalStrengthWatcher::CAgentSignalStrengthWatcher(RMobilePhone& aPhone)
: CActive(EPriorityStandard), iPhone(aPhone), iState(EInitialising)
	{
	CActiveScheduler::Add(this);
	}

void CAgentSignalStrengthWatcher::ConstructL()
	{
	// Start watching for signal strength
	iPhone.GetSignalStrength(iStatus,iSigStrengthInDBm,iSigStrengthInBars);
	SetActive();
	}

void CAgentSignalStrengthWatcher::RunL()
	{
	if(iStatus == KErrNone)
		{
		if(iSigStrengthInDBm != 0)
			{
			// The sig strength will be zero if not supported by the TSY
			if(iState == EInitialising)
				{
				iState = EUpdating;
				}
			// Renew request
			iPhone.NotifySignalStrengthChange(iStatus,iSigStrengthInDBm,iSigStrengthInBars);
			SetActive();
			}
		}
	}

void CAgentSignalStrengthWatcher::DoCancel()
	{
	if(iState == EInitialising)
		{
		iPhone.CancelAsyncRequest(EMobilePhoneGetSignalStrength);
		}
	else
		{
		// Must be EUpdating
		iPhone.CancelAsyncRequest(EMobilePhoneNotifySignalStrengthChange);
		}
	}

