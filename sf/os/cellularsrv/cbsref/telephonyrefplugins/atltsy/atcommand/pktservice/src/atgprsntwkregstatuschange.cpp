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
// +CGREG: <stat>[,<lac>,<ci>]
// <stat>:
//  0 not registered, ME is not currently searching a new operator to register to
//  1 registered, home network
//  2 not registered, but ME is currently searching a new operator to register to
//  3 registration denied
//  4 unknown
//  5 registered, roaming
// <lac>:
//  string type; two byte location area code in hexadecimal format.
// <ci>:
//  string type; two byte cell ID in hexadecimal format

#include <et_phone.h>		// For CTelObject
#include "atgprsntwkregstatuschange.h"		// Header file for this source file
#include "mslogger.h"					// For LOGTEXT macros

_LIT8(KATGetGprsNtwkRegResponse,"+CGREG:*");

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatusChange::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsNtwkRegStatusChange* CATGprsNtwkRegStatusChange::NewL(CGlobalPhonemanager& aGloblePhone,
		                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGprsNtwkRegStatusChange* self = CATGprsNtwkRegStatusChange::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatusChange::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsNtwkRegStatusChange* CATGprsNtwkRegStatusChange::NewLC(CGlobalPhonemanager& aGloblePhone,
		                              CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGprsNtwkRegStatusChange* self = new (ELeave) CATGprsNtwkRegStatusChange(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatusChange::~CATGprsNtwkRegStatusChange
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsNtwkRegStatusChange::~CATGprsNtwkRegStatusChange()
	{
	
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatusChange::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsNtwkRegStatusChange::ConstructL()
	{
	CAtCommandBase::ConstructL();
	RemoveAllExpectString();
	AddExpectStringL(KATGetGprsNtwkRegResponse);
	Enable();
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatusChange::CATGprsNtwkRegStatusChange
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsNtwkRegStatusChange::CATGprsNtwkRegStatusChange(CGlobalPhonemanager& aGloblePhone,
		                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
							   :CAtCommandBase(aGloblePhone, aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatusChange::CATGprsNtwkRegStatusChange
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsNtwkRegStatusChange::Enable()
	{
	AddUnsolicitedAtCommand();
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatusChange::CATGprsNtwkRegStatusChange
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsNtwkRegStatusChange::Disable()
	{
	RemoveAtCommand();
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatusChange::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsNtwkRegStatusChange::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
	if( array[0].MatchF(KATGetGprsNtwkRegResponse)==KErrNotFound)
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
		case 0:		// ETSI value as defined in ETSI 07.07 section 10.1.14
			iRegistrationStatus = RPacketService::ENotRegisteredNotSearching;
			break;
		case 1:		// ETSI value as defined in ETSI 07.07 section 10.1.14
			iRegistrationStatus=RPacketService::ERegisteredOnHomeNetwork;
			break;
		case 2:		// ETSI value as defined in ETSI 07.07 section 10.1.14
			iRegistrationStatus=RPacketService::ENotRegisteredSearching;
			break;
		case 3:		// ETSI value as defined in ETSI 07.07 section 10.1.14
			iRegistrationStatus=RPacketService::ERegistrationDenied;
			break;
		case 4:		// ETSI value as defined in ETSI 07.07 section 10.1.14
			iRegistrationStatus=RPacketService::EUnknown;
			break;
		case 5:		// ETSI value as defined in ETSI 07.07 section 10.1.14
			iRegistrationStatus=RPacketService::ERegisteredRoaming;
			break;
		default:
			iRegistrationStatus = RPacketService::EUnknown;
			break;
		}
	CleanupStack::PopAndDestroy(&array);
	iError = KErrNone;
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatusChange::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsNtwkRegStatusChange::EventSignal(TAtEventSource /*aEventSource*/, TInt /*aStatus*/)
	{
	iCtsyDispatcherCallback.CallbackPacketServicesNotifyPacketNetworkRegistrationStatusInd(iError,
				            iRegistrationStatus);
	}

//End file
