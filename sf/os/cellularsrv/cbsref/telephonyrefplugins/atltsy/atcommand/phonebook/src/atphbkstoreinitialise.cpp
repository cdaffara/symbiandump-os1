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
// @file atphbkstoreinitialise.cpp
// This contains CAtPhbkStoreInitialise which initialises all kinds of phonebook 
// 

//system include 
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <etelmm.h>

//user  include 
#include "atphbkstoreinitialise.h"
#include "mslogger.h"
#include "ltsycommondefine.h"

//constant defination
_LIT8(KAdnStore,"AT+CPBS=\"SM\"\r");
_LIT8(KFdnStore,"AT+CPBS=\"FD\"\r");
_LIT8(KGetSotreInfo,"AT+CPBR=?\r");
_LIT8(KCPBRResponseString,"+CPBR:");
_LIT8(KSeparator,"-");

// ---------------------------------------------------------------------------
// CAtPhbkStoreInitialise::CAtPhbkStoreInitialise
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreInitialise::CAtPhbkStoreInitialise(CGlobalPhonemanager& aGloblePhone, 
                                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                               :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtPhbkStoreInitialise::CAtPhbkStoreInitialise called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreInitialise::~CAtPhbkStoreInitialise
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreInitialise::~CAtPhbkStoreInitialise()
	{
	LOGTEXT(_L8("CAtPhbkStoreInitialise::~CAtPhbkStoreInitialise called"));
	
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreInitialise::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreInitialise* CAtPhbkStoreInitialise::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                                      CCtsyDispatcherCallback& aCtsyDispatcherCallback)
    {
	CAtPhbkStoreInitialise* self = new (ELeave)CAtPhbkStoreInitialise(aGloblePhone, 
			                                                          aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreInitialise::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreInitialise* CAtPhbkStoreInitialise::NewL(CGlobalPhonemanager& aGloblePhone, 
                                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
    {
	CAtPhbkStoreInitialise* self=CAtPhbkStoreInitialise::NewLC(aGloblePhone, 
                                                               aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreInitialise::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreInitialise::ConstructL()
	{
	CAtCommandBase::ConstructL();
	//first is ADN 
	iStoreArr.Reset();
	iStoreArr.Append(KAdnStore());
//	//second is FDN
	iStoreArr.Append(KFdnStore());
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreInitialise::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreInitialise::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreInitialise::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreInitialise::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtPhbkStoreInitialise::ExecuteCommand called"));
	iState = ESetStoreName;
	iStoreIndex = 0;
	iTxBuffer.Zero();	
	iTxBuffer.Copy(iStoreArr[iStoreIndex]);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreInitialise::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreInitialise::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtPhbkStoreInitialise::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	
	if(aStatus != KErrNone)
		{
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhonebookStoreInitialiseComp(aStatus,iStoreInfo);
		}
	else
		{
		if( aEventSource == EWriteCompletion )
			{
			LOGTEXT(_L8("CAtPhbkStoreInitialise::EventSignal,EWriteCompletion!"));
			if( iState==ESetStoreName )
				{
				iState = ESetStoreNameComplete;
				}
			else if( iState==EGetStoreInfo )
				{
				iState = EGetStoreInfoComplete;
				}
			 }
		else 
			{
			LOGTEXT(_L8("CAtPhbkStoreInitialise::EventSignal,EReadCompletion!"));
			if( iState==ESetStoreNameComplete )
				{
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				if(iError == KErrNone )
					{
					iState = EGetStoreInfo;
					GetStoreInfo();
					}
				else
					{
					iCtsyDispatcherCallback.CallbackPhonebookStoreInitialiseComp(iError,iStoreInfo);
					}
				}
			else if( iState==EGetStoreInfoComplete )
				{
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				iStoreIndex++;
				//stands for complete all phonebook store type's at+CPBR=?
				if ( iStoreIndex > 1 )
					{
					iCtsyDispatcherCallback.CallbackPhonebookStoreInitialiseComp(iError,iStoreInfo);
					}
				else 
					{
					iState = ESetStoreName;
					iTxBuffer.Zero();
					//For this case,only two store type
					iTxBuffer.Copy(iStoreArr[iStoreIndex]);   
					Write();
					}
				}
			}  
		}
	}	

// ---------------------------------------------------------------------------
// CAtPhbkStoreInitialise::GetStoreInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreInitialise::GetStoreInfo()
	{
	LOGTEXT(_L8("CAtPhbkStoreInitialise::GetPhoneStoreInfo callded"));
	iPhoneGlobals.iEventSignalActive = ETrue;
	iTxBuffer.Zero();
	iTxBuffer.Copy(KGetSotreInfo);
	Write();   
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreInitialise::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreInitialise::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
	else if( iState == EGetStoreInfoComplete )
		{
		/**
		* Parse result of AT+CPBR=? 
		* Response:+CPMS:<(startIndex-endIndex>,<len1>,<len2>
		* len1 is maximum length for phone number
		* len2 is maximum length for text 
		*/
		iError = KErrNone;
		RArray<TPtrC8> array;
		CleanupClosePushL(array);
		iParser->ParseRespondedBuffer(array,Buffer());
		// remove the string of AT+CPBR=?
		if(array[0].MatchF(KAnyATCommand) != KErrNotFound)
			{
			array.Remove(0);
			}
		TInt count = array.Count();
#ifdef _DEBUG	
		for( TInt i=0;i<count;i++ )
			{
		    LOGTEXT3(_L8("CAtPhbkStoreInitialise::ParseResponseL\tarray[%d]=%S"),
		    		      i,&array[i]);     
			}
#endif		
		if (count <= 1)
			{
			CleanupStack::PopAndDestroy();
			iError = KErrNotFound;
			return;
			}
		if(array[0].MatchF(KCPBRResponseString) == KErrNotFound)
			{
			CleanupStack::PopAndDestroy();
			iError = KErrNotFound;
			return;
			}
		//parse iTotalEntries
		TInt len = array[1].Length();
		TInt pos = array[1].FindF(KSeparator);
		TPtrC8 tempStr;
		tempStr.Set(array[1].Left(pos));
		TLex8 lex(tempStr);
		TUint16 val;
		TInt ret = lex.Val(val,EDecimal);
		if(ret != KErrNone)
			{
			CleanupStack::PopAndDestroy();
			iError = ret;
			return;
			}
		tempStr.Set(array[1].Right(len-pos-1));
		TLex8 lex1(tempStr);
		TUint16 val1;
		ret = lex1.Val(val1,EDecimal);
		if(ret!=KErrNone)
			{
			CleanupStack::PopAndDestroy();
			iError = ret;
			return;
			}
	    iTotalEntries = val1-val+1;

		
	  	//parse MaximumNumberLength
		TLex8 lexNumberLen(array[2]);
		ret = lexNumberLen.Val(iValNumberLen,EDecimal);
		if(ret != KErrNone)
			{
			CleanupStack::PopAndDestroy();
			iError = ret;
			return;
			}
				
		//parse MaximumTextLength
		TLex8 lexTextLen(array[3]);
		ret = lexTextLen.Val(iValTextLen,EDecimal);
		if(ret != KErrNone)
			{
			CleanupStack::PopAndDestroy();
			iError = ret;
			return;
			}
		AssignPhbkType();
		LOGTEXT2(_L8("CAtPhbkStoreInitialise::ParseResponseL\tiTotalEntries=%d"),iTotalEntries);
	    LOGTEXT3(_L8("CAtPhbkStoreInitialise::ParseResponseL\tPreferred MaxNumbereLen=%d, MaxTextLen=%d"), 
	    		                                              iValNumberLen,iValTextLen);
		CleanupStack::PopAndDestroy(&array);

		}
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreInitialise::AssignPhbkType
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreInitialise::AssignPhbkType()
	{
	if( iStoreIndex == 0 )
		{
		iStoreInfo.iAdnTotalEntries = iTotalEntries;
		iPhoneGlobals.iPhonebookStoreInfo.iAdnTotalEntries = iTotalEntries;
		iStoreInfo.iAdnMaximumNumberLength = iValNumberLen;
		iStoreInfo.iAdnMaximumTextLength = iValTextLen;
		iPhoneGlobals.iPhonebookStoreInfo.iAdnMaximumNumberLength =
		                                    iValNumberLen;
		iPhoneGlobals.iPhonebookStoreInfo.iAdnMaximumTextLength =
		                                     iValTextLen;
		}
	else if( iStoreIndex == 1 )
		{
		iStoreInfo.iFdnTotalEntries = iTotalEntries;
		iPhoneGlobals.iPhonebookStoreInfo.iFdnTotalEntries = iTotalEntries;
		iStoreInfo.iFdnMaximumNumberLength = iValNumberLen;
		iStoreInfo.iFdnMaximumTextLength = iValTextLen;
		iPhoneGlobals.iPhonebookStoreInfo.iFdnMaximumNumberLength = 
											iValNumberLen;
		iPhoneGlobals.iPhonebookStoreInfo.iFdnMaximumTextLength =
											iValTextLen;    
		}
	}

//End of file
