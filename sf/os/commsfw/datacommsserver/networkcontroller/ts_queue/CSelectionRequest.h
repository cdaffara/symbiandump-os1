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

#if !defined (__CSELECTIONREQUEST_H__)
#define __CSELECTIONREQUEST_H__

#include "CNetConRequestBase.h"
#include <commdbconnpref.h>
#include <comms-infras/nifif.h>

class CSelectionRequest : public CNetConRequestBase
	{
public:
	static CSelectionRequest* NewL(MNetConEnv* aController, MNetworkControllerObserver* aObserver, TConnStartType aConnStartType, const TConnPref& aPrefs, TInt aConnectionAttempt, TInt aLastConnectionError);
	virtual ~CSelectionRequest();

	// start processing this request
	virtual void StartRequest();

	// force premature completion of a request with a given error
	virtual void RequestComplete(TInt aError);

	// cancel this request
	virtual void CancelRequest();

	// bearer availability
	virtual void SetAvailableBearers(TUint32 aBearerSet);

	// accessors
	const TDesC& AgentName() const;
	const TPckgBuf<TSoIfConnectionInfo>& AgentConnectionInfo() const;
	const TConnStartType& ConnectionStartType() const;
	const TConnPref& ConnPrefs() const;

private:
	CSelectionRequest(MNetConEnv* aController, MNetworkControllerObserver* aObserver, TConnStartType aConnStartType, const TConnPref& aPrefs, TInt aConnectionAttempt, TInt aLastConnectionError);
	void ConstructL();

	static TInt AsyncCb(TAny* aThisPtr);
	void AsyncCbComplete();

private:
	TConnStartType iConnStartType;
	TConnPref iConnPrefs;
	TInt iConnectionAttempt;
	TInt iLastConnectionError;
	TPckgBuf<TSoIfConnectionInfo> iAgentConnInfo;
	CAsyncCallBack* iAsyncCb;
	};

#endif // __CSELECTIONREQUEST_H__

