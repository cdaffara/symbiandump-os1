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
// other items were commented in a header

#include "mslogger.h"
#include "atgprscontextdeactivate.h"

_LIT8(KATContextDeactivate,   "AT+CGACT=0,%d\r");
const TInt KDefaultDeactivateTimeout = 30;

// ---------------------------------------------------------------------------
// CATGprsContextDeactivate::NewL
//
// ---------------------------------------------------------------------------
CATGprsContextDeactivate* CATGprsContextDeactivate::NewL(CGlobalPhonemanager& aGloblePhone, 
                                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGprsContextDeactivate* self = new(ELeave) CATGprsContextDeactivate(aGloblePhone,
			                                                              aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}

// ---------------------------------------------------------------------------
// CATGprsContextDeactivate::CATGprsContextDeactivate
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsContextDeactivate::CATGprsContextDeactivate(CGlobalPhonemanager& aGloblePhone, 
                                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	iReadTimeOut = KDefaultDeactivateTimeout;
	}

// ---------------------------------------------------------------------------
// CATGprsContextDeactivate::ConstructL()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDeactivate::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Packet_DeactivateContext;
	}

// ---------------------------------------------------------------------------
// CATGprsContextDeactivate::~CATGprsContextDeactivate()
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsContextDeactivate::~CATGprsContextDeactivate()
	{
	}

// ---------------------------------------------------------------------------
// CATGprsContextDeactivate::StartRequest()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDeactivate::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATGprsContextDeactivate::ExecuteCommand()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDeactivate::ExecuteCommand()
	{
	LOGTEXT2(_L8("iCid=%d"),iContextId);
	// Send the AT+CGACT=0,x command to the phone
	iTxBuffer.Format(KATContextDeactivate,iContextId);
	Write();
	}

// ---------------------------------------------------------------------------
// CATGprsContextDeactivate::SetContext
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDeactivate::SetContext(const TInt aContextId)
	{
	iContextId = aContextId;
	}

// ---------------------------------------------------------------------------
// CATGprsContextDeactivate::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDeactivate::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
// CATGprsContextDeactivate::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDeactivate::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	if((KErrNone ==aStatus)&&(aEventSource == EWriteCompletion))
		{
		return;
		}
	TInt count = iPhoneGlobals.iContextList.Count();
	TInt i;
	//Get the context from context list
	for(i = 0; i < count;i++)
		{
		if(iPhoneGlobals.iContextList[i]->iContextID == iContextId)
			{
			break;
			}
		}
	if((KErrNone ==aStatus)&&(aEventSource == EReadCompletion)&&(iError==KErrNone))
		{
		iPhoneGlobals.iContextList[i]->iInfo.iStatus = RPacketContext::EStatusInactive;
		TContextMisc contextMisc;
		contextMisc.iStatus = RPacketContext::EStatusInactive;
		iPhoneGlobals.iPhoneStatus.iMode = RPhone::EModeOnlineCommand;
		iCtsyDispatcherCallback.CallbackPacketServicesNotifyPdpContextStatusChangeInd(iError,
				                iPhoneGlobals.iContextList[i]->iInfo.iName,
				                contextMisc);
		}
	iCtsyDispatcherCallback.CallbackPacketServicesDeactivatePdpContextComp(iError,
					        iPhoneGlobals.iContextList[i]->iInfo.iName);
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	}

// End of file
