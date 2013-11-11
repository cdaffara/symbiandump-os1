// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file provides the implementation of a class that
// completes sessions with LBS asynchronously.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/


#include "suplsessioncompleter.h"
#include "suplprotocolmanager.h"
#include "supldevloggermacros.h"

/** Static constructor.
@return A new instance of the CSuplSessionCompleter class.
*/  
CSuplSessionCompleter* CSuplSessionCompleter::NewL(MSuplProtocolManagerObserver& aGateway)
	{
	SUPLLOG(ELogP1, "CSuplSessionCompleter::NewL() Begin\n");
	CSuplSessionCompleter* self = new (ELeave) CSuplSessionCompleter(aGateway);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SUPLLOG(ELogP1, "CSuplSessionCompleter::NewL() End\n");
	return self;
	}


/** Standard constructor.
*/  
CSuplSessionCompleter::CSuplSessionCompleter(MSuplProtocolManagerObserver& aGateway): CActive(EPriorityLow),
							iGateway(aGateway)
	{
	SUPLLOG(ELogP1, "CSuplSessionCompleter::CSuplSessionCompleter() Begin\n");
	SUPLLOG(ELogP1, "CSuplSessionCompleter::CSuplSessionCompleter() End\n");
	}


/** Standard destructor.
*/  
CSuplSessionCompleter::~CSuplSessionCompleter()
	{
	SUPLLOG(ELogP1, "CSuplSessionCompleter::~CSuplSessionCompleter() Begin\n");
	iSessionInfo.Reset();
	SUPLLOG(ELogP1, "CSuplSessionCompleter::~CSuplSessionCompleter() End\n");
	}


/** Private second-stage constructor.
*/  
void CSuplSessionCompleter::ConstructL()
	{
	SUPLLOG(ELogP1, "CSuplSessionCompleter::ConstructL() Begin\n");
	CActiveScheduler::Add(this);
	SUPLLOG(ELogP1, "CSuplSessionCompleter::ConstructL() End\n");
	}


/** Store a session Id for later completion
*/  
void CSuplSessionCompleter::CompleteSession(const TLbsNetSessionId& aSessionId, TInt aError)
	{
	SUPLLOG(ELogP1, "CSuplSessionCompleter::CompleteSession() Begin\n");

	TSuplCompleteDetails completionInfo;
	completionInfo.iSessionId = aSessionId;
	completionInfo.iError = aError;
	
	TRAPD(err,iSessionInfo.InsertL(completionInfo,0));
	if (err != KErrNone)
		{
		// It is not possible to complete asynchronously,
		// so send a session complete indication now.
		iGateway.SessionCompleteInd(aSessionId, aError);
		}
	else
		{
		ActivateCompleter();
		}
	SUPLLOG(ELogP1, "CSuplSessionCompleter::CompleteSession() End\n");
	}
	
	
/** Store a session Id for later completion
*/  
void CSuplSessionCompleter::RunError()
	{
	SUPLLOG(ELogP1, "CSuplSessionCompleter::RunError() Begin\n");
	// Not much can be done.		
	SUPLLOG(ELogP1, "CSuplSessionCompleter::RunError() End\n");
	}
	
/**
*/
void CSuplSessionCompleter::DoCancel()
	{
	SUPLLOG(ELogP1, "CSuplSessionCompleter::DoCancel() Begin\n");
		
	SUPLLOG(ELogP1, "CSuplSessionCompleter::DoCancel() End\n");
	}

/** Take the next session Id stored and
send a session complete to LBS
*/  
void CSuplSessionCompleter::RunL()
	{
	SUPLLOG(ELogP1, "CSuplSessionCompleter::RunL() Begin\n");
	TLbsNetSessionId sessionId;
	TInt sessionErr = KErrNone;
	if(iSessionInfo.Count() > 0)
		{
		sessionId = iSessionInfo[iSessionInfo.Count() - 1].iSessionId;
		sessionErr = iSessionInfo[iSessionInfo.Count() - 1].iError;
		iSessionInfo.Remove(iSessionInfo.Count() - 1);
		}

	iGateway.SessionCompleteInd(sessionId, sessionErr);
	
	if(iSessionInfo.Count()>0)
		{
		ActivateCompleter();	
		}
	SUPLLOG(ELogP1, "CSuplSessionCompleter::RunL() End\n");
	}

void CSuplSessionCompleter::ActivateCompleter()
	{
	SUPLLOG(ELogP1, "CSuplSessionCompleter::ActivateCompleter() Begin\n");
	if(!IsActive())
		{
		TRequestStatus* localStatus = &iStatus;
		iStatus = KRequestPending;
		SetActive();
		User::RequestComplete(localStatus, KErrNone);
		}	
	SUPLLOG(ELogP1, "CSuplSessionCompleter::ActivateCompleter() End\n");
	}
