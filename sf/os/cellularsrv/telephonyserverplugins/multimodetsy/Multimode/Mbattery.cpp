// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// GBATTERY.CPP
// Queries the ME battery. Retrieves charge level and battery status
// 
//

#include <et_phone.h>
#include "Mbattery.h"
#include "mSLOGGER.H"
#include "ATIO.H"
#include "Matstd.h"

CATGetBattery* CATGetBattery::NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	{
	CATGetBattery* battery = new(ELeave) CATGetBattery(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(battery);
	battery->ConstructL();
	CleanupStack::Pop();
	return battery;
	}

CATGetBattery::CATGetBattery(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
: CATCommands(aIo,aTelObject,aInit,aPhoneGlobals),iPhoneGlobals(aPhoneGlobals)
{}

void CATGetBattery::ConstructL()
	{
	CATCommands::ConstructL();
	}

CATGetBattery::~CATGetBattery()
	{
	iIo->RemoveExpectStrings(this);
	iOKExpectString=NULL;
	iErrorExpectString=NULL;
	}

void CATGetBattery::Start(TTsyReqHandle aTsyReqHandle,TAny* aParams)
	{
	LOGTEXT(_L8("MMTsy:\tCATGetBattery:\tStarting ME Battery query."));
	iReqHandle = aTsyReqHandle;
	iBatteryInfo = (RMobilePhone::TMobilePhoneBatteryInfoV1*) aParams;
	iTxBuffer.Format(KGetBatteryChargeCommand);
	iIo->Write(this, iTxBuffer);
	iIo->SetTimeOut(this, 5000);
	iState=EATBatteryWaitForWriteComplete;
	}

void CATGetBattery::EventSignal(TEventSource aSource)
	{
	if (aSource==ETimeOutCompletion)
		{
		LOGTEXT(_L8("MMTsy:\tCATGetBattery:\tTimeout Error during battery-strength query"));
		RemoveStdExpectStrings();
		Complete(KErrTimedOut,aSource);
		return;
		}

	switch(iState)
		{
		case EATBatteryWaitForWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATBatteryReadComplete;
			}
		break;

		case EATBatteryReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{	
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret)
				{
				iBatteryInfo->iStatus = RMobilePhone::EPowerStatusUnknown;
				Complete(ret,aSource);
				return;
				}
			TRAP(ret,ParseBatteryResponseL());
			Complete(ret,aSource);
			}
		break;

		default:
		break;
		}//switch
	}//EventSignal

void CATGetBattery::Stop(TTsyReqHandle aTsyReqHandle)
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle, Panic(EIllegalTsyReqHandle));
	LOGTEXT2(_L8("CATGetBattery::Stop - Cancelling from state %d"), iState);
	
	if (iState==EATBatteryWaitForWriteComplete)
		{
		Complete(KErrCancel, EReadCompletion);
		}
	// else do nothing because we have already sent the AT command
	// or have not actually started doing anything!
	}

void CATGetBattery::Complete(TInt aError,TEventSource aSource)
	{
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	iOKExpectString = NULL;
	iErrorExpectString = NULL;

	CATCommands::Complete(aError,aSource);
	iTelObject->ReqCompleted(iReqHandle, aError);

	LOGTEXT2(_L8("MMTsy:CATGetBattery:\tCATGetBattery completed with error code : %d"), aError);
	iState = EATNotInProgress;
	}

void CATGetBattery::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iTelObject->ReqCompleted(iReqHandle, aStatus);
		iState = EATNotInProgress;
		}
	}

void CATGetBattery::ParseBatteryResponseL()
	{
	ParseBufferLC();
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	TBool first = ETrue;
	TUint iCounter = 0;
	TInt ret;

	while (entry = iter++, entry != NULL)
		{
		if (first)
			{
			first = EFalse;
			while (entry->iResultPtr != KCBCResponseString)
				{
				entry->Deque();
				delete entry;
				entry = iter++;
				if (entry == NULL)
					{
					CleanupStack::PopAndDestroy();
					User::Leave(KErrNotFound);
					}
				} //while
			LOGTEXT(_L8("MMTsy:\tCATGetBattery:\tFound +CBC string!"));
			iCounter = 1;
			}
		else
			{
			TLex8 aLex((entry->iResultPtr).Ptr());
			if (iCounter == 1)
				{	
				TInt batteryStatus;
				ret = aLex.Val(batteryStatus);
				if (ret == KErrNone)
					// Have to convert the answer from the AT command format
					// to multimode representation.
					iBatteryInfo->iStatus = ConvertBatteryStatus(batteryStatus);

				}
			else if (iCounter == 2)
				ret = aLex.Val(iBatteryInfo->iChargeLevel);
			iCounter++;
			}//else
		entry->Deque();
		delete entry;
		}//while
	CleanupStack::PopAndDestroy();
	}//ParseBatteryResponseL

RMobilePhone::TMobilePhoneBatteryStatus CATGetBattery::ConvertBatteryStatus(TInt aBatteryStatus)

/** * Converts the batterey status from ETSI standard to multimode representation *
 * This method returns the proper multimode representation for the battery status. 
 * @param aBatteryStatus points to a value representing the battery status the ETSI way(0 - 3, ).
 * @return  Returns the battery status in multimode representation.
*/

	{
	switch (aBatteryStatus)
		{		
	case 0:
		return RMobilePhone::EPoweredByBattery; // equal to 1
	case 1:
		return RMobilePhone::EBatteryConnectedButExternallyPowered; // equal to 2
	case 2:
		return RMobilePhone::ENoBatteryConnected; // equal to 3
	case 3:
		return RMobilePhone::EPowerFault; // equal to 4
	default:
		return RMobilePhone::EPowerStatusUnknown; //equal to 0
		}
	}
