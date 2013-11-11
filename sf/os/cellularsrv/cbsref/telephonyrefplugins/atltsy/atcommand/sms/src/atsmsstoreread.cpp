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
// @file atsmsstoreread.cpp
// This contains CAtSmsStoreRead which is used to Read PDU Message
// via aIndex 
// 


//system include 
#include <ctsy/ltsy/cctsydispatchercallback.h>

//user include 
#include "atsmsstoreread.h"
#include "mslogger.h"
#include "smsatutil.h"

//constant defination
_LIT8(KSmsReadCommand,"AT+CMGR=%d\r");
_LIT8(KCMGRResponseString,"+CMGR:");

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::CAtSmsStoreRead
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreRead::CAtSmsStoreRead(CGlobalPhonemanager& aGloblePhone, 
                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                 :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtSmsStoreRead::CAtSmsStoreRead called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::~CAtSmsStoreRead
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreRead::~CAtSmsStoreRead()
	{
	LOGTEXT(_L8("CAtSmsStoreRead::~CAtSmsStoreRead called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreRead* CAtSmsStoreRead::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                        CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsStoreRead* self = new (ELeave)CAtSmsStoreRead(aGloblePhone, 
                                                        aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreRead* CAtSmsStoreRead::NewL(CGlobalPhonemanager& aGloblePhone, 
                                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsStoreRead* self=CAtSmsStoreRead::NewLC(aGloblePhone, 
                                                 aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreRead::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreRead::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreRead::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtSmsStoreRead::ExecuteCommand callded"));
	ReadMessage();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreRead::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtSmsStoreRead::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
		
	if(aStatus == KErrNone)
		{
		if((aEventSource == EWriteCompletion))
			{
			LOGTEXT(_L8("CAtSmsStoreRead::EventSignal,EWriteCompletion!"));
			return;
			}
		aStatus = iError;
		}
	Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCtsyDispatcherCallback.CallbackPhonebookSmsStoreReadEntryComp(aStatus,iSmsData);
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::SetMsgIndex
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreRead::SetMsgIndex(TInt aIndex)
	{
	LOGTEXT(_L8("CAtSmsStoreRead::SetReadMessageParam callded"));
	iIndex = aIndex;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::ReadMessage
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreRead::ReadMessage()
	{
	LOGTEXT(_L8("CAtSmsStoreRead::ReadMessage callded"));
	iTxBuffer.Zero();
	iTxBuffer.Format(KSmsReadCommand,iIndex);
	Write();   
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreRead::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	/**
	* Response:+CMGR:<stat>,[<alpha>],<length><CR><LF><pdu>
	*/
	iError = KErrNone;	
	RArray<TPtrC8> array;
	CleanupClosePushL(array);
	iParser->ParseRespondedBuffer(array,Buffer());
	if(array[0].MatchF(KAnyATCommand)!=KErrNotFound)
		{
		array.Remove(0);
		}
	TInt count = array.Count();
#ifdef _DEBUG
	for( TInt i=0;i<count;i++ )
		{
        LOGTEXT3(_L8("CAtSmsStoreRead::ParseResponseL\tarray[%d]=%S"),i,&array[i]);  
		}
#endif   
	if (count <= 1)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return;
		}
	if(array[0].MatchF(KCMGRResponseString)==KErrNotFound)
		{
		LOGTEXT(_L8("CAtSmsStoreRead::ParseResponseL\tFound +CMGR String!"));
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return;
		}
	// parse the message status
	TLex8 lex(array[1]);
	TUint16 val;
	TInt ret=lex.Val(val,EDecimal);
	if(ret!=KErrNone)
		{
		CleanupStack::PopAndDestroy();
		iError = ret;
		return;
		}
	iSmsData.iMessageStatus = val;
	
	//parse the message length
	TLex8 lex1(array[3]);
	TUint msgLength;
	ret=lex1.Val(msgLength,EDecimal);
	if(ret!=KErrNone)
		{
		CleanupStack::PopAndDestroy();
		iError = ret;
		return;
		}
	LOGTEXT2(_L8("CAtSmsStoreRead::ParseResponseL\tMessage Length=%d"),msgLength);
	TBuf8<KMaxAsciiPduLength>  pdu;	   //Binary Message pdu
	SmsAtUtil::ConvertAsciiToBinary(array[4],pdu);
	TPtrC8 pduPtr(pdu);
	
	RMobilePhone::TMobileAddress gsmServiceCentre;   //messages's SCA
	if (msgLength*2 < array[4].Length())
		{
		// There is an SCA prepended so remove this and store it in iGsmServiceCentre
		SmsAtUtil::ReadAndRemoveAddressFromPdu(pduPtr,gsmServiceCentre);
		}
	iSmsData.iSmsMsg = pduPtr;
	iSmsData.iServiceCentre = gsmServiceCentre.iTelNumber;
	iSmsData.iMobileScNPI = gsmServiceCentre.iNumberPlan;
	iSmsData.iMobileScTON = gsmServiceCentre.iTypeOfNumber;
	LOGTEXT2(_L8("CAtSmsStoreRead::ParseResponseL\tMessage Status=%d"),
			                       iSmsData.iMessageStatus);
	LOGTEXT3(_L8("CAtSmsStoreRead::ParseResponseL\tSmsMsg=%S, ServiceCentre=%S"), 
			                       &iSmsData.iSmsMsg, &iSmsData.iServiceCentre);
	CleanupStack::PopAndDestroy(&array);
	}

//End of file
