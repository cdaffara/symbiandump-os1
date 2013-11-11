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
// @file atphbkstorewrite.cpp
// This contains CAtPhbkStoreWrite which is use to write a phonebook Entry
// 

//system include 
#include <etelmm.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mpbutil.h> 

//user include 
#include "atphbkstorewrite.h"
#include "mslogger.h"

//constant defination
_LIT8(KPhbkStoreWrite,"AT+CPBW=%d,\"%S\",145,\"%S\"\r");
_LIT8(KSelAdnPhonebook,"AT+CPBS=\"SM\"\r");
_LIT8(KSelFdnPhonebook,"AT+CPBS=\"FD\"\r");
const TInt KMaxNumberLen = 20;
const TInt KMaxTextLen = 14;

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::CAtPhbkStoreWrite
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreWrite::CAtPhbkStoreWrite(CGlobalPhonemanager& aGloblePhone, 
                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                     :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtPhbkStoreWrite::CAtPhbkStoreWrite called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::~CAtPhbkStoreWrite
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreWrite::~CAtPhbkStoreWrite()
	{
	LOGTEXT(_L8("CAtPhbkStoreWrite::~CAtPhbkStoreWrite called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreWrite* CAtPhbkStoreWrite::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                            CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkStoreWrite* self = new (ELeave)CAtPhbkStoreWrite(aGloblePhone, 
                                                            aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreWrite* CAtPhbkStoreWrite::NewL(CGlobalPhonemanager& aGloblePhone, 
                                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkStoreWrite* self=CAtPhbkStoreWrite::NewLC(aGloblePhone, 
                                                     aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreWrite::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::SetPhonebookType
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreWrite::SetPhonebookType(DispatcherPhonebook::TPhonebook aPhonebookType)
	{
	iPhoneBookType = aPhonebookType;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::SetPhonebookEntry
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreWrite::SetPhonebookEntryL(const TDesC8& aEntry)
	{
	TInt ret = KErrNone;
	TInt retVal = KErrNone;
	TUint8 tmpTagValue;
	TPtrC16 textPtr;
    TUint8 tagValue;
    CPhoneBookBuffer::TPhBkTagType tagType;
    TUint8 newEntryTag;
    newEntryTag = RMobilePhoneBookStore::ETagPBNewEntry;
    TUint16 index;
    TPtrC16 numPtr;
    TPtrC16 type;
    // used to write phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); 
	CleanupStack::PushL(pbBuffer);
	
    //  Set pointer to constant descriptor
    TPtr8 dataToSeparate( NULL, 0, 0 );
    dataToSeparate.Set( (TUint8*)aEntry.Ptr(),
    		aEntry.Length(), aEntry.Length() );
    
    //  Init and get entry info
    pbBuffer->Set( &dataToSeparate );
    pbBuffer->StartRead();
    ret = pbBuffer->GetTagAndType( tmpTagValue, tagType );
    // Ensure, that there is new entry containing data
    if ( KErrNone == ret && tmpTagValue == newEntryTag )
        {
        // Get the first value from TLV data
        ret = pbBuffer->GetTagAndType( tagValue, tagType );
        while(ret == KErrNone)
			{	
			switch ( tagValue )
		        {
		        // Get name
		        case RMobilePhoneBookStore::ETagPBText:
		        	retVal = pbBuffer->GetValue( textPtr );
		            if(retVal == KErrNone)
		                {
		                iPhbkEntry.iText = textPtr;
		            	}
		            break;
		         // Get number
		        case RMobilePhoneBookStore::ETagPBNumber:
		            ret = pbBuffer->GetValue( numPtr );
		            if(retVal == KErrNone)
		            	{
		            	iPhbkEntry.iNumber = numPtr;
		            	}
		            break;
		         // get the index
		        case RMobilePhoneBookStore::ETagPBAdnIndex:
		        	retVal = pbBuffer->GetValue(index);
		            if( retVal == KErrNone )
		            	{
		            	iPhbkEntry.iIndex = index;
		            	}
		            break;
		         // get ETagPBTonNpi , not needed
		        case RMobilePhoneBookStore::ETagPBTonNpi:
		        	retVal = pbBuffer->GetValue(type);
		            break;      
		        default:
		        	break;
		        }//switch 
			ret = pbBuffer->GetTagAndType( tagValue, tagType );
		    }//while
 		}
	CleanupStack::PopAndDestroy(pbBuffer);
	}	

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreWrite::StartRequest()
	{
	GetMaxNumberLen();
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreWrite::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtPhbkStoreWrite::ExecuteCommand called"));
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
// CAtPhbkStoreWrite::WriteEntry
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreWrite::WriteEntry()
	{
	LOGTEXT(_L8("CAtPhbkStoreWrite::WriteEntry callded"));
	iPhoneGlobals.iEventSignalActive = ETrue;
	iTxBuffer.Zero();
	TBuf8<KMaxNumberLen> tempNumber;
	TBuf8<KMaxTextLen>   tempText;
	//covert 16 bit to 8 bit
	tempNumber.Copy(iPhbkEntry.iNumber);
	tempText.Copy(iPhbkEntry.iText);
	iTxBuffer.Format(KPhbkStoreWrite,iPhbkEntry.iIndex,
			         &tempNumber,&tempText);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreWrite::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtPhbkStoreWrite::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	
	if(aStatus != KErrNone)
		{
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhonebookStoreWriteEntryComp(aStatus,iPhoneBookType,iPhbkEntry.iIndex,iMaxLen);
		}
	else
		{
		if( aEventSource== EWriteCompletion )
			{
			if( iState==ESetStoreName )
				{
				iState = ESetStoreNameComplete;
				}
			else if( iState==EWriteEntry )
				{
				iState = EWriteEntryComplete;
				}
			}
		else 
			{
			if( iState==ESetStoreNameComplete )
				{
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				if( iError==KErrNone )
					{
					iState = EWriteEntry;
					WriteEntry();
					}
				else
					{
					iCtsyDispatcherCallback.CallbackPhonebookStoreWriteEntryComp(iError,iPhoneBookType,
																				 iPhbkEntry.iIndex,iMaxLen);
					}
				}
			else if( iState== EWriteEntryComplete)
				{
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				iCtsyDispatcherCallback.CallbackPhonebookStoreWriteEntryComp(iError,iPhoneBookType,
															                 iPhbkEntry.iIndex,iMaxLen);
				}
			}
		}
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreWrite::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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

// ---------------------------------------------------------------------------
// CAtPhbkStoreWrite::GetMaxNumberLen
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreWrite::GetMaxNumberLen()
	{
	if( iPhoneBookType == DispatcherPhonebook::EIccAdn )
		{
		iMaxLen = iPhoneGlobals.iPhonebookStoreInfo.iAdnMaximumNumberLength;
		}
	else if( iPhoneBookType == DispatcherPhonebook::EIccFdn )
		{
		iMaxLen = iPhoneGlobals.iPhonebookStoreInfo.iFdnMaximumNumberLength;
		}
	}

//End of file
