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
#include "atgprscontextconnect.h"

_LIT8(KATContextConnect,   "ATD*99***%d#\r");
_LIT8(KLtsyGprsNoConnect, "NO CARRIER");
_LIT8(KLtsyGprsConnectStr, "CONNECT*");
_LIT8(KLtsyGprsConnect, "CONNECT");

// ---------------------------------------------------------------------------
// CATGprsContextConnect::NewL
//
// ---------------------------------------------------------------------------
CATGprsContextConnect* CATGprsContextConnect::NewL(CGlobalPhonemanager& aGloblePhone, 
                                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGprsContextConnect* self = new(ELeave) CATGprsContextConnect(aGloblePhone,
			                                                          aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}

// ---------------------------------------------------------------------------
// CATGprsContextConnect::CATGprsContextConnect
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsContextConnect::CATGprsContextConnect(CGlobalPhonemanager& aGloblePhone, 
                                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CATGprsContextConnect::ConstructL()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextConnect::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Packet_ConnectContext;
	AddExpectStringL(KLtsyGprsConnectStr);
	AddExpectStringL(KLtsyGprsNoConnect);
	iReadTimeOut = 30;
	}

// ---------------------------------------------------------------------------
// CATGprsContextConnect::~CATGprsContextConnect()
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsContextConnect::~CATGprsContextConnect()
	{
	}

// ---------------------------------------------------------------------------
// CATGprsContextConnect::StartRequest()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextConnect::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATGprsContextConnect::ExecuteCommand()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextConnect::ExecuteCommand()
	{
	LOGTEXT(_L8("CATGprsContextConnect::Start called"));
	LOGTEXT2(_L8("iCid=%d"),iContextId);
	iTxBuffer.Format(KATContextConnect,iContextId);
	Write();
	}

// ---------------------------------------------------------------------------
// CATGprsContextConnect::SetContext
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextConnect::SetContext(const TInt aContextId)
	{
	iContextId = aContextId;
	}

// ---------------------------------------------------------------------------
// CATGprsContextConnect::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGprsContextConnect::GetErrorValue()
	{
	return iError;
	}

// ---------------------------------------------------------------------------
// CATGprsContextConnect::GetConnectionSpeed
// other items were commented in a header
// ---------------------------------------------------------------------------
TUint CATGprsContextConnect::GetConnectionSpeed()
	{
	return iConnectionSpeed;
	}

// ---------------------------------------------------------------------------
// CATGprsContextConnect::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextConnect::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	RArray<TPtrC8> array;
	CleanupClosePushL(array);
	iParser->ParseRespondedBuffer(array,CurrentLine());
	TInt Count = array.Count();
	if (Count < 1)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrGeneral;
		return ;
		}
	if( array[0].MatchF(KLtsyGprsConnect)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrGeneral;
		return ;
		}
	else
		{
		TUint val = 0;
		TLex8 lex(array[1]);
		lex.Val(val);
		iConnectionSpeed = val;
		CleanupStack::PopAndDestroy(&array);
		iError = KErrNone;
		}
	}

// End of file
