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

#include "CReconnectRequest.h"
#include <comms-infras/nifagt.h>
#include "CNetworkController.h"
#include "NetConPanic.h"
#include "NetConLog.h"

CReconnectRequest* CReconnectRequest::NewL(MNetConEnv* aController, MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent, CStoreableOverrideSettings* aOverrides)
	{
	CReconnectRequest* self = new(ELeave) CReconnectRequest(aController, aObserver, aAgent, aOverrides);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CReconnectRequest::CReconnectRequest(MNetConEnv* aController, MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent, CStoreableOverrideSettings* aOverrides)
: CNetConRequestBase(aController, aObserver, aOverrides), iAgent(aAgent)
	{
	}

void CReconnectRequest::ConstructL()
	{

	TCallBack callback(AsyncCb, this);
	iAsyncCb = new(ELeave)CAsyncCallBack(callback, CActive::EPriorityStandard);
	}

CReconnectRequest::~CReconnectRequest()
	{

	if(iAsyncCb)
		delete iAsyncCb;
	}

void CReconnectRequest::StartRequest()
	{

	TInt testNo = REINTERPRET_CAST(TInt, iAgent);
	
	switch (testNo)
		{
		case 300:
		case 900:
		case 1000:
			RequestComplete(KErrNone);
			break;
		case 400:
			RequestComplete(KErrDiskFull);
			break;
		case 800:
			iAsyncCb->CallBack();
			break;
		default:
			User::Invariant();
		}
	}

void CReconnectRequest::StartReconnectL()
	{
	}

void CReconnectRequest::RequestComplete(TInt aError)
	{

	iController->RequestComplete(this, aError);
	}

void CReconnectRequest::CancelRequest()
	{
	}

void CReconnectRequest::SetAvailableBearers(TUint32)
	{
	}

void CReconnectRequest::MDPOReconnectComplete(TInt aError)
	{

	RequestComplete(aError);
	}

TInt CReconnectRequest::AsyncCb(TAny* aThisPtr)
	{
	CReconnectRequest* self = (CReconnectRequest*)aThisPtr;
	self->RequestComplete(KErrNone);
	return KErrNone;
	}

