// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @publishedPartner
 @released
*/

#include "delay.h"
#include "hctluartpowermanager.h"

#include <e32base.h>
#include <d32comm.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCTL_BASE);
#endif

CHCTLUartPowerManager::CHCTLUartPowerManager(MHCTLUartPowerObserver& aObserver, RBusDevComm& aPort)
 : CActive(EPriorityStandard), iObserver(aObserver), iPort(aPort)
	{
	LOG_FUNC
	}

CHCTLUartPowerManager::~CHCTLUartPowerManager()
	{
	LOG_FUNC
	
	Cancel();
	delete iPowerOffTimer;
	}

EXPORT_C CHCTLUartPowerManager* CHCTLUartPowerManager::NewL(MHCTLUartPowerObserver& aObserver, RBusDevComm& aPort)
	{
	LOG_STATIC_FUNC
	
	CHCTLUartPowerManager* self=new(ELeave)CHCTLUartPowerManager(aObserver, aPort);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CHCTLUartPowerManager::ConstructL()
	{
	LOG_FUNC
	
	CActiveScheduler::Add(this);
	
	TCallBack cb = TCallBack(CHCTLUartPowerManager::PowerOffTimeout, this);
	iPowerOffTimer = CDelay::NewL(cb, EPriorityStandard);
	}

void CHCTLUartPowerManager::DoCancel()
	{
	LOG_FUNC
	LOG(_L("HCTLUartPowerManager: Cancel signal notification"));

	iPort.NotifySignalChangeCancel();
	}

EXPORT_C void CHCTLUartPowerManager::Start()
	{
	LOG_FUNC
	
	if(!IsActive())
		{
		LOG(_L("HCTLUartPowerManager: Requesting signal notification"));

		iSignals = iPort.Signals();
		iPort.NotifySignalChange(iStatus, iSignals, KSignalCTS);
		SetActive();
		}
	}

/*static*/ TInt CHCTLUartPowerManager::PowerOffTimeout(TAny* aThis)
	{
	LOG_STATIC_FUNC
	
	// we got a timeout waiting for a signal change - hardware must be off
	reinterpret_cast<CHCTLUartPowerManager*>(aThis)->PowerChange();
	return KErrNone;
	}

void CHCTLUartPowerManager::PowerChange()
	{
	LOG_FUNC
	
	// the power has changed, update state and notify observer
	iState = (iState == EBTOn) ? EBTOff : EBTOn;
	LOG1(_L("HCTLUartPowerManager: Power Change: %d"), iState);

	iObserver.MhupoPowerChange(KErrNone);
	}

void CHCTLUartPowerManager::RunL()
	{
	LOG_FUNC
	TInt err = iStatus.Int();
	
	if (err == KErrNone)
		{
		TInt ctsHigh = iSignals & KSignalCTS;
		switch(iState)
			{
			case EBTOn:
				if (!ctsHigh) // do a check to see if CTS went high or low
					{
					LOG(_L("HCTLUartPowerManager: CTS low - might be flow control or power off..."))
					// we want to start the timer to see if CTS went low for flow 
					// control, or power off
					iPowerOffTimer->Cancel();
					iPowerOffTimer->After(KUartSignalChangeTimeout);
					}
				else
					{
					LOG(_L("HCTLUartPowerManager: CTS went from low to high - power on"));
					iPowerOffTimer->Cancel();
					}
	
				// in either case we want to see if there's more signal changes
				Start();
				break;
	
			case EBTOff:
				// This should be CTS up (since now *ON*)...
				__ASSERT_DEBUG(ctsHigh, PANIC(KHctlUartPowerManagerPanic, EHCTLUartPowerOnWithCTSLow));
				if (!ctsHigh)
					{
					LOG(_L("HCTLUartPowerManager: Error - CTS low when expecting high"));
					iObserver.MhupoPowerChange(KErrAbort);
					break;
					}
				
				LOG(_L("HCTLUartPowerManager: CTS went from low to high - power on"));
	
				// start waiting for more signal changes before notifying the observer
				Start();
				
				// notify the change in power
				PowerChange();
				break;
	
			default:
				// Panic here
				LOG1(_L("HCTLUartPowerManager: Unexpected state: %d"), iState);
				PANIC(KHctlUartPowerManagerPanic, EHCTLUartPowerInvalidState);
				break;
			}
		}
	else
		{
		// NotifySignalChange returned an error, tell the observer
		LOG1(_L("HCTLUartPowerManager: Signal notification failed: "), err);
		iObserver.MhupoPowerChange(err);
		}
	}
