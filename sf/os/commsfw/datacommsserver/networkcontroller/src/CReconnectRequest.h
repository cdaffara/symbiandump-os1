/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file CReconnectRequest.h
 @internalComponent
*/

#if !defined (__CRECONNECTREQUEST_H__)
#define __CRECONNECTREQUEST_H__

#include "CNetConRequestBase.h"

class CNifAgentBase;
NONSHARABLE_CLASS(CReconnectRequest) : public CNetConRequestBase
/**
Decides whether or not to re-establish a failed connection.

@internalComponent
*/
	{
public:
	static CReconnectRequest* NewL(MNetConEnv* aController, MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent, CStoreableOverrideSettings* aOverrides);
	virtual ~CReconnectRequest();

	virtual void StartRequest();							///< start processing this request
	virtual void RequestComplete(TInt aError);				///< force premature completion of a request with a given error
	virtual void SetAvailableBearers(TUint32 aBearerSet);	///< bearer availability
	virtual void MDPOReconnectComplete(TInt aError);		///< override part of the MDialogProcessorObserver interface

private:
	CReconnectRequest(MNetConEnv* aController, MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent, CStoreableOverrideSettings* aOverrides);
	void ConstructL();
	void StartReconnectL();

private:
	CNifAgentBase* const iAgent;
	};

#endif // __CRECONNECTREQUEST_H__

