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
// CATGetModemStatus
// Description:
// CATNetworkRegStatusChange

//user include
#include "atnetworkregstatuschange.h"

_LIT8(KATGetNetworkRegResponse,"+CREG:*");

// ---------------------------------------------------------------------------
// CATNetworkRegStatusChange::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNetworkRegStatusChange* CATNetworkRegStatusChange::NewL(CGlobalPhonemanager& aGloblePhone,
		                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATNetworkRegStatusChange* self = CATNetworkRegStatusChange::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CATNetworkRegStatusChange::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNetworkRegStatusChange* CATNetworkRegStatusChange::NewLC(CGlobalPhonemanager& aGloblePhone,
		                              CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATNetworkRegStatusChange* self = new (ELeave) CATNetworkRegStatusChange(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CATNetworkRegStatusChange::~CATNetworkRegStatusChange
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNetworkRegStatusChange::~CATNetworkRegStatusChange()
	{
	
	}

// ---------------------------------------------------------------------------
// CATNetworkRegStatusChange::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkRegStatusChange::ConstructL()
	{
	CAtCommandBase::ConstructL();
	RemoveAllExpectString();
	AddExpectStringL(KATGetNetworkRegResponse);
	Enable();
	}

// ---------------------------------------------------------------------------
// CATNetworkRegStatusChange::CATNetworkRegStatusChange
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNetworkRegStatusChange::CATNetworkRegStatusChange(CGlobalPhonemanager& aGloblePhone,
		                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
							   :CAtCommandBase(aGloblePhone, aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATNetworkRegStatusChange::CATNetworkRegStatusChange
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkRegStatusChange::Enable()
	{
	AddUnsolicitedAtCommand();
	}
// ---------------------------------------------------------------------------
// CATNetworkRegStatusChange::CATNetworkRegStatusChange
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkRegStatusChange::Disable()
	{
	RemoveAtCommand();
	}
// ---------------------------------------------------------------------------
// CATNetworkRegStatusChange::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkRegStatusChange::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	
	RArray<TPtrC8> array;
	CleanupClosePushL(array);
	iParser->ParseRespondedBuffer(array,CurrentLine());
	ClearCurrentLine();
	TInt Count = array.Count();
	if (Count < 1)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return ;
		}
	if( array[0].MatchF(KATGetNetworkRegResponse)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return ;
		}
	// Skip over one parameter
	
	TInt val = 0;
	TLex8 lex(array[1]);
	lex.Val(val);
	// Convert scond value to equivalent EtelMM enum
	switch(val)
		{
		case 0:		
			iRegistrationStatus = RMobilePhone::ENotRegisteredNoService;
			break;
		case 1:		
			iRegistrationStatus = RMobilePhone::ERegisteredOnHomeNetwork;
			break;
		case 2:		
			iRegistrationStatus = RMobilePhone::ENotRegisteredSearching;
			break;
		case 3:		
			iRegistrationStatus = RMobilePhone::ERegistrationDenied;
			break;
		case 4:		
			iRegistrationStatus = RMobilePhone::ERegistrationUnknown;
			break;
		case 5:		
			iRegistrationStatus = RMobilePhone::ERegisteredRoaming;
			break;
		default:
			iRegistrationStatus = RMobilePhone::ERegistrationUnknown;
			break;
		}
	CleanupStack::PopAndDestroy(&array);
	iError = KErrNone;
	}
// ---------------------------------------------------------------------------
// CATNetworkRegStatusChange::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkRegStatusChange::EventSignal(TAtEventSource /*aEventSource*/, TInt /*aStatus*/)
	{
	iCtsyDispatcherCallback.CallbackPhoneNotifyNetworkRegistrationStatusChangeInd(iError,
				            iRegistrationStatus);
	}
//End of file
