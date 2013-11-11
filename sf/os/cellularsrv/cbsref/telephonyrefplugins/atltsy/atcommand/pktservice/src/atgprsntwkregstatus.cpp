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
// The set command controls the presentation of an unsolicited result code +CGREG:
// <stat> when <n>=1 and there is a change in the MT's GPRS network registration
// status, or code +CGREG: <stat>[,<lac>,<ci>] when <n>=2 and there is a change of the
// network cell.
// The read command returns the status of result code presentation and an integer <stat>
// which shows whether the network has currently indicated the registration of the MT.
// Location information elements <lac> and <ci> are returned only when <n>=2 and MT
// is registered in the network.
// Defined values
// <n>:
//  0 disable network registration unsolicited result code
//  1 enable network registration unsolicited result code +CGREG: <stat>
//  2 enable network registration and location information unsolicited result code
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

#include "atgprsntwkregstatus.h"		// Header file for this source file
#include "mslogger.h"				// For LOGTEXT macros
#include "atgprsntwkregstatuschange.h"	

_LIT8(KATGprsNtwkRegStatusCommand, "AT+CGREG?\r");
_LIT8(KATGetGprsNetworkRegResponse,"+CGREG:");

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatus::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsNtwkRegStatus* CATGprsNtwkRegStatus::NewL(CGlobalPhonemanager& aGloblePhone, 
                                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback,
                                                 CATGprsNtwkRegStatusChange& aNtwkRegStatusChange)
	{
	CATGprsNtwkRegStatus* self = new(ELeave) CATGprsNtwkRegStatus(aGloblePhone,
			                                                      aCtsyDispatcherCallback,
			                                                      aNtwkRegStatusChange);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatus::CATGprsNtwkRegStatus
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsNtwkRegStatus::CATGprsNtwkRegStatus(CGlobalPhonemanager& aGloblePhone, 
                                           CCtsyDispatcherCallback& aCtsyDispatcherCallback,
                                           CATGprsNtwkRegStatusChange& aNtwkRegStatusChange)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback),
		iNtwkRegStatusChange(aNtwkRegStatusChange)
	{
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatus::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsNtwkRegStatus::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatus::~CATGprsNtwkRegStatus
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsNtwkRegStatus::~CATGprsNtwkRegStatus()
	{
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatus::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsNtwkRegStatus::StartRequest()
	{
	iNtwkRegStatusChange.Disable();
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatus::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsNtwkRegStatus::ExecuteCommand()
	{
	iTxBuffer.Copy(KATGprsNtwkRegStatusCommand);
	Write();
	}

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatus::ParseResponseL
// other items were commented in a header
//
// ---------------------------------------------------------------------------
void CATGprsNtwkRegStatus::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) != 0)
		{
		iError = KErrGeneral;
		return;
		}
	RArray<TPtrC8> array;
	CleanupClosePushL(array);
	iParser->ParseRespondedBuffer(array,PrecedingLine());
	TInt Count = array.Count();
	if (Count < 1)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrGeneral;
		return ;
		}
	if( array[0].MatchF(KATGetGprsNetworkRegResponse)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrGeneral;
		return ;
		}
	
	// Skip over one parameter
	TInt val = 0;
	TLex8 lex(array[2]);
	lex.Val(val);
	// Convert scond value to equivalent EtelMM enum
	switch(val)
		{
	case 0:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		iGprsRegistrationStatus = RPacketService::ENotRegisteredNotSearching;
		break;
	case 1:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		iGprsRegistrationStatus=RPacketService::ERegisteredOnHomeNetwork;
		break;
	case 2:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		iGprsRegistrationStatus=RPacketService::ENotRegisteredSearching;
		break;
	case 3:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		iGprsRegistrationStatus=RPacketService::ERegistrationDenied;
		break;
	case 4:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		iGprsRegistrationStatus=RPacketService::EUnknown;
		break;
	case 5:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		iGprsRegistrationStatus=RPacketService::ERegisteredRoaming;
		break;
	default:
		iGprsRegistrationStatus = RPacketService::EUnknown;
		break;
		}
	CleanupStack::PopAndDestroy(&array);
	iError = KErrNone;
	} //ParseResponseL

// ---------------------------------------------------------------------------
// CATGprsNtwkRegStatus::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsNtwkRegStatus::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	if(KErrNone ==aStatus)
		{
		if(aEventSource == EReadCompletion)
			{
			aStatus = iError;
			}
		else
			return;
		}
	iCtsyDispatcherCallback.CallbackPacketServicesGetPacketNetworkRegistrationStatusComp(aStatus,
					        iGprsRegistrationStatus);
	iNtwkRegStatusChange.Enable();
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	}

// End of file

