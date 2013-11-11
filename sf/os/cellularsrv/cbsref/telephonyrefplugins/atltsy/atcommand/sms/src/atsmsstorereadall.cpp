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
// @file atsmsstorereadall.cpp
// This contains CAtSmsStoreReadAll which is used to read all messages
// 

//system include 
#include <ctsy/ltsy/cctsydispatchercallback.h>

//user include 
#include "mslogger.h"
#include "atsmsstorereadall.h"
#include "smsatutil.h"

//constant defination
_LIT8(KSmsReadCommand,"AT+CMGR=%d\r");
_LIT8(KCMGRResponseString,"+CMGR:");

// ---------------------------------------------------------------------------
// CAtSmsStoreReadAll::CAtSmsStoreReadAll
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreReadAll::CAtSmsStoreReadAll(CGlobalPhonemanager& aGloblePhone, 
                                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                       :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtSmsStoreReadAll::CAtSmsStoreReadAll called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreReadAll::~CAtSmsStoreReadAll
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreReadAll::~CAtSmsStoreReadAll()
	{
	LOGTEXT(_L8("CAtSmsStoreReadAll::~CAtSmsStoreReadAll called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreReadAll::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreReadAll* CAtSmsStoreReadAll::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                                CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsStoreReadAll* self = new (ELeave)CAtSmsStoreReadAll( aGloblePhone,aCtsyDispatcherCallback );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreReadAll::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreReadAll* CAtSmsStoreReadAll::NewL(CGlobalPhonemanager& aGloblePhone, 
                                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsStoreReadAll* self=CAtSmsStoreReadAll::NewLC( aGloblePhone,aCtsyDispatcherCallback );
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreReadAll::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreReadAll::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreRead::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreReadAll::StartRequest()
	{
	iIndex = 1;
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreReadAll::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreReadAll::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtSmsStoreReadAll::ExecuteCommand callded"));
	ReadMessage();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreReadAll::ReadMessage
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreReadAll::ReadMessage()
	{
	LOGTEXT(_L8("CAtSmsStoreReadAll::ReadMessage callded"));
	iTxBuffer.Zero();
	iTxBuffer.Format(KSmsReadCommand,iIndex);
	Write();   
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreReadAll::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreReadAll::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtSmsStoreReadAll::EventSignal aStatus=%D iSource=%D"),
			                          aStatus,aEventSource);
	if(aStatus == KErrNone)
		{
		if((aEventSource == EWriteCompletion))
			{
			LOGTEXT(_L8("CAtSmsStoreRead::EventSignal,EWriteCompletion!"));
			return;
			}
		aStatus = iError;
		}		
		
	if(iIndex < iPhoneGlobals.iStoreTotal)
		{
		iCtsyDispatcherCallback.CallbackPhonebookSmsStoreReadAllComp(aStatus,iSmsData,iIndex,ETrue,EFalse);
		iIndex++;
		ReadMessage();
		}
	else 
		{
		//Handle last message 
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhonebookSmsStoreReadAllComp(aStatus,iSmsData,iIndex,EFalse,EFalse);
		}
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreReadAll::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreReadAll::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
        LOGTEXT3(_L8("CAtSmsStoreReadAll::ParseResponseL\tarray[%d]=%S"),i,&array[i]);  
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
		LOGTEXT(_L8("CAtSmsStoreReadAll::ParseResponseL\tFound +CMGR String!"));
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
	LOGTEXT2(_L8("CAtSmsStoreReadAll::ParseResponseL\tMessage Length=%d"),msgLength);
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
	LOGTEXT2(_L8("CAtSmsStoreReadAll::ParseResponseL\tMessage Status=%d"),
			                          iSmsData.iMessageStatus);
	LOGTEXT3(_L8("CAtSmsStoreReadAll::ParseResponseL\tSmsMsg=%S, ServiceCentre=%S"), 
			                          &iSmsData.iSmsMsg, &iSmsData.iServiceCentre);
	CleanupStack::PopAndDestroy(&array);
	}

//End of file
