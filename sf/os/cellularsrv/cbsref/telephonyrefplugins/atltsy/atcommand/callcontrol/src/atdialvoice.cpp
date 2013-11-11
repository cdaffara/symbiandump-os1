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
// @file atdialvoice.cpp
// This contains CATDialVoice which dial a voice call.
//

//user include
#include "atdialvoice.h"
#include "ltsycommondefine.h"
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "globalphonemanager.h"
#include "activecommandstore.h"
#include "atmanager.h"
#include "ltsycallinformationmanager.h"
#include "mslogger.h"

//const define 
const TInt KLtsyWaitForConnect = 60;
_LIT8(KLtsyDialVoiceCommandFormat,"ATD%S;\r");
_LIT8(KLtsyBusyString, "BUSY");
_LIT8(KLtsyNoAnswerString, "NO ANSWER");
_LIT8(KLtsyUnsolicitedCallCreated, "+WIND: 5*");
_LIT8(KLtsyUnsolicitedCallingAltert, "+WIND: 2");

// ---------------------------------------------------------------------------
// CATDialVoice::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATDialVoice* CATDialVoice::NewL(CGlobalPhonemanager& aGloblePhone,
		                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::NewL()"));
	
	CATDialVoice* self = CATDialVoice::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CATDialVoice::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CATDialVoice* CATDialVoice::NewLC(CGlobalPhonemanager& aGloblePhone,
		                          CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::NewLC()"));
	
	CATDialVoice* self = new (ELeave) CATDialVoice(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CATDialVoice::~CATDialVoice
// other items were commented in a header
// ---------------------------------------------------------------------------
CATDialVoice::~CATDialVoice()
	{
	delete iATH;
	}

// ---------------------------------------------------------------------------
// CATDialVoice::CATDialVoice
// other items were commented in a header
// ---------------------------------------------------------------------------
CATDialVoice::CATDialVoice(CGlobalPhonemanager& aGloblePhone, 
		                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
				           :CAtCommandBase(aGloblePhone, aCtsyDispatcherCallback)	
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::CATDialVoice()"));
	
	iAtType = ELtsyAT_Call_ATD;
	iResult = KErrNone;
	iOKFounded = EFalse;
	iCallId = KLtsyErrorCallId;
	iStatus = KErrNone;
	iDialStep = EATDialNotInProgress;
	iIsEmergencyCall = EFalse;
	}

// ---------------------------------------------------------------------------
// CATDialVoice::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::ConstructL()"));
	
	//Invoke base class function
	CAtCommandBase::ConstructL();
	
	//Set read and write timeout
	SetTimeOut(KLtsyDefaultWriteTimeOut, KLtsyWaitForConnect);
	
	//Add expecting string
	AddExpectStringL(KLtsyUnsolicitedCallCreated);
	AddExpectStringL(KLtsyUnsolicitedCallingAltert);
	AddExpectStringL(KLtsyBusyString);
	AddExpectStringL(KLtsyNoAnswerString);
	AddExpectStringL(KLtsyNoCarrierString);
	
	//Create Hang up call
	iATH = CATHangUp::NewL(iPhoneGlobals, iCtsyDispatcherCallback);
	
	//Add Observer
	iATH->AddAllCallReleaseObserver(this);
	}

// ---------------------------------------------------------------------------
// CATDialVoice::SetEmergnecyCallFlag
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::InitVariable()
	{
	iAtType = ELtsyAT_Call_ATD;
	iResult = KErrNone;
	iOKFounded = EFalse;
	iCallId = KLtsyErrorCallId;
	iStatus = KErrNone;
	iDialStep = EATDialNotInProgress;
	}

// ---------------------------------------------------------------------------
// CATDialVoice::ReleaseAllCallComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::ReleaseAllCallComplete(TInt /*aError*/)
	{
	//if aError equal KErrNone or other dial Emergency call
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATDialVoice::SetEmergnecyCallFlag
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::SetEmergnecyCallFlag(TBool aIsEmergencyCall)
	{
	iIsEmergencyCall = aIsEmergencyCall;
	}

// ---------------------------------------------------------------------------
// CATDialVoice::SetTelephoneNumber
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::SetTelephoneNumber(const TDesC8& aTelNum)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::SetTelephoneNumber()"));
	
	iTelNum.Copy(aTelNum.Left(iTelNum.MaxLength()));
	}

// ---------------------------------------------------------------------------
// CATDialVoice::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::ExecuteCommand()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::ExecuteCommand()"));
	LOGTEXT2(_L8("[Ltsy CallControl] Telephone number = %S"), &iTelNum);
	
	iOKFounded = EFalse;
	iTxBuffer.Format(KLtsyDialVoiceCommandFormat, &iTelNum);
	Write(); 
	iDialStep = EATWaitForWriteComplete;
	}

// ---------------------------------------------------------------------------
// CATDialVoice::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::StartRequest()
	{
	if (iIsEmergencyCall && iPhoneGlobals.GetCallInfoManager().IsHaveUsedCallId())
		{
		iATH->InitVariable();
		iATH->StartRequest();
		}
	else
		{
		ExecuteCommand();
		}
	}

// ---------------------------------------------------------------------------
// CATDialVoice::ParseUnsolicitedCommandBufL
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATDialVoice::ParseUnsolicitedCommandBufL(TUnsolicitedParams& aParams, const TDesC8& aCommandBuf)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::ParseUnsolicitedCommandBufL()"));
	
	RArray<TPtrC8> rArray;
	CleanupClosePushL(rArray);
	
	iParser->ParseRespondedBuffer(rArray, aCommandBuf);
	aParams.InitParams();
	
	TInt nCount = rArray.Count();
	if (nCount <= 1)
		{
		CleanupStack::PopAndDestroy(1);
		return KErrGeneral;
		}

	for(TInt index =1; index < nCount; index++)
		{
		if(index == 1)
			{
			TInt tVal(0);
			TLex8 tLex(rArray[index]);
			TInt nRes = tLex.Val(tVal);
			if (nRes == KErrNone)
				{
				aParams.iEvent = tVal;
				}
			else
				{
				CleanupStack::PopAndDestroy(1);
				return nRes;
				}
			}
		
		if(index == 2)
			{
			TInt tVal(0);
			TLex8 tLex(rArray[index]);
			TInt nRes = tLex.Val(tVal);
			if (nRes == KErrNone)
				{
				aParams.iIdx = tVal;
				}
			else
				{
				CleanupStack::PopAndDestroy(1);	
				return nRes;
				}
			}
		}
	
	CleanupStack::Pop(1);
	
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CATDialVoice::ProcessUnsolicitedCallCreated
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATDialVoice::ProcessUnsolicitedCallCreated(const TUnsolicitedParams& aParams)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::ProcessUnsolicitedCallCreated()"));
	
	if (aParams.iEvent !=  KLtsyUnsolicitedEvent05)
		{
		return KErrGeneral;
		}
	
	TInt nCallId = aParams.iIdx;
	if ((nCallId < KLtsyMinCallId) || (nCallId > KLtsyMaxCallId))
		{
		nCallId = iPhoneGlobals.GetCallInfoManager().FindUnUesedCallId();
		if (KErrNotFound == nCallId)
			{
			return KErrNotFound;
			}
		}
	
	iCallId = nCallId;
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CATDialVoice::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::ParseResponseL(const TDesC8& aResponseBuf)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::ParseResponseL()"));
	
	if (aResponseBuf.Match(KLtsyUnsolicitedCallCreated) == 0)
		{
		LOGTEXT2(_L8("[Ltsy CallControl] iDialStep = %d"),iDialStep);
		ASSERT(iDialStep == EATWaitForDiallingComplete);
		
		TUnsolicitedParams aParams;
		iResult = ParseUnsolicitedCommandBufL(aParams, aResponseBuf);
		if (iResult == KErrNone)
			{
			iResult = ProcessUnsolicitedCallCreated(aParams);
			}
		}
	else if (aResponseBuf.Match(KLtsyUnsolicitedCallingAltert) == 0)
		{
		LOGTEXT2(_L8("[Ltsy CallControl] iDialStep = %d"),iDialStep);
		ASSERT(iDialStep == EATWaitForConnectingComplete);
		iResult = KErrNone;
		}
	else if (aResponseBuf.Match(KLtsyOkString) == 0)
		{
		LOGTEXT2(_L8("[Ltsy CallControl] iDialStep = %d"),iDialStep);
		LOGTEXT(_L8("[Ltsy CallControl] The call was connected successfully"));
		
		// if no KLtsyUnsolicitedCallingAltert string was received before we receive "OK"
		// it aslo means the call has been connected even though such KLtsyUnsolicitedCallingAltert
		// or KLtsyUnsolicitedCallCreated was not received
		if(iDialStep == EATWaitForConnectingComplete || iDialStep == EATWaitForDiallingComplete)
		    {
		    LOGTEXT(_L8("[Ltsy CallControl] No alert string [+WIND: 2] received before we receive string [ok]"));
		    
		    if (iCallId == KLtsyErrorCallId)
		    	{
		    	iCallId = iPhoneGlobals.GetCallInfoManager().FindUnUesedCallId();
		    	}
		    
		    HandleConnectingComplete();
		    iDialStep = EATWaitForConnectedComplete;
		    }
		
		iResult = KErrNone;
		}
	else if (aResponseBuf.Match(KLtsyBusyString) == 0)
		{
		LOGTEXT(_L8("[Ltsy CallControl] Busy tone was detected"));
		//iResult = KErrEtelBusyDetected;
		iResult = KErrGsmCCUserBusy;
		}
	else if (aResponseBuf.Match(KLtsyNoAnswerString) == 0)
		{
		LOGTEXT(_L8("[Ltsy CallControl] No answer from remote party"));
		//iResult = KErrEtelNoAnswer;
		iResult = KErrGsmCCUserAlertingNoAnswer;
		}
	else if (aResponseBuf.Match(KLtsyNoCarrierString) == 0)
		{
		// that could be the problem of the client, when there were two ongoing call, but a new call is coming up. As
		// GSM only support two ongoing calls
		LOGTEXT(_L8("[Ltsy CallControl] No carrier was detected"));
		if(iCallId == KLtsyErrorCallId)
		    {
		    iResult = KErrEtelNoCarrier;
		    return;
		    }
		        
		const TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(iCallId));		
		if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed)
			{
			if (tCallInfo.GetCallState() == TLtsyCallInformation::EDialingCall)
				{
				iResult = KErrEtelNoCarrier;
				}
			else
				{
				iResult = KErrGsmCCNormalCallClearing;
				}
			}
		}
	else if (aResponseBuf.Match(KLtsyErrorString) == 0)
		{
		LOGTEXT(_L8("[Ltsy CallControl] There was an error connecting the call"));
		iResult = KErrArgument;
		}
	else
		{
		LOGTEXT(_L8("[Ltsy CallControl] An unknown problem occurred connecting the call"));
		iResult = KErrGeneral;
		}
	}

// ---------------------------------------------------------------------------
// CATDialVoice::Complete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::Complete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::Complete()"));
	LOGTEXT2(_L8("[Ltsy CallControl] iStatus = %d"), iStatus);
	LOGTEXT2(_L8("[Ltsy CallControl] iResult = %d"), iResult);
	//Remove Active Command and Stop timer
	CAtCommandBase::Complete();
		
	//Let other command can use I/O port
	iPhoneGlobals.iEventSignalActive = EFalse;
	}

// ---------------------------------------------------------------------------
// CATDialVoice::HandleIOErrorL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::HandleIOError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::HandleIOError()"));
	
	if (iDialStep == EATWaitForWriteComplete)
		{
		if (iIsEmergencyCall)
			{
			iCtsyDispatcherCallback.CallbackCallControlDialEmergencyComp(iStatus, KLtsyErrorCallId);
			}
		else
			{
			iCtsyDispatcherCallback.CallbackCallControlDialVoiceComp(iStatus, KLtsyErrorCallId);
			}
		}
	else
		{
		//Call id has not be allocated
		if (KLtsyErrorCallId == iCallId)
			{
			if (iIsEmergencyCall)
				{
				iCtsyDispatcherCallback.CallbackCallControlDialEmergencyComp(iStatus, iCallId);
				}
			else
				{
				iCtsyDispatcherCallback.CallbackCallControlDialVoiceComp(iStatus, iCallId);
				}
			}
		else
			{
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(iStatus, 
					                                                             iCallId, 
																		         RMobileCall::EStatusDisconnecting);
			
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(iStatus, 
					                                                             iCallId, 
																		         RMobileCall::EStatusIdle);
			//Setting ltsy call information
			iPhoneGlobals.GetCallInfoManager().ResetCallInformationByCallId(iCallId);
			}
		}
	}

// ---------------------------------------------------------------------------
// CATDialVoice::HandleResponseError
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::HandleResponseError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::HandleResponseError()"));
	
	//Call id has not be allocated
	if (KLtsyErrorCallId == iCallId)
		{
		if (iIsEmergencyCall)
			{
			iCtsyDispatcherCallback.CallbackCallControlDialEmergencyComp(iResult, iCallId);
			}
		else
			{
			iCtsyDispatcherCallback.CallbackCallControlDialVoiceComp(iResult, iCallId);
			}
		}
	else
		{
		iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(iResult, 
																			 iCallId, 
																			 RMobileCall::EStatusDisconnecting);
		
		iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(iResult, 
																			 iCallId, 
																			 RMobileCall::EStatusIdle);
		//Setting ltsy call information
		iPhoneGlobals.GetCallInfoManager().ResetCallInformationByCallId(iCallId);
		}
	}

// ---------------------------------------------------------------------------
// CATDialVoice::HandleDiallingCompleteL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::HandleDiallingComplete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDialVoice::HandleDiallingComplete()"));
	LOGTEXT2(_L8("[Ltsy CallControl] Call id = %d"), iCallId);
	
	if (iIsEmergencyCall)
		{
		iCtsyDispatcherCallback.CallbackCallControlDialEmergencyComp(KErrNone, iCallId);
		}
	else
		{
		iCtsyDispatcherCallback.CallbackCallControlDialVoiceComp(KErrNone, iCallId);
		}
	
	LOGTEXT(_L8("[Ltsy CallControl] Call status = RMobileCall::EStatusDialling"));
	iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
																		 iCallId, 
													 RMobileCall::EStatusDialling);
	
	//Setting ltsy call information
	TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(iCallId));
	tCallInfo.SetCallId(iCallId);
	tCallInfo.SetCallIdIsUsedInfo(TLtsyCallInformation::EUsed);
	tCallInfo.SetCallDirection(TLtsyCallInformation::EMoCall);
	tCallInfo.SetCallState(TLtsyCallInformation::EDialingCall);
	tCallInfo.SetCallMode(TLtsyCallInformation::EVoiceCall);
	tCallInfo.SetConferenceCall(TLtsyCallInformation::ENotConference);
	//Emergency call flag
	if (iIsEmergencyCall)
		{
		tCallInfo.SetEmergencyCallFlag(ETrue);
		}
	
	
	//If have another and it's state is not hold so setting hold
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		if (n != iCallId)
			{
			TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(n));
			if (tCallInfo.GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed && 
				tCallInfo.GetCallState() != TLtsyCallInformation::EHeldCall)
				{
				//Setting ltsy call state
				tCallInfo.SetCallState(TLtsyCallInformation::EHeldCall);
				
				//Notify CTSY state change
				iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
																					 n, 
																      RMobileCall::EStatusHold);				
				}
			}
		}
	}

// ---------------------------------------------------------------------------
// CATDialVoice::HandleConnectingCompleteL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::HandleConnectingComplete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Call status = RMobileCall::EStatusConnecting"));
	
	iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
																		 iCallId, 
													 RMobileCall::EStatusConnecting);	
	
	//Setting ltsy call information
	TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(iCallId));
	tCallInfo.SetCallState(TLtsyCallInformation::EAlertingCall);
	}

// ---------------------------------------------------------------------------
// CATDialVoice::HandleConnectedCompleteL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::HandleConnectedComplete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Call status = RMobileCall::EStatusConnected"));
	iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrNone, 
			                                                              iCallId, 
			                                           RMobileCall::EStatusConnected);
	
	//Setting ltsy call information
	TLtsyCallInformation& tCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(iCallId));
	tCallInfo.SetCallState(TLtsyCallInformation::EActiveCall);	
	}

// ---------------------------------------------------------------------------
// CATDialVoice::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDialVoice::EventSignal(TAtEventSource /*aEventSource*/, TInt aStatus)
	{
	LOGTEXT(_L8("[Ltsy] Starting CATDialVoice::EventSignal()"));
	
	iStatus = aStatus;
	
	if (aStatus != KErrNone)
		{
		HandleIOError();
		Complete();
		return;
		}
	
	switch (iDialStep)
		{
		case EATWaitForWriteComplete:
			iDialStep = EATWaitForDiallingComplete;
			break;
			
		case EATWaitForDiallingComplete:
			//first clean current line
			ClearCurrentLine();
			//Process result
			if (iResult == KErrNone)
				{
				HandleDiallingComplete();
				iDialStep = EATWaitForConnectingComplete;
				}
			else
				{
				HandleResponseError();
				Complete();
				}
			break;
			
		case EATWaitForConnectingComplete:
			//first clean current line
			ClearCurrentLine();
			//Process result
			if (iResult == KErrNone)
				{
				HandleConnectingComplete();
				iDialStep = EATWaitForConnectedComplete;
				}
			else
				{
				HandleResponseError();
				Complete();
				}
		    break;
		    
		case EATWaitForConnectedComplete:
			//first clean current line
			ClearCurrentLine();
			//Process result
			if (iResult == KErrNone)
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
