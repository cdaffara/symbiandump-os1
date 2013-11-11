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
// The execution command is used to attach the MT to, or detach the MT from, the GPRS
// service. After the command has completed, the MT remains in V.25ter command state.
// If the MT is already in the requested state, the command is ignored and the OK
// response is returned. If the requested state cannot be achieved, an ERROR or +CME
// ERROR response is returned. Extended error responses are enabled by the +CMEE
// command.
// Any active PDP contexts will be automatically deactivated when the attachment state
// changes to detached.
// The read command returns the current GPRS service state.
// The test command is used for requesting information on the supported GPRS service
// states.
// +CGATT= [<state>]
// <state>: indicates the state of GPRS attachment
// 0 ?detached
// 1 ?attached
// Other values are reserved and will result in an ERROR response to the
// execution command.

#include "mslogger.h"
#include "atgprsdetach.h"

_LIT8(KDetachCommand, "AT+CGATT=0\r");

// ---------------------------------------------------------------------------
// CATGprsDetach::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsDetach* CATGprsDetach::NewL(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGprsDetach* self = new(ELeave) CATGprsDetach(aGloblePhone,
			                                        aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}

// ---------------------------------------------------------------------------
// CATGprsDetach::CATGprsDetach
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsDetach::CATGprsDetach(CGlobalPhonemanager& aGloblePhone, 
                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CATGprsDetach::ConstructL()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsDetach::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CATGprsDetach::~CATGprsDetach()
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsDetach::~CATGprsDetach()
	{
	}

// ---------------------------------------------------------------------------
// CATGprsDetach::StartRequest()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsDetach::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATGprsDetach::ExecuteCommand()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsDetach::ExecuteCommand()
	{
	iTxBuffer.Copy(KDetachCommand);
	Write();
	}

// ---------------------------------------------------------------------------
// CATGprsDetach::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsDetach::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
// CATGprsDetach::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsDetach::EventSignal(TAtEventSource aEventSource, TInt aStatus)
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
	iCtsyDispatcherCallback.CallbackPacketServicesPacketDetachComp(aStatus);
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	}

// End of file
