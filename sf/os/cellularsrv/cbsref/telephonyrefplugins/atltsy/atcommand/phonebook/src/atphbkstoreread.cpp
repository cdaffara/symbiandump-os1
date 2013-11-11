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
// @file atphbkstoreread.cpp
// This contains CAtPhbkStoreRead which is use to read a phonebook Entry
// 

//system include
#include <etelmm.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mpbutil.h>
#include <cmmgsmphonestorageutility.h>

//user include 
#include "atphbkstoreread.h"
#include "mslogger.h"

//constant defination
_LIT8(KPhbkStoreRead,"AT+CPBR=%d,%d\r");
_LIT8(KSelAdnPhonebook,"AT+CPBS=\"SM\"\r");
_LIT8(KSelFdnPhonebook,"AT+CPBS=\"FD\"\r");
_LIT8(KCPBRMatchString,"+CPBR:*");
_LIT8(KCPBRSResponseString,"+CPBR:");

// ---------------------------------------------------------------------------
// CAtPhbkStoreRead::CAtPhbkStoreRead
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreRead::CAtPhbkStoreRead(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                   :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtPhbkStoreRead::CAtPhbkStoreRead called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreRead::~CAtPhbkStoreRead
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreRead::~CAtPhbkStoreRead()
	{
	LOGTEXT(_L8("CAtPhbkStoreRead::~CAtPhbkStoreRead called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreRead::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreRead* CAtPhbkStoreRead::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                          CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkStoreRead* self = new (ELeave)CAtPhbkStoreRead(aGloblePhone, 
                                                          aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreRead::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreRead* CAtPhbkStoreRead::NewL(CGlobalPhonemanager& aGloblePhone, 
                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkStoreRead* self=CAtPhbkStoreRead::NewLC(aGloblePhone, 
                                                   aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreRead::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreRead::ConstructL()
	{
	CAtCommandBase::ConstructL();
	AddExpectStringL(KLtsyOkString);
	AddExpectStringL(KLtsyErrorString);
	AddExpectStringL(KCPBRMatchString);
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreRead::SetPhonebookType
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreRead::SetPhonebookType(DispatcherPhonebook::TPhonebook aPhonebookType)
	{
	iPhoneBookType = aPhonebookType;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreRead::SetReadParam
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreRead::SetReadParam(TInt aIndex,TInt aSlotNum)
	{
	iIndex = aIndex;
	iSlotNum = aSlotNum;
	}	

// ---------------------------------------------------------------------------
// CAtPhbkStoreRead::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreRead::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreRead::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreRead::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtPhbkStoreRead::ExecuteCommand called"));
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
// CAtPhbkStoreRead::ReadEntry
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreRead::ReadEntry()
	{
	LOGTEXT(_L8("CAtPhbkStoreRead::ReadEntry callded"));
	iPhoneGlobals.iEventSignalActive = ETrue;
	iTxBuffer.Zero();
	iTxBuffer.Format(KPhbkStoreRead,iIndex,iIndex+iSlotNum-1);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreRead::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreRead::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtPhbkStoreRead::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	if(aStatus != KErrNone)
		{
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhonebookStoreReadEntryComp(aStatus,iPhoneBookType,iPhbkData);
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
					iCtsyDispatcherCallback.CallbackPhonebookStoreReadEntryComp(iError,iPhoneBookType,iPhbkData);
					}
				}
			else if( iState==EReadEntryComplete )
				{
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				//encounter OK or ERROR
				if( iEndFlag )
					{
					ClearBuffer();
					iPhbkData.Zero();
					iCtsyDispatcherCallback.CallbackPhonebookStoreReadEntryComp(iError,iPhoneBookType,iPhbkData);
					}
				else
					{
					if( iError==KErrNone )
						{
						TRAPD(err, GeneratePhbkDataL());
						iCtsyDispatcherCallback.CallbackPhonebookStoreReadEntryComp(err,iPhoneBookType,iPhbkData);
						ClearCurrentLine();
						}
					else
						{
						iCtsyDispatcherCallback.CallbackPhonebookStoreReadEntryComp(iError,iPhoneBookType,iPhbkData);
						}
					}
				}
			}
		}
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreRead::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreRead::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
		* +CPBR = <index>,<number>,<129/145>,<text> 
		* OK    
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
			LOGTEXT3(_L8("CAtPhbkStoreRead::ParseResponseL\tarray[%d]=%S"),i,&array[i]); 
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
// CAtPhbkStoreRead::GeneratePhbkData
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CAtPhbkStoreRead::GeneratePhbkDataL()
	{
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	iPhbkData.Zero();
	pbBuffer->Set(&iPhbkData); // Set it to point to the Client buffer
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
