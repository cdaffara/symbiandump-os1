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

#include <e32debug.h>
#include <ssm/ssmadaptation.h>
#include <s32mem.h> 
#include "susmiscadaptation.h"

/**
@internalComponent
*/

void CMiscAdaptation::SubmitOrQueueL(const RMessage2 &aMessage)
	{

	CAdaptationMessage *messageCopy = new(ELeave) CAdaptationMessage(aMessage);

	if(!IsActive())
		{
		Submit(messageCopy);
		}
	else 
		{
		CleanupStack::PushL(messageCopy);
		DEBUGPRINT2A("CMiscAdaptationRequests queuing request with function id: %d", aMessage.Function());
		User::LeaveIfError(iPendingRequestsQueue.Queue(messageCopy));
		CleanupStack::Pop(messageCopy);
		}
	}

void CMiscAdaptation::Submit(CAdaptationMessage*& aMessage)
	{
	DEBUGPRINT2A("CMiscAdaptationRequests immediate submission of request with function id: %d", aMessage->Function());
	iCurrentMessage = aMessage;
	switch(aMessage->Function())
		{
		case ESecurityStateChange :
			{
			TInt state = aMessage->Int0 ( );
			iMiscAdaptation.SecurityStateChange(state,iResponsePckg,iStatus);
			break;
			}
		case EGetGlobalStartupMode :
			{
			iMiscAdaptation.GetGlobalStartupMode(iModePckg,iStatus);
			break;
			}
		case EPrepareSimLanguages :
			{
			TSsmLanguageListPriority priority = (TSsmLanguageListPriority)aMessage->Int0();
			iMiscAdaptation.PrepareSimLanguages(priority,iSizePckg,iStatus);
			break;
			}
		case EGetHiddenReset :
			{
			iMiscAdaptation.GetHiddenReset(iHiddenResetPckg, iStatus);
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
CMiscAdaptation implements Miscellaneous Adaptation related functionality as part of 
CSsmAdaptationServer.CSsmAdaptationServer loads Misc Adaptation plugin and creates
CMiscAdaptation using the NewL. From then the loaded Misc Adaptation plugin will be owned 
by CMiscAdaptation.

@internalComponent
*/
CMiscAdaptation* CMiscAdaptation::NewL(MMiscAdaptation& aAdaptation)
	{
	CMiscAdaptation* self = new(ELeave) CMiscAdaptation(aAdaptation);
	return self;	
	}

CMiscAdaptation::~CMiscAdaptation()
	{
	iPendingRequestsQueue.NotifyAndRemoveAll();
	Cancel();
	iPendingRequestsQueue.Close();
	Release();
	}

CMiscAdaptation::CMiscAdaptation(MMiscAdaptation& aAdaptation) :CActive(EPriorityStandard),
iMiscAdaptation(aAdaptation),iResponsePckg(0),iModePckg(0),iSizePckg(0),iListPckg(0)
	{
	CActiveScheduler::Add(this);	
	}

void CMiscAdaptation::Release()
	{
	iMiscAdaptation.Release();
	}

void CMiscAdaptation::DoSecurityStateChangeL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}
	
void CMiscAdaptation::DoGetGlobalStartupModeL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}

void CMiscAdaptation::DoPrepareSimLanguagesL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}

void CMiscAdaptation::DoGetSimLanguagesArrayL(const RMessage2& aMessage)
	{
	const TInt simLanguageCount = aMessage.Int1();
	CBufFlat* const buf=CBufFlat::NewL(simLanguageCount * sizeof(TInt));
	CleanupStack::PushL(buf);

	iMiscAdaptation.GetSimLanguagesL(buf, simLanguageCount);

	aMessage.WriteL(0, buf->Ptr(0));
	
	CleanupStack::PopAndDestroy(buf);
	}

void CMiscAdaptation::DoMiscAdaptationCancelL(const RMessage2& aMessage)
	{
	if(iCurrentMessage != NULL)	
		{
		if(aMessage.Session() == iCurrentMessage->Session())
			{
			DEBUGPRINT1A("CMiscAdaptationRequests cancelling current request as requested");
			iMiscAdaptation.Cancel();
			}
		iPendingRequestsQueue.RemoveFromQueueAndComplete(aMessage);  	
		aMessage.Complete(KErrNone);
		}
	else
		{
		DEBUGPRINT1A("CMiscAdaptationRequests nothing to cancel, but cancel requested");
		aMessage.Complete(KErrNone);				
		}	
	}

void CMiscAdaptation::DoMiscAdaptationHiddenResetL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);
	}

// from CActive
void CMiscAdaptation::RunL()
	{
	
	DEBUGPRINT2A("CMiscAdaptation processed the request with funtion id: %d", iCurrentMessage->Function());
	WriteResponseDataToClientMessageL();
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



void CMiscAdaptation::WriteResponseDataToClientMessageL()
	{
	switch(iCurrentMessage->Function())
		{
			case ESecurityStateChange:
				{
				iCurrentMessage->WriteL(1,iResponsePckg);
				break;					
				}
			case EGetGlobalStartupMode:
				{
				iCurrentMessage->WriteL(0,iModePckg);
				break;					
				}
			case EPrepareSimLanguages:
				{
				iCurrentMessage->WriteL(1,iSizePckg);
				break;					
				}
			case EGetHiddenReset :
				{
				iCurrentMessage->WriteL(0,iHiddenResetPckg);
				}				
			default:
				{
				break;					
				}				
		}
	}


TInt CMiscAdaptation::RunError( TInt aError)
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

void CMiscAdaptation::DoCancel()
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


