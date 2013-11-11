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
// CATNetworkRegistration

#include <et_phone.h>

#include "atgetnetworkregistrationstatus.h"
#include "mslogger.h"
#include "atnetworkregstatuschange.h"


_LIT8(KATGetNetworkRegCommand, "AT+CREG?\r");
_LIT8(KATGetNetworkRegResponse,"+CREG:");
// Class CATNetworkRegistration
// ---------------------------------------------------------------------------
// CATNetworkRegistration::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNetworkRegistration* CATNetworkRegistration::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATNetworkRegistration* self = new(ELeave) CATNetworkRegistration(aGloblePhone,
			                                                          aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATNetworkRegistration::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNetworkRegistration::CATNetworkRegistration(CGlobalPhonemanager& aGloblePhone, 
	                                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATNetworkRegistration::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkRegistration::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Phone_NwkRegStatus;
	}
// ---------------------------------------------------------------------------
// CATNetworkRegistration::~CATNetworkRegistration
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNetworkRegistration::~CATNetworkRegistration()
	{
	}
// ---------------------------------------------------------------------------
// CATNetworkRegistration::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkRegistration::StartRequest()
	{
	ExecuteCommand();	
	}
// ---------------------------------------------------------------------------
// CATNetworkRegistration::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkRegistration::ExecuteCommand()
	{
	iTxBuffer.Copy(KATGetNetworkRegCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATNetworkRegistration::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATNetworkRegistration::GetErrorValue()
	{
	return iError;
	}
/**
 * This method parses the modems response to the 'AT+CGREG?' command.
 * An example response is '+CGREG: 0,1' where second value denotes the
 * current registration status.
 */
// ---------------------------------------------------------------------------
// CATNetworkRegistration::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkRegistration::ParseResponseL(const TDesC8& /*aResponseBuf*/)
		{
		if (CurrentLine().Match(KLtsyOkString) != 0)
			{
			iError = KErrGeneral;
			return ;
			}
		RArray<TPtrC8> array;
		CleanupClosePushL(array);
		iParser->ParseRespondedBuffer(array,PrecedingLine());
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
		TLex8 lex(array[2]);
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
		iPhoneGlobals.iPhoneStatus.iRegistrationStatus = iRegistrationStatus;
		CleanupStack::PopAndDestroy(&array);
		iError = KErrNone;
	} //ParseResponseL
// ---------------------------------------------------------------------------
// CATNetworkRegistration::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkRegistration::EventSignal(TAtEventSource aEventSource, TInt aStatus)
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
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCtsyDispatcherCallback.CallbackPhoneGetNetworkRegistrationStatusComp(aStatus,iRegistrationStatus);
	if(iIpcCompleteObserver)
        {
        iIpcCompleteObserver->NotifyIpcRequestComplete(iIpcId);
        }
	}
//
// End of file
