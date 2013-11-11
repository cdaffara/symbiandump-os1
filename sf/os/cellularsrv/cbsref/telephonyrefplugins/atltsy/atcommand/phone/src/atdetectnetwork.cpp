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
// Get the detected network

#include "atdetectnetwork.h"
#include "mslogger.h"

_LIT8(KDetectNetworkCommand,"AT+COPS=?\r");
_LIT8(KGetDetectNetworkRepondStr,"+COPS:");
const TInt KLtsyDetectNetworkReadTimeOut = 80;

// ---------------------------------------------------------------------------
// NetworkIdL
// Utility function to translate the 5 digit ASCII network identification 
// returned by the ME into Mobile Country Code (aCountryCode) and a Mobile 
// Network Code (aNetworkIdentity) strings. Tbe format returned by the ME is 
// XXXYY, where XXX represents the Mobile Country Code and YY represents the 
// Mobile Network Code.
// ---------------------------------------------------------------------------
static TInt NetworkIdL(const TDesC8& aCode,
					   RMobilePhone::TMobilePhoneNetworkCountryCode& aCountryCode, 
					   RMobilePhone::TMobilePhoneNetworkIdentity& aNetworkIdentity)
	{
	if (aCode.Length()!=5)
		{
		return KErrGeneral;
		}
	
	aCountryCode.SetLength(3);
	aCountryCode[0] = aCode[0];
	aCountryCode[1] = aCode[1];
	aCountryCode[2] = aCode[2];

	aNetworkIdentity.SetLength(2);
	aNetworkIdentity[0] = aCode[3];
	aNetworkIdentity[1] = aCode[4];
	return KErrNone;
	}
// Class CATDetectNetwork
// ---------------------------------------------------------------------------
// CATDetectNetwork::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATDetectNetwork* CATDetectNetwork::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATDetectNetwork* self = new(ELeave) CATDetectNetwork(aGloblePhone,aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATDetectNetwork::CATDetectNetwork
// other items were commented in a header
// ---------------------------------------------------------------------------
CATDetectNetwork::CATDetectNetwork(CGlobalPhonemanager& aGloblePhone, 
	                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	iReadTimeOut = KLtsyDetectNetworkReadTimeOut;
	}
// ---------------------------------------------------------------------------
// CATDetectNetwork::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDetectNetwork::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iList= NULL;
	iCommandRunning = EFalse;
	iCommandCancel = EFalse;
	}
// ---------------------------------------------------------------------------
// CATDetectNetwork::~CATDetectNetwork
// other items were commented in a header
// ---------------------------------------------------------------------------
CATDetectNetwork::~CATDetectNetwork()
	{
	if(iList)
		{
		delete iList;
		iList = NULL;
		}
	}
// ---------------------------------------------------------------------------
// CATDetectNetwork::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDetectNetwork::StartRequest()
	{
	iCommandRunning = ETrue;
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATDetectNetwork::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDetectNetwork::ExecuteCommand()
	{
	iTxBuffer.Copy(KDetectNetworkCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATDetectNetwork::CancelCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDetectNetwork::CancelCommand()
	{	
	iCommandCancel = ETrue;
	}
// ---------------------------------------------------------------------------
// CATDetectNetwork::ParseResponseL
// should be
// +COPS: (status,"longname","shortname",code),... ,,(list),(list)
// +COPS: [list of supported (<stat>,long alphanumeric <oper>,short alphanumeric 
// <oper>,numeric <oper>)s]
// <stat>:
// 0 unknown
// 1 available
// 2 current
// 3 forbidden
// <oper>: string type; <format> indicates if the format is alphanumeric or numeric;
// long alphanumeric format can be upto 16 characters long and short format up to 8 
// characters (refer GSM MoU SE.13 [9]); numeric format is the GSM Location Area 
// Identification number (refer GSM 04.08 [8] subclause 10.5.1.3) which consists of
// a three BCD digit country code coded as in ITU-T E.212 Annex A [10], plus a two 
// BCD digit network code, which is administration specific; returned <oper> shall 
// not be in BCD format, but in IRA characters converted from BCD; hence the number 
// has structure: (country code digit 3)(country code digit 2)(country code digit 1)
// (network code digit 2)(network code digit 1)
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDetectNetwork::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) != 0)
		{
		iError = KErrGeneral;
		return ;
		}
    if(iList)
    	{
    	delete iList;
    	iList = NULL;
    	}
	iList=CMobilePhoneNetworkListV2::NewL();
	
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
	if( array[0].MatchF(KGetDetectNetworkRepondStr)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return ;
		}
	TInt count = array.Count() - 1;
	for (TInt i = 1;i < count;i++)
		{
		RMobilePhone::TMobilePhoneNetworkInfoV2 info;	
		// This TSY only supports GSM mode so we can hardcode this
		info.iMode=RMobilePhone::ENetworkModeGsm;
		TInt val;
		TLex8 lex(array[i]);
		lex.Val(val);
		info.iStatus = RMobilePhone::TMobilePhoneNetworkStatus(val);
		i++;
		info.iLongName.Copy(array[i]);		
		i++;
		if(array[i].Length() > info.iShortName.MaxLength())
			info.iShortName.Copy(array[i].Mid(0,info.iShortName.MaxLength()));			
		else
			info.iShortName.Copy(array[i]);
		i++;
		User::LeaveIfError(NetworkIdL(array[i],info.iCountryCode, info.iNetworkId));  // MNC and MCC	
		
		iList->AddEntryL(info); 
		}
	CleanupStack::PopAndDestroy();
	} //ParseResponseL
// ---------------------------------------------------------------------------
// ATDetectNetwork::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATDetectNetwork::EventSignal(TAtEventSource aEventSource, TInt aStatus)
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
	if(iCommandCancel)
		{
		iCtsyDispatcherCallback.CallbackPhoneGetDetectedNetworksCancelComp(0);
		}
	else
		{
		iCtsyDispatcherCallback.CallbackPhoneGetDetectedNetworksComp(aStatus,*iList);
		}
	iCommandRunning = EFalse;
	iCommandCancel = EFalse;
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	
	}
//
// End of file

