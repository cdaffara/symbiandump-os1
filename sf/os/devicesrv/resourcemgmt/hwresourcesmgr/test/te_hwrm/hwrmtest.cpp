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

#include "hwrmtest.h"
#include "hwrmtestmacros.h"

#include <hwrm/hwrmvibracommands.h>
#include <hwrm/hwrmlightcommands.h>
#include <hwrm/hwrmpowercommands.h>
#include <hwrm/hwrmfmtxcommands.h>

#include <rmocklight.h>
#include <rmockvibra.h>
#include <rmockpower.h>

#include <e32property.h>

#include <hwrmvibrasdkcrkeys.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <hwrmlightdomaincrkeys.h>
#include <hwrmpowerdomaincrkeys.h>
#include <hwrmdomainpskeys.h>
#include <hwrmextendedlight.h>
#include <hwrmlight.h>
#include "HWRMPrivateCRKeys.h"

const TReal KOneTimeUnit = 1.0;
const TReal KTwoTimeUnits = KOneTimeUnit*2.0;
const TInt KSupportedTargets= 	CHWRMLight::EPrimaryDisplayAndKeyboard
							|	CHWRMLight::ESecondaryDisplayAndKeyboard
							|	CHWRMLight::ECustomTarget2
							|	CHWRMLight::ECustomTarget4;
							
const TInt KSystemTargets = KSupportedTargets & (CHWRMLight::EPrimaryDisplayAndKeyboard | CHWRMLight::ESecondaryDisplayAndKeyboard);

const TInt KMaxSampleReportingPeriod = 20000; // 20sec as per the CenRep Key "KPowerMaxReportingPeriod" setting

const TInt KOneSecond = 1000000;

#ifdef INSECURE_AUDIO_POLICY_KEYS    
const TInt KTestCategory = 0x101FB3E7;
const TInt KTestKeyNotifyChange = 0x1;
#endif // INSECURE_AUDIO_POLICY_KEYS

static const TInt KDefaultStepSize = 50;

#define SETCOLORPARAM(var, r, g, b) \
	{ \
	var.iRed = r; \
	var.iGreen = g; \
	var.iBlue = b; \
	}


CTestSuite* CHWRMTest::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	// Setup resources. These tests have to be run before any other lights or vibra steps
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestSetupLightsL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraOnL);
	
	//PREQ 1944
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestStartVibraOverlapPulseL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestStartVibraOverlapPulsesL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPulseOverlapPulsesL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestSequentialPulsesL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestUnSupportedPulseIntensityL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestUnSupportedPulseDurationL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPulseNotAllowedL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestReserveStartPulseVibraL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestReserveVibraL);
	
	// Vibra testing
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraOnPluginResponseErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraOnWithIntensityL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraOnWithIntensityOutOfRangeL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraOnWithDurationOutOfRangeL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraOnDuringForcedOffPeriodL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraOnAfterForcedOffPeriodL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraGetStatusL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraOnInfiniteL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraProfileL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraOnLoopL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraReserveSessionsL);

 	// Vibra profile skipping tests (privileged client)
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraProfilePrivilegedClientL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestVibraProfilePrivilegedClient2L);
	
	//Light testing
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightSimpleL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightBlinkKeyboardL); // case 52, 62
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightBlinkComplexL); // case 53-59, 61
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightBlinkKeyboardInfiniteCutOffTimerL); // case 60-99
	
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightReserveSessionsL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightReserveSessions2L);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightReserveDisplayL);	
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightGetSupportedTargetsL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightUnsupportedTargetL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightUnsupportedDurationL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightUnsupportedIntensityL);

	// Custom target tests	
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightOnSimpleCustomTargetsL);
	    	
	// System target tests, using Target Modifier Plugin
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestModifyTargetL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestSystemLightsL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestDisplayLightsOnL);

	// PREQ 2615
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightColorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightColor_ExtendedTargetL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightDefaultColorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightDefaultColor_ExtendedTargetL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightColor_UnSupportedTargetL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightColor_PluginErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFrozenStateRestoreLightColor_ExtendedTargetL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestBaseStateRestoreLightColor_ExtendedTargetL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightColor_MultiClientL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightColor_MultiClient_ExtendedTargetL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightColor_WhenInUseL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightDefaultColor_WhenInUseL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightOnDurationRGBL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightOnDurationWithIntenistyRGBL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightOnDurationRGB_ExtendedL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightBlinkDurationRGBL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightBlinkDurationWithIntensityRGBL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightBlinkDurationRGB_ExtendedL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightEnhancedLightLeaveL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestLightSetColorReserveL);
    
    // Power State tests
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerStatesL);	
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerExtendedBatteryStatusL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerBatteryStatusL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingStatusL);
    
	// Extended Lights
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestExtendedLightSessionsL);

	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestInvalidSessionsL);

	//Power
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerConnectL); 
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBatteryInfoL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBatteryInfoMultiClientL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBatteryInfoWithPluginErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBatteryVoltageInfoL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBatteryVoltageInfoMultiClientL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBatteryVoltageInfoWithPluginErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBaseTimeIntervalL);

	//Power - CR1833 
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotifyL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_InfiniteL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_AdjustReportingPeriodL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_BigIntervalL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_MultiClient_SameIntervalL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_MultiClient_DiffIntervalL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_RedundantL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_Start_PluginErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_Stop_PluginErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_IntervalEqualL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_IntervalGreaterL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_AdjustReportingPeriod_WithoutRequestL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_AdjustReportingPeriod_ElapsedTimeLessL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_AdjustReportingPeriod_ElapsedTimeMoreL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_AdjustReportingPeriod_NonZeroToZeroL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_AdjustReportingPeriod_ZeroToNonZeroL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_AdjustReportingPeriod_SameValueL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_AdjustReportingPeriod_NegativeValueL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_All_AdjustReportingPeriod_NegativeValueL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_SlowClientL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_WithoutCallbackL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_MultiClient_DiffInterval_SpecialCaseL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_Stop_WithoutStartL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_Stop_RedundantL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerNotify_Start_Set_PluginErrorL);

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBatteryChargingStatusL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBatteryChargingStatusMultiClient3L);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingStatusNotificationL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingStatusNotificationMultiClient3L);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerRemainingChargingTimeNotificationL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerRemainingChargingTimeWithPluginErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerStopRemainingChargingTimeWithPluginErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerRemainingChargingTimeNotificationErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerRemainingChargingTimeWhenStartedL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerRemainingChargingTimeNotificationMulticlient3L);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingTimeNotificationWhenChargingCompleteL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingTimeNotificationWhenDisconnectedL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingTimeNotificationWhenChargingComplete3L);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingTimeNotificationWhenDisconnected3L);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentWhenStartedL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationMulticlient3L);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationWhenChargingCompleteL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationWhenDisconnectedL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationWhenChargingComplete3L);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationWhenDisconnected3L);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentWithPluginErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerStopChargingCurrentWithPluginErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBatteryChargerType);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBatteryChargerTypeMulticlientL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerGetBatteryChargerTypeWithPluginErrorL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerCancelChargingTimeWithoutStart);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerCancelChargingCurrentWithoutStart);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationMaxPeriodReachedL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationSameIntervalL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationDifferentIntervalL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentandRemainingChargingTimeNotificationL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingandVoltageNotificationL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationSlowClientL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationSlowClientsL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingTimeNotificationWithoutCallbackL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationWithoutCallbackL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerClientSetCallBackL);
    ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestPowerChargingCurrentNotificationMaxPeriodReachedMCL);
#endif //SYMBIAN_HWRM_EXTPOWERINFO
		
	// FM Tx testing	

	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxConnectL); // Always run this test before the rest in order to set the Inactivity time out.
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxStatusOffL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxEnableInvalidFreqL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxEnableL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxEnableNewFreqL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxEnableInvalidFreqWhenOnL);
	
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxReserveL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxEnableDuringReservationL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxDisableL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxDisableDuringReservationL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxReserveDuringReservationL);

	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxGetFrequencyL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxSetFrequencyL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxSetFrequencyInvalidL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxSetFrequencyDuringReservationL);

	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxGetFrequencyRangeL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxGetJapanFrequencyRangeL);

	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxGetNextClearFrequencyL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxGetNextClearFrequencyAsyncL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxGetNextClearFrequencyAsyncCancelL);
	
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxGetNextClearFrequencyArrayL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxGetNextClearFrequencyAsyncArrayL);	

	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxSetNextClearFrequencyL);

	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxSetNextClearFrequencyAsyncL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxSetNextClearFrequencyAsyncCancelL);

	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxAutoFrequencyL);
	
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxSetRdsDataL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxSetRadiotextL);

	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxSetRadiotextContentL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxSetRadiotextTagL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxSetRadiotextTagsL);
	
#ifdef INSECURE_AUDIO_POLICY_KEYS    
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxAudioRoutingOnAndOffL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxPowerSaveOnAccessoryL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxPowerSaveOnOffStateL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxPowerSaveOnInactivityStateL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxPowerSaveOffL);
	ADD_TEST_STEP_ISO_CPP(CHWRMTest, TestFmTxEnableDuringPowersaveL);
	
#endif // INSECURE_AUDIO_POLICY_KEYS

	END_SUITE;
	}
		
// actual test cases
// -----------------
	
void CHWRMTest::TestVibraOnL()
	{
	/*test procedure
	1. Queue expected callback notifications
	2. Execute a command (that is supposed to trigger the callbacks)
	3. Start timer
	4. Wait for callbacks
	5. If timeout expires and all expected callbacks have not been triggered then test fails	
	*/
	INFO_PRINTF1(_L("Start test step: TestVibraOnL"));
	
	AddTestStateL(this, &CHWRMTest::VibraSetMinTimeOffL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraSetMaxTime10SecsOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOffL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraFeedbackSettingsL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

// -----------------------------------------------------------------------------
// StartVibraOverlapPulse.
// Try to request vibration and overlapping pulse request.
// -----------------------------------------------------------------------------
//
void CHWRMTest::TestStartVibraOverlapPulseL()
	{
	INFO_PRINTF1(_L("Start test step: TestStartVibraOverlapPulseL"));
	
	//test states are NOT self contained - they must be used in correct sequence
	AddTestStateL(this, &CHWRMTest::VibraPulseOpenL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::StartVibra5L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PulseVibraWithNoStatusUpdate0L, KTwoTimeUnits);	    	    	
	AddTestStateL(this, &CHWRMTest::CheckForVibraStatusStoppedL, KTwoTimeUnits * 2);

	ExecuteTestL();
	}

// -----------------------------------------------------------------------------
// StartVibraOverlapPulses.
// Try to request vibration and overlapping pulse requests.
// -----------------------------------------------------------------------------
//
void CHWRMTest::TestStartVibraOverlapPulsesL()
	{
	INFO_PRINTF1(_L("Start test step: TestStartVibraOverlapPulses"));
	
	//test states are NOT self contained - they must be used in correct sequence
	AddTestStateL(this, &CHWRMTest::VibraPulseOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::MultVibraPulsesL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CheckForVibraStatusStoppedL, KTwoTimeUnits * 3);
	
	ExecuteTestL();
	}

// ----------------------------------------------------------------------------- 
// PulseOverlapPulses.
// Try to request vibra pulse and overlapping pulses with it.
// -----------------------------------------------------------------------------
//
void CHWRMTest::TestPulseOverlapPulsesL()
	{
	INFO_PRINTF1(_L("Start test step: TestPulseOverlapPulsesL"));
	
	//test states are NOT self contained - they must be used in correct sequence
	AddTestStateL(this, &CHWRMTest::VibraPulseOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartVibra5L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PulsesOverlapPulsesL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CheckForVibraStatusStoppedL, KTwoTimeUnits * 3);
	
	ExecuteTestL();
	}
	
// -----------------------------------------------------------------------------
// SequentialPulses.
// Try to request vibra pulses frequently in sequence.
// -----------------------------------------------------------------------------	
void CHWRMTest::TestSequentialPulsesL()
	{
	INFO_PRINTF1(_L("Start test step: TestSequentialPulsesL"));
	
	AddTestStateL(this, &CHWRMTest::VibraPulseOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::SequentialPulsesL, KTwoTimeUnits);	
		
	
	ExecuteTestL();
	}				
	
 
// -----------------------------------------------------------------------------
// UnSupportedPulseIntensity.
// Try to request vibra pulse with incorrect intensity.
// -----------------------------------------------------------------------------
//
void CHWRMTest::TestUnSupportedPulseIntensityL()
	{
	INFO_PRINTF1(_L("Start test step: TestUnSupportedPulseIntensity"));
	
	AddTestStateL(this, &CHWRMTest::VibraPulseOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::UnSupportedPulseIntensity1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::UnSupportedPulseIntensity2L, KTwoTimeUnits);		
	AddTestStateL(this, &CHWRMTest::UnSupportedPulseIntensity3L, KTwoTimeUnits);		

	ExecuteTestL();
	}

// -----------------------------------------------------------------------------
// UnSupportedPulseIntensity.
// Try to request vibra pulse with incorrect intensity.
// -----------------------------------------------------------------------------
//
void CHWRMTest::TestUnSupportedPulseDurationL()
	{
	INFO_PRINTF1(_L("Start test step: TestUnSupportedPulseDurationL"));
	
	AddTestStateL(this, &CHWRMTest::VibraPulseOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::UnSupportedPulseDuration1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::UnSupportedPulseDuration2L, KTwoTimeUnits);		
	AddTestStateL(this, &CHWRMTest::UnSupportedPulseDuration3L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::UnSupportedPulseDuration4L, KTwoTimeUnits);	

	ExecuteTestL();
	}

// -----------------------------------------------------------------------------
// PulseNotAllowed.
// Try to request pulse when not allowed.
// -----------------------------------------------------------------------------
//
void CHWRMTest::TestPulseNotAllowedL()
	{
	INFO_PRINTF1(_L("Start test step: TestPulseNotAllowedL"));
	
	AddTestStateL(this, &CHWRMTest::VibraPulseOpenL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::VibraPulseDisableProfileL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PulseOnWhileDisabledL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::VibraPulseEnableProfileL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PulseVibraL, KTwoTimeUnits);
		
	ExecuteTestL();	
	}


// -----------------------------------------------------------------------------
// ReserveStartPulseVibra.
// Try to reserver vibra and concurrently request normal and pulse vibration.
// -----------------------------------------------------------------------------
//
void CHWRMTest::TestReserveStartPulseVibraL()
	{
	INFO_PRINTF1(_L("Start test step: TestReserveStartPulseVibraL"));
	
	//test states are NOT self contained - they must be used in correct sequence
	AddTestStateL(this, &CHWRMTest::VibraPulseOpenSessionsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraReserveL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::StartVibra5L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PulseVibraWithNoStatusUpdateL, KTwoTimeUnits);
	//check that vibra status update (stopped) has been received after 4 secs.
	AddTestStateL(this, &CHWRMTest::CheckForVibraStatusStoppedL, KTwoTimeUnits * 2);
	
	ExecuteTestL();	
	}
	
// -----------------------------------------------------------------------------
// Reserve_Vibra.
// Try to reserve vibra.
// -----------------------------------------------------------------------------
//
void CHWRMTest::TestReserveVibraL()
	{
	INFO_PRINTF1(_L("Start test step: TestReserveVibraL"));
	
	AddTestStateL(this, &CHWRMTest::VibraOpenSessionsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraReserveDefaultParamsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraReserveL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::Vibra2ReserveL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraReleaseL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::Vibra2ReleaseL, KTwoTimeUnits);

	ExecuteTestL();	
	}

void CHWRMTest::TestVibraOnPluginResponseErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraOnPluginResponseErrorL"));
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnWithResponseErrL, KTwoTimeUnits);
	ExecuteTestL();
	}
	
void CHWRMTest::TestVibraOnWithIntensityL()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraOnWithIntensityL"));
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnWithIntensityL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestVibraOnWithIntensityOutOfRangeL()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraOnWithIntensityOutOfRangeL"));
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnWithIntensityOutOfRangeL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnWithIntensityOutOfRangeL, KTwoTimeUnits);
	
	ExecuteTestL();
	}
	
void CHWRMTest::TestVibraOnWithDurationOutOfRangeL()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraOnWithDurationOutOfRangeL"));
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnWithDurationOutOfRangeL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnWithDurationOutOfRange2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnWithDurationOutOfRange3L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnWithDurationOutOfRange4L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnWithDurationGreaterThanMaximumL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraWaitForCutOffL, KOneTimeUnit*20);
	AddTestStateL(this, &CHWRMTest::VibraWaitForCoolOffL, KOneTimeUnit*20);
	
	ExecuteTestL();
	}

void CHWRMTest::TestVibraOnDuringForcedOffPeriodL()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraOnDuringForcedOffPeriodL"));
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnInfiniteL, KTwoTimeUnits);
	// wait for cut off
	AddTestStateL(this, &CHWRMTest::VibraWaitForCutOffL, KOneTimeUnit*20);
	AddTestStateL(this, &CHWRMTest::VibraOnDuringForcedOffPeriodL, KTwoTimeUnits);
	// wait for cool off to leave vibra ready for next step
	AddTestStateL(this, &CHWRMTest::VibraWaitForCoolOffL, KOneTimeUnit*20);
	
	ExecuteTestL();
	}

void CHWRMTest::TestVibraOnAfterForcedOffPeriodL()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraOnAfterForcedOffPeriodL"));
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOffL, KTwoTimeUnits);
	
	ExecuteTestL();
	}
	
void CHWRMTest::TestVibraGetStatusL()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraGetStatusL"));
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraGetStatusOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOffL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraGetStatusOffL, KTwoTimeUnits);
	ExecuteTestL();
	}

void CHWRMTest::TestVibraOnInfiniteL()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraOnInfiniteL"));	
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnInfiniteL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::VibraGetStatusOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraWaitForCutOffL, KOneTimeUnit*20);
	AddTestStateL(this, &CHWRMTest::VibraGetStatusOffL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraWaitForCoolOffL, KOneTimeUnit*20);
	ExecuteTestL();	
	}	
	
void CHWRMTest::TestVibraProfileL()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraProfileL"));
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraDisableProfileL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnWhileDisabledL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraEnableProfileL, KTwoTimeUnits);
	ExecuteTestL();
	}

void CHWRMTest::TestVibraOnLoopL()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraOnLoop"));
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnLoopL, KTwoTimeUnits);
	ExecuteTestL();
	}
	
void CHWRMTest::TestSetupLightsL()
	{
	INFO_PRINTF1(_L("Start test step: TestSetupLightsL"));
	AddTestStateL(this, &CHWRMTest::InitialiseLightsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOffAllTargetsL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestLightSimpleL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightSimpleL"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::LightOnDisplayAndKeyboardL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightGetStatusDisplayL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOffDisplayAndKeyboardL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOnDisplay_LimitedDurationL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::WaitForTargetToGoOffL, 10*KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit); // needed?
	ExecuteTestL();
	}
	
void CHWRMTest::TestLightBlinkKeyboardL() // case 52
	{
	INFO_PRINTF1(_L("Start test step: TestLightBlinkKeyboardL"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::LightBlinkKeyboardWithLimitedDurationL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightGetStatusKeyboardL, KTwoTimeUnits); // 62
	AddTestStateL(this, &CHWRMTest::WaitForTargetToGoOffL, 10*KOneTimeUnit);
	
	ExecuteTestL();
	}

void CHWRMTest::TestLightBlinkComplexL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightBlinkComplexL"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::LightOnDisplayAndKeyboardL, KTwoTimeUnits); // 53
	AddTestStateL(this, &CHWRMTest::LightBlinkDisplayWithLimitedDurationL, KTwoTimeUnits); // 54
	AddTestStateL(this, &CHWRMTest::LightOffKeyboardWithLimitedDurationL, KTwoTimeUnits); // 55
	AddTestStateL(this, &CHWRMTest::LightOffDisplayWithOverrideTimerL, KTwoTimeUnits); // 56
	AddTestStateL(this, &CHWRMTest::LightBlinkKeyboardWithOverrideTimerL, KTwoTimeUnits); // 57
	AddTestStateL(this, &CHWRMTest::WaitForTargetToGoOnL, 5*KTwoTimeUnits); // primary display
	AddTestStateL(this, &CHWRMTest::LightBlinkDisplayAndKeyboardWithLimitedDuration_KeyboardNoChangeL, KTwoTimeUnits); // 58
	AddTestStateL(this, &CHWRMTest::WaitForTargetToGoOnL, 5*KTwoTimeUnits); // primary display and keyboard
	AddTestStateL(this, &CHWRMTest::LightOffKeyboardWithLimitedDurationL, KTwoTimeUnits); // 59
	AddTestStateL(this, &CHWRMTest::LightGetStatusDisplayL, KTwoTimeUnits); // 61
	
	ExecuteTestL();
	}

void CHWRMTest::TestLightBlinkKeyboardInfiniteCutOffTimerL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightBlinkKeyboardInfiniteCutOffTimerL"));	
	AddLightPreparationStatesL();	
	AddTestStateL(this, &CHWRMTest::LightOnDisplayAndKeyboardL, KTwoTimeUnits); // preparation ???

	AddTestStateL(this, &CHWRMTest::LightBlinkKeyboardInfiniteCutOffTimerL, KTwoTimeUnits); // case 60
	AddTestStateL(this, &CHWRMTest::LightGetStatusKeyboardL, KTwoTimeUnits); // case 61
	AddTestStateL(this, &CHWRMTest::LightGetStatusDisplayL, KTwoTimeUnits); // case 62

	// The following requires the system target to include the primary keyboard
	// (and the primary display, but this is already ON by now)
	AddTestStateL(this, &CHWRMTest::LightSetSensorOnL, KTwoTimeUnits); // case 63
	AddTestStateL(this, &CHWRMTest::LightOnDisplayAndKeyboard_NoStatusChangeL, KTwoTimeUnits); // case 64
	AddTestStateL(this, &CHWRMTest::LightSetSensorOffL, KTwoTimeUnits); // case 65
	AddTestStateL(this, &CHWRMTest::LightBlinkWithDefaultsL, KTwoTimeUnits); // case 66
	AddTestStateL(this, &CHWRMTest::ResetIntensityAndLightTimeOutL, KTwoTimeUnits); // case 67
	AddTestStateL(this, &CHWRMTest::LightBlinkCycleDurationDefaultMismatchL, KTwoTimeUnits); // case 68
	
	ExecuteTestL();		
	}

void CHWRMTest::TestLightGetStatusDisplayL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightGetStatusDisplayL"));	
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::LightOnDisplayL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightGetStatusDisplayL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOffDisplayL, KTwoTimeUnits);
	ExecuteTestL();	
	}

void CHWRMTest::TestVibraReserveSessionsL()
	{	
	INFO_PRINTF1(_L("Start test step: TestVibraReserveSessionsL"));	
	AddTestStateL(this, &CHWRMTest::VibraOpenSessionsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraReserveDefaultParamsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraReserveL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::VibraTryStartL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::Vibra2TryStartL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::Vibra2ReserveL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::Vibra3ReserveL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraReleaseL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::Vibra2ReleaseL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestLightReserveSessionsL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightReserveSessionsL"));	
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::LightReserveDisplayL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOnDisplayAndKeyboard_LimitedDurationL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightReleaseReservedDisplayL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightReleaseUnreservedKeyboardL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOffKeyboardL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestLightReserveSessions2L()
	{
	INFO_PRINTF1(_L("Start test step: TestLightReserveSessions2L"));
	AddLightPreparationStatesL(ETrue);
	
	AddTestStateL(this, &CHWRMTest::LightReserveDisplayL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::Light2ReserveKeyboardL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::Light3ReserveDisplayAndKeyboardL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::LightTryBlinkDisplayAndKeyboardL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::Light2OnDisplayAndKeyboardL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::Light3TryBlinkDisplayAndKeyboardL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::Light2ReleaseUnreservedDisplayAndReservedKeyboardL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightTryBlinkDisplayL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::Light3OnKeyboardL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::Light3ReleaseSuspendedDisplayAndReservedKeyboardL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightReleaseReservedDisplayAndUnreservedKeyboardL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestLightReserveDisplayL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightReserveDisplayL"));	
	AddLightPreparationStatesL(ETrue);
	
	AddTestStateL(this, &CHWRMTest::LightReserveDefaultParamsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightReserveDisplayL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightReserveDisplayAndKeyboardL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::LightReserveDisplayL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestLightGetSupportedTargetsL()
	{
	INFO_PRINTF1(_L("Start test step: TestReserveLightSessionsL"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::LightGetSupportedTargetsL, KTwoTimeUnits);	
	ExecuteTestL();		
	}

void CHWRMTest::TestLightUnsupportedTargetL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightUnsupportedTargetL"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::LightReserveUnsupportedTargetL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOnUnsupportedTargetL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightReleaseUnsupportedTargetL, KTwoTimeUnits);
	ExecuteTestL();			
	}

void CHWRMTest::TestLightUnsupportedDurationL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightUnsupportedDurationL"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::LightOnUnsupportedDurationUpperBoundL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::LightOnUnsupportedDurationLowerBoundL, KOneTimeUnit);
	
	ExecuteTestL();			
	}

void CHWRMTest::TestLightUnsupportedIntensityL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightUnsupportedIntensityL"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::LightOnUnsupportedIntensityUpperBoundL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::LightOnUnsupportedIntensityLowerBoundL, KOneTimeUnit);
	
	ExecuteTestL();			
	}

void CHWRMTest::TestLightOnSimpleCustomTargetsL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightOnSimpleCustom2L"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::LightOnSimpleCustom1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOnSimpleCustom2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOnSimpleCustom4L, KTwoTimeUnits);
	
	ExecuteTestL();				
	}

void CHWRMTest::TestVibraProfilePrivilegedClientL()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraProfilePrivilegedClientL"));					    
    		
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnNoNotificationExpectedL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::VibraOffL, KTwoTimeUnits);// so that VibraOn produces change notification
	AddTestStateL(this, &CHWRMTest::VibraPrivilegedDisableProfileL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::VibraOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraOnNoNotificationExpectedL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::VibraOffWhenDisabledL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraEnableProfileL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestVibraProfilePrivilegedClient2L()
	{
	INFO_PRINTF1(_L("Start test step: TestVibraProfilePrivilegedClient2L"));
	
	AddTestStateL(this, &CHWRMTest::VibraOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraPrivilegedDisableProfileL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraPrivilegedStartDisabled1L, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::VibraOffWhenDisabledL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::VibraEnableProfileL, KTwoTimeUnits);
	
	ExecuteTestL();		
	}

void CHWRMTest::TestModifyTargetL()
	{
	INFO_PRINTF1(_L("Start test step: TestModifyTargetL"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::OpenFlipL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOnDisplay_NoStatusChangeL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOnKeyboardL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CloseFlipL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestSystemLightsL()
	{
	INFO_PRINTF1(_L("Start test step: TestSystemLightsL"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::SystemLightsOnL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::SystemLightsOffL, KOneTimeUnit);
	
	ExecuteTestL();
	}

void CHWRMTest::TestDisplayLightsOnL()
	{
	INFO_PRINTF1(_L("Start test step: TestDisplayLightsOnL"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::DisplayLightsOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::KeyboardLightsOnDisplayOffL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestLightColorL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightColorL"));
	AddLightPreparationStatesL();

	AddTestStateL(this, &CHWRMTest::DisplayLightColorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisplayAndKeyboardLightColorL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestLightColor_ExtendedTargetL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightColor_ExtendedTargetL"));
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::OpenEnhancedLightSessionL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnhancedLightOffAllTargetsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnableNotificationCheckingL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::DisplayLightColor_ExtendedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisplayAndKeyboardLightColor_ExtendedL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestLightDefaultColorL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightDefaultColorL"));
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::DisplayLightDefaultColorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisplayAndKeyboardLightDefaultColorL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestLightDefaultColor_ExtendedTargetL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightDefaultColor_ExtendedTargetL"));
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::OpenEnhancedLightSessionL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnhancedLightOffAllTargetsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnableNotificationCheckingL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::DisplayLightDefaultColor_ExtendedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisplayAndKeyboardLightDefaultColor_ExtendedL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestLightColor_UnSupportedTargetL()
	{
	INFO_PRINTF1(_L("Start test step: TestDisplayLightsOnL"));
	AddLightPreparationStatesL();

	AddTestStateL(this, &CHWRMTest::LightColorUnSupportedTargetL, KTwoTimeUnits);
	ExecuteTestL();	
	}

void CHWRMTest::TestLightColor_PluginErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightColor_PluginErrorL"));
	AddLightPreparationStatesL();

	AddTestStateL(this, &CHWRMTest::LightColorPluginErrorL, KTwoTimeUnits);
	ExecuteTestL();	
	}

void CHWRMTest::TestFrozenStateRestoreLightColor_ExtendedTargetL()
	{
	INFO_PRINTF1(_L("Start test step: TestFrozenStateRestoreLightColor_ExtendedTargetL"));
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::OpenEnhancedLightSessionL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnhancedLightOffAllTargetsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::ReserveDisplayWithfrozenstateOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOnDurationRGB_ExtendedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ReleaseDisplay_ExtendedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ReserveDisplayWithfrozenstateOn_RestoreColorL, KTwoTimeUnits);
	
	//AddTestStateL(this, &CHWRMTest::ReleaseDisplay_ExtendedL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestBaseStateRestoreLightColor_ExtendedTargetL()
	{
	INFO_PRINTF1(_L("Start test step: TestBaseStateRestoreLightColor_ExtendedTargetL"));
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::OpenEnhancedLightSessionL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnhancedLightOffAllTargetsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit);


    AddTestStateL(this, &CHWRMTest::DisplayLightColor_ExtendedL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::LightOnDurationRGB_ExtendedWithExpectL, 2*KTwoTimeUnits);
    
	ExecuteTestL();	
	}

void CHWRMTest::TestLightColor_MultiClientL()
	{
	INFO_PRINTF1(_L("Start test step: TestDisplayLightsOnL"));
	AddLightPreparationStatesL(ETrue);

	AddTestStateL(this, &CHWRMTest::ReserveDisplay_Client1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisplayLightColor_Client1L, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::ReserveKeyboard_Client2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::KeyboardLightColor_Client2L, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::ReserveDisplayKeyboard_Client3L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisplayKeyboardLightColor_Client3L, KTwoTimeUnits);

	// keyboard color should be restored for client3 session
	AddTestStateL(this, &CHWRMTest::ReleaseDisplayKeyboard_Client2L, KTwoTimeUnits);

	// default color for keyboard should be restored
	AddTestStateL(this, &CHWRMTest::ReleaseDisplayKeyboard_Client3L, KTwoTimeUnits);

	// default color for display should be restored
	AddTestStateL(this, &CHWRMTest::ReleaseDisplayKeyboard_Client1L, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestLightColor_MultiClient_ExtendedTargetL()
	{
	INFO_PRINTF1(_L("Start test step: TestDisplayLightsOnL"));
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::OpenEnhancedLightSessionL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnhancedLightOffAllTargetsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnableNotificationCheckingL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::ReserveDisplay_Client1_ExtL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisplayLightColor_Client1_ExtL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::ReserveKeyboard_Client2_ExtL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::KeyboardLightColor_Client2_ExtL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::ReserveDisplayKeyboard_Client3_ExtL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisplayKeyboardLightColor_Client3_ExtL, KTwoTimeUnits);

	// keyboard color should be restored for client3 session
	AddTestStateL(this, &CHWRMTest::ReleaseDisplayKeyboard_Client2_ExtL, KTwoTimeUnits);

	// default color for keyboard should be restored
	AddTestStateL(this, &CHWRMTest::ReleaseDisplayKeyboard_Client3_ExtL, KTwoTimeUnits);

	// default color for display should be restored
	AddTestStateL(this, &CHWRMTest::ReleaseDisplayKeyboard_Client1_ExtL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestLightColor_WhenInUseL()
	{
	INFO_PRINTF1(_L("Start test step: TestDisplayLightsOnL"));
	AddLightPreparationStatesL(ETrue);
	
	AddTestStateL(this, &CHWRMTest::ReserveDisplay_Client1L, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::ReserveKeyboard_Client2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisplayKeyboardLightColor_Client1_ErrorL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestLightDefaultColor_WhenInUseL()
	{
	INFO_PRINTF1(_L("Start test step: TestDisplayLightsOnL"));
	AddLightPreparationStatesL(ETrue);
	
	AddTestStateL(this, &CHWRMTest::ReserveDisplay_Client1L, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::ReserveKeyboard_Client2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisplayKeyboardLightDefaultColor_Client1_ErrorL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::SetClientColorL()
	{
	INFO_PRINTF1(_L("Step state: SetClientColorL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;
	
	//Set color for the client session
	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplayAndKeyboard, 10, 20, 30, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 10,20,30);
	light.SetLightColorL(CHWRMLight::EPrimaryDisplayAndKeyboard, rgb);
	}

void CHWRMTest::SetClientColor_ExtendedL()
	{
	INFO_PRINTF1(_L("Step state: SetClientColor_ExtendedL"));
	
	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, 10, 20, 30, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 10, 20, 30);
	light.SetLightColorL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, rgb);
	}

void CHWRMTest::RestoreClientColorL()
	{
	INFO_PRINTF1(_L("Step state: RestoreClientColorL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();

    RMockLight& mockLight = GetMockLight();
	//Restore the client session color	
	DisableNotificationCheckingL();
    
    HWRMLightCommand::TLightsOffData lightOffData;
    lightOffData.iTarget  = CHWRMLight::EPrimaryDisplay;
    lightOffData.iFadeOut = ETrue;
    HWRMLightCommand::TLightsOffCmdPackage lightsDataOffPckg(lightOffData);
    
	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplay, 10, 20, 30, EFalse, KErrNone, 0);    

    
    mockLight.ExpectProcessCmdL(HWRMLightCommand::ELightsOffCmdId, lightsDataOffPckg);
    HWRMLightCommand::TErrorCodeResponsePackage response(KErrNone);
	mockLight.CompleteL(HWRMLightCommand::ELightsOffCmdId, 0, response);
    
    ExpectSetColorCommand(CHWRMLight::EPrimaryKeyboard, 10, 20, 30, EFalse, KErrNone, 0);
	}

void CHWRMTest::RestoreClientColor_ExtendedL()
	{
	INFO_PRINTF1(_L("Step state: RestoreClientColor_ExtendedL"));
	
	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL();
    DisableNotificationCheckingL();
	//Restore the client session color	
	
    RMockLight& mockLight = GetMockLight();
    HWRMLightCommand::TLightsOffData lightOffData;
    lightOffData.iTarget  = CHWRMLight::EPrimaryDisplay;
    lightOffData.iFadeOut = ETrue;
    HWRMLightCommand::TLightsOffCmdPackage lightsDataOffPckg(lightOffData);
    
	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplay, 10, 20, 30, EFalse, KErrNone);

    
    mockLight.ExpectProcessCmdL(HWRMLightCommand::ELightsOffCmdId, lightsDataOffPckg);
    HWRMLightCommand::TErrorCodeResponsePackage response(KErrNone);
	mockLight.CompleteL(HWRMLightCommand::ELightsOffCmdId, 0, response);
    
    ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryKeyboard, 10, 20, 30, EFalse, KErrNone, 0);
	}

void CHWRMTest::LightOnDurationRGBL()
	{
	INFO_PRINTF1(_L("Step state: LightOnDurationRGBL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	//Set color for the specific duration
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOn);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOn);
	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplayAndKeyboard, 75, 25, 100, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 75, 25, 100);
	light.LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, 4*1000, rgb);
	}

void CHWRMTest::LightOnDurationWithIntensityRGBL()
	{
	INFO_PRINTF1(_L("Step state: LightOnDurationWithIntensityRGBL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	//Set color for the specific duration
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOn);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOn);
	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplayAndKeyboard, 75, 25, 100, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 75, 25, 100);
	light.LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, 4*1000, 20, ETrue, rgb);
	}


void CHWRMTest::LightOnDurationRGB_ExtendedWithExpectL()
	{
	INFO_PRINTF1(_L("Step state: LightOnDurationRGB_ExtendedL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	//Set color for the specific duration
	ExpectLightStatusNotificationL(CHWRMExtendedLight::EPrimaryDisplay, CHWRMLight::ELightOn);
	//ExpectLightStatusNotificationL(CHWRMExtendedLight::EPrimaryKeyboard, CHWRMLight::ELightOn);
	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplay, 75, 25, 100, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 75, 25, 100);
	light.LightOnL(CHWRMExtendedLight::EPrimaryDisplay, 4*1000, rgb);
    INFO_PRINTF1(_L("Step state: LightOnDurationRGB_ExtendedL Waiting"));
    User::After(2*1000*1000);
    ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplay, 10, 15, 20, EFalse, KErrNone);
    User::After(3*1000*1000);
    INFO_PRINTF1(_L("Step state: LightOnDurationRGB_ExtendedL Waiting over"));    
	}


void CHWRMTest::LightOnDurationRGB_ExtendedL()
	{
	INFO_PRINTF1(_L("Step state: LightOnDurationRGB_ExtendedL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	//Set color for the specific duration
	ExpectExtLightStatusNotificationL(CHWRMExtendedLight::EPrimaryDisplay, CHWRMExtendedLight::ELightOn);
	ExpectExtLightStatusNotificationL(CHWRMExtendedLight::EPrimaryKeyboard, CHWRMExtendedLight::ELightOn);
	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, 75, 25, 100, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 75, 25, 100);
	light.LightOnL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, 4*1000, rgb);
	}

void CHWRMTest::LightBlinkDurationRGBL()
	{
	INFO_PRINTF1(_L("Step state: LightBlinkDurationRGBL"));
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	//Set color for the specific duration
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightBlink);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightBlink);
	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplayAndKeyboard, 75, 25, 100, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 75, 25, 100);
	light.LightBlinkL(CHWRMLight::EPrimaryDisplayAndKeyboard, 4*1000, rgb);
	}

void CHWRMTest::LightBlinkDurationWithIntensityRGBL()
	{
	INFO_PRINTF1(_L("Step state: LightBlinkDurationWithIntensityRGBL"));
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	//Set color for the specific duration
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightBlink);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightBlink);
	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplayAndKeyboard, 75, 25, 100, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 75, 25, 100);
	light.LightBlinkL(CHWRMLight::EPrimaryDisplayAndKeyboard, 4*1000, 500, 500, 20, rgb);
	}

void CHWRMTest::LightBlinkDurationRGB_ExtendedL()
	{
	INFO_PRINTF1(_L("Step state: LightBlinkDurationRGB_ExtendedL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;
	//Set color for the specific duration
	ExpectExtLightStatusNotificationL(CHWRMExtendedLight::EPrimaryDisplay, CHWRMExtendedLight::ELightBlink);
	ExpectExtLightStatusNotificationL(CHWRMExtendedLight::EPrimaryKeyboard, CHWRMExtendedLight::ELightBlink);
	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, 75, 25, 100, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 75, 25, 100);
	light.LightBlinkL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard, 4*1000, rgb);
	}

void CHWRMTest::LightEnhancedLeave1L()
    {
    INFO_PRINTF1(_L("Step state: LightEnhancedLeave1L"));

    ExpectLeave(KErrBadHandle);
    CHWRMEnhancedLight* p = CHWRMEnhancedLight::NewL((MHWRMExtendedLightObserver*)NULL);
    
    }

void CHWRMTest::LightEnhancedLeave2L()
    {
    INFO_PRINTF1(_L("Step state: LightEnhancedLeave2L"));

    ExpectLeave(KErrBadHandle);
    CHWRMEnhancedLight* p = CHWRMEnhancedLight::NewLC((MHWRMExtendedLightObserver*)NULL);
    
    }

void CHWRMTest::LightEnhancedLeave3L()
    {
    INFO_PRINTF1(_L("Step state: LightEnhancedLeave3L"));

    ExpectLeave(KErrBadHandle);
    CHWRMEnhancedLight* p = CHWRMEnhancedLight::NewL((MHWRMExtendedLightObserver*)NULL, 0);
    
    }



void CHWRMTest::TestLightOnDurationRGBL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightOnDurationL"));
	AddLightPreparationStatesL();

	AddTestStateL(this, &CHWRMTest::SetClientColorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOnDurationRGBL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RestoreClientColorL, KTwoTimeUnits*4);

	ExecuteTestL();
	}

void CHWRMTest::TestLightOnDurationWithIntenistyRGBL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightOnDurationWithIntenistyRGBL"));
	AddLightPreparationStatesL();

	AddTestStateL(this, &CHWRMTest::SetClientColorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOnDurationWithIntensityRGBL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RestoreClientColorL, KTwoTimeUnits*4);

	ExecuteTestL();
	}


void CHWRMTest::TestLightOnDurationRGB_ExtendedL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightOnDurationRGB_ExtendedL"));
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::OpenEnhancedLightSessionL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnhancedLightOffAllTargetsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnableNotificationCheckingL, KOneTimeUnit);

	AddTestStateL(this, &CHWRMTest::SetClientColor_ExtendedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightOnDurationRGB_ExtendedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RestoreClientColor_ExtendedL, KTwoTimeUnits*4);

	ExecuteTestL();
	}

void CHWRMTest::TestLightBlinkDurationRGBL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightBlinkDurationL"));
	AddLightPreparationStatesL();

	AddTestStateL(this, &CHWRMTest::SetClientColorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightBlinkDurationRGBL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RestoreClientColorL, KTwoTimeUnits*4);

	ExecuteTestL();
	}

void CHWRMTest::TestLightBlinkDurationWithIntensityRGBL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightBlinkDurationL"));
	AddLightPreparationStatesL();

	AddTestStateL(this, &CHWRMTest::SetClientColorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightBlinkDurationWithIntensityRGBL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RestoreClientColorL, KTwoTimeUnits*4);

	ExecuteTestL();
	}


void CHWRMTest::TestLightBlinkDurationRGB_ExtendedL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightBlinkDurationRGB_ExtendedL"));
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::OpenEnhancedLightSessionL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnhancedLightOffAllTargetsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnableNotificationCheckingL, KOneTimeUnit);

	AddTestStateL(this, &CHWRMTest::SetClientColor_ExtendedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::LightBlinkDurationRGB_ExtendedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RestoreClientColor_ExtendedL, KTwoTimeUnits*4);

	ExecuteTestL();
	}

void CHWRMTest::TestLightEnhancedLightLeaveL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightSetColorReserveL"));

	AddTestStateL(this, &CHWRMTest::LightEnhancedLeave1L, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::LightEnhancedLeave2L, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::LightEnhancedLeave3L, KTwoTimeUnits);

	ExecuteTestL();	
	}



void CHWRMTest::TestLightSetColorReserveL()
	{
	INFO_PRINTF1(_L("Start test step: TestLightSetColorReserveL"));
	AddLightPreparationStatesL(ETrue);

	AddTestStateL(this, &CHWRMTest::ReserveDisplay_Client1L, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::ReserveDisplay_Client2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::DisplayLightOnColor_Client2L, 3*KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestPowerStatesL()
    {
    INFO_PRINTF1(_L("Start test step: TestPowerStatesL"));
    AddTestStateL(this, &CHWRMTest::PowerStateOpenL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExpectLegacyBatteryStatusL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExpectLegacyChargingStatusL, KTwoTimeUnits);    
    AddTestStateL(this, &CHWRMTest::BatteryLevelL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::BatteryStateL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusL, KTwoTimeUnits);    
    
    ExecuteTestL(); 
    }

void CHWRMTest::TestPowerExtendedBatteryStatusL()
    {
    INFO_PRINTF1(_L("Start test step: TestPowerExtendedBatteryStatusL"));
    AddTestStateL(this, &CHWRMTest::PowerStateOpenL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExpectNewBatteryStatusL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExpectNewChargingStatusL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExtendedBatteryStateStatusOkL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExtendedBatteryStateUnknownErrorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ExtendedBatteryStateUnsupportedL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExtendedBatteryStateAuthErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExtendedBatteryStateStatusLowL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExtendedBatteryStateStatusEmptyL, KTwoTimeUnits);    
    // Reset battery status to "Ok"
    AddTestStateL(this, &CHWRMTest::ExtendedBatteryStateStatusOkL, KTwoTimeUnits);
    
    ExecuteTestL();
    }
	
void CHWRMTest::TestPowerBatteryStatusL()
    {
    INFO_PRINTF1(_L("Start test step: TestPowerBatteryStatusL"));
    AddTestStateL(this, &CHWRMTest::PowerStateOpenL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExpectLegacyBatteryStatusL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExpectLegacyChargingStatusL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::BatteryStateStatusOkL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::BatteryStateUnknownErrorL, KTwoTimeUnits);
		AddTestStateL(this, &CHWRMTest::BatteryStateUnsupportedL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::BatteryStateAuthErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::BatteryStateStatusLowL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::BatteryStateStatusEmptyL, KTwoTimeUnits);    
    // Reset battery status to "Ok"
    AddTestStateL(this, &CHWRMTest::BatteryStateStatusOkL, KTwoTimeUnits);
    
    ExecuteTestL();
    }	

void CHWRMTest::TestPowerChargingStatusL()
    {
    INFO_PRINTF1(_L("Start test step: TestPowerChargingStatusL"));
    AddTestStateL(this, &CHWRMTest::PowerStateOpenL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExpectLegacyBatteryStatusL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ExpectLegacyChargingStatusL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusNotConnectedL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusChargingOkL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusNotChargingL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusAlmostCompleteL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusCompletedChargingL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusChargingContinuesL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusIllegalErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusUnderVoltageErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusOverVoltageErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusTooHotErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingStatusTooColdErrorL, KTwoTimeUnits);
    //Reset Charging Status to "completed"
    AddTestStateL(this, &CHWRMTest::ChargingStatusCompletedChargingL, KTwoTimeUnits);
    
    ExecuteTestL();
    }

void CHWRMTest::TestExtendedLightSessionsL()
	{
	INFO_PRINTF1(_L("Start test step: TestExtendedLightSessionsL"));
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::OpenExtLightSessionL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ExtendedLightOffAllTargetsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::EnableNotificationCheckingL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::ExtendedLightOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ExtendedLightOffL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ExtendedLight2BlinkL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ExtendedLight2OffL, KTwoTimeUnits);
	
	ExecuteTestL();
	}
	

void CHWRMTest::AddLightPreparationStatesL(TBool aMultipleSessions)
	{
	AddTestStateL(this, &CHWRMTest::DisableNotificationCheckingL, KOneTimeUnit);
	if (aMultipleSessions)
		{
		AddTestStateL(this, &CHWRMTest::LightOpenSessionsL, KTwoTimeUnits);
		}
	else
		{
		AddTestStateL(this, &CHWRMTest::LightOpenL, KTwoTimeUnits);
		}
	AddTestStateL(this, &CHWRMTest::LightOffAllTargetsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::WaitOneTenthL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::EnableNotificationCheckingL, KOneTimeUnit);
	}

void CHWRMTest::TestInvalidSessionsL()
	{
	INFO_PRINTF1(_L("Start test step: TestInvalidSessionsL"));
	AddTestStateL(this, &CHWRMTest::InvalidSessionVibraL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::InvalidSessionLightL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::InvalidSessionExtendedLightL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerConnectL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerConnectL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerGetBatteryInfoL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBatteryInfoL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerGetBatteryInfoL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerGetBatteryInfoMultiClientL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBatteryInfoMultiClientL"));
	AddTestStateL(this, &CHWRMTest::PowerOpen3SessionsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerGetBatteryInfoMultiClientL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerGetBatteryInfoWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBatteryInfoWithPluginErrorL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerGetBatteryInfoWithPluginErrorL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerGetBatteryVoltageInfoL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBatteryVoltageInfoL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerGetBatteryVoltageInfoL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerGetBatteryVoltageInfoMultiClientL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBatteryVoltageInfoMultiClientL"));
	AddTestStateL(this, &CHWRMTest::PowerOpen3SessionsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerGetBatteryVoltageInfoMultiClientL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerGetBatteryVoltageInfoWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBatteryVoltageInfoWithPluginErrorL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerGetBatteryVoltageInfoWithPluginErrorL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerGetBaseTimeIntervalL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBaseTimeIntervalL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerGetBaseTimeIntervalL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
#define SET_CHARGINGSTATUS(s) \
	{ \
	iMockPowerState.CompleteL(KHWRMExtendedChargingStatus, s, 5); \
	User::After(KOneSecond); \
	iChargingStatus = s; \
	if(iChargingStatusCBregistered) ExpectedExtendedChargingStatusNotificationsL(s); \
	}

void CHWRMTest::PowerAndPowerStateOpenL()
	{
	INFO_PRINTF1(_L("Step state: PowerAndPowerStateOpenL"));
	PowerOpenL();
	OpenPowerStateSessionforChrgStatusL();

	// No callback registered yet
	iChargingStatusCBregistered = 0;
	}

void CHWRMTest::PowerAndPowerState3SessionsOpenL()
	{
	INFO_PRINTF1(_L("Step state: PowerAndPowerState3SessionsOpenL"));
	PowerOpen3SessionsL();
	OpenPowerStateSessionforChrgStatusL();

	// No callback registered yet
	iChargingStatusCBregistered = 0;
	}

void CHWRMTest::PowerChargingStatusErrorL()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusErrorL"));
	SET_CHARGINGSTATUS(EChargingStatusError);
	}

void CHWRMTest::PowerChargingStatusIllegalErrorL()
    {
    INFO_PRINTF1(_L("Step state: PowerChargingStatusIllegalErrorL"));
    SET_CHARGINGSTATUS(EChargingStatusIllegalChargerError);
    }

void CHWRMTest::PowerChargingStatusUnderVoltageErrorL()
    {
    INFO_PRINTF1(_L("Step state: PowerChargingStatusUnderVoltageErrorL"));
    SET_CHARGINGSTATUS(EChargingStatusChargerUnderVoltageError);
    }

void CHWRMTest::PowerChargingStatusOverVoltageErrorL()
    {
    INFO_PRINTF1(_L("Step state: PowerChargingStatusOverVoltageErrorL"));
    SET_CHARGINGSTATUS(EChargingStatusChargerOverVoltageError);
    }

void CHWRMTest::PowerChargingStatusTooHotErrorL()
    {
    INFO_PRINTF1(_L("Step state: PowerChargingStatusTooHotErrorL"));
    SET_CHARGINGSTATUS(EChargingStatusTempTooHotError);
    }

void CHWRMTest::PowerChargingStatusTooColdErrorL()
    {
    INFO_PRINTF1(_L("Step state: PowerChargingStatusTooColdErrorL"));
    SET_CHARGINGSTATUS(EChargingStatusTempTooColdError);
    }

void CHWRMTest::PowerChargingStatusNotConnectedL()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusNotConnectedL"));
	SET_CHARGINGSTATUS(EChargingStatusNotConnected);
	}

void CHWRMTest::PowerChargingStatusChargingL()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusChargingL"));
	SET_CHARGINGSTATUS(EChargingStatusCharging);
	}

void CHWRMTest::PowerChargingStatusNotChargingL()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusNotChargingL"));
	SET_CHARGINGSTATUS(EChargingStatusNotCharging);
	}

void CHWRMTest::PowerChargingStatusAlmostCompleteL()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusAlmostCompleteL"));
	SET_CHARGINGSTATUS(EChargingStatusAlmostComplete);
	}

void CHWRMTest::PowerChargingStatusCompleteL()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusCompleteL"));
	SET_CHARGINGSTATUS(EChargingStatusChargingComplete);
	}

void CHWRMTest::PowerChargingStatusContinuedL()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusContinuedL"));
	SET_CHARGINGSTATUS(EChargingStatusChargingContinued);
	}

void CHWRMTest::PowerChargingStatusError3L()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusError3L"));
	if(iChargingStatusCBregistered)
		{
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusError);
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusError);
		}
	SET_CHARGINGSTATUS(EChargingStatusError);
	}

void CHWRMTest::PowerChargingStatusIllegalError3L()
    {
    INFO_PRINTF1(_L("Step state: PowerChargingStatusIllegalError3L"));
    if(iChargingStatusCBregistered)
        {
        ExpectedExtendedChargingStatusNotificationsL(EChargingStatusIllegalChargerError);
        ExpectedExtendedChargingStatusNotificationsL(EChargingStatusIllegalChargerError);
        }
    SET_CHARGINGSTATUS(EChargingStatusIllegalChargerError);
    }

void CHWRMTest::PowerChargingStatusUnderVoltageError3L()
    {
    INFO_PRINTF1(_L("Step state: PowerChargingStatusUnderVoltageError3L"));
    if(iChargingStatusCBregistered)
        {
        ExpectedExtendedChargingStatusNotificationsL(EChargingStatusChargerUnderVoltageError);
        ExpectedExtendedChargingStatusNotificationsL(EChargingStatusChargerUnderVoltageError);
        }
    SET_CHARGINGSTATUS(EChargingStatusChargerUnderVoltageError);
    }

void CHWRMTest::PowerChargingStatusOverVoltageError3L()
    {
    INFO_PRINTF1(_L("Step state: PowerChargingStatusOverVoltageError3L"));
    if(iChargingStatusCBregistered)
        {
        ExpectedExtendedChargingStatusNotificationsL(EChargingStatusChargerOverVoltageError);
        ExpectedExtendedChargingStatusNotificationsL(EChargingStatusChargerOverVoltageError);
        }    
    SET_CHARGINGSTATUS(EChargingStatusChargerOverVoltageError);
    }

void CHWRMTest::PowerChargingStatusTooHotError3L()
    {
    INFO_PRINTF1(_L("Step state: PowerChargingStatusTooHotError3L"));
    if(iChargingStatusCBregistered)
        {
        ExpectedExtendedChargingStatusNotificationsL(EChargingStatusTempTooHotError);
        ExpectedExtendedChargingStatusNotificationsL(EChargingStatusTempTooHotError);
        }   
    SET_CHARGINGSTATUS(EChargingStatusTempTooHotError);
    }

void CHWRMTest::PowerChargingStatusTooColdError3L()
    {
    INFO_PRINTF1(_L("Step state: PowerChargingStatusTooColdError3L"));
    if(iChargingStatusCBregistered)
        {
        ExpectedExtendedChargingStatusNotificationsL(EChargingStatusTempTooColdError);
        ExpectedExtendedChargingStatusNotificationsL(EChargingStatusTempTooColdError);
        }
    SET_CHARGINGSTATUS(EChargingStatusTempTooColdError);
    }

void CHWRMTest::PowerChargingStatusNotConnected3L()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusNotConnected3L"));
	if(iChargingStatusCBregistered)
		{
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusNotConnected);
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusNotConnected);
		}
	SET_CHARGINGSTATUS(EChargingStatusNotConnected);
	}

void CHWRMTest::PowerChargingStatusCharging3L()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusCharging3L"));
	if(iChargingStatusCBregistered)
		{
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusCharging);
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusCharging);
		}
	SET_CHARGINGSTATUS(EChargingStatusCharging);
	}

void CHWRMTest::PowerChargingStatusNotCharging3L()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusNotCharging3L"));
	if(iChargingStatusCBregistered)
		{
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusNotCharging);
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusNotCharging);
		}
	SET_CHARGINGSTATUS(EChargingStatusNotCharging);
	}

void CHWRMTest::PowerChargingStatusAlmostComplete3L()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusAlmostComplete3L"));
	if(iChargingStatusCBregistered)
		{
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusAlmostComplete);
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusAlmostComplete);
		}
	SET_CHARGINGSTATUS(EChargingStatusAlmostComplete);
	}

void CHWRMTest::PowerChargingStatusComplete3L()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusComplete3L"));
	if(iChargingStatusCBregistered)
		{
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusChargingComplete);
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusChargingComplete);
		}
	SET_CHARGINGSTATUS(EChargingStatusChargingComplete);
	}

void CHWRMTest::PowerChargingStatusContinued3L()
	{
	INFO_PRINTF1(_L("Step state: PowerChargingStatusContinued3L"));
	if(iChargingStatusCBregistered)
		{
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusChargingContinued);
		ExpectedExtendedChargingStatusNotificationsL(EChargingStatusChargingContinued);
		}
	SET_CHARGINGSTATUS(EChargingStatusChargingContinued);
	}


void CHWRMTest::CheckPowerChargingStatusL()
	{
	INFO_PRINTF1(_L("Step state: CheckPowerChargingStatusL"));
	CHWRMPower& power = GetPowerSessionL(0);
	CHWRMPower::TBatteryChargingStatus chrgstatus;

	User::LeaveIfError(power.GetBatteryChargingStatus(chrgstatus));

	if(chrgstatus != iChargingStatus)
		{
		INFO_PRINTF3(_L("!Error: Expected Charging Status %d Actual %d"), iChargingStatus, chrgstatus);
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::CheckPowerChargingStatusMultiClient3L()
	{
	INFO_PRINTF1(_L("Step state: CheckPowerChargingStatusMultiClient3L"));
	CHWRMPower& power0 = GetPowerSessionL(0);
	CHWRMPower& power1 = GetPowerSessionL(1);
	CHWRMPower& power2 = GetPowerSessionL(2);
	CHWRMPower::TBatteryChargingStatus chrgstatus0, chrgstatus1, chrgstatus2;

	User::LeaveIfError(power0.GetBatteryChargingStatus(chrgstatus0));
	User::LeaveIfError(power1.GetBatteryChargingStatus(chrgstatus1));
	User::LeaveIfError(power2.GetBatteryChargingStatus(chrgstatus2));

	if(chrgstatus0 != iChargingStatus)
		{
		INFO_PRINTF3(_L("!Error: Expected Charging Status %d Actual %d"), iChargingStatus, chrgstatus0);
		User::Leave(KErrGeneral);
		}
	
	if(chrgstatus1 != iChargingStatus)
		{
		INFO_PRINTF3(_L("!Error: Expected Charging Status %d Actual %d"), iChargingStatus, chrgstatus1);
		User::Leave(KErrGeneral);
		}

	if(chrgstatus2 != iChargingStatus)
		{
		INFO_PRINTF3(_L("!Error: Expected Charging Status %d Actual %d"), iChargingStatus, chrgstatus2);
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::RegisterChargingStatusCallbackL()
	{
	INFO_PRINTF1(_L("Step state: RegisterChargingStatusCallbackL"));
	CHWRMPower& power = GetPowerSessionL(0);

	power.SetBatteryChargingObserver(this, NULL, NULL);

	// Callback registered now
	iChargingStatusCBregistered = 1;
	}

void CHWRMTest::RegisterChargingStatusCallbackMultiClient3L()
	{
	INFO_PRINTF1(_L("Step state: RegisterChargingStatusCallbackMultiClient3L"));
	CHWRMPower& power0 = GetPowerSessionL(0);
	CHWRMPower& power1 = GetPowerSessionL(1);
	CHWRMPower& power2 = GetPowerSessionL(2);

	power0.SetBatteryChargingObserver(this, NULL, NULL);
	power1.SetBatteryChargingObserver(this, NULL, NULL);
	power2.SetBatteryChargingObserver(this, NULL, NULL);

	// Callback registered now
	iChargingStatusCBregistered = 1;
	}

void CHWRMTest::RegisterChargingTimeCallbackL()
	{
	INFO_PRINTF1(_L("Step state: RegisterChargingTimeCallbackL"));
	CHWRMPower& power = GetPowerSessionL(0);

	power.SetBatteryChargingObserver(NULL, NULL, this);
	}

void CHWRMTest::RegisterChargingTimeCallbackMultiClient3L()
	{
	INFO_PRINTF1(_L("Step state: RegisterChargingTimeCallbackMultiClient3L"));
	CHWRMPower& power0 = GetPowerSessionL(0);
	CHWRMPower& power1 = GetPowerSessionL(1);
	CHWRMPower& power2 = GetPowerSessionL(2);

	power0.SetBatteryChargingObserver(NULL, NULL, this);
	power1.SetBatteryChargingObserver(NULL, NULL, this);
	power2.SetBatteryChargingObserver(NULL, NULL, this);
	}

void CHWRMTest::RegisterChargingCurrentCallbackL()
	{
	INFO_PRINTF1(_L("Step state: RegisterChargingCurrentCallbackL"));
	CHWRMPower& power = GetPowerSessionL(0);

	power.SetBatteryChargingObserver(NULL, this, NULL);
	}

void CHWRMTest::RegisterChargingCurrentCallbackMultiClient3L()
	{
	INFO_PRINTF1(_L("Step state: RegisterChargingCurrentCallbackMultiClient3L"));
	CHWRMPower& power0 = GetPowerSessionL(0);
	CHWRMPower& power1 = GetPowerSessionL(1);
	CHWRMPower& power2 = GetPowerSessionL(2);

	power0.SetBatteryChargingObserver(NULL, this, NULL);
	power1.SetBatteryChargingObserver(NULL, this, NULL);
	power2.SetBatteryChargingObserver(NULL, this, NULL);
	}

void CHWRMTest::ResetChargingStatusCBFlagL()
	{
	INFO_PRINTF1(_L("Step state: ResetChargingStatusCBFlagL"));
	iChargingStatusCBregistered = 0;
	}

void CHWRMTest::StartRemainingChargingTimeNoCallbackL()
	{
	INFO_PRINTF1(_L("Step state: StartRemainingChargingTimeNoCallback"));
	ExpectLeave(KErrBadHandle);
	StartRemainingChargingTimeL();
	}

void CHWRMTest::StartRemainingChargingTimeL()
	{
	INFO_PRINTF1(_L("Step state: StartRemainingChargingTimeL"));
	CHWRMPower& power = GetPowerSessionL(0);
	TRequestStatus status;

	User::LeaveIfError(power.NotifyBatteryFullChargingTime(status));

	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::StartChargingCurrentNoCallbackL()
	{
	INFO_PRINTF1(_L("Step state: StartChargingCurrentNoCallbackL"));
	ExpectLeave(KErrBadHandle);
	StartChargingCurrentL();
	}

void CHWRMTest::StartChargingCurrentL()
	{
	INFO_PRINTF1(_L("Step state: StartChargingCurrentL"));
	CHWRMPower& power = GetPowerSessionL(0);
	TRequestStatus status;

	User::LeaveIfError(power.NotifyAverageChargingCurrent(status, 3));

	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::StartChargingCurrent4L()
	{
	INFO_PRINTF1(_L("Step state: StartChargingCurrent4L"));
	CHWRMPower& power = GetPowerSessionL(0);
	TRequestStatus status;

	User::LeaveIfError(power.NotifyAverageChargingCurrent(status, 4));

	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::StartRemainingChargingTime1L()
	{
	INFO_PRINTF1(_L("Step state: StartRemainingChargingTimeL"));
	CHWRMPower& power = GetPowerSessionL(1);
	TRequestStatus status;

	User::LeaveIfError(power.NotifyBatteryFullChargingTime(status));

	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::StartChargingCurrent1L()
	{
	INFO_PRINTF1(_L("Step state: StartChargingCurrent1L"));
	CHWRMPower& power = GetPowerSessionL(1);
	TRequestStatus status;

	User::LeaveIfError(power.NotifyAverageChargingCurrent(status, 6));

	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::StartRemainingChargingTime2L()
	{
	INFO_PRINTF1(_L("Step state: StartRemainingChargingTimeL"));
	CHWRMPower& power = GetPowerSessionL(2);
	TRequestStatus status;

	User::LeaveIfError(power.NotifyBatteryFullChargingTime(status));

	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::StartChargingCurrent2L()
	{
	INFO_PRINTF1(_L("Step state: StartChargingCurrent2L"));
	CHWRMPower& power = GetPowerSessionL(2);
	TRequestStatus status;

	User::LeaveIfError(power.NotifyAverageChargingCurrent(status, 9));

	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}


void CHWRMTest::CancelRemainingChargingTimeL()
	{
	INFO_PRINTF1(_L("Step state: CancelRemainingChargingTimeL"));
	CHWRMPower& power = GetPowerSessionL(0);

	power.StopChargingNotificationL(CHWRMPower::EChargingNotifierBatteryFullChargingTime);
	}

void CHWRMTest::CancelChargingCurrentL()
	{
	INFO_PRINTF1(_L("Step state: CancelChargingCurrentL"));
	CHWRMPower& power = GetPowerSessionL(0);

	power.StopChargingNotificationL(CHWRMPower::EChargingNotifierChargingCurrent);
	}

void CHWRMTest::CancelRemainingChargingTime1L()
	{
	INFO_PRINTF1(_L("Step state: StartRemainingChargingTimeL"));
	CHWRMPower& power = GetPowerSessionL(1);

	power.StopChargingNotificationL(CHWRMPower::EChargingNotifierBatteryFullChargingTime);
	}

void CHWRMTest::CancelChargingCurrent1L()
	{
	INFO_PRINTF1(_L("Step state: CancelChargingCurrent1L"));
	CHWRMPower& power = GetPowerSessionL(1);

	power.StopChargingNotificationL(CHWRMPower::EChargingNotifierChargingCurrent);
	}

void CHWRMTest::CancelRemainingChargingTime2L()
	{
	INFO_PRINTF1(_L("Step state: StartRemainingChargingTimeL"));
	CHWRMPower& power = GetPowerSessionL(2);

	power.StopChargingNotificationL(CHWRMPower::EChargingNotifierBatteryFullChargingTime);
	}

void CHWRMTest::CancelChargingCurrent2L()
	{
	INFO_PRINTF1(_L("Step state: CancelChargingCurrent2L"));
	CHWRMPower& power = GetPowerSessionL(2);

	power.StopChargingNotificationL(CHWRMPower::EChargingNotifierChargingCurrent);
	}

void CHWRMTest::StartRemainingChargingTimeErrorL()
	{
	INFO_PRINTF1(_L("Step state: StartRemainingChargingTimeErrorL"));
	CHWRMPower& power = GetPowerSessionL(0);
	TRequestStatus status;
	TInt err(KErrNone);

	err = power.NotifyBatteryFullChargingTime(status);

	if(err != KErrNotReady)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::StartChargingCurrentErrorL()
	{
	INFO_PRINTF1(_L("Step state: StartChargingCurrentErrorL"));
	CHWRMPower& power = GetPowerSessionL(0);
	TRequestStatus status;
	TInt err(KErrNone);

	err = power.NotifyAverageChargingCurrent(status, 4);

	if(err != KErrNotReady)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::StartRemainingChargingTimeWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Step state: StartRemainingChargingTimeWithPluginErrorL"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	TInt batteryError = KErrGeneral;
	
	ExpectLeave(KErrGeneral);
	ExpectBatteryChargingTimeMeasurementsErrorL(batteryError);

	RMockPower& mockPower = GetMockPower();

	TBuf8<1> emptyDes;
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EStartBatteryFullChargingTimeReportingCmdId, emptyDes);

	HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponsePckg(batteryError);
	mockPower.CompleteL(HWRMPowerCommand::EStartBatteryFullChargingTimeReportingCmdId, 2, batteryErrorResponsePckg);
	
	
	TRequestStatus status;	
	User::LeaveIfError(power.NotifyBatteryFullChargingTime(status));
	
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::StartChargingCurrentWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Step state: StartChargingCurrentWithPluginErrorL"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	TInt batteryError = KErrGeneral;
	
	ExpectLeave(KErrGeneral);
	ExpectBatteryChargingCurrentMeasurementsErrorL(batteryError);

	RMockPower& mockPower = GetMockPower();

	HWRMPowerCommand::TChrgCurrentIntervalMultiplePackage interval(2);
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EStartAverageBatteryChargingCurrentReportingCmdId, interval);

	HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponse;
	

	HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponsePckg(batteryError);
	mockPower.CompleteL(HWRMPowerCommand::EStartAverageBatteryChargingCurrentReportingCmdId, 2, batteryErrorResponsePckg);
	
	
	TRequestStatus status;	
	User::LeaveIfError(power.NotifyAverageChargingCurrent(status, 2));
	
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::StopRemainingChargingTimeWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Step state: StopRemainingChargingTimeWithPluginErrorL"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	//the client will not receive any error. It's session data has been removed and any results have been sent.
	RMockPower& mockPower = GetMockPower();

	TBuf8<1> emptyDes;
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EStopBatteryFullChargingTimeReportingCmdId, emptyDes);
	
	HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponse;

	HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponsePckg(KErrGeneral);
	mockPower.CompleteL(HWRMPowerCommand::EStopBatteryFullChargingTimeReportingCmdId, 2, batteryErrorResponsePckg);
	
	power.StopChargingNotificationL(CHWRMPower::EChargingNotifierBatteryFullChargingTime);
	}

void CHWRMTest::StopChargingCurrentWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Step state: StopChargingCurrentWithPluginErrorL"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	//the client will not receive any error. It's session data has been removed and any results have been sent.
	RMockPower& mockPower = GetMockPower();

	TBuf8<1> emptyDes;
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EStopAverageBatteryChargingCurrentReportingCmdId, emptyDes);
	
	HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponse;

	HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponsePckg(KErrGeneral);
	mockPower.CompleteL(HWRMPowerCommand::EStopAverageBatteryChargingCurrentReportingCmdId, 2, batteryErrorResponsePckg);
	
	power.StopChargingNotificationL(CHWRMPower::EChargingNotifierChargingCurrent);
	}

void CHWRMTest::StartRemainingChargingTimeWhenStartedL()
	{
	INFO_PRINTF1(_L("Step state: StartRemainingChargingTimeWhenStartedL"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	ExpectLeave(KErrAlreadyExists);
	
	TRequestStatus status;

	User::LeaveIfError(power.NotifyBatteryFullChargingTime(status ));
	
	User::WaitForRequest(status);	
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::StartChargingCurrentWhenStartedL()
	{
	INFO_PRINTF1(_L("Step state: StartChargingCurrentWhenStartedL"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	ExpectLeave(KErrAlreadyExists);
	
	TRequestStatus status;

	User::LeaveIfError(power.NotifyAverageChargingCurrent(status, 4));
	
	User::WaitForRequest(status);	
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::ChargingTimeNotificationWithErrorCompleteL()
	{
	INFO_PRINTF1(_L("Step state: ChargingTimeNotificationWithErrorCompleteL"));
	ExpectBatteryChargingTimeMeasurementsErrorL(KErrCancel);
	PowerChargingStatusCompleteL();
	}

void CHWRMTest::ChargingCurrentNotificationWithChargingCompleteL()
	{
	INFO_PRINTF1(_L("Step state: ChargingCurrentNotificationWithChargingCompleteL"));
	// No error expected in this case
	PowerChargingStatusCompleteL();
	}

void CHWRMTest::ChargingTimeNotificationWithErrorDisconnectedL()
	{
	INFO_PRINTF1(_L("Step state: ChargingTimeNotificationWithErrorDisconnectedL"));
	ExpectBatteryChargingTimeMeasurementsErrorL(KErrCancel);
	PowerChargingStatusNotConnectedL();
	}

void CHWRMTest::ChargingCurrentNotificationWithErrorDisconnectedL()
	{
	INFO_PRINTF1(_L("Step state: ChargingCurrentNotificationWithErrorDisconnectedL"));
	ExpectBatteryChargingCurrentMeasurementsErrorL(KErrCancel);
	PowerChargingStatusNotConnectedL();
	}

void CHWRMTest::ChargingTimeNotificationWithErrorComplete3L()
	{
	INFO_PRINTF1(_L("Step state: ChargingTimeNotificationWithErrorComplete3L"));
	ExpectBatteryChargingTimeMeasurementsErrorL(KErrCancel);
	ExpectBatteryChargingTimeMeasurementsErrorL(KErrCancel);
	ExpectBatteryChargingTimeMeasurementsErrorL(KErrCancel);
	PowerChargingStatusCompleteL();
	}

void CHWRMTest::ChargingTimeNotificationWithErrorDisconnected3L()
	{
	INFO_PRINTF1(_L("Step state: ChargingTimeNotificationWithErrorDisconnected3L"));
	ExpectBatteryChargingTimeMeasurementsErrorL(KErrCancel);
	ExpectBatteryChargingTimeMeasurementsErrorL(KErrCancel);
	ExpectBatteryChargingTimeMeasurementsErrorL(KErrCancel);
	PowerChargingStatusNotConnectedL();
	}

void CHWRMTest::ChargingCurrentNotificationWithErrorDisconnected3L()
	{
	INFO_PRINTF1(_L("Step state: ChargingCurrentNotificationWithErrorDisconnected3L"));
	ExpectBatteryChargingCurrentMeasurementsErrorL(KErrCancel);
	ExpectBatteryChargingCurrentMeasurementsErrorL(KErrCancel);
	ExpectBatteryChargingCurrentMeasurementsErrorL(KErrCancel);
	PowerChargingStatusNotConnectedL();
	}

void CHWRMTest::GetBatteryChargerTypeNoCharger()
	{
	INFO_PRINTF1(_L("Step state: GetBatteryChargerTypeNoCharger"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	RMockPower& mockPower = GetMockPower();

	TBuf8<1> emptyDes;
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, emptyDes);

	HWRMPowerCommand::TBatteryChargerTypeData responsedata;
	responsedata.iErrorCode = KErrNone;
	responsedata.iChargerType = HWRMPowerCommand::EBatteryChargerUnKnown;

	HWRMPowerCommand::TBatteryChargerTypeResponsePackage response(responsedata);
	mockPower.CompleteL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, 2, response);
	
	CHWRMPower::TBatteryChargerType data;
	User::LeaveIfError(power.GetBatteryChargerType(data));

	if( data != CHWRMPower::EBatteryChargerUnKnown )
		{
		User::Leave(KErrGeneral);
		}
	}

static TInt GetChargerTypeL(TInt aType)
	{
	TInt err(KErrNone), stat(KErrNone);
	CHWRMPower::TBatteryChargerType data;
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler();
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	CHWRMPower* power = CHWRMPower::NewL();
	
	err = power->GetBatteryChargerType(data);
	if( (err != KErrNone) || (data != (CHWRMPower::TBatteryChargerType)aType) )
		{
		stat = KErrGeneral;
		}
	
	delete power;
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(sched);

	return stat;
	}

static TInt ClientThreadFunction(TAny* aParam)
	{
	TInt stat(KErrNone);
	TInt expresult = *((TInt*)aParam);
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	
	TRAPD(error, (stat = GetChargerTypeL(expresult)));
	if(error != KErrNone)
		{
		stat = KErrGeneral;
		}
	
	delete cleanup;
	return stat;
	}

void CHWRMTest::TestBatteryChargerTypeMultiClientL(TInt aType)
	{
	RThread r0, r1, r2;
	TInt err(KErrNone);
	TRequestStatus s0, s1, s2;

	INFO_PRINTF1(_L("Step state: TestBatteryChargerTypeMultiClientL"));
	
	err = r0.Create(KNullDesC, ClientThreadFunction, KDefaultStackSize, NULL, (TAny*)&aType);
	if(err != KErrNone) 
		{
		User::Leave(KErrGeneral);
		}
	r0.Logon(s0);
	
	err = r1.Create(KNullDesC, ClientThreadFunction, KDefaultStackSize, NULL, (TAny*)&aType);
	if(err != KErrNone) 
		{
		User::Leave(KErrGeneral);
		}
	r1.Logon(s1);
	
	err = r2.Create(KNullDesC, ClientThreadFunction, KDefaultStackSize, NULL, (TAny*)&aType);
	if(err != KErrNone) 
		{
		User::Leave(KErrGeneral);
		}
	r2.Logon(s2);
	
	RMockPower& mockPower = GetMockPower();

	TBuf8<1> emptyDes;
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, emptyDes);

	HWRMPowerCommand::TBatteryChargerTypeData responsedata;
	responsedata.iErrorCode = KErrNone;
	responsedata.iChargerType = (HWRMPowerCommand::TBatteryChargerType)aType;

	HWRMPowerCommand::TBatteryChargerTypeResponsePackage response(responsedata);
	mockPower.CompleteL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, 5, response);

	r0.Resume();
	r1.Resume();
	r2.Resume();
	
	User::WaitForRequest(s0);
	User::WaitForRequest(s1);
	User::WaitForRequest(s2);

	r0.Close();
	r1.Close();
	r2.Close();

	User::LeaveIfError(s0.Int());
	User::LeaveIfError(s1.Int());
	User::LeaveIfError(s2.Int());
	}

void CHWRMTest::TestPowerClientChargingCurrentSetCallBackL()
    {
    INFO_PRINTF1(_L("Step state: TestPowerClientChargingCurrentSetCallBackL"));
    CHWRMPower& power0 = GetPowerSessionL(0);

    //Call the API twice
    power0.SetBatteryChargingObserver(NULL,this,NULL);
    power0.SetBatteryChargingObserver(NULL,this,NULL);
    
    }

void CHWRMTest::TestPowerClientRemainingChargingTimeSetCallBackL()
    {
    INFO_PRINTF1(_L("Step state: TestPowerClientRemainingChargingTimeSetCallBackL"));
    CHWRMPower& power0 = GetPowerSessionL(0);

    //Call the API twice
    power0.SetBatteryChargingObserver(NULL,NULL,this);
    power0.SetBatteryChargingObserver(NULL,NULL,this);
    
    }

void CHWRMTest::TestSetCallBackLeavingL()
    {
    INFO_PRINTF1(_L("Step state: TestSetCallBackLeavingL"));
    CHWRMPower& power1 = GetPowerSessionL(1);

    __UHEAP_SETFAIL(RHeap::EDeterministic, 1);
    ExpectLeave(KErrNoMemory);
    TInt err = power1.SetBatteryChargingObserver(NULL,this,NULL);
    __UHEAP_RESET;
    User::LeaveIfError(err);
    }

void CHWRMTest::ChargingCurrentMaxPeriodReachedMCL()
    {
    INFO_PRINTF1(_L("Step state: ChargingCurrentMaxPeriodReachedMCL"));
    ExpectBatteryChargingCurrentMeasurementsErrorL(KErrTimedOut); //For Client1
    ExpectBatteryChargingCurrentMeasurementsErrorL(KErrTimedOut); //For Client2
    ExpectBatteryChargingCurrentMeasurementsErrorL(KErrTimedOut); //For Client3
    }


void CHWRMTest::GetBatteryChargerTypeNoChargerMulticlientL()
	{
	INFO_PRINTF1(_L("Step state: GetBatteryChargerTypeNoChargerMulticlientL"));
	TestBatteryChargerTypeMultiClientL((TInt)CHWRMPower::EBatteryChargerUnKnown);
	}

void CHWRMTest::GetBatteryChargerTypeACMain()
	{
	INFO_PRINTF1(_L("Step state: GetBatteryChargerTypeACMain"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	RMockPower& mockPower = GetMockPower();

	TBuf8<1> emptyDes;
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, emptyDes);

	HWRMPowerCommand::TBatteryChargerTypeData responsedata;
	responsedata.iErrorCode = KErrNone;
	responsedata.iChargerType = HWRMPowerCommand::EBatteryChargerDedicated;

	HWRMPowerCommand::TBatteryChargerTypeResponsePackage response(responsedata);
	mockPower.CompleteL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, 2, response);
	
	CHWRMPower::TBatteryChargerType data;
	User::LeaveIfError(power.GetBatteryChargerType(data));

	if( data != CHWRMPower::EBatteryChargerDedicated )
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::GetBatteryChargerTypeACMainMulticlientL()
	{
	INFO_PRINTF1(_L("Step state: GetBatteryChargerTypeACMainMulticlientL"));
	TestBatteryChargerTypeMultiClientL((TInt)CHWRMPower::EBatteryChargerDedicated);
	}

void CHWRMTest::GetBatteryChargerTypeUSB()
	{
	INFO_PRINTF1(_L("Step state: GetBatteryChargerTypeUSB"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	RMockPower& mockPower = GetMockPower();

	TBuf8<1> emptyDes;
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, emptyDes);

	HWRMPowerCommand::TBatteryChargerTypeData responsedata;
	responsedata.iErrorCode = KErrNone;
	responsedata.iChargerType = HWRMPowerCommand::EBatteryChargerUsbDedicated;

	HWRMPowerCommand::TBatteryChargerTypeResponsePackage response(responsedata);
	mockPower.CompleteL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, 2, response);
	
	CHWRMPower::TBatteryChargerType data;
	User::LeaveIfError(power.GetBatteryChargerType(data));

	if( data != CHWRMPower::EBatteryChargerUsbDedicated )
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::GetBatteryChargerTypeUSBMulticlientL()
	{
	INFO_PRINTF1(_L("Step state: GetBatteryChargerTypeUSBMulticlientL"));
	TestBatteryChargerTypeMultiClientL((TInt)CHWRMPower::EBatteryChargerUsbDedicated);
	}

void CHWRMTest::GetBatteryChargerTypeUSBHispeedHost()
    {
    INFO_PRINTF1(_L("Step state: GetBatteryChargerTypeUSBHispeedHost"));
    
    CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
    
    RMockPower& mockPower = GetMockPower();

    TBuf8<1> emptyDes;
    mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, emptyDes);

    HWRMPowerCommand::TBatteryChargerTypeData responsedata;
    responsedata.iErrorCode = KErrNone;
    responsedata.iChargerType = HWRMPowerCommand::EBatteryChargerUsbHispeedHost;

    HWRMPowerCommand::TBatteryChargerTypeResponsePackage response(responsedata);
    mockPower.CompleteL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, 2, response);
    
    CHWRMPower::TBatteryChargerType data;
    User::LeaveIfError(power.GetBatteryChargerType(data));

    if( data != CHWRMPower::EBatteryChargerUsbHispeedHost )
        {
        User::Leave(KErrGeneral);
        }
    }

void CHWRMTest::GetBatteryChargerTypeUSBHispeedHostMulticlientL()
    {
    INFO_PRINTF1(_L("Step state: GetBatteryChargerTypeUSBHispeedHostMulticlientL"));
    TestBatteryChargerTypeMultiClientL((TInt)CHWRMPower::EBatteryChargerUsbHispeedHost);
    }

void CHWRMTest::GetBatteryChargerTypeUSBAca()
    {
    INFO_PRINTF1(_L("Step state: GetBatteryChargerTypeUSBAca"));
    
    CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
    
    RMockPower& mockPower = GetMockPower();

    TBuf8<1> emptyDes;
    mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, emptyDes);

    HWRMPowerCommand::TBatteryChargerTypeData responsedata;
    responsedata.iErrorCode = KErrNone;
    responsedata.iChargerType = HWRMPowerCommand::EBatteryChargerUsbAca;

    HWRMPowerCommand::TBatteryChargerTypeResponsePackage response(responsedata);
    mockPower.CompleteL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, 2, response);
    
    CHWRMPower::TBatteryChargerType data;
    User::LeaveIfError(power.GetBatteryChargerType(data));

    if( data != CHWRMPower::EBatteryChargerUsbAca )
        {
        User::Leave(KErrGeneral);
        }
    }

void CHWRMTest::GetBatteryChargerTypeUSBAcaMulticlientL()
    {
    INFO_PRINTF1(_L("Step state: GetBatteryChargerTypeUSBAcaMulticlientL"));
    TestBatteryChargerTypeMultiClientL((TInt)CHWRMPower::EBatteryChargerUsbAca);
    }

void CHWRMTest::GetBatteryChargerTypeWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Step state: GetBatteryChargerTypeWithPluginErrorL"));
	
	CHWRMPower& power0 = GetPowerSessionWithCallbackRegisteredL();
	
	RMockPower& mockPower = GetMockPower();

	TBuf8<1> emptyDes;
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, emptyDes);

	HWRMPowerCommand::TBatteryChargerTypeData responsedata;
	responsedata.iErrorCode = KErrGeneral;
	responsedata.iChargerType = HWRMPowerCommand::EBatteryChargerUsbDedicated;

	HWRMPowerCommand::TBatteryChargerTypeResponsePackage response(responsedata);
	mockPower.CompleteL(HWRMPowerCommand::EGetBatteryChargerTypeCmdId, 2, response);
	
	CHWRMPower::TBatteryChargerType data0;
	TInt err = power0.GetBatteryChargerType(data0);

	if( err != KErrGeneral )
		{
		User::Leave(err);
		}
	}

void CHWRMTest::ChargingCurrentMaxPeriodReachedL()
	{
	INFO_PRINTF1(_L("Step state: ChargingCurrentMaxPeriodReachedL"));
	ExpectBatteryChargingCurrentMeasurementsErrorL(KErrTimedOut);
	}

void CHWRMTest::StartChargingCurrentSameIntervalL()
	{
	INFO_PRINTF1(_L("Step state: StartChargingCurrentSameIntervalL"));
	CHWRMPower& power0 = GetPowerSessionL(0);
	CHWRMPower& power1 = GetPowerSessionL(1);
	CHWRMPower& power2 = GetPowerSessionL(2);
	TRequestStatus status0, status1, status2;

	User::LeaveIfError(power0.NotifyAverageChargingCurrent(status0, 2));
	User::WaitForRequest(status0);
	User::LeaveIfError(status0.Int());
	
	User::LeaveIfError(power1.NotifyAverageChargingCurrent(status1, 2));
	User::WaitForRequest(status1);
	User::LeaveIfError(status1.Int());
	
	User::LeaveIfError(power2.NotifyAverageChargingCurrent(status2, 2));
	User::WaitForRequest(status2);
	User::LeaveIfError(status2.Int());
	}

void CHWRMTest::StartChargingCurrentDifferentIntervalL()
	{
	INFO_PRINTF1(_L("Step state: StartChargingCurrentDifferentIntervalL"));
	CHWRMPower& power0 = GetPowerSessionL(0);
	CHWRMPower& power1 = GetPowerSessionL(1);
	CHWRMPower& power2 = GetPowerSessionL(2);
	TRequestStatus status0, status1, status2;

	User::LeaveIfError(power0.NotifyAverageChargingCurrent(status0, 2));
	User::WaitForRequest(status0);
	User::LeaveIfError(status0.Int());
	
	User::LeaveIfError(power1.NotifyAverageChargingCurrent(status1, 4));
	User::WaitForRequest(status1);
	User::LeaveIfError(status1.Int());
	
	User::LeaveIfError(power2.NotifyAverageChargingCurrent(status2, 6));
	User::WaitForRequest(status2);
	User::LeaveIfError(status2.Int());
	}

void CHWRMTest::StopChargingCurrentAllL()
	{
	INFO_PRINTF1(_L("Step state: StopChargingCurrentAllL"));
	CHWRMPower& power0 = GetPowerSessionL(0);
	CHWRMPower& power1 = GetPowerSessionL(1);
	CHWRMPower& power2 = GetPowerSessionL(2);

	power0.StopChargingNotificationL(CHWRMPower::EChargingNotifierChargingCurrent);
	power1.StopChargingNotificationL(CHWRMPower::EChargingNotifierChargingCurrent);
	power2.StopChargingNotificationL(CHWRMPower::EChargingNotifierChargingCurrent);
	}

void CHWRMTest::SetChargingCurrentMeasurementTime200L()
	{
	INFO_PRINTF1(_L("Step state: SetChargingCurrentMeasurementTime200L"));
	SetPowerChargingCurrentMeasurementProcessingTime(200);
	}

void CHWRMTest::TestPowerGetBatteryChargingStatusL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBatteryChargingStatusL"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusErrorL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotConnectedL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotChargingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusAlmostCompleteL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusCompleteL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusContinuedL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusL, KOneTimeUnit);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerGetBatteryChargingStatusMultiClient3L()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBatteryChargingStatusMultiClient3L"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusErrorL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotConnectedL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotChargingL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusAlmostCompleteL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusCompleteL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusContinuedL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::CheckPowerChargingStatusMultiClient3L, KOneTimeUnit);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingStatusNotificationL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingStatusNotificationL"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingStatusCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusErrorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusIllegalErrorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusUnderVoltageErrorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusOverVoltageErrorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusTooHotErrorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusTooColdErrorL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotConnectedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotChargingL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusAlmostCompleteL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusCompleteL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusContinuedL, KTwoTimeUnits);

	// Notification only if a change
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusCompleteL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetChargingStatusCBFlagL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusCompleteL, KTwoTimeUnits); // No change so notification
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingStatusNotificationMultiClient3L()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingStatusNotificationMultiClient3L"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingStatusCallbackMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusError3L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusIllegalError3L, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::PowerChargingStatusUnderVoltageError3L, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::PowerChargingStatusOverVoltageError3L, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::PowerChargingStatusTooHotError3L, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::PowerChargingStatusTooColdError3L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotConnected3L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusCharging3L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotCharging3L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusAlmostComplete3L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusComplete3L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusContinued3L, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerRemainingChargingTimeNotificationL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerRemainingChargingTimeNotificationL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusAlmostCompleteL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusCompleteL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusContinuedL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerRemainingChargingTimeWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerRemainingChargingTimeWithPluginErrorL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeWithPluginErrorL, 4*KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerStopRemainingChargingTimeWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerStopRemainingChargingTimeWithPluginErrorL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StopRemainingChargingTimeWithPluginErrorL, 4*KTwoTimeUnits);	

	ExecuteTestL();
	}

void CHWRMTest::TestPowerRemainingChargingTimeNotificationErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerStopRemainingChargingTimeWithPluginErrorL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackL, KOneTimeUnit);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotConnectedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeErrorL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotChargingL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeErrorL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusErrorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeErrorL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusIllegalErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeErrorL, KTwoTimeUnits);
        
    AddTestStateL(this, &CHWRMTest::PowerChargingStatusOverVoltageErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeErrorL, KTwoTimeUnits);
    
    AddTestStateL(this, &CHWRMTest::PowerChargingStatusUnderVoltageErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeErrorL, KTwoTimeUnits);
    
    AddTestStateL(this, &CHWRMTest::PowerChargingStatusTooHotErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeErrorL, KTwoTimeUnits);
        
    AddTestStateL(this, &CHWRMTest::PowerChargingStatusTooColdErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeErrorL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerRemainingChargingTimeWhenStartedL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerRemainingChargingTimeWhenStartedL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeWhenStartedL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerRemainingChargingTimeNotificationMulticlient3L()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerRemainingChargingTimeNotificationMulticlient3L"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime1L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime2L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTime1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTime2L, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusAlmostCompleteL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime1L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime2L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTime1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTime2L, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusCompleteL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime1L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime2L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTime1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTime2L, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusContinuedL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime1L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime2L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTime1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTime2L, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingTimeNotificationWhenChargingCompleteL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingTimeNotificationWhenChargingCompleteL"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 4*KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::ChargingTimeNotificationWithErrorCompleteL, 2*KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingTimeNotificationWhenDisconnectedL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingTimeNotificationWhenDisconnectedL"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 4*KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::ChargingTimeNotificationWithErrorDisconnectedL, 2*KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingTimeNotificationWhenChargingComplete3L()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingTimeNotificationWhenChargingComplete3L"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime1L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime2L, 2*KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::ChargingTimeNotificationWithErrorComplete3L, 2*KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingTimeNotificationWhenDisconnected3L()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingTimeNotificationWhenDisconnected3L"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime1L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTime2L, 2*KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::ChargingTimeNotificationWithErrorDisconnected3L, 2*KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusAlmostCompleteL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusCompleteL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusContinuedL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationErrorL"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackL, KOneTimeUnit);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotConnectedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentErrorL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusNotChargingL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentErrorL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusErrorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentErrorL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusIllegalErrorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentErrorL, KTwoTimeUnits);
	    
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusOverVoltageErrorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentErrorL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusUnderVoltageErrorL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentErrorL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusTooHotErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::StartChargingCurrentErrorL, KTwoTimeUnits);
	    
    AddTestStateL(this, &CHWRMTest::PowerChargingStatusTooColdErrorL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::StartChargingCurrentErrorL, KTwoTimeUnits);
	        
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentWhenStartedL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentWhenStartedL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentWhenStartedL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationMulticlient3L()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationMulticlient3L"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrent1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrent2L, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusAlmostCompleteL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrent1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrent2L, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusCompleteL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrent1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrent2L, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::PowerChargingStatusContinuedL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrent1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrent2L, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationWhenChargingCompleteL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationWhenChargingCompleteL"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 4*KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::ChargingCurrentNotificationWithChargingCompleteL, 2*KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationWhenDisconnectedL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationWhenDisconnectedL"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 4*KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::ChargingCurrentNotificationWithErrorDisconnectedL, 2*KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationWhenChargingComplete3L()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationWhenChargingComplete3L"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent1L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent2L, 2*KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::ChargingCurrentNotificationWithChargingCompleteL, 2*KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationWhenDisconnected3L()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationWhenDisconnected3L"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent1L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent2L, 2*KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::ChargingCurrentNotificationWithErrorDisconnected3L, 2*KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentWithPluginErrorL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentWithPluginErrorL, 4*KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerStopChargingCurrentWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerStopChargingCurrentWithPluginErrorL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StopChargingCurrentWithPluginErrorL, 4*KTwoTimeUnits);	

	ExecuteTestL();
	}

void CHWRMTest::TestPowerGetBatteryChargerType()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBatteryChargerType"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::GetBatteryChargerTypeNoCharger, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GetBatteryChargerTypeACMain, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GetBatteryChargerTypeUSB, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GetBatteryChargerTypeUSBHispeedHost, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GetBatteryChargerTypeUSBAca, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerGetBatteryChargerTypeMulticlientL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBatteryChargerTypeMulticlientL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	
	AddTestStateL(this, &CHWRMTest::GetBatteryChargerTypeNoChargerMulticlientL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GetBatteryChargerTypeACMainMulticlientL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GetBatteryChargerTypeUSBMulticlientL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GetBatteryChargerTypeUSBHispeedHostMulticlientL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GetBatteryChargerTypeUSBAcaMulticlientL, 2*KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerGetBatteryChargerTypeWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerGetBatteryChargerType"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GetBatteryChargerTypeWithPluginErrorL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerCancelChargingTimeWithoutStart()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerCancelChargingTimeWithoutStart"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerCancelChargingCurrentWithoutStart()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerCancelChargingCurrentWithoutStart"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationMaxPeriodReachedL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationMaxPeriodReachedL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent4L, 5*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ChargingCurrentMaxPeriodReachedL, 6*KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationSameIntervalL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationSameIntervalL"));

	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentSameIntervalL, 5*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StopChargingCurrentAllL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationDifferentIntervalL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationDifferentIntervalL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackMultiClient3L, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentDifferentIntervalL, 5*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StopChargingCurrentAllL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentandRemainingChargingTimeNotificationL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentandRemainingChargingTimeNotificationL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingandVoltageNotificationL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingandVoltageNotificationL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::RegisterChargingTimeCallbackL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationSlowClientL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationSlowClientL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::SetChargingCurrentMeasurementTime200L, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationSlowClientsL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationSlowClientL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackMultiClient3L, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::SetChargingCurrentMeasurementTime200L, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent1L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrent2L, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrent1L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelChargingCurrent2L, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingTimeNotificationWithoutCallbackL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingTimeNotificationWithoutCallbackL"));
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeNoCallbackL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerChargingCurrentNotificationWithoutCallbackL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationWithoutCallbackL"));
	AddTestStateL(this, &CHWRMTest::PowerAndPowerStateOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartChargingCurrentNoCallbackL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestPowerClientSetCallBackL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerClientSetCallBackL"));
	
	AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
    
	AddTestStateL(this, &CHWRMTest::TestPowerClientChargingCurrentSetCallBackL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::StartChargingCurrentL, 4*KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::CancelChargingCurrentL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::TestPowerClientRemainingChargingTimeSetCallBackL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::StartRemainingChargingTimeL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::CancelRemainingChargingTimeL, KTwoTimeUnits);

    AddTestStateL(this, &CHWRMTest::TestSetCallBackLeavingL, KTwoTimeUnits);

	ExecuteTestL();
	}
void CHWRMTest::TestPowerChargingCurrentNotificationMaxPeriodReachedMCL()
    {
    INFO_PRINTF1(_L("Start test step: TestPowerChargingCurrentNotificationMaxPeriodReachedMCL"));

    AddTestStateL(this, &CHWRMTest::PowerAndPowerState3SessionsOpenL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::RegisterChargingCurrentCallbackMultiClient3L, KOneTimeUnit);

    AddTestStateL(this, &CHWRMTest::PowerChargingStatusChargingL, KTwoTimeUnits);

    AddTestStateL(this, &CHWRMTest::StartChargingCurrentDifferentIntervalL, 5*KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ChargingCurrentMaxPeriodReachedMCL, 8*KTwoTimeUnits);

    ExecuteTestL();
    }


#endif //SYMBIAN_HWRM_EXTPOWERINFO

void CHWRMTest::RegisterPowerCallbackL()
	{
	INFO_PRINTF1(_L("Step state: RegisterPowerCallbackL"));
	CHWRMPower& pwr = GetPowerSessionL(0);

	pwr.SetPowerReportObserver(this);
	}

void CHWRMTest::RegisterNullPowerCallbackL()
	{
	INFO_PRINTF1(_L("Step state: RegisterNullPowerCallbackL"));
	CHWRMPower& pwr = GetPowerSessionL(0);

	pwr.SetPowerReportObserver(NULL);
	}


void CHWRMTest::PowerNotifyL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotifyL"));
	TRequestStatus stat;
	CHWRMPower& pwr = GetPowerSessionL(0);
	
	User::LeaveIfError(pwr.StartAveragePowerReporting(stat, 4));
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	}

void CHWRMTest::PowerNotify_NoCallbackL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_NoCallbackL"));
	ExpectLeave(KErrBadHandle);
	PowerNotifyL();
	}

void CHWRMTest::PowerNotify_CancelL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_CancelL"));
	CHWRMPower& pwr = GetPowerSessionL(0);
	pwr.StopAveragePowerReportingL();
	}

void CHWRMTest::PowerNotify_Cancel_RedundantL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_CancelL"));
	PowerNotify_CancelL();
	}

void CHWRMTest::SetReportingPeriod30L()
	{
	INFO_PRINTF1(_L("Step state: SetReportingPeriod30L"));
	SetCRPowerAttributeL(KPowerMaxReportingPeriod, 30000);
	}
void CHWRMTest::SetReportingPeriod5L()
	{
	INFO_PRINTF1(_L("Step state: SetReportingPeriod5L"));
	SetCRPowerAttributeL(KPowerMaxReportingPeriod, 5000);
	}
void CHWRMTest::SetReportingPeriod10L()
	{
	INFO_PRINTF1(_L("Step state: SetReportingPeriod10L"));
	SetCRPowerAttributeL(KPowerMaxReportingPeriod, 10000);
	}
void CHWRMTest::SetReportingPeriodZeroL()
	{
	INFO_PRINTF1(_L("Step state: SetReportingPeriodZeroL"));
	SetCRPowerAttributeL(KPowerMaxReportingPeriod, 0);
	}
void CHWRMTest::SetReportingPeriod_Power_NegativeL()
	{
	INFO_PRINTF1(_L("Step state: SetReportingPeriod_Power_NegativeL"));
    ExpectBatteryPowerMeasurementsErrorL(KErrGeneral);
	SetCRPowerAttributeL(KPowerMaxReportingPeriod, -1);
	}

void CHWRMTest::SetReportingPeriod10L_TimeoutL()
	{
	INFO_PRINTF1(_L("Step state: SetReportingPeriod10L_TimeoutL"));
	SetCRPowerAttributeL(KPowerMaxReportingPeriod, 10000);
	ExpectBatteryPowerMeasurementsErrorL(KErrTimedOut);
	}
	
void CHWRMTest::PowerNotify_Continue_NoTimeoutL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_Continue_NoTimeoutL"));
	ExpectBatteryPowerMeasurementsErrorL(KErrNone);
	}

void CHWRMTest::PowerNotify_Continue_TimeoutL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_Continue_TimeoutL"));
	ExpectBatteryPowerMeasurementsErrorL(KErrTimedOut);
	}

void CHWRMTest::ResetReportingPeriodL()
	{
	INFO_PRINTF1(_L("Step state: ResetReportingPeriodL"));
	SetCRPowerAttributeL(KPowerMaxReportingPeriod, KMaxSampleReportingPeriod);
	}

void CHWRMTest::PowerNotify_Interval20L()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_Interval20L"));
	TRequestStatus stat;
	CHWRMPower& pwr = GetPowerSessionL(0);
	
	User::LeaveIfError(pwr.StartAveragePowerReporting(stat, 20));
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());	
	}
void CHWRMTest::PowerNotify_Interval50L()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_Interval50L"));
	TRequestStatus stat;
	CHWRMPower& pwr = GetPowerSessionL(0);
	
	User::LeaveIfError(pwr.StartAveragePowerReporting(stat, 50));
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());	
	}

void CHWRMTest::PowerNotify_Interval60L()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_Interval60L"));
	TRequestStatus stat;
	CHWRMPower& pwr = GetPowerSessionL(0);
	
	User::LeaveIfError(pwr.StartAveragePowerReporting(stat, 60));
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());	
	}

void CHWRMTest::PowerNotify_Interval1L()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_Interval1L"));
	TRequestStatus stat;
	CHWRMPower& pwr = GetPowerSessionL(0);
	
	User::LeaveIfError(pwr.StartAveragePowerReporting(stat, 1));
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());	
	}


void CHWRMTest::RegisterPowerCallbackMC3L()
	{
	INFO_PRINTF1(_L("Step state: RegisterPowerCallbackMC3L"));
	CHWRMPower& pwr0 = GetPowerSessionL(0);
	CHWRMPower& pwr1 = GetPowerSessionL(1);
	CHWRMPower& pwr2 = GetPowerSessionL(2);

	pwr0.SetPowerReportObserver(this);
	pwr1.SetPowerReportObserver(this);
	pwr2.SetPowerReportObserver(this);
	}

void CHWRMTest::PowerNotifyMC3_SameIntervalL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotifyMC3_SameIntervalL"));
	TRequestStatus stat0, stat1, stat2;
	CHWRMPower& pwr0 = GetPowerSessionL(0);
	CHWRMPower& pwr1 = GetPowerSessionL(1);
	CHWRMPower& pwr2 = GetPowerSessionL(2);
	
	User::LeaveIfError(pwr0.StartAveragePowerReporting(stat0, 5));
	User::WaitForRequest(stat0);
	User::LeaveIfError(stat0.Int());
	User::LeaveIfError(pwr1.StartAveragePowerReporting(stat1, 5));
	User::WaitForRequest(stat1);
	User::LeaveIfError(stat1.Int());
	User::LeaveIfError(pwr2.StartAveragePowerReporting(stat2, 5));
	User::WaitForRequest(stat2);
	User::LeaveIfError(stat2.Int());
	}

void CHWRMTest::PowerNotify_CancelMC3L()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_CancelMC3L"));
	CHWRMPower& pwr0 = GetPowerSessionL(0);
	CHWRMPower& pwr1 = GetPowerSessionL(1);
	CHWRMPower& pwr2 = GetPowerSessionL(2);
	pwr0.StopAveragePowerReportingL();
	pwr1.StopAveragePowerReportingL();
	pwr2.StopAveragePowerReportingL();
	}

void CHWRMTest::PowerNotifyMC3_DiffIntervalL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotifyMC3_DiffIntervalL"));
	TRequestStatus stat0, stat1, stat2;
	CHWRMPower& pwr0 = GetPowerSessionL(0);
	CHWRMPower& pwr1 = GetPowerSessionL(1);
	CHWRMPower& pwr2 = GetPowerSessionL(2);
	
	User::LeaveIfError(pwr0.StartAveragePowerReporting(stat0, 5));
	User::WaitForRequest(stat0);
	User::LeaveIfError(stat0.Int());
	User::LeaveIfError(pwr1.StartAveragePowerReporting(stat1, 7));
	User::WaitForRequest(stat1);
	User::LeaveIfError(stat1.Int());
	User::LeaveIfError(pwr2.StartAveragePowerReporting(stat2, 12));
	User::WaitForRequest(stat2);
	User::LeaveIfError(stat2.Int());
	}

void CHWRMTest::RegisterPowerCallbackMC2L()
	{
	INFO_PRINTF1(_L("Step state: RegisterPowerCallbackMC2L"));
	CHWRMPower& pwr0 = GetPowerSessionL(0);
	CHWRMPower& pwr1 = GetPowerSessionL(1);

	pwr0.SetPowerReportObserver(this);
	pwr1.SetPowerReportObserver(this);
	}

void CHWRMTest::PowerNotifyMC2_DiffInterval_SpecialCaseL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotifyMC2_DiffInterval_SpecialCaseL"));
	TRequestStatus stat0, stat1;
	CHWRMPower& pwr0 = GetPowerSessionL(0);
	CHWRMPower& pwr1 = GetPowerSessionL(1);
	
	User::LeaveIfError(pwr0.StartAveragePowerReporting(stat0, 5));
	User::WaitForRequest(stat0);
	User::LeaveIfError(stat0.Int());
	User::LeaveIfError(pwr1.StartAveragePowerReporting(stat1, 100));
	User::WaitForRequest(stat1);
	User::LeaveIfError(stat1.Int());
	}
void CHWRMTest::PowerNotify_CancelMC2L()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_CancelMC2L"));
	CHWRMPower& pwr0 = GetPowerSessionL(0);
	CHWRMPower& pwr1 = GetPowerSessionL(1);
	pwr0.StopAveragePowerReportingL();
	pwr1.StopAveragePowerReportingL();
	}


void CHWRMTest::PowerNotify_RedundantL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_RedundantL"));
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	ExpectLeave(KErrAlreadyExists);
	
	TRequestStatus status;

	User::LeaveIfError(power.StartAveragePowerReporting(status, 4));
	
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}
void CHWRMTest::PowerNotify_Start_PluginErrorL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_Start_PluginErrorL"));
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	TInt batteryError = KErrGeneral;
	
	ExpectLeave(KErrGeneral);
	ExpectBatteryPowerMeasurementsErrorL(batteryError);

	RMockPower& mockPower = GetMockPower();

	HWRMPowerCommand::TPowerIntervalMultiplePackage interval(6);
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EStartAverageBatteryPowerReportingCmdId, interval);

	HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponse;
	

	HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponsePckg(batteryError);
	mockPower.CompleteL(HWRMPowerCommand::EStartAverageBatteryPowerReportingCmdId, 2, batteryErrorResponsePckg);
	
	
	TRequestStatus status;	
	User::LeaveIfError(power.StartAveragePowerReporting(status, 6));
	
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::PowerNotify_Start_Set_PluginErrorL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_Start_Set_PluginErrorL"));
	iPower1 = CHWRMPower::NewL();
    
	iPower2 = CHWRMPower::NewLC();
    CleanupStack::Pop();

    
    iPower1->SetPowerReportObserver(this);
    iPower2->SetPowerReportObserver(this);
    
	TInt batteryError = KErrGeneral;
	
	ExpectLeave(KErrGeneral);
	ExpectBatteryPowerMeasurementsErrorL(batteryError);

	
	TRequestStatus status;	
    User::LeaveIfError(iPower1->StartAveragePowerReporting(status, 6));
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

    RMockPower& mockPower = GetMockPower();
    TUint intervalMultiple = 2;
    TPckgBuf<TUint> intervalMultiplePckg(intervalMultiple);
    mockPower.ExpectProcessCmdL(HWRMPowerCommand::ESetPowerReportingIntervalMultipleCmdId, intervalMultiplePckg);

    HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponse;
    

    HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponsePckg(batteryError);
    mockPower.CompleteL(HWRMPowerCommand::ESetPowerReportingIntervalMultipleCmdId, 4, batteryErrorResponsePckg);
    

    User::LeaveIfError(iPower2->StartAveragePowerReporting(status, 4));

    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());
    
	}

void CHWRMTest::PowerNotify_ClosePowerInstanceL()
    {
    delete iPower1;
    delete iPower2;
    }

void CHWRMTest::PowerNotify_Cancel_PluginErrorL()
	{
	INFO_PRINTF1(_L("Step state: PowerNotify_Cancel_PluginErrorL"));
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	//the client will not receive any error. It's session data has been removed and any results have been sent.
	RMockPower& mockPower = GetMockPower();

	TBuf8<1> emptyDes;
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EStopAverageBatteryPowerReportingCmdId, emptyDes);
	
	HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponse;

	HWRMPowerCommand::TErrorCodeResponsePackage batteryErrorResponsePckg(KErrGeneral);
	mockPower.CompleteL(HWRMPowerCommand::EStopAverageBatteryPowerReportingCmdId, 2, batteryErrorResponsePckg);
	
	power.StopAveragePowerReportingL();
	}
void CHWRMTest::SetPowerMeasurementTime200L()
	{
	INFO_PRINTF1(_L("Step state: SetPowerMeasurementTime200L"));	
	SetPowerAttributesMeasurementProcessingTime(200);
	}

	
void CHWRMTest::TestPowerNotifyL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotifyL"));	
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_CancelL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_InfiniteL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_InfiniteL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::SetReportingPeriodZeroL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 16*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_CancelL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_AdjustReportingPeriodL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_AdjustReportingPeriodL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 3*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::SetReportingPeriod30L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Continue_NoTimeoutL, 7*KTwoTimeUnits); //No timeout in this state
	AddTestStateL(this, &CHWRMTest::PowerNotify_Continue_TimeoutL, 10*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_BigIntervalL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_BigIntervalL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Interval20L, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_CancelL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_MultiClient_SameIntervalL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_MultiClient_SameIntervalL"));
	AddTestStateL(this, &CHWRMTest::PowerOpen3SessionsL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackMC3L, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyMC3_SameIntervalL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_CancelMC3L, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_MultiClient_DiffIntervalL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_MultiClient_DiffIntervalL"));
	AddTestStateL(this, &CHWRMTest::PowerOpen3SessionsL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackMC3L, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyMC3_DiffIntervalL, 7*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_CancelMC3L, 3*KTwoTimeUnits);
	//AddTestStateL(this, &CHWRMTest::PowerNotify_CancelMC3L, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_RedundantL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_RedundantL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_RedundantL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotify_CancelL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_Start_PluginErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_Start_PluginErrorL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Start_PluginErrorL, 0);
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_Start_Set_PluginErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_Start_Set_PluginErrorL"));
    //Open MockSy here. Power Session is not used in PowerNotify_Start_Set_PluginErrorL
    AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Start_Set_PluginErrorL, 5*KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::PowerNotify_ClosePowerInstanceL, 2*KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	ExecuteTestL();
	}


void CHWRMTest::TestPowerNotify_Stop_PluginErrorL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_Stop_PluginErrorL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Cancel_PluginErrorL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_IntervalEqualL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_IntervalEqualL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::SetReportingPeriod5L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Interval50L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Continue_TimeoutL, 5*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_IntervalGreaterL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_IntervalGreaterL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::SetReportingPeriod5L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Interval60L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Continue_TimeoutL, 5*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_AdjustReportingPeriod_WithoutRequestL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_AdjustReportingPeriod_WithoutRequestL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::SetReportingPeriod30L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 3*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Continue_NoTimeoutL, 7*KTwoTimeUnits); //No timeout in this state
	AddTestStateL(this, &CHWRMTest::PowerNotify_Continue_TimeoutL, 10*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_AdjustReportingPeriod_ElapsedTimeLessL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_AdjustReportingPeriod_ElapsedTimeLessL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 3*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::SetReportingPeriod30L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Continue_NoTimeoutL, 7*KTwoTimeUnits); //No timeout in this state
	AddTestStateL(this, &CHWRMTest::PowerNotify_Continue_TimeoutL, 10*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_AdjustReportingPeriod_ElapsedTimeMoreL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_AdjustReportingPeriod_ElapsedTimeMoreL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 6*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::SetReportingPeriod10L_TimeoutL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_AdjustReportingPeriod_NonZeroToZeroL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_AdjustReportingPeriod_NonZeroToZeroL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 3*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::SetReportingPeriodZeroL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Continue_NoTimeoutL, 12*KTwoTimeUnits); //No timeout in this state
	AddTestStateL(this, &CHWRMTest::PowerNotify_CancelL, 0);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_AdjustReportingPeriod_ZeroToNonZeroL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_AdjustReportingPeriod_ZeroToNonZeroL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::SetReportingPeriodZeroL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 3*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Continue_TimeoutL, 11*KTwoTimeUnits);
		
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_AdjustReportingPeriod_SameValueL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_AdjustReportingPeriod_SameValueL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 3*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Continue_TimeoutL, 8*KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_AdjustReportingPeriod_NegativeValueL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_AdjustReportingPeriod_NegativeValueL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 3*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::SetReportingPeriod_Power_NegativeL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_All_AdjustReportingPeriod_NegativeValueL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_All_AdjustReportingPeriod_NegativeValueL"));
	AddTestStateL(this, &CHWRMTest::PowerOpen3SessionsL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 3*KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::SetReportingPeriod_Power_NegativeL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	
	ExecuteTestL();
	}


void CHWRMTest::TestPowerNotify_SlowClientL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_SlowClientL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
    AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::SetPowerMeasurementTime200L, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Interval1L, 6*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_CancelL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_WithoutCallbackL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_WithoutCallbackL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
    AddTestStateL(this, &CHWRMTest::RegisterNullPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotify_NoCallbackL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_MultiClient_DiffInterval_SpecialCaseL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_MultiClient_DiffInterval_SpecialCaseL"));
	AddTestStateL(this, &CHWRMTest::PowerOpen2SessionsL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackMC2L, 0);
	AddTestStateL(this, &CHWRMTest::SetReportingPeriod10L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotifyMC2_DiffInterval_SpecialCaseL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_CancelMC2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ResetReportingPeriodL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_Stop_WithoutStartL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_Stop_WithoutStartL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotify_CancelL, 0);
	
	ExecuteTestL();
	}

void CHWRMTest::TestPowerNotify_Stop_RedundantL()
	{
	INFO_PRINTF1(_L("Start test step: TestPowerNotify_Stop_RedundantL"));
	AddTestStateL(this, &CHWRMTest::PowerOpenL, 0);
	AddTestStateL(this, &CHWRMTest::RegisterPowerCallbackL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotifyL, 4*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::PowerNotify_CancelL, 0);
	AddTestStateL(this, &CHWRMTest::PowerNotify_Cancel_RedundantL, KTwoTimeUnits);
	
	ExecuteTestL();
	}


//
// Tx Test States and Steps
//

void CHWRMTest::TestFmTxConnectL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxConnectL"));
	
	// set Inactivity timout to 20 seconds so that state does not change 
	// unexpectedly during the tests because of the timeout.
	AddTestStateL(this, &CHWRMTest::FmTxSetInactivityTimeOutTestModeL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestFmTxStatusOffL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxStatusOffL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxDisableL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxStatusOffL, KOneTimeUnit);	 		

	ExecuteTestL();
	}

void CHWRMTest::TestFmTxEnableInvalidFreqL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxEnableInvalidFreqL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableInvalidFreqL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxStatusOffL, KOneTimeUnit);			
	AddTestStateL(this, &CHWRMTest::FmTxDisableL, KTwoTimeUnits);			

	ExecuteTestL();		
	}

void CHWRMTest::TestFmTxEnableL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxEnableL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxStatusEnabledL, KOneTimeUnit);
	// for conditional coverage. Status does not change
	AddTestStateL(this, &CHWRMTest::FmTxEnableWhileActiveOrInactiveL, KOneTimeUnit); 	
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestFmTxEnableNewFreqL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxEnableNewFreqL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableNewFreqL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxStatusEnabledL, KOneTimeUnit); 	
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);			

	ExecuteTestL();	
	}

void CHWRMTest::TestFmTxEnableInvalidFreqWhenOnL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxEnableInvalidFreqWhenOnL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableValidFreqL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxEnableInvalidFreqL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxStatusEnabledL, KOneTimeUnit); 
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);			

	ExecuteTestL();	
	}

void CHWRMTest::TestFmTxReserveL()
    {
    INFO_PRINTF1(_L("Start test step: TestFmTxReserveL"));
    	
    AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::FmTxReserveL, KTwoTimeUnits);
    AddTestStateL(this, &CHWRMTest::FmTxReleaseL, KTwoTimeUnits);

    ExecuteTestL();
    }

void CHWRMTest::TestFmTxEnableDuringReservationL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxEnableDuringReservationL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpen2SessionsL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxDisableL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxReserveL, KTwoTimeUnits); 	
	
	AddTestStateL(this, &CHWRMTest::FmTx2EnableDuringReservationL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTx2StatusOffL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::FmTxReleaseL, KTwoTimeUnits);

	ExecuteTestL();
	}

void CHWRMTest::TestFmTxDisableL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxDisableL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxDisableL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxStatusOffL, KOneTimeUnit); 
	
	ExecuteTestL();	
	}

void CHWRMTest::TestFmTxDisableDuringReservationL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxDisableDuringReservationL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpen2SessionsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTx2ReserveL, KTwoTimeUnits); 	
	AddTestStateL(this, &CHWRMTest::FmTx2EnableL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTx2StatusEnabledL, KOneTimeUnit); 
	
	AddTestStateL(this, &CHWRMTest::FmTxGetCurrentStatusL, KOneTimeUnit); 
	AddTestStateL(this, &CHWRMTest::FmTxDisableDuringReservation, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxCompareCurrentAndTempStatusL, KTwoTimeUnits);

	AddTestStateL(this, &CHWRMTest::FmTx2DisableFromOnL, KTwoTimeUnits); 
	
	ExecuteTestL();	
	}

void CHWRMTest::TestFmTxReserveDuringReservationL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxReserveDuringReservationL	"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpen2SessionsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTx2EnableL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxReserveL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTx2ReserveL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxReleaseL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTx2DisableFromOnL, KTwoTimeUnits); 
	
	ExecuteTestL();	
	}

void CHWRMTest::TestFmTxGetFrequencyL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxGetFrequencyL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxGetFrequencyL, KOneTimeUnit);		

    ExecuteTestL();
	}

void CHWRMTest::TestFmTxSetFrequencyL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxSetFrequencyL"));

	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxGetFrequencyStepSizeL, KOneTimeUnit);
	
	AddTestStateL(this, &CHWRMTest::FmTxSetFrequencyL, KTwoTimeUnits);		
	AddTestStateL(this, &CHWRMTest::FmTxCheckSetFrequencyL, KOneTimeUnit);	
	// Set frequency that needs to be rounded up to the closest valid frequency
	// base on the step size and the min & max frequency range
	AddTestStateL(this, &CHWRMTest::FmTxSetFrequencyRoundUpL, KTwoTimeUnits);		
	AddTestStateL(this, &CHWRMTest::FmTxCheckSetFrequencyL, KOneTimeUnit);
	// Set frequency that needs to be rounded up to the closest valid frequency
	// base on the step size and the min & max frequency range
	AddTestStateL(this, &CHWRMTest::FmTxSetFrequencyRoundDownL, KTwoTimeUnits);		
	AddTestStateL(this, &CHWRMTest::FmTxCheckSetFrequencyL, KOneTimeUnit);	
	
	// the same while enabled
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxSetFrequencyL, KTwoTimeUnits);		
	AddTestStateL(this, &CHWRMTest::FmTxCheckSetFrequencyL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxSetFrequencyInvalidL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxSetFrequencyInvalidL"));
	
	// Update frequency first in order to get observed value updated
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetFrequencyL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxCheckSetFrequencyL, KTwoTimeUnits);	
	
	// Now try to set illegal frequency
	AddTestStateL(this, &CHWRMTest::FmTxSetFrequencyInvalidL, KTwoTimeUnits);		
	AddTestStateL(this, &CHWRMTest::FmTxCheckSetFrequencyL, KTwoTimeUnits);	
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxSetFrequencyDuringReservationL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxSetFrequencyDuringReservationL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpen2SessionsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxReserveL, KTwoTimeUnits); 	
	AddTestStateL(this, &CHWRMTest::FmTxSetFrequencyL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxCheckSetFrequencyL, KTwoTimeUnits);	
	
	AddTestStateL(this, &CHWRMTest::FmTx2SetFrequencyDuringReservationL,KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxCheckSetFrequencyL, KTwoTimeUnits);	//check that frequency did not change
	
	AddTestStateL(this, &CHWRMTest::FmTxReleaseL, KTwoTimeUnits);
	
	ExecuteTestL();	
	}

void CHWRMTest::TestFmTxGetFrequencyRangeL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxGetFrequencyRangeL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxSetFrequencyRangeEuropeL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxGetFrequencyRangeL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxCheckGetFrequencyRangeL, KOneTimeUnit);	
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxGetJapanFrequencyRangeL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxGetJapanFrequencyRangeL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetFrequencyRangeJapanL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxGetFrequencyRangeL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxCheckGetJapanFrequencyRangeL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxSetFrequencyRangeEuropeL, KOneTimeUnit);
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxGetNextClearFrequencyL()
	{
	INFO_PRINTF1(_L("Start test step: GetNextClearFrequencyTestCase"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxGetNextClearFrequencyAsyncL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxGetNextClearFrequencyAsyncL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyAsyncL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestFmTxGetNextClearFrequencyAsyncCancelL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxGetNextClearFrequencyAsyncCancelL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyAsyncCancelL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestFmTxGetNextClearFrequencyAsyncArrayL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxGetNextClearFrequencyAsyncArrayL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyAsyncArrayErrArgumentMinL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyAsyncArrayErrArgumentMaxL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyAsyncArrayL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestFmTxGetNextClearFrequencyArrayL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxGetNextClearFrequencyArrayL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyArrayWhileDisabledL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyArrayErrArgumentMinL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyArrayErrArgumentMaxL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyArrayL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
	ExecuteTestL();
	}

void CHWRMTest::TestFmTxSetNextClearFrequencyL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxSetNextClearFrequencyL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetNextClearFrequencyL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxSetNextClearFrequencyAsyncL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxSetNextClearFrequencyAsyncL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyAsyncL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetNextClearFrequencyAsyncL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxSetNextClearFrequencyAsyncCancelL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxSetNextClearFrequencyAsyncCancelL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxGetNextClearFrequencyAsyncL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetNextClearFrequencyAsyncCancelL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxAutoFrequencyL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxAutoFrequencyL"));
	
	// Not currently supported. Get the KErrNotSupported errors
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxIsAutoFrequencyEnabled, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxEnableAutoFrequencyL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxDisableAutoFrequencyL, KTwoTimeUnits);	
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxSetRdsDataL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxSetRdsDataL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetRdsPtyL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetRdsPtyKErrArgumentMinL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxSetRdsPtyKErrArgumentMaxL, KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxSetRdsPtynL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxSetRdsMsFalseL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxSetRdsMsTrueL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxSetRdsLangIdL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxSetRdsLangIdKErrArgumentMinL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxSetRdsLangIdKErrArgumentMaxL, KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxClearRdsDataL, 3*KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);

    ExecuteTestL();
	}

void CHWRMTest::TestFmTxSetRadiotextL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxSetRadiotextL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetRadiotextL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxSetRadiotextContentL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxSetRadiotextContentL"));
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetRadiotextContent2WhileDisabledL, KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetRadiotextContent1L, KTwoTimeUnits); //invalid content, lower limit
	AddTestStateL(this, &CHWRMTest::FmTxSetRadiotextContent3L, KTwoTimeUnits); //invalid content, upper limit
	AddTestStateL(this, &CHWRMTest::FmTxSetRadiotextContent2L, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits); //The ETxOffCmd has to be queued in mockhwrmSY
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxSetRadiotextTagL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxSetRadiotextTagL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetRadiotextTag2WhileDisabledL, KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetRadiotextTag1L, KTwoTimeUnits);	// with invalid tag
	AddTestStateL(this, &CHWRMTest::FmTxSetRadiotextTag2L, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);

    ExecuteTestL();
	}

void CHWRMTest::TestFmTxSetRadiotextTagsL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxSetRadiotextTagsL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, 2*KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxSetRadiotextTags1L, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxSetRadiotextTags2L, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);
	
    ExecuteTestL();
	}

#ifdef INSECURE_AUDIO_POLICY_KEYS    
void CHWRMTest::TestFmTxAudioRoutingOnAndOffL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxAudioRoutingOnAndOffL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxAudioRoutingOnL, KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxCheckExpectedAudioRoutingStateL, KOneTimeUnit);		
	AddTestStateL(this, &CHWRMTest::FmTxAudioRoutingOffL, KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxCheckExpectedAudioRoutingStateL, KOneTimeUnit);		
	AddTestStateL(this, &CHWRMTest::FmTxDisableFromOnL, KTwoTimeUnits);

    ExecuteTestL();
	}

void CHWRMTest::TestFmTxPowerSaveOnAccessoryL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxPowerSaveOnAccessoryL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxPowerSaveOnAccessoryL, 5*KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxCheckExpectedAudioRoutingStateL, KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxWatcherPluginDisconnectL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxCheckPowerSaveStateL, KTwoTimeUnits);
	
	// mimic watcher plugin behaviour, after headset is attached 
	AddTestStateL(this, &CHWRMTest::FmTxPowerSaveOnAccessoryL, 5*KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxCheckExpectedAudioRoutingStateL, KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxEnableDuringPowersaveL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxWatcherPluginDisconnectL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxCheckPowerSaveStateL, KTwoTimeUnits);
		
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxPowerSaveOnOffStateL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxPowerSaveOnOffStateL"));

	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxExpectPowerSaveOffL, 21*KOneTimeUnit);
		
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxPowerSaveOnInactivityStateL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxPowerSaveOnOffStateL"));

	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxAudioRoutingOnL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxAudioRoutingOffL, KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxExpectPowerSaveOnInactivityL, 21*KOneTimeUnit);

	// mimic watcher plugin behaviour, after headset is attached 
	AddTestStateL(this, &CHWRMTest::FmTxPowerSaveOnAccessoryL, 5*KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxCheckExpectedAudioRoutingStateL, KOneTimeUnit);		
	AddTestStateL(this, &CHWRMTest::FmTxWatcherPluginDisconnectL, 4*KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxCheckPowerSaveStateL, KTwoTimeUnits);
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxPowerSaveOffL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxPowerSaveOffL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableL, KTwoTimeUnits); 
	AddTestStateL(this, &CHWRMTest::FmTxPowerSaveOnAccessoryL, 5*KOneTimeUnit);	
	AddTestStateL(this, &CHWRMTest::FmTxPowerSaveOffL,5*KOneTimeUnit);
	AddTestStateL(this, &CHWRMTest::FmTxCheckPowerSaveStateL, KTwoTimeUnits);
	
    ExecuteTestL();
	}

void CHWRMTest::TestFmTxEnableDuringPowersaveL()
	{
	INFO_PRINTF1(_L("Start test step: TestFmTxEnableDuringPowersaveL"));
	
	AddTestStateL(this, &CHWRMTest::FmTxOpenL, KTwoTimeUnits);	
	/*connect mutually exclusive accessory*/
	AddTestStateL(this, &CHWRMTest::FmTxWatcherPluginConnectL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FmTxEnableDuringPowersaveL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxStatusPowersaveL, KTwoTimeUnits);	
	AddTestStateL(this, &CHWRMTest::FmTxEnableDuringPowersave2L, KTwoTimeUnits); // Covers other conditions inf HWRMFmTxService
	AddTestStateL(this, &CHWRMTest::FmTxWatcherPluginDisconnectL, KTwoTimeUnits);	
	
    ExecuteTestL();
	}

#endif // INSECURE_AUDIO_POLICY_KEYS

void CHWRMTest::InvalidSessionLightL()
	{
	INFO_PRINTF1(_L("Step state: InvalidSessionLightL"));
	
	ExpectLeave(KErrBadHandle);
	CHWRMLight* light = CHWRMLight::NewL(NULL);
	}

void CHWRMTest::InvalidSessionExtendedLightL()
	{
	INFO_PRINTF1(_L("Step state: InvalidSessionExtendedLightL"));
	
	ExpectLeave(KErrBadHandle);
	MHWRMExtendedLightObserver* dummyObserver(NULL);
	CHWRMExtendedLight* extLight = CHWRMExtendedLight::NewL(dummyObserver);
	}

void CHWRMTest::InvalidSessionVibraL()
	{
	INFO_PRINTF1(_L("Step state: InvalidSessionVibraL"));
	
	ExpectLeave(KErrBadHandle);
	CHWRMVibra* vibra = CHWRMVibra::NewL(NULL);
	}
	
void CHWRMTest::PowerStateOpenL()
	{
	INFO_PRINTF1(_L("Step state: PowerStateOpenL"));
		
	// connect to mock power state plugin
	OpenPowerStateSessionL();		
	}

void CHWRMTest::ExpectLegacyBatteryStatusL()
    {
    delete iExtendedBatteryStatusObserver;
    iExtendedBatteryStatusObserver = NULL;
    iExpectedExtendedBatteryStatusNotifications.Reset();
    }

void CHWRMTest::ExpectLegacyChargingStatusL()
    {
    delete iExtendedChargingStatusObserver;
    iExtendedChargingStatusObserver = NULL;
    iExpectedExtendedChargingStatusNotifications.Reset();
    }

void CHWRMTest::ExpectNewBatteryStatusL()
    {
    delete iBatteryStatusObserver;
    iBatteryStatusObserver = NULL;
    iExpectedBatteryStatusNotifications.Reset();
    }

void CHWRMTest::ExpectNewChargingStatusL()
    {
    delete iChargingStatusObserver;
    iChargingStatusObserver = NULL;
    iExpectedChargingStatusNotifications.Reset();
    }

void CHWRMTest::BatteryLevelL()
	{
	INFO_PRINTF1(_L("Step state: BatteryLevelL"));
		
	ExpectBatteryLevelNotificationL(EBatteryLevelLevel7);
	TInt delay(5);
	iMockPowerState.CompleteL(KHWRMBatteryLevel,EBatteryLevelLevel7,delay);
	User::After(KOneSecond);
	}

void CHWRMTest::BatteryStateL()
	{
	INFO_PRINTF1(_L("Step state: BatteryStateL"));
		
	ExpectBatteryStatusNotificationL(EBatteryStatusOk);
	TInt delay(5);
	iMockPowerState.CompleteL(KHWRMBatteryStatus,EBatteryStatusOk,delay);
	User::After(KOneSecond);
	
	}
	
// Pump New Values from MockServer and test the legacy keys
void CHWRMTest::BatteryStateUnknownErrorL()
    {
    INFO_PRINTF1(_L("Step state: BatteryStateUnknownErrorL"));
     
    ExpectBatteryStatusNotificationL(EBatteryStatusUnknown);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMBatteryStatus,EBatteryStatusUnknown,delay);
    User::After(KOneSecond);    
    }
	
void CHWRMTest::BatteryStateUnsupportedL()
    {
    INFO_PRINTF1(_L("Step state: BatteryStateUnsupportedL"));
     
    ExpectBatteryStatusNotificationL(EBatteryStatusUnknown);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMBatteryStatus,EBatteryStatusNotSupported,delay);
    User::After(KOneSecond);    
    }	

void CHWRMTest::BatteryStateAuthErrorL()
    {
    INFO_PRINTF1(_L("Step state: BatteryStateAuthErrorL"));
    
    ExpectBatteryStatusNotificationL(EBatteryStatusUnknown);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMBatteryStatus,EBatteryStatusDbiAuthFailed,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::BatteryStateStatusOkL()
    {
    INFO_PRINTF1(_L("Step state: BatteryStateStatusOkL"));
        
    ExpectBatteryStatusNotificationL(EBatteryStatusOk);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMBatteryStatus,EBatteryStatusOk,delay);
    User::After(KOneSecond);    
    }

void CHWRMTest::BatteryStateStatusLowL()
    {
    INFO_PRINTF1(_L("Step state: BatteryStateStatusLowL"));
    
    ExpectBatteryStatusNotificationL(EBatteryStatusLow);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMBatteryStatus,EBatteryStatusLow,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::BatteryStateStatusEmptyL()
    {
    INFO_PRINTF1(_L("Step state: BatteryStateStatusEmptyL"));
    
    ExpectBatteryStatusNotificationL(EBatteryStatusEmpty);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMBatteryStatus,EBatteryStatusEmpty,delay);
    User::After(KOneSecond);
    }	
			
// Pump New Values from MockServer and test the extended keys
void CHWRMTest::ExtendedBatteryStateUnknownErrorL()
    {
    INFO_PRINTF1(_L("Step state: ExtendedBatteryStateUnknownErrorL"));
     
    ExpectExtendedBatteryStatusNotificationL(EBatteryStatusUnknown);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMExtendedBatteryStatus,EBatteryStatusUnknown,delay);
    User::After(KOneSecond);    
    }

void CHWRMTest::ExtendedBatteryStateUnsupportedL()
    {
    INFO_PRINTF1(_L("Step state: ExtendedBatteryStateUnsupportedL"));
     
    ExpectExtendedBatteryStatusNotificationL(EBatteryStatusNotSupported);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMExtendedBatteryStatus,EBatteryStatusNotSupported,delay);
    User::After(KOneSecond);    
    }
	
void CHWRMTest::ExtendedBatteryStateAuthErrorL()
    {
    INFO_PRINTF1(_L("Step state: ExtendedBatteryStateAuthErrorL"));
    
    ExpectExtendedBatteryStatusNotificationL(EBatteryStatusDbiAuthFailed);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMExtendedBatteryStatus,EBatteryStatusDbiAuthFailed,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ExtendedBatteryStateStatusOkL()
    {
    INFO_PRINTF1(_L("Step state: ExtendedBatteryStateStatusOkL"));
        
    ExpectExtendedBatteryStatusNotificationL(EBatteryStatusOk);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMExtendedBatteryStatus,EBatteryStatusOk,delay);
    User::After(KOneSecond);    
    }

void CHWRMTest::ExtendedBatteryStateStatusLowL()
    {
    INFO_PRINTF1(_L("Step state: ExtendedBatteryStateStatusLowL"));
    
    ExpectExtendedBatteryStatusNotificationL(EBatteryStatusLow);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMExtendedBatteryStatus,EBatteryStatusLow,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ExtendedBatteryStateStatusEmptyL()
    {
    INFO_PRINTF1(_L("Step state: ExtendedBatteryStateStatusEmptyL"));
    
    ExpectExtendedBatteryStatusNotificationL(EBatteryStatusEmpty);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMExtendedBatteryStatus,EBatteryStatusEmpty,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ChargingStatusL()
	{
	INFO_PRINTF1(_L("Step state: ChargingStatusL"));
	
	ExpectedChargingStatusNotificationsL(EChargingStatusChargingComplete);
	TInt delay(5);
	iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusChargingComplete,delay);
	User::After(KOneSecond);
	
	}

void CHWRMTest::ChargingStatusNotConnectedL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusNotConnectedL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusNotConnected);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusNotConnected,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ChargingStatusChargingOkL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusChargingOkL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusCharging);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusCharging,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ChargingStatusNotChargingL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusNotChargingL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusNotCharging);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusNotCharging,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ChargingStatusAlmostCompleteL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusAlmostCompleteL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusAlmostComplete);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusAlmostComplete,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ChargingStatusCompletedChargingL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusCompletedChargingL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusChargingComplete);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusChargingComplete,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ChargingStatusChargingContinuesL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusChargingContinuesL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusChargingContinued);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusChargingContinued,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ChargingStatusErrorL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusErrorL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusError);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusError,delay);
    User::After(KOneSecond);
    }

// Below API's publishes new Charging States and expects mapped states using the legacy P&S key
void CHWRMTest::ChargingStatusIllegalErrorL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusIllegalErrorL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusError);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusIllegalChargerError,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ChargingStatusUnderVoltageErrorL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusUnderVoltageL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusError);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusChargerUnderVoltageError,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ChargingStatusOverVoltageErrorL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusOverVoltageL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusError);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusChargerOverVoltageError,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ChargingStatusTooHotErrorL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusTooHotErrorL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusError);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusTempTooHotError,delay);
    User::After(KOneSecond);
    }

void CHWRMTest::ChargingStatusTooColdErrorL()
    {
    INFO_PRINTF1(_L("Step state: ChargingStatusTooColdErrorL"));
    
    ExpectedChargingStatusNotificationsL(EChargingStatusError);
    TInt delay(5);
    iMockPowerState.CompleteL(KHWRMChargingStatus,EChargingStatusTempTooColdError,delay);
    User::After(KOneSecond);
    }
	
void CHWRMTest::VibraOpenL()
	{
	INFO_PRINTF1(_L("Step state: VibraOpenL"));
	
	ExpectVibraModeNotificationL(CHWRMVibra::EVibraModeON);	
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped);

	// one vibra session
	OpenSessionsL(0,1);	
	}
	
void CHWRMTest::VibraPulseOpenL()
	{
	INFO_PRINTF1(_L("Step state: VibraPulseOpenL"));
	
	ExpectVibraModeNotificationL(CHWRMVibra::EVibraModeON);	
	ExpectVibraFeedbackModeNotificationL(CHWRMVibra::EVibraFeedbackModeON);	
	
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped);	

	// one vibra session
	OpenSessionsL(0,1);
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();		
	vibra.SetFeedbackObserver(this);	
	}
	
void CHWRMTest::VibraPulseOpenSessionsL()
	{
	INFO_PRINTF1(_L("Step state: VibraPulseOpenSessionsL"));
	
	ExpectVibraModeNotificationL(CHWRMVibra::EVibraModeON);	
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped);
	
	ExpectVibraFeedbackModeNotificationL(CHWRMVibra::EVibraFeedbackModeON);	
	
	// two vibra sessions
	OpenSessionsL(0,2);	
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();		
	vibra.SetFeedbackObserver(this);
	}
	
void CHWRMTest::VibraOpenWhileDisabledL()
	{
	INFO_PRINTF1(_L("Step state: VibraOpenWhileDisabledL"));
	
	ExpectVibraModeNotificationL(CHWRMVibra::EVibraModeOFF);	
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusNotAllowed);
	
	// one vibra session
	OpenSessionsL(0,1);	
	}

void CHWRMTest::VibraOnL()
	{
	INFO_PRINTF1(_L("Step state: VibraOnL"));
	
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	// First queue expected notification
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusOn);
	// then send the command
	vibra.StartVibraL(iVibraMaxOnTime/2);
	}

void CHWRMTest::VibraOnNoNotificationExpectedL()
	{
	INFO_PRINTF1(_L("Step state: VibraOnNoNotificationExpectedL"));
	
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	// then send the command
	vibra.StartVibraL(iVibraMaxOnTime/2, KHWRMVibraMinIntensity);
	}

void CHWRMTest::DoPulseVibraL(CHWRMVibra& aVibra, TInt aDuration)
	{
	INFO_PRINTF2(_L("DoPulseVibra - aDuration = %d") ,aDuration);

	aVibra.PulseVibraL(aDuration);			
	}

void CHWRMTest::DoPulseVibraL(CHWRMVibra& aVibra, TInt aDuration, TInt aIntensity)
	{
	INFO_PRINTF3(_L("DoPulseVibra - aDuration = %d, aIntensity=%d") ,aDuration, aIntensity);

	aVibra.PulseVibraL(aDuration, aIntensity);			
	}
	
void CHWRMTest::DoPulseVibraL(CHWRMVibra& aVibra)
	{
	INFO_PRINTF1(_L("DoPulseVibra with default duration and intensity"));

	aVibra.PulseVibraL();			
	}
	

void CHWRMTest::PulseVibraL()
	{
    INFO_PRINTF1(_L("Step state: PulseVibraL"));
            
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();		
	
	DoPulseVibraL(vibra, 1000, 30);	
	}
	
void CHWRMTest::MultVibraPulsesL()
	{
	INFO_PRINTF1(_L("Step state: MultVibraPulsesL"));
            
	//only one Vibra status update (EVibraStatusOn) should be received
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusOn);            
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();				
	
	vibra.StartVibraL(5000);
	
	DoPulseVibraL(vibra, 600,50); 
	DoPulseVibraL(vibra, 200,100);
	
	//a Vibra status update of VibraStatusStopped should be received in the 
	//next test state: CheckForVibraStatusStoppedL		
	}	
	
void CHWRMTest::PulsesOverlapPulsesL()	
	{
	INFO_PRINTF1(_L("Step state: PulsesOverlapPulsesL"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();		
	
    // Vibra status does not get updated for pulse requests (when real pulse command
    // configured to be processed by plugin via KHWRMInternalSettingsDeviceSupportsPulse), 
    // because otherwise short pulses
    // a) would bring quite a lot of overhead when publishing vibra on/off status very frequently
    // b) in addition to publishing status, hwrm should keep timers active just for publishing status
	
	DoPulseVibraL(vibra, 5000, 10);	
	DoPulseVibraL(vibra, 1000,100);
	DoPulseVibraL(vibra, 1400,30);
	DoPulseVibraL(vibra, 1000,80);
	DoPulseVibraL(vibra, 200, 50);
	DoPulseVibraL(vibra, 600, 60);
	
	//a Vibra status update of VibraStatusStopped should be received in the 
	//next test state: CheckForVibraStatusStoppedL			
	}
		
		
void CHWRMTest::SequentialPulsesL()
	{
	INFO_PRINTF1(_L("Step state: SequentialPulsesL"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();		
		
	//ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusOn);
//	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped);		
	
	TInt intensity = 0;		
	for(TInt i=0; i < 20; i++)
		{		
		intensity = intensity + 5; 			
		DoPulseVibraL(vibra, 100, intensity);		
		}		
	}

void CHWRMTest::UnSupportedPulseIntensity1L()
	{
	INFO_PRINTF1(_L("Step state: UnSupportedPulseIntensity1L"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
		
	ExpectLeave(KErrArgument);
	
	DoPulseVibraL(vibra, 100, -100);			
	}
	
void CHWRMTest::UnSupportedPulseIntensity2L()
	{
	INFO_PRINTF1(_L("Step state: UnSupportedPulseIntensity2L"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
		
	ExpectLeave(KErrArgument);
		
	DoPulseVibraL(vibra, 100, 0);		
	}

void CHWRMTest::UnSupportedPulseIntensity3L()
	{
	INFO_PRINTF1(_L("Step state: UnSupportedPulseIntensity3L"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();		
	
	ExpectLeave(KErrArgument);
	
	DoPulseVibraL(vibra, 100, 101);
	
	}

void CHWRMTest::UnSupportedPulseDuration1L()
	{
	INFO_PRINTF1(_L("Step state: UnSupportedPulseDuration1L"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();		
	
	ExpectLeave(KErrArgument);
	
	DoPulseVibraL(vibra, KHWRMVibraMaxDuration + 1);	
	}

void CHWRMTest::UnSupportedPulseDuration2L()
	{
	INFO_PRINTF1(_L("Step state: UnSupportedPulseDuration2L"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();		
	
	ExpectLeave(KErrArgument);
	
	DoPulseVibraL(vibra, -1);	
	}

void CHWRMTest::UnSupportedPulseDuration3L()
	{
	INFO_PRINTF1(_L("Step state: UnSupportedPulseDuration3L"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();		
	
	ExpectLeave(KErrArgument);
	
	DoPulseVibraL(vibra, KHWRMVibraMaxDuration + 1, KHWRMVibraMinIntensity);	
	}

void CHWRMTest::UnSupportedPulseDuration4L()
	{
	INFO_PRINTF1(_L("Step state: UnSupportedPulseDuration4L"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();		
	
	ExpectLeave(KErrArgument);
	
	DoPulseVibraL(vibra, -1, KHWRMVibraMinIntensity);	
	}

void CHWRMTest::VibraOffL()
	{
	INFO_PRINTF1(_L("Step state: VibraOffL"));

	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();	
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped);
	vibra.StopVibraL();
	}

void CHWRMTest::VibraFeedbackSettingsL()
	{
	INFO_PRINTF1(_L("Step state: VibraFeedbackSettingsL"));

	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	
	vibra.VibraFeedbackSettings();
	}

void CHWRMTest::VibraOffWhenDisabledL()
	{
	INFO_PRINTF1(_L("Step state: VibraOffWhenDisabledL"));

	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();	    		
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusNotAllowed);
	vibra.StopVibraL();
	}
	

void CHWRMTest::VibraOnWithResponseErrL()
	{
	INFO_PRINTF1(_L("Step state: VibraOnWithPluginErrL"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	
	/* This is an example how to use the MockSY */
	
	// Queue expected SY call and completion
	
	// First get the MockSY session
	RMockVibra& mockVibra = GetMockVibra();
	// Queue expected command. transId is not require. If not provided, the MockSY doesn't check it
	// and execute following complete with the same transID than the Process command
	mockVibra.ExpectProcessCmdL(HWRMVibraCommand::EStartVibraWithDefaultSettingsCmdId, KNullDesC8);
	// and the corresponding complete (time is in MockSY unit: 0.1 sec)
	// The SY will complete the request with KErrGeneral
	HWRMLightCommand::TErrorCodeResponsePackage responseErr(KErrGeneral);
	mockVibra.CompleteL(HWRMVibraCommand::EStartVibraWithDefaultSettingsCmdId, 1, responseErr);

	// Queue expected local notifications
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusUnknown);

	// Expect StartVibraL to leave with KErrGeneral (the value returned by the MockSY)
	ASSERT_LEAVE(KErrGeneral,vibra.StartVibraL(iVibraMaxOnTime/2));
	}
	
void CHWRMTest::VibraOnWithIntensityL()
	{
	INFO_PRINTF1(_L("Step state: VibraOnWithIntensityL"));
	
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	// read the vibra attributes from the central repository
	GetVibraTimeAttributesL();	

	// test state: start vibra
	// First queue expected notification
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusOn);
	// then send the command
	vibra.StartVibraL(iVibraMaxOnTime/2,50); // intensity 50	
	}

void CHWRMTest::VibraOnWithIntensityOutOfRangeL()
	{
	INFO_PRINTF1(_L("Step state: VibraOnWithIntensityOutOfRangeL"));
	
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	// read the vibra attributes from the central repository
	GetVibraTimeAttributesL();	

	// Attempting to start vibra with an invalid intensity results in a KErrArgument error: 
	// (see CHWRMVibraImpl::StartVibraL(TInt,TInt))
	
	ExpectLeave(KErrArgument);
	vibra.StartVibraL(0, KHWRMVibraMaxIntensity+1);
	}

void CHWRMTest::VibraOnWithIntensityOutOfRange2L()
	{
	INFO_PRINTF1(_L("Step state: VibraOnWithIntensityOutOfRange2L"));
	
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	// read the vibra attributes from the central repository
	GetVibraTimeAttributesL();	

	// Attempting to start vibra with an invalid intensity results in a KErrArgument error: 
	// (see CHWRMVibraImpl::StartVibraL(TInt,TInt))
	
	ExpectLeave(KErrArgument);
	vibra.StartVibraL(0, KHWRMVibraMinIntensity-1);
	}
	
void CHWRMTest::VibraOnWithDurationOutOfRangeL()
	{
	INFO_PRINTF1(_L("Step state: VibraOnWithDurationOutOfRangeL"));
		
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
		
	// read the vibra attributes from the central repository
	GetVibraTimeAttributesL();	

	ExpectLeave(KErrArgument);
	vibra.StartVibraL(KHWRMVibraMaxDuration + 1, KHWRMVibraMaxIntensity);
	}

void CHWRMTest::VibraOnWithDurationOutOfRange2L()
	{
	INFO_PRINTF1(_L("Step state: VibraOnWithDurationOutOfRange2L"));
		
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
		
	// read the vibra attributes from the central repository
	GetVibraTimeAttributesL();	

	ExpectLeave(KErrArgument);
	vibra.StartVibraL(-1, KHWRMVibraMinIntensity);
	}

void CHWRMTest::VibraOnWithDurationOutOfRange3L()
	{
	INFO_PRINTF1(_L("Step state: VibraOnWithDurationOutOfRange3L"));
		
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
		
	// read the vibra attributes from the central repository
	GetVibraTimeAttributesL();	

	ExpectLeave(KErrArgument);
	vibra.StartVibraL(KHWRMVibraMaxDuration + 1);
	}

void CHWRMTest::VibraOnWithDurationOutOfRange4L()
	{
	INFO_PRINTF1(_L("Step state: VibraOnWithDurationOutOfRange4L"));
		
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
		
	// read the vibra attributes from the central repository
	GetVibraTimeAttributesL();	

	ExpectLeave(KErrArgument);
	vibra.StartVibraL(-1);
	}

void CHWRMTest::VibraOnWithDurationGreaterThanMaximumL()
	{
	
	//attempt to start vibra with a duration > KVibraCtrlMaxTime but < KHWRMVibraMaxDuration
	INFO_PRINTF1(_L("Step state: VibraOnWithDurationGreaterThanMaximumL"));	

	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();

	// read the vibra attributes from the central repository
	GetVibraTimeAttributesL();
	
	//must wait for the max time and check Vibra is stopped automatically
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusOn);
	vibra.StartVibraL(iVibraMaxOnTime + 1);
	}
	
void CHWRMTest::VibraOnDuringForcedOffPeriodL()
	{
	INFO_PRINTF1(_L("Step state: VibraOnDuringForcedOffPeriodL"));
	
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();

	ExpectLeave(KErrLocked);
	vibra.StartVibraL(iVibraMaxOnTime/2);
	}

void CHWRMTest::VibraSetMinTimeOffL()
	{
	INFO_PRINTF1(_L("Step state: VibraSetMinTimeOffL"));
	SetCRVibraAttributeL(KVibraMinOffTime,5000); // 5 seconds
	GetCRVibraAttributeL(KVibraMinOffTime,iVibraMinOffTime);
	}

void CHWRMTest::VibraRestoreDefaultMinTimeOffL()
	{
	INFO_PRINTF1(_L("Step state: VibraRestoreDefaultMinTimeOffL"));	
	SetCRVibraAttributeL(KVibraMinOffTime,KDefaultVibraMinOffTime);
	SetCRVibraAttributeL(KVibraMinOffTime,iVibraMinOffTime);
	}

void CHWRMTest::VibraSetMaxTime10SecsOnL()
	{
	INFO_PRINTF1(_L("Step state: VibraSetMaxTimeOnL"));
	SetCRVibraAttributeL(KVibraCtrlMaxTime,10000); // 10 seconds
	GetCRVibraAttributeL(KVibraCtrlMaxTime,iVibraMaxOnTime);	
	}

void CHWRMTest::VibraSetMaxTime5SecsOnL()
	{
	INFO_PRINTF1(_L("Step state: VibraSetMaxTimeOnL"));
	SetCRVibraAttributeL(KVibraCtrlMaxTime,5000); // 5 seconds	
	GetCRVibraAttributeL(KVibraCtrlMaxTime,iVibraMaxOnTime);
	
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped); // ???
	}

void CHWRMTest::VibraGetStatusOnL()
	{
	INFO_PRINTF1(_L("Step state: VibraGetStatusOnL"));
	
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	
	CHWRMVibra::TVibraStatus status = vibra.VibraStatus();
	if (status != CHWRMVibra::EVibraStatusOn)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::VibraGetStatusOffL()
	{
	INFO_PRINTF1(_L("Step state: VibraGetStatusOffL"));
	
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	
	CHWRMVibra::TVibraStatus status = vibra.VibraStatus();
	if (status != CHWRMVibra::EVibraStatusStopped)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::VibraOn5SecsL()
	{
	INFO_PRINTF1(_L("Step state: VibraOn5SecsL"));	
	
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();	
	
	vibra.StartVibraL(iVibraMaxOnTime/5000); //5 secs
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusOn);
	User::After(KOneSecond);
	}

void CHWRMTest::WaitOneSecondL()
	{
	INFO_PRINTF1(_L("Step state: WaitOneSecondL"));	
	User::After(KOneSecond);	
	}


void CHWRMTest::WaitOneTenthL()
	{
	INFO_PRINTF1(_L("Step state: WaitOneTenthL"));	
	User::After(KOneSecond/10);	
	}
	
void CHWRMTest::VibraWaitForCutOffL()
	{
	INFO_PRINTF1(_L("Step state: VibraWaitForCutOffL"));
	
	GetCRVibraAttributeL(KVibraCtrlMaxTime, iVibraMaxOnTime);
	INFO_PRINTF2(_L("Waiting for cut off [iVibraMaxOnTime=%d]"),iVibraMaxOnTime);
	
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped);
	User::After(iVibraMaxOnTime);	
	}

void CHWRMTest::VibraWaitForCoolOffL()
	{
	INFO_PRINTF1(_L("Step state: VibraWaitForCoolOffL"));
	
	GetCRVibraAttributeL(KVibraMinOffTime, iVibraMinOffTime);	
	INFO_PRINTF2(_L("Waiting for cool off [iVibraMinOffTime=%d]"),iVibraMinOffTime);
	
	User::After(iVibraMinOffTime);
	}	

void CHWRMTest::VibraOnInfiniteL()
	{
	INFO_PRINTF1(_L("Step state: VibraOnInfiniteL"));	
	
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();	
	
	vibra.StartVibraL(0);
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusOn);
	}
	
void CHWRMTest::VibraDisableProfileL()
	{
	INFO_PRINTF1(_L("Step state: VibraDisableProfileL"));
	
	TInt value;
	SetCRVibraAttributeL(KVibraCtrlProfileVibraEnabled, 0);
	GetCRVibraAttributeL(KVibraCtrlProfileVibraEnabled, value);
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusNotAllowed);
	ExpectVibraModeNotificationL(CHWRMVibra::EVibraModeOFF);
	}
	
void CHWRMTest::VibraPulseDisableProfileL()
	{
	INFO_PRINTF1(_L("Step state: VibraPulseDisableProfileL"));
	
	TInt value;
	SetCRVibraAttributeL(KVibraCtrlProfileFeedbackEnabled, 0);
	GetCRVibraAttributeL(KVibraCtrlProfileFeedbackEnabled, value);
	//ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusNotAllowed);
	ExpectVibraFeedbackModeNotificationL(CHWRMVibra::EVibraFeedbackModeOFF);
	}	
		
void CHWRMTest::VibraPrivilegedDisableProfileL()
	{
	INFO_PRINTF1(_L("Step state: VibraPrivilegedDisableProfileL"));
	
	TInt value;
	SetCRVibraAttributeL(KVibraCtrlProfileVibraEnabled, 0);
	GetCRVibraAttributeL(KVibraCtrlProfileVibraEnabled, value);	
	ExpectVibraModeNotificationL(CHWRMVibra::EVibraModeOFF);	
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusNotAllowed);
	}	
	
void CHWRMTest::VibraOnWhileDisabledL()
	{
	INFO_PRINTF1(_L("Step state: VibraOnWhileDisabledL"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	
	ExpectLeave(KErrAccessDenied);
	vibra.StartVibraL(iVibraMaxOnTime/10);	
	}
	
	
void CHWRMTest::PulseOnWhileDisabledL()
	{
	INFO_PRINTF1(_L("Step state: PulseOnWhileDisabledL"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	
	ExpectLeave(KErrAccessDenied);
	vibra.PulseVibraL(1000, 50);	
	}	

void CHWRMTest::VibraEnableProfileL()
	{
	INFO_PRINTF1(_L("Step state: VibraEnableProfileL"));
	SetCRVibraAttributeL(KVibraCtrlProfileVibraEnabled, 1);	
	ExpectVibraModeNotificationL(CHWRMVibra::EVibraModeON);
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped);
	}

void CHWRMTest::VibraPulseEnableProfileL()
	{
	INFO_PRINTF1(_L("Step state: VibraPulseEnableProfileL"));
	
	SetCRVibraAttributeL(KVibraCtrlProfileFeedbackEnabled, 1);	
	ExpectVibraFeedbackModeNotificationL(CHWRMVibra::EVibraFeedbackModeON);
	//ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped);
	}
	
void CHWRMTest::StartVibra5L()
	{
	INFO_PRINTF1(_L("Step state: StartVibra5L"));
	
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusOn);
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();			
	vibra.StartVibraL(5000);		
	}
	
void CHWRMTest::PulseVibraWithNoStatusUpdateL()
	{
	INFO_PRINTF1(_L("Step state: PulseVibraWithNoStatusUpdateL"));
	
	//Having already issued a start vibra request in previous test 
	//state: StartVibra5L, no vibra status update is expected in this 
	//test state - if a status update is received, this test state will fail.			
	CHWRMVibra& vibra = GetVibraSessionL(1);
	DoPulseVibraL(vibra);

	//a Vibra status update of VibraStatusStopped should be received in the 
	//next test state: CheckForVibraStatusStoppedL
	}		

void CHWRMTest::CheckForVibraStatusStoppedL()
	{
	INFO_PRINTF1(_L("Step state: CheckForVibraStatusStoppedL"));
	
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped);		
	}	
	
void CHWRMTest::PulseVibraWithNoStatusUpdate0L()
	{
	INFO_PRINTF1(_L("Step state: PulseVibraWithNoStatusUpdate0L"));
	
	//Having already issued a start vibra request in previous test 
	//state: StartVibra5L, no vibra status update is expected in this 
	//test state - if a status update is received, this test state will fail.			
	CHWRMVibra& vibra = GetVibraSessionL(0);
	DoPulseVibraL(vibra);

	//a Vibra status update of VibraStatusStopped should be received in the 
	//next test state: CheckForVibraStatusStoppedL
	}		

		
void CHWRMTest::VibraOnLoopL()
	{
	INFO_PRINTF1(_L("Step state: VibraOnLoopL"));
	
	// get the vibra object
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();	
	
	vibra.StartVibraL(iVibraMaxOnTime/2);	
	}
	
	
void CHWRMTest::LightOpenL()
	{
	INFO_PRINTF1(_L("Step state: LightOpenL"));	
	
	// This is meant as a step initialisation state. Opening a light session will produce
	// several light status change notifications depending on the previous status of the targets.
	// Light status change notification checking should be switched off before running this test state
	// and it should be switched back on before the "real" testing begins.
	
	OpenSessionsL(1,0);
   	}

void CHWRMTest::LightOpenSessionsL()
	{
	INFO_PRINTF1(_L("Step state: LightOpenSessionsL"));

	// This is meant as a step initialisation state. Opening a light session will produce
	// several light status change notifications depending on the previous status of the targets.
	// Light status change notification checking should be switched off before running this test state
	// and it should be switched back on before the "real" testing begins.

	OpenSessionsL(3,0);	// three light sessions
	}
	
void CHWRMTest::LightOnDisplayAndKeyboard_LimitedDurationL()
	{
	INFO_PRINTF1(_L("Step state: LightOnDisplayAndKeyboard_LimitedDurationL"));
	
	SetCRLightAttributes(KDisplayLightsTimeout, 5);//Removes the dependency on test cases 16 
									//(TestLightBlinkKeyboardInfiniteCutOffTimerL) and 18(TestLightReserveSessionsL)	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	
	//As part of PREQ2000 mulitiple targeted ExpectLightStatusNotificationL() calls must be broken into
	//individual target ExpectLightStatusNotificationL() calls
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOn);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOn);
	light.LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, 60000);
	}
	
void CHWRMTest::LightOffDisplayAndKeyboardL()
	{
	INFO_PRINTF1(_L("Step state: LightOffDisplayAndKeyboardL"));

	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL(); 
	//As part of PREQ2000 mulitiple targeted ExpectLightStatusNotificationL() calls must be broken into
	//individual target ExpectLightStatusNotificationL() calls
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff);		
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOff);		
	light.LightOffL(CHWRMLight::EPrimaryDisplayAndKeyboard);
	}
		
void CHWRMTest::LightOnAllTargetsL()
	{
	INFO_PRINTF1(_L("Step state: LightOnAllTargetsL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(KSupportedTargets, CHWRMLight::ELightOn);
	light.LightOnL(KSupportedTargets, 5000);
	}
	
void CHWRMTest::LightOffAllTargetsL()
	{
	INFO_PRINTF1(_L("Step state: LightOffAllTargetsL"));

    // prerequisite: light session has already been opened
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL(); 
	//As part of PREQ2000 mulitiple targeted ExpectLightStatusNotificationL() calls must be broken into
	//individual target ExpectLightStatusNotificationL() calls
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOff);
	ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplay, CHWRMLight::ELightOff);
	ExpectLightStatusNotificationL(CHWRMLight::ESecondaryKeyboard, CHWRMLight::ELightOff);
	ExpectLightStatusNotificationL(CHWRMLight::ECustomTarget2, CHWRMLight::ELightOff);
	ExpectLightStatusNotificationL(CHWRMLight::ECustomTarget4, CHWRMLight::ELightOff);	
	light.LightOffL(KSupportedTargets);
	
	}
	
void CHWRMTest::LightBlinkKeyboardWithLimitedDurationL()
	{
	INFO_PRINTF1(_L("Step state: LightBlinkKeyboard_LimitedDurationL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL(); 
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightBlink);  
	iLimitedDuration = 4*KOneSecond;
	iTimedTarget = CHWRMLight::EPrimaryKeyboard;
	light.LightBlinkL(CHWRMLight::EPrimaryKeyboard, iLimitedDuration/1000, 300, 400, 40);                      
	}

void CHWRMTest::InitialiseLightsL()
	{
	INFO_PRINTF1(_L("Step state: InitialiseLightL"));
	
	// This test state should only be run immediately after the HWRM server has started. Its
	// purpose is to open a light session for the first time.	
	OpenSessionsL(1,0);	
	}
	
void CHWRMTest::LightOnDisplayAndKeyboardL()
	{
	INFO_PRINTF1(_L("Step state: LightOnDisplayAndKeyboardL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	//As part of PREQ2000 mulitiple targeted ExpectLightStatusNotificationL() calls must be broken into
	//individual target ExpectLightStatusNotificationL() calls
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOn);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOn);
	light.LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, 0, 50, EFalse);

	}

void CHWRMTest::LightOnDisplayAndKeyboard_NoStatusChangeL()
	{
	INFO_PRINTF1(_L("Step state: LightOnDisplayAndKeyboard_NoStatusChangeL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	light.LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard);
	}
	
void CHWRMTest::LightBlinkDisplayWithLimitedDurationL()
	{
	INFO_PRINTF1(_L("Step state: LightBlinkDisplay_LimitedDurationL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightBlink);
	light.LightBlinkL(CHWRMLight::EPrimaryDisplay, 3*KOneSecond/1000); // 3s
	}

void CHWRMTest::LightBlinkDisplayAndKeyboardWithLimitedDuration_KeyboardNoChangeL()
	{
	INFO_PRINTF1(_L("Step state: LightBlinkDisplayAndKeyboardWithLimitedDuration_KeyboardNoChangeL"));
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightBlink);
	iLimitedDuration = 2*KOneSecond;
	iTimedTargets.Reset();
	const TInt bitSet = 1;

	TInt currentTarget = 1;
	for (TInt i=0; i<2; i++ )
		if(currentTarget & CHWRMLight::EPrimaryDisplayAndKeyboard)
			{
			iTimedTargets.AppendL(bitSet);
			currentTarget = currentTarget << 1;
			}
	light.LightBlinkL(CHWRMLight::EPrimaryDisplayAndKeyboard, 2*KOneSecond/1000, 500, 550, 60); // 2s
	}
	
void CHWRMTest::LightOffKeyboardWithLimitedDurationL()
	{
	INFO_PRINTF1(_L("Step state: LightOffKeyboardWithLtdDurationL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOff);		
	light.LightOffL(CHWRMLight::EPrimaryKeyboard, 3*KOneSecond/1000, ETrue); // 3s
	}

void CHWRMTest::LightOffDisplayWithOverrideTimerL()
	{
	INFO_PRINTF1(_L("Step state: LightOffDisplayWithOverrideTimerL"));

	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff);		
	light.LightOffL(CHWRMLight::EPrimaryDisplay, 1.5*KOneSecond/1000, ETrue); // 1.5 s
	}

void CHWRMTest::LightBlinkKeyboardWithOverrideTimerL() // case 57
	{
	INFO_PRINTF1(_L("Step state: LightBlinkKeyboardWithOverrideTimerL"));	
	const TInt bitSet = 1;
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightBlink);

	// we'll need to wait for display to go on after this state
	iLimitedDuration = 2.5*KOneSecond;
	iTimedTargets.Reset();
	iTimedTargets.Append(bitSet);
	light.LightBlinkL(CHWRMLight::EPrimaryKeyboard, 5000, 500, 400, 60);
	}
	

void CHWRMTest::LightBlinkKeyboardInfiniteCutOffTimerL()
	{
	INFO_PRINTF1(_L("Step state: LightBlinkKeyboardInfiniteCutOffTimerL"));

	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightBlink);
	light.LightBlinkL(CHWRMLight::EPrimaryKeyboard);
	}

void CHWRMTest::LightGetStatusKeyboardL()
	{
	INFO_PRINTF1(_L("Step state: LightGetStatusKeyboardL"));

	// get the light object
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	CHWRMLight::TLightStatus status = light.LightStatus(CHWRMLight::EPrimaryKeyboard);
	
	if (status != CHWRMLight::ELightBlink)
		{
		User::Leave(KErrGeneral);
		}
	}
	
void CHWRMTest::LightGetStatusDisplayL()
	{
	INFO_PRINTF1(_L("Step state: LightGetStatusDisplayL"));

	// get the light object
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	CHWRMLight::TLightStatus status = light.LightStatus(CHWRMLight::EPrimaryDisplay);
	
	if (status != CHWRMLight::ELightOn)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::LightSetSensorOnL()
	{
	INFO_PRINTF1(_L("Step state: LightSetSensorOnL"));
	
	User::ResetInactivityTime();
	
	// system targets will be switched on.
	// Expect change notification only on those that are not already ON
	TInt onTargets(0);
	GetTargetsOn(onTargets); // this gives us which targets are already ON
	//As part of PREQ2000 mulitiple targeted ExpectLightStatusNotificationL() calls must be broken into
	//individual target ExpectLightStatusNotificationL() calls
	TInt expectedTargets = KSystemTargets & ~onTargets;
	TInt whichTarget = 1;	
	for(TInt i=0; i<KSupportedTargets; i++)
		{
		if (whichTarget & expectedTargets)
			{
			ExpectLightStatusNotificationL(whichTarget, CHWRMLight::ELightOn);
			}
		whichTarget = whichTarget << 1;
		} 

	SetCRLightAttributes(KDisplayLightsTimeout, 60);
	SetLightControlAttributes(20, 40, 1);
	}

void CHWRMTest::LightGetSensorOnL()
	{
	INFO_PRINTF1(_L("Step state: LightGetSensorOnL"));

	TInt value;
	GetCRLightAttributes(KLightIntensity, value);
	if (value != 20)
		{
		User::Leave(KErrGeneral);
		}
	
	GetCRLightAttributes(KLightSensorSensitivity, value);
	if (value != 40)
		{
		User::Leave(KErrGeneral);
		}		
	}
	
void CHWRMTest::LightSetSensorOffL()
	{
	INFO_PRINTF1(_L("Step state: LightSetSensorOffL"));
	SetLightControlAttributes(20, 50, 0);

	TInt value;
	GetCRLightAttributes(KLightIntensity, value);
	if (value != 20)
		{
		User::Leave(KErrGeneral);
		}
	
	GetCRLightAttributes(KLightSensorSensitivity, value);
	if (value != 50)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::LightBlinkWithDefaultsL()
	{
	INFO_PRINTF1(_L("Step state: LightBlinkWithDefaultsL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	//As part of PREQ2000 mulitiple targeted ExpectLightStatusNotificationL() calls must be broken into
	//individual target ExpectLightStatusNotificationL() calls
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightBlink);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightBlink);
	light.LightBlinkL(CHWRMLight::EPrimaryDisplayAndKeyboard);
	}
	
void CHWRMTest::ResetIntensityAndLightTimeOutL()
	{
	INFO_PRINTF1(_L("Step state: ResetIntensityAndLightTimeOut"));
	
	SetCRLightAttributes(KDisplayLightsTimeout, 5);

	// wait 5 seconds to ensure inactivity timeout
    User::After(5*KOneSecond);
    
    // system targets will not be switched on because sensor is now off (LightSetSensorOffL)
	SetLightControlAttributes(50, 50, 0);
	}

void CHWRMTest::LightBlinkCycleDurationDefaultMismatchL()
	{
	INFO_PRINTF1(_L("Step state: LightBlinkCycleDurationDefaultMismatchL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLeave(KErrArgument);	
	light.LightBlinkL(CHWRMLight::EPrimaryDisplayAndKeyboard, 2000, KHWRMDefaultCycleTime, 700, 100);
	User::ResetInactivityTime();
	}

void CHWRMTest::VibraOpenSessionsL()
	{
	INFO_PRINTF1(_L("Step state: VibraOpenSessionsL"));
	
	ExpectVibraModeNotificationL(CHWRMVibra::EVibraModeON);	
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped);
	
	// three vibra sessions
	OpenSessionsL(0,3);	
	}
	
void CHWRMTest::VibraReserveL()
	{
	INFO_PRINTF1(_L("Step state: VibraReserveL"));
	
	CHWRMVibra& vibra = GetVibraSessionL(0);
	vibra.ReserveVibraL(EFalse, ETrue);
	}
	
	
void CHWRMTest::VibraReserveDefaultParamsL()
	{
	INFO_PRINTF1(_L("Step state: VibraReserveDefaultParamsL"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();

	// trying to reserve a resource with default params without a CONE environment
	// causes a KErrNotReady error.
	ExpectLeave(KErrNotReady);
	vibra.ReserveVibraL();
	}

void CHWRMTest::VibraTryStartL()
	{
	INFO_PRINTF1(_L("Step state: VibraTryStartL"));
	
	CHWRMVibra& vibra = GetVibraSessionL(0);
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusOn);
	vibra.StartVibraL(1000);	
	}

void CHWRMTest::Vibra2TryStartL()
	{
	INFO_PRINTF1(_L("Step state: Vibra2TryStartL"));
	
	CHWRMVibra& vibra2 = GetVibraSessionL(1);
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusStopped);
	ExpectLeave(KErrInUse);	
	vibra2.StartVibraL(1000);
	}

void CHWRMTest::Vibra2ReserveL()
	{
	INFO_PRINTF1(_L("Step state: Vibra2ReserveL"));
	
	CHWRMVibra& vibra2 = GetVibraSessionL(1);
	vibra2.ReserveVibraL(EFalse, ETrue);
	}

void CHWRMTest::Vibra3ReserveL()
	{
	INFO_PRINTF1(_L("Step state: Vibra3ReserveL"));
	
	CHWRMVibra& vibra3 = GetVibraSessionL(2);
	vibra3.ReserveVibraL(EFalse, ETrue);
	}

void CHWRMTest::VibraReleaseL()
	{
	INFO_PRINTF1(_L("Step state: VibraReleaseL"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();	
	vibra.ReleaseVibra();
	}

void CHWRMTest::Vibra2ReleaseL()
	{
	INFO_PRINTF1(_L("Step state: Vibra2ReleaseL"));
	
	CHWRMVibra& vibra = GetVibraSessionL(1);	
	vibra.ReleaseVibra();
	}

void CHWRMTest::LightReserveDisplayL()
	{
	INFO_PRINTF1(_L("Step state: LightReserveDisplayL"));	
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	light.ReserveLightL(CHWRMLight::EPrimaryDisplay, EFalse, ETrue);
	}
	
void CHWRMTest::LightOnDisplayL()
	{
	INFO_PRINTF1(_L("Step state: LightOnDisplayL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOn);
	light.LightOnL(CHWRMLight::EPrimaryDisplay);	
	}

void CHWRMTest::LightOnDisplay_NoStatusChangeL()
	{
	INFO_PRINTF1(_L("Step state: LightOnDisplay_NoStatusChangeL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	light.LightOnL(CHWRMLight::EPrimaryDisplay);
	}
	
void CHWRMTest::LightOnDisplay_LimitedDurationL()
	{
	INFO_PRINTF1(_L("Step state: LightOnDisplayLimitedDurationL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOn);
	iTimedTarget = CHWRMLight::EPrimaryDisplay;
	iLimitedDuration = 5*KOneSecond;
	light.LightOnL(CHWRMLight::EPrimaryDisplay,5*KOneSecond/1000); // hwrm server takes ms
	}

void CHWRMTest::WaitForTargetToGoOffL()
	{
	INFO_PRINTF3(_L("Step state: WaitForTargetToGoOffL - iTimedTarget=%d, iLimitedDuration=%d"),iTimedTarget,iLimitedDuration);
	ExpectLightStatusNotificationL(iTimedTarget, CHWRMLight::ELightOff);
	User::After(iLimitedDuration + KOneSecond);
	}

void CHWRMTest::WaitForTargetToGoOnL()
	{
	INFO_PRINTF1(_L("Step state: WaitForTargetToGoOnL"));
	//As part of PREQ2000 mulitiple targeted ExpectLightStatusNotificationL() calls must be broken into
	//individual target ExpectLightStatusNotificationL() calls
	TInt targetsUsed = iTimedTargets.Count();
	TInt initialTarget = 1;
	for(TInt i=0; i<targetsUsed; i++ )
		{
		TInt ActualTarget = iTimedTargets[i]*initialTarget;
		if(ActualTarget != 0)
			{
			ExpectLightStatusNotificationL(ActualTarget, CHWRMLight::ELightOn);
			INFO_PRINTF3(_L("Step state: WaitForTargetToGoOnL - iTimedTarget=%d, iLimitedDuration=%d"),iTimedTargets[i],iLimitedDuration);
			}		
		initialTarget = initialTarget << 1;
		}
	
	User::After(iLimitedDuration + KOneSecond);
	for(TInt i=targetsUsed; i>0; i--)
		{
		iTimedTargets.Remove(i-1);
		}	
	iTimedTargets.Reset();
	}
	
void CHWRMTest::LightOffDisplayL()
	{
	INFO_PRINTF1(_L("Step state: LightOffDisplayL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff);
	light.LightOffL(CHWRMLight::EPrimaryDisplay);	
	}

void CHWRMTest::LightOnKeyboardL()
	{
	INFO_PRINTF1(_L("Step state: LightOnKeyboardL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();

	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOn);
	light.LightOnL(CHWRMLight::EPrimaryKeyboard);	
	}

void CHWRMTest::LightOffKeyboardL()
	{
	INFO_PRINTF1(_L("Step state: LightOffKeyboardL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOff);
	light.LightOffL(CHWRMLight::EPrimaryKeyboard);
	}

void CHWRMTest::LightReleaseReservedDisplayL()
	{
	INFO_PRINTF1(_L("Step state: LightReleaseReservedDisplayL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	// Should return to default state
    ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff);
	light.ReleaseLight(CHWRMLight::EPrimaryDisplay);	
	}

void CHWRMTest::LightReleaseUnreservedKeyboardL()
	{
	INFO_PRINTF1(_L("Step state: LightReleaseUnreservedKeyboardL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	//unreserved release, so no change should occur from last notification
	light.ReleaseLight(CHWRMLight::EPrimaryKeyboard);	
	}

void CHWRMTest::Light2ReserveKeyboardL()
	{
	INFO_PRINTF1(_L("Step state: Light2ReserveKeyboardL"));	
	
	SetCRLightAttributes(KDisplayLightsTimeout, 5);	//Removes the dependency on test cases 16 
									//(TestLightBlinkKeyboardInfiniteCutOffTimerL) and 18(TestLightReserveSessionsL)
	CHWRMLight& light2 = GetLightSessionL(1);
	light2.ReserveLightL(CHWRMLight::EPrimaryKeyboard, EFalse, ETrue);
	}

void CHWRMTest::Light3ReserveDisplayAndKeyboardL()
	{
	// suspended reservation
	INFO_PRINTF1(_L("Step state: Light3ReserveDisplayAndKeyboardL"));
		
	CHWRMLight& light3 = GetLightSessionL(2);
	light3.ReserveLightL(CHWRMLight::EPrimaryDisplayAndKeyboard, EFalse, ETrue);
	}

void CHWRMTest::LightTryBlinkDisplayAndKeyboardL()
	{
	// suspended reservation
	INFO_PRINTF1(_L("Step state: LightTryBlinkDisplayAndKeyboardL"));	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLeave(KErrInUse);	
	light.LightBlinkL(CHWRMLight::EPrimaryDisplayAndKeyboard);
	}

void CHWRMTest::Light2OnDisplayAndKeyboardL()
	{
	// suspended reservation
	INFO_PRINTF1(_L("Step state: Light2OnDisplayAndKeyboardL"));
		
	CHWRMLight& light2 = GetLightSessionL(1);
	ExpectLeave(KErrInUse);	
	light2.LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard);
	}

void CHWRMTest::Light3TryBlinkDisplayAndKeyboardL()
	{
	INFO_PRINTF1(_L("Step state: Light3TryBlinkDisplayAndKeyboardL"));	
	
	CHWRMLight& light3 =GetLightSessionL(2);
	light3.LightBlinkL(CHWRMLight::EPrimaryDisplayAndKeyboard);
	}

void CHWRMTest::Light2ReleaseUnreservedDisplayAndReservedKeyboardL()
	{
	INFO_PRINTF1(_L("Step state: Light2ReleaseUnreservedDisplayAndReservedKeyboardL"));
	
	//only keyboard reserved. light 3 should gain active control of keyboard
	CHWRMLight& light2 = GetLightSessionL(1);

    // default state for unreserved target expected, should already be off 
    // (so no notifications should be received for the display)
 
	// virtual state of light 3 to be restored (i.e. lights blink)
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightBlink);
	light2.ReleaseLight(CHWRMLight::EPrimaryDisplayAndKeyboard);	
	}

void CHWRMTest::LightTryBlinkDisplayL()
	{
	INFO_PRINTF1(_L("Step state: LightTryBlinkDisplayL"));
	
	CHWRMLight& light = GetLightSessionL(0);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightBlink);
	light.LightBlinkL(CHWRMLight::EPrimaryDisplay);	
	}

void CHWRMTest::Light3OnKeyboardL()
	{
	//got active reservation
	INFO_PRINTF1(_L("Step state: Light3OnKeyboardL"));	
	
	CHWRMLight& light3 = GetLightSessionL(2);
    // keyboard set to blink in LightTryBlinkDisplayL
    ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOn);
	light3.LightOnL(CHWRMLight::EPrimaryKeyboard);
	}

void CHWRMTest::Light3ReleaseSuspendedDisplayAndReservedKeyboardL()
	{
	//both reserved, though display suspended. Keyboard should return to default state
	INFO_PRINTF1(_L("Step state: Light3ReleaseSuspendedDisplayAndReservedKeyboardL"));
	
	CHWRMLight& light3 = GetLightSessionL(2);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOff);
	light3.ReleaseLight(CHWRMLight::EPrimaryDisplayAndKeyboard);	
	}

void CHWRMTest::LightReleaseReservedDisplayAndUnreservedKeyboardL()
	{
	//only display reserved. Display should return to default state
	INFO_PRINTF1(_L("Step state: LightReleaseReservedDisplayAndUnreservedKeyboardL"));
	
	CHWRMLight& light = GetLightSessionL(0);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff);
	light.ReleaseLight(CHWRMLight::EPrimaryDisplayAndKeyboard);	
	}

void CHWRMTest::LightReserveDisplayAndKeyboardL()
	{
	INFO_PRINTF1(_L("Step state: LightReserveDisplayAndKeyboardL"));
	
	CHWRMLight& light = GetLightSessionL(0);
	light.ReserveLightL(CHWRMLight::EPrimaryDisplay | CHWRMLight::EPrimaryKeyboard, EFalse, ETrue);	
	}

void CHWRMTest::LightGetSupportedTargetsL()
	{
	INFO_PRINTF1(_L("Step state: LightGetSupportedTargetsL"));

	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	
	TInt supported;
	supported = light.SupportedTargets();
	if (supported != KSupportedTargets)
		{
		User::Leave(KErrGeneral);
		}                             
	}

void CHWRMTest::LightReserveUnsupportedTargetL()
	{
	INFO_PRINTF1(_L("Step state: LightReserveUnsupportedTargetL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLeave(KErrNotSupported);
	light.ReserveLightL(CHWRMLight::EPrimaryDisplayAndKeyboard | CHWRMLight::ECustomTarget1, EFalse, ETrue);
	}

void CHWRMTest::LightOnUnsupportedTargetL()
	{
	INFO_PRINTF1(_L("Step state: LightOnUnsupportedTargetL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLeave(KErrNotSupported);
	light.LightOnL(CHWRMLight::ECustomTarget1);	
	}

void CHWRMTest::LightReleaseUnsupportedTargetL()
	{
	INFO_PRINTF1(_L("Step state: LightReleaseUnsupportedTargetL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	light.ReleaseLight(CHWRMLight::ECustomTarget1);
	}

void CHWRMTest::LightOnSimpleCustom2L()
	{
	INFO_PRINTF1(_L("Step state: LightOnSimpleCustom2L"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::ECustomTarget2, CHWRMLight::ELightOn);	
	light.LightOnL(CHWRMLight::ECustomTarget2, 5000);
	}

void CHWRMTest::LightOnSimpleCustom1L()
	{
	INFO_PRINTF1(_L("Step state: LightOnSimpleCustom1L"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLeave(KErrNotSupported);	
	light.LightOnL(CHWRMLight::ECustomTarget1);
	}

void CHWRMTest::LightOnSimpleCustom4L()
	{
	INFO_PRINTF1(_L("Step state: LightOnSimpleCustom4L"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	ExpectLightStatusNotificationL(CHWRMLight::ECustomTarget4, CHWRMLight::ELightOn);	
	light.LightOnL(CHWRMLight::ECustomTarget4, 5000);
	}

void CHWRMTest::LightReserveDefaultParamsL()
	{
	INFO_PRINTF1(_L("Step state: LightReserveDefaultParamsL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	
	// trying to reserve a resource with default params without a CONE environment
	// causes a KErrNotReady error.
	ExpectLeave(KErrNotReady);
	light.ReserveLightL(CHWRMLight::EPrimaryDisplay);
	}


void CHWRMTest::LightOnUnsupportedDurationUpperBoundL()
	{
	INFO_PRINTF1(_L("Step state: LightOnUnsupportedDurationUpperBoundL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();

	ExpectLeave(KErrArgument);
	
	light.LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, KHWRMLightMaxDuration + 1, 50, EFalse);
	}

void CHWRMTest::LightOnUnsupportedDurationLowerBoundL()
	{
	INFO_PRINTF1(_L("Step state: LightOnUnsupportedDurationLowerBoundL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();

	ExpectLeave(KErrArgument);
	
	light.LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, -1, 50, EFalse);
	}

void CHWRMTest::LightOnUnsupportedIntensityUpperBoundL()
	{
	INFO_PRINTF1(_L("Step state: LightOnUnsupportedIntensityUpperBoundL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();

	ExpectLeave(KErrArgument);
	
	light.LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, 0, KHWRMLightMaxIntensity + 1, EFalse);
	}

void CHWRMTest::LightOnUnsupportedIntensityLowerBoundL()
	{
	INFO_PRINTF1(_L("Step state: LightOnUnsupportedIntensityLowerBoundL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();

	ExpectLeave(KErrArgument);
	
	// KHWRMLightMinIntensity = 1 and KHWRMDefaultIntensity = 0 
	// so test KHWRMDefaultIntensity - 2
	light.LightOnL(CHWRMLight::EPrimaryDisplayAndKeyboard, 0, KHWRMLightMinIntensity -2 , EFalse);
	}
	
void CHWRMTest::VibraPrivilegedStartDisabled1L()
	{
	//StartVibraL while disabled - 1, privileged client
	INFO_PRINTF1(_L("Step state: VibraPrivilegedStartDisabled1L"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusOn);
	vibra.StartVibraL(iVibraMaxOnTime/2);
	}

void CHWRMTest::VibraPrivilegedStartDisabled2L()
	{
	//StartVibraL while disabled - 2, privileged client
	INFO_PRINTF1(_L("Step state: VibraPrivilegedStartDisabled2L"));
	
	CHWRMVibra& vibra = GetVibraSessionWithCallbackRegisteredL();
	ExpectVibraStatusNotificationL(CHWRMVibra::EVibraStatusOn);	
	vibra.StartVibraL(iVibraMaxOnTime/2, KHWRMVibraMinIntensity);
	}

void CHWRMTest::OpenFlipL()
	{
	INFO_PRINTF1(_L("Step state: OpenFlipL"));
	
	// Set flags	
    TInt err = RProperty::Set(KPSUidHWRM, KHWRMFlipStatus, EPSHWRMFlipOpen);
    User::LeaveIfError(err);
	}

void CHWRMTest::CloseFlipL()
	{
	INFO_PRINTF1(_L("Step state: CloseFlipL"));
	
	// Set flags	
    TInt err = RProperty::Set(KPSUidHWRM, KHWRMFlipStatus, EPSHWRMFlipClosed);
    User::LeaveIfError(err);
	}

void CHWRMTest::SystemLightsOnL()
	{
	INFO_PRINTF1(_L("Step state: SystemLightsOnL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	
	//As part of PREQ2000 mulitiple targeted ExpectLightStatusNotificationL() calls must be broken into
	//individual target ExpectLightStatusNotificationL() calls
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOn);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOn);
	ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplay, CHWRMLight::ELightOn);
	ExpectLightStatusNotificationL(CHWRMLight::ESecondaryKeyboard, CHWRMLight::ELightOn);
	
	light.LightOnL(CHWRMLight::ESystemTarget, 0);
	}

void CHWRMTest::SystemLightsOffL()
	{
	INFO_PRINTF1(_L("Step state: SystemLightsOffL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	
	//As part of PREQ2000 mulitiple targeted ExpectLightStatusNotificationL() calls must be broken into
	//individual target ExpectLightStatusNotificationL() calls
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff);
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOff);
	ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplay, CHWRMLight::ELightOff);
	ExpectLightStatusNotificationL(CHWRMLight::ESecondaryKeyboard, CHWRMLight::ELightOff);
	light.LightOffL(CHWRMLight::ESystemTarget, 0);
	}

void CHWRMTest::DisplayLightsOnL()
	{
	INFO_PRINTF1(_L("Step state: DisplayLightsOnL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOn);
	light.LightOnL(CHWRMLight::EPrimaryDisplay, 0);
	}

void CHWRMTest::KeyboardLightsOnDisplayOffL()
	{
	INFO_PRINTF1(_L("Step state: KeyboardLightsOnDisplayOffL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff);
	light.LightOffL(CHWRMLight::EPrimaryDisplay, 0);
	
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOn);
	light.LightOnL(CHWRMLight::EPrimaryKeyboard, 0);
	}

void CHWRMTest::ExpectSetColorCommand(TInt aTarget, TUint8 aRed, TUint8 aGreen, TUint8 aBlue, TBool aDefaultColorCmd, TInt aErrorCode, TUint delay)
	{
	INFO_PRINTF1(_L("Step state: ExpectSetColorCommand"));

	RMockLight& mockLight = GetMockLight();
	
	HWRMLightCommand::TLightsColorData lightColorData;
    Mem::FillZ(&lightColorData, sizeof(HWRMLightCommand::TLightsColorData));
	lightColorData.iTarget = aTarget;
	lightColorData.iRed = aRed;
	lightColorData.iGreen = aGreen;
	lightColorData.iBlue = aBlue;
	lightColorData.iDefaultColor = aDefaultColorCmd;
	HWRMLightCommand::TLightsColorCmdPackage lightColorPackg(lightColorData);

	mockLight.ExpectProcessCmdL(HWRMLightCommand::ELightsSetColorCmdId, lightColorPackg);

	HWRMLightCommand::TErrorCodeResponsePackage response(aErrorCode);
	mockLight.CompleteL(HWRMLightCommand::ELightsSetColorCmdId, delay, response);
	}

void CHWRMTest::DisplayLightColorL()
	{
	INFO_PRINTF1(_L("Step state: DisplayLightColorL"));

	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplay, 10, 15, 20, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 10, 15, 20);
	light.SetLightColorL(CHWRMLight::EPrimaryDisplay, rgb);
	}

void CHWRMTest::DisplayAndKeyboardLightColorL()
	{
	INFO_PRINTF1(_L("Step state: DisplayAndKeyboardLightColorL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplay|CHWRMLight::EPrimaryKeyboard, 50, 60, 70, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 50, 60, 70);
	light.SetLightColorL(CHWRMLight::EPrimaryDisplay|CHWRMLight::EPrimaryKeyboard, rgb);
	}

void CHWRMTest::DisplayLightColor_ExtendedL()
	{
	INFO_PRINTF1(_L("Step state: DisplayLightColor_ExtendedL"));

	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplay, 10, 15, 20, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 10, 15, 20);
	light.SetLightColorL(CHWRMExtendedLight::EPrimaryDisplay, rgb);
	}

void CHWRMTest::DisplayAndKeyboardLightColor_ExtendedL()
	{
	INFO_PRINTF1(_L("Step state: DisplayAndKeyboardLightColor_ExtendedL"));
	
	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplay|CHWRMExtendedLight::EPrimaryKeyboard, 50, 60, 70, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 50, 60, 70);
	light.SetLightColorL(CHWRMExtendedLight::EPrimaryDisplay|CHWRMExtendedLight::EPrimaryKeyboard, rgb);
	}

void CHWRMTest::DisplayLightDefaultColorL()
	{
	INFO_PRINTF1(_L("Step state: DisplayLightDefaultColorL"));

	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();

	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplay, 0, 0, 0, ETrue, KErrNone);

	light.SetLightDefaultColorL(CHWRMLight::EPrimaryDisplay);
	}

void CHWRMTest::DisplayAndKeyboardLightDefaultColorL()
	{
	INFO_PRINTF1(_L("Step state: DisplayAndKeyboardLightDefaultColorL"));
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();

	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplay|CHWRMLight::EPrimaryKeyboard, 0, 0, 0, ETrue, KErrNone);

	light.SetLightDefaultColorL(CHWRMLight::EPrimaryDisplay|CHWRMLight::EPrimaryKeyboard);
	}

void CHWRMTest::DisplayLightDefaultColor_ExtendedL()
	{
	INFO_PRINTF1(_L("Step state: DisplayLightDefaultColor_ExtendedL"));

	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL();

	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplay, 0, 0, 0, ETrue, KErrNone);

	light.SetLightDefaultColorL(CHWRMExtendedLight::EPrimaryDisplay);
	}

void CHWRMTest::DisplayAndKeyboardLightDefaultColor_ExtendedL()
	{
	INFO_PRINTF1(_L("Step state: DisplayAndKeyboardLightDefaultColor_ExtendedL"));
	
	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL();

	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplay|CHWRMExtendedLight::EPrimaryKeyboard, 0, 0, 0, ETrue, KErrNone);

	light.SetLightDefaultColorL(CHWRMExtendedLight::EPrimaryDisplay|CHWRMExtendedLight::EPrimaryKeyboard);
	}

void CHWRMTest::LightColorUnSupportedTargetL()
	{
	INFO_PRINTF1(_L("Step state: LightColorUnSupportedTargetL"));
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	ExpectLeave(KErrNotSupported);
	ExpectLeave(KErrNotSupported);
	SETCOLORPARAM(rgb, 10, 20, 30);
	light.SetLightColorL(CHWRMLight::EPrimaryDisplayAndKeyboard|CHWRMLight::ECustomTarget1, rgb);
	light.SetLightDefaultColorL(CHWRMExtendedLight::EPrimaryDisplayAndKeyboard|CHWRMExtendedLight::ECustomTarget1);
	}

void CHWRMTest::LightColorPluginErrorL()
	{
	INFO_PRINTF1(_L("Step state: LightColorPluginErrorL"));

	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();
	THWRMLightColor rgb;

	ExpectLeave(KErrGeneral);
	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplay, 10, 15, 20, EFalse, KErrGeneral);
	SETCOLORPARAM(rgb, 10, 15, 20);
	light.SetLightColorL(CHWRMLight::EPrimaryDisplay, rgb);
	}

void CHWRMTest::ReserveDisplayWithfrozenstateOnL()
	{
	INFO_PRINTF1(_L("Step state: ReserveDisplayWithfrozenstateOnL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL();

	light.ReserveLightL(CHWRMExtendedLight::EPrimaryDisplay, ETrue, ETrue);
	}

void CHWRMTest::ReleaseDisplay_ExtendedL()
	{
	INFO_PRINTF1(_L("Step state: ReleaseDisplay_ExtendedL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL();

	light.ReleaseLight(CHWRMExtendedLight::EPrimaryDisplay);
	}

void CHWRMTest::ReserveDisplayWithfrozenstateOn_RestoreColorL()
	{
	INFO_PRINTF1(_L("Step state: ReserveDisplayWithfrozenstateOnL"));
	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplay, 75, 25, 100, EFalse, KErrNone);
	ReserveDisplayWithfrozenstateOnL();
	}

void CHWRMTest::ReserveDisplay_Client1L()
	{
	INFO_PRINTF1(_L("Step state: ReserveDisplay_Client1L"));
	CHWRMLight& light = GetLightSessionL(0);

	light.ReserveLightL(CHWRMLight::EPrimaryDisplay, EFalse, ETrue);
	}

void CHWRMTest::ReserveDisplay_Client2L()
	{
	INFO_PRINTF1(_L("Step state: ReserveDisplay_Client2L"));
	CHWRMLight& light = GetLightSessionL(1);

	light.ReserveLightL(CHWRMLight::EPrimaryDisplay, EFalse, ETrue);
	}


void CHWRMTest::ReserveDisplay_Client1_ExtL()
	{
	INFO_PRINTF1(_L("Step state: ReserveDisplay_Client1_ExtL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionL(0);

	light.ReserveLightL(CHWRMExtendedLight::EPrimaryDisplay, EFalse, ETrue);
	}

void CHWRMTest::DisplayLightColor_Client1L()
	{
	INFO_PRINTF1(_L("Step state: DisplayLightColor_Client1L"));
	CHWRMLight& light = GetLightSessionL(0);
	THWRMLightColor rgb;

	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplay, 10, 15, 20, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 10, 15, 20);
	light.SetLightColorL(CHWRMLight::EPrimaryDisplay, rgb);
	}

void CHWRMTest::DisplayLightOnColor_Client2L()
	{
	INFO_PRINTF1(_L("Step state: DisplayLightColor_Client2L"));
	CHWRMLight& light = GetLightSessionL(1);
	THWRMLightColor rgb;

	SETCOLORPARAM(rgb, 10, 15, 20);
	light.LightOnL(CHWRMLight::EPrimaryDisplay, 4*1000, rgb);
	}


void CHWRMTest::DisplayLightColor_Client1_ExtL()
	{
	INFO_PRINTF1(_L("Step state: DisplayLightColor_Client1_ExtL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionL(0);
	THWRMLightColor rgb;

	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplay, 10, 15, 20, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 10, 15, 20);
	light.SetLightColorL(CHWRMExtendedLight::EPrimaryDisplay, rgb);
	}

void CHWRMTest::ReserveKeyboard_Client2L()
	{
	INFO_PRINTF1(_L("Step state: ReserveKeyboard_Client2L"));
	CHWRMLight& light = GetLightSessionL(1);

	light.ReserveLightL(CHWRMLight::EPrimaryKeyboard, EFalse, ETrue);
	}

void CHWRMTest::ReserveKeyboard_Client2_ExtL()
	{
	INFO_PRINTF1(_L("Step state: ReserveKeyboard_Client2_ExtL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionL(1);

	light.ReserveLightL(CHWRMExtendedLight::EPrimaryKeyboard, EFalse, ETrue);
	}

void CHWRMTest::KeyboardLightColor_Client2L()
	{
	INFO_PRINTF1(_L("Step state: KeyboardLightColor_Client2L"));
	CHWRMLight& light = GetLightSessionL(1);
	THWRMLightColor rgb;

	ExpectSetColorCommand(CHWRMLight::EPrimaryKeyboard, 20, 35, 40, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 20, 35, 40);
	light.SetLightColorL(CHWRMLight::EPrimaryKeyboard, rgb);
	}

void CHWRMTest::KeyboardLightColor_Client2_ExtL()
	{
	INFO_PRINTF1(_L("Step state: KeyboardLightColor_Client2_ExtL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionL(1);
	THWRMLightColor rgb;

	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryKeyboard, 20, 35, 40, EFalse, KErrNone);
	SETCOLORPARAM(rgb, 20, 35, 40);
	light.SetLightColorL(CHWRMExtendedLight::EPrimaryKeyboard, rgb);
	}

void CHWRMTest::ReserveDisplayKeyboard_Client3L()
	{
	INFO_PRINTF1(_L("Step state: ReserveDisplayKeyboard_Client3L"));
	CHWRMLight& light = GetLightSessionL(2);

	light.ReserveLightL(CHWRMLight::EPrimaryDisplay|CHWRMLight::EPrimaryKeyboard, EFalse, ETrue);
	}

void CHWRMTest::ReserveDisplayKeyboard_Client3_ExtL()
	{
	INFO_PRINTF1(_L("Step state: ReserveDisplayKeyboard_Client3_ExtL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionL(2);

	light.ReserveLightL(CHWRMExtendedLight::EPrimaryDisplay|CHWRMExtendedLight::EPrimaryKeyboard, EFalse, ETrue);
	}

void CHWRMTest::DisplayKeyboardLightColor_Client3L()
	{
	INFO_PRINTF1(_L("Step state: DisplayKeyboardLightColor_Client3L"));
	CHWRMLight& light = GetLightSessionL(2);
	THWRMLightColor rgb;

	SETCOLORPARAM(rgb, 20, 35, 40);
	light.SetLightColorL(CHWRMLight::EPrimaryDisplay|CHWRMLight::EPrimaryKeyboard, rgb);
	}

void CHWRMTest::DisplayKeyboardLightColor_Client3_ExtL()
	{
	INFO_PRINTF1(_L("Step state: DisplayKeyboardLightColor_Client3_ExtL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionL(2);
	THWRMLightColor rgb;

	SETCOLORPARAM(rgb, 20, 35, 40);
	light.SetLightColorL(CHWRMExtendedLight::EPrimaryDisplay|CHWRMExtendedLight::EPrimaryKeyboard, rgb);
	}

// keyboard color should be restored for client3 session
void CHWRMTest::ReleaseDisplayKeyboard_Client2L()
	{
	INFO_PRINTF1(_L("Step state: ReleaseDisplayKeyboard_Client2L"));
	CHWRMLight& light = GetLightSessionL(1);

	ExpectSetColorCommand(CHWRMLight::EPrimaryKeyboard, 20, 35, 40, EFalse, KErrNone);

	light.ReleaseLight(CHWRMLight::EPrimaryDisplay|CHWRMLight::EPrimaryKeyboard);
	}

void CHWRMTest::ReleaseDisplayKeyboard_Client2_ExtL()
	{
	INFO_PRINTF1(_L("Step state: ReleaseDisplayKeyboard_Client2_ExtL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionL(1);

	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryKeyboard, 20, 35, 40, EFalse, KErrNone);

	light.ReleaseLight(CHWRMExtendedLight::EPrimaryDisplay|CHWRMExtendedLight::EPrimaryKeyboard);
	}

// default color for keyboard should be restored
void CHWRMTest::ReleaseDisplayKeyboard_Client3L()
	{
	INFO_PRINTF1(_L("Step state: ReleaseDisplayKeyboard_Client3L"));
	CHWRMLight& light = GetLightSessionL(2);

	ExpectSetColorCommand(CHWRMLight::EPrimaryKeyboard, 0, 0, 0, ETrue, KErrNone);

	light.ReleaseLight(CHWRMLight::EPrimaryDisplay|CHWRMLight::EPrimaryKeyboard);
	}

void CHWRMTest::ReleaseDisplayKeyboard_Client3_ExtL()
	{
	INFO_PRINTF1(_L("Step state: ReleaseDisplayKeyboard_Client3_ExtL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionL(2);

	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryKeyboard, 0, 0, 0, ETrue, KErrNone);

	light.ReleaseLight(CHWRMExtendedLight::EPrimaryDisplay|CHWRMExtendedLight::EPrimaryKeyboard);
	}

// default color for display should be restored
void CHWRMTest::ReleaseDisplayKeyboard_Client1L()
	{
	INFO_PRINTF1(_L("Step state: ReleaseDisplayKeyboard_Client1L"));
	CHWRMLight& light = GetLightSessionL(0);

	ExpectSetColorCommand(CHWRMLight::EPrimaryDisplay, 0, 0, 0, ETrue, KErrNone);

	light.ReleaseLight(CHWRMLight::EPrimaryDisplay|CHWRMLight::EPrimaryKeyboard);
	}

void CHWRMTest::ReleaseDisplayKeyboard_Client1_ExtL()
	{
	INFO_PRINTF1(_L("Step state: ReleaseDisplayKeyboard_Client1_ExtL"));
	CHWRMEnhancedLight& light = GetEnhancedLightSessionL(0);

	ExpectSetColorCommand(CHWRMExtendedLight::EPrimaryDisplay, 0, 0, 0, ETrue, KErrNone);

	light.ReleaseLight(CHWRMExtendedLight::EPrimaryDisplay|CHWRMExtendedLight::EPrimaryKeyboard);
	}

void CHWRMTest::DisplayKeyboardLightColor_Client1_ErrorL()
	{
	INFO_PRINTF1(_L("Step state: DisplayKeyboardLightColor_Client1_ErrorL"));
	CHWRMLight& light = GetLightSessionL(0);
	THWRMLightColor rgb;

	ExpectLeave(KErrInUse);
	light.SetLightColorL(CHWRMLight::EPrimaryDisplay|CHWRMLight::EPrimaryKeyboard, rgb);
	}

void CHWRMTest::DisplayKeyboardLightDefaultColor_Client1_ErrorL()
	{
	INFO_PRINTF1(_L("Step state: DisplayKeyboardLightDefaultColor_Client1_ErrorL"));
	CHWRMLight& light = GetLightSessionL(0);

	ExpectLeave(KErrInUse);
	light.SetLightDefaultColorL(CHWRMLight::EPrimaryDisplay|CHWRMLight::EPrimaryKeyboard);
	}

void CHWRMTest::DisableNotificationCheckingL()
	{
	INFO_PRINTF1(_L("Step state: DisableNotificationCheckingL"));
	SetIgnoreStatusNotifications(ETrue);
	}
	
void CHWRMTest::EnableNotificationCheckingL()
	{
	INFO_PRINTF1(_L("Step state: EnableNotificationCheckingL"));
	SetIgnoreStatusNotifications(EFalse);
	}

void CHWRMTest::OpenExtLightSessionL()
	{
	INFO_PRINTF1(_L("Start state: OpenExtLightSessionL"));
	
	ExpectExtLightStatusNotificationL(KSupportedTargets, CHWRMExtendedLight::ELightStatusUnknown);

	//open 2 extended light sessions
	OpenSessionsL(0,0,2);
	}

void CHWRMTest::OpenEnhancedLightSessionL()
	{
	INFO_PRINTF1(_L("Start state: OpenEnhancedLightSessionL"));
	
	ExpectExtLightStatusNotificationL(KSupportedTargets, CHWRMExtendedLight::ELightStatusUnknown);

	//open 2 extended light sessions
	OpenLightSessionEnhancedL(3);
	}

void CHWRMTest::ExtendedLightOnL()
	{
	INFO_PRINTF1(_L("Start state: ExtendedLightsOnL"));	
	
	CHWRMExtendedLight& light = GetExtLightSessionWithCallbackRegisteredL();
	ExpectExtLightStatusNotificationL(CHWRMExtendedLight::EPrimaryDisplay, CHWRMExtendedLight::ELightOn);
	light.LightOnL(CHWRMExtendedLight::EPrimaryDisplay);			
	}

void CHWRMTest::ExtendedLight2BlinkL()
	{
	INFO_PRINTF1(_L("Start state: ExtendedLightsOnL"));
	
	CHWRMExtendedLight& light2 = GetExtLightSessionL(1);
	ExpectExtLightStatusNotificationL(CHWRMExtendedLight::EPrimaryKeyboard, CHWRMExtendedLight::ELightBlink);
	light2.LightBlinkL(CHWRMExtendedLight::EPrimaryKeyboard, 2000);	
	}

void CHWRMTest::ExtendedLightOffL()
	{
	INFO_PRINTF1(_L("Start state: ExtendedLightsOnL"));	
	
	CHWRMExtendedLight& light = GetExtLightSessionWithCallbackRegisteredL();
	ExpectExtLightStatusNotificationL(CHWRMExtendedLight::EPrimaryDisplay, CHWRMExtendedLight::ELightOff);
	light.LightOffL(CHWRMExtendedLight::EPrimaryDisplay);			
	}

void CHWRMTest::ExtendedLight2OffL()
	{
	INFO_PRINTF1(_L("Start state: ExtendedLightsOnL"));	
	
	CHWRMExtendedLight& light2 = GetExtLightSessionL(1);
	ExpectExtLightStatusNotificationL(CHWRMExtendedLight::EPrimaryKeyboard, CHWRMExtendedLight::ELightOff);
	light2.LightOffL(CHWRMExtendedLight::EPrimaryKeyboard, 2000);			
	}

void CHWRMTest::ExtendedLightOffAllTargetsL()
	{
	INFO_PRINTF1(_L("Step state: ExtendedLightOffAllTargetsL"));

    // prerequisite: light session has already been opened
	CHWRMExtendedLight& light = GetExtLightSessionWithCallbackRegisteredL(); 
	ExpectLightStatusNotificationL(KSupportedTargets, CHWRMLight::ELightOff);		
	light.LightOffL(KSupportedTargets);
	}

void CHWRMTest::EnhancedLightOffAllTargetsL()
	{
	INFO_PRINTF1(_L("Step state: EnhancedLightOffAllTargetsL"));

    // prerequisite: light session has already been opened
	CHWRMEnhancedLight& light = GetEnhancedLightSessionWithCallbackRegisteredL(); 
	ExpectLightStatusNotificationL(KSupportedTargets, CHWRMLight::ELightOff);		
	light.LightOffL(KSupportedTargets);
	}

void CHWRMTest::PowerGetCRAttributesL()
	{
	INFO_PRINTF1(_L("Step state: PowerGetCRAttributes"));
	
	// saved as testbase's members
	GetPowerAttributesL();
	}

void CHWRMTest::PowerOpenL()
	{
	INFO_PRINTF1(_L("Step state: PowerOpenL"));
	
	// one power session
	ResetReportingPeriodL();
	OpenSessionsL(0,0,0,0,1);	
	}

void CHWRMTest::PowerOpen2SessionsL()
	{
	INFO_PRINTF1(_L("Step state: PowerOpen2SessionsL"));
    ResetReportingPeriodL();
	OpenSessionsL(0,0,0,0,2);	
	}

void CHWRMTest::PowerOpen3SessionsL()
	{
	INFO_PRINTF1(_L("Step state: PowerOpen3SessionsL"));
    ResetReportingPeriodL();
	OpenSessionsL(0,0,0,0,3);	
	}

void CHWRMTest::PowerGetBatteryInfoL()
	{
	INFO_PRINTF1(_L("Step state: PowerGetBatteryInfoL"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();

	TRequestStatus status;
	CHWRMPower::TBatteryConsumptionData consumptionData;
	
	power.GetBatteryInfo(status, consumptionData);
	
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	if(consumptionData.iNominalCapacity != 200000 ||
	   consumptionData.iRemainingCapacity != 120000 ||
	   consumptionData.iRemainingPercentageCapacity != 60)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::PowerGetBatteryInfoMultiClientL()
	{
	INFO_PRINTF1(_L("Step state: PowerGetBatteryInfoMultiClientL"));
	
	// the 3 requests are grouped together in one step so that they are
	// all received by the server before the MockSY responds to 
	// the server and completes the first request.
	CHWRMPower& power1 = GetPowerSessionWithCallbackRegisteredL();
	CHWRMPower& power2 = GetPowerSessionL(1);
	CHWRMPower& power3 = GetPowerSessionL(2);
			
	TRequestStatus status1;
	TRequestStatus status2;
	TRequestStatus status3;
	
	CHWRMPower::TBatteryConsumptionData consumptionData1;
	CHWRMPower::TBatteryConsumptionData consumptionData2;
	CHWRMPower::TBatteryConsumptionData consumptionData3;
	
	power1.GetBatteryInfo(status1, consumptionData1);
	power2.GetBatteryInfo(status2, consumptionData2);
	power3.GetBatteryInfo(status3, consumptionData3);

	User::WaitForRequest(status1);
	User::LeaveIfError(status1.Int());
	User::WaitForRequest(status2);
	User::LeaveIfError(status2.Int());
	User::WaitForRequest(status3);
	User::LeaveIfError(status3.Int());
	
	if(consumptionData1.iNominalCapacity != 200000 ||
	   consumptionData1.iRemainingCapacity != 120000 ||
	   consumptionData1.iRemainingPercentageCapacity != 60)
		{
		User::Leave(KErrGeneral);
		}
	
	if(consumptionData2.iNominalCapacity != 200000 ||
	   consumptionData2.iRemainingCapacity != 120000 ||
	   consumptionData2.iRemainingPercentageCapacity != 60)
		{
		User::Leave(KErrGeneral);
		}
	
	if(consumptionData3.iNominalCapacity != 200000 ||
	   consumptionData3.iRemainingCapacity != 120000 ||
	   consumptionData3.iRemainingPercentageCapacity != 60)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::PowerGetBatteryInfoWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Step state: PowerGetBatteryInfoWithPluginErrorL"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();

	ExpectLeave(KErrNotReady);
	
	RMockPower& mockPower = GetMockPower();

	TBuf8<1> emptyDes;
	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryInfoCmdId, emptyDes);

	HWRMPowerCommand::TBatteryConsumptionResponseData batteryConsumptionReport;
	batteryConsumptionReport.iErrorCode = KErrNotReady;
	batteryConsumptionReport.iNominalCapacity = 0;
	batteryConsumptionReport.iRemainingCapacity = 0;
	batteryConsumptionReport.iRemainingPercentageCapacity= 0;

	HWRMPowerCommand::TBatteryConsumptionResponsePackage batteryConsumptionReportPackg(batteryConsumptionReport);
	mockPower.CompleteL(HWRMPowerCommand::EGetBatteryInfoCmdId, 1, batteryConsumptionReportPackg);
	
	TRequestStatus status;	
	CHWRMPower::TBatteryConsumptionData consumptionData;
	power.GetBatteryInfo(status, consumptionData);
	
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::PowerGetBatteryVoltageInfoL()
	{
	INFO_PRINTF1(_L("Step state: PowerGetBatteryVoltageInfoL"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();

	TRequestStatus status;
	
	CHWRMPower::TBatteryVoltageData voltageData;
	
	power.GetBatteryVoltageInfo(status, voltageData);

	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	
	if(voltageData.iMaximumVoltage != 1000 ||
	   voltageData.iMinimumVoltage != 1 ||
	   voltageData.iRemainingVoltage != 69)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::PowerGetBatteryVoltageInfoMultiClientL()
	{
	INFO_PRINTF1(_L("Step state: PowerGetBatteryVoltageInfoMultiClientL"));
	
	// the 3 requests are grouped together in one step so that they are
	// all received by the server before the MockSY responds to 
	// the server and completes the first request.
	CHWRMPower& power1 = GetPowerSessionWithCallbackRegisteredL();
	CHWRMPower& power2 = GetPowerSessionL(1);
	CHWRMPower& power3 = GetPowerSessionL(2);
			
	TRequestStatus status1;
	TRequestStatus status2;
	TRequestStatus status3;
	
	CHWRMPower::TBatteryVoltageData voltageData1;
	CHWRMPower::TBatteryVoltageData voltageData2;
	CHWRMPower::TBatteryVoltageData voltageData3;
	
	power1.GetBatteryVoltageInfo(status1, voltageData1);
	power2.GetBatteryVoltageInfo(status2, voltageData2);
	power3.GetBatteryVoltageInfo(status3, voltageData3);

	User::WaitForRequest(status1);
	User::LeaveIfError(status1.Int());
	User::WaitForRequest(status2);
	User::LeaveIfError(status2.Int());
	User::WaitForRequest(status3);
	User::LeaveIfError(status3.Int());
	
	if(voltageData1.iMaximumVoltage != 1000 ||
	   voltageData1.iMinimumVoltage != 1 ||
	   voltageData1.iRemainingVoltage != 69)
		{
		User::Leave(KErrGeneral);
		}
	
	if(voltageData2.iMaximumVoltage != 1000 ||
	   voltageData2.iMinimumVoltage != 1 ||
	   voltageData2.iRemainingVoltage != 69)
		{
		User::Leave(KErrGeneral);
		}
	
	if(voltageData3.iMaximumVoltage != 1000 ||
	   voltageData3.iMinimumVoltage != 1 ||
	   voltageData3.iRemainingVoltage != 69)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::PowerGetBatteryVoltageInfoWithPluginErrorL()
	{
	INFO_PRINTF1(_L("Step state: PowerGetBatteryVoltageInfoWithPluginErrorL"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();

	ExpectLeave(KErrNotReady);
	
	RMockPower& mockPower = GetMockPower();

	TBuf8<1> emptyDes;

	mockPower.ExpectProcessCmdL(HWRMPowerCommand::EGetBatteryVoltageInfoCmdId, emptyDes);

	HWRMPowerCommand::TBatteryVoltageResponseData batteryVoltageReport;
	batteryVoltageReport.iErrorCode = KErrNotReady;
	batteryVoltageReport.iMaximumVoltage = 0;
	batteryVoltageReport.iMinimumVoltage = 0;
	batteryVoltageReport.iRemainingVoltage = 0;

	HWRMPowerCommand::TBatteryVoltageResponsePackage batteryVoltageReportPackg(batteryVoltageReport);
	mockPower.CompleteL(HWRMPowerCommand::EGetBatteryVoltageInfoCmdId, 1, batteryVoltageReportPackg);
	
	TRequestStatus status;	
	CHWRMPower::TBatteryVoltageData voltageData;
	power.GetBatteryVoltageInfo(status, voltageData); 
	
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CHWRMTest::PowerGetBaseTimeIntervalL()
	{
	INFO_PRINTF1(_L("Step state: PowerGetBaseTimeIntervalL"));
	
	CHWRMPower& power = GetPowerSessionWithCallbackRegisteredL();
	
	TInt baseTimeInterval(0);
	
	power.GetBaseTimeIntervalL(baseTimeInterval);
	}



void CHWRMTest::FmTxOpenL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxOpenL"));

	ExpectFmTxStatusNotificationL( EFmTxStateOff );
	
	OpenSessionsL(0,0,0,1);	
	}

void CHWRMTest::FmTxOpenWhileInactiveL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxOpenWhileInactiveL"));
					
	ExpectFmTxStatusNotificationL( EFmTxStateInactive);
	
	OpenSessionsL(0,0,0,1);	
	}

void CHWRMTest::FmTxOpenWhileActiveL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxOpenWhileActiveL"));
					
	ExpectFmTxStatusNotificationL( EFmTxStateActive);
	
	OpenSessionsL(0,0,0,1);
	}

void CHWRMTest::FmTxOpenWhileOnL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxOpenWhileOnL"));
					
	ExpectFmTxStatusNotificationL( EFmTxStateActive, EFmTxStateInactive );
	
	OpenSessionsL(0,0,0,1);
	}

void CHWRMTest::FmTxOpenWhilePowerSaveAccessoryL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxOpenWhilePowerSaveAccessoryL"));
					
	ExpectFmTxStatusNotificationL( EFmTxStatePowerSaveAccessory);
	
	OpenSessionsL(0,0,0,1);	
	}


void CHWRMTest::FmTxEnableL()
	{
	INFO_PRINTF1(_L("Step state: FmTxEnableL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	ExpectFmTxStatusNotificationL(EFmTxStateInactive, EFmTxStateActive); 

	fmTx.EnableL();
	}

void CHWRMTest::FmTx2EnableL()
	{
	INFO_PRINTF1(_L("Step state: FmTx2EnableL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionL(1);
	TInt frequency = 0;
	
	ExpectFmTxStatusNotificationL(EFmTxStateInactive, EFmTxStateActive);
	
	fmTx.EnableL(frequency);
	}


void CHWRMTest::FmTxEnableWhileActiveOrInactiveL()
	{
	INFO_PRINTF1(_L("Step state: FmTxEnableWhileActiveOrInactiveL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL(); 

	fmTx.EnableL();
	}


void CHWRMTest::FmTxStatusEnabledL()
	{
	INFO_PRINTF1(_L("Step state: FmTxStatusEnabledL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TFmTxState currentStatus = fmTx.Status();	
		
	if (currentStatus != EFmTxStateInactive && currentStatus !=  EFmTxStateActive)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxStatusOffL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxStatusOffL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TFmTxState currentStatus = fmTx.Status();	
		
	if (currentStatus != EFmTxStateOff)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxDisableL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxDisableL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
			
	fmTx.DisableL();
	}

void CHWRMTest::FmTxReserveL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxReserveL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	// make client trusted or forceNoCCoeEn false
	TBool forceNoCCoeEn = ETrue;		
	
	fmTx.ReserveL(forceNoCCoeEn);
	}

void CHWRMTest::FmTx2ReserveL()
	{	
	INFO_PRINTF1(_L("Step state: FmTx2ReserveL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionL(1);
	
	TBool forceNoCCoeEn = ETrue; 
	
	fmTx.ReserveL(forceNoCCoeEn);
	}

void CHWRMTest::FmTxOpen2SessionsL()
	{
	INFO_PRINTF1(_L("Step state: FmTxOpen2SessionsL"));
	
	ExpectFmTxStatusNotificationL( EFmTxStateOff );

	OpenSessionsL(0,0,0,2);	
	}

void CHWRMTest::FmTxEnableInvalidFreqL()
	{
	INFO_PRINTF1(_L("Step state: FmTxEnableInvalidFreqL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	TInt frequency = 108050; //an invalid frequency
	
	ExpectLeave(KErrArgument);
	
	fmTx.EnableL(frequency);
	}

void CHWRMTest::FmTxDisableDuringReservation()
	{
	INFO_PRINTF1(_L("Step state: FmTxDisableDuringReservation"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	ExpectLeave(KErrInUse);
	
	fmTx.DisableL();
	}

void CHWRMTest::FmTx2EnableDuringReservationL()
	{
	INFO_PRINTF1(_L("Step state: FmTx2EnableDuringReservationL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionL(1);
	
	ExpectLeave(KErrInUse);
	
	fmTx.EnableL();
	}

void CHWRMTest::FmTxGetCurrentStatusL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetCurrentStatusL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	iTempFmTxState  = fmTx.Status();
	}

void CHWRMTest::FmTxCompareCurrentAndTempStatusL()
	{
	INFO_PRINTF1(_L("Step state: FmTxCompareCurrentAndTempStatusL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	if (iTempFmTxState != fmTx.Status())
		{
		User::Leave(KErrGeneral);		
		}
	}

void CHWRMTest::FmTxEnableValidFreqL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxEnableValidFreqL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TInt frequency = fmTx.Frequency();
	
	ExpectFmTxStatusNotificationL(EFmTxStateInactive, EFmTxStateActive); 
	
	fmTx.EnableL(frequency);
	}


void CHWRMTest::FmTxEnableNewFreqL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxEnableNewFreqL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TInt frequency = fmTx.Frequency();
	IncrementFrequency(frequency);
		
	ExpectFmTxStatusNotificationL(EFmTxStateInactive, EFmTxStateActive);
	ExpectFmTxFrequencyNotificationL(frequency);
	
	fmTx.EnableL(frequency);
	}


void CHWRMTest::FmTxSetFrequencyInvalidL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetFrequencyInvalidL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	ExpectLeave(KErrArgument);
	
	fmTx.SetFrequencyL(75000/*an invalid frequency*/);		
	}


void CHWRMTest::FmTx2SetFrequencyDuringReservationL()
	{	
	INFO_PRINTF1(_L("Step state: FmTx2SetFrequencyDuringReservationL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionL(1);
	
	TInt frequency = fmTx.Frequency();

	IncrementFrequency(frequency);
	
	ExpectLeave(KErrInUse);
	
	fmTx.SetFrequencyL(frequency);
	}

void CHWRMTest::FmTxGetNextClearFrequencyArrayL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetNextClearFrequencyArrayL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TClearFrequencyArray clearFreqs;
	TUint count=KClearFrequencyArrayMax;
	
	fmTx.GetNextClearFrequencyL( clearFreqs, count );
	
	// the frequency that is returned is hard coded in the cmockfmtxengine
	if(	clearFreqs[0] != 89000 ||
		clearFreqs[1] != 89100 ||
		clearFreqs[2] != 89200 ||
		clearFreqs[3] != 89300 ||
		clearFreqs[4] != 89400 ||
		clearFreqs[5] != 89500 ||
		clearFreqs[6] != 89600 ||
		clearFreqs[7] != 89700 ||
		clearFreqs[8] != 89800 ||
		clearFreqs[9] != 89900)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxGetNextClearFrequencyArrayWhileDisabledL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetNextClearFrequencyArrayWhileDisabledL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TClearFrequencyArray clearFreqs;
	TUint count = 1;
	
	ExpectLeave(KErrNotReady);
	
	fmTx.GetNextClearFrequencyL( clearFreqs, count );
	}

void CHWRMTest::FmTxGetNextClearFrequencyArrayErrArgumentMinL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetNextClearFrequencyArrayErrArgumentMinL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	TClearFrequencyArray clearFreqs;
	TUint count=0; // 0 < count <= KClearFrequencyArrayMax	
	
	ExpectLeave(KErrArgument);	
	fmTx.GetNextClearFrequencyL( clearFreqs, count );
	}

void CHWRMTest::FmTxGetNextClearFrequencyArrayErrArgumentMaxL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetNextClearFrequencyArrayErrArgumentMaxL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	TClearFrequencyArray clearFreqs;
	TUint count=KClearFrequencyArrayMax + 1; // 0 < count <= KClearFrequencyArrayMax
	
	ExpectLeave(KErrArgument);
	fmTx.GetNextClearFrequencyL( clearFreqs, count );
	}


void CHWRMTest::FmTxIsAutoFrequencyEnabled()
	{
	INFO_PRINTF1(_L("Step state: FmTxIsAutoFrequencyEnabled"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	fmTx.IsAutoFrequencyEnabled();
	}

void CHWRMTest::FmTxEnableAutoFrequencyL()
	{
	INFO_PRINTF1(_L("Step state: FmTxEnableAutoFrequencyL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	//Currently not supported
    ExpectLeave(KErrNotSupported);
	
	fmTx.EnableAutoFrequencyL();
	}

void CHWRMTest::FmTxDisableAutoFrequencyL()
	{
	INFO_PRINTF1(_L("Step state: FmTxDisableAutoFrequencyL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
    //Currently not supported
    ExpectLeave(KErrNotSupported);
	
	fmTx.DisableAutoFrequencyL();
	}

void CHWRMTest::FmTxGetNextClearFrequencyL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetNextClearFrequencyL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	TInt clearFrequency;
	fmTx.GetNextClearFrequencyL(clearFrequency);
	
	// the frequency that is returned is hard coded in the cmockfmtxengine
	if (clearFrequency != 89000)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxGetNextClearFrequencyAsyncL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetNextClearFrequencyAsyncL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TRequestStatus status;
	TInt clearFreq(0);
	
	fmTx.GetNextClearFrequency(status, clearFreq);
		
	User::WaitForRequest(status);
	
	// the frequency that is returned is hard coded in the cmockfmtxengine
	if (status.Int() != KErrNone || clearFreq != 89000 ) 
		{
		User::Leave(KErrGeneral);
		}
	else
		{
		iTempNextClearFreq=clearFreq;
		}
	}

void CHWRMTest::FmTxGetNextClearFrequencyAsyncCancelL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetNextClearFrequencyAsyncCancelL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TRequestStatus status;
	TInt clearFreq(0);
	
	fmTx.CancelGetNextClearFrequency(); // covers the case that no pending command exists
	
	fmTx.GetNextClearFrequency(status, clearFreq);
	fmTx.CancelGetNextClearFrequency();
	
	User::WaitForRequest(status);
		
	if (status.Int() != KErrCancel) 
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxGetNextClearFrequencyAsyncArrayL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetNextClearFrequencyAsyncArrayL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	TRequestStatus status;
	TClearFrequencyArray clearFreqs;
	TUint count = KClearFrequencyArrayMax-1;
	TInt countRequested(count);
	
	fmTx.GetNextClearFrequencyL(status, clearFreqs, count);
		
	User::WaitForRequest(status);		
	TInt err = status.Int();
	
	if (err != KErrNone || count != countRequested)
		{
		User::Leave(KErrGeneral);
		}
	
	// the frequency that is returned is hard coded in the cmockfmtxengine
	if(	clearFreqs[0] != 89000 ||
		clearFreqs[1] != 89100 ||
		clearFreqs[2] != 89200 ||
		clearFreqs[3] != 89300 ||
		clearFreqs[4] != 89400 ||
		clearFreqs[5] != 89500 ||
		clearFreqs[6] != 89600 ||
		clearFreqs[7] != 89700 ||
		clearFreqs[8] != 89800)
		{
		User::Leave(KErrGeneral);
		}	
	}


void CHWRMTest::FmTxGetNextClearFrequencyAsyncArrayErrArgumentMinL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetNextClearFrequencyAsyncArrayErrArgumentMinL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	TClearFrequencyArray clearFreqs;
	TUint count=0; // 0 < count <= KClearFrequencyArrayMax	
	TUint countRequested = count;
	
	ExpectLeave(KErrArgument);	
	TRequestStatus status;
	fmTx.GetNextClearFrequencyL( status, clearFreqs, count );
	
	User::WaitForRequest(status);		
	TInt err = status.Int();
	
	if (err != KErrNone || count != countRequested)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxGetNextClearFrequencyAsyncArrayErrArgumentMaxL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetNextClearFrequencyAsyncArrayErrArgumentMaxL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	TClearFrequencyArray clearFreqs;	
	TUint count=KClearFrequencyArrayMax + 1; // 0 < count <= KClearFrequencyArrayMax
	TUint countRequested = count;
	
	ExpectLeave(KErrArgument);
	TRequestStatus status;
	fmTx.GetNextClearFrequencyL( status, clearFreqs, count );
	
	User::WaitForRequest(status);		
	TInt err = status.Int();
	
	if (err != KErrNone || count != countRequested)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxSetNextClearFrequencyL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetNextClearFrequencyL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TInt clearFrequency;
	
	if (fmTx.Frequency() != 89000)
		{
		ExpectFmTxFrequencyNotificationL(89000);
		}
	fmTx.SetNextClearFrequencyL(clearFrequency);
	}

void CHWRMTest::FmTxSetNextClearFrequencyAsyncL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetNextClearFrequencyAsyncL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TRequestStatus status;
	TInt err;	
	
	// iTempNextClearFreq is set by the previous state (FmTxGetNextClearFrequencyAsyncL)
	fmTx.SetNextClearFrequency(status, iTempNextClearFreq);
	
	User::WaitForRequest(status);
	
	err = status.Int();
	
	if (err != KErrNone)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxSetNextClearFrequencyAsyncCancelL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetNextClearFrequencyAsyncCancelL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TRequestStatus status;
	TInt err;	
	
	// iTempNextClearFreq is set by the previous state (FmTxGetNextClearFrequencyAsyncL)
	fmTx.SetNextClearFrequency(status, iTempNextClearFreq);
	fmTx.CancelSetNextClearFrequency();
	
	User::WaitForRequest(status);
	
	err=status.Int();
	
	if (err != KErrCancel)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxGetFrequencyRangeL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetFrequencyRangeL"));
	
	SetFmTxFrequencyRangeEuropeL(); //Set the frequency range to the European values
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	fmTx.GetFrequencyRangeL(iTempFrequencyRange);
	}

void CHWRMTest::FmTxGetFrequencyStepSizeL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetFrequencyStepSizeL"));
	
	GetFmTxFrequencyStepSizeL(); //Set the frequency range to the European values
	}

void CHWRMTest::FmTxSetFrequencyRangeEuropeL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetFrequencyRangeEuropeL"));
	
	SetFmTxFrequencyRangeEuropeL(); //Set the frequency range to the European values
	}

void CHWRMTest::FmTxSetFrequencyRangeJapanL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetFrequencyRangeJapanL"));
	
	SetFmTxFrequencyRangeJapanL(); //Set the frequency range to the Japanese values
	}

void CHWRMTest::FmTxSetInactivityTimeOutTestModeL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetInactivityTimeOutTestModeL"));
	
	// set Inactivity timout to 20 seconds so that state does not change 
	// unexpectedly during the tests because of the timeout.
	SetFmTxInactivityTimeOutL(20);
	}

void CHWRMTest::FmTxCheckGetFrequencyRangeL()
	{
	INFO_PRINTF1(_L("Step state: FmTxCheckGetFrequencyRangeL"));
		
	TFmTxFrequencyRange correctRange;
	correctRange.iMinFrequency = 88100;
	correctRange.iMaxFrequency = 107900;
	correctRange.iStepSize = KDefaultStepSize;
	
	if (correctRange.iMinFrequency != iTempFrequencyRange.iMinFrequency ||
	 	correctRange.iMaxFrequency != iTempFrequencyRange.iMaxFrequency ||
	 	correctRange.iStepSize != iTempFrequencyRange.iStepSize)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxCheckGetJapanFrequencyRangeL()
	{
	INFO_PRINTF1(_L("Step state: FmTxCheckGetJapanFrequencyRangeL"));
		
	TFmTxFrequencyRange correctRange;
	correctRange.iMinFrequency = 88100;
	correctRange.iMaxFrequency = 90000;
	correctRange.iStepSize = KDefaultStepSize;	
	
	if (correctRange.iMinFrequency != iTempFrequencyRange.iMinFrequency ||
	 	correctRange.iMaxFrequency != iTempFrequencyRange.iMaxFrequency ||
	 	correctRange.iStepSize != iTempFrequencyRange.iStepSize)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxSetFrequencyL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetFrequency"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TInt frequency = fmTx.Frequency();

	IncrementFrequency(frequency);
	iTempFrequency = frequency;	
	
	ExpectFmTxFrequencyNotificationL(frequency);
	
	fmTx.SetFrequencyL(frequency);
	}

void CHWRMTest::FmTxSetFrequencyRoundUpL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetFrequencyRoundUpL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TInt frequency = fmTx.Frequency();

	IncrementFrequency(frequency);		
	iTempFrequency = frequency;	
	
	ExpectFmTxFrequencyNotificationL(iTempFrequency);
	
	// will round up to iTempFrequency
	fmTx.SetFrequencyL(frequency - 1);
	}

void CHWRMTest::FmTxSetFrequencyRoundDownL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetFrequencyRoundDownL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TInt frequency = fmTx.Frequency();

	IncrementFrequency(frequency); 
	iTempFrequency = frequency;	// eventually this is the frequency that FmTx will be set to
	IncrementFrequency(frequency);
	
	frequency = frequency - iFmTxFrequencyStepSize/2 - 1;
	
	ExpectFmTxFrequencyNotificationL(iTempFrequency);
	
	fmTx.SetFrequencyL(frequency);
	}

void CHWRMTest::FmTxCheckSetFrequencyL()
	{
	INFO_PRINTF1(_L("Step state: FmTxCheckSetFrequencyL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	if (iTempFrequency != fmTx.Frequency())
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::IncrementFrequency(TInt &aFrequency)
	{
	aFrequency += KDefaultStepSize; // kHz
	if (aFrequency > 108000)
		{
		aFrequency = 87500; // ignore Japanese FM range
		}
	}

void CHWRMTest::FmTxGetFrequencyL()
	{
	INFO_PRINTF1(_L("Step state: FmTxGetFrequency"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	fmTx.Frequency();
	}

void CHWRMTest::FmTxSetRdsPtyL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRdsPtyL"));
	
	TFmTxRdsData data;
	data.iPty = KRdsPtyVariedSpeech;
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	fmTx.SetRdsPtyL( data.iPty );

	TFmTxRdsData getData;
	fmTx.GetRdsData(getData);
	
	if (getData.iPty != data.iPty)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxSetRdsPtyKErrArgumentMinL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRdsPtyKErrArgumentMinL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	ExpectLeave(KErrArgument);
	fmTx.SetRdsPtyL( KRdsPtyNone-1 );
	}

void CHWRMTest::FmTxSetRdsPtyKErrArgumentMaxL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRdsPtyKErrArgumentMaxL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	ExpectLeave(KErrArgument);
	fmTx.SetRdsPtyL( KRdsPtyAlarm + 1 );
	}

void CHWRMTest::FmTxSetRdsPtynL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRdsPtynL"));

	TFmTxRdsData data;
	data.iPtyn.Copy( _L("PWsTunes") );
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	fmTx.SetRdsPtynL( data.iPtyn );
	
	TFmTxRdsData getData;
	fmTx.GetRdsData(getData);
	
	if (getData.iPtyn != data.iPtyn)
		{
		User::Leave(KErrGeneral);
		}
	}
	
void CHWRMTest::FmTxSetRdsMsFalseL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRdsMsFalseL"));
	
	TFmTxRdsData data;
	data.iMs = EFalse;
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	fmTx.SetRdsMsL( data.iMs );

	TFmTxRdsData getData;
	fmTx.GetRdsData(getData);
	
	if (getData.iMs != data.iMs)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxSetRdsMsTrueL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRdsMsTrueL"));
	
	TFmTxRdsData data;
	data.iMs = ETrue;
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	fmTx.SetRdsMsL( data.iMs );

	TFmTxRdsData getData;
	fmTx.GetRdsData(getData);
	
	if (getData.iMs != data.iMs)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxClearRdsDataL()	
	{
	INFO_PRINTF1(_L("Step state: FmTxClearRdsDataL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	fmTx.ClearRdsData(); 
	
	TFmTxRdsData getData;
	fmTx.GetRdsData(getData);
	
	if (getData.iPty !=  KRdsPtyNone ||
		getData.iPtyn.Length() != 0 ||
		!getData.iMs ||
		getData.iLanguageId != KRdsLanguageUnknown)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxSetRdsLangIdL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRdsLangIdL"));
	
	TFmTxRdsData data;
	data.iLanguageId = KRdsLanguageKorean;
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	fmTx.SetRdsLanguageIdL( data.iLanguageId ); 

	TFmTxRdsData getData;
	
	fmTx.GetRdsData(getData);
	
	if (getData.iLanguageId != data.iLanguageId)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxSetRdsLangIdKErrArgumentMinL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRdsLangIdKErrArgumentMinL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	ExpectLeave(KErrArgument);
	fmTx.SetRdsLanguageIdL( KRdsLanguageUnknown-1 ); 
	}

void CHWRMTest::FmTxSetRdsLangIdKErrArgumentMaxL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRdsLangIdKErrArgumentMaxL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	ExpectLeave(KErrArgument);
	fmTx.SetRdsLanguageIdL( KRdsLanguageAmharic + 1); 
	}

// -----------------------------------------------------------------------------
// CHwResManFmTxTest::SetRadiotextTestCase
// Set the RT data transmitted
// -----------------------------------------------------------------------------
//
void CHWRMTest::FmTxSetRadiotextL()
    {
    INFO_PRINTF1(_L("Step state: FmTxSetRadiotext"));
	
    TBuf<64> text;	
	text.Copy( _L("Blah blah blah") );
    
    CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	fmTx.ClearRadiotextData();  //to cover this method as well
	fmTx.SetRadiotextL(text);
    }

void CHWRMTest::FmTxSetRadiotextContent1L()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRadiotextContent1L"));
	
	TRtPlusContentType content = static_cast<TRtPlusContentType>(-1);
	TBuf<64> text;	
	text.Copy( _L("RTplus content") );
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	HWRMFmTxCommand::TRtData rtData;
	rtData.iText.Copy(text);

	rtData.iTag1.iContentType = content;
	rtData.iTag1.iStartMarker = 0;
	rtData.iTag1.iLengthMarker = rtData.iText.Length();
	rtData.iTag2.iContentType = ERtPlusInfoOther;
	rtData.iTag2.iStartMarker = 0;
	rtData.iTag2.iLengthMarker = 0;
	
	ExpectLeave(KErrArgument);		

	// First try setting data with invalid content type
	fmTx.SetRadiotextPlusL(text, content);
	}

void CHWRMTest::FmTxSetRadiotextContent2L()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRadiotextContent2L"));
	
	TRtPlusContentType content = ERtPlusItemAlbum;
	TBuf<64> text;	
	text.Copy( _L("RTplus content") );
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	HWRMFmTxCommand::TRtData rtData;
	rtData.iText.Copy(text);
	rtData.iTag1.iContentType = content;
	rtData.iTag1.iStartMarker = 0;
	rtData.iTag1.iLengthMarker = rtData.iText.Length();
	rtData.iTag2.iContentType = ERtPlusInfoOther;
	rtData.iTag2.iStartMarker = 0;
	rtData.iTag2.iLengthMarker = 0;

	fmTx.SetRadiotextPlusL(text, content);
	}

void CHWRMTest::FmTxSetRadiotextContent2WhileDisabledL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRadiotextContent2WhileDisabledL"));
	
	TRtPlusContentType content = ERtPlusItemAlbum;
	TBuf<64> text;	
	text.Copy( _L("RTplus content") );
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	HWRMFmTxCommand::TRtData rtData;
	rtData.iText.Copy(text);
	rtData.iTag1.iContentType = content;
	rtData.iTag1.iStartMarker = 0;
	rtData.iTag1.iLengthMarker = rtData.iText.Length();
	rtData.iTag2.iContentType = ERtPlusInfoOther;
	rtData.iTag2.iStartMarker = 0;
	rtData.iTag2.iLengthMarker = 0;

	ExpectLeave(KErrNotReady);	
	fmTx.SetRadiotextPlusL(text, content);
	}

void CHWRMTest::FmTxSetRadiotextContent3L()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRadiotextContent3L"));
	
	TRtPlusContentType content = static_cast<TRtPlusContentType>(ERtPlusGetData + 1);
	TBuf<64> text;	
	text.Copy( _L("RTplus content") );
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	HWRMFmTxCommand::TRtData rtData;
	rtData.iText.Copy(text);

	rtData.iTag1.iContentType = content;
	rtData.iTag1.iStartMarker = 0;
	rtData.iTag1.iLengthMarker = rtData.iText.Length();
	rtData.iTag2.iContentType = ERtPlusInfoOther;
	rtData.iTag2.iStartMarker = 0;
	rtData.iTag2.iLengthMarker = 0;
	
	ExpectLeave(KErrArgument);		

	// First try setting data with invalid content type
	fmTx.SetRadiotextPlusL(text, content);
	}

void CHWRMTest::FmTxSetRadiotextTag1L()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRadiotextTag1L"));
	
	TBuf<64> text;	
	text.Copy( _L("RTplus tag") );

	TRtPlusTag tag;
	tag.iContentType = ERtPlusItemTitle;
    tag.iStartMarker = 0;
    tag.iLengthMarker = 100; // not valid
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	HWRMFmTxCommand::TRtData rtData;
	rtData.iText.Copy(text);
	rtData.iTag1.iContentType = tag.iContentType;
	rtData.iTag1.iStartMarker = tag.iStartMarker;
	rtData.iTag1.iLengthMarker = tag.iLengthMarker;
	rtData.iTag2.iContentType = ERtPlusInfoOther;
	rtData.iTag2.iStartMarker = 0;
	rtData.iTag2.iLengthMarker = 0;

	ExpectLeave(KErrArgument);
	
	// First try setting data with invalid content type
	fmTx.SetRadiotextPlusL(text, tag);
	}

void CHWRMTest::FmTxSetRadiotextTag2L()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRadiotextTag2L"));
	
	TBuf<64> text;	
	text.Copy( _L("RTplus tag") );

	TRtPlusTag tag;
	tag.iContentType = ERtPlusItemTitle;
    tag.iStartMarker = 0;
    tag.iLengthMarker = text.Length(); //  valid
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	HWRMFmTxCommand::TRtData rtData;
	rtData.iText.Copy(text);
	rtData.iTag1.iContentType = tag.iContentType;
	rtData.iTag1.iStartMarker = tag.iStartMarker;
	rtData.iTag1.iLengthMarker = tag.iLengthMarker;
	rtData.iTag2.iContentType = ERtPlusInfoOther;
	rtData.iTag2.iStartMarker = 0;
	rtData.iTag2.iLengthMarker = 0;

	fmTx.SetRadiotextPlusL(text, tag);
	}

void CHWRMTest::FmTxSetRadiotextTag2WhileDisabledL()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRadiotextTag2WhileDisabledL"));
	
	TBuf<64> text;	
	text.Copy( _L("RTplus tag") );

	TRtPlusTag tag;
	tag.iContentType = ERtPlusItemTitle;
    tag.iStartMarker = 0;
    tag.iLengthMarker = text.Length(); //  valid
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	HWRMFmTxCommand::TRtData rtData;
	rtData.iText.Copy(text);
	rtData.iTag1.iContentType = tag.iContentType;
	rtData.iTag1.iStartMarker = tag.iStartMarker;
	rtData.iTag1.iLengthMarker = tag.iLengthMarker;
	rtData.iTag2.iContentType = ERtPlusInfoOther;
	rtData.iTag2.iStartMarker = 0;
	rtData.iTag2.iLengthMarker = 0;

	ExpectLeave(KErrNotReady);
	
	fmTx.SetRadiotextPlusL(text, tag);
	}

void CHWRMTest::FmTxSetRadiotextTag3L()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRadiotextTag3L"));
	
	TBuf<64> text;	
	text.Copy( _L("RTplus tag") );

	TRtPlusTag tag;
	tag.iContentType = ERtPlusItemTitle;
    tag.iStartMarker = 64; // NOT valid. less than lenghtMarker
    tag.iLengthMarker = text.Length();

	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	HWRMFmTxCommand::TRtData rtData;
	rtData.iText.Copy(text);
	rtData.iTag1.iContentType = tag.iContentType;
	rtData.iTag1.iStartMarker = tag.iStartMarker;
	rtData.iTag1.iLengthMarker = tag.iLengthMarker;
	rtData.iTag2.iContentType = ERtPlusInfoOther;
	rtData.iTag2.iStartMarker = 0;
	rtData.iTag2.iLengthMarker = 0;

	ExpectLeave(KErrArgument);
	
	// First try setting data with invalid content type
	fmTx.SetRadiotextPlusL(text, tag);
	}

void CHWRMTest::FmTxSetRadiotextTags1L()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRadiotextTags1L"));
	
	TBuf<64> text;	
	text.Copy( _L("RTplus tag") );

	TRtPlusTag tag;
	TRtPlusTag tag2;
    tag.iContentType = ERtPlusGetData;
    tag.iStartMarker = 0;
    tag.iLengthMarker = text.Length();
    tag2.iContentType = ERtPlusGetData;
    tag2.iStartMarker = 0;
    tag2.iLengthMarker = 100; // not valid
    
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();

	HWRMFmTxCommand::TRtData rtData;
	rtData.iText.Copy(text);
	rtData.iTag1.iContentType = tag.iContentType;
	rtData.iTag1.iStartMarker = tag.iStartMarker;
	rtData.iTag1.iLengthMarker = tag.iLengthMarker;
	rtData.iTag2.iContentType = tag2.iContentType;
	rtData.iTag2.iStartMarker = tag2.iStartMarker;
	rtData.iTag2.iLengthMarker = tag2.iLengthMarker;

	ExpectLeave(KErrArgument);
	
	// First try setting data with invalid content type
	fmTx.SetRadiotextPlusL(text, tag, tag2);
	}

void CHWRMTest::FmTxSetRadiotextTags2L()
	{
	INFO_PRINTF1(_L("Step state: FmTxSetRadiotextTags2L"));
	
	TBuf<64> text;	
	text.Copy( _L("RTplus tag") );

	TRtPlusTag tag;
	TRtPlusTag tag2;
    tag.iContentType = ERtPlusGetData;
    tag.iStartMarker = 0;
    tag.iLengthMarker = text.Length();
    tag2.iContentType = ERtPlusGetData;
    tag2.iStartMarker = 0;
    tag2.iLengthMarker = text.Length();
    
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	HWRMFmTxCommand::TRtData rtData;
	rtData.iText.Copy(text);
	rtData.iTag1.iContentType = tag.iContentType;
	rtData.iTag1.iStartMarker = tag.iStartMarker;
	rtData.iTag1.iLengthMarker = tag.iLengthMarker;
	rtData.iTag2.iContentType = tag2.iContentType;
	rtData.iTag2.iStartMarker = tag2.iStartMarker;
	rtData.iTag2.iLengthMarker = tag2.iLengthMarker;

	// First try setting data with invalid content type
	fmTx.SetRadiotextPlusL(text, tag, tag2);
	}

void CHWRMTest::FmTx2StatusEnabledL()
	{	
	INFO_PRINTF1(_L("Step state: FmTx2StatusEnabledL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionL(1);
	
	TFmTxState currentStatus = fmTx.Status();	
		
	if (currentStatus != EFmTxStateInactive && currentStatus !=  EFmTxStateActive)
		{
		User::Leave(KErrGeneral);
		}
	}
	
void CHWRMTest::FmTx2StatusOffL()
	{	
	INFO_PRINTF1(_L("Step state: FmTx2StatusOffL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionL(1);
	
	TFmTxState currentStatus = fmTx.Status();	
		
	if (currentStatus != EFmTxStateOff)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTx2DisableFromOnL()
	{	
	INFO_PRINTF1(_L("Step state: FmTx2DisableFromOnL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionL(1);
			
	ExpectFmTxStatusNotificationL(EFmTxStateOff); 
	
	fmTx.DisableL();
	}

void CHWRMTest::FmTxDisableFromOnL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxDisableFromOnL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	ExpectFmTxStatusNotificationL(EFmTxStateOff);
			
	fmTx.DisableL();
	}
	

void CHWRMTest::FmTxReleaseL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxReleaseL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	fmTx.Release();
	}

void CHWRMTest::FmTx2EnableExpectKErrInUseAndNotificationL()
	{	
	INFO_PRINTF1(_L("Step state: FmTx2EnableExpectKErrInUseAndNotificationL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionL(1);
	TInt frequency = 0;
	
	ExpectFmTxStatusNotificationL(EFmTxStateInactive, EFmTxStateActive);
	ExpectLeave(KErrInUse);
	
	fmTx.EnableL(frequency);
	}

void CHWRMTest::FmTx2EnableNoNotificationL()
	{	
	INFO_PRINTF1(_L("Step state: FmTx2EnableNoNotificationL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionL(1);
	TInt frequency = 0;

	fmTx.EnableL(frequency);
	}

#ifdef INSECURE_AUDIO_POLICY_KEYS

void CHWRMTest::FmTxCheckPowerSaveStateL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxCheckPowerSaveStateL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	TFmTxState currentState = fmTx.Status();
	if (currentState != iTempFmTxState)
		{
		User::Leave(KErrGeneral);
		}
	}


void CHWRMTest::FmTxAudioRoutingOnL()
	{
	INFO_PRINTF1(_L("Step state: FmTxAudioRoutingOnL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	TFmTxState currentState  = fmTx.Status();
	if (currentState == EFmTxStateInactive)
		{
		iTempFmTxState=EFmTxStateActive;
		//Since State changed we are going to receive a notification
		ExpectFmTxStatusNotificationL(iTempFmTxState);
		}
	else
		{
		iTempFmTxState=currentState;
		}
	
	SetAudioRoutedFlagL(ETrue);
	}

void CHWRMTest::FmTxEnableDuringPowersaveL()
	{
	INFO_PRINTF1(_L("Step state: FmTxEnableDuringPowersaveL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	ExpectLeave(KErrNotReady); //mutually exclusive accessory is connected
	ExpectFmTxStatusNotificationL(EFmTxStatePowerSaveInactivity, EFmTxStatePowerSaveAccessory);
	
	fmTx.EnableL();	 //defaults to the last used frequency
	}


void CHWRMTest::FmTxEnableDuringPowersave2L()
	{
	INFO_PRINTF1(_L("Step state: FmTxEnableDuringPowersave2L"));
	
	//used after another EnableL command while in Powersave. For extra conditional coverage
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	ExpectLeave(KErrNotReady); //mutually exclusive accessory is connected
	
	fmTx.EnableL();	 //defaults to the last used frequency
	}

void CHWRMTest::FmTxCheckExpectedAudioRoutingStateL()
	{	
	INFO_PRINTF1(_L("Step state: FmTxCheckExpectedAudioRoutingStateL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	
	if (fmTx.Status() != iTempFmTxState)
		{
		User::Leave(KErrGeneral);
		}
	}

void CHWRMTest::FmTxAudioRoutingOffL()
	{
	INFO_PRINTF1(_L("Step state: FmTxAudioRoutingOffL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	TFmTxState currentState  = fmTx.Status();
	
	if (currentState == EFmTxStateActive)
		{
		iTempFmTxState=EFmTxStateInactive;
		//Since State changed we are going to receive a notification
		ExpectFmTxStatusNotificationL(iTempFmTxState);
		}
	else
		{
		iTempFmTxState=currentState;
		}
	
	SetAudioRoutedFlagL(EFalse);
	}

void CHWRMTest::FmTxPowerSaveOnAccessoryL()
	{
	INFO_PRINTF1(_L("Step state: FmTxPowerSaveOnAccessoryL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	TFmTxState currentState  = fmTx.Status();

	if (currentState == EFmTxStateOff)
		{
		iTempFmTxState = currentState;
		}
	else
		{
		//Since State changed we are going to receive a notification
		iTempFmTxState = EFmTxStatePowerSaveAccessory;
		ExpectFmTxStatusNotificationL(iTempFmTxState);
		}
	// mimic watcher plugin behaviour, after headset is attached

	SetWatcherPluginConnectedStatusL(ETrue);
	}

void CHWRMTest::FmTxWatcherPluginConnectL()
	{
	INFO_PRINTF1(_L("Step state: FmTxWatcherPluginConnectL"));
	
	SetWatcherPluginConnectedStatusL(ETrue);
	}

void CHWRMTest::FmTxWatcherPluginDisconnectL()
	{
	INFO_PRINTF1(_L("Step state: FmTxWatcherPluginDisconnectL"));

	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	TFmTxState currentState  = fmTx.Status();

	if (currentState != EFmTxStatePowerSaveAccessory )
		{
		iTempFmTxState = currentState;		
		}
	else
		{
		iTempFmTxState = EFmTxStateOff;
		//Since State changed we are going to receive a notification
		ExpectFmTxStatusNotificationL(iTempFmTxState);
		}
	
	SetWatcherPluginConnectedStatusL(EFalse);
	}

void CHWRMTest::FmTxExpectPowerSaveOnInactivityL()
	{
	INFO_PRINTF1(_L("Step state: FmTxExpectPowerSaveOnInactivityL"));
	
	//Since State changed we are going to receive a notification
	iTempFmTxState = EFmTxStatePowerSaveInactivity;
	ExpectFmTxStatusNotificationL(iTempFmTxState);
	}

void CHWRMTest::FmTxExpectPowerSaveOffL()
	{
	INFO_PRINTF1(_L("Step state: FmTxExpectPowerSaveOffL"));
	
	//Since State changed we are going to receive a notification
	iTempFmTxState = EFmTxStateOff;
	ExpectFmTxStatusNotificationL(iTempFmTxState);
	}

void CHWRMTest::FmTxPowerSaveOffL()
	{
	INFO_PRINTF1(_L("Step state: FmTxPowerSaveOffL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
		
	TFmTxState currentState  = fmTx.Status();

	if (currentState != EFmTxStatePowerSaveAccessory )
		{
		iTempFmTxState = currentState;		
		}
	else
		{
		iTempFmTxState = EFmTxStateOff;
		//Since State changed we are going to receive a notification
		ExpectFmTxStatusNotificationL(iTempFmTxState);
		}
	           
	SetWatcherPluginConnectedStatusL(EFalse);
	}

void CHWRMTest::FmTxStatusPowersaveL()
	{
	INFO_PRINTF1(_L("Step state: FmTxStatusPowersaveL"));
	
	CHWRMFmTx& fmTx = GetFmTxSessionWithCallbackRegisteredL();
	TFmTxState currentState  = fmTx.Status();
		
	if (currentState != EFmTxStatePowerSaveInactivity && currentState != EFmTxStatePowerSaveAccessory)
		{
		User::Leave(KErrGeneral);
		}
	}

// -----------------------------------------------------------------------------
// CHWRMTest::SetWatcherPluginConnectedStatusL
// Called from test cases.
// -----------------------------------------------------------------------------
//
void CHWRMTest::SetWatcherPluginConnectedStatusL(TBool aStatus )
	{	
	INFO_PRINTF2(_L("CHWRMTest::SetWatcherPluginConnectedStatusL - aStatus = %d"), aStatus);
	
	TInt setErr = RProperty::Set(TUid::Uid(KTestCategory),KTestKeyNotifyChange,(TInt)aStatus);
	
	User::LeaveIfError(setErr);		
	}


// -----------------------------------------------------------------------------
// CHWRMTest::SetAudioRoutedFlagL
// Called from test cases.
// -----------------------------------------------------------------------------
//
void CHWRMTest::SetAudioRoutedFlagL(TBool aFlag)
	{
	INFO_PRINTF2(_L("CHWRMTest::SetAudioRoutedFlagL - aFlag = %d"), aFlag);
	
	User::LeaveIfError(iAudioRoutedProperty.Set((TInt)aFlag));
	}	
#endif // INSECURE_AUDIO_POLICY_KEYS	


//
// THE FOLLOWING STEPS AND STATES TEST LIGHT API DEPENDING ON 
// PRODUCT-SPECIFIC PHYSICAL DEVICE CONFIGURATION (FLIP, SLIDER, ETC.
//
// THEY ARE NOT CURRENTLY USED, BUT MAY BE USEFUL IN FUTURE
//

/*	


void CHWRMTest::TestFlipOpenedLightsOnL()
	{
	INFO_PRINTF1(_L("Start test step: TestFlipOpenedLightsOnL"));
	AddLightPreparationStatesL(ETrue);
	
	AddTestStateL(this, &CHWRMTest::SystemTargetLightingInitialiseL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FlipOpenedLightsOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FlipClosedLightsOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GripOpenedLightsOnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FlipClosedGripOpenedL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FlipOpenedLights2OnL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GripClosedKEFSimulationOnlyL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FlipClosedKEFSimulationOnlyL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GripOpenedKEFSimulationOnlyL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FlipOpenedReservedLightsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::ReleaseSystemTargetL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::SystemLightsOn3SecsL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FlipClosedWaitTimerL, KTwoTimeUnits);	
	ExecuteTestL();	
	}

void CHWRMTest::TestFlipClosedLightsOnL()
	{
	INFO_PRINTF1(_L("Start test step: TestFlipClosedLightsOnL"));	
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::SystemTargetLightingInitialiseL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::FlipClosedLightsOnL, KTwoTimeUnits);
	ExecuteTestL();	
	}

void CHWRMTest::TestGripOpenedLightsOnL()
	{
	INFO_PRINTF1(_L("Start test step: TestGripOpenedLightsOnL"));	
	AddLightPreparationStatesL();
	
	AddTestStateL(this, &CHWRMTest::SystemTargetLightingInitialiseL, KTwoTimeUnits);
	AddTestStateL(this, &CHWRMTest::GripOpenedLightsOnL, KTwoTimeUnits);
	ExecuteTestL();	
	}
*/

/*
void CHWRMTest::SystemLightsInitialiseL()
	{
	//System target lightning initializations: flip open, grip open, sensor in use
	INFO_PRINTF1(_L("Step state: SystemLightsInitialiseL"));
		
	// Set flags
	SetLightControlAttributes(50, 55, 1);
    RProperty::Set(KPSUidHWRM, KHWRMFlipStatus, EPSHWRMFlipOpen);
    RProperty::Set( KPSUidHWRM, KHWRMGripStatus, EPSHWRMGripOpen );
    
    User::After(2*KOneSecond);
    
    CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();	
    // Sensor enabling will cause lights on so depending on execution order of things, there might come failures before 
    // this expected status can be set.    
    
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplayAndKeyboard | CHWRMLight::ESecondaryDisplayAndKeyboard, CHWRMLight::ELightOff);
    light.LightOffL(CHWRMLight::ESystemTarget, 0);	
	}
	

	
void CHWRMTest::FlipOpenedLightsOnL()
	{
	//Flip opened, switch system lights on
	INFO_PRINTF1(_L("Step state: FlipOpenedLightsOnL"));

	RProperty::Set(KPSUidHWRM, KHWRMFlipStatus, EPSHWRMFlipOpen);
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();	
	light.LightOnL(CHWRMLight::ESystemTarget, 0);

	// KEF plugin simulation
	// NOT SUPPORTED: 
	light.LightOffL(CHWRMLight::ESecondaryDisplayAndKeyboard, 0);

	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplayAndKeyboard, CHWRMLight::ELightOn);
	ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplayAndKeyboard, CHWRMLight::ELightOff);
	}

void CHWRMTest::FlipClosedLightsOnL()
	{
	//Flip closed, switch system lights on
	INFO_PRINTF1(_L("Step state: FlipClosedLightsOnL"));

	RProperty::Set(KPSUidHWRM, KHWRMFlipStatus, EPSHWRMFlipClosed);
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();	
	light.LightOnL(CHWRMLight::ESystemTarget, 0);

	// KEF plugin simulation
	// NOT SUPPORTED:
	light.LightOffL(CHWRMLight::ESecondaryDisplayAndKeyboard, 0);

	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplayAndKeyboard, CHWRMLight::ELightOn);
	ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplayAndKeyboard, CHWRMLight::ELightOff);
	}

void CHWRMTest::GripOpenedLightsOnL()
	{
	//Grip opened, switch system lights on
	INFO_PRINTF1(_L("Step state: GripOpenedLightsOnL"));

	RProperty::Set(KPSUidHWRM, KHWRMGripStatus, EPSHWRMGripOpen);
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();	
	light.LightOnL(CHWRMLight::ESystemTarget, 0);

   	// Grip opening will not shut targets off when fold is closed
   	// KEF plugin simulation
   	// NOT SUPPORTED
	light.LightOffL(CHWRMLight::ESecondaryDisplayAndKeyboard, 0);
	}

void CHWRMTest::FlipClosedGripOpenedL()
	{
	//Reserve system target, state now: Flip closed, grip open
	INFO_PRINTF1(_L("Step state: FlipClosedGripOpenedL"));

	RProperty::Set(KPSUidHWRM, KHWRMGripStatus, EPSHWRMGripOpen);
	
	CHWRMLight& light = GetLightSessionWithCallbackRegisteredL();	
	light.ReserveLightL(CHWRMLight::ESystemTarget, EFalse, ETrue);

    // All target states should be same as previously so no notification is received.
	}

void CHWRMTest::FlipOpenedLights2OnL()
	{
	//Flip opened, Unreserved switch system lights on
	INFO_PRINTF1(_L("Step state: FlipOpenedLights2OnL"));

	RProperty::Set(KPSUidHWRM, KHWRMFlipStatus, EPSHWRMFlipOpen);
	
	CHWRMLight& light2 = GetLightSessionL(1);
	// NOT RECEIVED
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplayAndKeyboard, CHWRMLight::ELightOn);	
	light2.LightOnL(CHWRMLight::ESystemTarget, 0);

	// KEF plugin simulation
	// NOT SUPPORTED:
	ExpectLeave(KErrInUse);
	light2.LightOffL(CHWRMLight::ESecondaryDisplayAndKeyboard, 0);  

    // Reservation activations and deactivations should change lights to expected states
    ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplayAndKeyboard, CHWRMLight::ELightOn);
    ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplayAndKeyboard, CHWRMLight::ELightOff);
	}

void CHWRMTest::GripClosedKEFSimulationOnlyL()
	{
	INFO_PRINTF1(_L("Step state: GripClosedKEFSimulationOnlyL"));

	RProperty::Set( KPSUidHWRM, KHWRMGripStatus, EPSHWRMGripClosed);
	
	CHWRMLight& light2 = GetLightSessionL(1);	
	
	// KEF plugin simulation
	// NOT RECEIVED:
	//ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOff);
	ExpectLeave(KErrInUse);
	light2.LightOffL(CHWRMLight::EPrimaryKeyboard, 0); 
	
    // same as previously - ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOn);
    // same as previously - ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplayAndKeyboard, CHWRMLight::ELightOff);
	}

void CHWRMTest::FlipClosedKEFSimulationOnlyL()
	{
	INFO_PRINTF1(_L("Step state: FlipClosedKEFSimulationOnlyL"));

	RProperty::Set(KPSUidHWRM, KHWRMFlipStatus, EPSHWRMFlipClosed);

	CHWRMLight& light2 = GetLightSessionL(1);	
	
	// KEF plugin simulation
	// NOT RECEIVED:
	ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff);
	ExpectLeave(KErrInUse);
	light2.LightOffL(CHWRMLight::EPrimaryDisplayAndKeyboard, 0);
	
	// KEF plugin simulation
	ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplayAndKeyboard, CHWRMLight::ELightOn);
	ExpectLeave(KErrInUse);
	light2.LightOnL(CHWRMLight::ESecondaryDisplayAndKeyboard, 0); 
	 
    ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff);
    // same as previously - ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOff);
    ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplayAndKeyboard, CHWRMLight::ELightOn);
	}

void CHWRMTest::GripOpenedKEFSimulationOnlyL()
	{
	INFO_PRINTF1(_L("Step state: GripOpenedKEFSimulationOnlyL"));

	RProperty::Set( KPSUidHWRM, KHWRMGripStatus, EPSHWRMGripOpen);
	
	CHWRMLight& light2 = GetLightSessionL(1);	

	// Grip opening will not turn targets on when fold is closed, but try to light the keyboard anyway	
	// KEF plugin simulation
	ExpectLeave(KErrInUse); // N.B this is not expected in original test
	light2.LightOnL(CHWRMLight::EPrimaryKeyboard, 0);
	
    // All target states should be same as previously
    //ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff);
    //ExpectLightStatusNotificationL(CHWRMLight::EPrimaryKeyboard, CHWRMLight::ELightOff);
    //ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplayAndKeyboard, CHWRMLight::ELightOn);	
 	}

void CHWRMTest::FlipOpenedReservedLightsL()
	{
	//Flip opened, Reserved system lights call
	INFO_PRINTF1(_L("Step state: FlipOpenedReservedLightsL"));

	RProperty::Set(KPSUidHWRM, KHWRMFlipStatus, EPSHWRMFlipOpen);

	CHWRMLight& light = GetLightSessionL(0);	
	//ExpectLightStatusNotificationL(CHWRMLight::EPrimaryDisplayAndKeyboard, CHWRMLight::ELightOn)
	light.LightOnL(CHWRMLight::ESystemTarget, 0);
	
	// KEF plugin simulation
	// NOT RECEIVED:
	ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplayAndKeyboard, CHWRMLight::ELightOff);
	// NOT SUPPORTED:
	light.LightOffL(CHWRMLight::ESecondaryDisplayAndKeyboard, 0);
	}	
	
void CHWRMTest::FlipClosedWaitTimerL()
	{
	//After 1 seconds close flip and wait for timer to expire
	INFO_PRINTF1(_L("Step state: FlipClosedWaitTimerL"));

	RProperty::Set(KPSUidHWRM, KHWRMFlipStatus, EPSHWRMFlipClosed);
	
   	// primaries are not affected as they are inactivated
    ExpectLightStatusNotificationL(CHWRMLight::ESecondaryDisplayAndKeyboard, CHWRMLight::ELightOn);

	User::After(2500000);
	}

void CHWRMTest::ReleaseSystemTargetL()
	{
	INFO_PRINTF1(_L("Step state: ReleaseSystemTargetL"));

	CHWRMLight& light = GetLightSessionL(0);	
	light.ReleaseLight(CHWRMLight::ESystemTarget);

  	// All target states should be same as previously, so no change notifications should be produced
	}
	
void CHWRMTest::SystemLightsOn3SecsL()
	{
	INFO_PRINTF1(_L("Step state: SystemLightsOn3SecsL"));

	CHWRMLight& light = GetLightSessionL(0);	
	light.LightOnL(CHWRMLight::ESystemTarget, 3000);

  	// All target states should be same as previously, so no change notifications should be produced
	}
	
*/

//END
