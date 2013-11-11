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
// @file atchld.cpp
// This contains CATChld which hold, resume, release, swap one call.
// 


//sysmtem include

//user include
#include "atchld.h"
#include "ltsycallinformationmanager.h"
#include "athangupcommandrecords.h"
#include "mslogger.h"

//const define
const TInt KLtsyCHLDReadTimeout = 3;
const TInt KLtsyCHLDWaitForModemTime = 1000*250; // 0.25 seconds
_LIT8(KLtsyCHLDCommandFormat1, "AT+CHLD=%d\r");
_LIT8(KLtsyCHLDCommandFormat2, "AT+CHLD=%d%d\r");
_LIT8(KLtsyReleaseOutgoingCommand, "ATH1\r");    //Hang up outging call


// ---------------------------------------------------------------------------
// CATChld::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATChld* CATChld::NewL(CGlobalPhonemanager& aGloblePhone,
		               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATChld* self = CATChld::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CATChld::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CATChld* CATChld::NewLC(CGlobalPhonemanager& aGloblePhone,
		               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATChld* self = new (ELeave) CATChld(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


// ---------------------------------------------------------------------------
// CATChld::~CATChld
// other items were commented in a header
// ---------------------------------------------------------------------------
CATChld::~CATChld()
	{
	}

// ---------------------------------------------------------------------------
// CATChld::CATChld
// other items were commented in a header
// ---------------------------------------------------------------------------
CATChld::CATChld(CGlobalPhonemanager& aGloblePhone,
		         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
				 : CAtCommandBase(aGloblePhone, aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::CATChld()"));
	
	//Set At type
	iAtType = ELtsyAT_Call_CHLD;
	
	//Init variable
	InitVariable();
	}

// ---------------------------------------------------------------------------
// CATChld::InitVariable
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::InitVariable()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::InitVariable()"));
	
	iIOStatus = KErrNone;
	iATResult = KErrNone;
	iATStep = EATNotInProgress;
	iIpcType = ELtsyReqCallHangup;
	iChldType = ERleaseSpecificCall;
	iCallId = KLtsyErrorCallId;
	iConnectedCallId = KLtsyErrorCallId;
	iHangupCause = -1;
	}

// ---------------------------------------------------------------------------
// CATChld::IsHoldCall
// other items were commented in a header
// ---------------------------------------------------------------------------
TBool CATChld::IsHoldCall(TInt aHoldCallId)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::IsHoldCall()"));
	if (aHoldCallId >= KLtsyMinCallId && aHoldCallId <= KLtsyMaxCallId)
		{
		const TLtsyCallInformation& tCallInfo = iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(aHoldCallId);
		if (TLtsyCallInformation::EUsed == tCallInfo.GetCallIdIsUsedInfo() &&
			TLtsyCallInformation::EHeldCall == tCallInfo.GetCallState())
			{
			return ETrue;
			}		
		}
	return EFalse;
	}

// ---------------------------------------------------------------------------
// CATChld::IsActiveCall
// other items were commented in a header
// ---------------------------------------------------------------------------
TBool CATChld::IsActiveCall(TInt aActiveCallId)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::IsActiveCall()"));
	
	if (aActiveCallId >= KLtsyMinCallId && aActiveCallId <= KLtsyMaxCallId)
		{
		const TLtsyCallInformation& tCallInfo = iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(aActiveCallId);
		if (TLtsyCallInformation::EUsed == tCallInfo.GetCallIdIsUsedInfo() &&
			TLtsyCallInformation::EActiveCall == tCallInfo.GetCallState())
			{
			return ETrue;
			}		
		}	
	
	return EFalse;
	}

// ---------------------------------------------------------------------------
// CATChld::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::ConstructL()"));
	
	//Invoke base class function
	CAtCommandBase::ConstructL();
	
	//Set read and write timeout
	SetTimeOut(KLtsyDefaultWriteTimeOut, KLtsyCHLDReadTimeout);	
	}

// ---------------------------------------------------------------------------
// CATChld::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::ExecuteCommand()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::ExecuteCommand()"));
	
	if (iChldType == EReleaseAllHeldCallOrUDUB ||
		iChldType == EReleaseAllActiveCallAndAcceptHeldWaiting ||
		iChldType == EHeldAllActiveCallAndAcceptHeldWaiting ||
		iChldType == EAddHeldCallToConversation ||
		iChldType == ECallTranster)
		{
		iTxBuffer.Format(KLtsyCHLDCommandFormat1, iChldType);
		}
	else
		{
		if (iChldType == ERleaseSpecificCall) //Release one call
			{
			
			const TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(iCallId));
			
			if (TLtsyCallInformation::EDialingCall == tCallInfo.GetCallState() ||
				TLtsyCallInformation::EAlertingCall == 	tCallInfo.GetCallState())
				{
				//Hang up dialing or Alerting call(Outgoing call)
				iTxBuffer.Copy(KLtsyReleaseOutgoingCommand);
				}
			else //Other call
				{
				iTxBuffer.Format(KLtsyCHLDCommandFormat2, EReleaseAllActiveCallAndAcceptHeldWaiting, iCallId);
				}
			}
		else
			{
			iTxBuffer.Format(KLtsyCHLDCommandFormat2, EHeldAllActiveCallAndAcceptHeldWaiting, iCallId);
			}
		}
	
	LOGTEXT2(_L8("[Ltsy CallControl] CHLD Command = %S"), &iTxBuffer);
	
	//Setting waiting for modem time, when Command is too fast, Modem response error
	if (iIpcType == ELtsyReqCallHold || 
		iIpcType == ELtsyReqCallResume ||
		iIpcType == ELtsyReqCallSwap)
		{
		User::After(KLtsyCHLDWaitForModemTime);
		}
	
	Write();
	iATStep = EATWaitForWriteComplete;
	}

// ---------------------------------------------------------------------------
// CATChld::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATChld::SetIpcType
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::SetIpcType(TIpcType aIpcType)
	{
	iIpcType = aIpcType;
	}

// ---------------------------------------------------------------------------
// CATChld::SetChldType
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::SetChldType(TChldType aChldType)
	{
	iChldType = aChldType;
	}

// ---------------------------------------------------------------------------
// CATChld::SetCallId
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::SetCallId(TInt aCallId)
	{
	iCallId = aCallId;
	}

// ---------------------------------------------------------------------------
// CATChld::SetConnectedCallId
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::SetConnectedCallId(TInt aConnectedCallId)
	{
	iConnectedCallId = aConnectedCallId;
	}

// ---------------------------------------------------------------------------
// CATChld::SetHangupCase
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::SetHangupCase(TInt aHangupCase)
	{
	iHangupCause = aHangupCase;
	}

// ---------------------------------------------------------------------------
// CATChld::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::ParseResponseL(const TDesC8& aResponseBuf)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::ParseResponseL()"));
	
	if (aResponseBuf.Match(KLtsyOkString) == 0)
		{
		iATResult = KErrNone;
		}
	else if (aResponseBuf.Match(KLtsyErrorString) == 0)
		{
		iATResult = KErrArgument;
		}
	}

// ---------------------------------------------------------------------------
// CATChld::HandleIOError
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::HandleIOError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::HandleIOError()"));
	
	//Hangup
	if (iIpcType == ELtsyReqCallHangup)
		{
		iCtsyDispatcherCallback.CallbackCallControlHangUpComp(iIOStatus, iCallId);
		}
	else if (iIpcType == ELtsyReqCallHold) //Hold call
		{
		iCtsyDispatcherCallback.CallbackCallControlHoldComp(iIOStatus, iCallId);
		}
	else if (iIpcType == ELtsyReqCallResume) //Resume call
		{
		iCtsyDispatcherCallback.CallbackCallControlResumeComp(iIOStatus, iCallId);
		}
	else if (iIpcType == ELtsyReqCallSwap) //Swap call
		{
		iCtsyDispatcherCallback.CallbackCallControlSwapComp(iIOStatus);
		}		
	}

// ---------------------------------------------------------------------------
// CATChld::HandleResponseError
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::HandleResponseError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::HandleResponseError()"));
	
	//Hangup
	if (iIpcType == ELtsyReqCallHangup)
		{
		iCtsyDispatcherCallback.CallbackCallControlHangUpComp(iATResult, iCallId);
		}
	else if (iIpcType == ELtsyReqCallHold) //Hold call
		{
		iCtsyDispatcherCallback.CallbackCallControlHoldComp(iATResult, iCallId);
		}
	else if (iIpcType == ELtsyReqCallResume) //Resume call
		{
		iCtsyDispatcherCallback.CallbackCallControlResumeComp(iATResult, iCallId);
		}
	else if (iIpcType == ELtsyReqCallSwap) //Swap call
		{
		iCtsyDispatcherCallback.CallbackCallControlSwapComp(iATResult);
		}
	}

// ---------------------------------------------------------------------------
// CATChld::HandleCHLDComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::HandleCHLDComplete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::HandleCHLDComplete()"));
	
	//Hangup
	if (iIpcType == ELtsyReqCallHangup)
		{
		HandleHangupComplete();
		}
	else if (iIpcType == ELtsyReqCallHold) //Hold call
		{
		HandleHoldComplete();
		}
	else if (iIpcType == ELtsyReqCallResume) //Resume call
		{
		HandleResumeComplete();
		}
	else if (iIpcType == ELtsyReqCallSwap) //Swap call
		{
		HandleSwapComplete();
		}
	}

// ---------------------------------------------------------------------------
// CATChld::HandleHangupComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::HandleHangupComplete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::HandleHangupComplete()"));
	
	const TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(iCallId));
	
	if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed)
		{
		if (tCallInfo.GetCallState() == TLtsyCallInformation::EInComingCall ||
			tCallInfo.GetCallState() == TLtsyCallInformation::EWaitingCall	)
			{
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrGsmCCCallRejected, 
					                                                             iCallId, 
					                                                             RMobileCall::EStatusDisconnecting);
			
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrGsmCCCallRejected, 
					                                                             iCallId, 
					                                                             RMobileCall::EStatusIdle);			
			}
		else
			{
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrGsmCCNormalCallClearing, 
					                                                             iCallId, 
					                                                             RMobileCall::EStatusDisconnecting);
			
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrGsmCCNormalCallClearing, 
					                                                             iCallId, 
					                                                             RMobileCall::EStatusIdle);			
			}
		}
	
	//Call back
	iCtsyDispatcherCallback.CallbackCallControlHangUpComp(KErrNone, iCallId);
	//Setting all call id state unused
	iPhoneGlobals.GetCallInfoManager().ResetCallInformationByCallId(iCallId);
	}


// ---------------------------------------------------------------------------
// CATChld::HandleHoldComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::HandleHoldComplete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::HandleHoldComplete()"));
	
	iCtsyDispatcherCallback.CallbackCallControlHoldComp(KErrNone, iCallId);
	iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
			                                                             iCallId, 
			                                                             RMobileCall::EStatusHold);
	
	//Notify CTSY Call id Idel
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(n));
		if (n == iCallId)
			{
			if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed)
				{
				//Setting my call state active
				tCallInfo.SetCallState(TLtsyCallInformation::EHeldCall);				
				}
			}
		else
			{
			if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed)
				{
				//Setting my call state active
				tCallInfo.SetCallState(TLtsyCallInformation::EActiveCall);
				
				//Notify CTSY call state change
				iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
																					 n,
																                     RMobileCall::EStatusConnected);
				}
			}
		}	
	}

// ---------------------------------------------------------------------------
// CATChld::HandleResumeComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::HandleResumeComplete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::HandleResumeComplete()"));
	
	iCtsyDispatcherCallback.CallbackCallControlResumeComp(KErrNone, iCallId);
	iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
			                                                             iCallId, 
			                                                             RMobileCall::EStatusConnected);

	//Notify CTSY Call id Idel
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(n));
		if (n == iCallId)
			{
			if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed)
				{
				//Setting my call state active
				tCallInfo.SetCallState(TLtsyCallInformation::EActiveCall);				
				}			
			}
		else
			{
			if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed)
				{
				//Setting my call state active
				tCallInfo.SetCallState(TLtsyCallInformation::EHeldCall);
				
				//Notify CTSY call state change
				iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
																					 n,
																                     RMobileCall::EStatusHold);
				}			
			}
		}
	}

// ---------------------------------------------------------------------------
// CATChld::HandleSwapComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::HandleSwapComplete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::HandleSwapComplete()"));
	
	iCtsyDispatcherCallback.CallbackCallControlSwapComp(KErrNone);
	
	//One call or two calls situation
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(n));
		
		if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed &&
			tCallInfo.GetCallState() == TLtsyCallInformation::EActiveCall)
			{
			tCallInfo.SetCallState(TLtsyCallInformation::EHeldCall);
			//Notify CTSY call state change
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
																				 n,
															                     RMobileCall::EStatusHold);			
			}
		else if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed &&
				((tCallInfo.GetCallState() == TLtsyCallInformation::EHeldCall) || 
				 (tCallInfo.GetCallState() == TLtsyCallInformation::EWaitingCall) ||
				 (tCallInfo.GetCallState() == TLtsyCallInformation::EInComingCall))) //????
			{
			tCallInfo.SetCallState(TLtsyCallInformation::EActiveCall);
			//Notify CTSY call state change
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
																				 n,
															                     RMobileCall::EStatusConnected);			
			}		
		}
	}

// ---------------------------------------------------------------------------
// CATChld::Complete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::Complete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::Complete()"));
	
	//Check Hang up arrray
	if ((iIpcType == ELtsyReqCallHangup) && iPhoneGlobals.GetCallInfoManager().GetHangupCommandRecords().IsHaveRecords())
		{
		InitVariable();
		
		TATHanupCommandInfo tHangupInfo;
		iPhoneGlobals.GetCallInfoManager().GetHangupCommandRecords().GetFirstHangupCommandInfoAndRemoveIt(tHangupInfo);
		
		//Set hang up info
		SetCallId(tHangupInfo.iCallId);
		SetHangupCase(tHangupInfo.iHangupCause);
		SetIpcType(CATChld::ELtsyReqCallHangup);
		SetChldType(CATChld::ERleaseSpecificCall);
		StartRequest();
		return;
		}
	
	//Invoke base class complete
	CAtCommandBase::Complete();
	
	//Set hang up active
	iPhoneGlobals.GetCallInfoManager().GetHangupCommandRecords().SetHangupActiveFlag(EFalse);
	
	//Let other command can use I/O port
	iPhoneGlobals.iEventSignalActive = EFalse;
	}

// ---------------------------------------------------------------------------
// CATChld::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATChld::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATChld::EventSignal()"));
	if(aEventSource == EReadCompletion)
	    {
	    LOGTEXT(_L8("[Ltsy CallControl] The EventSource = EReadCompletion"));
	    }
	else if(aEventSource == EWriteCompletion)
	    {
	    LOGTEXT(_L8("[Ltsy CallControl] The EventSource = EWriteCompletion"));
	    }
	else if(aEventSource == ETimeoutCompletion)
	    {
	    LOGTEXT(_L8("[Ltsy CallControl] The EventSource = ETimeoutCompletion"));
	    }
	else
	    {
	    LOGTEXT(_L8("[Ltsy CallControl] The EventSource = Unknown"));
	    }
	    
	LOGTEXT2(_L8("[Ltsy CallControl]aStatus = %d"),aStatus);

	iIOStatus = aStatus;
	
	if (iIOStatus != KErrNone)
		{
		HandleIOError();
		Complete();
		return;
		}
	
	switch (iATStep)
		{
		case EATWaitForWriteComplete:
			iATStep = EATReadComplete;
			break;
			
		case EATReadComplete:
			ClearCurrentLine();
			if (iATResult == KErrNone)
				{
				HandleCHLDComplete();
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
