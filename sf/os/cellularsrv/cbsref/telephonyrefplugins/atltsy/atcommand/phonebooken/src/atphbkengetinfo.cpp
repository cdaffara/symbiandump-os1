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
// @file atphbkengetinfo.cpp
// This contains CAtPhbkEnGetInfo which is used to get emergency call info
// 

//system include 
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <etelmm.h>

//user include
#include "atphbkengetinfo.h"
#include "mslogger.h"
#include "ltsycommondefine.h"

//constant defination
_LIT8(KSetEnStore,"AT+CPBS=\"%S\"\r");
_LIT8(KGetEnStoreInfo,"AT+CPBS?\r");
_LIT8(KCPBSResponseString,"+CPBS:");
_LIT8(KPhonebookEn,"EN");

// ---------------------------------------------------------------------------
// CAtPhbkEnGetInfo::CAtPhbkEnGetInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnGetInfo::CAtPhbkEnGetInfo(CGlobalPhonemanager& aGloblePhone, 
                                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                                   :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtPhbkEnGetInfo::CAtPhbkEnGetInfo called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnGetInfo::~CAtPhbkEnGetInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnGetInfo::~CAtPhbkEnGetInfo()
	{
	LOGTEXT(_L8("CAtPhbkEnGetInfo::~CAtPhbkEnGetInfo called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnGetInfo::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnGetInfo* CAtPhbkEnGetInfo::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                                          CCtsyDispatcherCallback& aCtsyDispatcherCallback)
    {
	CAtPhbkEnGetInfo* self = new (ELeave)CAtPhbkEnGetInfo(aGloblePhone, 
			                                                              aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnGetInfo::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnGetInfo* CAtPhbkEnGetInfo::NewL(CGlobalPhonemanager& aGloblePhone, 
                                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
    {
	CAtPhbkEnGetInfo* self=CAtPhbkEnGetInfo::NewLC(aGloblePhone, 
                                                                   aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnGetInfo::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnGetInfo::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}


// ---------------------------------------------------------------------------
// CAtPhbkEnGetInfo::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnGetInfo::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnGetInfo::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnGetInfo::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtPhbkEnGetInfo::ExecuteCommand called"));
	iState = ESetEnStore;
	TPtrC8  storeName;
	iTxBuffer.Zero();
	storeName.Set(KPhonebookEn);
	iTxBuffer.Format(KSetEnStore,&storeName);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnGetInfo::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnGetInfo::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtPhbkEnGetInfo::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	
	if(aStatus != KErrNone)
		{
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhonebookEnStoreGetInfoComp(aStatus,iUsed);
		}
	else
		{
		if( aEventSource == EWriteCompletion )
			{
			LOGTEXT(_L8("CAtPhbkEnGetInfo::EventSignal,EWriteCompletion!"));
			if( iState==ESetEnStore )
				{
				iState = ESetEnStoreComplete;
				}
			else if( iState==EGetEnStoreInfo )
				{
				iState = EGetEnStoreInfoComplete;
				}
			 }
		else 
			{
			LOGTEXT(_L8("CAtPhbkEnGetInfo::EventSignal,EReadCompletion!"));
			if( iState == ESetEnStoreComplete )
				{
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				if(iError == KErrNone)
					{
					iState = EGetEnStoreInfo;
					GetEnStoreInfo();
					}
				else
					{
					iCtsyDispatcherCallback.CallbackPhonebookEnStoreGetInfoComp(iError,iUsed);
					}
				}
			else if( iState == EGetEnStoreInfoComplete )
				{
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				iCtsyDispatcherCallback.CallbackPhonebookEnStoreGetInfoComp(iError,iUsed);
				}
			}
		}
	}	

// ---------------------------------------------------------------------------
// CAtPhbkEnGetInfo::GetEnStoreInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnGetInfo::GetEnStoreInfo()
	{
	LOGTEXT(_L8("CAtPhbkEnGetInfo::GetEnStoreInfo callded"));
	iPhoneGlobals.iEventSignalActive = ETrue;
	iTxBuffer.Zero();
	iTxBuffer.Copy(KGetEnStoreInfo);
	Write();   
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnGetInfo::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnGetInfo::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if( iState==ESetEnStoreComplete )
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
	else if( iState==EGetEnStoreInfoComplete )
		{
		/**
		* Here is an Example 
		* +CPBS: "EN",2,16 
        * phonebooken is selected,2 locations are used and 
        * total 16 locations are available
		*/
		iError = KErrNone;
		RArray<TPtrC8> array;
		CleanupClosePushL(array);
		iParser->ParseRespondedBuffer(array,Buffer());
		//remove AT+CPBS
		if(array[0].MatchF(KAnyATCommand)!=KErrNotFound)
			{
			array.Remove(0);
			}
		TInt count = array.Count();
#ifdef _DEBUG			
		for( TInt i=0;i<count;i++ )
			{
		    LOGTEXT3(_L8("CAtPhbkEnGetInfo::ParseResponseL\tarray[%d]=%S"),
		    		      i,&array[i]);     
			}
#endif			
		if (count <= 1)
			{
			CleanupStack::PopAndDestroy();
			iError = KErrNotFound;
			return;
			}
		if(array[0].MatchF(KCPBSResponseString)==KErrNotFound)
			{
			CleanupStack::PopAndDestroy();
			iError = KErrNotFound;
			return;
			}
				
		//parse used
		TLex8 lex(array[2]);
		TUint16 val;
		TInt ret=lex.Val(val,EDecimal);
		if(ret!=KErrNone)
			{
			CleanupStack::PopAndDestroy();
			iError = ret;
			return;
			}
		iUsed = val;
		
		//parse total
		TLex8 lex1(array[3]);
		ret=lex1.Val(val,EDecimal);
		if(ret!=KErrNone)
			{
			CleanupStack::PopAndDestroy();
			iError = ret;
			return;
			}
		iTotal = val;
	    LOGTEXT3(_L8("CAtPhbkEnGetInfo::ParseResponseL\tPreferred store used=%d, total=%d"), 
	    		                                iUsed,iTotal);
		CleanupStack::PopAndDestroy(&array);
		}
	}

//End of file
