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
// CNotifyBatteryAndSignal

#include "notifybatteryandsignal.h"
#include "mslogger.h"
#include "atgetbatteryinfo.h"
#include "atgetsignalstrength.h"

const TInt KLtsyOneSecond = 1000000;

// Class CNotifyBatteryAndSignal
// ---------------------------------------------------------------------------
// CNotifyBatteryAndSignal::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CNotifyBatteryAndSignal* CNotifyBatteryAndSignal::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CNotifyBatteryAndSignal* self = new(ELeave) CNotifyBatteryAndSignal(aGloblePhone,
			                                                            aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop();
	return self ;
	}
// ---------------------------------------------------------------------------
// CNotifyBatteryAndSignal::CNotifyBatteryAndSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
CNotifyBatteryAndSignal::CNotifyBatteryAndSignal(CGlobalPhonemanager& aGloblePhone, 
	                                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CATCommandControlBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CNotifyBatteryAndSignal::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CNotifyBatteryAndSignal::ConstructL()
	{
	iATCommandName = EATNone;
	iCallbackTimer = CCallbackTimer::NewL(*this);
	iATGetBattery = CATGetBattery::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	iATGetSignal = CATGetSignal::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	}
// ---------------------------------------------------------------------------
// CNotifyBatteryAndSignal::~CNotifyBatteryAndSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
CNotifyBatteryAndSignal::~CNotifyBatteryAndSignal()
	{
	if(iATGetBattery)
		{
		delete iATGetBattery;
		iATGetBattery = NULL;
		}
	if(iATGetSignal)
		{
		delete iATGetSignal;
		iATGetSignal = NULL;
		}
	}
// ---------------------------------------------------------------------------
// CNotifyBatteryAndSignal::AtCommandExecuteComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CNotifyBatteryAndSignal::AtCommandExecuteComplete(TAtEventSource aEventSource, TInt /*aStatus*/)
	{
	if(aEventSource == EWriteCompletion)
		{
		return;
		}
	TInt ret = KErrNone;
	switch(iATCommandName)
		{
		case EATGetBattery:
			iATGetBattery->RemoveAtCommandObserver();
			iPhoneGlobals.iEventSignalActive = EFalse;
			if(aEventSource == EReadCompletion)
				{
				ret = iATGetBattery->GetErrorValue();
				if(KErrNone == ret)
					{
					iBatteryInfo = iATGetBattery->GetBatteryinfo();
					TRfStateInfo RfStateInfo;
					RfStateInfo = iATGetBattery->GetRfStateInfo();
					if(iPhoneGlobals.iPhoneStatus.iRfStateInfo!=RfStateInfo)
						{
						iPhoneGlobals.iPhoneStatus.iRfStateInfo = RfStateInfo;
						iCtsyDispatcherCallback.CallbackPhoneNotifyRfStatusChangeInd(ret,RfStateInfo);
						}
					if(iPhoneGlobals.iPhoneStatus.iBatteryInfo.iChargeLevel != iBatteryInfo.iChargeLevel)
						{
						iPhoneGlobals.iPhoneStatus.iBatteryInfo.iChargeLevel = iBatteryInfo.iChargeLevel;
						iCtsyDispatcherCallback.CallbackPhoneNotifyBatteryInfoChangeInd(ret,iBatteryInfo);
						}
					}
				}
			iATGetBattery->Complete();
			iATCommandName = EATGetSignal;
			break;
		case EATGetSignal: 
			iATGetSignal->RemoveAtCommandObserver();
			iPhoneGlobals.iEventSignalActive = EFalse;
			if(aEventSource == EReadCompletion)
				{
				ret = iATGetSignal->GetErrorValue();
				if(KErrNone == ret)
					{
					iSignalInfo = iATGetSignal->getSignalInfo();
					if(iPhoneGlobals.iPhoneStatus.iSignalInfo.iSignalStrength != iSignalInfo.iSignalStrength)
						{
						iCtsyDispatcherCallback.CallbackPhoneNotifySignalStrengthChangeInd(ret,
								                iSignalInfo.iSignalStrength,
								                iSignalInfo.iBar);
						}
					}
				}
			iATGetSignal->Complete();
			iATCommandName = EATGetBattery;
			break;
		default:
			break;
		}
	StartTimer(30);
	}
// ---------------------------------------------------------------------------
// CNotifyBatteryAndSignal::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CNotifyBatteryAndSignal::StartRequest()
	{
	StartTimer(10);
	iATCommandName = EATGetBattery;
	}
// ---------------------------------------------------------------------------
// CNotifyBatteryAndSignal::TimerRun
// other items were commented in a header
// ---------------------------------------------------------------------------
void CNotifyBatteryAndSignal::TimerRun(TInt aError)
    {
	if((aError == KErrNone)&&
	   (!iPhoneGlobals.iEventSignalActive)&&
	   (iPhoneGlobals.iPhoneStatus.iMode==RPhone::EModeOnlineCommand))
	    {
		switch(iATCommandName)
			{
			case EATGetBattery:
				iPhoneGlobals.iEventSignalActive = ETrue;
				iATGetBattery->SetAtCommandObserver(this);
				iATGetBattery->ExecuteCommand();
				break;
			case EATGetSignal:
				iPhoneGlobals.iEventSignalActive = ETrue;
				iATGetSignal->SetAtCommandObserver(this);
				iATGetSignal->ExecuteCommand();
				break;
			default:
				break;
			}
		}
	else
		{
		StartTimer(30);
		}
	}

/**
 * 
 * 
 */
void CNotifyBatteryAndSignal::StartTimer(TInt aSeconds)
    {
	iCallbackTimer->After(aSeconds*KLtsyOneSecond);
	}
//
// End of file
