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
// CATGprsAttach
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
// 0 – detached
// 1 – attached
// Other values are reserved and will result in an ERROR response to the
// execution command.
// other items were commented in a header
// 

#include "atgprsattach.h"
#include "mslogger.h"

_LIT8(KAttachCommand, "AT+CGATT=1\r");

// ---------------------------------------------------------------------------
// CATGprsAttach::NewL
// other items were commented in a header
// --------------------------------------------------------------------------
CATGprsAttach* CATGprsAttach::NewL(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGprsAttach* self = new(ELeave) CATGprsAttach(aGloblePhone,
			                                        aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}

// ---------------------------------------------------------------------------
// CATGprsAttach::CATGprsAttach
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsAttach::CATGprsAttach(CGlobalPhonemanager& aGloblePhone, 
                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CATGprsAttach::ConstructL()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsAttach::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CATGprsAttach::~CATGprsAttach()
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsAttach::~CATGprsAttach()
	{
	}

// ---------------------------------------------------------------------------
// CATGprsAttach::StartRequest()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsAttach::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATGprsAttach::ExecuteCommand()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsAttach::ExecuteCommand()
	{
	iTxBuffer.Copy(KAttachCommand);
	Write();
	}

// ---------------------------------------------------------------------------
// CATGprsAttach::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsAttach::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
// CATGprsAttach::EventSigna
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsAttach::EventSignal(TAtEventSource aEventSource, TInt aStatus)
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
	iCtsyDispatcherCallback.CallbackPacketServicesPacketAttachComp(aStatus);
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	}

//
// End of file
