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
// Queries the ME battery. Retrieves charge level and battery status

#include "atgetbatteryinfo.h"
#include "mslogger.h"
#include "requestbase.h"


_LIT8(KGetBatteryCommand,"AT+CIND?\r");
_LIT8(KGetBatteryCmdRepondStr,"+CIND:");
// Class CATGetBattery
// ---------------------------------------------------------------------------
// CATGetBattery::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetBattery* CATGetBattery::NewL(CGlobalPhonemanager& aGloblePhone, 
	                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGetBattery* self = new(ELeave) CATGetBattery(aGloblePhone,
			                                        aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATGetBattery::CATGetBattery
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetBattery::CATGetBattery(CGlobalPhonemanager& aGloblePhone, 
	                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATGetBattery::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetBattery::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Phone_GetBattery;
	}
// ---------------------------------------------------------------------------
// CATGetBattery::~CATGetBattery
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetBattery::~CATGetBattery()
	{
	}
// ---------------------------------------------------------------------------
// CATGetBattery::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetBattery::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATGetBattery::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetBattery::ExecuteCommand()
	{
	iTxBuffer.Copy(KGetBatteryCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATGetBattery::CATGetBattery
// other items were commented in a header
// ---------------------------------------------------------------------------
RMobilePhone::TMobilePhoneBatteryInfoV1 CATGetBattery::GetBatteryinfo()
	{
	return iBatteryInfo;
	}
// ---------------------------------------------------------------------------
// CATGetBattery::CATGetBattery
// other items were commented in a header
// ---------------------------------------------------------------------------
TRfStateInfo CATGetBattery::GetRfStateInfo()
	{
	return iRfStateInfo;
	}
// ---------------------------------------------------------------------------
// CATGetBattery::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGetBattery::GetErrorValue()
	{
	return iError;
	}

// ---------------------------------------------------------------------------
// CATGetBattery::ParseResponseL
// This method parses the modems response to the 'AT+CIND?' command.
// +CIND: <ind>[,<ind>[,...]]
// An example response is '+CIND: 3,3,1,1,1,1,1,1,1' where first value 
// denotes the current battery charge level.
// "battchg" battery charge level (0-5)
// "signal" signal quality (0-5)
// "service" service availability (0-1)
// "sounder" sounder activity (0-1)
// "message" message received (0-1)
// "call" call in progress (0-1)
// "vox" transmit activated by voice activity (0-1)
// "roam" roaming indicator (0-1)
// "smsfull" a short message memory storage in the MT has become full (1), or memory locations are available (0);
// i.e. the range is (0-1)
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetBattery::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
	if( array[0].MatchF(KGetBatteryCmdRepondStr)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return ;
		}
	TInt val = 0;
	TLex8 lex(array[1]);
	lex.Val(val);
	if (val < 0) 
		{
		iBatteryInfo.iStatus = RMobilePhone::EPowerStatusUnknown;
		iBatteryInfo.iChargeLevel = 0;
		}
	else	
		{
		iBatteryInfo.iStatus = RMobilePhone::EPoweredByBattery;
		iBatteryInfo.iChargeLevel = val;
		}
	TLex8 lexRf(array[3]);
	lexRf.Val(val);
	if(val == 1)
		{
		iRfStateInfo = ERfsStateInfoNormal;
		}
	else
		{
		iRfStateInfo = ERfsStateInfoInactive;
		}
	
	CleanupStack::PopAndDestroy(&array);
	iError = KErrNone;
	}
// ---------------------------------------------------------------------------
// CATGetBattery::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetBattery::EventSignal(TAtEventSource aEventSource, TInt aStatus)
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
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCtsyDispatcherCallback.CallbackPhoneGetBatteryInfoComp(aStatus,iBatteryInfo);
	}
//
// End of file
