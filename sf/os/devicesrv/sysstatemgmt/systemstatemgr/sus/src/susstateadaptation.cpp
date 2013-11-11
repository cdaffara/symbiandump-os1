// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "susstateadaptation.h"
#include <e32debug.h>

/**
@publishedPartner
*/

/*
	//Add aMessage to the queue if AO is busy
	//else
	//Store the message in iCurrentMessage so that RunL can call aMessage::Complete()
	//Unpack params 
	//Submit request
*/ 

void CStateAdaptationRequests::SubmitOrQueueL(const RMessage2 &aMessage)
	{
	
	CAdaptationMessage *messageCopy = new(ELeave) CAdaptationMessage(aMessage);
		
	if(!IsActive())
		{
		Submit(messageCopy);
		}
	else 
		{
		CleanupStack::PushL(messageCopy);
		DEBUGPRINT2A("CStateAdaptationRequests queuing request with function id: %d", aMessage.Function());
		User::LeaveIfError(iPendingRequestsQueue.Queue(messageCopy));
		CleanupStack::Pop(messageCopy);
		}	
	}
 
void CStateAdaptationRequests::Submit(CAdaptationMessage*& aMessage)
	{
	DEBUGPRINT2A("CStateAdaptationRequests immediate submission of request with function id: %d", aMessage->Function());
	iCurrentMessage = aMessage;
	switch(aMessage->Function())
		{
		case ERequestCoopSysStateChange :
			{
			TSsmState newState;
			newState.SetFromInt(aMessage->Int0());
			iStateAdaptation.RequestCoopSysStateChange(newState, iStatus);
			break;
			}
		case ERequestCoopSysSelfTest :
			{
			iStateAdaptation.RequestCoopSysSelfTest(iStatus);
			break;
			}
		case ERequestCoopSysPerformRestartActions :
			{
			TInt reason = aMessage->Int0();
			iStateAdaptation.RequestCoopSysPerformRestartActions(reason, iStatus);
			break;
			}
		case ERequestCoopSysPerformShutdownActions :
			{
			TInt reason = aMessage->Int0();
			iStateAdaptation.RequestCoopSysPerformShutdownActions(reason, iStatus);
			break;
			}
		case ERequestCoopSysPerformRfsActions :
			{
			TSsmRfsType rfsType = ( TSsmRfsType )aMessage->Int0() ;
			iStateAdaptation.RequestCoopSysPerformRfsActions(rfsType, iStatus);
			break;
			}
		default:
			{
			aMessage->Complete(KErrArgument);
			return;
			}
		}
	SetActive();
	}

/**
CStateAdaptationRequests implements State Adaptation related functionality as part of CSsmAdaptationServer.
CSsmAdaptationServer loads State Adaptation plugin and creates CStateAdaptationRequests using the NewL.
From then the loaded State Adaptation plugin will be owned by CStateAdaptationRequests.

@internalComponent
*/



CStateAdaptationRequests* CStateAdaptationRequests::NewL(MStateAdaptation& aAdaptation)
	{
	CStateAdaptationRequests* self = new(ELeave) CStateAdaptationRequests(aAdaptation);
	return self;
	} 

CStateAdaptationRequests::CStateAdaptationRequests(MStateAdaptation& aAdaptation) : CActive(EPriorityStandard), iStateAdaptation(aAdaptation)
	{
	CActiveScheduler::Add(this);
	}

CStateAdaptationRequests::~CStateAdaptationRequests()
	{
	iPendingRequestsQueue.NotifyAndRemoveAll();
	Cancel();	// This call will delete iCurrentMessage if any
	iPendingRequestsQueue.Close();
	Release();
	}

//from MstateAdaptation
void CStateAdaptationRequests::Release()
	{
	iStateAdaptation.Release();
	}


void CStateAdaptationRequests::DoRequestCoopSysStateChangeL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}  

void CStateAdaptationRequests::DoRequestCoopSysSelfTestL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}

void CStateAdaptationRequests::DoRequestCoopSysPerformRestartActionsL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}

void CStateAdaptationRequests::DoRequestCoopSysPerformShutdownActionsL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}

void CStateAdaptationRequests::DoRequestCoopSysPerformRfsActionsL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}

/*

ALGO
	If iCurrentMessage == aMessage
	then call plugin's RequestCancel()
	otherwise 
	search the queue and complete the message with KErrCancel
*/

void CStateAdaptationRequests::DoRequestCancel(const RMessage2& aMessage)
	{

	if(iCurrentMessage != NULL)	
		{
		if(aMessage.Session() == iCurrentMessage->Session())
			{
			DEBUGPRINT1A("CStateAdaptationRequests cancelling current request as requested");
			iStateAdaptation.RequestCancel();
			}
		iPendingRequestsQueue.RemoveFromQueueAndComplete(aMessage); 	
		if(iPendingRequestsQueue.IsEmpty())
		    {
		    iPendingRequestsQueue.Close();
		    }
		aMessage.Complete(KErrNone);
		}
	else
		{
		DEBUGPRINT1A("CStateAdaptationRequests nothing to cancel, but cancel requested");
		aMessage.Complete(KErrNone);				
		}
		
	}

/**
 * Returns the state adaptation in use by this object
 * 
 * @internalComponent
 */
MStateAdaptation& CStateAdaptationRequests::Adaptation()
	{
	return iStateAdaptation;
	}

/*
	from CActive
	Complete aMessage
	Dequeue the queue and submit another request
*/

void CStateAdaptationRequests::RunL()
	{
	
	DEBUGPRINT2A("CStateAdaptationRequests processed the request with funtion id: %d", iCurrentMessage->Function());
	iCurrentMessage->Complete(iStatus.Int());
	delete iCurrentMessage;
	iCurrentMessage = NULL;  

	if( (iPendingRequestsQueue.IsEmpty()) == EFalse )
		{
		CAdaptationMessage *messageCopy = NULL;
		iPendingRequestsQueue.Dequeue(messageCopy);
		Submit(messageCopy);
		} 
	}

TInt CStateAdaptationRequests::RunError( TInt aError)
	{
	if(iCurrentMessage != NULL)	
		{
		iCurrentMessage->Complete(aError);
		delete iCurrentMessage;
		iCurrentMessage = NULL; 
		}
	
	while( (iPendingRequestsQueue.IsEmpty()) == EFalse )
		{
		iPendingRequestsQueue.Dequeue(iCurrentMessage);
		iCurrentMessage->Complete(aError);
		delete iCurrentMessage;
		iCurrentMessage = NULL;
		}
	
	return KErrNone;
	}

void CStateAdaptationRequests::DoCancel()
	{
	if(iCurrentMessage != NULL)	
		{
		iCurrentMessage->Complete(KErrCancel);
		delete iCurrentMessage;
		iCurrentMessage = NULL;
		}
		
	while( (iPendingRequestsQueue.IsEmpty()) == EFalse )
		{
		iPendingRequestsQueue.Dequeue(iCurrentMessage);
		iCurrentMessage->Complete(KErrCancel);
		delete iCurrentMessage;
		iCurrentMessage = NULL;
		}
	}

	
// C Class
CStateAdaptationObservers* CStateAdaptationObservers::NewL(MStateAdaptation& aAdaptation)
	{
	CStateAdaptationObservers* self = new(ELeave) CStateAdaptationObservers(aAdaptation);
	
	CleanupStack::PushL(self);
	CleanupStack::Pop(); // self
	return self;
	}

CStateAdaptationObservers::CStateAdaptationObservers(MStateAdaptation& aAdaptation) : CActive(EPriorityStandard), iStateAdaptation(aAdaptation)
	{
	CActiveScheduler::Add(this); 
	}

CStateAdaptationObservers::~CStateAdaptationObservers()
	{
	Cancel();
	iObserversList.Close();
	}

//from MstateAdaptation

void CStateAdaptationObservers::DoGetLastCoopSysEvent(const RMessage2& aMessage)
	{
	TRAPD(err,aMessage.WriteL(0,iEventPckg));
	aMessage.Complete(err);	
	}


void CStateAdaptationObservers::DoNotifyCoopSysEventL(const RMessage2& aMessage)
	{
	if(iObserversList.Count() == 0)
		{
		// First observer so start notification
		StartNotification();
		}
	CAdaptationMessage *newObserver = new(ELeave) CAdaptationMessage(aMessage);
	CleanupStack::PushL(newObserver);
	iObserversList.AddObserverL(newObserver);	// CAdaptationMessage's ownership is passed to iObserversList
	CleanupStack::Pop();//newObserver
	}

void CStateAdaptationObservers::DoNotifyCoopSysEventCancelL(const RMessage2& aMessage)
	{
	iStateAdaptation.NotifyCancel();
	CAdaptationMessage *newObserver = new(ELeave) CAdaptationMessage(aMessage);
	iObserversList.RemoveObserver(newObserver);	// CAdaptationMessage's ownership is passed to iObserversList
	aMessage.Complete(KErrNone); 
	
	if(iObserversList.Count() == 0)
		{
		// Last observer cancelled, stop notification
		if (IsActive())
			{
			Cancel();
			}
		}
	}


/**
 * Starts notification for this active object and sets it active
 * 
 * @internalComponent
 */
void CStateAdaptationObservers::StartNotification()
	{
	DEBUGPRINT1A("CStateAdaptationObservers starting request for event notification");
	iStateAdaptation.NotifyCoopSysEvent(iEventPckg,iStatus);

	SetActive();	
	}

//from CActive


void CStateAdaptationObservers::RunL()
	{
	DEBUGPRINT1A("CStateAdaptationObservers received event notification");
	iObserversList.NotifyAndRemoveAll(iEventPckg(),iStatus.Int());
	}

TInt CStateAdaptationObservers::RunError( TInt /*aError */)
	{
	iObserversList.Close();
	return KErrNone;
	}

void CStateAdaptationObservers::DoCancel()
	{
	DEBUGPRINT1A("CStateAdaptationObservers cancelling request for event notification");
	iStateAdaptation.NotifyCancel();
	iObserversList.NotifyAndRemoveAll(iEventPckg(), KErrCancel);
	}


//Observer related functionality
void RStateAdaptationObserversList::AddObserverL(CAdaptationMessage *aNotificationMessage)
	{
	//any error will cause a leave
	iObservers.AppendL(aNotificationMessage);
	}
void RStateAdaptationObserversList::Close()
	{
	// Before close, cleanup and signal all clients
	NotifyAndRemoveAll(static_cast<TSsmCoopSysEventType>(0), KErrCancel);
	// Now free the resources in the array
	iObservers.Close();
	}

TInt RStateAdaptationObserversList::Count()
	{
	return iObservers.Count();
	}

void RStateAdaptationObserversList::NotifyAndRemoveAll(TSsmCoopSysEventType aEventType,TInt aCompleteCode)
	{
	TInt index,count = iObservers.Count();
	
	for(index =0;index < count ;index++)
		{
		TPckg<TSsmCoopSysEventType> pckgEvType(aEventType);
		
		// Complete the client with the requested code unless
		// the descriptor write fails
		TInt completeCode = aCompleteCode;
		// Only copy across the event type if it was successful
		if(aCompleteCode == KErrNone)
			{		
			TRAPD(err,iObservers[index]->WriteL(0,pckgEvType));
			if(err != KErrNone)
				{
				completeCode = err;
				}
			}
		iObservers[index]->Complete(completeCode);
		delete iObservers[index];
		iObservers[index] = NULL;
		}
	iObservers.Reset();
	}


void RStateAdaptationObserversList::RemoveObserver(CAdaptationMessage *aCancelMessage)
	{
	if (aCancelMessage == NULL)
		{
		return;
		}
	TInt index,count = iObservers.Count();
	CAdaptationMessage *notificationMessage;
	for(index =0; index < count; index++)
		{
		notificationMessage = iObservers[index];
		
		// Compare on session pointers to check that the cancel only happens to notifications from the
		// same session
		if( notificationMessage->Session() == aCancelMessage->Session() )
			{
			CAdaptationMessage *message= iObservers[index];
			iObservers[index]->Complete(KErrCancel);
			iObservers.Remove(index);
			delete message;
			break;
			}
		}
	delete aCancelMessage;
	}





