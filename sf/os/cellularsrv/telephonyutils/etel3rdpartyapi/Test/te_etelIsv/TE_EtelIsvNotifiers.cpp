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

#include "TE_EtelIsvbase.h"
#include "TE_EtelIsvNotifiers.h"
#include "TE_EtelIsvCallFunc.h"
 

/**
Each test step initialises it's own name
*/
CTestNotifyChange::CTestNotifyChange()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestRegisterForNotification"));
	}

/**
@SYMTestCaseID Etel3rdParty 26.1, TestNotifyChange
@SYMTestCaseDesc Test NotifyChange 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc,
REQ 2116.4.2, 2116.4.3, 2116.4.3, 2116.4.4, 2116.4.6, 2116.3.2
*/ 
TVerdict CTestNotifyChange::doTestStepL()
	{
	INFO_PRINTF1(_L("Test notifications"));
	
	TInt numberNotifierTestsActive = 0;	
	
	const TInt invalidNotificationEvent = 61;
	
	//Test 1: Try send a request for an invalid event range
	SetTestNumber(16);
	INFO_PRINTF1(_L("Testing invalid event notification"));
	CTelephony::TFlightModeV1 notifierStatus;
	CTelephony::TFlightModeV1Pckg notifierStatusPckg(notifierStatus);	
	TDes8* notifierStatusData;
	if (iBufferMode)
		{
		notifierStatusData = &iNotifierStatusBuff;
		}
	else
		{
		notifierStatusData = &notifierStatusPckg;
		}
		
	CTestRegisterNotifierAct* modeChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, (CTelephony::TNotificationEvent)invalidNotificationEvent);	
	//add the number of notification status objects for the test
	CTelephony::TFlightModeV1 expNotifierStatus1;
	CTelephony::TFlightModeV1Pckg expNotifierStatusPckg1(expNotifierStatus1);	
	expNotifierStatus1.iFlightModeStatus = CTelephony::EFlightModeOn;	
	modeChangeAct->iExpNotificationCompleteList.Append(expNotifierStatusPckg1);
	
	numberNotifierTestsActive = 0;
	modeChangeAct->StartNotifier(*notifierStatusData, KErrArgument);	
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(modeChangeAct);	

	//Notification Test 2: EFlightModeChange
	SetTestNumber(16);
	INFO_PRINTF1(_L("Testing EFlightModeChange notification"));
	CTelephony::TFlightModeV1 fmNotifierStatus;
	CTelephony::TFlightModeV1Pckg fmNotifierStatusPckg(fmNotifierStatus);	
	
	TDes8* fmNotifierStatusData;
	if (iBufferMode)
		{
		fmNotifierStatusData = &iFmNotifierStatusBuff;
		}
	else
		{
		fmNotifierStatusData = &fmNotifierStatusPckg;
		}
		
	CTestRegisterNotifierAct* FlightModeChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::EFlightModeChange);
	
	//add the number of notification status objects for the test	
	CTelephony::TFlightModeV1 fmExpNotifierStatus1;
	CTelephony::TFlightModeV1Pckg fmExpNotifierStatusPckg1(fmExpNotifierStatus1);	
	fmExpNotifierStatus1.iFlightModeStatus = CTelephony::EFlightModeOff;	
	FlightModeChangeAct->iExpNotificationCompleteList.Append(fmExpNotifierStatusPckg1);
	
	CTelephony::TFlightModeV1 fmExpNotifierStatus2;
	CTelephony::TFlightModeV1Pckg fmExpNotifierStatusPckg2(fmExpNotifierStatus2);	
	fmExpNotifierStatus2.iFlightModeStatus = CTelephony::EFlightModeOn;
	FlightModeChangeAct->iExpNotificationCompleteList.Append(fmExpNotifierStatusPckg2);
	
	CTelephony::TFlightModeV1 fmExpNotifierStatus3;
	CTelephony::TFlightModeV1Pckg fmExpNotifierStatusPckg3(fmExpNotifierStatus3);	
	fmExpNotifierStatus3.iFlightModeStatus = CTelephony::EFlightModeOff;
	FlightModeChangeAct->iExpNotificationCompleteList.Append(fmExpNotifierStatusPckg3);
	
	numberNotifierTestsActive = 0;
	FlightModeChangeAct->StartNotifier(*fmNotifierStatusData, KErrNone, ETrue);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(FlightModeChangeAct);

	//Notification Test 3: EIndicatorChange
	SetTestNumber(17);
	INFO_PRINTF1(_L("Testing EIndicatorChange notification"));
	CTelephony::TIndicatorV1 indNotifierStatus;
	CTelephony::TIndicatorV1Pckg indNotifierStatusPckg(indNotifierStatus);	

	TDes8* indNotifierStatusData;
	if (iBufferMode)
		{
		indNotifierStatusData = &iIndNotifierStatusBuff;
		}
	else
		{
		indNotifierStatusData = &indNotifierStatusPckg;
		}
		
	CTestRegisterNotifierAct* IndicatorChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::EIndicatorChange);
	
	//add the number of notification status objects for the test	
	CTelephony::TIndicatorV1 indExpNotifierStatus1;
	CTelephony::TIndicatorV1Pckg indExpNotifierStatusPckg1(indExpNotifierStatus1);	
	indExpNotifierStatus1.iCapabilities = CTelephony::KIndChargerConnected | CTelephony::KIndNetworkAvailable | CTelephony::KIndCallInProgress;
	indExpNotifierStatus1.iIndicator = CTelephony::KIndChargerConnected;
	IndicatorChangeAct->iExpNotificationCompleteList.Append(indExpNotifierStatusPckg1);
	
	#if defined(_DEBUG)
	//test OOM case
		numberNotifierTestsActive = 0;
		__UHEAP_FAILNEXT(1);
		IndicatorChangeAct->StartNotifier(*indNotifierStatusData, KErrNoMemory);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
	
	CTelephony::TIndicatorV1 indExpNotifierStatus2;
	CTelephony::TIndicatorV1Pckg indExpNotifierStatusPckg2(indExpNotifierStatus2);	
	indExpNotifierStatus2.iCapabilities = CTelephony::KIndChargerConnected | CTelephony::KIndNetworkAvailable | CTelephony::KIndCallInProgress;
	indExpNotifierStatus2.iIndicator = CTelephony::KIndChargerConnected | CTelephony::KIndNetworkAvailable;
	IndicatorChangeAct->iExpNotificationCompleteList.Append(indExpNotifierStatusPckg2);
	
	CTelephony::TIndicatorV1 indExpNotifierStatus3;
	CTelephony::TIndicatorV1Pckg indExpNotifierStatusPckg3(indExpNotifierStatus3);	
	indExpNotifierStatus3.iCapabilities = CTelephony::KIndChargerConnected | CTelephony::KIndNetworkAvailable | CTelephony::KIndCallInProgress;
	indExpNotifierStatus3.iIndicator = CTelephony::KIndChargerConnected;
	IndicatorChangeAct->iExpNotificationCompleteList.Append(indExpNotifierStatusPckg3);
	
	numberNotifierTestsActive = 0;
	IndicatorChangeAct->StartNotifier(*indNotifierStatusData, KErrNone, ETrue);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(IndicatorChangeAct);

	//Notification Test 4: EBatteryInfoChange
	SetTestNumber(18);
	INFO_PRINTF1(_L("Testing EBatteryInfoChange notification"));
	CTelephony::TBatteryInfoV1 batNotifierStatus;
	CTelephony::TBatteryInfoV1Pckg batNotifierStatusPckg(batNotifierStatus);	
	
	TDes8* batNotifierStatusData;
	if (iBufferMode)
		{
		batNotifierStatusData = &iBatNotifierStatusBuff;
		}
	else
		{
		batNotifierStatusData = &batNotifierStatusPckg;
		}
	
	CTestRegisterNotifierAct* batteryInfoChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::EBatteryInfoChange);

	CTelephony::TBatteryInfoV1 batExpNotifierStatus1;
	CTelephony::TBatteryInfoV1Pckg batExpNotifierStatusPckg1(batExpNotifierStatus1);	
	batExpNotifierStatus1.iChargeLevel = 2;
	batExpNotifierStatus1.iStatus = CTelephony::EBatteryConnectedButExternallyPowered;
	batteryInfoChangeAct->iExpNotificationCompleteList.Append(batExpNotifierStatusPckg1);

	#if defined(_DEBUG)	
	//test OOM case
		numberNotifierTestsActive = 0;
		__UHEAP_FAILNEXT(1);
		batteryInfoChangeAct->StartNotifier(*batNotifierStatusData, KErrNoMemory);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
	
	CTelephony::TBatteryInfoV1 batExpNotifierStatus2;
	CTelephony::TBatteryInfoV1Pckg batExpNotifierStatusPckg2(batExpNotifierStatus2);	
	batExpNotifierStatus2.iChargeLevel = 3;
	batExpNotifierStatus2.iStatus = CTelephony::ENoBatteryConnected;
	batteryInfoChangeAct->iExpNotificationCompleteList.Append(batExpNotifierStatusPckg2);	

	numberNotifierTestsActive = 0;
	batteryInfoChangeAct->StartNotifier(*batNotifierStatusData, KErrNone, ETrue);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(batteryInfoChangeAct);
	
	//Notification Test 5: ESignalStrengthChange
	SetTestNumber(19);
	INFO_PRINTF1(_L("Testing ESignalStrengthChange notification"));
	CTelephony::TSignalStrengthV1 sigNotifierStatus;
	CTelephony::TSignalStrengthV1Pckg sigNotifierStatusPckg(sigNotifierStatus);	
	
	TDes8* sigNotifierStatusData;
	if (iBufferMode)
		{
		sigNotifierStatusData = &iSigNotifierStatusBuff;
		}
	else
		{
		sigNotifierStatusData = &sigNotifierStatusPckg;
		}
		
	CTestRegisterNotifierAct* signalStrengthChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::ESignalStrengthChange);

	CTelephony::TSignalStrengthV1 sigExpNotifierStatus1;
	CTelephony::TSignalStrengthV1Pckg sigExpNotifierStatusPckg1(sigExpNotifierStatus1);	
	sigExpNotifierStatus1.iBar = 3;
	sigExpNotifierStatus1.iSignalStrength = -51;
	signalStrengthChangeAct->iExpNotificationCompleteList.Append(sigExpNotifierStatusPckg1);
	
	#if defined(_DEBUG)
	//test OOM case
		numberNotifierTestsActive = 0;
		__UHEAP_FAILNEXT(1);
		signalStrengthChangeAct->StartNotifier(*sigNotifierStatusData, KErrNoMemory);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
	
	CTelephony::TSignalStrengthV1 sigExpNotifierStatus2;
	CTelephony::TSignalStrengthV1Pckg sigExpNotifierStatusPckg2(sigExpNotifierStatus2);	
	sigExpNotifierStatus2.iBar = 2;
	sigExpNotifierStatus2.iSignalStrength = -40;
	signalStrengthChangeAct->iExpNotificationCompleteList.Append(sigExpNotifierStatusPckg2);

	numberNotifierTestsActive = 0;
	signalStrengthChangeAct->StartNotifier(*sigNotifierStatusData,KErrNone, ETrue);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(signalStrengthChangeAct);

	//Notification Test 6: EPin1LockInfoChange
	SetTestNumber(20);
	INFO_PRINTF1(_L("Testing EPin1LockInfoChange notification"));
	CTelephony::TIccLockInfoV1 locNotifierStatus;
	CTelephony::TIccLockInfoV1Pckg locNotifierStatusPckg(locNotifierStatus);
	
	TDes8* locNotifierStatusData;
	if (iBufferMode)
		{
		locNotifierStatusData = &iLocNotifierStatusBuff;
		}
	else
		{
		locNotifierStatusData = &locNotifierStatusPckg;
		}
	
	CTestRegisterNotifierAct* pin1lockInfoChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::EPin1LockInfoChange);
	
	CTelephony::TIccLockInfoV1 locExpNotifierStatus1;
	CTelephony::TIccLockInfoV1Pckg locExpNotifierStatusPckg1(locExpNotifierStatus1);	
	locExpNotifierStatus1.iSetting = CTelephony::ELockSetEnabled;
	locExpNotifierStatus1.iStatus = CTelephony::EStatusUnlocked;
	pin1lockInfoChangeAct->iExpNotificationCompleteList.Append(locExpNotifierStatusPckg1);
	
	#if defined(_DEBUG)
	//test OOM case
		numberNotifierTestsActive = 0;
		__UHEAP_FAILNEXT(1);
		pin1lockInfoChangeAct->StartNotifier(*locNotifierStatusData, KErrNoMemory);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif

	//try initiating both pin1 & pin2 request notification. The test should pass.
	CTestRegisterNotifierAct* temppin2lockInfoChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::EPin2LockInfoChange);
	temppin2lockInfoChangeAct->iExpNotificationCompleteList.Append(locExpNotifierStatusPckg1);
		
	CTelephony::TIccLockInfoV1 locExpNotifierStatus2;
	CTelephony::TIccLockInfoV1Pckg locExpNotifierStatusPckg2(locExpNotifierStatus2);	
	locExpNotifierStatus2.iSetting = CTelephony::ELockSetDisabled;
	locExpNotifierStatus2.iStatus = CTelephony::EStatusUnlocked;
	pin1lockInfoChangeAct->iExpNotificationCompleteList.Append(locExpNotifierStatusPckg2);	
	
	numberNotifierTestsActive = 0;
	pin1lockInfoChangeAct->StartNotifier(*locNotifierStatusData, KErrNone, ETrue);
	temppin2lockInfoChangeAct->StartNotifier(*locNotifierStatusData, KErrNone, ETrue);
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(temppin2lockInfoChangeAct);
	CleanupStack::PopAndDestroy(pin1lockInfoChangeAct);

	//Notification Test 6b: EPin2LockInfoChange
	INFO_PRINTF1(_L("Testing EPin2LockInfoChange notification"));
	CTestRegisterNotifierAct* pin2lockInfoChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::EPin2LockInfoChange);
	
	locExpNotifierStatus1.iSetting = CTelephony::ELockSetDisabled;
	locExpNotifierStatus1.iStatus = CTelephony::EStatusLocked;
	pin2lockInfoChangeAct->iExpNotificationCompleteList.Append(locExpNotifierStatusPckg1);	
	
	locExpNotifierStatus2.iSetting = CTelephony::ELockSetUnknown;
	locExpNotifierStatus2.iStatus = CTelephony::EStatusLocked;
	pin2lockInfoChangeAct->iExpNotificationCompleteList.Append(locExpNotifierStatusPckg2);	
	
	numberNotifierTestsActive = 0;	
	pin2lockInfoChangeAct->StartNotifier(*locNotifierStatusData, KErrNone, ETrue);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(pin2lockInfoChangeAct);
	

	//Notification Test 7: EVoiceLineStatusChange
	SetTestNumber(21);
	INFO_PRINTF1(_L("Testing EVoiceLineStatusChange notification"));
	CTelephony::TCallStatusV1 linNotifierStatus;
	CTelephony::TCallStatusV1Pckg linNotifierStatusPckg(linNotifierStatus);	

	TDes8* linNotifierStatusData;
	if (iBufferMode)
		{
		linNotifierStatusData = &iLinNotifierStatusBuff;
		}
	else
		{
		linNotifierStatusData = &linNotifierStatusPckg;
		}
		
	CTestRegisterNotifierAct* lineStatusChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::EVoiceLineStatusChange);

	CTelephony::TCallStatusV1 linExpNotifierStatus1;
	CTelephony::TCallStatusV1Pckg linExpNotifierStatusPckg1(linExpNotifierStatus1);	
	linExpNotifierStatus1.iStatus = CTelephony::EStatusDialling;
	lineStatusChangeAct->iExpNotificationCompleteList.Append(linExpNotifierStatusPckg1);
	
	CTelephony::TCallStatusV1 linExpNotifierStatus2;
	CTelephony::TCallStatusV1Pckg linExpNotifierStatusPckg2(linExpNotifierStatus2);	
	linExpNotifierStatus2.iStatus = CTelephony::EStatusConnecting;
	lineStatusChangeAct->iExpNotificationCompleteList.Append(linExpNotifierStatusPckg2);
	
	CTelephony::TCallStatusV1 linExpNotifierStatus3;
	CTelephony::TCallStatusV1Pckg linExpNotifierStatusPckg3(linExpNotifierStatus3);	
	linExpNotifierStatus3.iStatus = CTelephony::EStatusConnected;
	lineStatusChangeAct->iExpNotificationCompleteList.Append(linExpNotifierStatusPckg3);	
	
	CTelephony::TCallStatusV1 linExpNotifierStatus4;
	CTelephony::TCallStatusV1Pckg linExpNotifierStatusPckg4(linExpNotifierStatus4);	
	linExpNotifierStatus4.iStatus = CTelephony::EStatusDisconnecting;
	lineStatusChangeAct->iExpNotificationCompleteList.Append(linExpNotifierStatusPckg4);
	
	CTelephony::TCallStatusV1 linExpNotifierStatus5;
	CTelephony::TCallStatusV1Pckg linExpNotifierStatusPckg5(linExpNotifierStatus5);	
	linExpNotifierStatus5.iStatus = CTelephony::EStatusIdle;
	lineStatusChangeAct->iExpNotificationCompleteList.Append(linExpNotifierStatusPckg5);	
	
	numberNotifierTestsActive = 0;
	lineStatusChangeAct->StartNotifier(*linNotifierStatusData, KErrNone, ETrue);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(lineStatusChangeAct);

	//Notification Test 7a: EFaxLineStatusChange & EDataLineStatusChange
	INFO_PRINTF1(_L("Testing EFaxLineStatusChange & EDataLineStatusChange notification"));
	TRequestStatus faxDataNotifierStatus(0);
	CTelephony::TCallStatusV1 faxDataLinExpNotifierStatus;
	CTelephony::TCallStatusV1Pckg faxDataLinExpNotifierStatusPckg(faxDataLinExpNotifierStatus);
	
	TDes8* faxDataLinExpNotifierStatusData;
	if (iBufferMode)
		{
		faxDataLinExpNotifierStatusData = &iFaxDataLinExpNotifierStatusBuff;
		}
	else
		{
		faxDataLinExpNotifierStatusData = &faxDataLinExpNotifierStatusPckg;
		}
	

	CTelephony::TNotificationEvent telEvent = CTelephony::EFaxLineStatusChange;
	iTelephony->NotifyChange(faxDataNotifierStatus, telEvent, *faxDataLinExpNotifierStatusData);
	User::WaitForRequest(faxDataNotifierStatus);
	TEST(faxDataNotifierStatus == KErrNotSupported);

	faxDataNotifierStatus = 0;
	telEvent = CTelephony::EDataLineStatusChange;
	iTelephony->NotifyChange(faxDataNotifierStatus, telEvent, *faxDataLinExpNotifierStatusData);
	User::WaitForRequest(faxDataNotifierStatus);
	TEST(faxDataNotifierStatus == KErrNotSupported);

	//Notification Test 8: EOwnedCall1CallStatusChange
	//don't need to set test number as previous test (line notifier) is the same
	INFO_PRINTF1(_L("Testing EOwnedCall1CallStatusChange notification"));
	CTelephony::TCallStatusV1 calNotifierStatus;
	CTelephony::TCallStatusV1Pckg calNotifierStatusPckg(calNotifierStatus);	
	TDes8* calNotifierStatusData;
	if (iBufferMode)
		{
		calNotifierStatusData = &iCalNotifierStatusBuff;
		}
	else
		{
		calNotifierStatusData = &calNotifierStatusPckg;
		}
		
	CTestRegisterNotifierAct* call1StatusChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::EOwnedCall1StatusChange);

	CTelephony::TCallStatusV1 calExpNotifierStatus1;
	CTelephony::TCallStatusV1Pckg calExpNotifierStatusPckg1(calExpNotifierStatus1);	
		
	calExpNotifierStatus1.iStatus = CTelephony::EStatusDisconnecting;
	call1StatusChangeAct->iExpNotificationCompleteList.Append(calExpNotifierStatusPckg1);
	
	CTelephony::TCallStatusV1 calExpNotifierStatus2;
	CTelephony::TCallStatusV1Pckg calExpNotifierStatusPckg2(calExpNotifierStatus2);	
	calExpNotifierStatus2.iStatus = CTelephony::EStatusIdle;
	call1StatusChangeAct->iExpNotificationCompleteList.Append(calExpNotifierStatusPckg2);	
		
	//make call
	call1StatusChangeAct->MakeCallL();
	CActiveScheduler::Start();

	numberNotifierTestsActive = 0;
	call1StatusChangeAct->StartNotifier(*calNotifierStatusData, KErrNone, ETrue);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(call1StatusChangeAct);
	
	//Notification Test 8b: EOwnedCall2CallStatusChange
	//don't need to set test number as previous test (line notifier) is the same
	INFO_PRINTF1(_L("Testing EOwnedCall2CallStatusChange notification"));
	CTestRegisterNotifierAct* call2StatusChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::EOwnedCall2StatusChange);

	calExpNotifierStatus1.iStatus = CTelephony::EStatusDisconnecting;
	call2StatusChangeAct->iExpNotificationCompleteList.Append(calExpNotifierStatusPckg1);
	
	calExpNotifierStatus2.iStatus = CTelephony::EStatusIdle;
	call2StatusChangeAct->iExpNotificationCompleteList.Append(calExpNotifierStatusPckg2);
	
	//make first call
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	CTelephony::TCallStatusV1 status;
	CTelephony::TCallStatusV1Pckg statusPckg(status);
	
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = _L("1234");
	testCallParams1.iExpStat = KErrNone;
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);	
		
	// Create a call - should succeed
	testCallParams1.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	//should be conected
	TInt err = iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg);
	TEST1(err == KErrNone, ETrue);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
	//put first call on hold
	CTestHoldAct* TestHoldAct = CTestHoldAct::NewLC(this, iTelephony);	
	TestHoldAct->TestHold(testCallParams1);
	CActiveScheduler::Start();	
	
	//make second call
	call2StatusChangeAct->MakeCallL();
	CActiveScheduler::Start();
	
	//hangup first call
	CTestHangupAct* testHangupAct1 = CTestHangupAct::NewLC(this, iTelephony);

	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(testHangupAct1);
	CleanupStack::PopAndDestroy(TestHoldAct);
	CleanupStack::PopAndDestroy(testDialNewCallAct1);
	
	err = iTelephony->GetLineStatus(testCallParams1.iLine,  statusPckg);
	TEST1(err == KErrNone, ETrue);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);	
	
	//set notifier going for the second owned call
	numberNotifierTestsActive = 0;
	call2StatusChangeAct->StartNotifier(*calNotifierStatusData, KErrNone);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(call2StatusChangeAct);

	//Notification Test 9: EOwnedCall1RemotePartyInfoChange
	SetTestNumber(22);
	INFO_PRINTF1(_L("Testing EOwnedCall1RemotePartyInfoChange notification"));
	CTelephony::TRemotePartyInfoV1 remNotifierStatus;
	CTelephony::TRemotePartyInfoV1Pckg remNotifierStatusPckg(remNotifierStatus);	
		
	TDes8* remNotifierStatusData;
	if (iBufferMode)
		{
		remNotifierStatusData = &iRemNotifierStatusBuff;
		}
	else
		{
		remNotifierStatusData = &remNotifierStatusPckg;
		}
		
	CTestRegisterNotifierAct*call1RemotePartyInfoChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::EOwnedCall1RemotePartyInfoChange);

	CTelephony::TRemotePartyInfoV1 remExpNotifierStatus1;
	CTelephony::TRemotePartyInfoV1Pckg remExpNotifierStatusPckg1(remExpNotifierStatus1);	
	remExpNotifierStatus1.iCallingName = _L("call0");
	remExpNotifierStatus1.iDirection = CTelephony::EMobileOriginated;
	remExpNotifierStatus1.iRemoteIdStatus = CTelephony::ERemoteIdentityAvailable;
	remExpNotifierStatus1.iRemoteNumber.iTypeOfNumber = CTelephony::EInternationalNumber;
	remExpNotifierStatus1.iRemoteNumber.iNumberPlan = CTelephony::EIsdnNumberPlan;
	remExpNotifierStatus1.iRemoteNumber.iTelNumber = _L("12345678");
	call1RemotePartyInfoChangeAct->iExpNotificationCompleteList.Append(remExpNotifierStatusPckg1);
	
	//make call
	call1RemotePartyInfoChangeAct->MakeCallL();
	CActiveScheduler::Start();
	
	#if defined(_DEBUG)
	//test OOM case
		numberNotifierTestsActive = 0;
		__UHEAP_FAILNEXT(1);
		call1RemotePartyInfoChangeAct->StartNotifier(*remNotifierStatusData, KErrNoMemory);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
	
	numberNotifierTestsActive = 0;
	call1RemotePartyInfoChangeAct->StartNotifier(*remNotifierStatusData, KErrNone, ETrue);
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(call1RemotePartyInfoChangeAct);	

	//Notification Test 9b: EOwnedCall2RemotePartyInfoChange
	INFO_PRINTF1(_L("Testing EOwnedCall2RemotePartyInfoChange notification"));
	//put first call on hold
	TestHoldAct = CTestHoldAct::NewLC(this, iTelephony);	
	TestHoldAct->TestHold(testCallParams1);
	CActiveScheduler::Start();	
	CleanupStack::PopAndDestroy(TestHoldAct);
	
	err = iTelephony->GetLineStatus(testCallParams1.iLine,  statusPckg);
	TEST1(err == KErrNone, ETrue);
	TEST1(status.iStatus == CTelephony::EStatusHold, ETrue);
	
	//make 2nd call
	testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(testDialNewCallAct1);
	
	err = iTelephony->GetLineStatus(testCallParams1.iLine,  statusPckg);
	TEST1(err == KErrNone, ETrue);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
	CTestRegisterNotifierAct*call2RemotePartyInfoChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::EOwnedCall2RemotePartyInfoChange);
	
	//use same parameters as last test.
	call2RemotePartyInfoChangeAct->iExpNotificationCompleteList.Append(remExpNotifierStatusPckg1);
	
	numberNotifierTestsActive = 0;
	call2RemotePartyInfoChangeAct->StartNotifier(*remNotifierStatusData, KErrNone);
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(call2RemotePartyInfoChangeAct);	
	
	//Notification Test 10: ENetworkRegistrationStatusChange
	SetTestNumber(23);
	INFO_PRINTF1(_L("Testing ENetworkRegistrationStatusChange notification"));
	CTelephony::TNetworkRegistrationV1 regNotifierStatus;
	CTelephony::TNetworkRegistrationV1Pckg regNotifierStatusPckg(regNotifierStatus);	
	
	TDes8* regNotifierStatusData;
	if (iBufferMode)
		{
		regNotifierStatusData = &iRegNotifierStatusBuff;
		}
	else
		{
		regNotifierStatusData = &regNotifierStatusPckg;
		}
	
	CTestRegisterNotifierAct*NetworkRegistrationChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::ENetworkRegistrationStatusChange);
	
	CTelephony::TNetworkRegistrationV1 regExpNotifierStatus1;
	CTelephony::TNetworkRegistrationV1Pckg regExpNotifierStatusPckg1(regExpNotifierStatus1);	
	regExpNotifierStatus1.iRegStatus = CTelephony::ENotRegisteredNoService;
	NetworkRegistrationChangeAct->iExpNotificationCompleteList.Append(regExpNotifierStatusPckg1);
	
	#if defined(_DEBUG)
	//test OOM case
		numberNotifierTestsActive = 0;
		__UHEAP_FAILNEXT(1);
		NetworkRegistrationChangeAct->StartNotifier(*regNotifierStatusData, KErrNoMemory);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
	
	CTelephony::TNetworkRegistrationV1 regExpNotifierStatus2;
	CTelephony::TNetworkRegistrationV1Pckg regExpNotifierStatusPckg2(regExpNotifierStatus2);	
	regExpNotifierStatus2.iRegStatus = CTelephony::ENotRegisteredSearching;
	NetworkRegistrationChangeAct->iExpNotificationCompleteList.Append(regExpNotifierStatusPckg2);
	
	CTelephony::TNetworkRegistrationV1 regExpNotifierStatus3;
	CTelephony::TNetworkRegistrationV1Pckg regExpNotifierStatusPckg3(regExpNotifierStatus3);	
	regExpNotifierStatus3.iRegStatus = CTelephony::ERegisteredOnHomeNetwork;
	NetworkRegistrationChangeAct->iExpNotificationCompleteList.Append(regExpNotifierStatusPckg3);
		
	numberNotifierTestsActive = 0;
	NetworkRegistrationChangeAct->StartNotifier(*regNotifierStatusData, KErrNone, ETrue);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(NetworkRegistrationChangeAct);

	//Notification Test 11: ENetworkInfoChange
	SetTestNumber(24);
	INFO_PRINTF1(_L("Testing ENetworkInfoChange notification"));
	CTelephony::TNetworkInfoV1 netNotifierStatus;
	CTelephony::TNetworkInfoV1Pckg netNotifierStatusPckg(netNotifierStatus);	
		
	TDes8* netNotifierStatusData;
	if (iBufferMode)
		{
		netNotifierStatusData = &iNetNotifierStatusBuff;
		}
	else
		{
		netNotifierStatusData = &netNotifierStatusPckg;
		}
		
	CTestRegisterNotifierAct*NetworkInfoChangeAct = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::ECurrentNetworkInfoChange);
	NetworkInfoChangeAct->SetNetworkInfoVersion(1);
	
	CTelephony::TNetworkInfoV1 netExpNotifierStatus1;
	CTelephony::TNetworkInfoV1Pckg netExpNotifierStatusPckg1(netExpNotifierStatus1);	
	netExpNotifierStatus1.iCountryCode=_L("ABCD");
	netExpNotifierStatus1.iNetworkId=_L("Network0");
	netExpNotifierStatus1.iDisplayTag=_L("Vodafone");
	netExpNotifierStatus1.iShortName=_L("Voda");
	netExpNotifierStatus1.iLongName=_L("Vodafone_UK");	
	netExpNotifierStatus1.iMode=CTelephony::ENetworkModeGsm;
	netExpNotifierStatus1.iStatus = CTelephony::ENetworkStatusUnknown;
	netExpNotifierStatus1.iBandInfo=CTelephony::EBandUnknown;
	netExpNotifierStatus1.iCdmaSID.SetLength(0);
	netExpNotifierStatus1.iAccess = CTelephony::ENetworkAccessGsm; //If v1=ENetworkAccessUnknown, if V2=ENetworkAccessGsm
	netExpNotifierStatus1.iAreaKnown = EFalse;
	netExpNotifierStatus1.iLocationAreaCode = 0;
	netExpNotifierStatus1.iCellId = 0;
	NetworkInfoChangeAct->iExpNotificationCompleteList.Append(netExpNotifierStatusPckg1);
	
	#if defined(_DEBUG)
	//test OOM case
		numberNotifierTestsActive = 0;
		__UHEAP_FAILNEXT(1);
		NetworkInfoChangeAct->StartNotifier(*netNotifierStatusData, KErrNoMemory);
		CActiveScheduler::Start();
		__UHEAP_RESET;
	#endif
	
	CTelephony::TNetworkInfoV1 netExpNotifierStatus2;
	CTelephony::TNetworkInfoV1Pckg netExpNotifierStatusPckg2(netExpNotifierStatus2);	
	netExpNotifierStatus2 = netExpNotifierStatus1;
	netExpNotifierStatus2.iCountryCode=_L("EFGH");
	netExpNotifierStatus2.iNetworkId=_L("Network1");
	netExpNotifierStatus2.iDisplayTag=_L("Cellnet");
	netExpNotifierStatus2.iShortName=_L("Cell");
	netExpNotifierStatus2.iLongName=_L("Cellnet_UK");	
	netExpNotifierStatus2.iStatus = CTelephony::ENetworkStatusAvailable;
	NetworkInfoChangeAct->iExpNotificationCompleteList.Append(netExpNotifierStatusPckg2);
	
	CTelephony::TNetworkInfoV1 netExpNotifierStatus3;
	CTelephony::TNetworkInfoV1Pckg netExpNotifierStatusPckg3(netExpNotifierStatus3);	
	netExpNotifierStatus3 = netExpNotifierStatus1;
	netExpNotifierStatus3.iCountryCode=_L("IJKL");
	netExpNotifierStatus3.iNetworkId=_L("Network2");
	netExpNotifierStatus3.iDisplayTag=_L("One2One");
	netExpNotifierStatus3.iShortName=_L("121");
	netExpNotifierStatus3.iLongName=_L("One2One_UK");	
	netExpNotifierStatus3.iStatus = CTelephony::ENetworkStatusCurrent;
	NetworkInfoChangeAct->iExpNotificationCompleteList.Append(netExpNotifierStatusPckg3);
	
	numberNotifierTestsActive = 0;
	NetworkInfoChangeAct->StartNotifier(*netNotifierStatusData, KErrNone, ETrue);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(NetworkInfoChangeAct);
	
	// V2
	INFO_PRINTF1(_L("Testing ENetworkInfoChange notification using TNetworkInfoV2"));
	CTelephony::TNetworkInfoV2 netNotifierStatusV2;
	CTelephony::TNetworkInfoV2Pckg netNotifierStatusPckgV2(netNotifierStatusV2);
	TDes8* netNotifierStatusDataV2;
	if (iBufferMode)
		{
		netNotifierStatusDataV2 = &iNetNotifierStatusBuffV2;
		}
	else
		{
		netNotifierStatusDataV2 = &netNotifierStatusPckgV2;
		}
		
	CTestRegisterNotifierAct*NetworkInfoChangeActV2 = CTestRegisterNotifierAct::NewLC(this, iTelephony, numberNotifierTestsActive, CTelephony::ECurrentNetworkInfoChange);
	NetworkInfoChangeAct->SetNetworkInfoVersion(2);
	
	CTelephony::TNetworkInfoV2 netExpNotifierStatus4;
	CTelephony::TNetworkInfoV2Pckg netExpNotifierStatusPckg4(netExpNotifierStatus4);
	netExpNotifierStatus4.iCountryCode=_L("QRST");
	netExpNotifierStatus4.iNetworkId=_L("Network4");
	netExpNotifierStatus4.iDisplayTag=_L("Symbian");
	netExpNotifierStatus4.iShortName=_L("SymbianNe");
	netExpNotifierStatus4.iLongName=_L("SymbianNetwor");	
	netExpNotifierStatus4.iMode=CTelephony::ENetworkModeGsm;
	netExpNotifierStatus4.iStatus = CTelephony::ENetworkStatusUnknown;
	netExpNotifierStatus4.iBandInfo=CTelephony::EBandUnknown;
	netExpNotifierStatus4.iCdmaSID.SetLength(0);
	netExpNotifierStatus4.iAccess = CTelephony::ENetworkAccessGsm; //If v1=ENetworkAccessUnknown, if V2=ENetworkAccessGsm
	netExpNotifierStatus4.iAreaKnown = EFalse;
	netExpNotifierStatus4.iLocationAreaCode = 0;
	netExpNotifierStatus4.iCellId = 0;
	NetworkInfoChangeActV2->iExpNotificationCompleteList.Append(netExpNotifierStatusPckg4);
	
	numberNotifierTestsActive = 0;
	NetworkInfoChangeActV2->StartNotifier(*netNotifierStatusDataV2, KErrNone, ETrue);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(NetworkInfoChangeActV2);
	
	return TestStepResult();	
	}

CTestNotifyChangeCancel::CTestNotifyChangeCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestRegisterForNotificationCancel"));
	}

TVerdict CTestNotifyChangeCancel::doTestStepL()
{
	INFO_PRINTF1(_L("Test cancelling notifications"));
	CTestCanceller* canceller = NULL;

//Cancel Notification Test 1: EFlightModeChange
	SetTestNumber(25);	
	INFO_PRINTF1(_L("Testing EFlightModeChange notification cancel"));
	CTelephony::TFlightModeV1 fmNotifierStatus;
	CTelephony::TFlightModeV1Pckg fmNotifierStatusPckg(fmNotifierStatus);	
	CTestCancelRegisterNotifierAct* FlightModeChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::EFlightModeChange);
	 canceller = FlightModeChangeAct->RetrieveCanceller();

	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EFlightModeChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	FlightModeChangeAct->StartNotifier(fmNotifierStatusPckg, KErrNone);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(FlightModeChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(FlightModeChangeAct);
	
//Notification Test 2: EIndicatorChange
	SetTestNumber(26);	
	INFO_PRINTF1(_L("Testing EIndicatorChange notification cancel"));
	CTelephony::TIndicatorV1 indNotifierStatus;
	CTelephony::TIndicatorV1Pckg indNotifierStatusPckg(indNotifierStatus);	
	CTestCancelRegisterNotifierAct* IndicatorChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::EIndicatorChange);
	canceller = IndicatorChangeAct->RetrieveCanceller();
	
	cancelerr = iTelephony->CancelAsync(CTelephony::EIndicatorChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	IndicatorChangeAct->StartNotifier(indNotifierStatusPckg, KErrNone);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(IndicatorChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(IndicatorChangeAct);
	
//Cancel Notification Test 3: EBatteryInfoChange
	SetTestNumber(27);	
	INFO_PRINTF1(_L("Testing EBatteryInfoChange notification cancel"));
	CTelephony::TBatteryInfoV1 batNotifierStatus;
	CTelephony::TBatteryInfoV1Pckg batNotifierStatusPckg(batNotifierStatus);	
	CTestCancelRegisterNotifierAct* batteryInfoChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::EBatteryInfoChange);
	canceller = batteryInfoChangeAct->RetrieveCanceller();
	
	cancelerr = iTelephony->CancelAsync(CTelephony::EBatteryInfoChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	batteryInfoChangeAct->StartNotifier(batNotifierStatusPckg, KErrNone);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(batteryInfoChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(batteryInfoChangeAct);

//Cancel Notification Test 4: ESignalStrengthChange
	SetTestNumber(28);
	INFO_PRINTF1(_L("Testing ESignalStrengthChange notification cancel"));
	CTelephony::TSignalStrengthV1 sigNotifierStatus;
	CTelephony::TSignalStrengthV1Pckg sigNotifierStatusPckg(sigNotifierStatus);	
	CTestCancelRegisterNotifierAct* signalStrengthChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::ESignalStrengthChange);
	
	cancelerr = iTelephony->CancelAsync(CTelephony::ESignalStrengthChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	canceller = signalStrengthChangeAct->RetrieveCanceller();
	signalStrengthChangeAct->StartNotifier(sigNotifierStatusPckg, KErrNone);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(signalStrengthChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(signalStrengthChangeAct);

//Cancel Notification Test 5a: EPin1LockInfoChange
	SetTestNumber(29);
	INFO_PRINTF1(_L("Testing EPin1LockInfoChange notification cancel"));
	CTelephony::TIccLockInfoV1 locNotifierStatus;
	CTelephony::TIccLockInfoV1Pckg locNotifierStatusPckg(locNotifierStatus);	
	CTestCancelRegisterNotifierAct* pin1lockInfoChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::EPin1LockInfoChange);
	
	cancelerr = iTelephony->CancelAsync(CTelephony::EPin1LockInfoChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	canceller = pin1lockInfoChangeAct->RetrieveCanceller();
	pin1lockInfoChangeAct->StartNotifier(locNotifierStatusPckg, KErrNone);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(pin1lockInfoChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(pin1lockInfoChangeAct);
	
//Cancel Notification Test 5b: EPin2LockInfoChange
	SetTestNumber(29);
	INFO_PRINTF1(_L("Testing EPin2LockInfoChange notification cancel"));
	CTestCancelRegisterNotifierAct* pin2lockInfoChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::EPin2LockInfoChange);
	
	cancelerr = iTelephony->CancelAsync(CTelephony::EPin2LockInfoChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	canceller = pin2lockInfoChangeAct->RetrieveCanceller();
	pin2lockInfoChangeAct->StartNotifier(locNotifierStatusPckg, KErrNone);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(pin2lockInfoChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(pin2lockInfoChangeAct);

//Cancel Notification Test 6: EVoiceLineStatusChange
	SetTestNumber(30);
	INFO_PRINTF1(_L("Testing EVoiceLineStatusChange notification cancel"));
	CTelephony::TCallStatusV1 linNotifierStatus;
	CTelephony::TCallStatusV1Pckg linNotifierStatusPckg(linNotifierStatus);	
	CTestCancelRegisterNotifierAct* lineStatusChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::EVoiceLineStatusChange);
	
	cancelerr = iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	canceller = lineStatusChangeAct->RetrieveCanceller();
	lineStatusChangeAct->StartNotifier(linNotifierStatusPckg, KErrNone);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(lineStatusChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(lineStatusChangeAct);

//Cancel Notification Test 6a: EFaxLineStatusChange & EDataLineStatusChange
	INFO_PRINTF1(_L("Testing EFaxLineStatusChange & EDataLineStatusChange notification"));
	TRequestStatus faxDataNotifierStatus(0);
	CTelephony::TCallStatusV1 faxDataLinExpNotifierStatus;
	CTelephony::TCallStatusV1Pckg faxDataLinExpNotifierStatusPckg(faxDataLinExpNotifierStatus);

	CTelephony::TNotificationEvent telEvent = CTelephony::EFaxLineStatusChange;
	iTelephony->NotifyChange(faxDataNotifierStatus, telEvent, faxDataLinExpNotifierStatusPckg);
	cancelerr = iTelephony->CancelAsync(CTelephony::EFaxLineStatusChangeCancel);
	User::WaitForRequest(faxDataNotifierStatus);
	TEST(faxDataNotifierStatus == KErrNotSupported);
	TEST(cancelerr == KErrNotSupported);

	faxDataNotifierStatus = 0;
	cancelerr = 0;
	telEvent = CTelephony::EDataLineStatusChange;
	iTelephony->NotifyChange(faxDataNotifierStatus, telEvent, faxDataLinExpNotifierStatusPckg);
	cancelerr = iTelephony->CancelAsync(CTelephony::EDataLineStatusChangeCancel);
	User::WaitForRequest(faxDataNotifierStatus);
	TEST(faxDataNotifierStatus == KErrNotSupported);
	TEST(cancelerr == KErrNotSupported);

//Cancel Notification Test 7a: EOwnedCall1CallStatusChange
	SetTestNumber(31); 
	INFO_PRINTF1(_L("Testing EOwnedCall1CallStatusChange notification cancel"));
	
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);
	
	CTelephony::TCallStatusV1 calNotifierStatus;
	CTelephony::TCallStatusV1Pckg calNotifierStatusPckg(calNotifierStatus);	
	CTestCancelRegisterNotifierAct* call1StatusChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::EOwnedCall1StatusChange);
//make call
	//Set the call parameters for call 1
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = _L("1789");
	testCallParams1.iExpStat = KErrNone;
	
	testCallParams1.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	CTelephony::TCallStatusV1 status;
	CTelephony::TCallStatusV1Pckg statusPckg(status);		
	iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);	
	
	cancelerr = iTelephony->CancelAsync(CTelephony::EOwnedCall1StatusChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	canceller = call1StatusChangeAct->RetrieveCanceller();
	call1StatusChangeAct->StartNotifier(calNotifierStatusPckg, KErrNone);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(call1StatusChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(call1StatusChangeAct);
		
//Cancel Notification Test 7b: EOwnedCall2CallStatusChange
	//don't need to set test number as previous test is the same
	INFO_PRINTF1(_L("Testing EOwnedCall2CallStatusChange notification cancel"));
	
	CTestCancelRegisterNotifierAct* call2StatusChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::EOwnedCall2StatusChange);
	
	//put 1st call on hold
	//put first call on hold
	CTestHoldAct* TestHoldAct = CTestHoldAct::NewLC(this, iTelephony);	
	TestHoldAct->TestHold(testCallParams1);
	CActiveScheduler::Start();	
	CleanupStack::PopAndDestroy(TestHoldAct);
	
	iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusHold, ETrue);
	
	//make 2nd call
	CTestDialNewCallAct* testDialNewCallAct2 = CTestDialNewCallAct::NewLC(this, iTelephony);
	
	TTestCallParams testCallParams2;
	testCallParams2.iLine = CTelephony::EVoiceLine;
	testCallParams2.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams2.iTelAddress.iTelNumber = _L("1789");
	testCallParams2.iExpStat = KErrNone;
	
	testCallParams2.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct2->TestDialNewCall(testCallParams2);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(testDialNewCallAct2);
			
	iTelephony->GetCallStatus(testCallParams2.iCallId,  statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);		
	
	//At this point Call1 = EStatusHold & Call2 = EStatusConnected
	
	//cancel the EOwnedCall2StatusChange pending request
	cancelerr = iTelephony->CancelAsync(CTelephony::EOwnedCall2StatusChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	canceller = call1StatusChangeAct->RetrieveCanceller();
	call2StatusChangeAct->StartNotifier(calNotifierStatusPckg, KErrNone);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(call2StatusChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(call2StatusChangeAct);
	CleanupStack::PopAndDestroy(testDialNewCallAct1);

//Cancel Notification Test 8a: EOwnedCall1RemotePartyInfoChange
	INFO_PRINTF1(_L("Testing EOwnedCall1RemotePartyInfoChange notification cancel"));
	CTelephony::TRemotePartyInfoV1 remNotifierStatus;
	CTelephony::TRemotePartyInfoV1Pckg remNotifierStatusPckg(remNotifierStatus);	
	CTestCancelRegisterNotifierAct*call1RemotePartyInfoChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::EOwnedCall1RemotePartyInfoChange);
	
	cancelerr = iTelephony->CancelAsync(CTelephony::EOwnedCall1RemotePartyInfoChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	canceller = call1RemotePartyInfoChangeAct->RetrieveCanceller();
	call1RemotePartyInfoChangeAct->StartNotifier(remNotifierStatusPckg, KErrAccessDenied);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(call1RemotePartyInfoChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(call1RemotePartyInfoChangeAct);
	
//Cancel Notification Test 8b: EOwnedCall2RemotePartyInfoChange

//re-use active & held call from previous test.
	INFO_PRINTF1(_L("Testing EOwnedCall2RemotePartyInfoChange notification cancel"));
	CTestCancelRegisterNotifierAct*call2RemotePartyInfoChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::EOwnedCall2RemotePartyInfoChange);
	
	cancelerr = iTelephony->CancelAsync(CTelephony::EOwnedCall2RemotePartyInfoChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	canceller = call2RemotePartyInfoChangeAct->RetrieveCanceller();
	call2RemotePartyInfoChangeAct->StartNotifier(remNotifierStatusPckg, KErrAccessDenied);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(call2RemotePartyInfoChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(call2RemotePartyInfoChangeAct);

//Cancel Notification Test 9: ENetworkRegistrationStatusChange
	SetTestNumber(32);
	INFO_PRINTF1(_L("Testing ENetworkRegistrationStatusChange notification cancel"));
	CTelephony::TNetworkRegistrationV1 regNotifierStatus;
	CTelephony::TNetworkRegistrationV1Pckg regNotifierStatusPckg(regNotifierStatus);	
	CTestCancelRegisterNotifierAct*NetworkRegistrationChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::ENetworkRegistrationStatusChange);
	
	cancelerr = iTelephony->CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	canceller = NetworkRegistrationChangeAct->RetrieveCanceller();
	NetworkRegistrationChangeAct->StartNotifier(regNotifierStatusPckg, KErrNone);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(NetworkRegistrationChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(NetworkRegistrationChangeAct);

//Cancel Notification Test 10: ENetworkInfoChange
	INFO_PRINTF1(_L("Testing ENetworkInfoChange notification cancel"));
	CTelephony::TNetworkInfoV1 netNotifierStatus;
	CTelephony::TNetworkInfoV1Pckg netNotifierStatusPckg(netNotifierStatus);	
	CTestCancelRegisterNotifierAct*NetworkInfoChangeAct = CTestCancelRegisterNotifierAct::NewLC(this, iTelephony, CTelephony::ECurrentNetworkInfoChange);
	
	cancelerr = iTelephony->CancelAsync(CTelephony::ECurrentNetworkInfoChangeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	canceller = NetworkInfoChangeAct->RetrieveCanceller();
	NetworkInfoChangeAct->StartNotifier(netNotifierStatusPckg, KErrNone);
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(NetworkRegistrationChangeAct->iStatus.Int() == KErrCancel, ETrue);	
	CleanupStack::PopAndDestroy(NetworkInfoChangeAct);	
	
	return TestStepResult();
	}

/**
AO handling notifications.  On receiving a notification it is processed by a specific event handler
*/

CTestRegisterNotifierAct* CTestRegisterNotifierAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TInt& aNumberNotifierTestsActive, CTelephony::TNotificationEvent aEvent)
//Factory constructor
	{
	CTestRegisterNotifierAct* obj = new(ELeave) CTestRegisterNotifierAct(aTestStep, aTelephony, aNumberNotifierTestsActive, aEvent);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}

CTestRegisterNotifierAct::CTestRegisterNotifierAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TInt& aNumberNotifierTestsActive, CTelephony::TNotificationEvent aEvent)
	: CEtelIsvActBase(aTestStep, aTelephony)
	, iEvent(aEvent)
	, iNumberNotifierTestsActive(aNumberNotifierTestsActive)
	, iCallParamsPckg(iCallParams)
	, iCallsActive(EFalse)	
	{  
	}

void CTestRegisterNotifierAct::ConstructL()
	{  
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestRegisterNotifierAct::~CTestRegisterNotifierAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup
	iNumberNotifierTestsActive--;
	delete iCanceller;
	delete iDialNewCallAct;
	delete iHangupAct;
	iExpNotificationCompleteList.Reset();
	}

void CTestRegisterNotifierAct::DoCancel()
	{
	switch(iEvent)
		{
	case CTelephony::EFlightModeChange:
		iTelephony->CancelAsync(CTelephony::EFlightModeChangeCancel);	
		break;
	case CTelephony::EIndicatorChange:
		iTelephony->CancelAsync(CTelephony::EIndicatorChangeCancel);	
		break;
	case CTelephony::EBatteryInfoChange:
		iTelephony->CancelAsync(CTelephony::EBatteryInfoChangeCancel);	
		break;
	case CTelephony::ESignalStrengthChange:
		iTelephony->CancelAsync(CTelephony::ESignalStrengthChangeCancel);	
		break;
	case CTelephony::EPin1LockInfoChange:
		iTelephony->CancelAsync(CTelephony::EPin1LockInfoChangeCancel);	
		break;
	case CTelephony::EPin2LockInfoChange:
		iTelephony->CancelAsync(CTelephony::EPin2LockInfoChangeCancel);	
		break;
	case CTelephony::EVoiceLineStatusChange:
		iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);	
		break;
	case CTelephony::EOwnedCall1StatusChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall1StatusChangeCancel);	
		break;
	case CTelephony::EOwnedCall2StatusChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall2StatusChangeCancel);	
		break;
	case CTelephony::EOwnedCall1RemotePartyInfoChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall1RemotePartyInfoChangeCancel);	
		break;
	case CTelephony::EOwnedCall2RemotePartyInfoChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall2RemotePartyInfoChangeCancel);	
		break;
	case CTelephony::ENetworkRegistrationStatusChange:
		iTelephony->CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);	
		break;
	case CTelephony::ECurrentNetworkInfoChange:
		iTelephony->CancelAsync(CTelephony::ECurrentNetworkInfoChangeCancel);	
		break;
	default:
		break;
		}	
	}
	
void CTestRegisterNotifierAct::StartNotifier(TDes8& aNotifierData, TRequestStatus aExpStat, TBool aCheckConcurrency)
	{
	iNotifierData=&aNotifierData;
	iExpStat=aExpStat;
	iNumberNotificationsBeforeFinish = iExpNotificationCompleteList.Count();
	SendNotifyRequest(aCheckConcurrency);
	TRAP_IGNORE(InvokeNotifyConditionL());
	iNotificationCount = 0;
	iNumberNotifierTestsActive++;
	}
	
void CTestRegisterNotifierAct::InvokeNotifyConditionL()
	{
	switch(iEvent)
		{
	case CTelephony::EVoiceLineStatusChange:
		MakeCallL();
		break;
	case CTelephony::EOwnedCall1StatusChange:
	case CTelephony::EOwnedCall2StatusChange:
		HangupCallL();
		break;
	default:
		break;
		}
	}
	
void CTestRegisterNotifierAct::SendNotifyRequest(TBool aCheckConcurrency)
	{
	iTelephony->NotifyChange(iStatus, iEvent, *iNotifierData);
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->NotifyChange(tmpStatus, iEvent, *iNotifierData);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}	
	SetActive();
	}

void CTestRegisterNotifierAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
	
	if(++iNotificationCount <= iNumberNotificationsBeforeFinish)
		{	//process the notification
		switch(iEvent)
			{
		case CTelephony::EFlightModeChange:
			ProcessFlightModeChangeNotification();
			break;
		case CTelephony::EIndicatorChange:
			ProcessIndicatorNotification();
			break;
		case CTelephony::EBatteryInfoChange:
			ProcessBatteryInfoNotification();
			break;
		case CTelephony::ESignalStrengthChange:
			ProcessSignalStrengthNotification();
			break;
		case CTelephony::EPin1LockInfoChange:
		case CTelephony::EPin2LockInfoChange:
			ProcessLockInfoNotification();
			break;
		case CTelephony::EVoiceLineStatusChange:
			ProcessLineStatusNotification();
			break;
		case CTelephony::EOwnedCall1StatusChange:
		case CTelephony::EOwnedCall2StatusChange:
			ProcessCallStatusNotification();
			break;
		case CTelephony::EOwnedCall1RemotePartyInfoChange:
		case CTelephony::EOwnedCall2RemotePartyInfoChange:
			ProcessRemotePartyInfoNotification();
			break;
		case CTelephony::ENetworkRegistrationStatusChange:
			ProcessNetworkRegistrationStatusNotification();
			break;
		case CTelephony::ECurrentNetworkInfoChange:
			ProcessCurrentNetworkInfoNotification();
			break;
		default:
			break;
			}
		}
	
	// Once we have gone through all our expected status objects then finish the test
	if(iNotificationCount >= iNumberNotificationsBeforeFinish || iStatus.Int() != KErrNone)
		iNumberNotifierTestsActive--;
	else	//post for further notifications
		SendNotifyRequest();
	
	//stop scheduler when all running tests complete
	if(iNumberNotifierTestsActive <= 0 && !iCallsActive)
		CActiveScheduler::Stop();
	}

void CTestRegisterNotifierAct::ProcessFlightModeChangeNotification()
	{
	if(iStatus.Int() == KErrNone)
		{
		//get expected status for this notification		
		TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
		CTelephony::TFlightModeV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TFlightModeV1Pckg&, expnotifierpckg);
		CTelephony::TFlightModeV1& expparams = (exppckg)();
		//get actual notification status
		CTelephony::TFlightModeV1& params = reinterpret_cast<CTelephony::TFlightModeV1&> 
												( const_cast<TUint8&> ( *iNotifierData->Ptr() ) );
		
		iTestStep->TEST1(params.iFlightModeStatus == expparams.iFlightModeStatus, ETrue);
		iTestStep->INFO_PRINTF2(_L("EFlightModeChange Notification ExpStatus: %d"), expparams.iFlightModeStatus);
		iTestStep->INFO_PRINTF2(_L("EFlightModeChange Notification Status: %d"), params.iFlightModeStatus);
		
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for EFlightModeChange Notification: %d"), iStatus.Int() );
		}
	}
	
void CTestRegisterNotifierAct::ProcessIndicatorNotification()
	{
	if(iStatus.Int() == KErrNone)
		{
		//get expected status for this notification		
		TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
		CTelephony::TIndicatorV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TIndicatorV1Pckg&, expnotifierpckg);
		CTelephony::TIndicatorV1& expparams = (exppckg)();
		//get actual notification status
		CTelephony::TIndicatorV1& params = reinterpret_cast<CTelephony::TIndicatorV1&> 
												( const_cast<TUint8&> ( *iNotifierData->Ptr() ) );		
		
		iTestStep->TEST1(expparams.iIndicator == params.iIndicator, ETrue);
		iTestStep->TEST1(expparams.iCapabilities == params.iCapabilities, ETrue);
		iTestStep->INFO_PRINTF2(_L("EIndicatorChange Notification ExpIndicator: %d"), expparams.iIndicator);
		iTestStep->INFO_PRINTF2(_L("EIndicatorChange Notification Indicator: %d"),params.iIndicator);
		iTestStep->INFO_PRINTF2(_L("EIndicatorChange Notification ExpCapability: %d"), expparams.iCapabilities);
		iTestStep->INFO_PRINTF2(_L("EIndicatorChange Notification Capability: %d"),params.iCapabilities);				
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for EIndicatorChange Notification: %d"), iStatus.Int() );
		}
	}
	
void CTestRegisterNotifierAct::ProcessBatteryInfoNotification()
	{
	if(iStatus.Int() == KErrNone)
		{
		//get expected status for this notification		
		TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
		CTelephony::TBatteryInfoV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TBatteryInfoV1Pckg&, expnotifierpckg);
		CTelephony::TBatteryInfoV1& expparams = (exppckg)();
		//get actual notification status
		CTelephony::TBatteryInfoV1& params = reinterpret_cast<CTelephony::TBatteryInfoV1&> 
												( const_cast<TUint8&> ( *iNotifierData->Ptr() ) );
		
		iTestStep->TEST1(expparams.iStatus == params.iStatus, ETrue);
		iTestStep->TEST1(expparams.iChargeLevel == params.iChargeLevel, ETrue);	
		iTestStep->INFO_PRINTF2(_L("EBatteryInfoChange Notification ExpStatus: %d"),expparams.iStatus);	
		iTestStep->INFO_PRINTF2(_L("EBatteryInfoChange Notification Status: %d"),params.iStatus);
		iTestStep->INFO_PRINTF2(_L("EBatteryInfoChange Notification ExpChargeLevel: %d"),expparams.iChargeLevel);	
		iTestStep->INFO_PRINTF2(_L("EBatteryInfoChange Notification ChargeLevel: %d"),params.iChargeLevel);
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for EBatteryInfoChange Notification: %d"), iStatus.Int() );
		}
	}
	
void CTestRegisterNotifierAct::ProcessSignalStrengthNotification()
	{
	if(iStatus.Int() == KErrNone)
		{
		//get expected status for this notification		
		TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
		CTelephony::TSignalStrengthV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TSignalStrengthV1Pckg&, expnotifierpckg);
		CTelephony::TSignalStrengthV1& expparams = (exppckg)();
		//get actual notification status
		CTelephony::TSignalStrengthV1& params = reinterpret_cast<CTelephony::TSignalStrengthV1&> 
												( const_cast<TUint8&> ( *iNotifierData->Ptr() ) );
		
		iTestStep->TEST1(expparams.iSignalStrength == params.iSignalStrength, ETrue);	
		iTestStep->TEST1(expparams.iBar == params.iBar, ETrue);
		iTestStep->INFO_PRINTF2(_L("ESignalStrengthChange Notification ExpSignalStrength: %d"),expparams.iSignalStrength);	
		iTestStep->INFO_PRINTF2(_L("ESignalStrengthChange Notification SignalStrength: %d"),params.iSignalStrength);	
		iTestStep->INFO_PRINTF2(_L("ESignalStrengthChange Notification ExpBar: %d"),expparams.iBar);	
		iTestStep->INFO_PRINTF2(_L("ESignalStrengthChange Notification Bar: %d"),params.iBar);
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for ESignalStrengthChange Notification: %d"), iStatus.Int() );
		}
	}
	
void CTestRegisterNotifierAct::ProcessLockInfoNotification()
	{
	if(iStatus.Int() == KErrNone)
		{
		//get expected status for this notification		
		TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
		CTelephony::TIccLockInfoV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TIccLockInfoV1Pckg&, expnotifierpckg);
		CTelephony::TIccLockInfoV1& expparams = (exppckg)();
		//get actual notification status
		CTelephony::TIccLockInfoV1& params = reinterpret_cast<CTelephony::TIccLockInfoV1&> 
												( const_cast<TUint8&> ( *iNotifierData->Ptr() ) );
		
		iTestStep->TEST1(expparams.iStatus == params.iStatus, ETrue);		
		iTestStep->TEST1(expparams.iSetting == params.iSetting, ETrue);
		iTestStep->INFO_PRINTF2(_L("ELockInfoChange Notification ExpStatus: %d"),expparams.iStatus);
		iTestStep->INFO_PRINTF2(_L("ELockInfoChange Notification Status: %d"),params.iStatus);
		iTestStep->INFO_PRINTF2(_L("ELockInfoChange Notification ExpSetting: %d"),expparams.iSetting);
		iTestStep->INFO_PRINTF2(_L("ELockInfoChange Notification Setting: %d"),params.iSetting);
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for ELockInfoChange Notification: %d"), iStatus.Int() );
		}
	}
	
void CTestRegisterNotifierAct::ProcessLineStatusNotification()
	{
	if(iStatus.Int() == KErrNone)
		{
		//get expected status for this notification		
		TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
		CTelephony::TCallStatusV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TCallStatusV1Pckg&, expnotifierpckg);
		CTelephony::TCallStatusV1& expparams = (exppckg)();
		//get actual notification status
		CTelephony::TCallStatusV1& params = reinterpret_cast<CTelephony::TCallStatusV1&> 
												( const_cast<TUint8&> ( *iNotifierData->Ptr() ) );
		
		iTestStep->TEST1(expparams.iStatus == params.iStatus, ETrue);
		iTestStep->INFO_PRINTF2(_L("ELineStatusChange Notification ExpStatus: %d"),expparams.iStatus);
		iTestStep->INFO_PRINTF2(_L("ELineStatusChange Notification Status: %d"),params.iStatus);			
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for ELineStatusChange Notification: %d"), iStatus.Int() );
		}
	}
	
void CTestRegisterNotifierAct::ProcessCallStatusNotification()
	{
	if(iStatus.Int() == KErrNone)
		{
		//get expected status for this notification		
		TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
		CTelephony::TCallStatusV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TCallStatusV1Pckg&, expnotifierpckg);
		CTelephony::TCallStatusV1& expparams = (exppckg)();
		//get actual notification status
		CTelephony::TCallStatusV1& params = reinterpret_cast<CTelephony::TCallStatusV1&> 
												( const_cast<TUint8&> ( *iNotifierData->Ptr() ) );		
		
		iTestStep->TEST1(expparams.iStatus == params.iStatus, ETrue);
		iTestStep->INFO_PRINTF2(_L("ECallStatusChange Notification ExpStatus: %d"),expparams.iStatus);
		iTestStep->INFO_PRINTF2(_L("ECallStatusChange Notification Status: %d"),params.iStatus);			
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for ECallStatusChange Notification: %d"), iStatus.Int() );
		}
	}
	
void CTestRegisterNotifierAct::ProcessRemotePartyInfoNotification()
	{
	if(iStatus.Int() == KErrNone)
		{
		//get expected status for this notification		
		TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
		CTelephony::TRemotePartyInfoV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TRemotePartyInfoV1Pckg&, expnotifierpckg);
		CTelephony::TRemotePartyInfoV1& expparams = (exppckg)();
		//get actual notification status
		CTelephony::TRemotePartyInfoV1& params = reinterpret_cast<CTelephony::TRemotePartyInfoV1&> 
													( const_cast<TUint8&> ( *iNotifierData->Ptr() ) );
		
		iTestStep->TEST1(expparams.iRemoteIdStatus == params.iRemoteIdStatus, ETrue);
		iTestStep->TEST1(expparams.iCallingName == params.iCallingName, ETrue);	
		iTestStep->TEST1(expparams.iRemoteNumber.iTypeOfNumber == params.iRemoteNumber.iTypeOfNumber, ETrue);
		iTestStep->TEST1(expparams.iRemoteNumber.iNumberPlan == params.iRemoteNumber.iNumberPlan, ETrue);
		iTestStep->TEST1(expparams.iRemoteNumber.iTelNumber == params.iRemoteNumber.iTelNumber, ETrue);
		iTestStep->TEST1(expparams.iDirection == params.iDirection, ETrue);
		
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpRemoteIdStatus: %d"),expparams.iRemoteIdStatus);
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification RemoteIdStatus: %d"),params.iRemoteIdStatus);	
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpCallingName: %S"),&expparams.iCallingName);
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification CallingName: %S"),&params.iCallingName);	
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpRemoteNumber.TON: %d"),expparams.iRemoteNumber.iTypeOfNumber);
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification RemoteNumber.TON: %d"),params.iRemoteNumber.iTypeOfNumber);
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpRemoteNumber.NPI: %d"),expparams.iRemoteNumber.iNumberPlan);
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification RemoteNumber.NPI: %d"),params.iRemoteNumber.iNumberPlan);	
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpRemoteNumber.Number: %S"),&expparams.iRemoteNumber.iTelNumber);
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification RemoteNumber.Number: %S"),&params.iRemoteNumber.iTelNumber);
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpDirection: %d"),expparams.iDirection);
		iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification Direction: %d"),params.iDirection);			
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for ERemotePartyInfoChange Notification: %d"), iStatus.Int() );
		}
	}
	
void CTestRegisterNotifierAct::ProcessNetworkRegistrationStatusNotification()
	{
	if(iStatus.Int() == KErrNone)
		{
		//get expected status for this notification		
		TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
		CTelephony::TNetworkRegistrationV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TNetworkRegistrationV1Pckg&, expnotifierpckg);
		CTelephony::TNetworkRegistrationV1& expparams = (exppckg)();
		//get actual notification status
		CTelephony::TNetworkRegistrationV1& params = reinterpret_cast<CTelephony::TNetworkRegistrationV1&> 
														( const_cast<TUint8&> ( *iNotifierData->Ptr() ) );
				
		iTestStep->TEST1(expparams.iRegStatus == params.iRegStatus, ETrue);	
		iTestStep->INFO_PRINTF2(_L("ENetworkRegistrationStatusChange Notification ExpRegStatus: %d"),expparams.iRegStatus);
		iTestStep->INFO_PRINTF2(_L("ENetworkRegistrationStatusChange Notification RegStatus: %d"),params.iRegStatus);	
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for ENetworkRegistrationStatusChange Notification: %d"), iStatus.Int() );
		}
	}
	
void CTestRegisterNotifierAct::ProcessCurrentNetworkInfoNotification()
	{
	switch(iNetworkInfoVersion)
		{
	case 1:
		{
		if(iStatus.Int() == KErrNone)
			{
			//get expected status for this notification		
			TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
			CTelephony::TNetworkInfoV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TNetworkInfoV1Pckg&, expnotifierpckg);
			CTelephony::TNetworkInfoV1& expparams = (exppckg)();
			//get actual notification status
			CTelephony::TNetworkInfoV1& params = reinterpret_cast<CTelephony::TNetworkInfoV1&> 
													( const_cast<TUint8&> ( *iNotifierData->Ptr() ) );	
			
			iTestStep->TEST1(expparams.iMode == params.iMode, ETrue);
			iTestStep->TEST1(expparams.iStatus == params.iStatus, ETrue);
			iTestStep->TEST1(expparams.iCountryCode == params.iCountryCode, ETrue);
			iTestStep->TEST1(expparams.iNetworkId == params.iNetworkId, ETrue);
			iTestStep->TEST1(expparams.iDisplayTag == params.iDisplayTag, ETrue);
			iTestStep->TEST1(expparams.iShortName == params.iShortName, ETrue);
			iTestStep->TEST1(expparams.iBandInfo == params.iBandInfo, ETrue);
			iTestStep->TEST1(expparams.iCdmaSID == params.iCdmaSID, ETrue);
			iTestStep->TEST1(expparams.iLongName == params.iLongName, ETrue);
			iTestStep->TEST1(expparams.iAccess == params.iAccess, ETrue);
			iTestStep->TEST1(expparams.iAreaKnown == params.iAreaKnown, ETrue);
			iTestStep->TEST1(expparams.iLocationAreaCode == params.iLocationAreaCode, ETrue);
			iTestStep->TEST1(expparams.iCellId == params.iCellId, ETrue);
			
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpMode: %d"),expparams.iMode);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification Mode: %d"),params.iMode);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpStatus: %d"),expparams.iStatus);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification Status: %d"),params.iStatus);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpCountryCode: %S"),&expparams.iCountryCode);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification CountryCode: %S"),&params.iCountryCode);	
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpNetworkId: %S"),&expparams.iNetworkId);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification NetworkId: %S"),&params.iNetworkId);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpDisplayTag: %S"),&expparams.iDisplayTag);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification DisplayTag: %S"),&params.iDisplayTag);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpShortName: %S"),&expparams.iShortName);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ShortName: %S"),&params.iShortName);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpBandInfo: %d"),expparams.iBandInfo);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification BandInfo: %d"),params.iBandInfo);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpCdmaSID: %S"),&expparams.iCdmaSID);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification CdmaSID: %S"),&params.iCdmaSID);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpLongName: %S"),&expparams.iLongName);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification LongName: %S"),&params.iLongName);	
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpAccess: %d"),expparams.iAccess);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification Access: %d"),params.iAccess);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpAreaKnown: %d"),expparams.iAreaKnown);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification AreaKnown: %d"),params.iAreaKnown);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpLocationAreaCode: %d"),expparams.iLocationAreaCode);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification LocationAreaCode: %d"),params.iLocationAreaCode);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpCellId: %d"),expparams.iCellId);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification CellId: %d"),params.iCellId);					
			}
		else
			{
			iTestStep->INFO_PRINTF2(_L("Error for ECurrentNetworkInfo Notification: %d"), iStatus.Int() );
			}			
		}
		break;
	case 2:
		{
		if(iStatus.Int() == KErrNone)
			{
			//get expected status for this notification		
			TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
			CTelephony::TNetworkInfoV2Pckg& exppckg = reinterpret_cast<CTelephony::TNetworkInfoV2Pckg&>(expnotifierpckg);
			CTelephony::TNetworkInfoV2& expparams = (exppckg)();
			//get actual notification status
			CTelephony::TNetworkInfoV2& params = reinterpret_cast<CTelephony::TNetworkInfoV2&> 
													( const_cast<TUint8&> ( *iNotifierData->Ptr() ) );	
			
			iTestStep->TEST1(expparams.iMode == params.iMode, ETrue);
			iTestStep->TEST1(expparams.iStatus == params.iStatus, ETrue);
			iTestStep->TEST1(expparams.iCountryCode == params.iCountryCode, ETrue);
			iTestStep->TEST1(expparams.iNetworkId == params.iNetworkId, ETrue);
			iTestStep->TEST1(expparams.iDisplayTag == params.iDisplayTag, ETrue);
			iTestStep->TEST1(expparams.iShortName == params.iShortName, ETrue);
			iTestStep->TEST1(expparams.iBandInfo == params.iBandInfo, ETrue);
			iTestStep->TEST1(expparams.iCdmaSID == params.iCdmaSID, ETrue);
			iTestStep->TEST1(expparams.iLongName == params.iLongName, ETrue);
			iTestStep->TEST1(expparams.iAccess == params.iAccess, ETrue);
			iTestStep->TEST1(expparams.iAreaKnown == params.iAreaKnown, ETrue);
			iTestStep->TEST1(expparams.iLocationAreaCode == params.iLocationAreaCode, ETrue);
			iTestStep->TEST1(expparams.iCellId == params.iCellId, ETrue);
			
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpMode: %d"),expparams.iMode);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification Mode: %d"),params.iMode);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpStatus: %d"),expparams.iStatus);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification Status: %d"),params.iStatus);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpCountryCode: %S"),&expparams.iCountryCode);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification CountryCode: %S"),&params.iCountryCode);	
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpNetworkId: %S"),&expparams.iNetworkId);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification NetworkId: %S"),&params.iNetworkId);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpDisplayTag: %S"),&expparams.iDisplayTag);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification DisplayTag: %S"),&params.iDisplayTag);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpShortName: %S"),&expparams.iShortName);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ShortName: %S"),&params.iShortName);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpBandInfo: %d"),expparams.iBandInfo);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification BandInfo: %d"),params.iBandInfo);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpCdmaSID: %S"),&expparams.iCdmaSID);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification CdmaSID: %S"),&params.iCdmaSID);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpLongName: %S"),&expparams.iLongName);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification LongName: %S"),&params.iLongName);	
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpAccess: %d"),expparams.iAccess);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification Access: %d"),params.iAccess);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpAreaKnown: %d"),expparams.iAreaKnown);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification AreaKnown: %d"),params.iAreaKnown);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpLocationAreaCode: %d"),expparams.iLocationAreaCode);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification LocationAreaCode: %d"),params.iLocationAreaCode);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification ExpCellId: %d"),expparams.iCellId);
			iTestStep->INFO_PRINTF2(_L("ECurrentNetworkInfo Notification CellId: %d"),params.iCellId);					
			}
		else
			{
			iTestStep->INFO_PRINTF2(_L("Error for ECurrentNetworkInfo Notification: %d"), iStatus.Int() );
			}		
		}
		break;
		}

	}
	
void CTestRegisterNotifierAct::MakeCallL()
	{
	_LIT(KGoodNumber,"1789");
	iCallParams.iIdRestrict = CTelephony::ESendMyId;
		
	iTestCallParams.iLine = CTelephony::EVoiceLine;
	iTestCallParams.iCallParamsV1Pckg = &iCallParamsPckg;
	iTestCallParams.iTelAddress.iTelNumber = KGoodNumber;
	iTestCallParams.iExpStat = KErrNone;
	
	iDialNewCallAct = CTestDialNewCallAct::NewL(iTestStep, iTelephony, this);	
	
	// Create a call - should succeed
	iTestCallParams.iCallId = CTelephony::EISVMaxNumOfCalls;
	iDialNewCallAct->TestDialNewCall(iTestCallParams);
	iCallsActive = ETrue;
	}

void CTestRegisterNotifierAct::HangupCallL()
	{
	iHangupAct	= CTestHangupAct::NewL(iTestStep, iTelephony, this);
	iHangupAct->TestHangup(iTestCallParams);	
	}
void  CTestRegisterNotifierAct::ActionParent(const TRequestStatus aCallStatus, const TActionEvent aEvent)
	{
/**
Gets called from a child AO when its RunL has been called.  
*/
	//got callback from child AO. Proceed the test.
	if(aCallStatus.Int() == KErrNone)
		{
		if(iEvent == CTelephony::EVoiceLineStatusChange && aEvent == EDialNewCall)	
			{
			TRAP_IGNORE(HangupCallL());
			return;
			}
		else if(iEvent == CTelephony::EVoiceLineStatusChange && aEvent == EHangup)
			{
			iCallsActive = EFalse;	
			CActiveScheduler::Stop();
			}
		else if((iEvent == CTelephony::EOwnedCall1StatusChange || iEvent == CTelephony::EOwnedCall2StatusChange) && aEvent == EDialNewCall)
			{
			CActiveScheduler::Stop();
			}
		else if((iEvent == CTelephony::EOwnedCall1RemotePartyInfoChange || iEvent == CTelephony::EOwnedCall2RemotePartyInfoChange) && aEvent == EDialNewCall)
			{
			CActiveScheduler::Stop();
			iCallsActive = 0;
			}
		else if((iEvent == CTelephony::EOwnedCall1StatusChange || iEvent == CTelephony::EOwnedCall2StatusChange) && aEvent == EHangup)
			{
			iCallsActive = EFalse;	
			CActiveScheduler::Stop();
			}
		}	
	else
		{
		iCallsActive = EFalse;	
		CActiveScheduler::Stop();
		}
		
	}
	
/**
Sets iNetworkInfoVersion to aVersion.

@param aVersion The version of TNetworkInfo being tested.
*/
void CTestRegisterNotifierAct::SetNetworkInfoVersion(TInt aVersion)
	{
	iNetworkInfoVersion = aVersion;
	}

CTestCancelRegisterNotifierAct* CTestCancelRegisterNotifierAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, CTelephony::TNotificationEvent aEvent)
//Factory constructor
	{
	CTestCancelRegisterNotifierAct* obj = new(ELeave) CTestCancelRegisterNotifierAct(aTestStep, aTelephony, aEvent);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
CTestCancelRegisterNotifierAct::CTestCancelRegisterNotifierAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, CTelephony::TNotificationEvent aEvent)
	: CEtelIsvActBase(aTestStep, aTelephony)
	, iEvent(aEvent)
	, iCallParamsPckg(iCallParams)
	{  
	}

void CTestCancelRegisterNotifierAct::ConstructL()
	{
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestCancelRegisterNotifierAct::~CTestCancelRegisterNotifierAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup
	delete iCanceller;
	iExpNotificationCompleteList.Reset();
	}
	
void CTestCancelRegisterNotifierAct::StartNotifier(TDes8& aNotifierPckg, TRequestStatus aExpStat)
	{
	iNotifierPckg=&aNotifierPckg;
	iExpStat=aExpStat;
	
	iTelephony->NotifyChange(iStatus, iEvent, *iNotifierPckg);
	SetActive();
	}
	
void CTestCancelRegisterNotifierAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
	CActiveScheduler::Stop();	
	}

void CTestCancelRegisterNotifierAct::DoCancel()
	{
	switch(iEvent)
		{
	case CTelephony::EFlightModeChange:
		iTelephony->CancelAsync(CTelephony::EFlightModeChangeCancel);	
		break;
	case CTelephony::EIndicatorChange:
		iTelephony->CancelAsync(CTelephony::EIndicatorChangeCancel);	
		break;
	case CTelephony::EBatteryInfoChange:
		iTelephony->CancelAsync(CTelephony::EBatteryInfoChangeCancel);	
		break;
	case CTelephony::ESignalStrengthChange:
		iTelephony->CancelAsync(CTelephony::ESignalStrengthChangeCancel);	
		break;
	case CTelephony::EPin1LockInfoChange:
		iTelephony->CancelAsync(CTelephony::EPin1LockInfoChangeCancel);	
		break;
	case CTelephony::EPin2LockInfoChange:
		iTelephony->CancelAsync(CTelephony::EPin2LockInfoChangeCancel);	
		break;
	case CTelephony::EVoiceLineStatusChange:
		iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);	
		break;
	case CTelephony::EOwnedCall1StatusChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall1StatusChangeCancel);	
		break;
	case CTelephony::EOwnedCall2StatusChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall2StatusChangeCancel);	
		break;
	case CTelephony::EOwnedCall1RemotePartyInfoChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall1RemotePartyInfoChangeCancel);	
		break;
	case CTelephony::EOwnedCall2RemotePartyInfoChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall2RemotePartyInfoChangeCancel);	
		break;
	case CTelephony::ENetworkRegistrationStatusChange:
		iTelephony->CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);	
		break;
	case CTelephony::ECurrentNetworkInfoChange:
		iTelephony->CancelAsync(CTelephony::ECurrentNetworkInfoChangeCancel);	
		break;
	default:
		break;
		}	
	}
	
CTestCallAndNotify::CTestCallAndNotify()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallAndNotify"));
}

enum TVerdict CTestCallAndNotify::doTestStepL()
{
	INFO_PRINTF1(_L("CTestCallAndNotify::doTestStepL Start"));
	
	SetTestNumber(15); 
	
	//Notification Test 1: EOwnedCall1CallStatusChange with call closure
	INFO_PRINTF1(_L("Testing EOwnedCall1CallStatusChange notification"));
	CTelephony::TCallStatusV1 calNotifierStatus;
	CTelephony::TCallStatusV1Pckg calNotifierStatusPckg(calNotifierStatus);	
	CTestNotifyCallClosureAct* callStatusChangeAct = CTestNotifyCallClosureAct::NewLC(this, iTelephony, CTelephony::EOwnedCall1StatusChange);

	CTelephony::TCallStatusV1 calExpNotifierStatus1;
	CTelephony::TCallStatusV1Pckg calExpNotifierStatusPckg1(calExpNotifierStatus1);	
	calExpNotifierStatus1.iStatus = CTelephony::EStatusDisconnecting;
	callStatusChangeAct->iExpNotificationCompleteList.Append(calExpNotifierStatusPckg1);
	
	CTelephony::TCallStatusV1 calExpNotifierStatus2;
	CTelephony::TCallStatusV1Pckg calExpNotifierStatusPckg2(calExpNotifierStatus2);	
	calExpNotifierStatus2.iStatus = CTelephony::EStatusIdle;
	callStatusChangeAct->iExpNotificationCompleteList.Append(calExpNotifierStatusPckg2);	
	
	CTelephony::TCallStatusV1 calExpNotifierStatus3;
	CTelephony::TCallStatusV1Pckg calExpNotifierStatusPckg3(calExpNotifierStatus3);	
	calExpNotifierStatus3.iStatus = CTelephony::EStatusIdle;
	callStatusChangeAct->iExpNotificationCompleteList.Append(calExpNotifierStatusPckg3);	
		
	//make call
	callStatusChangeAct->MakeCallL();
	CActiveScheduler::Start();
	
	callStatusChangeAct->StartNotifier(calNotifierStatusPckg, KErrNone);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(callStatusChangeAct);
	callStatusChangeAct = NULL;
	
	//Notification Test 2: EOwnedCall1RemotePartyInfoChange
	INFO_PRINTF1(_L("Testing EOwnedCall1RemotePartyInfoChange notification"));
	CTelephony::TRemotePartyInfoV1 remNotifierStatus;
	CTelephony::TRemotePartyInfoV1Pckg remNotifierStatusPckg(remNotifierStatus);	
	CTestNotifyCallClosureAct* RemotePartyInfoChangeAct = CTestNotifyCallClosureAct::NewLC(this, iTelephony, CTelephony::EOwnedCall1RemotePartyInfoChange);

	CTelephony::TRemotePartyInfoV1 remExpNotifierStatus1;
	CTelephony::TRemotePartyInfoV1Pckg remExpNotifierStatusPckg1(remExpNotifierStatus1);	
	remExpNotifierStatus1.iCallingName = _L("call0");
	remExpNotifierStatus1.iDirection = CTelephony::EMobileOriginated;
	remExpNotifierStatus1.iRemoteIdStatus = CTelephony::ERemoteIdentityAvailable;
	remExpNotifierStatus1.iRemoteNumber.iTypeOfNumber = CTelephony::EInternationalNumber;
	remExpNotifierStatus1.iRemoteNumber.iNumberPlan = CTelephony::EIsdnNumberPlan;
	remExpNotifierStatus1.iRemoteNumber.iTelNumber = _L("12345678");
	RemotePartyInfoChangeAct->iExpNotificationCompleteList.Append(remExpNotifierStatusPckg1);
	
	//make call
	RemotePartyInfoChangeAct->MakeCallL();
	CActiveScheduler::Start();	

	RemotePartyInfoChangeAct->StartNotifier(remNotifierStatusPckg, KErrCancel);
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(RemotePartyInfoChangeAct);
	RemotePartyInfoChangeAct = NULL;	
	
	//For the next tests we must first set up a call 1 as we want to test call 2 notifiers
	//Set the call parameters for call 1
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = _L("1789");
	testCallParams1.iExpStat = KErrNone;
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);
	
	testCallParams1.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	CTelephony::TCallStatusV1 status;
	CTelephony::TCallStatusV1Pckg statusPckg(status);		
	iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);	
	
	//put first call on hold
	CTestHoldAct* TestHoldAct = CTestHoldAct::NewLC(this, iTelephony);	
	TestHoldAct->TestHold(testCallParams1);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(TestHoldAct);
	CleanupStack::PopAndDestroy(testDialNewCallAct1);
	
	//Notification Test 3: EOwnedCall2CallStatusChange with call closure
	INFO_PRINTF1(_L("Testing EOwnedCall2CallStatusChange notification"));
	callStatusChangeAct = CTestNotifyCallClosureAct::NewLC(this, iTelephony, CTelephony::EOwnedCall2StatusChange);
	
	callStatusChangeAct->iExpNotificationCompleteList.Append(calExpNotifierStatusPckg1);
	callStatusChangeAct->iExpNotificationCompleteList.Append(calExpNotifierStatusPckg2);
	callStatusChangeAct->iExpNotificationCompleteList.Append(calExpNotifierStatusPckg3);
	
	//make 2nd call
	callStatusChangeAct->MakeCallL();
	CActiveScheduler::Start();
	
	callStatusChangeAct->StartNotifier(calNotifierStatusPckg, KErrNone);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(callStatusChangeAct);
	
	//Notification Test 4: EOwnedCall2RemotePartyInfoChange
	INFO_PRINTF1(_L("Testing EOwnedCall2RemotePartyInfoChange notification"));
	RemotePartyInfoChangeAct = CTestNotifyCallClosureAct::NewLC(this, iTelephony, CTelephony::EOwnedCall2RemotePartyInfoChange);

	RemotePartyInfoChangeAct->iExpNotificationCompleteList.Append(remExpNotifierStatusPckg1);
	
	//make 2nd call
	RemotePartyInfoChangeAct->MakeCallL();
	CActiveScheduler::Start();	

	RemotePartyInfoChangeAct->StartNotifier(remNotifierStatusPckg, KErrCancel);
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(RemotePartyInfoChangeAct);
	RemotePartyInfoChangeAct = NULL;

	return TestStepResult();
}



CTestNotifyCallClosureAct* CTestNotifyCallClosureAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, CTelephony::TNotificationEvent aEvent)
//Factory constructor
	{
	CTestNotifyCallClosureAct* obj = new(ELeave) CTestNotifyCallClosureAct(aTestStep, aTelephony, aEvent);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
CTestNotifyCallClosureAct::CTestNotifyCallClosureAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, CTelephony::TNotificationEvent aEvent)
	: CEtelIsvActBase(aTestStep, aTelephony)
	, iEvent(aEvent)
	, iCallParamsPckg(iCallParams)
	, iCallsActive(EFalse)
	{  
	}

void CTestNotifyCallClosureAct::ConstructL()
	{  
	CActiveScheduler::Add(this);
	}

CTestNotifyCallClosureAct::~CTestNotifyCallClosureAct()
	{ 
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup
	delete iDialNewCallAct;
	delete iHangupAct;
	iExpNotificationCompleteList.Reset();
	}

void CTestNotifyCallClosureAct::DoCancel()
	{
	switch(iEvent)
		{
	case CTelephony::EOwnedCall1StatusChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall1StatusChangeCancel);	
		break;
	case CTelephony::EOwnedCall2StatusChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall2StatusChangeCancel);	
		break;
	case CTelephony::EOwnedCall1RemotePartyInfoChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall1RemotePartyInfoChangeCancel);	
		break;
	case CTelephony::EOwnedCall2RemotePartyInfoChange:
		iTelephony->CancelAsync(CTelephony::EOwnedCall2RemotePartyInfoChangeCancel);	
		break;
	default:
		break;
		}	
	}
	
void CTestNotifyCallClosureAct::StartNotifier(TDes8& aNotifierPckg, TRequestStatus aExpStat)
	{
	iNotifierPckg=&aNotifierPckg;
	iExpStat=aExpStat;
	iNumberNotificationsBeforeFinish = iExpNotificationCompleteList.Count();
	SendNotifyRequest();
	InvokeNotifyCondition();
	iNotificationCount = 0;
	}
	
void CTestNotifyCallClosureAct::InvokeNotifyCondition()
	{
	switch(iEvent)
		{
	case CTelephony::EOwnedCall1StatusChange:
	case CTelephony::EOwnedCall1RemotePartyInfoChange:
	case CTelephony::EOwnedCall2StatusChange:
	case CTelephony::EOwnedCall2RemotePartyInfoChange:
		TRAP_IGNORE(HangupCallL());
		break;				
	default:
		break;
		}
	}
	
void CTestNotifyCallClosureAct::SendNotifyRequest()
	{
	iTelephony->NotifyChange(iStatus, iEvent, *iNotifierPckg);
	SetActive();
	}

void CTestNotifyCallClosureAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().

	iTestStep->TEST1(iStatus.Int() == KErrNone || iStatus.Int() == KErrCancel 
						|| iStatus.Int() == KErrAccessDenied, ETrue);
	
	if(++iNotificationCount <= iNumberNotificationsBeforeFinish)
		{	//process the notification
		switch(iEvent)
			{
		case CTelephony::EOwnedCall1StatusChange:
		case CTelephony::EOwnedCall2StatusChange:
			if(iStatus.Int() == KErrNone)
				{
				//get expected status for this notification		
				TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
				CTelephony::TCallStatusV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TCallStatusV1Pckg&, expnotifierpckg);
				CTelephony::TCallStatusV1& expparams = (exppckg)();
				//get actual notification status
				CTelephony::TCallStatusV1Pckg& pckg = REINTERPRET_CAST(CTelephony::TCallStatusV1Pckg&, *iNotifierPckg);
				CTelephony::TCallStatusV1& params = (pckg)();
				
				iTestStep->TEST1(expparams.iStatus == params.iStatus, ETrue);
				iTestStep->INFO_PRINTF2(_L("ECallStatusChange Notification ExpStatus: %d"),expparams.iStatus);
				iTestStep->INFO_PRINTF2(_L("ECallStatusChange Notification Status: %d"),params.iStatus);		
				}
			break;
		case CTelephony::EOwnedCall1RemotePartyInfoChange:
		case CTelephony::EOwnedCall2RemotePartyInfoChange:
			if(iStatus.Int() == KErrNone)
				{
			//get expected status for this notification		
			TDes8& expnotifierpckg = iExpNotificationCompleteList[iNotificationCount-1];
			CTelephony::TRemotePartyInfoV1Pckg& exppckg = REINTERPRET_CAST(CTelephony::TRemotePartyInfoV1Pckg&, expnotifierpckg);
			CTelephony::TRemotePartyInfoV1& expparams = (exppckg)();
			//get actual notification status
			CTelephony::TRemotePartyInfoV1Pckg& pckg = REINTERPRET_CAST(CTelephony::TRemotePartyInfoV1Pckg&, *iNotifierPckg);
			CTelephony::TRemotePartyInfoV1& params = (pckg)();		
			
			iTestStep->TEST1(expparams.iRemoteIdStatus == params.iRemoteIdStatus, ETrue);
			iTestStep->TEST1(expparams.iCallingName == params.iCallingName, ETrue);	
			iTestStep->TEST1(expparams.iRemoteNumber.iTypeOfNumber == params.iRemoteNumber.iTypeOfNumber, ETrue);
			iTestStep->TEST1(expparams.iRemoteNumber.iNumberPlan == params.iRemoteNumber.iNumberPlan, ETrue);
			iTestStep->TEST1(expparams.iRemoteNumber.iTelNumber == params.iRemoteNumber.iTelNumber, ETrue);
			iTestStep->TEST1(expparams.iDirection == params.iDirection, ETrue);
			
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpRemoteIdStatus: %d"),expparams.iRemoteIdStatus);
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification RemoteIdStatus: %d"),params.iRemoteIdStatus);	
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpCallingName: %S"),&expparams.iCallingName);
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification CallingName: %S"),&params.iCallingName);	
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpRemoteNumber.TON: %d"),expparams.iRemoteNumber.iTypeOfNumber);
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification RemoteNumber.TON: %d"),params.iRemoteNumber.iTypeOfNumber);
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpRemoteNumber.NPI: %d"),expparams.iRemoteNumber.iNumberPlan);
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification RemoteNumber.NPI: %d"),params.iRemoteNumber.iNumberPlan);	
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpRemoteNumber.Number: %d"),&expparams.iRemoteNumber.iTelNumber);
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification RemoteNumber.Number: %d"),&params.iRemoteNumber.iTelNumber);
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification ExpDirection: %d"),expparams.iDirection);
			iTestStep->INFO_PRINTF2(_L("ERemotePartyInfoChange Notification Direction: %d"),params.iDirection);	
				}
			break;
		default:
			break;
			}
		}
	
	if(iStatus.Int() == KErrNone)	//post for further notifications, while cancel not found
		SendNotifyRequest();
	}
	
void CTestNotifyCallClosureAct::MakeCallL()
	{
	_LIT(KGoodNumber,"1789");
	iCallParams.iIdRestrict = CTelephony::ESendMyId;
		
	iTestCallParams.iLine = CTelephony::EVoiceLine;
	iTestCallParams.iCallParamsV1Pckg = &iCallParamsPckg;
	iTestCallParams.iTelAddress.iTelNumber = KGoodNumber;
	iTestCallParams.iExpStat = KErrNone;
	
	iDialNewCallAct = CTestDialNewCallAct::NewL(iTestStep, iTelephony, this);	
	
	// Create a call - should succeed
	iTestCallParams.iCallId = CTelephony::EISVMaxNumOfCalls;
	iDialNewCallAct->TestDialNewCall(iTestCallParams);
	iCallsActive = ETrue;
	}

void CTestNotifyCallClosureAct::HangupCallL()
	{
	iHangupAct	= CTestHangupAct::NewL(iTestStep, iTelephony, this);
	iHangupAct->TestHangup(iTestCallParams);	
	}
void  CTestNotifyCallClosureAct::ActionParent(const TRequestStatus aCallStatus, const TActionEvent aEvent)
	{
/**
Gets called from a child AO when its RunL has been called.  
*/
	//got callback from child AO. Proceed the test.
	if(aCallStatus.Int() == KErrNone)
		{
		if((iEvent == CTelephony::EOwnedCall1StatusChange || iEvent == CTelephony::EOwnedCall2StatusChange) && aEvent == EDialNewCall)
			{
			CActiveScheduler::Stop();
			}
		else if((iEvent == CTelephony::EOwnedCall1RemotePartyInfoChange || iEvent == CTelephony::EOwnedCall2RemotePartyInfoChange) && aEvent == EDialNewCall)
			{
			CActiveScheduler::Stop();			
			}
		else if((iEvent == CTelephony::EOwnedCall1StatusChange || iEvent == CTelephony::EOwnedCall2StatusChange) && aEvent == EHangup)
			{
			iCallsActive = EFalse;	
			CActiveScheduler::Stop();
			}
		else if((iEvent == CTelephony::EOwnedCall1RemotePartyInfoChange || iEvent == CTelephony::EOwnedCall2RemotePartyInfoChange) && aEvent == EHangup)
			{
			iCallsActive = EFalse;	
			CActiveScheduler::Stop();
			}
		}	
	else
		{
		iCallsActive = EFalse;	
		CActiveScheduler::Stop();
		}		
	}
