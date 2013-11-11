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
// @file atsmsstoredelete.cpp
// This contains CAtSmsStoreDelete and CAtSmsStoreDeleteAll which is used to delete message(s)
// 

//system include 
#include <ctsy/ltsy/cctsydispatchercallback.h>

//user include
#include "atsmsstoredelete.h"
#include "mslogger.h"

//constant defination
const TUint KSmsDeleteAllTimeOut = 200;
_LIT8(KSmsDeleteCommand,"AT+CMGD=%d\r");
_LIT8(KSmsDeleteAllCommand,"AT+CMGD=1,4\r");

// ---------------------------------------------------------------------------
// CAtSmsStoreDelete::CAtSmsStoreDelete
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreDelete::CAtSmsStoreDelete(CGlobalPhonemanager& aGloblePhone, 
                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                     :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtSmsStoreDelete::CAtSmsStoreDelete called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDelete::~CAtSmsStoreDelete
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreDelete::~CAtSmsStoreDelete()
	{
	LOGTEXT(_L8("CAtSmsStoreDelete::~CAtSmsStoreDelete called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDelete::NewLC(
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreDelete* CAtSmsStoreDelete::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                            CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsStoreDelete* self = new (ELeave)CAtSmsStoreDelete(aGloblePhone, 
                                                            aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDelete::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreDelete* CAtSmsStoreDelete::NewL(CGlobalPhonemanager& aGloblePhone, 
                                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsStoreDelete* self=CAtSmsStoreDelete::NewLC(aGloblePhone, 
                                                     aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDelete::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreDelete::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDelete::SetDelIndex
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreDelete::SetDelIndex(TInt aIndex)
	{
	LOGTEXT(_L8("CAtSmsStoreDelete::SetReadMessageParam callded"));
	iIndex = aIndex;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDelete::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreDelete::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDelete::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreDelete::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtSmsStoreDelete::ExecuteCommand callded"));
	iTxBuffer.Zero();
	iTxBuffer.Format(KSmsDeleteCommand,iIndex); // Delete Command through aIndex
	Write();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDelete::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreDelete::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtSmsStoreDelete::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	if(aStatus == KErrNone)
		{
		if((aEventSource == EWriteCompletion))
			{
			LOGTEXT(_L8("CAtSmsStoreDelete::EventSignal,EWriteCompletion!"));
			return;
			}
		aStatus = iError;
		}
	Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCtsyDispatcherCallback.CallbackPhonebookSmsStoreDeleteEntryComp(aStatus);
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDelete::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreDelete::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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

// End of CAtSmsStoreDelete 

// ---------------------------------------------------------------------------
// CAtSmsStoreDeleteAll::CAtSmsStoreDeleteAll
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreDeleteAll::CAtSmsStoreDeleteAll(CGlobalPhonemanager& aGloblePhone, 
										   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
										   :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtSmsStoreDeleteAll::CAtSmsStoreDeleteAll called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDeleteAll::~CAtSmsStoreDeleteAll
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreDeleteAll::~CAtSmsStoreDeleteAll()
	{
	LOGTEXT(_L8("CAtSmsStoreDeleteAll::~CAtSmsStoreDeleteAll called"));
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDeleteAll::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreDeleteAll* CAtSmsStoreDeleteAll::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                                  CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsStoreDeleteAll* self = new (ELeave)CAtSmsStoreDeleteAll(aGloblePhone, 
                                                                  aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDeleteAll::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsStoreDeleteAll* CAtSmsStoreDeleteAll::NewL(CGlobalPhonemanager& aGloblePhone, 
                                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsStoreDeleteAll* self=CAtSmsStoreDeleteAll::NewLC(aGloblePhone, 
                                                           aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDeleteAll::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreDeleteAll::ConstructL()
	{
	CAtCommandBase::ConstructL();
	//Set read and write timeout
	SetTimeOut(KLtsyDefaultWriteTimeOut, KSmsDeleteAllTimeOut);
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDeleteAll::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreDeleteAll::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDeleteAll::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreDeleteAll::ExecuteCommand()
	{
	LOGTEXT(_L8("CAtSmsStoreDeleteAll::ExecuteCommand callded"));
	iTxBuffer.Zero();
	iTxBuffer.Format(KSmsDeleteAllCommand); // Delete Command through aIndex
	Write();
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDeleteAll::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreDeleteAll::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT3(_L8("CAtSmsStoreDeleteAll::EventSignal aStatus=%D iSource=%D"),aStatus,aEventSource);
	if(aStatus == KErrNone)
		{
		if((aEventSource == EWriteCompletion))
			{
			LOGTEXT(_L8("CAtSmsStoreDeleteAll::EventSignal,EWriteCompletion!"));
			return;
			}
		aStatus = iError;
		}
	Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCtsyDispatcherCallback.CallbackPhonebookSmsStoreDeleteAllComp(aStatus);
	}

// ---------------------------------------------------------------------------
// CAtSmsStoreDeleteAll::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsStoreDeleteAll::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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

//End of file
