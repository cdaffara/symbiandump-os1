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
// @file atphbkgetphonestoreinfo.cpp
// This contains CAtPhbkGetPhoneStoreInfo which is used to get phonebook information
// 

//system include 
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <etelmm.h>

//user include
#include "atphbkgetphonestoreinfo.h"
#include "mslogger.h"
#include "ltsycommondefine.h"

//constant defination
_LIT8(KSetPhoneStore,"AT+CPBS=\"%S\"\r");
_LIT8(KGetPhoneStoreInfo,"AT+CPBS?\r");
_LIT8(KCPBSResponseString,"+CPBS:");
_LIT8(KAdnPhonebook,"SM");
_LIT8(KFdnPhonebook,"FD");
_LIT8(KGetPrefMemCommand,"AT+CPMS?\r");
_LIT8(KCPMSResponseString,"+CPMS:");


// ---------------------------------------------------------------------------
// CAtPhbkGetPhoneStoreInfo::CAtPhbkGetPhoneStoreInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkGetPhoneStoreInfo::CAtPhbkGetPhoneStoreInfo(CGlobalPhonemanager& aGloblePhone, 
                                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                                   :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtPhbkGetPhoneStoreInfo::CAtPhbkGetPhoneStoreInfo called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkGetPhoneStoreInfo::~CAtPhbkGetPhoneStoreInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkGetPhoneStoreInfo::~CAtPhbkGetPhoneStoreInfo()
	{
	LOGTEXT(_L8("CAtPhbkGetPhoneStoreInfo::~CAtPhbkGetPhoneStoreInfo called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkGetPhoneStoreInfo::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkGetPhoneStoreInfo* CAtPhbkGetPhoneStoreInfo::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                                          CCtsyDispatcherCallback& aCtsyDispatcherCallback)
    {
	CAtPhbkGetPhoneStoreInfo* self = new (ELeave)CAtPhbkGetPhoneStoreInfo(aGloblePhone, 
			                                                              aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkGetPhoneStoreInfo::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkGetPhoneStoreInfo* CAtPhbkGetPhoneStoreInfo::NewL(CGlobalPhonemanager& aGloblePhone, 
                                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
    {
	CAtPhbkGetPhoneStoreInfo* self=CAtPhbkGetPhoneStoreInfo::NewLC(aGloblePhone, 
                                                                   aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkGetPhoneStoreInfo::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkGetPhoneStoreInfo::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CAtPhbkGetPhoneStoreInfo::SetPhoneStoreParam
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkGetPhoneStoreInfo::SetPhoneStoreParam(const TDesC& aStoreName)
	{
	iStoreName = aStoreName;
	}

// ---------------------------------------------------------------------------
// CAtPhbkGetPhoneStoreInfo::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkGetPhoneStoreInfo::StartRequest()
	{
	if( iStoreName.CompareF(KETelIccSmsStore) == KErrNone)
		{
		iState = ESetIccSmsStore;
		iTxBuffer.Zero();
		iTxBuffer.Copy(KGetPrefMemCommand);
		Write();
		}
	else
		{
	    ExecuteCommand();
		}
	}

// ---------------------------------------------------------------------------
// CAtPhbkGetPhoneStoreInfo::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkGetPhoneStoreInfo::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtPhbkGetPhoneStoreInfo::ExecuteCommand called"));
	iState = ESetPhoneStore;
	TPtrC8  storeName;
	iTxBuffer.Zero();
	if( iStoreName.CompareF(KETelIccFdnPhoneBook) == KErrNone )
		{
		storeName.Set(KFdnPhonebook);
		}
	else if( iStoreName.CompareF(KETelIccAdnPhoneBook) == KErrNone)
		{
		storeName.Set(KAdnPhonebook);
		}
	iTxBuffer.Format(KSetPhoneStore,&storeName);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkGetPhoneStoreInfo::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkGetPhoneStoreInfo::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtPhbkGetPhoneStoreInfo::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	
	if(aStatus != KErrNone)
		{
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhonebookGetPhoneStoreInfoComp(aStatus,iPhoneStoreInfo);
		}
	else
		{
		if( aEventSource == EWriteCompletion )
			{
			LOGTEXT(_L8("CAtPhbkGetPhoneStoreInfo::EventSignal,EWriteCompletion!"));
			if( iState == ESetPhoneStore )
				{
				iState = ESetPhoneStoreComplete;
				}
			else if( iState == EGetPhoneStoreInfo )
				{
				iState = EGetPhoneStoreInfoComplete;
				}
			else if( iState ==ESetIccSmsStore )
				{
				iState = ESetIccSmsStoreComplete;
				}
			}
    	else 
			{
			LOGTEXT(_L8("CAtPhbkGetPhoneStoreInfo::EventSignal,EReadCompletion!"));
			if( iState == ESetPhoneStoreComplete )
				{
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				if(iError == KErrNone)
					{
					iState = EGetPhoneStoreInfo;
					GetPhoneStoreInfo();
					}
				else
					{
					iCtsyDispatcherCallback.CallbackPhonebookGetPhoneStoreInfoComp(iError,iPhoneStoreInfo);
					}
				}
			else if( iState == EGetPhoneStoreInfoComplete )
				{
				iPhoneStoreInfo.iName = iStoreName;
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				iCtsyDispatcherCallback.CallbackPhonebookGetPhoneStoreInfoComp(iError,iPhoneStoreInfo);
				}
			else if( iState == ESetIccSmsStoreComplete)
				{
				iPhoneStoreInfo.iName = iStoreName;
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				iCtsyDispatcherCallback.CallbackPhonebookGetPhoneStoreInfoComp(iError,iPhoneStoreInfo);
				}
			}
		}
	}	

// ---------------------------------------------------------------------------
// CAtPhbkGetPhoneStoreInfo::GetPhoneStoreInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkGetPhoneStoreInfo::GetPhoneStoreInfo()
	{
	LOGTEXT(_L8("CAtPhbkGetPhoneStoreInfo::GetPhoneStoreInfo callded"));
	iPhoneGlobals.iEventSignalActive = ETrue;
	iTxBuffer.Zero();
	iTxBuffer.Copy(KGetPhoneStoreInfo);
	Write();   
	}

// ---------------------------------------------------------------------------
// CAtPhbkGetPhoneStoreInfo::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkGetPhoneStoreInfo::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if( iState == ESetPhoneStoreComplete )
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
	else if( iState == EGetPhoneStoreInfoComplete )
		{
		/**
		* Here is an Example 
		* +CPBS: "SM",6,250  
        * ADN phonebook is selected,6 locations are used and 
        * total 250 locations are available
		*/
		iError = KErrNone;
		RArray<TPtrC8> array;
		CleanupClosePushL(array);
		iParser->ParseRespondedBuffer(array,Buffer());
		//remove AT+CPBS
		if(array[0].MatchF(KAnyATCommand) != KErrNotFound)
			{
			array.Remove(0);
			}
		TInt count = array.Count();
#ifdef _DEBUG			
		for( TInt i=0;i<count;i++ )
			{
		    LOGTEXT3(_L8("CAtPhbkGetPhoneStoreInfo::ParseResponseL\tarray[%d]=%S"),
		    		      i,&array[i]);     
			}
#endif			
		if (count <= 1)
			{
			CleanupStack::PopAndDestroy();
			iError = KErrNotFound;
			return;
			}
		if(array[0].MatchF(KCPBSResponseString) == KErrNotFound)
			{
			CleanupStack::PopAndDestroy();
			iError = KErrNotFound;
			return;
			}
				
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
		iPhoneStoreInfo.iUsedEntries = val;
		
		//parse total
		TLex8 lex1(array[3]);
		ret = lex1.Val(val,EDecimal);
		if(ret != KErrNone)
			{
			CleanupStack::PopAndDestroy();
			iError = ret;
			return;
			}
		iPhoneStoreInfo.iTotalEntries = val;
		iPhoneStoreInfo.iType = RMobilePhoneStore::EPhoneBookStore;
	    LOGTEXT3(_L8("CAtPhbkGetPhoneStoreInfo::ParseResponseL\tPreferred store used=%d, total=%d"), 
	    		                                iPhoneStoreInfo.iUsedEntries,iPhoneStoreInfo.iTotalEntries);
		CleanupStack::PopAndDestroy(&array);
		}
	else 
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
		iPhoneStoreInfo.iUsedEntries = val;
		
		//parse total
		TLex8 lex1(array[3]);
		ret = lex1.Val(val,EDecimal);
		if(ret != KErrNone)
			{
			CleanupStack::PopAndDestroy();
			iError = ret;
			return;
			}
		iPhoneStoreInfo.iTotalEntries = val;
		iPhoneStoreInfo.iType = RMobilePhoneStore::EShortMessageStore;
		LOGTEXT3(_L8("CAtPhbkGetPhoneStoreInfo::ParseResponseL\tPreferred store used=%d, total=%d"), 
			    		                        iPhoneStoreInfo.iUsedEntries,iPhoneStoreInfo.iTotalEntries);
		CleanupStack::PopAndDestroy(&array);
		}
	}

//End of file
