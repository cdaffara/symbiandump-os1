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

#ifndef hwrmtest_h
#define hwrmtest_h

#include "hwrmtestbase.h"
#include <hwrmvibra.h>
#include <hwrmpower.h>
#include <hwrmlight.h>
#include <hwrmfmtx.h>
#include <rmocklight.h>
#include <rmockvibra.h>
#include <rmockpower.h>
#include <rmockfmtx.h>
#include <hwrmfmtxaudiopolicydomainpskeys.h>

class CHWRMTest : public CHWRMTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);
		
public:
	//
	// Test Steps
	//
	// Vibra testing
	void TestVibraOnL();
	void TestVibraOnPluginResponseErrorL();
	void TestVibraOnWithIntensityL();
	void TestVibraOnWithIntensityOutOfRangeL();
	void TestVibraOnWithDurationOutOfRangeL();
	void TestVibraOnDuringForcedOffPeriodL();
	void TestVibraOnAfterForcedOffPeriodL();
	void TestVibraGetStatusL();	
	void TestVibraOnInfiniteL();
	void TestVibraProfileL();
	void TestVibraOnLoopL();

	void TestVibraReserveSessionsL();
	
	//PREQ1944
	void TestStartVibraOverlapPulseL();
	void TestStartVibraOverlapPulsesL();
	void TestPulseOverlapPulsesL();
	void TestSequentialPulsesL();
	void TestUnSupportedPulseIntensityL();
	void TestUnSupportedPulseDurationL();
	void TestPulseNotAllowedL();
	void TestReserveStartPulseVibraL();
	void TestReserveVibraL();
	
	// Vibra profile skipping tests (privileged client)
	void TestVibraProfilePrivilegedClientL();
	void TestVibraProfilePrivilegedClient2L();
	
	//Light testing
	void TestSetupLightsL();
	void TestLightSimpleL();
	void TestLightBlinkKeyboardL();
	void TestLightBlinkComplexL();
	void TestLightOnDisplayAndKeyboardL();
	void TestLightBlinkDisplayWithLtdDurationL();
	void TestLightOffKeyboardWithLtdDurationL();
	void TestLightOffDisplayWithOverrideTimerL();
	void TestLightBlinkKeyboardWithOverrideTimerL();
	void TestLightBlinkDisplayAndKeyboardL();
	void TestLightBlinkKeyboardInfiniteCutOffTimerL();
	void TestLightGetStatusDisplayL();
	void TestLightSetSensorOnL();

	void TestLightReserveSessionsL();
	void TestLightReserveSessions2L();
	void TestLightReserveDisplayL();
		
	// Custom target tests
	void TestLightGetSupportedTargetsL();
	void TestLightUnsupportedTargetL();
	void TestLightUnsupportedDurationL();
	void TestLightUnsupportedIntensityL();
	void TestLightOnSimpleCustomTargetsL();

	// System target tests
	void TestModifyTargetL();
	void TestSystemLightsL();
	void TestDisplayLightsOnL();

	// PREQ 2615
	void TestLightColorL();
	void TestLightColor_ExtendedTargetL();
	void TestLightDefaultColorL();
	void TestLightDefaultColor_ExtendedTargetL();
	void TestLightColor_UnSupportedTargetL();
	void TestLightColor_PluginErrorL();
	void TestFrozenStateRestoreLightColor_ExtendedTargetL();
	void TestBaseStateRestoreLightColor_ExtendedTargetL();
	void TestLightColor_MultiClientL();
	void TestLightColor_MultiClient_ExtendedTargetL();
	void TestLightColor_WhenInUseL();
	void TestLightDefaultColor_WhenInUseL();
	void TestLightOnDurationRGBL();
    void TestLightOnDurationWithIntenistyRGBL();
	void TestLightBlinkDurationRGBL();
    void TestLightBlinkDurationWithIntensityRGBL();
	void TestLightOnDurationRGB_ExtendedL();
	void TestLightBlinkDurationRGB_ExtendedL();
    void TestLightEnhancedLightLeaveL();
    void TestLightSetColorReserveL();
	
	// Power State tests
	void TestPowerStatesL();
	void TestPowerExtendedBatteryStatusL();
	void TestPowerBatteryStatusL();
	void TestPowerChargingStatusL();
	
	// Extended Lights
	void TestExtendedLightSessionsL();
	
	void TestInvalidSessionsL();
	
	// Power
	void TestPowerConnectL();
	void TestPowerGetBatteryInfoL();
	void TestPowerGetBatteryInfoMultiClientL();
	void TestPowerGetBatteryVoltageInfoL();
	void TestPowerGetBatteryVoltageInfoMultiClientL();
	void TestPowerGetBaseTimeIntervalL();
	void TestPowerGetBatteryInfoWithPluginErrorL();
	void TestPowerGetBatteryVoltageInfoWithPluginErrorL();
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
	void TestPowerGetBatteryChargingStatusL();
	void TestPowerGetBatteryChargingStatusMultiClient3L();
	void TestPowerChargingStatusNotificationL();
	void TestPowerChargingStatusNotificationMultiClient3L();
	void TestPowerRemainingChargingTimeNotificationL();
	void TestPowerRemainingChargingTimeWithPluginErrorL();
	void TestPowerStopRemainingChargingTimeWithPluginErrorL();
	void TestPowerRemainingChargingTimeNotificationErrorL();
	void TestPowerRemainingChargingTimeWhenStartedL();
	void TestPowerRemainingChargingTimeNotificationMulticlient3L();
	void TestPowerChargingTimeNotificationWhenChargingCompleteL();
	void TestPowerChargingTimeNotificationWhenDisconnectedL();
	void TestPowerChargingTimeNotificationWhenChargingComplete3L();
	void TestPowerChargingTimeNotificationWhenDisconnected3L();
	void TestPowerChargingCurrentNotificationL();
	void TestPowerChargingCurrentNotificationErrorL();
	void TestPowerChargingCurrentWhenStartedL();
	void TestPowerChargingCurrentNotificationMulticlient3L();
	void TestPowerChargingCurrentNotificationWhenChargingCompleteL();
	void TestPowerChargingCurrentNotificationWhenDisconnectedL();
	void TestPowerChargingCurrentNotificationWhenChargingComplete3L();
	void TestPowerChargingCurrentNotificationWhenDisconnected3L();
	void TestPowerChargingCurrentWithPluginErrorL();
	void TestPowerStopChargingCurrentWithPluginErrorL();
	void TestPowerGetBatteryChargerType();
	void TestPowerGetBatteryChargerTypeMulticlientL();
	void TestPowerGetBatteryChargerTypeWithPluginErrorL();
	void TestPowerCancelChargingTimeWithoutStart();
	void TestPowerCancelChargingCurrentWithoutStart();
	void TestPowerChargingCurrentNotificationMaxPeriodReachedL();
	void TestPowerChargingCurrentNotificationSameIntervalL();
	void TestPowerChargingCurrentNotificationDifferentIntervalL();
	void TestPowerChargingCurrentandRemainingChargingTimeNotificationL();
	void TestPowerChargingandVoltageNotificationL();
	void TestPowerChargingCurrentNotificationSlowClientL();
	void TestPowerChargingCurrentNotificationSlowClientsL();
	void TestPowerChargingTimeNotificationWithoutCallbackL();
	void TestPowerChargingCurrentNotificationWithoutCallbackL();
    void TestPowerClientSetCallBackL();
    void TestPowerChargingCurrentNotificationMaxPeriodReachedMCL();
#endif //SYMBIAN_HWRM_EXTPOWERINFO

	void TestPowerNotifyL();
	void TestPowerNotify_InfiniteL();
	void TestPowerNotify_AdjustReportingPeriodL();
	void TestPowerNotify_BigIntervalL();
	void TestPowerNotify_MultiClient_SameIntervalL();
	void TestPowerNotify_MultiClient_DiffIntervalL();
	void TestPowerNotify_RedundantL();
	void TestPowerNotify_Start_PluginErrorL();
	void TestPowerNotify_Stop_PluginErrorL();
	void TestPowerNotify_IntervalEqualL();
	void TestPowerNotify_IntervalGreaterL();
	void TestPowerNotify_AdjustReportingPeriod_WithoutRequestL();
	void TestPowerNotify_AdjustReportingPeriod_ElapsedTimeLessL();
	void TestPowerNotify_AdjustReportingPeriod_ElapsedTimeMoreL();
	void TestPowerNotify_AdjustReportingPeriod_NonZeroToZeroL();
	void TestPowerNotify_AdjustReportingPeriod_ZeroToNonZeroL();
	void TestPowerNotify_AdjustReportingPeriod_SameValueL();
	void TestPowerNotify_AdjustReportingPeriod_NegativeValueL();
	void TestPowerNotify_SlowClientL();
	void TestPowerNotify_WithoutCallbackL();
	void TestPowerNotify_MultiClient_DiffInterval_SpecialCaseL();
	void TestPowerNotify_Stop_WithoutStartL();
	void TestPowerNotify_Stop_RedundantL();
    void TestPowerNotify_Start_Set_PluginErrorL();
    void TestPowerNotify_All_AdjustReportingPeriod_NegativeValueL();

	// FmTx Testing
	void TestFmTxConnectL();
	void TestFmTxStatusOffL();
	void TestFmTxEnableInvalidFreqL();
	void TestFmTxEnableL();
	void TestFmTxEnableNewFreqL();
	void TestFmTxEnableInvalidFreqWhenOnL();
	void TestFmTxReserveL();
	void TestFmTxEnableDuringReservationL();
	void TestFmTxDisableL();
	void TestFmTxDisableDuringReservationL();
	void TestFmTxReserveDuringReservationL();
	void TestFmTxGetFrequencyL();
	void TestFmTxSetFrequencyL();
	void TestFmTxSetFrequencyInvalidL();
	void TestFmTxSetFrequencyDuringReservationL();
	void TestFmTxGetFrequencyRangeL();
	void TestFmTxGetJapanFrequencyRangeL();
	void TestFmTxGetNextClearFrequencyL();
	void TestFmTxGetNextClearFrequencyWhileDisabledL();
	void TestFmTxGetNextClearFrequencyAsyncL();
	void TestFmTxGetNextClearFrequencyAsyncWhileDisabledL();
	void TestFmTxGetNextClearFrequencyAsyncCancelL();
	void TestFmTxGetNextClearFrequencyArrayL();
	void TestFmTxGetNextClearFrequencyArrayWhileDisabledL();
	void TestFmTxGetNextClearFrequencyAsyncArrayL();
	void TestFmTxGetNextClearFrequencyAsyncArrayWhileDisabledL();
	void TestFmTxSetNextClearFrequencyL();
	void TestFmTxSetNextClearFrequencyWhileDisabledL();
	void TestFmTxSetNextClearFrequencyAsyncL();
	void TestFmTxSetNextClearFrequencyAsyncWhileDisabledL();
	void TestFmTxSetNextClearFrequencyAsyncCancelL();
	void TestFmTxSetNextClearFrequencyAsyncCancelWhileDisabledL();
	void TestFmTxAutoFrequencyL();
	void TestFmTxSetRdsDataL();
	void TestFmTxSetRdsDataWhileDisabledL();
	void TestFmTxSetRadiotextL();
	void TestFmTxSetRadiotextWhileDisabledL();
	void TestFmTxSetRadiotextContentL();
	void TestFmTxSetRadiotextTagL();
	void TestFmTxSetRadiotextTagWhileDisabledL();
	void TestFmTxSetRadiotextTagsL();
	void TestFmTxSetRadiotextTagsWhileDisabledL();

#ifdef INSECURE_AUDIO_POLICY_KEYS    
	void TestFmTxEnableDuringPowersaveL(); 
	void TestFmTxAudioRoutingOnAndOffL();
	void TestFmTxPowerSaveOnAccessoryL();
	void TestFmTxPowerSaveOnAccessoryWhileDisabledL();
	void TestFmTxPowerSaveOnInactivityStateL();
	void TestFmTxPowerSaveOnOffStateL();
	void TestFmTxPowerSaveOffL();	
#endif // INSECURE_AUDIO_POLICY_KEYS  	

private:
	//
	// Test States
	//
	void VibraOpenL();
	void VibraPulseOpenL();
	void VibraPulseOpenSessionsL();
	void PulseVibraL();	
	void MultVibraPulsesL();
	void PulsesOverlapPulsesL();
	void SequentialPulsesL();
	void UnSupportedPulseIntensity1L();
	void UnSupportedPulseIntensity2L();
	void UnSupportedPulseIntensity3L();
	void UnSupportedPulseDuration1L();
	void UnSupportedPulseDuration2L();
	void UnSupportedPulseDuration3L();
	void UnSupportedPulseDuration4L();
	void DoPulseVibraL(CHWRMVibra& aVibra, TInt aDuration, TInt aIntensity);
	void DoPulseVibraL(CHWRMVibra& aVibra, TInt aDuration);
	void DoPulseVibraL(CHWRMVibra& aVibra);
	void VibraOpenWhileDisabledL();
	void VibraOnL();
	void VibraOnNoNotificationExpectedL();
	void VibraOffL();
	void VibraFeedbackSettingsL();
	void VibraOffWhenDisabledL();	
	void VibraOnWithResponseErrL();
	void VibraOnWithIntensityL();
	void VibraOnWithIntensityOutOfRangeL();
	void VibraOnWithIntensityOutOfRange2L();
	void VibraOnWithDurationOutOfRangeL();
	void VibraOnWithDurationOutOfRange2L();
	void VibraOnWithDurationOutOfRange3L();
	void VibraOnWithDurationOutOfRange4L();
	void VibraOnWithDurationGreaterThanMaximumL();
	void VibraOnDuringForcedOffPeriodL();
	void VibraSetMinTimeOffL();
	void VibraRestoreDefaultMinTimeOffL();
	void VibraSetMaxTime10SecsOnL();
	void VibraSetMaxTime5SecsOnL();
	void VibraGetStatusOnL();
	void VibraGetStatusOffL();
	void VibraOn5SecsL();
	void WaitOneSecondL();
	void CheckForVibraStatusStoppedL();
	void WaitOneTenthL();
	void VibraWaitForCutOffL();
	void VibraWaitForCoolOffL();
	void VibraOnInfiniteL();
	void VibraDisableProfileL();
	void VibraPulseDisableProfileL();
	void VibraPrivilegedDisableProfileL();
	void VibraOnWhileDisabledL();
	void PulseOnWhileDisabledL();
	void VibraEnableProfileL();
	void VibraPulseEnableProfileL();
	void StartVibra5L();
	void PulseVibraWithNoStatusUpdateL();
	void PulseVibraWithNoStatusUpdate0L();
	void VibraOnLoopL();
	void VibraOpenSessionsL(); //multiple sessions
	
	void InitialiseLightsL();
	void LightOpenL();
    void LightOpenSessionsL();
    
    void WaitForTargetToGoOffL();
    void WaitForTargetToGoOnL();
    
    void LightOnAllTargetsL();
	void LightOffAllTargetsL();
	void LightOnDisplayL();
	void LightOnDisplay_LimitedDurationL();
	void LightOnDisplay_NoStatusChangeL();
	void LightOffDisplayL();
	
	void LightOnKeyboardL();
	void LightOffKeyboardL();
	void LightOnDisplayAndKeyboardL();
	void LightOffDisplayAndKeyboardL();
	void LightOnDisplayAndKeyboard_LimitedDurationL();
	void LightOnDisplayAndKeyboard_NoStatusChangeL();
	void LightBlinkKeyboardWithLimitedDurationL();
	void LightBlinkDisplayWithLimitedDurationL();
	void LightOffKeyboardWithLimitedDurationL();
	void LightOffDisplayWithOverrideTimerL();
	void LightBlinkKeyboardWithOverrideTimerL();
	void LightBlinkDisplayAndKeyboardWithLimitedDuration_KeyboardNoChangeL();
	void LightBlinkKeyboardInfiniteCutOffTimerL();
	void LightGetStatusKeyboardL();
	void LightGetStatusDisplayL();	
	void LightSetSensorOnL();
	void LightGetSensorOnL();
	void LightSetSensorOffL();
	void LightBlinkWithDefaultsL();
	void ResetIntensityAndLightTimeOutL();
	void LightBlinkCycleDurationDefaultMismatchL();
	void VibraReserveL();
	void VibraReserveDefaultParamsL();
	void VibraTryStartL();
	void Vibra2TryStartL();
	void Vibra2ReserveL();
	void Vibra3ReserveL();
	void VibraReleaseL();
	void Vibra2ReleaseL();
	void LightReserveDefaultParamsL();
	void LightReserveDisplayL();
	void LightReleaseReservedDisplayL();
	void LightReleaseUnreservedKeyboardL();
	void Light2ReserveKeyboardL();
	void Light3ReserveDisplayAndKeyboardL();
	void LightTryBlinkDisplayAndKeyboardL();
	void Light2OnDisplayAndKeyboardL();
	void Light3TryBlinkDisplayAndKeyboardL();
	void Light2ReleaseUnreservedDisplayAndReservedKeyboardL();
	void LightTryBlinkDisplayL();
	void Light3OnKeyboardL();
	void Light3ReleaseSuspendedDisplayAndReservedKeyboardL();
	void LightReleaseReservedDisplayAndUnreservedKeyboardL();
	void LightReserveDisplayAndKeyboardL();
	void LightGetSupportedTargetsL();
	void LightReserveUnsupportedTargetL();
	void LightOnUnsupportedTargetL();
	void LightReleaseUnsupportedTargetL();
	void LightOnSimpleCustom1L();
	void LightOnSimpleCustom2L();
	void LightOnSimpleCustom4L();
	void LightOnUnsupportedDurationUpperBoundL();
	void LightOnUnsupportedDurationLowerBoundL();
	void LightOnUnsupportedIntensityLowerBoundL();
	void LightOnUnsupportedIntensityUpperBoundL();
	void VibraPrivilegedStartDisabled1L();
	void VibraPrivilegedStartDisabled2L();
	
	void SystemTargetLightingInitialiseL();
	
	void SystemLightsInitialiseL();
	void SystemLightsOnL();
	void SystemLightsOffL();
	void DisplayLightsOnL();
	void KeyboardLightsOnDisplayOffL();

	void ExpectSetColorCommand(TInt aTarget, TUint8 aRed, TUint8 aGreen, TUint8 aBlue, TBool aDefaultColorCmd, TInt aErrorCode, TUint delay=2);
	void DisplayLightColorL();
	void DisplayAndKeyboardLightColorL();
	void DisplayLightColor_ExtendedL();
	void DisplayAndKeyboardLightColor_ExtendedL();
	void DisplayLightDefaultColorL();
	void DisplayAndKeyboardLightDefaultColorL();
	void DisplayLightDefaultColor_ExtendedL();
	void DisplayAndKeyboardLightDefaultColor_ExtendedL();
	void LightColorUnSupportedTargetL();
	void LightColorPluginErrorL();
	void ReserveDisplayWithfrozenstateOnL();
	void ReleaseDisplay_ExtendedL();
	void ReserveDisplayWithfrozenstateOn_RestoreColorL();
	void ReserveDisplay_Client1L();
    void ReserveDisplay_Client2L();
	void ReserveDisplay_Client1_ExtL();
	void DisplayLightColor_Client1L();
    void DisplayLightOnColor_Client2L();
	void DisplayLightColor_Client1_ExtL();
	void ReserveKeyboard_Client2L();
	void ReserveKeyboard_Client2_ExtL();
	void KeyboardLightColor_Client2L();
	void KeyboardLightColor_Client2_ExtL();
	void ReserveDisplayKeyboard_Client3L();
	void ReserveDisplayKeyboard_Client3_ExtL();
	void DisplayKeyboardLightColor_Client3L();
	void DisplayKeyboardLightColor_Client3_ExtL();
	void ReleaseDisplayKeyboard_Client2L();
	void ReleaseDisplayKeyboard_Client2_ExtL();
	void ReleaseDisplayKeyboard_Client3L();
	void ReleaseDisplayKeyboard_Client3_ExtL();
	void ReleaseDisplayKeyboard_Client1L();
	void ReleaseDisplayKeyboard_Client1_ExtL();
	void DisplayKeyboardLightColor_Client1_ErrorL();
	void DisplayKeyboardLightDefaultColor_Client1_ErrorL();	
	void SetClientColorL();
	void RestoreClientColorL();
	void SetClientColor_ExtendedL();
	void RestoreClientColor_ExtendedL();
	void LightOnDurationRGBL();
    void LightOnDurationWithIntensityRGBL();
	void LightBlinkDurationRGBL();	
    void LightBlinkDurationWithIntensityRGBL();	
	void LightOnDurationRGB_ExtendedL();
    void LightOnDurationRGB_ExtendedWithExpectL();
	void LightBlinkDurationRGB_ExtendedL();
    void LightEnhancedLeave1L();
    void LightEnhancedLeave2L();
    void LightEnhancedLeave3L();
	
	void CloseFlipL();
	void OpenFlipL();
		
	void PowerStateOpenL();
	void ExpectLegacyBatteryStatusL();
    void ExpectLegacyChargingStatusL();
    void ExpectNewBatteryStatusL();
    void ExpectNewChargingStatusL();
	void BatteryLevelL();
	void BatteryStateL();
	void BatteryStateUnknownErrorL();
	void BatteryStateUnsupportedL();
	void BatteryStateAuthErrorL();
	void BatteryStateStatusOkL();
	void BatteryStateStatusLowL();
	void BatteryStateStatusEmptyL();
	void ExtendedBatteryStateUnknownErrorL();
	void ExtendedBatteryStateUnsupportedL();
	void ExtendedBatteryStateAuthErrorL();
	void ExtendedBatteryStateStatusOkL();
	void ExtendedBatteryStateStatusLowL();
	void ExtendedBatteryStateStatusEmptyL();
	void ChargingStatusL();
	void ChargingStatusNotConnectedL();
	void ChargingStatusChargingOkL();
	void ChargingStatusNotChargingL();
	void ChargingStatusAlmostCompleteL();
	void ChargingStatusCompletedChargingL();
	void ChargingStatusChargingContinuesL();
	void ChargingStatusErrorL();
	void ChargingStatusIllegalErrorL();
	void ChargingStatusUnderVoltageErrorL();
	void ChargingStatusOverVoltageErrorL();
	void ChargingStatusTooHotErrorL();
	void ChargingStatusTooColdErrorL();

	void DisableNotificationCheckingL();
	void EnableNotificationCheckingL();

	void OpenExtLightSessionL();
	void OpenEnhancedLightSessionL();
	void ExtendedLightOnL();
	void ExtendedLight2BlinkL();
	void ExtendedLightOffL();
	void ExtendedLight2OffL();
	void ExtendedLightOffAllTargetsL();
	void EnhancedLightOffAllTargetsL();
		
	void InvalidSessionLightL();
	void InvalidSessionExtendedLightL();
	void InvalidSessionVibraL();
	
	void AddLightPreparationStatesL(TBool aMultipleSessions = EFalse);
	
	void PowerGetCRAttributesL();
	void PowerOpenL();
	void PowerOpen2SessionsL();
	void PowerOpen3SessionsL();
	void PowerGetBatteryInfoL();
	void PowerGetBatteryInfoMultiClientL();
	void PowerGetBatteryInfoWithPluginErrorL();
	void PowerGetBatteryVoltageInfoL();
	void PowerGetBatteryVoltageInfoMultiClientL();
	void PowerGetBatteryVoltageInfoWithPluginErrorL();
	void PowerGetBaseTimeIntervalL();
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
	void PowerAndPowerStateOpenL();
	void PowerAndPowerState3SessionsOpenL();
	void PowerChargingStatusErrorL();
	void PowerChargingStatusIllegalErrorL();
	void PowerChargingStatusUnderVoltageErrorL();
	void PowerChargingStatusOverVoltageErrorL();
	void PowerChargingStatusTooHotErrorL();
	void PowerChargingStatusTooColdErrorL();	
	void PowerChargingStatusNotConnectedL();
	void PowerChargingStatusChargingL();
	void PowerChargingStatusNotChargingL();
	void PowerChargingStatusAlmostCompleteL();
	void PowerChargingStatusCompleteL();	
	void PowerChargingStatusContinuedL();	
	void PowerChargingStatusError3L();
	void PowerChargingStatusIllegalError3L();
	void PowerChargingStatusUnderVoltageError3L();
	void PowerChargingStatusOverVoltageError3L();
	void PowerChargingStatusTooHotError3L();
	void PowerChargingStatusTooColdError3L();	
	void PowerChargingStatusNotConnected3L();
	void PowerChargingStatusCharging3L();
	void PowerChargingStatusNotCharging3L();
	void PowerChargingStatusAlmostComplete3L();
	void PowerChargingStatusComplete3L();	
	void PowerChargingStatusContinued3L();	
	void CheckPowerChargingStatusL();
	void CheckPowerChargingStatusMultiClient3L();
	void RegisterChargingStatusCallbackL();
	void RegisterChargingStatusCallbackMultiClient3L();
	void ResetChargingStatusCBFlagL();
	void RegisterChargingTimeCallbackL();
	void RegisterChargingTimeCallbackMultiClient3L();
	void RegisterChargingCurrentCallbackL();
	void RegisterChargingCurrentCallbackMultiClient3L();
	void StartRemainingChargingTimeL();
	void StartRemainingChargingTime1L();
	void StartRemainingChargingTime2L();
	void CancelRemainingChargingTimeL();
	void CancelRemainingChargingTime1L();
	void CancelRemainingChargingTime2L();
	void StartRemainingChargingTimeWithPluginErrorL();
	void StopRemainingChargingTimeWithPluginErrorL();
	void StartRemainingChargingTimeErrorL();
	void StartRemainingChargingTimeWhenStartedL();
	void ChargingTimeNotificationWithErrorCompleteL();
	void ChargingTimeNotificationWithErrorDisconnectedL();
	void ChargingTimeNotificationWithErrorComplete3L();
	void ChargingTimeNotificationWithErrorDisconnected3L();
	void StartChargingCurrentL();
	void StartChargingCurrent1L();
	void StartChargingCurrent2L();
	void StartChargingCurrent4L();
	void CancelChargingCurrentL();
	void CancelChargingCurrent1L();
	void CancelChargingCurrent2L();
	void StartChargingCurrentErrorL();
	void StartChargingCurrentWhenStartedL();
	void ChargingCurrentNotificationWithChargingCompleteL();
	void ChargingCurrentNotificationWithErrorDisconnectedL();
	void ChargingCurrentNotificationWithErrorDisconnected3L();
	void StartChargingCurrentWithPluginErrorL();
	void StopChargingCurrentWithPluginErrorL();
	void GetBatteryChargerTypeNoCharger();
	void GetBatteryChargerTypeACMain();
	void GetBatteryChargerTypeUSB();
	void GetBatteryChargerTypeNoChargerMulticlientL();
	void GetBatteryChargerTypeACMainMulticlientL();
	void GetBatteryChargerTypeUSBMulticlientL();
	void GetBatteryChargerTypeUSBHispeedHost();
	void GetBatteryChargerTypeUSBHispeedHostMulticlientL();
	void GetBatteryChargerTypeUSBAca();
	void GetBatteryChargerTypeUSBAcaMulticlientL();	
	void GetBatteryChargerTypeWithPluginErrorL();
	void ChargingCurrentMaxPeriodReachedL();
	void StartChargingCurrentSameIntervalL();
	void StartChargingCurrentDifferentIntervalL();
	void StopChargingCurrentAllL();
	void SetChargingCurrentMeasurementTime200L();
	void StartRemainingChargingTimeNoCallbackL();
	void StartChargingCurrentNoCallbackL();
	void TestBatteryChargerTypeMultiClientL(TInt aType);
    void TestPowerClientChargingCurrentSetCallBackL();
    void TestPowerClientRemainingChargingTimeSetCallBackL();
    void TestSetCallBackLeavingL();
    void ChargingCurrentMaxPeriodReachedMCL();
#endif //SYMBIAN_HWRM_EXTPOWERINFO

	void RegisterPowerCallbackL();
    void RegisterNullPowerCallbackL();
	void PowerNotifyL();
	void PowerNotify_CancelL();
	void PowerNotify_Cancel_RedundantL();
	void SetReportingPeriod30L();
	void SetReportingPeriod5L();
	void SetReportingPeriod10L();
	void SetReportingPeriodZeroL();
	void SetReportingPeriod_Power_NegativeL();
	void SetReportingPeriod10L_TimeoutL();
	void PowerNotify_Continue_NoTimeoutL();
	void PowerNotify_Continue_TimeoutL();
	void ResetReportingPeriodL();
	void PowerNotify_Interval20L();
	void PowerNotify_Interval50L();
	void PowerNotify_Interval60L();
    void PowerNotify_Interval1L();
	void RegisterPowerCallbackMC3L();
	void PowerNotifyMC3_SameIntervalL();
	void PowerNotify_CancelMC3L();
	void PowerNotifyMC3_DiffIntervalL();
	void PowerNotify_RedundantL();
	void PowerNotify_Start_PluginErrorL();
    void PowerNotify_Start_Set_PluginErrorL();
    void PowerNotify_ClosePowerInstanceL();
	void PowerNotify_Cancel_PluginErrorL();
	void SetPowerMeasurementTime200L();
	void PowerNotify_NoCallbackL();
	void RegisterPowerCallbackMC2L();
	void PowerNotifyMC2_DiffInterval_SpecialCaseL();
	void PowerNotify_CancelMC2L();

	// FmTx Test States
	void FmTxOpenL();
	void FmTxOpenWhileInactiveL();
	void FmTxOpenWhileActiveL();
	void FmTxOpenWhileOnL();
	void FmTxOpenWhilePowerSaveAccessoryL();
	void FmTxEnableL();
	void FmTxStatusEnabledL();
	void FmTxStatusOffL();
	void FmTxDisableL();
	void FmTxReserveL();
	void FmTx2ReserveL();
	void FmTxOpen2SessionsL();
	void FmTx2EnableL();
	void FmTxEnableWhileActiveOrInactiveL();
	void FmTxEnableInvalidFreqL();
	void FmTxSetRdsDataL();
	void FmTxSetRadiotextL();

	void FmTxSetRadiotextContent1L();

	void FmTxSetRadiotextContent2L();
	void FmTxSetRadiotextContent2WhileDisabledL();
	void FmTxSetRadiotextContent3L();	
	void FmTxSetRadiotextTag1L();
	void FmTxSetRadiotextTag2L();
	void FmTxSetRadiotextTag3L();
	void FmTxSetRadiotextTags1L();
	void FmTxSetRadiotextTags2L();
	void FmTxGetFrequencyL();
	void FmTxSetFrequencyL();
	void FmTxSetFrequencyRoundUpL();
	void FmTxSetFrequencyRoundDownL();
	void FmTxCheckSetFrequencyL();
	void FmTxGetFrequencyRangeL();
	void FmTxCheckGetFrequencyRangeL();
	void FmTxGetFrequencyStepSizeL();
	void FmTxSetFrequencyRangeEuropeL();
	void FmTxSetFrequencyRangeJapanL();
	void FmTxCheckGetJapanFrequencyRangeL();
	void FmTxGetNextClearFrequencyL();
	void FmTxGetNextClearFrequencyWhileDisabledL();
	void FmTxGetNextClearFrequencyArrayL();
	void FmTxGetNextClearFrequencyArrayWhileDisabledL();
	void FmTxGetNextClearFrequencyArrayErrArgumentMinL();
	void FmTxGetNextClearFrequencyArrayErrArgumentMaxL();
	void FmTxGetNextClearFrequencyAsyncL();
	void FmTxGetNextClearFrequencyAsyncCancelL();
	void FmTxGetNextClearFrequencyAsyncArrayL();
	void FmTxGetNextClearFrequencyAsyncArrayErrArgumentMinL();
	void FmTxGetNextClearFrequencyAsyncArrayErrArgumentMaxL();
	void FmTxSetNextClearFrequencyL();
	void FmTxSetNextClearFrequencyAsyncL();
	void FmTxSetNextClearFrequencyAsyncCancelL();
	void FmTxEnableNewFreqL();
	void FmTxEnableValidFreqL();
	void FmTx2StatusEnabledL();
	void FmTxDisableFromOnL();
	void FmTxDisableFromOnQueuePluginMsgL();
	void FmTxGetCurrentStatusL();
	void FmTxCompareCurrentAndTempStatusL();
	void FmTxReleaseL();
	void FmTx2EnableExpectKErrInUseAndNotificationL();
	void FmTx2EnableNoNotificationL();
	void FmTxDisableDuringReservation();
	void FmTx2EnableDuringReservationL();
	void FmTx2StatusOffL();
	void FmTx2DisableFromOnL();
	void FmTxSetFrequencyInvalidL();
	void FmTx2SetFrequencyDuringReservationL();
	void FmTxCheckPowerSaveStateL();
	void FmTxIsAutoFrequencyEnabled();
	void FmTxEnableAutoFrequencyL();
	void FmTxDisableAutoFrequencyL();
	void FmTxSetRdsPtyL();
	void FmTxSetRdsPtyKErrArgumentMinL();
	void FmTxSetRdsPtyKErrArgumentMaxL();
	void FmTxSetRdsPtynL();
	void FmTxSetRdsMsFalseL();
	void FmTxSetRdsMsTrueL();
	void FmTxSetRdsLangIdL();
	void FmTxSetRdsLangIdKErrArgumentMinL();
	void FmTxSetRdsLangIdKErrArgumentMaxL();
	void FmTxSetRdsPtyWhileDisabledL();
	void FmTxSetRdsPtynWhileDisabledL();
	void FmTxSetRdsMsWhileDisabledL();
	void FmTxClearRdsDataL();
	void FmTxSetRdsLangIdWhileDisabledL();
	void FmTxSetRadiotextTag2WhileDisabledL();
	void FmTxSetRadiotextTags2WhileDisabledL();
	void FmTxGetNextClearFrequencyAsyncBeforeCancelL();
	void FmTxSetInactivityTimeOutTestModeL();
#ifdef INSECURE_AUDIO_POLICY_KEYS    
	void FmTxWatcherPluginConnectL();
	void FmTxWatcherPluginDisconnectL();
	void FmTxEnableDuringPowersaveL();
	void FmTxEnableDuringPowersave2L();
	void FmTxAudioRoutingOnL();
	void FmTxCheckExpectedAudioRoutingStateL();
	void FmTxAudioRoutingOffL();
	void FmTxPowerSaveOnAccessoryL();
	void FmTxExpectPowerSaveOnInactivityL();
	void FmTxExpectPowerSaveOffL();
	void FmTxPowerSaveOffL();	
	void FmTxStatusPowersaveL();
	void SetWatcherPluginConnectedStatusL(TBool aStatus );
	void SetAudioRoutedFlagL(TBool aFlag);
#endif // INSECURE_AUDIO_POLICY_KEYS  		
	
	// Test case utilities
	void IncrementFrequency(TInt &aFrequency);
private:
	// variables used for limited duration tests
	TInt iLimitedDuration;	
	CHWRMLight::TLightTarget iTimedTarget;
	RArray<TInt> iTimedTargets;

	TInt iTempFrequency;
	TFmTxFrequencyRange iTempFrequencyRange;
	TFmTxState iTempFmTxState;
	TFmTxState iTempFmTxStateAlter;
	TInt iTempNextClearFreq;
    CHWRMPower* iPower1;
    CHWRMPower* iPower2;
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
	EPSHWRMChargingStatus iChargingStatus;
	TInt iChargingStatusCBregistered;
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	};

#endif // hwrmtest_h
