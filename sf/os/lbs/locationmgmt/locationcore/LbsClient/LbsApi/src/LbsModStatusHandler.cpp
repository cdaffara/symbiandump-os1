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
//

#include "LbsModStatusHandler.h"
#include "LbsExpansion.h"
#include "lbsdevloggermacros.h"

	
CModStatusHandler * CModStatusHandler::NewL(TUid aModuleId,MModuleStatusObserver* aObserver)
	{

	CModStatusHandler * self = new(ELeave) CModStatusHandler (aModuleId, aObserver);

	CleanupStack::PushL(self);

	self->ConstructL(aModuleId);
	CleanupStack::Pop();

	return self;
	}
	

	
	
CModStatusHandler::CModStatusHandler(TUid aModuleId, MModuleStatusObserver* aObserver) : 
CActive(CActive::EPriorityStandard),	
iModuleId(aModuleId),
iObserver(aObserver)
	{

	}

void CModStatusHandler::DoCancel()
	{
	// cancel RLbsModuleStatus async request
	iModStatus.CancelNotifyModuleStatusChange();
	}

void CModStatusHandler::StartGettingStatusChanges(
						TRequestStatus& aRequestStatusOfClient,
						TPositionModuleStatusEventBase& aStatusEvent)

	{
	aRequestStatusOfClient = KRequestPending;
	iRequestStatusOfClient = &aRequestStatusOfClient;
	iStatusEvent = &aStatusEvent;
	Cancel();
	iModStatus.NotifyModuleStatusChange(iStatus);
	SetActive();	
	}

void CModStatusHandler::ConstructL(TUid aModuleId)
	{

	iModStatus.OpenL(aModuleId);
	
	CActiveScheduler::Add(this);
	}

//------------------------------------------------------------------------------------------------------------
CModStatusHandler::~CModStatusHandler()
	{
	Cancel();
	iModStatus.Close();
	}
 

TInt CModStatusHandler::RunError(TInt aError)
{
 	return aError;
}

void CModStatusHandler::RunL()
	{
	TInt status = iStatus.Int();
	
 	switch (status)
		{
		case KErrNone:
			{
			TPositionModuleStatus posModuleStatus;
			TPositionModuleStatusEventBase::TModuleEvent occurredEventsSinceLastGet;

			TInt res =	iModStatus.GetModuleStatus(&posModuleStatus, sizeof(posModuleStatus),occurredEventsSinceLastGet);

			TPositionModuleStatusEventBase::TModuleEvent requestedEvents
			 			=iStatusEvent->RequestedEvents();
    		
    		if (requestedEvents & occurredEventsSinceLastGet)
    			{

    			iStatusEvent->SetOccurredEvents(occurredEventsSinceLastGet);
    			iStatusEvent->SetModuleId(iModuleId);
 
 	        	if (iStatusEvent->PositionClassSize() >= sizeof(TPositionModuleStatus))
	        		{
	        		TPositionModuleStatusEvent* posModStatEvent = 
						(reinterpret_cast<TPositionModuleStatusEvent*>(iStatusEvent));
	        		posModStatEvent->SetModuleStatus(posModuleStatus);
					}
 	        	
#ifdef ENABLE_LBS_DEV_LOGGER
 	        		LBSLOG(ELogP9, "<-A RPositionServer::RunL() lbslocserver.exe\n");
 	        		TPositionModuleStatusEventBase tempModuleStatus(*iStatusEvent);
 	        		LBSLOG(ELogP9, "  < TPositionModuleStatusEventBase aStatusEvent  =\n");
 	        		LBSLOG_POSMODULESTATUSEVENT(tempModuleStatus);
#endif

				iObserver->ModuleStatusChanged();	
    		
 				User::RequestComplete(iRequestStatusOfClient, KErrNone);
    			}
    		else
    			{
    			// not an event of any interest to us so wait for another
    			iModStatus.NotifyModuleStatusChange(iStatus);
				SetActive();
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
	
TUid CModStatusHandler::ModuleId()
	{
	return iModuleId;
	}


TRequestStatus*& CModStatusHandler::RequestStatusOfClient()
	{
	return iRequestStatusOfClient;
	}	
	


