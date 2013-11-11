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
// The execution command returns a list of PDP addresses for the specified context
// identifiers.
// The test command returns a list of defined <cid>s
// Defined values
// <cid>: a numeric parameter which specifies a particular PDP context definition. 
// If no <cid> is specified, the addresses for all defined contexts are returned.
// <PDP_address>: a string that identifies the MT in the address space applicable 
// to the PDP. The address may be static or dynamic. For a static address, it will 
// be the one set by the +CGDCONT command when the context was defined. For a 
// dynamic address it will be the one assigned during the last PDP context 
// activation that used the context definition referred to by <cid>. 
// <PDP_address> is omitted if none is available.

#include "mslogger.h"
#include "atgprsgetcontextaddr.h"

_LIT8(KATGetContextAddress,    "AT+CGPADDR=%d\r");
_LIT8(KATGetContextAddressStr, "+CGPADDR:");

// ---------------------------------------------------------------------------
// CATGPRSGetContextAddr::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGPRSGetContextAddr* CATGPRSGetContextAddr::NewL(CGlobalPhonemanager& aGloblePhone, 
                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGPRSGetContextAddr* self = new(ELeave) CATGPRSGetContextAddr(aGloblePhone,aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}

// ---------------------------------------------------------------------------
// CATGPRSGetContextAddr::CATGPRSGetContextAddr
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGPRSGetContextAddr::CATGPRSGetContextAddr(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CATGPRSGetContextAddr::ConstructL()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGPRSGetContextAddr::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Packet_GetContextAddr;
	}

// ---------------------------------------------------------------------------
// CATGPRSGetContextAddr::~CATGPRSGetContextAddr()
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGPRSGetContextAddr::~CATGPRSGetContextAddr()
	{
	}

// ---------------------------------------------------------------------------
// CATGPRSGetContextAddr::StartRequest()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGPRSGetContextAddr::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATGPRSGetContextAddr::ExecuteCommand()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGPRSGetContextAddr::ExecuteCommand()
	{
	LOGTEXT(_L8("CATGPRSGetContextAddr::Start called"));
	LOGTEXT2(_L8("iCid=%d"),iContextId);
	iTxBuffer.Format(KATGetContextAddress,iContextId);
	Write();
	}

// ---------------------------------------------------------------------------
// CATGPRSGetContextAddr::SetContext
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGPRSGetContextAddr::SetContext(const TInt aContextId)
	{
	iContextId = aContextId;
	}

// ---------------------------------------------------------------------------
// CATGPRSGetContextAddr::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGPRSGetContextAddr::GetErrorValue()
	{
	return iError;
	}

// ---------------------------------------------------------------------------
// CATGPRSGetContextAddr::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGPRSGetContextAddr::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
	if( array[0].MatchF(KATGetContextAddressStr)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrGeneral;
		return ;
		}
	TInt count = iPhoneGlobals.iContextList.Count();
	TInt i = 0;
	//Get the context from context list
	for(i = 0; i < count;i++)
		{
		if(iPhoneGlobals.iContextList[i]->iContextID == iContextId)
			{
			break;
			}
		}
	if(Count > 2 )
		{
		iPhoneGlobals.iContextList[i]->iDefContextConfigGPRS.iPdpAddress.Copy(array[2]);
		iError = KErrNone;
		}
	else
		{
		iError = KErrGeneral;
		}
	CleanupStack::PopAndDestroy(&array);
	}

// End of file
