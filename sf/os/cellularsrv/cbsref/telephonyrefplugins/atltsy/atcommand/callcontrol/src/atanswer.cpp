// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file atanswer.cpp
// This contains CATAnswer which answering incomging call.
// 

//system include

//user include
#include "atanswer.h"
#include "ltsycallinformationmanager.h"
#include "mslogger.h"

//const define
const TInt KLtsyAnswerReadTimeOut = 20;
_LIT8(KLtsyAnswerCallCommand, "ATA\r");


// ---------------------------------------------------------------------------
// CATAnswer::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATAnswer* CATAnswer::NewL(CGlobalPhonemanager& aGloblePhone,
						   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATAnswer::NewL()"));
	
	CATAnswer* self = CATAnswer::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


// ---------------------------------------------------------------------------
// CATAnswer::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CATAnswer* CATAnswer::NewLC(CGlobalPhonemanager& aGloblePhone,
						    CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATAnswer::NewLC()"));
	
	CATAnswer* self = new (ELeave) CATAnswer(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CATAnswer::~CATAnswer
// other items were commented in a header
// ---------------------------------------------------------------------------
CATAnswer::~CATAnswer()
	{
	}

// ---------------------------------------------------------------------------
// CATAnswer::SetAnswerCallId
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATAnswer::SetAnswerCallId(TInt aCallId)
	{
	iCallId = aCallId;
	}


// ---------------------------------------------------------------------------
// CATAnswer::SetIsIsvCall
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATAnswer::SetIsIsvCall(TBool aIsIsvCall)
	{
	iIsIsvCall = aIsIsvCall;
	}


// ---------------------------------------------------------------------------
// CATAnswer::SetIsIsvCall
// other items were commented in a header
// ---------------------------------------------------------------------------
CATAnswer::CATAnswer(CGlobalPhonemanager& aGloblePhone,
		             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
					 :CAtCommandBase(aGloblePhone, aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATAnswer::CATAnswer()"));
	
	iIOStatus = KErrNone;
	iATResult = KErrNone;
	iAnswerStep = EATNotInProgress;
	}


// ---------------------------------------------------------------------------
// CATAnswer::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATAnswer::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATAnswer::ConstructL()"));
	
	CAtCommandBase::ConstructL();
	AddExpectStringL(KLtsyNoCarrierString);
	
	//Set read and write timeout
	SetTimeOut(KLtsyDefaultWriteTimeOut, KLtsyAnswerReadTimeOut);	
	}

// ---------------------------------------------------------------------------
// CATAnswer::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATAnswer::ExecuteCommand()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATAnswer::ExecuteCommand()"));
	
	//Write AT command
	iTxBuffer.Copy(KLtsyAnswerCallCommand);
	Write();
	iAnswerStep = EATWaitForWriteComplete;
	}

// ---------------------------------------------------------------------------
// CATAnswer::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATAnswer::StartRequest()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATAnswer::StartRequest()"));
	
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATAnswer::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATAnswer::ParseResponseL(const TDesC8& aResponseBuf)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATAnswer::ParseResponseL()"));
	
	if (aResponseBuf.Match(KLtsyOkString) == 0)
		{
		iATResult = KErrNone;
		}
	else if (aResponseBuf.Match(KLtsyErrorString) == 0)
		{
		iATResult = KErrArgument;
		}
	else if (aResponseBuf.Match(KLtsyNoCarrierString) == 0)
		{
		iATResult = KErrEtelNoCarrier;
		}
	}

// ---------------------------------------------------------------------------
// CATAnswer::HandleIOError
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATAnswer::HandleIOError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATAnswer::HandleIOError()"));
	
	//Call back
	iCtsyDispatcherCallback.CallbackCallControlAnswerComp(iIOStatus, iCallId);
	}

// ---------------------------------------------------------------------------
// CATAnswer::HandleResponseError
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATAnswer::HandleResponseError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATAnswer::HandleResponseError()"));
	
	//Call back
	iCtsyDispatcherCallback.CallbackCallControlAnswerComp(iATResult, iCallId);
	}

// ---------------------------------------------------------------------------
// CATAnswer::HandleConnectedComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATAnswer::HandleConnectedComplete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATAnswer::HandleConnectedComplete()"));

	//Setting call states
	iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
			                                                             iCallId, 
			                                                             RMobileCall::EStatusAnswering);	
		
	iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
			                                                             iCallId, 
			                                                             RMobileCall::EStatusConnected);
			                                                             
    //Call back
	iCtsyDispatcherCallback.CallbackCallControlAnswerComp(KErrNone, iCallId);
				                                                             						
	//Change Call State to active
	TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(iCallId));
	if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed)
		{
		tCallInfo.SetCallState(TLtsyCallInformation::EActiveCall);
		}
	
	//If exist other call , should set hold
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		if (n != iCallId)
			{
			TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(n));
			if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed &&
				tCallInfo.GetCallState() != TLtsyCallInformation::EHeldCall )
				{
				tCallInfo.SetCallState(TLtsyCallInformation::EHeldCall);
				
				//Notify CTSY call state has changed
				iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
						                                                             n, 
						                                                 RMobileCall::EStatusHold);				
				}
			}
		}
	}

// ---------------------------------------------------------------------------
// CATAnswer::Complete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATAnswer::Complete()
	{
	//Invoke base class complete
	CAtCommandBase::Complete();
	
	//Let other command can use I/O port
	iPhoneGlobals.iEventSignalActive = EFalse;	
	}

// ---------------------------------------------------------------------------
// CATAnswer::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATAnswer::EventSignal(TAtEventSource /*aEventSource*/, TInt aStatus)
	{  
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATAnswer::EventSignal()"));
	LOGTEXT2(_L8("[Ltsy CallControl] aStatus = %d"),aStatus);
	
	iIOStatus = aStatus;
	
	//I/O error
	if (iIOStatus != KErrNone)
		{
		HandleIOError();
		Complete();
		return;
		}
	
	switch (iAnswerStep)
		{
		case EATWaitForWriteComplete:
			iAnswerStep = EATReadComplete;
			break;
			
		case EATReadComplete:
			if (iATResult == KErrNone)
				{
				HandleConnectedComplete();
				}
			else
				{
				HandleResponseError();
				}
			Complete();
			break;
			
		default:
			break;
		}
	}

//End of file
