// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "TE_EtelIsvbase.h"

_LIT(KEtelISVBufferConfigStr, "BufferMode");

CEtelIsvTestStep::CEtelIsvTestStep()
	{}

CEtelIsvTestStep::~CEtelIsvTestStep()
	{}

TVerdict CEtelIsvTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;	//mark for memory leaks!!

	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);

	//
	// Ensure the SIMTSY test number is zero'd before we start...
	//
	User::LeaveIfError(iTestNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
	User::LeaveIfError(iTestNumberProperty.Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, 0));
	
	iTelephony = CTelephony::NewL();
	
	//
	// Read the optional configuration info
	//
	if ( !GetBoolFromConfig(ConfigSection(),KEtelISVBufferConfigStr,iBufferMode) )
		{
		// default test mode
		iBufferMode = EFalse;
		}
	
	if (iBufferMode)
		{
		INFO_PRINTF1(_L("Testing in \"buffer\" mode") );
		}
	else 
		{
		INFO_PRINTF1(_L("Testing in \"NON buffer\" mode") );
		}

	return TestStepResult();
	}

TVerdict CEtelIsvTestStep::doTestStepPostambleL()
	{
	delete iTelephony;
	iTelephony = NULL;
    iTestNumberProperty.Close();

	if (iMyAsyncOneShot)
		{
		delete iMyAsyncOneShot;
		iMyAsyncOneShot = NULL;
		}
	delete iScheduler;

	__UHEAP_MARKEND;

 	return TestStepResult();
	}

void CEtelIsvTestStep::PrintIndicatorValues(TInt aIndicator, TInt aIndicatorCaps)
	{
	if(aIndicatorCaps&CTelephony::KIndChargerConnected)
		{
		if(aIndicator&CTelephony::KIndChargerConnected)
			{
			INFO_PRINTF1(_L("Battery charger connected"));
			}
		else
			{
			INFO_PRINTF1(_L("Battery charger not connected"));
			}
		}
	else
		{
		INFO_PRINTF1(_L("Battery charger indiction reporting unavailable"));
		}

	if(aIndicatorCaps&CTelephony::KIndNetworkAvailable)
		{
		if(aIndicator&CTelephony::KIndNetworkAvailable)
			{
			INFO_PRINTF1(_L("Network service available"));
			}
		else
			{
			INFO_PRINTF1(_L("Network service not available"));
			}
		}
	else
		{
		INFO_PRINTF1(_L("Network service indiction reporting unavailable"));
		}

	if(aIndicatorCaps&CTelephony::KIndCallInProgress)
		{
		if(aIndicator&CTelephony::KIndCallInProgress)
			{
			INFO_PRINTF1(_L("Call In progress"));
			}
		else
			{
			INFO_PRINTF1(_L("Call Not In progress"));
			}
		}
	else
		{
		INFO_PRINTF1(_L("Call In progress indiction reporting unavailable"));
		}
	}

void CEtelIsvTestStep::PrintBatteryInfo(CTelephony::TBatteryInfoV1& aBatteryInfo)
	{
	TBuf<200> BatteryStatus;

	switch (aBatteryInfo.iStatus)
		{
		case CTelephony::EPowerStatusUnknown:
			BatteryStatus = _L("EPowerStatusUnknown");
			break;
		case CTelephony::EPoweredByBattery:
			BatteryStatus = _L("EPoweredByBattery");
			break;
		case CTelephony::EBatteryConnectedButExternallyPowered:
			BatteryStatus = _L("EBatteryConnectedButExternallyPowered");
			break;
		case CTelephony::ENoBatteryConnected:
			BatteryStatus = _L("ENoBatteryConnected");
			break;
		case CTelephony::EPowerFault:
			BatteryStatus = _L("EPowerFault");
			break;
		default:
			BatteryStatus = _L("UNKNOWN");
			break;
		}
	INFO_PRINTF2(_L("Battery status : %S"), &BatteryStatus);
	INFO_PRINTF2(_L("Battery level : %d"), aBatteryInfo.iChargeLevel);
	}

void CEtelIsvTestStep::PrintLockInfo(CTelephony::TIccLock aLock, TInt aLockStatus, TInt aLockSetting)
	{
	TBuf<200> LockStatus;
	TBuf<200> LockSetting;
	TBuf<10> lock;

	if(aLock==CTelephony::ELockPin1)
		{
		lock = _L("Lock1");
		}
	else
		{
		lock = _L("Lock2");
		}

	switch (aLockStatus)
		{
		case CTelephony::EStatusLockUnknown:
			LockStatus = _L("EStatusLockUnknown");
			break;
		case CTelephony::EStatusLocked:
			LockStatus = _L("EStatusLocked");
			break;
		case CTelephony::EStatusUnlocked:
			LockStatus = _L("EStatusUnlocked");
			break;
		case CTelephony::EStatusBlocked:
			LockStatus = _L("EStatusBlocked");
			break;
		default:
			break;
		}

	switch (aLockSetting)
		{
		case CTelephony::ELockSetUnknown:
			LockSetting = _L("ELockSetUnknown");
			break;
		case CTelephony::EStatusLocked:
			LockSetting = _L("ELockSetEnabled");
			break;
		case CTelephony::EStatusUnlocked:
			LockSetting = _L("ELockSetDisabled");
			break;
		default:
			break;
		}

	INFO_PRINTF3(_L("%S - Lock status : %S"), &lock, &LockStatus);
	INFO_PRINTF3(_L("%S - Lock setting : %S"), &lock, &LockSetting);
	}

void CEtelIsvTestStep::PrintNetworkRegStatus(TInt aNetworkRegStatus)
	{
	TBuf<200> NetworkRegStatus;

	switch (aNetworkRegStatus)
		{
		case CTelephony::ERegistrationUnknown:
			NetworkRegStatus = _L("ERegistrationUnknown");
			break;
		case CTelephony::ENotRegisteredNoService:
			NetworkRegStatus = _L("ENotRegisteredNoService");
			break;
		case CTelephony::ENotRegisteredEmergencyOnly:
			NetworkRegStatus = _L("ENotRegisteredEmergencyOnly");
			break;
		case CTelephony::ENotRegisteredSearching:
			NetworkRegStatus = _L("ENotRegisteredSearching");
			break;
		case CTelephony::ERegisteredBusy:
			NetworkRegStatus = _L("ERegisteredBusy");
			break;
		case CTelephony::ERegisteredOnHomeNetwork:
			NetworkRegStatus = _L("ERegisteredOnHomeNetwork");
			break;
		case CTelephony::ERegistrationDenied:
			NetworkRegStatus = _L("ERegistrationDenied");
			break;
		case CTelephony::ERegisteredRoaming:
			NetworkRegStatus = _L("ERegisteredRoaming");
			break;
		default:
			break;
		}
	INFO_PRINTF2(_L("Network registration status : %S"), &NetworkRegStatus);

	}

void CEtelIsvTestStep::PrintNetworkInfo(TInt aNetworkMode, TInt aNetworkAccess, TInt aNetworkStatus, TInt aNetworkBandInfo)
	{
	TBuf<200> NetworkMode;
	TBuf<200> NetworkAccess;
	TBuf<200> NetworkStatus;
	TBuf<200> NetworkBandInfo;

	switch (aNetworkMode)
		{
		case CTelephony::ENetworkModeUnknown:
			NetworkMode = _L("ENetworkModeUnknown");
			break;
		case CTelephony::ENetworkModeUnregistered:
			NetworkMode = _L("ENetworkModeUnregistered");
			break;
		case CTelephony::ENetworkModeGsm:
			NetworkMode = _L("ENetworkModeGsm");
			break;
		case CTelephony::ENetworkModeAmps:
			NetworkMode = _L("ENetworkModeAmps");
			break;
		case CTelephony::ENetworkModeCdma95:
			NetworkMode = _L("ENetworkModeCdma95");
			break;
		case CTelephony::ENetworkModeCdma2000:
			NetworkMode = _L("ENetworkModeCdma2000");
			break;
		case CTelephony::ENetworkModeWcdma:
			NetworkMode = _L("ENetworkModeWcdma");
			break;
		case CTelephony::ENetworkModeTdcdma:
			NetworkMode = _L("ENetworkModeTdcdma");
			break;
		default:
			break;
		}

	switch (aNetworkAccess)
		{
		case CTelephony::ENetworkAccessUnknown:
			NetworkAccess = _L("ENetworkAccessUnknown");
			break;
		case CTelephony::ENetworkAccessGsm:
			NetworkAccess = _L("ENetworkAccessGsm");
			break;
		case CTelephony::ENetworkAccessGsmCompact:
			NetworkAccess = _L("ENetworkAccessGsmCompact");
			break;
		case CTelephony::ENetworkAccessUtran:
			NetworkAccess = _L("ENetworkAccessUtran");
			break;
		default:
			break;
		}

	switch (aNetworkStatus)
		{
		case CTelephony::ENetworkStatusUnknown:
			NetworkStatus = _L("ENetworkStatusUnknown");
			break;
		case CTelephony::ENetworkStatusAvailable:
			NetworkStatus = _L("ENetworkStatusAvailable");
			break;
		case CTelephony::ENetworkStatusCurrent:
			NetworkStatus = _L("ENetworkStatusCurrent");
			break;
		case CTelephony::ENetworkStatusForbidden:
			NetworkStatus = _L("ENetworkStatusForbidden");
			break;
		default:
			break;
		}

	switch (aNetworkBandInfo)
		{
		case CTelephony::EBandUnknown:
			NetworkBandInfo = _L("EBandUnknown");
			break;
		case CTelephony::E800BandA:
			NetworkBandInfo = _L("E800BandA");
			break;
		case CTelephony::E800BandB:
			NetworkBandInfo = _L("E800BandB");
			break;
		case CTelephony::E800BandC:
			NetworkBandInfo = _L("E800BandC");
			break;
		case CTelephony::E1900BandA:
			NetworkBandInfo = _L("E1900BandA");
			break;
		case CTelephony::E1900BandB:
			NetworkBandInfo = _L("E1900BandB");
			break;
		case CTelephony::E1900BandC:
			NetworkBandInfo = _L("E1900BandC");
			break;
		case CTelephony::E1900BandD:
			NetworkBandInfo = _L("E1900BandD");
			break;
		case CTelephony::E1900BandE:
			NetworkBandInfo = _L("E1900BandE");
			break;
		case CTelephony::E1900BandF:
			NetworkBandInfo = _L("E1900BandF");
			break;
		default:
			break;
		}

	INFO_PRINTF2(_L("Network mode : %S"), &NetworkMode);
	INFO_PRINTF2(_L("Network access : %S"), &NetworkAccess);
	INFO_PRINTF2(_L("Network status : %S"), &NetworkStatus);
	INFO_PRINTF2(_L("Network band info : %S"), &NetworkBandInfo);
	}

void CEtelIsvTestStep::SetTestNumber(TInt aTestNumber)
  	{
	delete iTelephony;
	iTelephony=NULL;

  	INFO_PRINTF1(_L(""));
  	INFO_PRINTF2(_L("Changing the Test Number to %d"),aTestNumber);

	TRequestStatus status;
	iTestNumberProperty.Subscribe(status);
	TEST1(iTestNumberProperty.Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,aTestNumber) == KErrNone, ETrue);
	User::WaitForRequest(status);
	TEST1(status.Int() == KErrNone, ETrue);
	TInt testNumberCheck;
	TEST1(iTestNumberProperty.Get(testNumberCheck) == KErrNone, ETrue);
	TEST1(aTestNumber == testNumberCheck, ETrue);

	TRAPD(ret, iTelephony = CTelephony::NewL());
	TEST1(ret==KErrNone, ETrue);

	if (ret != KErrNone)
		{
		iTelephony=NULL;
		}
	}

/**
Uses a mechanism to tell sim.tsy that the timers it holds should have
their countdown reduced.  The timers are set up in sim.tsy as a
consequence of the delays specified in the c:\config.txt.
*/
void CEtelIsvTestStep::SignalSimTsyToReduceTimersL()
	{
	RProperty reduceTimersProperty;
	User::LeaveIfError(reduceTimersProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTimersReduceTime));
	CleanupClosePushL(reduceTimersProperty);

	User::LeaveIfError(reduceTimersProperty.Set(KUidPSSimTsyCategory, KPSSimTsyTimersReduceTime,
	                                            KReduceSimTsyTimers));

	CleanupStack::PopAndDestroy(&reduceTimersProperty);
	}

CTestCanceller::CTestCanceller(CActive* aRetrieve)
: CAsyncOneShot(EPriorityNormal),iRetrieve(aRetrieve)
	{}

CTestCanceller::~CTestCanceller()
	{}

void CTestCanceller::RunL()
	{
	iRetrieve->Cancel();
	CActiveScheduler::Stop();
	}

CEtelIsvActBase::CEtelIsvActBase(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CActive(CActive::EPriorityStandard), iCanceller(NULL), iTelephony(aTelephony), iTestStep(aTestStep)
	{}

void CEtelIsvActBase::CallMe(TInt& aCallCount)
/**
Notify property to get SIMTSY to initiate a call
*/
	{
	aCallCount++;	//increment count, implies a change of state which SIMTSY will react to.

	RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyIncomingVoiceCall, aCallCount);
	User::After(1000000L);

	return;
	}

/**
Public constructor.

@return Pointer to created CMyAsyncOneShot object.
*/
CMyAsyncOneShot* CMyAsyncOneShot::NewL()
	{
	CMyAsyncOneShot* self = new (ELeave) CMyAsyncOneShot();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Default constructor.
*/
CMyAsyncOneShot::CMyAsyncOneShot()
: CAsyncOneShot(CActive::EPriorityStandard)
	{ // NOP
	}

/**
Second phase constructor.
*/
void CMyAsyncOneShot::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	}

/**
Starts the timer.
*/
void CMyAsyncOneShot::StartOneShot()
	{
	iTimer.After(iStatus,10000);
	Call();
	}
/**
Stops the active scheduler. Does nothing else.
*/
void CMyAsyncOneShot::RunL()
	{
	CActiveScheduler::Stop();
	}

/**
Default destructor.
*/
CMyAsyncOneShot::~CMyAsyncOneShot()
	{
	Cancel();
	iTimer.Close();
	}

/**
Cancels the timer.
*/
void CMyAsyncOneShot::DoCancel()
	{
	iTimer.Cancel();
	}
