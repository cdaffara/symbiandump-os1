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
// @file atdtmfvts.h
// This contains CATDtmfVts which dial a voice call.
// 

//system include

//user include
#include "atdtmfvts.h"
#include "mslogger.h"
#include "ltsycommondefine.h"
#include "ltsycallinformationmanager.h"

//const define
_LIT8(KLtsyVTSFirstCharFormat, "AT+VTS=%c");
_LIT8(KLtsyVTSMoreCharFormat, ";+VTS=%c");
_LIT8(KLtsyCarriageReturn,"\r");
const TInt KLtsyVTSReadTimeout = 2;
const TInt KLtsyStopAndCancelTimeOut = 100000; // 0.1 second


CATDtmfVts* CATDtmfVts::NewL(CGlobalPhonemanager& aGloblePhone,
			                 CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::NewL()"));
	
	CATDtmfVts* self = CATDtmfVts::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

CATDtmfVts* CATDtmfVts::NewLC(CGlobalPhonemanager& aGloblePhone,
			                  CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::NewLC()"));
	
	CATDtmfVts* self = new (ELeave) CATDtmfVts(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CATDtmfVts::~CATDtmfVts()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::~CATDtmfVts()"));
	}

CATDtmfVts::CATDtmfVts(CGlobalPhonemanager& aGloblePhone,
		               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
					   :CAtCommandBase(aGloblePhone, aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::CATDtmfVts()"));
	
	InitVariable();
	}

void CATDtmfVts::InitVariable()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::InitVariable()"));
	
	iDtmfWorkType = EDtmfUnknow;
	iOKFounded = EFalse;
	iAnswerStep = EATNotInProgress;
	iCallId = KLtsyErrorCallId;
	iIOStatus = KErrNone;
	iATResult = KErrNone;	
	}

void CATDtmfVts::SetDtmfWorkType(TDtmfWorkType aDtmfWorkType)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::SetDtmfWorkType()"));
	
	iDtmfWorkType = aDtmfWorkType;
	}

void CATDtmfVts::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::ConstructL()"));
	
	//Invoke base class function
	CAtCommandBase::ConstructL();
	
	//Create Timer
	iCallbackTimer = CCallbackTimer::NewL(*this);
	
	//Set read and write timeout
	SetTimeOut(KLtsyDefaultWriteTimeOut, KLtsyVTSReadTimeout);
	}

void CATDtmfVts::ExecuteCommand()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::ExecuteCommand()"));
	
	if (iDtmfWorkType == EDtmfStopOneTone ||
	    iDtmfWorkType == EDtmfCancelMoreTones)
		{
		StartTimer();
		}
	else
		{
		iOKFounded = EFalse;
		if (iTxBuffer.Length() > 0)
			{
			LOGTEXT2(_L8("[Ltsy CallControl] VTS = %S"), &iTxBuffer);
			Write();
			iAnswerStep = EATWaitForWriteComplete;
			}		
		}
	}

TBool CATDtmfVts::IsHaveActiveCall(TInt aActiveCallId)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::IsHaveActiveCall()"));
	
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

void CATDtmfVts::StartRequest()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::StartRequest()"));
	
	ExecuteCommand();
	}

TInt CATDtmfVts::SetCallId(TInt aCallId)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::SetCallId()"));
	
	if (!IsHaveActiveCall(aCallId))
		{
		return KErrEtelCallNotActive;
		}
	
	iCallId = aCallId;
	
	return KErrNone;
	}

TInt CATDtmfVts::SetDtmfString(const TDesC& aDtmfString)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::SetDtmfString()"));
	
	if (!StringIsDtmf(aDtmfString))
		{
		return KErrArgument;
		}
	
	TInt nLen = aDtmfString.Length();
	for (TInt n = 0; n < nLen; n++)
		{
		if (n == 0)
			{
			iTxBuffer.Format(KLtsyVTSFirstCharFormat, (TUint8)(aDtmfString[n]));
			}
		else
			{
			TBuf8<16> buf;
			buf.Format(KLtsyVTSMoreCharFormat, (TUint8)(aDtmfString[n]));
			
			if ((buf.Length() + iTxBuffer.Length()) >= KLtsyGenericBufferSize)
				{
				return KErrOverflow;
				}
			iTxBuffer.Append(buf);
			}
		}
	
	//Converts the content of this descriptor to upper case.
	iTxBuffer.UpperCase();
	iTxBuffer.Append(KLtsyCarriageReturn);
	
	return KErrNone;
	}

TBool CATDtmfVts::CharIsDtmf(const TChar& aDtmfChar)
	{
	LOGTEXT(_L8("[Ltsy CallControl]  Starting CATDtmfVts::IsDtmf()"));
	
	TUint uC = aDtmfChar.GetUpperCase();
	
	switch(uC)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case '*':
		case '#':
			return ETrue;
		}
	return EFalse;
	}

TBool CATDtmfVts::StringIsDtmf(const TDesC& aDtmfString)
	{
	LOGTEXT(_L8("[Ltsy CallControl]  Starting CATDtmfVts::StringIsDtmf()"));
	
	TInt nLen = aDtmfString.Length();
	if (nLen == 0) 
		{
		return EFalse;
		}
	
	for (TInt n = 0; n < nLen; n++)
		{
		if (!CharIsDtmf(aDtmfString[n]))
			{
			return EFalse;
			}
		}
	
	return ETrue;
	}

void CATDtmfVts::Complete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::Complete()"));
	LOGTEXT2(_L8("[Ltsy CallControl] iIOStatus = %d"), iIOStatus);
	LOGTEXT2(_L8("[Ltsy CallControl] iATResult = %d"), iATResult);
	
	//Remove Ative Command and stop timer
	CAtCommandBase::Complete();
	
	//Init
	InitVariable();
	
	//Let other command can use I/O port
	iPhoneGlobals.iEventSignalActive = EFalse;	
	}

void CATDtmfVts::StartTimer()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::StartTimer()"));	
	
    if (iCallbackTimer->IsActive())
    	{
    	iCallbackTimer->Cancel();
    	}
	iCallbackTimer->After(KLtsyStopAndCancelTimeOut);	
	}

void CATDtmfVts::TimerRun(TInt aError)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::TimerRun()"));
	
	if (aError != KErrNone)
		{
		User::After(KLtsyStopAndCancelTimeOut);
		}
	
	if (iDtmfWorkType == EDtmfStopOneTone)
		{
		iCtsyDispatcherCallback.CallbackCallControlStopDtmfToneComp(KErrNone);
		}
	else if (iDtmfWorkType == EDtmfCancelMoreTones)
		{
		iCtsyDispatcherCallback.CallbackCallControlSendDtmfTonesCancelComp(KErrNone);
		}
	
	//Init
	InitVariable();
	
	//Let other command can use I/O port
	iPhoneGlobals.iEventSignalActive = EFalse;	
	}

void CATDtmfVts::HandleIOError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::HandleIOError()"));
	
	if (iDtmfWorkType == EDtmfSendOneTone)
		{
		iCtsyDispatcherCallback.CallbackCallControlStartDtmfToneComp(iIOStatus);
		}
	else if (iDtmfWorkType == EDtmfSendMoreTones)
		{
		iCtsyDispatcherCallback.CallbackCallControlSendDtmfTonesComp(iIOStatus);
		}		
	}

void CATDtmfVts::HandleResponseError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::HandleResponseError()"));
	
	if (iDtmfWorkType == EDtmfSendOneTone)
		{
		iCtsyDispatcherCallback.CallbackCallControlStartDtmfToneComp(iATResult);
		}
	else if (iDtmfWorkType == EDtmfSendMoreTones)
		{
		iCtsyDispatcherCallback.CallbackCallControlSendDtmfTonesComp(iATResult);
		}			
	}

void CATDtmfVts::HandleSendDtmfTonesSuccess()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::HandleSendDtmfTonesSuccess()"));
	
	if (iDtmfWorkType == EDtmfSendOneTone)
		{
		iCtsyDispatcherCallback.CallbackCallControlStartDtmfToneComp(KErrNone);
		}
	else if (iDtmfWorkType == EDtmfSendMoreTones)
		{
		iCtsyDispatcherCallback.CallbackCallControlSendDtmfTonesComp(KErrNone);
		}
	}

void CATDtmfVts::ParseResponseL(const TDesC8& aResponseBuf)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::ParseResponseL()"));
	
	if (aResponseBuf.Match(KLtsyOkString) == 0)
		{
		iATResult = KErrNone;
		iOKFounded = ETrue;
		}
	else if(aResponseBuf.Match(KLtsyErrorString) == 0)
		{
		iATResult = KErrArgument;
		}
	}

void CATDtmfVts::EventSignal(TAtEventSource /*aEventSource*/, TInt aStatus)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATDtmfVts::EventSignal()"));
	LOGTEXT2(_L8("[Ltsy CallControl] aEventSource = %d\taStatus = %d"),aStatus);
	
	iIOStatus = aStatus;
	
	//Process I/O Error
	if (iIOStatus != KErrNone)
		{
		HandleResponseError();
		Complete();
		return;
		}
	
	//Process at step
	switch(iAnswerStep)
		{
		case EATWaitForWriteComplete:
			iAnswerStep = EATReadComplete;
			break;
			
		case EATReadComplete:
			ClearCurrentLine();
			if (iATResult == KErrNone)
				{
				HandleSendDtmfTonesSuccess();
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
