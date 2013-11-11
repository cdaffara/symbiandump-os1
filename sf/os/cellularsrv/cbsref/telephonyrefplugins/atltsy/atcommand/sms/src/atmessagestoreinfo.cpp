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
// @file atmessagestoreinfo.cpp
// This contains CAtMessageStoreInfo which is used to get message store information
// 

//system include 
#include <ctsy/ltsy/cctsydispatchercallback.h>

//user include 
#include "atmessagestoreinfo.h"
#include "mslogger.h"

//constant defination
_LIT8(KGetPrefMemCommand,"AT+CPMS?\r");
_LIT8(KCPMSResponseString,"+CPMS:");

// ---------------------------------------------------------------------------
// CAtMessageStoreInfo::CAtMessageStoreInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtMessageStoreInfo::CAtMessageStoreInfo(CGlobalPhonemanager& aGloblePhone, 
                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                         :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtMessageStoreInfo::CAtMessageStoreInfo called"));
	}

// ---------------------------------------------------------------------------
// CAtMessageStoreInfo::~CAtMessageStoreInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtMessageStoreInfo::~CAtMessageStoreInfo()
	{
	LOGTEXT(_L8("CAtMessageStoreInfo::~CAtMessageStoreInfo called"));
	}

// ---------------------------------------------------------------------------
// CAtMessageStoreInfo::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtMessageStoreInfo* CAtMessageStoreInfo::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                                CCtsyDispatcherCallback& aCtsyDispatcherCallback)
    {
	CAtMessageStoreInfo* self = new (ELeave)CAtMessageStoreInfo(aGloblePhone, 
			                                                    aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtMessageStoreInfo::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtMessageStoreInfo* CAtMessageStoreInfo::NewL(CGlobalPhonemanager& aGloblePhone, 
                                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
    {
	CAtMessageStoreInfo* self=CAtMessageStoreInfo::NewLC(aGloblePhone, 
                                                         aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtMessageStoreInfo::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtMessageStoreInfo::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CAtMessageStoreInfo::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtMessageStoreInfo::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtMessageStoreInfo::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtMessageStoreInfo::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtMessageStoreInfo::ExecuteCommand called"));
	
	iTxBuffer.Zero();
	iTxBuffer.Copy(KGetPrefMemCommand);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtMessageStoreInfo::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtMessageStoreInfo::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtMessageStoreInfo::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	if(aStatus == KErrNone)
		{
		if((aEventSource == EWriteCompletion))
			{
			LOGTEXT(_L8("CAtMessageStoreInfo::EventSignal,EWriteCompletion!"));
			return;
			}
		aStatus = iError;
		}
	Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCtsyDispatcherCallback.CallbackSmsGetSmsStoreInfoComp(aStatus,iTotal,iUsed);
	}	

// ---------------------------------------------------------------------------
// CAtMessageStoreInfo::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtMessageStoreInfo::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	/**
	* Parse result of AT+CPMS? 
	* Response:+CPMS:<mem1>,<used1>,<total1>,<mem2>,<used2>,<total2>
	* Here only the first mem is used. mem1 is SM
	*/
	iError = KErrNone;
	RArray<TPtrC8> array;
	CleanupClosePushL(array);
	iParser->ParseRespondedBuffer(array,Buffer());
	if(array[0].MatchF(KAnyATCommand) != KErrNotFound)
		{
		array.Remove(0);
		}
	TInt Count = array.Count();
	if (Count <= 1)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return;
		}
	if(array[0].MatchF(KCPMSResponseString) == KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return;
		}
	//parse name 
	TPtrC8 name(array[1]);
	
	//parse used
	TLex8 lex(array[2]);
	TUint16 val;
	TInt ret = lex.Val(val,EDecimal);
	if(ret != KErrNone)
		{
		CleanupStack::PopAndDestroy();
		iError = ret;
		return;
		}
	iUsed = val;
	
	//parse total
	TLex8 lex1(array[3]);
	ret = lex1.Val(val,EDecimal);
	if(ret != KErrNone)
		{
		CleanupStack::PopAndDestroy();
		iError = ret;
		return;
		}
	iTotal = val;
	
	LOGTEXT2(_L8("CAtMessageStoreInfo::ParseResponseL\tPreferred store name=%S"),&name);
 	LOGTEXT3(_L8("CAtMessageStoreInfo::ParseResponseL\tPreferred store used=%d, total=%d"), 
 			                                           iUsed,iTotal);
	CleanupStack::PopAndDestroy(&array);
	}

//End of file
