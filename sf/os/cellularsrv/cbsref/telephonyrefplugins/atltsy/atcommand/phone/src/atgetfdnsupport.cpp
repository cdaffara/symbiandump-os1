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
// CATGetFdnSupport

//user include
#include "atgetfdnsupport.h"
#include "mslogger.h"


_LIT8(KGetFdnSupportCommand,"AT+CPBS=?\r");
_LIT8(KGetFdnSupportCmdRepondStr,"+CPBS:");
_LIT8(KFdnString,"FD");
// Class CATGetFdnSupport
// ---------------------------------------------------------------------------
// CATGetFdnSupport::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetFdnSupport* CATGetFdnSupport::NewL(CGlobalPhonemanager& aGloblePhone, 
	                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGetFdnSupport* self = new(ELeave) CATGetFdnSupport(aGloblePhone,
			                                        aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATGetFdnSupport::CATGetFdnSupport
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetFdnSupport::CATGetFdnSupport(CGlobalPhonemanager& aGloblePhone, 
	                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATGetFdnSupport::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetFdnSupport::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iFdnSupport = EFalse;
	iAtType = ELtsyAT_Phone_CheckFdnSupport; 
	}
// ---------------------------------------------------------------------------
// CATGetFdnSupport::~CATGetFdnSupport
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetFdnSupport::~CATGetFdnSupport()
	{
	}
// ---------------------------------------------------------------------------
// CATGetFdnSupport::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetFdnSupport::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATGetFdnSupport::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetFdnSupport::ExecuteCommand()
	{
	iTxBuffer.Copy(KGetFdnSupportCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATGetFdnSupport::CATGetFdnSupport
// other items were commented in a header
// ---------------------------------------------------------------------------
TBool CATGetFdnSupport::GetFdnSupport()
	{
	return iFdnSupport;
	}

// ---------------------------------------------------------------------------
// CATGetFdnSupport::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGetFdnSupport::GetErrorValue()
	{
	return iError;
	}
// ---------------------------------------------------------------------------
// CATGetFdnSupport::ParseResponseL
// This method parses the modems response to the 'AT+CPBS=?' command.
// +CPBS: <pb>
// <pb> values reserved by this TS:
// "SM" SIM phonebook
// "FD" SIM fixdialling-phonebook
// "ON" SIM (or ME) own numbers (MSISDNs) list (reading of this storage may be 
// available through +CNUM also) 
// "EN" SIM (or ME) emergency number (+CPBW is not be applicable for this storage) 
// "LD" SIM last-dialling-phonebook
// "MC" ME missed (unanswered received) calls list (+CPBW may not be applicable for 
// this storage)
// "ME" ME phonebook
// "MT" combined ME and SIM phonebook
// "RC" ME received calls list (+CPBW may not be applicable for this storage)
// "SN" SDN  (service dailing phonebook)
// An example response is '+CPBS: "SM","FD","ON","EN" 
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetFdnSupport::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
		return;
		}
	if( array[0].MatchF(KGetFdnSupportCmdRepondStr)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return ;
		}
	for(TInt i = 1; i < Count; i++)
		{
		if( array[i].MatchF(KFdnString)!=KErrNotFound)
			{
			iFdnSupport = ETrue;
			break;
			}
		}
	
	CleanupStack::PopAndDestroy(&array);
	iError = KErrNone;
	}
//
// End of file
