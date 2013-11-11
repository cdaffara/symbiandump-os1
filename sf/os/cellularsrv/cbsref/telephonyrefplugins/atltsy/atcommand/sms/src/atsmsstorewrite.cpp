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
// @file atsmsstorewrite.cpp
// This contains CAtSmsStoreWrite which is used to write sms message
// 

//system include 
#include <ctsy/ltsy/cctsydispatchercallback.h>

//user include
#include "atsmsstorewrite.h"
#include "mslogger.h"
#include "atmanager.h"
#include "commengine.h"
#include "smsatutil.h"

//constant defination
_LIT8(KCMGWResponseString,"+CMGW:");
_LIT8(KSmsWriteLengthCommand,"AT+CMGW = %d\r");
_LIT8(KDefaultSCA,"00");
_LIT8(KSmsEnterPduResponse,">");
const TInt KLtsyOnePause = 100000;   //0.1s

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::CAtSmsStoreWrite
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreWrite::CAtSmsStoreWrite(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                   :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtSmsStoreWrite::CAtSmsStoreWrite called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::~CAtSmsStoreWrite
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreWrite::~CAtSmsStoreWrite()
	{
	LOGTEXT(_L8("CAtSmsStoreWrite::~CAtSmsStoreWrite called"));
	delete iCallbackTimer;
	iCallbackTimer = NULL;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreWrite* CAtSmsStoreWrite::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                          CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsStoreWrite* self = new (ELeave)CAtSmsStoreWrite(aGloblePhone, 
                                                          aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreWrite* CAtSmsStoreWrite::NewL(CGlobalPhonemanager& aGloblePhone, 
                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsStoreWrite* self=CAtSmsStoreWrite::NewLC(aGloblePhone, 
                                                   aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreWrite::ConstructL()
	{
	CAtCommandBase::ConstructL();
	//Create Timer
	iCallbackTimer = CCallbackTimer::NewL(*this);
	//set writing timeout
	iWriteTimeOut = KLtsyDefaultWriteTimeOut;
	AddExpectStringL(KSmsEnterPduResponse);
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::SeGsmSmsEntry
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreWrite::SeGsmSmsEntry(RMobileSmsStore::TMobileGsmSmsEntryV1 aMobileGsmEntry)
	{
	iMobileGsmEntry = aMobileGsmEntry;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreWrite::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreWrite::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtSmsStoreWrite::ExecuteCommand callded"));
	WriteMessage();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::WriteMessage
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreWrite::WriteMessage()
	{
	LOGTEXT(_L8("CAtSmsStoreWrite::WriteMessage callded"));
	iTxBuffer.Zero();
	
	RMobilePhone::TMobileAddress  msgSca = iMobileGsmEntry.iServiceCentre;
	if( msgSca.iTelNumber.Length()!=0 )
		{
		//Here is a SCA
		TInt ret = SmsAtUtil::AppendAddressToAscii(iMsgDataAscii,msgSca);
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
	SmsAtUtil::AppendDataToAscii(iMsgDataAscii,iMobileGsmEntry.iMsgData);
	iState = ESetPDULengthComplete;
	StartWritingPduLength();	
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::StartWritingPduLength
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreWrite::StartWritingPduLength()
	{
	LOGTEXT(_L8("CAtSmsStoreWrite::StartWritingPduLength called"));
	TInt pduLen = iMobileGsmEntry.iMsgData.Length();
	iTxBuffer.Zero();
	iTxBuffer.Format(KSmsWriteLengthCommand,pduLen);
	ClearBuffer();
	iPhoneGlobals.iAtManager->SetSolicitedAtCommand(this);
	iPhoneGlobals.iCommEngine->CommWrite(iTxBuffer);	
	StartTimer(iWriteTimeOut);
	}


// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::StartWritingPdu
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreWrite::StartWritingPdu()
	{
	LOGTEXT(_L8("CAtSmsStoreWrite::StartWritingPdu called"));
	iTxBuffer.Zero();
	iTxBuffer.Append(iMsgDataAscii);
	iTxBuffer.Append(KLtsyCtrlZChar);
	ClearBuffer();
	iPhoneGlobals.iAtManager->SetSolicitedAtCommand(this);
	iPhoneGlobals.iCommEngine->CommWrite(iTxBuffer);	
	StartTimer(iWriteTimeOut);
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreWrite::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtSmsStoreWrite::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
		
	if(aStatus == KErrNone)
		{
		if((aEventSource == EWriteCompletion))
			{
			LOGTEXT(_L8("CAtSmsStoreWrite::EventSignal,EWriteCompletion!"));
			return;
			}
		else
			{
			if( iState == ESetPDULengthComplete )
				{
	            if(iError==KErrNone)
	            	{
			    	iState = EWritePDUComplete;
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
	iCtsyDispatcherCallback.CallbackPhonebookSmsStoreWriteEntryComp(aStatus,iLocation,EFalse);
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreWrite::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
    LOGTEXT(_L8("CAtSmsStoreWrite::ParseResponseL called!"));
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
		TInt pos = responseBuf.FindF(KCMGWResponseString);
		if (pos == KErrNotFound)
			{
			LOGTEXT(_L8("CAtSmsStoreWrite::ParseResponseL\tError - Cannot find '+CMGW:' string"));
			iError = KErrNotFound;
			return;
			}
		pos += KCMGWResponseString().Length();
		//skip a "space" start
		while(!(TChar(responseBuf[pos]).IsDigit()))
			{
			++pos;
			}
		// Place the message reference number into buffer 
		//(ie: everything after +CMGW: string)
		TInt refPos = responseBuf.Length()-pos;
		responseBuf.Set(responseBuf.Right(refPos));
		
		
		TLex8 lex(responseBuf);
		TUint16 val;
		TInt ret = lex.Val(val,EDecimal);
		if(ret != KErrNone)
			{
			iError = ret;
			return;
			}
		iLocation = val;
		LOGTEXT2(_L8("CAtSmsStoreWrite::ParseResponseL write index =%d"),iLocation);
    	}
	}


// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::BeginTimer
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreWrite::BeginTimer()
	{
	LOGTEXT(_L8("CAtSmsStoreWrite::BeginTimer() called"));	
	
    if (iCallbackTimer->IsActive())
    	{
    	iCallbackTimer->Cancel();
    	}
	iCallbackTimer->After(KLtsyOnePause);	
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreWrite::TimerRun
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreWrite::TimerRun(TInt aError)
	{
	LOGTEXT(_L8("CAtSmsStoreWrite::TimerRun() called"));
	if(aError == KErrNone)
		{
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhonebookSmsStoreWriteEntryComp(
				                iCallbackVal,iLocation,EFalse);
		
		}
	}

//End of file
