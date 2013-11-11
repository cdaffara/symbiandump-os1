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
// CActivatContextAndGetAddr
// This command specifies PDP context parameter values for a PDP context identified 
// by the local context identification parameter, <cid>.
// Eleven PDP contexts can be defined through Wavecom software.
// A special form of the set command, +CGDCONT= <cid> causes the values for context
// number <cid> to become undefined.
// The test command returns values supported as a compound value. If the MT supports
// several PDP types, <PDP_type>, the parameter value ranges for each <PDP_type> are
// returned on a separate line.
// The read command returns the current settings for each defined context.
// The test command returns values supported as a compound value. If the MT supports
// several PDP types, <PDP_type), the parameter value ranges for each <PDP_type> are
// returned on a separate line.
// +CGDCONT=[<cid> [,<PDP_type> [,<APN>[,<PDP_addr> [,<d_comp>[,<h_comp>]]]]]]
// Defined values
// <cid>: (PDP Context Identifier) a numeric parameter (1-32) which specifies a particular
// PDP context definition. The parameter is local to the TE-MT interface and is used in
// other PDP context-related commands.
// <PDP_type>: (Packet Data Protocol type) a string parameter which specifies the type of
// packet data protocol
// IP Internet Protocol
// PPP Point to Point Protocol
// <APN>: (Access Point Name) a string parameter, which is a logical name that is used to
// select the GGSN or the external packet data network.
// If the value is null or omitted, then the subscription value will be requested.
// WAVECOM confidential ?
// This confidential document is the property of WAVECOM and may not be copied or circulated
// without permission
// <PDP_address>: a string parameter that identifies the MT in the address space
// applicable to the PDP.
// If the value is null or omitted, then a value may be provided by the TE during the PDP
// startup procedure or, failing that, a dynamic address will be requested.
// The read form of the command will continue to return the null string even if an address
// has been allocated during the PDP startup procedure. The allocated address may be
// read using the +CGPADDR command.
// <d_comp>: a numeric parameter that controls PDP data compression
// 0 - off (default if value is omitted)
// 1 ?on
// Other values are reserved.
// <h_comp>: a numeric parameter that controls PDP header compression
// 0 - off (default if value is omitted)
// 1 ?on
// Other values are reserved.
// other items were commented in a header

#include "mslogger.h"
#include "atgprsconfig.h"

_LIT8(KIPType4	, "IP");
_LIT8(KIPType6	, "IP");
_LIT8(KX25		, "X25");
_LIT8(KSetCGDCONTCommand,"AT+CGDCONT=%d,\"%S\",\"%S\"\r");
_LIT8(KATDeleteContext1, "AT+CGDCONT=1\r");

// ---------------------------------------------------------------------------
// CATGPRSSetConfig::NewL
//
// ---------------------------------------------------------------------------
CATGPRSSetConfig* CATGPRSSetConfig::NewL(CGlobalPhonemanager& aGloblePhone, 
                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGPRSSetConfig* self = new(ELeave) CATGPRSSetConfig(aGloblePhone,aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}

// ---------------------------------------------------------------------------
// CATGPRSSetConfig::CATGPRSSetConfig
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGPRSSetConfig::CATGPRSSetConfig(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CATGPRSSetConfig::ConstructL()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGPRSSetConfig::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iDeleteContext = EFalse;
	}

// ---------------------------------------------------------------------------
// CATGPRSSetConfig::~CATGPRSSetConfig()
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGPRSSetConfig::~CATGPRSSetConfig()
	{
	}

// ---------------------------------------------------------------------------
// CATGPRSSetConfig::StartRequest()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGPRSSetConfig::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATGPRSSetConfig::ExecuteCommand()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGPRSSetConfig::ExecuteCommand()
	{
	iDeleteContext = ETrue;
	iTxBuffer.Copy(KATDeleteContext1);
	Write();
	}

// ---------------------------------------------------------------------------
// CATGPRSSetConfig::MakeupCGDCONT()
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGPRSSetConfig::MakeupCGDCONT()
	{
	TInt count = iPhoneGlobals.iContextList.Count();
	TInt i;
	//Get the context location from context list
	for(i = 0; i < count;i++)
		{
		if(iPhoneGlobals.iContextList[i]->iContextID == iContextId)
			{
			break;
			}
		}
	const TInt KPDPTypeIdent=5;
	TBuf8<KPDPTypeIdent>						pdpType;		// PDP Type identifier
	TBuf8<RPacketContext::KGSNNameLength>		gsnName;			// Access point Name
	TBuf8<RPacketContext::KMaxPDPAddressLength>	pdpAddress;	// PDP pre-assigned address
	switch(iPhoneGlobals.iContextList[i]->iDefContextConfigGPRS.iPdpType)
		{
		case RPacketContext::EPdpTypeIPv4:
			pdpType.Format(KIPType4);
			break;
		case RPacketContext::EPdpTypeIPv6:
			pdpType.Format(KIPType6);
			break;
		case RPacketContext::EPdpTypeX25:
			pdpType.Format(KX25);
			break;
		default:
			pdpType.Format(KIPType6);
			break;
		}

	gsnName.Copy(iPhoneGlobals.iContextList[i]->iDefContextConfigGPRS.iAccessPointName);
	pdpAddress.Copy(iPhoneGlobals.iContextList[i]->iDefContextConfigGPRS.iPdpAddress);

	iTxBuffer.Format(KSetCGDCONTCommand, iContextId, &pdpType, &gsnName);

	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CATGPRSSetConfig::SetContext
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGPRSSetConfig::SetContext(const TInt aContextId)
	{
	iContextId = aContextId;
	}

// ---------------------------------------------------------------------------
// CATGPRSSetConfig::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGPRSSetConfig::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
// CATGPRSSetConfig::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGPRSSetConfig::EventSignal(TAtEventSource aEventSource, TInt aStatus)
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
	if(iDeleteContext)
		{
		iDeleteContext = EFalse;
		TInt ret=MakeupCGDCONT();
		Write();
		}
	else
		{
		TInt count = iPhoneGlobals.iContextList.Count();
		TInt i;
		//Get the context location from context list
		for(i = 0; i < count;i++)
			{
			if(iPhoneGlobals.iContextList[i]->iContextID == iContextId)
				{
				break;
				}
			}
		iCtsyDispatcherCallback.CallbackPacketServicesSetPdpContextConfigComp(aStatus,
						        iPhoneGlobals.iContextList[i]->iInfo.iName);
        iCtsyDispatcherCallback.CallbackPacketServicesNotifyPdpContextAddedInd(aStatus,
                                iPhoneGlobals.iContextList[i]->iInfo.iName);
		CAtCommandBase::Complete();
		iPhoneGlobals.iEventSignalActive = EFalse;
		}
	}

//
// End of file
