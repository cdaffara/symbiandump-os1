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
// @file atphbkstorecache.cpp
// This contains CAtPhbkStoreCache which is used to cache phonebook Entries
// 

//system include 
#include <etelmm.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mpbutil.h>
#include <cmmgsmphonestorageutility.h>

//user include 
#include "atphbkstorecache.h"
#include "mslogger.h"

//constant defination
_LIT8(KPhbkStoreRead,"AT+CPBR=%d,%d\r");
_LIT8(KSelAdnPhonebook,"AT+CPBS=\"SM\"\r");
_LIT8(KSelFdnPhonebook,"AT+CPBS=\"FD\"\r");
_LIT8(KCPBRMatchString,"+CPBR:*");
_LIT8(KCPBRSResponseString,"+CPBR:");
const TInt KAdnStartIndex = 1;
const TInt KFdnStartIndex = 1;

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::CAtPhbkStoreCache
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreCache::CAtPhbkStoreCache(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                   :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtPhbkStoreCache::CAtPhbkStoreCache called"));
	iEndFlag = EFalse;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::~CAtPhbkStoreCache
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreCache::~CAtPhbkStoreCache()
	{
	LOGTEXT(_L8("CAtPhbkStoreCache::~CAtPhbkStoreCache called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreCache* CAtPhbkStoreCache::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                          CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkStoreCache* self = new (ELeave)CAtPhbkStoreCache(aGloblePhone, 
                                                          aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreCache* CAtPhbkStoreCache::NewL(CGlobalPhonemanager& aGloblePhone, 
                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkStoreCache* self=CAtPhbkStoreCache::NewLC(aGloblePhone, 
                                                   aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreCache::ConstructL()
	{
	CAtCommandBase::ConstructL();
	AddExpectStringL(KLtsyOkString);
	AddExpectStringL(KLtsyErrorString);
	AddExpectStringL(KCPBRMatchString);
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::SetPhonebookType
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreCache::SetPhonebookType(DispatcherPhonebook::TPhonebook aPhonebookType)
	{
	iPhoneBookType = aPhonebookType;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreCache::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreCache::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtPhbkStoreCache::ExecuteCommand called"));
	iState = ESetStoreName;
	iTxBuffer.Zero();	
	if( iPhoneBookType == DispatcherPhonebook::EIccAdn )
		{
		iTxBuffer.Copy(KSelAdnPhonebook);
		}
	else if( iPhoneBookType == DispatcherPhonebook::EIccFdn )
		{
		iTxBuffer.Copy(KSelFdnPhonebook);
		}
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::ReadEntry
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreCache::ReadEntry()
	{
	LOGTEXT(_L8("CAtPhbkStoreCache::ReadEntry callded"));
	iPhoneGlobals.iEventSignalActive = ETrue;
	iTxBuffer.Zero();
	TInt totalEntries(KErrNone);
	if( iPhoneBookType == DispatcherPhonebook::EIccAdn )
		{
		totalEntries = iPhoneGlobals.iPhonebookStoreInfo.iAdnTotalEntries;
		iTxBuffer.Format(KPhbkStoreRead,KAdnStartIndex,totalEntries);
		}
	else if( iPhoneBookType == DispatcherPhonebook::EIccFdn )
		{
		totalEntries = iPhoneGlobals.iPhonebookStoreInfo.iFdnTotalEntries; 
		iTxBuffer.Format(KPhbkStoreRead,KFdnStartIndex,totalEntries);
		}
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreCache::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtPhbkStoreCache::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	if(aStatus != KErrNone)
		{
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhonebookStoreCacheComp(aStatus,iPhoneBookType,iPhbkData);
		}
	else
		{
		if( aEventSource== EWriteCompletion )
			{
			if( iState==ESetStoreName )
				{
				iState = ESetStoreNameComplete;
				}
			else if( iState==EReadEntry )
				{
				iState = EReadEntryComplete;
				}
			}
		else 
			{
			if( iState==ESetStoreNameComplete )
				{
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				if(iError == KErrNone)
					{
					ClearBuffer();
					iState = EReadEntry;
					ReadEntry();
					}
				else 
					{
					iCtsyDispatcherCallback.CallbackPhonebookStoreCacheComp(iError,iPhoneBookType,iPhbkData);
					}
				}
			else if( iState==EReadEntryComplete )
				{
				//encounter OK or ERROR
				if( iEndFlag )
					{
					ClearBuffer();
					iPhbkData.Zero();
					Complete();
					iPhoneGlobals.iEventSignalActive = EFalse;
					iCtsyDispatcherCallback.CallbackPhonebookStoreCacheComp(iError,iPhoneBookType,iPhbkData);
					}
				else 
					{
					if( iError==KErrNone )
						{
						TRAPD(err, GeneratePhbkDataL());
						iCtsyDispatcherCallback.CallbackPhonebookStoreCacheComp(err,iPhoneBookType,iPhbkData);
						ClearCurrentLine();
						}
					else 
						{
						iCtsyDispatcherCallback.CallbackPhonebookStoreCacheComp(iError,iPhoneBookType,iPhbkData);
						ClearCurrentLine();
						}
					}
				}
			}
		}
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreCache::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if( iState == ESetStoreNameComplete )
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
	else if( iState == EReadEntryComplete )
		{
		/**
		* Here is an Example 
		* at+CPBR=1,250    
		*+CPBR: 1,"1234001",129,""
		*+CPBR: 2,"123456002",129,"TelNo 002"
		*+CPBR: 3,"123456003",129,"TelNo 003"
		*+CPBR: 4,"123456004",129,"TelNo 004"
		*OK
		*/
		RArray<TPtrC8> array;
		CleanupClosePushL(array);
		iParser->ParseRespondedBuffer(array,Buffer());
		// remove the string of AT+CPBR=..
		if(array[0].MatchF(KAnyATCommand) != KErrNotFound)
			{
			array.Remove(0);
			}
		TInt count = array.Count();
#ifdef _DEBUG		
		for( TInt i=0;i<count;i++ )
			{
			LOGTEXT3(_L8("CAtPhbkStoreCache::ParseResponseL\tarray[%d]=%S"),i,&array[i]);  
			}
#endif		
		if((array[1].Match(KLtsyOkString) == KErrNone))
			{
			CleanupStack::PopAndDestroy();
			iError = KErrNone;
			iEndFlag = ETrue;
			return; 
			}
		else if((array[1].Match(KLtsyErrorString) == KErrNone))
			{
			CleanupStack::PopAndDestroy();
			iError = KErrGeneral;
			iEndFlag = ETrue;
			return; 
			}
	  
		iError = KErrNone;
		//remove number line if it has 
		if( array[0].MatchF(KCPBRSResponseString) == KErrNotFound )
			{
			array.Remove(0);
			}
		if( array[0].MatchF(KCPBRSResponseString) == KErrNotFound )
			{
			CleanupStack::PopAndDestroy();
			iError = KErrNotFound;
			return;
			}
		//parse index
		TLex8 lex(array[1]);
		TUint16 val;
		TInt ret = lex.Val(val,EDecimal);
		if(ret != KErrNone)
			{
			CleanupStack::PopAndDestroy();
			iError = ret;
			return;
			}
		//Index  
		iReadEntry.iIndex = val;             
		iReadEntry.iNumber.Copy(array[2]);   
		//Parse TON/NPI
		TLex8 lex1(array[3]);
		TUint16 val1;
		ret = lex1.Val(val1,EDecimal);
		if(ret != KErrNone)
			{
			CleanupStack::PopAndDestroy();
			iError = ret;
			return;
			}
		 //TON/NPI
		iReadEntry.iTon = (TUint8)val1;  
		iReadEntry.iText.Copy(array[4]);
		CleanupStack::PopAndDestroy();
		} 
  	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreCache::GeneratePhbkData
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CAtPhbkStoreCache::GeneratePhbkDataL()
	{
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	iPhbkData.Zero();
	 // Set it to point to the Client buffer
	pbBuffer->Set(&iPhbkData);
	pbBuffer->AddNewEntryTag();
	
	TInt ret = pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex,iReadEntry.iIndex);
	if( ret!=KErrNone )
		{
		delete pbBuffer;
		return ret;
		}
	ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, iReadEntry.iNumber);
	if( ret!=KErrNone )
		{
		delete pbBuffer;
		return ret;
		}
	ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, iReadEntry.iTon);
	if(ret!=KErrNone)
		{
		delete pbBuffer;
		return ret;
		}
	ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, iReadEntry.iText);
	if(ret != KErrNone)
		{
		delete pbBuffer;
		return ret;
		}
	delete pbBuffer;
	return KErrNone;
	}

//End of file
