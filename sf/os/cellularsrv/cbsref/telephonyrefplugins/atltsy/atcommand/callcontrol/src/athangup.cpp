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
// Hangup a ongoning call

//system include

//user include
#include "athangup.h"
#include "atmanager.h"
#include "activecommandstore.h"
#include "mslogger.h"
#include "ltsycallinformationmanager.h"
#include "allcallreleasecompleteobserver.h"

//const define
const TInt KLtsyHangupReadTimeout = 2;
_LIT8(KLtsyHangUpAllCallCommand, "ATH\r"); //ATH0


CATHangUp* CATHangUp::NewL(CGlobalPhonemanager& aGloblePhone,
			               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::NewL()"));
	
	CATHangUp* self = CATHangUp::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

CATHangUp* CATHangUp::NewLC(CGlobalPhonemanager& aGloblePhone,
			               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::NewLC()"));
	
	CATHangUp* self = new (ELeave) CATHangUp(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CATHangUp::~CATHangUp()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::~CATHangUp()"));
	}

CATHangUp::CATHangUp(CGlobalPhonemanager& aGloblePhone,
		             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
					 :CAtCommandBase(aGloblePhone, aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::CATHangUp()"));
	
	iAtType = ELtsyAT_Call_ATH;
	iIOStatus = KErrNone;
	iATResult = KErrNone;
	iHangupStep = EATHangUpNotInProgress;
	iHangupOperator = EUnknownAPI;
	iAllCallReleaseObserver = NULL;
	}

void CATHangUp::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::ConstructL()"));
	
	//Invoke base class function
	CAtCommandBase::ConstructL();
	
	//Set read and write timeout
	SetTimeOut(KLtsyDefaultWriteTimeOut, KLtsyHangupReadTimeout);
	}


void CATHangUp::ExecuteCommand()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::ExecuteCommand()"));
	
	iTxBuffer.Copy(KLtsyHangUpAllCallCommand);
	Write();
	iHangupStep = EATHangUpWaitForWriteComplete;
	}

void CATHangUp::StartRequest()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::StartRequest()"));
	
	ExecuteCommand();
	}

void CATHangUp::AddAllCallReleaseObserver(MAllCallReleaseCompleteObserver* aObserver)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::AddAllCallReleaseObserver()"));
	
	iAllCallReleaseObserver = aObserver;
	}

void CATHangUp::InitVariable()
	{
	iAtType = ELtsyAT_Call_ATH;
	iIOStatus = KErrNone;
	iATResult = KErrNone;
	iHangupStep = EATHangUpNotInProgress;
	iHangupOperator = EUnknownAPI;	
	}

void CATHangUp::SetHangupOperator(THangupOperator aOperator)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::SetHangupOperator()"));
	
	iHangupOperator = aOperator;
	}

void CATHangUp::HandleIOError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::HandleIOError()"));
	
	if (iHangupOperator == ECustomAPI)
		{
		iCtsyDispatcherCallback.CallbackCallControlTerminateAllCallsComp(iIOStatus);
		}
	else if (iHangupOperator == ERMobilePhoneAPI)
		{
		iCtsyDispatcherCallback.CallbackPhoneTerminateAllCallsComp(iIOStatus);
		}
	else
		{
		LOGTEXT(_L8("[Ltsy CallControl] Hangup unknown operator!!!!"));
		}
	}

void CATHangUp::HandleResponseError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::HandleResponseError()"));
	
	if (iHangupOperator == ECustomAPI)
		{
		iCtsyDispatcherCallback.CallbackCallControlTerminateAllCallsComp(iATResult);
		}
	else if (iHangupOperator == ERMobilePhoneAPI)
		{
		iCtsyDispatcherCallback.CallbackPhoneTerminateAllCallsComp(iATResult);
		}
	else
		{
		LOGTEXT(_L8("[Ltsy CallControl] Hangup unknown operator!!!!"));
		}
	}

void CATHangUp::HandleDisconnectedComplete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::HandleDisconnectedComplete()"));
	
	if (iHangupOperator == ECustomAPI)
		{
		iCtsyDispatcherCallback.CallbackCallControlTerminateAllCallsComp(iATResult);
		}
	else if (iHangupOperator == ERMobilePhoneAPI)
		{
		iCtsyDispatcherCallback.CallbackPhoneTerminateAllCallsComp(iATResult);
		}
	else 
		{
		LOGTEXT(_L8("[Ltsy CallControl] Hangup unknown operator!!!!"));
		if (iAllCallReleaseObserver == NULL)
			{
			return;
			}
		}

	//Notify CTSY all call idel
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(n));
		if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed)
			{
			
			if (tCallInfo.GetCallState() == TLtsyCallInformation::EInComingCall ||
				tCallInfo.GetCallState() == TLtsyCallInformation::EWaitingCall	)
				{
				iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrGsmCCCallRejected, 
						                                                             n, 
						                                                             RMobileCall::EStatusDisconnecting);			
				iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrGsmCCCallRejected, 
																					 n,
																					 RMobileCall::EStatusIdle);				
				}
			else
				{
				iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrGsmCCNormalCallClearing, 
						                                                             n, 
						                                                             RMobileCall::EStatusDisconnecting);			
				iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrGsmCCNormalCallClearing, 
																					 n,
																					 RMobileCall::EStatusIdle);				
				}
			}		
		}
	
	//Setting all call id state unused
	iPhoneGlobals.GetCallInfoManager().ResetAllCalInformation();
	}

void CATHangUp::ParseResponseL(const TDesC8& aResponseBuf)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::ParseResponseL()"));
	
	if (aResponseBuf.Match(KLtsyOkString) == 0)
		{
		LOGTEXT(_L8("[Ltsy CallControl] Hang up call reponse is OK"));
		
		iATResult = KErrNone;
		}
	else if (aResponseBuf.Match(KLtsyErrorString) == 0)
		{
		LOGTEXT(_L8("[Ltsy CallControl] Hang up call reponse is Error"));
		
		iATResult = KErrArgument;
		}
	}

void CATHangUp::Complete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::Complete()"));
	LOGTEXT2(_L8("[Ltsy CallControl] iIOStatus = %d"), iIOStatus);
	LOGTEXT2(_L8("[Ltsy CallControl] iATResult = %d"), iATResult);
	
	//Remove Ative Command and stop timer
	CAtCommandBase::Complete();
		
	//Let other command can use I/O port
	if (iAllCallReleaseObserver == NULL)
		{
		iPhoneGlobals.iEventSignalActive = EFalse;
		}
	else
		{
		if (iIOStatus != KErrNone)
			{
			iAllCallReleaseObserver->ReleaseAllCallComplete(iIOStatus);
			}
		else
			{
			iAllCallReleaseObserver->ReleaseAllCallComplete(iATResult);
			}
		}
	}

void CATHangUp::EventSignal(TAtEventSource /*aEventSource*/, TInt aStatus)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATHangUp::EventSignal()"));
	LOGTEXT2(_L8("[Ltsy CallControl] aEventSource = %d\taStatus = %d"),aStatus);
	
	iIOStatus = aStatus;
	if (iIOStatus != KErrNone)
		{
		HandleIOError();
		Complete();
		return;
		}
	
	switch (iHangupStep)
		{
		case EATHangUpWaitForWriteComplete:
			iHangupStep = EATHangUpReadComplete;
			break;
			
		case EATHangUpReadComplete:
			if (iATResult == KErrNone)
				{
				HandleDisconnectedComplete();
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
