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
// @file atphbkenread.cpp
// This contains CAtPhbkEnRead which is use to read a PhonebookEn Entry
// 

//system include
#include <etelmm.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mpbutil.h>
#include <cmmgsmphonestorageutility.h>

//user include 
#include "atphbkenread.h"
#include "mslogger.h"

//constant defination
_LIT8(KPhbkStoreRead,"AT+CPBR=%d\r");
_LIT8(KSelPhonebookEn,"AT+CPBS=\"EN\"\r");
_LIT8(KCPBRMatchString,"+CPBR:*");
_LIT8(KCPBRSResponseString,"+CPBR:");

// ---------------------------------------------------------------------------
// CAtPhbkEnRead::CAtPhbkEnRead
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnRead::CAtPhbkEnRead(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                   :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtPhbkEnRead::CAtPhbkEnRead called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnRead::~CAtPhbkEnRead
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnRead::~CAtPhbkEnRead()
	{
	LOGTEXT(_L8("CAtPhbkEnRead::~CAtPhbkEnRead called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnRead::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnRead* CAtPhbkEnRead::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                          CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkEnRead* self = new (ELeave)CAtPhbkEnRead(aGloblePhone, 
                                                          aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnRead::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkEnRead* CAtPhbkEnRead::NewL(CGlobalPhonemanager& aGloblePhone, 
                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkEnRead* self=CAtPhbkEnRead::NewLC(aGloblePhone, 
                                                   aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnRead::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnRead::ConstructL()
	{
	CAtCommandBase::ConstructL();
	AddExpectStringL(KLtsyOkString);
	AddExpectStringL(KLtsyErrorString);
	AddExpectStringL(KCPBRMatchString);
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnRead::SetIndex
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnRead::SetIndex(TInt aIndex)
	{
	iIndex = aIndex;
	}	

// ---------------------------------------------------------------------------
// CAtPhbkEnRead::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnRead::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnRead::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnRead::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtPhbkEnRead::ExecuteCommand called"));
	iState = ESetEnName;
	iTxBuffer.Zero();	
	iTxBuffer.Copy(KSelPhonebookEn);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnRead::ReadEntry
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnRead::ReadEntry()
	{
	LOGTEXT(_L8("CAtPhbkEnRead::ReadEntry callded"));
	iPhoneGlobals.iEventSignalActive = ETrue;
	iTxBuffer.Zero();
	iTxBuffer.Format(KPhbkStoreRead,iIndex);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnRead::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnRead::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtPhbkEnRead::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	if(aStatus != KErrNone)
		{
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhonebookEnStoreReadEntryComp(aStatus,iIndex,iNumber);
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
					iCtsyDispatcherCallback.CallbackPhonebookEnStoreReadEntryComp(iError,iIndex,iNumber);
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
					iCtsyDispatcherCallback.CallbackPhonebookEnStoreReadEntryComp(iError,iIndex,iNumber);
					}
				else
					{
					iCtsyDispatcherCallback.CallbackPhonebookEnStoreReadEntryComp(iError,iIndex,iNumber);
					ClearCurrentLine();
					}
				}
			} 
		}
	}

// ---------------------------------------------------------------------------
// CAtPhbkEnRead::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkEnRead::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
		* +CPBR = <index>,<number>,<129>
		* OK    
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
		for( TInt i=0;i<count;i++ )
			{
			LOGTEXT3(_L8("CAtPhbkEnRead::ParseResponseL\tarray[%d]=%S"),i,&array[i]); 
			}
#endif		
		if((array[0].Match(KLtsyOkString) == KErrNone))
			{
			CleanupStack::PopAndDestroy();
			iError = KErrNone;
			iEndFlag = ETrue;
			return; 
			}
		else if((array[0].Match(KLtsyErrorString) == KErrNone))
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
