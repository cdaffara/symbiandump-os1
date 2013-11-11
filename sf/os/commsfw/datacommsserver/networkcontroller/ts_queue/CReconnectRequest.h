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

#if !defined (__CRECONNECTREQUEST_H__)
#define __CRECONNECTREQUEST_H__

#include "CNetConRequestBase.h"

class CNifAgentBase;
class CReconnectRequest : public CNetConRequestBase
	{
public:
	static CReconnectRequest* NewL(MNetConEnv* aController, MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent, CStoreableOverrideSettings* aOverrides);
	virtual ~CReconnectRequest();

	// start processing this request
	virtual void StartRequest();

	// force premature completion of a request with a given error
	virtual void RequestComplete(TInt aError);

	// cancel this request
	virtual void CancelRequest();

	// bearer availability
	virtual void SetAvailableBearers(TUint32 aBearerSet);

	// override part of the MDialogProcessorObserver interface
	virtual void MDPOReconnectComplete(TInt aError);

private:
	CReconnectRequest(MNetConEnv* aController, MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent, CStoreableOverrideSettings* aOverrides);
	void ConstructL();
	void StartReconnectL();

private:
	CNifAgentBase* const iAgent;

private:
	static TInt AsyncCb(TAny* aThisPtr);
	CAsyncCallBack* iAsyncCb;
	};

#endif // __CRECONNECTREQUEST_H__

