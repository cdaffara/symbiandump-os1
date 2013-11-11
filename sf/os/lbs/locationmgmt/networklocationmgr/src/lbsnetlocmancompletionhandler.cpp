// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// lbsnetlocmanresponsehandler.cpp
// Includes
// Component
// 
//

#include "lbsnetlocmancompletionhandler.h"
#include "lbsnetlocmanlogic.h"
#include "lbsdevloggermacros.h"


//****************************************************************
// Class CNetLocManCompletionHandler
//****************************************************************
/** Static constructor.
@param aCompletionObserver A pointer to the observing class
@released
@internalComponent
*/
CNetLocManCompletionHandler* CNetLocManCompletionHandler::NewL(MLbsLocSessionCompleteObserver* aCompletionObserver)
	{
	
	CNetLocManCompletionHandler* self = new(ELeave) CNetLocManCompletionHandler (aCompletionObserver);

	CleanupStack::PushL(self);

	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}
	
	
/** Constructor.
@param aCompletionObserver A pointer to the observing class
@released
@internalComponent
*/
CNetLocManCompletionHandler::CNetLocManCompletionHandler(MLbsLocSessionCompleteObserver* aCompletionObserver) : 
CActive(CActive::EPriorityStandard), iCompletionObserver(aCompletionObserver)
	{

	}


/** 
@see CActive::DoCancel
@released
@internalComponent
*/
void CNetLocManCompletionHandler::DoCancel()
	{
	// cancel RLbsNetworkLocationResponses async request
	iLocationResp.CancelNotifyNetworkLocationResponseUpdate();
	}


/** Second phase constructor.
@released
@internalComponent
*/
void CNetLocManCompletionHandler::ConstructL()
	{
	iLocationResp.OpenL(KLbsNetLocManagerUid);
	
	
	CActiveScheduler::Add(this);
	StartMonitor();
	}


/** Destructor
@released
@internalComponent
*/
CNetLocManCompletionHandler::~CNetLocManCompletionHandler()
	{
	Cancel();
	iLocationResp.Close();
	}
 

/** 
@see CActive::RunError
@param aError An error code
@released
@internalComponent
*/
TInt CNetLocManCompletionHandler::RunError(TInt aError)
	{
	// handle RunL leaving - in our case we do nothing 
	// as this means we have had encountered a program inconsistency
	// By doing nothing here  causes the main manager thread to exit
	// and the Lbs root process resolves any issues - by stopping and restarting
	// lbs components
 	return aError;
	}	


/** Subscribe to the location update response property
@released
@internalComponent
*/
void CNetLocManCompletionHandler::StartMonitor()
	{
	LBSLOG(ELogP2, "CNetLocManCompletionHandler::StartMonitor:");
	Cancel();
	// subscribe to property from Network Gateway -> Network Location Manager
	iLocationResp.NotifyNetworkLocationResponseUpdate(iStatus);
	SetActive();	
	}


/** Handle responses from the NetworkGateway
This function handles the incomming location responses from the
NetworkGateway, and pass them on to the LocServer, provided that
the session id match.
@see CActive::RunL
@released
@internalComponent
*/
void CNetLocManCompletionHandler::RunL()
	{
	TInt status = iStatus.Int();

	StartMonitor();
		
	if(KErrNone == status)
		{
		const TLbsNetSessionIdInt KInvalidSessionId;
		TLbsNetLocNetworkLocationCompleteMsg updateResponse(KInvalidSessionId, KErrNone); 
		User::LeaveIfError(iLocationResp.GetNetworkLocationResponse(updateResponse));

		iCompletionObserver->OnSessionComplete(updateResponse.Reason(), 
											   updateResponse.SessionId());
		}
	else
		{
		User::Leave(status);
		}		
	}
