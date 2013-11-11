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
// Basic GSM Network and Operator information Implementation file.
// 
// This file contains the implementation of the CATNetworkInfo, CATDetectNetwork,
// CNotifyNetworkRegistrationStatusChange and CCurrentNetworkChangedNotify classes.
// Find the current Network Operator in Numeric, Long and Short formats.
// NB - Some phones do not fully support all the formats, if any. The commands are 
// supported implement this command. Valid responses are set in the TMobilePhoneLocationAreaV1
// and the TMobilePhoneNetworkInfoV1 structures and the Network's status is set to Current.



#include <etelmm.h>
#include "atgetnetworkinfo.h"
#include "mslogger.h"

_LIT8(KGetCurrentNetworkInfoCommand, "AT+COPS?\r");
_LIT8(KGetCurrentNetworkInfoResponse,"+COPS:");
/**
 * Utility function to translate the 5 digit ASCII network identification returned by the ME
 * into Mobile Country Code (aCountryCode) and a Mobile Network Code (aNetworkIdentity) strings.
 * Tbe format returned by the ME is XXXYY, where XXX represents the Mobile Country Code and YY
 * represents the Mobile Network Code.
 */
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
// Class CATNetworkInfo
// ---------------------------------------------------------------------------
// CATNetworkInfo::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNetworkInfo* CATNetworkInfo::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATNetworkInfo* self = new(ELeave) CATNetworkInfo(aGloblePhone,
			                                          aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATNetworkInfo::CATNetworkInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNetworkInfo::CATNetworkInfo(CGlobalPhonemanager& aGloblePhone, 
	                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATNetworkInfo::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkInfo::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iGetHomeNwk = EFalse;
	iAtType = ELtsyAT_Phone_NwkInfo;
	}
// ---------------------------------------------------------------------------
// CATNetworkInfo::~CATNetworkInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNetworkInfo::~CATNetworkInfo()
	{
	}
// ---------------------------------------------------------------------------
// CATNetworkInfo::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkInfo::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATNetworkInfo::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkInfo::ExecuteCommand( )
	{
	iTxBuffer.Copy(KGetCurrentNetworkInfoCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATNetworkInfo::GetNetWorkInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
RMobilePhone::TMobilePhoneNetworkInfoV5 CATNetworkInfo:: GetNetWorkInfo()
	{
	return iNetworkInfo;
	}
// ---------------------------------------------------------------------------
// CATNetworkInfo::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATNetworkInfo::GetErrorValue()
	{
	return iError;
	}
// ---------------------------------------------------------------------------
// CATNetworkInfo::SetToGetHomeNwk
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkInfo::SetToGetHomeNwk()
	{
	iGetHomeNwk = ETrue;
	}
/**
 * This method parses the modems response to the 'AT+CGREG?' command.
 * An example response is '+CGREG: 0,1' where second value denotes the
 * current registration status.
 */
// ---------------------------------------------------------------------------
// CATNetworkInfo::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkInfo::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
	
	if( array[0].MatchF(KGetCurrentNetworkInfoResponse)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return ;
		}
	TInt val = 0;
	TLex8 lex(array[1]);
	lex.Val(val);
	iPhoneGlobals.iPhoneStatus.iLocalNetworkSel.iMethod = RMobilePhone::TMobilePhoneSelectionMethod(val);
	if(Count > 2)
		{
		TLex8 lex1(array[2]);
		lex1.Val(iNameFormat);
		switch (iNameFormat)
			{
			case 0:
				if(array[3].Length() > iNetworkInfo.iLongName.MaxLength())
					iNetworkInfo.iLongName.Copy(array[3].Mid(0,iNetworkInfo.iLongName.MaxLength()));			
				else
					iNetworkInfo.iLongName.Copy(array[3]);
				break;
			case 1:
				if(array[2].Length() > iNetworkInfo.iShortName.MaxLength())
					iNetworkInfo.iShortName.Copy(array[3].Mid(0,iNetworkInfo.iShortName.MaxLength()));			
				else 
					iNetworkInfo.iShortName.Copy(array[3]);	
				break;
			case 2:
				/*if(array[2].Length() > iNetworkInfo.iShortName.MaxLength())
					iNetworkInfo.iShortName.Copy(array[3].Mid(0,iNetworkInfo.iShortName.MaxLength()));			
				else 
					iNetworkInfo.iShortName.Copy(array[3]);*/	
				User::LeaveIfError(NetworkIdL(array[3],iNetworkInfo.iCountryCode, iNetworkInfo.iNetworkId));
				break;
			default:
				User::Leave(KErrGeneral);
				break;
				}
		}
	else
		{
		iError = KErrGeneral;
		}
	iNetworkInfo.iStatus=RMobilePhone::ENetworkStatusCurrent;
	CleanupStack::PopAndDestroy();
	iError = KErrNone;
	}
// ---------------------------------------------------------------------------
// CATNetworkInfo::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNetworkInfo::EventSignal(TAtEventSource aEventSource, TInt aStatus)
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
	iPhoneGlobals.iPhoneStatus.iCurrentNetwork = iNetworkInfo;
	if(iGetHomeNwk)
		{
		iCtsyDispatcherCallback.CallbackPhoneGetHomeNetworkComp(aStatus,iNetworkInfo);
		}
	else
		{
		iCtsyDispatcherCallback.CallbackPhoneGetCurrentNetworkInfoComp(aStatus,
								iNetworkInfo, 
								iPhoneGlobals.iPhoneStatus.iLocationArea);
		}
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	}
//
// End of file





