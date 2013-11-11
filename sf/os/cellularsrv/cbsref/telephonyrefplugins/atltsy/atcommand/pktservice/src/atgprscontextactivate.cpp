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
// The execution command is used to activate or deactivate the specified PDP context (s).
// After the command has completed, the MT remains in V.25ter command state. If any
// PDP context is already in the requested state, the state for that context remains
// unchanged.
// If the requested state for any specified context cannot be achieved, an ERROR or +CME
// ERROR response is returned. Extended error responses are enabled by the +CMEE
// command.
// If the MT is not GPRS attached when the activation form of the command is executed,
// the MT first performs a GPRS attach and them attempts to activate the specified
// contexts. If the attach fails then the MT responds with ERROR or, if extended error
// responses are enabled, with the appropriate failure-to-attach error message.
// If no <cid>s are specified the activation form of the command activates all defined
// contexts.
// If no <cid>s are specified the deactivation form of the command deactivates all active
// contexts.
// Two PDP contexts can be activated through Wavecom software.
// The read command returns the current activation states for all the defined PDP
// contexts.
// The test command is used for requesting information on the supported PDP context
// activation states.
// Defined Values
// <state>: indicates the state of PDP context activation
//  0 ?deactivated
//  1 ?activated
//  Other values are reserved and will result in an ERROR response to the
// execution command.
// <cid>: a numeric parameter which specifies a particular PDP context.
//  other items were commented in a header

#include "mslogger.h"
#include "atgprscontextactivate.h"

_LIT8(KATContextActivate,   "AT+CGACT=1,%d\r");
const TInt KLtsyGprsContextActivateTimeOut = 30;

// ---------------------------------------------------------------------------
// CATGprsContextActivate::NewL
//
// ---------------------------------------------------------------------------
CATGprsContextActivate* CATGprsContextActivate::NewL(CGlobalPhonemanager& aGloblePhone, 
                                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGprsContextActivate* self = new(ELeave) CATGprsContextActivate(aGloblePhone,
			                                                          aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}

// ---------------------------------------------------------------------------
// CATGprsContextActivate::CATGprsContextActivate
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsContextActivate::CATGprsContextActivate(CGlobalPhonemanager& aGloblePhone, 
                                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CATGprsContextActivate::ConstructL()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextActivate::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Packet_ActivateContext;
	iReadTimeOut = KLtsyGprsContextActivateTimeOut;
	}

// ---------------------------------------------------------------------------
// CATGprsContextActivate::~CATGprsContextActivate()
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsContextActivate::~CATGprsContextActivate()
	{
	}

// ---------------------------------------------------------------------------
// CATGprsContextActivate::StartRequest()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextActivate::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATGprsContextActivate::ExecuteCommand()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextActivate::ExecuteCommand()
	{
	LOGTEXT(_L8("CATGprsContextActivate::Start called"));
	LOGTEXT2(_L8("iCid=%d"),iContextId);
	iTxBuffer.Format(KATContextActivate,iContextId);
	Write();
	}

// ---------------------------------------------------------------------------
// CATGprsContextActivate::SetContext
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextActivate::SetContext(const TInt aContextId)
	{
	iContextId = aContextId;
	}

// ---------------------------------------------------------------------------
// CATGprsContextActivate::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGprsContextActivate::GetErrorValue()
	{
	return iError;
	}

// ---------------------------------------------------------------------------
// CATGprsContextActivate::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextActivate::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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

// End of file
