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
// Includes
// Component
// 
//

#include "lbsnetlocmanresponsehandler.h"
#include "lbsnetlocmanlogic.h"
#include "lbsdevloggermacros.h"
#include "LbsNetworkLocateLogEvent.h"


const TInt KLbsLogQueueSize = 10;

//****************************************************************
// Class CNetLocManResponseHandler
//****************************************************************
/** Static constructor.
@param aNetLocMan A reference to the parent class
@released
@internalComponent
*/
CNetLocManResponseHandler* CNetLocManResponseHandler::NewL(CNetLocManLogic& aNetLocMan)
	{
	CNetLocManResponseHandler* self = new(ELeave) CNetLocManResponseHandler (aNetLocMan);

	CleanupStack::PushL(self);

	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}
	
	
/** Constructor.
@param aNetLocMan A reference to the parent class
@released
@internalComponent
*/
CNetLocManResponseHandler::CNetLocManResponseHandler(CNetLocManLogic& aNetLocMan) : 
CActive(CActive::EPriorityStandard), iNetLocMan(aNetLocMan)
	{

	}


/** 
@see CActive::DoCancel
@released
@internalComponent
*/
void CNetLocManResponseHandler::DoCancel()
	{
// cancel async request
iPositionUpdate.CancelNotifyNetworkLocationUpdate();
iCompletionHandler->Cancel();
	}


/** Second phase constructor.
@released
@internalComponent
*/
void CNetLocManResponseHandler::ConstructL()
	{
	iPositionUpdate.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionReference);
iCompletionHandler = CNetLocManCompletionHandler::NewL(this);
	
	iLogger.Open(KLbsLogQueueSize);
	
	CActiveScheduler::Add(this);
	}


/** Destructor
@released
@internalComponent
*/
CNetLocManResponseHandler::~CNetLocManResponseHandler()
	{
	Cancel();
delete iCompletionHandler;
	iPositionUpdate.Close();
	iLogger.Close();
	}
 

/** 
@see CActive::RunError
@param aError An error code
@released
@internalComponent
*/
TInt CNetLocManResponseHandler::RunError(TInt aError)
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
void CNetLocManResponseHandler::StartGettingRequests()
	{
	LBSLOG(ELogP2, "CNetLocManResponseHandler::StartGettingRequests:");
	Cancel();
	// subscribe to property from Network Gateway -> Network Location Manager
	iPositionUpdate.NotifyNetworkLocationUpdate(iStatus);
	SetActive();	
	}

void CNetLocManResponseHandler::OnSessionComplete(TInt aStatus, 
							const TLbsNetSessionIdInt& aSessionId)
	{
	TLbsNetSessionIdInt sessionIdRequ;
	iNetLocMan.GetSessionId(sessionIdRequ);
	// Only action it if it relates to the current session Id
	if(aSessionId == sessionIdRequ)
		{
		// The position info is probably unnecessary (it's eitehr invalid or the 
		// client already has it), but pass it on anyway in case the client
		// is interested.
		TPositionCourseInfo posInfo;
		TTime targetTime;
		TTime actualTime;
		TLbsNetSessionIdInt sessionId;
		iPositionUpdate.GetPositionInfo(sessionId, posInfo, targetTime, actualTime);
	
		ReportPosUpdate(aStatus, aSessionId.SessionOwner(), posInfo, 0);
		}
	}

void CNetLocManResponseHandler::ReportPosUpdate(TInt aStatus, TUid aSessionOwner, 
											TPositionInfo& aPosInfo, TTime aTargetTime)
	{
	/** LBSLOGGER - Start Logging */
	// -------------------------------------------------------------------------
	CLbsNetworkLocateLogEvent* event = NULL;
	TRAPD(logErr, event = CLbsNetworkLocateLogEvent::NewL(aSessionOwner, NULL));
	if (logErr == KErrNone)
		{
		event->SetPositionInfo(&aPosInfo);
		// -------------------------------------------------------------------------
		if (KErrNone == aStatus)
			{
			event->SetRequestOutcome(ELbsRequestOutcomeSuccess);
			}
		else
			{
			event->SetRequestOutcome(ELbsRequestOutcomeFail);
			}
		iLogger.AddEvent(*event);
		delete event;
		}
		// -------------------------------------------------------------------------
		/** LBSLOGGER - End Logging */

		// deliver location info to NetLocManager
		LBSLOG(ELogP2, "CNetLocManResponseHandler::ReportPosUpdate: passing position update to Net Loc Manager");
		iNetLocMan.UpdateLocation(aStatus, aPosInfo, aTargetTime);

	}

/** Handle responses from the NetworkGateway
This function handles the incomming location responses from the
NetworkGateway, and pass them on to the LocServer, provided that
the session id match.
@see CActive::RunL
@released
@internalComponent
*/
void CNetLocManResponseHandler::RunL()
	{
	TInt status = iStatus.Int();
	TPositionCourseInfo posInfo;

	switch (status)
		{
		case KErrNone:
			{

			TLbsNetSessionIdInt sessionIdRequ;
			iNetLocMan.GetSessionId(sessionIdRequ);
			StartGettingRequests();			
			
			TTime targetTime;
			TTime actualTime;
			TLbsNetSessionIdInt sessionId;
			TInt error = iPositionUpdate.GetPositionInfo(sessionId, posInfo, targetTime, actualTime);

			LBSLOG3(ELogP2, "CNetLocManResponseHandler::RunL() Sessionid %d, %d", sessionId, sessionIdRequ);
			// Only pass on the update if it relates to a request issued through 
			// the Net Loc Manager
			if(sessionId == sessionIdRequ)
				{
				ReportPosUpdate(error, sessionId.SessionOwner(), posInfo, targetTime);
				}
			break;
			}
		default:
			{
			User::LeaveIfError(status);
			break;
			}
		}
	}
