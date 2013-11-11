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
//

#include <comms-infras/nifagt.h>
#include <comms-infras/nifif.h>
#include "CSelectionRequest.h"
#include "CNetworkController.h"
#include "NetConPanic.h"
#include "NetConLog.h"
#include "NetConError.h"

CSelectionRequest* CSelectionRequest::NewL(MNetConEnv* aController, MNetworkControllerObserver* aObserver, TConnStartType aConnStartType, const TConnPref& aPrefs, TInt aConnectionAttempt, TInt aLastConnectionError)
 	{
	CSelectionRequest* self = new(ELeave) CSelectionRequest(aController, aObserver, aConnStartType, aPrefs, aConnectionAttempt, aLastConnectionError);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CSelectionRequest::CSelectionRequest(MNetConEnv* aController, MNetworkControllerObserver* aObserver, TConnStartType aConnStartType, const TConnPref& aPrefs, TInt aConnectionAttempt, TInt aLastConnectionError)
: CNetConRequestBase(aController, aObserver, NULL), iConnStartType(aConnStartType), iConnPrefs(aPrefs), iConnectionAttempt(aConnectionAttempt), iLastConnectionError(aLastConnectionError)
	{ }

void CSelectionRequest::ConstructL()
	{

	TCallBack callback(AsyncCb, this);
	iAsyncCb = new(ELeave) CAsyncCallBack(callback, CActive::EPriorityStandard);
	}

CSelectionRequest::~CSelectionRequest()
	{

	if(iAsyncCb)
		delete iAsyncCb;
	}

TInt CSelectionRequest::AsyncCb(TAny* aThisPtr)
	{
	CSelectionRequest* self = (CSelectionRequest*)aThisPtr;
	self->AsyncCbComplete();
	return KErrNone;
	}

void CSelectionRequest::AsyncCbComplete()
	{

	RequestComplete(KErrNone);
	}

void CSelectionRequest::StartRequest()
	{

	// if iConnectionAttempt > 100 then it is used to indicate the test number and hence the behaviour!
	switch(iConnectionAttempt)
		{
		case 100:
		case 300:
		case 900:
		case 1000:
			RequestComplete(KErrNone);
			break;
		case 200:
		case 1100:
			RequestComplete(KErrBadPower);
			break;
		case 600:
			iAsyncCb->CallBack();
			break;
		default:
			User::Invariant();
		}
	}

void CSelectionRequest::RequestComplete(TInt aError)
	{

	iController->RequestComplete(this, aError);
	}

void CSelectionRequest::CancelRequest()
	{
	}

void CSelectionRequest::SetAvailableBearers(const TUint32)
	{

	User::Invariant();
	}

const TDesC& CSelectionRequest::AgentName() const
	{

	_LIT(KAgentName, "An Agent Name");
	return KAgentName();
	}

const TPckgBuf<TSoIfConnectionInfo>& CSelectionRequest::AgentConnectionInfo() const
	{

	return iAgentConnInfo;
	}

const TConnStartType& CSelectionRequest::ConnectionStartType() const
	{
	return iConnStartType;
	}

const TConnPref& CSelectionRequest::ConnPrefs() const
	{

	return iConnPrefs;
	}

