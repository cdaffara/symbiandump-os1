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
// @file atsmsack.cpp
// This contains CAtSmsAck and CAtSmsNack which is used to send 
// positive(negative) acknowledgment
// 

//system include 
#include <etelmm.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>

//user include 
#include "atsmsack.h"
#include "mslogger.h"
#include "smsatutil.h"

//constant defination
_LIT8(KSendCNMACommand,"AT+CNMA\r\n");
_LIT8(KCNMAForNack,"AT+CNMA=2,%d\r\n");

// ---------------------------------------------------------------------------
// CAtSmsAck::CAtSmsAck
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsAck::CAtSmsAck(CGlobalPhonemanager& aGloblePhone, 
                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                     :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtSmsAck::CAtSmsAck called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsAck::~CAtSmsAck
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsAck::~CAtSmsAck()
	{
	LOGTEXT(_L8("CAtSmsAck::~CAtSmsAck called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsAck::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsAck* CAtSmsAck::NewLC(CGlobalPhonemanager& aGloblePhone, 
                            CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsAck* self = new (ELeave)CAtSmsAck(aGloblePhone, 
                                            aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsAck::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsAck* CAtSmsAck::NewL(CGlobalPhonemanager& aGloblePhone, 
                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsAck* self=CAtSmsAck::NewLC(aGloblePhone, 
                                     aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsAck::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsAck::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CAtSmsAck::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsAck::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtSmsAck::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsAck::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtSmsAck::ExecuteCommand called"));
	iTxBuffer.Zero();
	iTxBuffer.Copy( KSendCNMACommand );
	Write();
	}

// ---------------------------------------------------------------------------
// CAtSmsAck::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsAck::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtSmsAck::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	if(aStatus == KErrNone)
		{
		if((aEventSource == EWriteCompletion))
			{
			LOGTEXT(_L8("CAtSmsNack::EventSignal,EWriteCompletion!"));
			return;
			}
		aStatus = iError;
		}
	Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCtsyDispatcherCallback.CallbackSmsAckSmsStoredComp(aStatus);
	}

// ---------------------------------------------------------------------------
// CAtSmsAck::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsAck::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) == 0)
		{
		iError = KErrNone;
		}
	else
		{
		iError = KErrGeneral;
		}
	}

// ---------------------------------------------------------------------------
// CAtSmsNack::CAtSmsNack
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsNack::CAtSmsNack(CGlobalPhonemanager& aGloblePhone, 
                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                       :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtSmsNack::CAtSmsNack called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsNack::~CAtSmsNack
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsNack::~CAtSmsNack()
	{
	LOGTEXT(_L8("CAtSmsNack::~CAtSmsNack called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsNack::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsNack* CAtSmsNack::NewLC(CGlobalPhonemanager& aGloblePhone, 
                              CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsNack* self = new (ELeave)CAtSmsNack(aGloblePhone, 
                                              aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsNack::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsNack* CAtSmsNack::NewL(CGlobalPhonemanager& aGloblePhone, 
                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsNack* self=CAtSmsNack::NewLC(aGloblePhone, 
                                       aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsNack::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsNack::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CAtSmsNack::SetNackTpdu
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsNack::SetNackTpdu(TPtrC8 aRpError)
	{
	iRpError = aRpError;
	}

// ---------------------------------------------------------------------------
// CAtSmsNack::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsNack::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtSmsNack::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsNack::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtSmsNack::ExecuteCommand called"));
	iTxBuffer.Zero();
	TInt pduLen = iRpError.Length();
	TBuf8<KLtsyGenericBufferSize> nackAsciiData;
	nackAsciiData.Zero();
	SmsAtUtil::AppendDataToAscii(nackAsciiData,iRpError);
	iTxBuffer.Format(KCNMAForNack,pduLen);
	iTxBuffer.Append(nackAsciiData);
	iTxBuffer.Append(KLtsyCtrlZChar);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtSmsNack::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsNack::EventSignal(TAtEventSource aEventSource, TInt aStatus)
/**
 * Handle the events from the comm port
 *ValidateExpectString
 * @param aSource denotes if event is due to read, write or timeout
 */
	{
	LOGTEXT3(_L8("CAtSmsNack::EventSignal iStatus=%D iSource=%D"),aStatus,aEventSource);
	if(aStatus == KErrNone)
		{
		if((aEventSource == EWriteCompletion))
			{
			LOGTEXT(_L8("CAtSmsNack::EventSignal,EWriteCompletion!"));
			return;
			}
		aStatus = iError;
		}
	Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCtsyDispatcherCallback.CallbackSmsAckSmsStoredComp(aStatus);
	}

// ---------------------------------------------------------------------------
// CAtSmsNack::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsNack::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) == 0)
		{
		iError = KErrNone;
		}
	else
		{
		iError = KErrGeneral;
		}
	}

//End of file
