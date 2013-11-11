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
// @file atphbkstoredelete.cpp
// This contains CAtPhbkStoreDelete which is used to delete a phonebook Entry
// 

//system include 
#include <etelmm.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>

//user include 
#include "atphbkstoredelete.h"
#include "mslogger.h"

//constant defination
_LIT8(KPhbkStoreDelete,"AT+CPBW=%d\r");
_LIT8(KSelAdnPhonebook,"AT+CPBS=\"SM\"\r");
_LIT8(KSelFdnPhonebook,"AT+CPBS=\"FD\"\r");

// ---------------------------------------------------------------------------
// CAtPhbkStoreDelete::CAtPhbkStoreDelete
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreDelete::CAtPhbkStoreDelete(CGlobalPhonemanager& aGloblePhone, 
                                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                       :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtPhbkStoreDelete::CAtPhbkStoreDelete called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreDelete::~CAtPhbkStoreDelete
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreDelete::~CAtPhbkStoreDelete()
	{
	LOGTEXT(_L8("CAtPhbkStoreDelete::~CAtPhbkStoreDelete called"));
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreDelete::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreDelete* CAtPhbkStoreDelete::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                              CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkStoreDelete* self = new (ELeave)CAtPhbkStoreDelete(aGloblePhone, 
                                                              aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreDelete::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtPhbkStoreDelete* CAtPhbkStoreDelete::NewL(CGlobalPhonemanager& aGloblePhone, 
                                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtPhbkStoreDelete* self=CAtPhbkStoreDelete::NewLC(aGloblePhone, 
                                                       aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreDelete::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreDelete::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreDelete::SetPhonebookType
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreDelete::SetPhonebookType(DispatcherPhonebook::TPhonebook aPhonebookType)
	{
	iPhoneBookType = aPhonebookType;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreDelete::SetDeleteIndex
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreDelete::SetDeleteIndex(TInt aIndex)
	{
	iIndex = aIndex;
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreDelete::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreDelete::StartRequest()
	{
	GetMaxNumberLen();
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreDelete::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreDelete::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtPhbkStoreDelete::ExecuteCommand called"));
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
// CAtPhbkStoreDelete::DeleteEntry
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreDelete::DeleteEntry()
	{
	LOGTEXT(_L8("CAtPhbkStoreDelete::DeleteEntry callded"));
	iPhoneGlobals.iEventSignalActive = ETrue;
	iTxBuffer.Zero();
	iTxBuffer.Format(KPhbkStoreDelete,iIndex);
	Write();
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreDelete::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreDelete::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtPhbkStoreDelete::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	
	if(aStatus != KErrNone)
		{
		Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhonebookStoreDeleteEntryComp(aStatus,iPhoneBookType,iMaxLen);
		}
	else
		{
		if( aEventSource== EWriteCompletion )
			{
			if( iState==ESetStoreName )
				{
				iState = ESetStoreNameComplete;
				}
			else if( iState==EDeleteEntry )
				{
				iState = EDeleteEntryComplete;
				}
			}
		else 
			{
			if( iState==ESetStoreNameComplete )
				{
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				if( iError == KErrNone )
					{
					iState = EDeleteEntry;
					DeleteEntry();
					}
				else
					{
					iCtsyDispatcherCallback.CallbackPhonebookStoreDeleteEntryComp(iError,iPhoneBookType,iMaxLen);
					}
				}
			else if( iState== EDeleteEntryComplete)
				{
				Complete();
				iPhoneGlobals.iEventSignalActive = EFalse;
				iCtsyDispatcherCallback.CallbackPhonebookStoreDeleteEntryComp(iError,iPhoneBookType,iMaxLen);
				}
			}
		}
	}

// ---------------------------------------------------------------------------
// CAtPhbkStoreDelete::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreDelete::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
// CAtPhbkStoreDelete::GetMaxNumberLen
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtPhbkStoreDelete::GetMaxNumberLen()
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
