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
// @file atphbkenreadall.cpp
// This contains CAtPhbkEnReadAll which is used to read all PhonebookEn Entries
// 

//system include 
#include <etelmm.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mpbutil.h>
#include <cmmgsmphonestorageutility.h>

//user include 
#include "atphbkenreadall.h"
#include "mslogger.h"

//constant defination
_LIT8(KPhbkStoreRead,"AT+CPBR=%d,%d\r");
_LIT8(KSelPhonebookEn,"AT+CPBS=\"EN\"\r");
_LIT8(KCPBRMatchString,"+CPBR:*");
_LIT8(KCPBRSResponseString,"+CPBR:");
const TInt KEnStartIndex = 1;
const TInt KEnEndIndex = 16;

// ---------------------------------------------------------------------------
// CAtPhbkEnReadAll::CAtPhbkEnReadAll
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnReadAll::CAtPhbkEnReadAll(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                   :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtPhbkEnReadAll::CAtPhbkEnReadAll called"));
	iEndFlag = EFalse;
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnReadAll::~CAtPhbkEnReadAll
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnReadAll::~CAtPhbkEnReadAll()
	{
	LOGTEXT(_L8("CAtPhbkEnReadAll::~CAtPhbkEnReadAll called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnReadAll::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnReadAll* CAtPhbkEnReadAll::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                          CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkEnReadAll* self = new (ELeave)CAtPhbkEnReadAll(aGloblePhone, 
                                                          aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnReadAll::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnReadAll* CAtPhbkEnReadAll::NewL(CGlobalPhonemanager& aGloblePhone, 
                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkEnReadAll* self=CAtPhbkEnReadAll::NewLC(aGloblePhone, 
                                                   aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnReadAll::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnReadAll::ConstructL()
	{
	CAtCommandBase::ConstructL();
	AddExpectStringL(KLtsyOkString);
	AddExpectStringL(KLtsyErrorString);
	AddExpectStringL(KCPBRMatchString);
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnReadAll::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnReadAll::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnReadAll::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnReadAll::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtPhbkEnReadAll::ExecuteCommand called"));
	iState = ESetEnName;
	iTxBuffer.Zero();	
	iTxBuffer.Copy(KSelPhonebookEn);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnReadAll::ReadEntry
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnReadAll::ReadEntry()
	{
	LOGTEXT(_L8("CAtPhbkEnReadAll::ReadEntry callded"));
	iPhoneGlobals.iEventSignalActive = ETrue;
	iTxBuffer.Zero();
	iTxBuffer.Format(KPhbkStoreRead,KEnStartIndex,KEnEndIndex);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnReadAll::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnReadAll::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtPhbkEnReadAll::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	if(aStatus != KErrNone)
		{
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhonebookEnStoreReadAllComp(aStatus,iIndex,iNumber,EFalse);
		}
	else
		{
		if( aEventSource== EWriteCompletion )
			{
			if( iState==ESetEnName )
				{
				iState = ESetEnNameComplete;
				}
			else if( iState==EReadEntry )
				{
				iState = EReadEntryComplete;
				}
			}
		else 
			{
			if( iState==ESetEnNameComplete )
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
					iCtsyDispatcherCallback.CallbackPhonebookEnStoreReadAllComp(iError,iIndex,iNumber,EFalse);
					}
				}
			else if( iState==EReadEntryComplete )
				{
				//encounter OK or ERROR
				if( iEndFlag )
					{
					ClearBuffer();
					iIndex++;
					iNumber.Zero();
					Complete();
					iPhoneGlobals.iEventSignalActive = EFalse;
		    		iCtsyDispatcherCallback.CallbackPhonebookEnStoreReadAllComp(iError,iIndex,iNumber,EFalse);
					}
				else 
					{
					iCtsyDispatcherCallback.CallbackPhonebookEnStoreReadAllComp(iError,iIndex,iNumber,ETrue);
					ClearCurrentLine();
					}
				}
			}
		}
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnReadAll::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnReadAll::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if( iState == ESetEnNameComplete )
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
		* at+CPBR=1,16  
		*+CPBR: 1,"112",129
		*+CPBR: 2,"911",129
		*OK
		*/
		RArray<TPtrC8> array;
		CleanupClosePushL(array);
		iParser->ParseRespondedBuffer(array,Buffer());
		// remove the string of AT+CPBR=..
		if(array[0].MatchF(KAnyATCommand)!=KErrNotFound)
			{
			array.Remove(0);
			}
		TInt count = array.Count();
#ifdef _DEBUG		
		for( TInt i = 0;i<count;i++ )
			{
			LOGTEXT3(_L8("CAtPhbkEnReadAll::ParseResponseL\tarray[%d]=%S"),i,&array[i]);  
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
		iIndex = val;     
		iNumber.Zero();
		iNumber.Copy(array[2]);   
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
		CleanupStack::PopAndDestroy();
		} 
  	}

//End of file
