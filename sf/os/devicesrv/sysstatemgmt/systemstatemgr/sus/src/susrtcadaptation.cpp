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

#include <e32debug.h>
#include <ssm/ssmadaptation.h>
#include "susrtcadaptation.h"

/**
@publishedPartner
*/
void CRtcAdaptation::SubmitOrQueueL(const RMessage2 &aMessage)
	{
	CAdaptationMessage *messageCopy = new(ELeave) CAdaptationMessage(aMessage);
	
	if(!IsActive())
		{
		SubmitL(messageCopy);
		}
	else 
		{
		CleanupStack::PushL(messageCopy);
		DEBUGPRINT2A("CRtcAdaptationRequests queuing request with function id: %d", aMessage.Function());
		User::LeaveIfError(iPendingRequestsQueue.Queue(messageCopy));
		CleanupStack::Pop(messageCopy);
		}	
	}

void CRtcAdaptation::SubmitL(CAdaptationMessage*& aMessage)
	{
		DEBUGPRINT2A("CRtcAdaptationRequests immediate submission of request with function id: %d", aMessage->Function());
		iCurrentMessage = aMessage;
		switch(aMessage->Function())
			{
			case EValidateRtc :
				{
				iRtcAdaptation.ValidateRtc(iValidityPckg, iStatus);
				break;
				}
			case ESetWakeupAlarm :
				{
				iParam = HBufC8::NewL(aMessage->GetDesLength(0));
				aMessage->ReadL(0,iParam->Des());
 				iRtcAdaptation.SetWakeupAlarm(*iParam, iStatus);
				break;
				}
			case EUnsetWakeupAlarm :
				{
				iRtcAdaptation.UnsetWakeupAlarm(iStatus);
				break;
				}
			default :
				{
				break;
				}
			}
		SetActive();
	}


/**
CRtcAdaptation implements RTC Adaptation related functionality as part of CSsmAdaptationServer.
CSsmAdaptationServer loads RTC Adaptation plugin and creates CRtcAdaptation using the NewL.
From then the loaded RTC Adaptation plugin will be owned by CRtcAdaptation.

@internalComponent
*/

CRtcAdaptation* CRtcAdaptation::NewL(MRtcAdaptation& aAdaptation)
	{
	CRtcAdaptation* self = new(ELeave) CRtcAdaptation(aAdaptation);
	return self;	
	}

CRtcAdaptation::~CRtcAdaptation()
	{
	iPendingRequestsQueue.NotifyAndRemoveAll();
	Cancel();
	iPendingRequestsQueue.Close();
	Release();
	}

CRtcAdaptation::CRtcAdaptation(MRtcAdaptation& aAdaptation) : CActive(EPriorityStandard), iRtcAdaptation(aAdaptation)
	{
	CActiveScheduler::Add(this);
	}

void CRtcAdaptation::Release()
	{
	iRtcAdaptation.Release();
	}



void CRtcAdaptation::DoValidateRtcL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}

void CRtcAdaptation::DoSetWakeupAlarmL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}

void CRtcAdaptation::DoUnsetWakeupAlarmL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}



void CRtcAdaptation::DoRtcCancelL(const RMessage2& aMessage)
	{
	
	if(iCurrentMessage != NULL)	
		{
		if(aMessage.Session() == iCurrentMessage->Session())
			{
			DEBUGPRINT1A("CRtcAdaptationRequests cancelling current request as requested");
			iRtcAdaptation.Cancel();
			}
		iPendingRequestsQueue.RemoveFromQueueAndComplete(aMessage);  	
		aMessage.Complete(KErrNone);
		}
	else
		{
		DEBUGPRINT1A("CRtcAdaptationRequests nothing to cancel, but cancel requested");
		aMessage.Complete(KErrNone);				
		}
				
	}

void CRtcAdaptation::RunL()
	{
	
	WriteResponseDataToClientMessageL();

	DEBUGPRINT2A("CRtcAdaptation processed the request with funtion id: %d", iCurrentMessage->Function());
	iCurrentMessage->Complete(iStatus.Int());
	delete iCurrentMessage;
	delete iParam;
	iCurrentMessage = NULL;  
	iParam = NULL;

	if( (iPendingRequestsQueue.IsEmpty()) == EFalse )
		{
		CAdaptationMessage *messageCopy = NULL;
		iPendingRequestsQueue.Dequeue(messageCopy);
		SubmitL(messageCopy);
		} 
	}

void CRtcAdaptation::WriteResponseDataToClientMessageL()
	{
	switch(iCurrentMessage->Function())
		{
			case EValidateRtc:
				{
				iCurrentMessage->WriteL(0,iValidityPckg);
				break;					
				}
			default:
				{
				break;					
				}				
		}
	}


TInt CRtcAdaptation::RunError( TInt aError )
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

void CRtcAdaptation::DoCancel()
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

