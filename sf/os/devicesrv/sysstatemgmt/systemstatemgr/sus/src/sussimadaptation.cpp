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
//

#include "sussimadaptation.h"
#include <e32debug.h>

/**
@internalComponent
*/
/*
	//Add aMessage to the queue if AO is busy
	//else
	//Store the message in iCurrentMessage so that RunL can call aMessage::Complete()
	//Unpack params 
	//Submit request
*/ 


void CSimAdaptationRequests::SubmitOrQueueL(const RMessage2 &aMessage)
	{
	CAdaptationMessage *messageCopy = new(ELeave) CAdaptationMessage(aMessage);
		
	if(!IsActive())
		{
		Submit(messageCopy);
		}
	else 
		{
		CleanupStack::PushL(messageCopy);
		DEBUGPRINT2A("CSimAdaptationRequests queuing request with function id: %d", aMessage.Function());
		User::LeaveIfError(iPendingRequestsQueue.Queue(messageCopy));
		CleanupStack::Pop(messageCopy);
		}	
	}

void CSimAdaptationRequests::Submit(CAdaptationMessage*& aMessage)
	{
	DEBUGPRINT2A("CSimAdaptationRequests immediate submission of request with function id: %d", aMessage->Function());
	iCurrentMessage = aMessage;
	switch(aMessage->Function())
		{
		case EGetSimOwned :
			{
			iSimAdaptation.GetSimOwned(iSimOwnedPckg, iStatus);
			break;	
			}
		default :
			{
			aMessage->Complete(KErrArgument);
			return;
			}
		}
	SetActive();
	}

/**
CSimAdaptationRequests implements Sim Adaptation related functionality as part of CSsmAdaptationServer.
CSsmAdaptationServer loads Sim Adaptation plugin and creates CSimAdaptationRequests using the NewL.
From then the loaded Sim Adaptation plugin will be owned by CSimAdaptationRequests.

@internalComponent
*/
CSimAdaptationRequests* CSimAdaptationRequests::NewL(MSimAdaptation& aAdaptation)
	{
	CSimAdaptationRequests* self = new(ELeave) CSimAdaptationRequests(aAdaptation);
	return self;
	}

CSimAdaptationRequests::CSimAdaptationRequests(MSimAdaptation& aAdaptation) : CActive(EPriorityStandard), iSimAdaptation(aAdaptation)
	{
	CActiveScheduler::Add(this);
	}

CSimAdaptationRequests::~CSimAdaptationRequests()
	{
	iPendingRequestsQueue.NotifyAndRemoveAll();
	Cancel();	// This call will delete iCurrentMessage if any
	iPendingRequestsQueue.Close();
	Release();
	}

void CSimAdaptationRequests::Release()
	{
	iSimAdaptation.Release();
	}

void CSimAdaptationRequests::DoGetSimOwnedL(const RMessage2& aMessage)
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

void CSimAdaptationRequests::DoGetSimCancel(const RMessage2& aMessage)
	{
	
	if(iCurrentMessage != NULL)	
		{
		if(aMessage.Session() == iCurrentMessage->Session())
			{
			DEBUGPRINT1A("CSimAdaptationRequests cancelling current request as requested");
			iSimAdaptation.GetCancel();
			}
		iPendingRequestsQueue.RemoveFromQueueAndComplete(aMessage);  	
		aMessage.Complete(KErrNone);
		}
	else
		{
		DEBUGPRINT1A("CSimAdaptationRequests nothing to cancel, but cancel requested");
		aMessage.Complete(KErrNone);				
		}
					
	}

/**
 * Returns the SIM adaptation in use by this object
 * 
 * @internalComponent
 */
MSimAdaptation& CSimAdaptationRequests::Adaptation()
	{
	return iSimAdaptation;
	}

void CSimAdaptationRequests::RunL()
	{
	
	WriteResponseDataToClientMessageL();

	DEBUGPRINT2A("CSimAdaptationRequests processed the request with funtion id: %d", iCurrentMessage->Function());
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

void CSimAdaptationRequests::WriteResponseDataToClientMessageL()
	{
	switch(iCurrentMessage->Function())
		{
			case EGetSimOwned:
				{
				iCurrentMessage->WriteL(0,iSimOwnedPckg);
				break;					
				}
			default:
				{
				break;					
				}				
		}			
	}
TInt CSimAdaptationRequests::RunError( TInt aError )
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

void CSimAdaptationRequests::DoCancel()
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

// class CSimAdaptationObservers
CSimAdaptationObservers* CSimAdaptationObservers::NewL(MSimAdaptation& aAdaptation)
	{
	CSimAdaptationObservers* self = new(ELeave) CSimAdaptationObservers(aAdaptation);

	CleanupStack::PushL(self);
	CleanupStack::Pop(); // self
	return self;
	}

CSimAdaptationObservers::CSimAdaptationObservers(MSimAdaptation& aAdaptation) : CActive(EPriorityStandard), iSimAdaptation(aAdaptation)
	{
	CActiveScheduler::Add(this);
	}

CSimAdaptationObservers::~CSimAdaptationObservers()
	{
	Cancel();
	iObserversList.Close();
	}

void CSimAdaptationObservers::DoGetLastSimEvent(const RMessage2& aMessage)
	{
	TRAPD(err,aMessage.WriteL(0, iEventPckg));
	aMessage.Complete(err);	
	}

void CSimAdaptationObservers::DoNotifySimEventL(const RMessage2& aMessage)
	{
	if(iObserversList.Count() == 0)
		{
		// First observer so start notification
		StartNotification();
		}
	CAdaptationMessage *newObserver = new(ELeave) CAdaptationMessage(aMessage);
	CleanupStack::PushL(newObserver);
	iObserversList.AddObserverL(newObserver);
	CleanupStack::Pop();//newObserver
	}

void CSimAdaptationObservers::DoNotifySimEventCancelL(const RMessage2& aMessage)
	{
	iSimAdaptation.NotifyCancel();
	CAdaptationMessage *newObserver = new(ELeave) CAdaptationMessage(aMessage);
	iObserversList.RemoveObserver(newObserver);
	aMessage.Complete(KErrNone); 
	
	if(iObserversList.Count() == 0)
		{
		// Last observer cancelled, stop notification
		if(IsActive())
			{
			Cancel();
			}
		}
	}

void CSimAdaptationObservers::StartNotification()
	{
	DEBUGPRINT1A("CSimAdaptationObservers starting request for event notification");
	iSimAdaptation.NotifySimEvent(iEventPckg,iStatus);

	SetActive();	
	}

void CSimAdaptationObservers::RunL()
	{
	DEBUGPRINT1A("CSimAdaptationObservers received event notification");
	iObserversList.NotifyAndRemoveAll(iEventPckg(),iStatus.Int());
	}

TInt CSimAdaptationObservers::RunError( TInt /*aError */)
	{
	iObserversList.Close();
	return KErrNone;
	}

void CSimAdaptationObservers::DoCancel()
	{
	DEBUGPRINT1A("CSimAdaptationObservers cancelling request for event notification");
	iSimAdaptation.NotifyCancel();
	iObserversList.NotifyAndRemoveAll(iEventPckg(), KErrCancel);
	}

//Observer related functionality
void RSimAdaptationObserversList::AddObserverL(CAdaptationMessage *aNotificationMessage)
	{
	//any error will cause a leave
	iObservers.AppendL(aNotificationMessage);
	}
void RSimAdaptationObserversList::Close()
	{
	// Notify all clients first
	NotifyAndRemoveAll(static_cast<TSsmSimEventType>(0), KErrCancel);
	// Call RArray close()
	iObservers.Close();
	}

TInt RSimAdaptationObserversList::Count()
	{
	return iObservers.Count();
	}

void RSimAdaptationObserversList::NotifyAndRemoveAll(TSsmSimEventType aEventType,TInt aCompleteCode)
	{
	TInt index,count = iObservers.Count();
	
	for(index =0;index < count ;index++)
		{
		TPckgBuf<TSsmSimEventType> pckgEvType(aEventType);
		
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
/*
ALGO	
	//parse TRequestStatus from each CAdaptationMessage and
	//Find or FindL 
	//Index will be returned
	//then call Remove()  with Index in the array
*/

void RSimAdaptationObserversList::RemoveObserver(CAdaptationMessage *aCancelMessage)
	{
	if(aCancelMessage == NULL)
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
		if( notificationMessage->Session() == aCancelMessage->Session())
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











