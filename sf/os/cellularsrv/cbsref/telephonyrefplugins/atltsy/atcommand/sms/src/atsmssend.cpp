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
// @file atsmssend.cpp
// This contains CAtSmsSend which is used to send sms message
// 

//system include
#include <etelmm.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>

// user include
#include "atsmssend.h"
#include "mslogger.h"
#include "ltsycommondefine.h"
#include "atmanager.h"
#include "commengine.h"
#include "smsatutil.h"

//Constants
_LIT8(KSmsSendCommandPdu,"AT+CMGS=%d\r");
_LIT8(KCMGSResponseString,"+CMGS:");
_LIT8(KDefaultSCA,"00");
_LIT8(KSmsEnterPduResponse,">");
_LIT8(KSendExpectedString,"+CMGS:*");
const TInt KLtsyOnePause = 100000;
const TInt KLtsySendSmsWriteTimeOut = 20;
const TInt KLtsySendSmsReadTimeOut = 30;

// ---------------------------------------------------------------------------
// CAtSmsSend::CAtSmsSend
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsSend::CAtSmsSend(CGlobalPhonemanager& aGloblePhone, 
                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                       :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)                  
 	{
	LOGTEXT(_L8("CAtSmsSend::CAtSmsSend called"));
	iWriteTimeOut = KLtsySendSmsWriteTimeOut;
	iReadTimeOut = KLtsySendSmsReadTimeOut;
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::~CAtSmsSend
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsSend::~CAtSmsSend()
	{
	LOGTEXT(_L8("CAtSmsSend::~CAtSmsSend called"));
	delete iCallbackTimer;
	iCallbackTimer = NULL;
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsSend* CAtSmsSend::NewLC(CGlobalPhonemanager& aGloblePhone, 
                              CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsSend* self = new (ELeave)CAtSmsSend(aGloblePhone,
			                                  aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsSend* CAtSmsSend::NewL(CGlobalPhonemanager& aGloblePhone, 
                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsSend* self=CAtSmsSend::NewLC(aGloblePhone,aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsSend::ConstructL()
	{
	CAtCommandBase::ConstructL();
	//Create Timer
	iCallbackTimer = CCallbackTimer::NewL(*this);
	//set writing timeout
	iWriteTimeOut = KLtsyDefaultWriteTimeOut;
	AddExpectStringL(KSmsEnterPduResponse);
	AddExpectStringL(KSendExpectedString);
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsSend::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsSend::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtSmsSend::ExecuteCommand called"));
	LOGTEXT(_L8("SendPDUMessage..."));
	
	iMsgDataAscii.Zero();
	TInt ret = ConfirmSca();
	if(ret == KErrNone)
		{
		//Here is a SCA
		ret = SmsAtUtil::AppendAddressToAscii(iMsgDataAscii,iSmsSendParam.iGsmServiceCentre);
		if( ret!=KErrNone )
			{
			BeginTimer();
			iCallbackVal = ret;
			return;
			}
		}
	else 
		{
		//Here is no SCA,usign default SCA
		iMsgDataAscii.Append(KDefaultSCA);
		}
	const TInt msgDataAsciiLen(iMsgDataAscii.Length());
	// Convert PDU to ASCII
	SmsAtUtil::AppendDataToAscii(iMsgDataAscii,iSmsSendParam.iSmsTpdu);
	iState = ESetPDULengthComplete;
	StartWritingPduLength();
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::StartWritingPduLength
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsSend::StartWritingPduLength()
	{
	LOGTEXT(_L8("CAtSmsSend::StartWritingPduLength called"));
	TInt pduLen = iSmsSendParam.iSmsTpdu.Length();
	iTxBuffer.Zero();
	iTxBuffer.Format(KSmsSendCommandPdu,pduLen);
	ClearBuffer();
	iPhoneGlobals.iAtManager->SetSolicitedAtCommand(this);
	iPhoneGlobals.iCommEngine->CommWrite(iTxBuffer);	
	StartTimer(iWriteTimeOut);
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::StartWritingPdu
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsSend::StartWritingPdu()
	{
	LOGTEXT(_L8("CAtSmsSend::StartWritingPdu called"));
	iTxBuffer.Zero();
	iTxBuffer.Append(iMsgDataAscii);
	iTxBuffer.Append(KLtsyCtrlZChar);
	ClearBuffer();
	iPhoneGlobals.iAtManager->SetSolicitedAtCommand(this);
	iPhoneGlobals.iCommEngine->CommWrite(iTxBuffer);	
	StartTimer(iWriteTimeOut);
	}


// ---------------------------------------------------------------------------
// CAtSmsSend::SetMessageParam
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsSend::SetMessageParam(TSmsSendParam aMsgParam)
	{
	iSmsSendParam = aMsgParam;
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsSend::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtSmsSend::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
    if(aStatus == KErrNone)
		{
		if(aEventSource == EWriteCompletion)
			{
			LOGTEXT(_L8("CAtSmsSend::EventSignal,EWriteCompletion!"));
			return;
			}
		else
			{
			if( iState == ESetPDULengthComplete )
				{
	            if(iError==KErrNone)
	            	{
			    	iState = ESendPDUComplete;
			    	Complete();
			       	StartWritingPdu();
			    	return;     
	            	}
				}
			}
		aStatus = iError;
		}
    Complete();
   	iPhoneGlobals.iEventSignalActive = EFalse;
    iCtsyDispatcherCallback.CallbackSmsSendSmsMessageComp(aStatus,iSmsSendResponse.iValRef,
    		                                              iSmsSendResponse.iSubmitReport);
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsSend::ParseResponseL(const TDesC8& /*aResponseBuf*/)
    {
    LOGTEXT(_L8("CAtSmsSend::ParseResponseL called!"));
    if(iState == ESetPDULengthComplete) 
	    {
   	    if(CurrentLine().MatchF(KSmsEnterPduResponse) != KErrNotFound)
	    	{
	    	iError = KErrNone;
	    	}
	    else
	    	{
	    	iError = KErrNotFound;
	    	}
	    }
    else
		{
		iError = KErrNone;
		TPtrC8 responseBuf;
		responseBuf.Set(Buffer());
		TInt pos = responseBuf.FindF(KCMGSResponseString);
		if (pos == KErrNotFound)
			{
			LOGTEXT(_L8("CAtSmsSend::ParseCMGSResponse \"+CMGS:\" not found"));
			iError = KErrNotFound;
			return;
			}
	
		// Locate the message reference number
		// (ie. read in all digits form the first found to the end of the string)
		const TInt bufLength=responseBuf.Length();
		pos += KCMGSResponseString().Length();
		while(pos<bufLength && !(TChar(responseBuf[pos]).IsDigit()))
				++pos;
		if(pos == bufLength)
			{
			LOGTEXT(_L8("CAtSmsSend::ParseCMGSResponse cannot find any digits after \"+CMS:\" "));
			iError = KErrNotFound;
			return;
			}
	
		// Read message number and store in clients data structure
		TPtrC8 ptr = responseBuf.Mid(pos);	
		TLex8 lex(ptr);
		TUint16 val;
		TInt ret = lex.Val(val,EDecimal);
		if(ret != KErrNone)
			{
			LOGTEXT(_L8("CAtSmsSend::ParseCMGSResponse cannot read Message Reference Number"));
			iError = ret;
			return;
			}
		iSmsSendResponse.iValRef = val;
		LOGTEXT2(_L8("CAtSmsSend  Message reference number %d"),val);
	    }
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::ConfirmSca
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CAtSmsSend::ConfirmSca()
	{
	if(iSmsSendParam.iGsmServiceCentre.iTelNumber.Length() == 0)
		{
		return KErrNotFound;
		}
	if(!(iSmsSendParam.iGsmServiceCentre.iNumberPlan == RMobilePhone::EIsdnNumberPlan &&
		(iSmsSendParam.iGsmServiceCentre.iTypeOfNumber == RMobilePhone::EInternationalNumber ||
		iSmsSendParam.iGsmServiceCentre.iTypeOfNumber == RMobilePhone::EUnknownNumber)))
		{
		return KErrCorrupt;
		}
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::BeginTimer
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsSend::BeginTimer()
	{
	LOGTEXT(_L8("CAtSmsSend::BeginTimer() called"));	
	
    if (iCallbackTimer->IsActive())
    	{
    	iCallbackTimer->Cancel();
    	}
	iCallbackTimer->After(KLtsyOnePause);	
	}

// ---------------------------------------------------------------------------
// CAtSmsSend::TimerRun
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsSend::TimerRun(TInt aError)
	{
	LOGTEXT(_L8("CAtSmsSend::TimerRun() called"));
	if(aError == KErrNone)
		{
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackSmsSendSmsMessageComp(iCallbackVal,iSmsSendResponse.iValRef,
				                                              iSmsSendResponse.iSubmitReport);
		}
	}

//End of file
