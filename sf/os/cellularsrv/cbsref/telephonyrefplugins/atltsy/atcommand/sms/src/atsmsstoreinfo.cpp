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
// @file atsmsstoreinfo.cpp
// This contains CAtSmsStoreInfo which is used to get sms store information
// 

//system include 
#include <ctsy/ltsy/cctsydispatchercallback.h>

//user include 
#include "atsmsstoreinfo.h"
#include "mslogger.h"
#include "ltsycommondefine.h"
#include "globalphonemanager.h"

//constant defination
_LIT8(KGetPrefMemCommand,"AT+CPMS?\r");
_LIT8(KCPMSResponseString,"+CPMS:");

// ---------------------------------------------------------------------------
// CAtSmsStoreInfo::CAtSmsStoreInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreInfo::CAtSmsStoreInfo(CGlobalPhonemanager& aGloblePhone, 
                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                 :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtSmsStoreInfo::CAtSmsStoreInfo called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreInfo::~CAtSmsStoreInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreInfo::~CAtSmsStoreInfo()
	{
	LOGTEXT(_L8("CAtSmsStoreInfo::~CAtSmsStoreInfo called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreInfo::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreInfo* CAtSmsStoreInfo::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                        CCtsyDispatcherCallback& aCtsyDispatcherCallback)
    {
	CAtSmsStoreInfo* self = new (ELeave)CAtSmsStoreInfo(aGloblePhone, 
			                                            aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreInfo::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreInfo* CAtSmsStoreInfo::NewL(CGlobalPhonemanager& aGloblePhone, 
                                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
    {
	CAtSmsStoreInfo* self=CAtSmsStoreInfo::NewLC(aGloblePhone, 
                                                 aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreInfo::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreInfo::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreInfo::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreInfo::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreInfo::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreInfo::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtSmsStoreInfo::ExecuteCommand called"));
	iTxBuffer.Zero();
	iTxBuffer.Copy(KGetPrefMemCommand);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreInfo::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreInfo::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtSmsStoreInfo::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	if(aStatus == KErrNone)
		{
		if((aEventSource == EWriteCompletion))
			{
			LOGTEXT(_L8("CAtSmsStoreInfo::EventSignal,EWriteCompletion!"));
			return;
			}
		aStatus = iError;
		}
	Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;	
	iCtsyDispatcherCallback.CallbackPhonebookSmsStoreGetInfoComp(aStatus,iTotal,iUsed);
	}	

// ---------------------------------------------------------------------------
// CAtSmsStoreInfo::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreInfo::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
	iPhoneGlobals.iStoreTotal = iTotal;
	LOGTEXT2(_L8("CAtSmsStoreInfo::CPMSResponseL\tPreferred store name=%S"),&name);
 	LOGTEXT3(_L8("CAtSmsStoreInfo::CPMSResponseL\tPreferred store used=%d, total=%d"), 
 			                                     iUsed,iTotal);
	CleanupStack::PopAndDestroy(&array);
	}

//End of file
