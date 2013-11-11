/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
@file 
The TEFUnit test suite for CustomIPC in the Common TSY.
*/

#include "cctsycustomipcfu.h"
#include <ctsy/mmtsy_names.h>
#include <etel.h>
#include <etelmm.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "CMmCustomSecurityTsy.h"

// constants used by ReadViagHomeZoneCache/WriteViagHomeZoneCache tests
const TInt KViagHomeZoneCacheIdMin = 1;
const TInt KViagHomeZoneCacheIdMax = KPhEngMaxViagHomeZones;
const TInt KViagHomeZoneCacheRecordIdMin = 0;
const TInt KViagHomeZoneCacheRecordIdMax = KPhEngMaxCacheId - 1;
const TInt KOneSecond=1000000;  // Used in a time out function, 1 second (in microSeconds)

CTestSuite* CCTsyCustomIPCFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetDriveMode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetDriveMode0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetDriveMode0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetDriveMode0005L);		
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestActivateSimLock0001L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestActivateSimLock0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestActivateSimLock0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestActivateSimLock0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestActivateSimLock0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDeActivateSimLock0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDeActivateSimLock0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDeActivateSimLock0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDeActivateSimLock0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDeActivateSimLock0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyDtmfEvent0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyDtmfEvent0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyDtmfEvent0004L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetDiagnosticInfo0001L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetAlsBlocked0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetAlsBlocked0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetAlsBlocked0004L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetAlsBlocked0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetAlsBlocked0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetAlsBlocked0002L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetAlsBlocked0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetAlsBlocked0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckAlsPpSupport0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckAlsPpSupport0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckAlsPpSupport0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckAlsPpSupport0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetRemoteAlertingToneStatus0001L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCallOrigin0001L);
	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestTerminateCall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestTerminateCall0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestTerminateCall0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestTerminateCall0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestTerminateCall0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyAlsBlockedChanged0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyAlsBlockedChanged0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyAlsBlockedChanged0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCipheringInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCipheringInfo0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCipheringInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCipheringInfo0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyCipheringInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyCipheringInfoChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyCipheringInfoChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyNSPSStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyNSPSStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyNSPSStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNetWakeup0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNetWakeup0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNetWakeup0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNetWakeup0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneParams0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneParams0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneParams0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneParams0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneParams0005L);
	
    ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneCache0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneCache0001bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneCache0001cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneCache0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneCache0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneCache0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadViagHomeZoneCache0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteViagHomeZoneCache0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteViagHomeZoneCache0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteViagHomeZoneCache0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteViagHomeZoneCache0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteViagHomeZoneCache0005L);
	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestClearCallBlackList0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestClearCallBlackList0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestClearCallBlackList0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestClearCallBlackList0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSsAdditionalInfoNotification0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSsAdditionalInfoNotification0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSsAdditionalInfoNotification0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSsRequestCompleteNotification0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSsRequestCompleteNotification0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSsRequestCompleteNotification0004L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestIsBlocked0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestIsBlocked0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckSecurityCode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckSecurityCode0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckSecurityCode0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckSecurityCode0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetActivePin0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetAirTimeDuration0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetNetworkProviderName0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetNetworkProviderName0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetNetworkProviderName0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetNetworkProviderName0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetNetworkProviderName0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckEmergencyNumber0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckEmergencyNumber0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckEmergencyNumber0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckEmergencyNumber0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyPndCacheReady0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyPndCacheReady0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyPndCacheReady0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyPndCacheReady0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyPndCacheReady0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetPndCacheStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetPndCacheStatus0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetOperatorName0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetOperatorName0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetOperatorName0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetOperatorName0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetProgrammableOperatorLogo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetProgrammableOperatorLogo0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetProgrammableOperatorLogo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetProgrammableOperatorLogo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetProgrammableOperatorLogo0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyProgrammableOperatorLogoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifySsNetworkEvent0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifySsNetworkEvent0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifySsNetworkEvent0004L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCancelUssdSession0001L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSatRefreshCompleteNotification0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSatRefreshCompleteNotification0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSatRefreshCompleteNotification0004L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckTwoDigitDialSupport0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckTwoDigitDialSupport0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckTwoDigitDialSupport0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestCheckTwoDigitDialSupport0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestResetNetServer0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestResetNetServer0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestResetNetServer0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestResetNetServer0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReleaseFile0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestRestartFile0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestStartSimCbTopicBrowsing0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestStartSimCbTopicBrowsing0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestStartSimCbTopicBrowsing0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetNextSimCbTopic0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDeleteSimCbTopic0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDeleteSimCbTopic0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDeleteSimCbTopic0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDeleteSimCbTopic0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDeleteSimCbTopic0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyNetworkConnectionFailure0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyNetworkConnectionFailure0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyNetworkConnectionFailure0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSendAPDUReq0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSendAPDUReq0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSendAPDUReq0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSendAPDUReq0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSendAPDUReq0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDisablePhoneLock0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDisablePhoneLock0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDisablePhoneLock0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestDisablePhoneLock0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyEGprsInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyEGprsInfoChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyEGprsInfoChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyEGprsInfoChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetEGprsInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetEGprsInfo0003L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadSimFile0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadSimFile0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadSimFile0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadSimFile0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadSimFile0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetLifeTime0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetLifeTime0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetLifeTime0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetLifeTime0004L);

	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGet3GPBInfo0001L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSystemNetworkModes0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSystemNetworkModes0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSystemNetworkModes0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSystemNetworkModes0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetSystemNetworkMode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetSystemNetworkMode0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetSystemNetworkMode0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetSystemNetworkMode0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCurrentSystemNetworkModes0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCurrentSystemNetworkModes0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCurrentSystemNetworkModes0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCurrentSystemNetworkModes0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestPowerSimOn0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestPowerSimOn0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestPowerSimOn0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestPowerSimOn0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestPowerSimOff0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestPowerSimOff0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestPowerSimOff0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestPowerSimOff0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSimWarmReset0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSimWarmReset0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSimWarmReset0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSimWarmReset0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetATR0001L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetATR0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetATR0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetATR0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetATR0005L);
	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSimCardReaderStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSimCardReaderStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSimCardReaderStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSimCardReaderStatus0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifySimCardStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifySimCardStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifySimCardStatus0004L);	
	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetWlanSimAuthenticationData0001L); //!!!
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetWlanSimAuthenticationData0002L); 
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetWlanSimAuthenticationData0003L); 
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetWlanSimAuthenticationData0004L); 
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetWlanSimAuthenticationData0005L); 
	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetSimMessageStatusRead0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteViagHomeZoneUHZIUESettings0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteViagHomeZoneUHZIUESettings0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteViagHomeZoneUHZIUESettings0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteViagHomeZoneUHZIUESettings0005L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetAlwaysOn0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetAlwaysOn0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetAlwaysOn0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetAlwaysOn0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyRauEvent0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyRauEvent0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyRauEvent0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadHSxPAStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadHSxPAStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadHSxPAStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestReadHSxPAStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteHSxPAStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteHSxPAStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteHSxPAStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestWriteHSxPAStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyHSxPAStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyHSxPAStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyHSxPAStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetIccCallForwardingIndicatorStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetIccCallForwardingIndicatorStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetIccCallForwardingIndicatorStatus0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetIccCallForwardingIndicatorStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetIccCallForwardingIndicatorStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyIccCallForwardingStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyIccCallForwardingStatusChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyIccCallForwardingStatusChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyIccCallForwardingStatusChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCellInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCellInfo0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCellInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCellInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetCellInfo0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyCellInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyCellInfoChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyCellInfoChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestNotifyCellInfoChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSystemNetworkBand0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSystemNetworkBand0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSystemNetworkBand0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetSystemNetworkBand0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetSystemNetworkBand0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetSystemNetworkBand0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetSystemNetworkBand0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestSetSystemNetworkBand0005L);

	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetUSIMServiceSupport0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetUSIMServiceSupport0002L);		
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetUSIMServiceSupport0003L);		
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetUSIMServiceSupport0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCustomIPCFU, TestGetUSIMServiceSupport0005L);
	
	END_SUITE;
	}


//////////////////////////////////////////////////////////////////
// Actual test cases
/////////////////////////////////////////////////////////////////


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSDM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SetDriveMode
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetDriveMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetDriveMode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	   
		
	TRequestStatus requestStatus;	
	RMmCustomAPI::TSetDriveMode modeStatus(RMmCustomAPI::EActivateDriveMode);    
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	// data for ExpectL
	TMockLtsyData1< RMmCustomAPI::TSetDriveMode > ltsyData(modeStatus);	
    ltsyData.SerialiseL(expectData);	    
    iMockLTSY.ExpectL(ECustomSetDriveModeIPC, expectData, KErrNotSupported);        	
    
	customAPI.SetDriveMode(requestStatus, modeStatus);	
	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	    
    iMockLTSY.ExpectL(ECustomSetDriveModeIPC, expectData);   
    iMockLTSY.CompleteL(ECustomSetDriveModeIPC, KErrGeneral);	
	
	customAPI.SetDriveMode(requestStatus, modeStatus);	
	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::SetDriveMode when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomSetDriveModeIPC, expectData);   
    iMockLTSY.CompleteL(ECustomSetDriveModeIPC, KErrNone);	
	
	customAPI.SetDriveMode(requestStatus, modeStatus);	
	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::SetDriveMode
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomSetDriveModeIPC, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSDM-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::SetDriveMode
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::SetDriveMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetDriveMode0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
		
	TRequestStatus requestStatus;	
	RMmCustomAPI::TSetDriveMode modeStatus(RMmCustomAPI::EActivateDriveMode); 

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::SetDriveMode
 	//------------------------------------------------------------------------- 	

	// data for ExpectL
	TMockLtsyData1< RMmCustomAPI::TSetDriveMode > ltsyData(modeStatus);	
    ltsyData.SerialiseL(expectData);	    
    iMockLTSY.ExpectL(ECustomSetDriveModeIPC, expectData); 
    
    customAPI.SetDriveMode(requestStatus, modeStatus);
	
	iMockLTSY.CompleteL(ECustomSetDriveModeIPC, KErrNone, 20);
	
	customAPI.CancelAsyncRequest(ECustomSetDriveModeIPC);
		
	User::WaitForRequest(requestStatus);		
	// TSY has started a request and it is not possible to then
    // cancel it. The best thing for the TSY to do in
    // this case is to proceed as though the Cancel never happened.
    // The server's call to the TSY cancel function will return
    // synchronously. The TSY then continues to wait for the methods
    // acknowledgement and when it receives it, the TSY will complete
    // the original request.
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();
	
		
	CleanupStack::PopAndDestroy(3); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSDM-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::SetDriveMode
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::SetDriveMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetDriveMode0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);
			
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 expectData2;
	CleanupClosePushL(expectData2);	
	
	TRequestStatus requestStatus;	
	RMmCustomAPI::TSetDriveMode modeStatus(RMmCustomAPI::EActivateDriveMode); 
	
	TRequestStatus requestStatus2;	
	RMmCustomAPI::TSetDriveMode modeStatus2(RMmCustomAPI::EDeactivateDriveMode); 
	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::SetDriveMode
 	//-------------------------------------------------------------------------
 	
 	// data for the first ExpectL
	TMockLtsyData1< RMmCustomAPI::TSetDriveMode > ltsyData(modeStatus);	
    ltsyData.SerialiseL(expectData);	    
       
    // data for the second ExpectL
	TMockLtsyData1< RMmCustomAPI::TSetDriveMode > ltsyData2(modeStatus2);	
    ltsyData2.SerialiseL(expectData2);	    
        
    // first SetDriveMode
    iMockLTSY.ExpectL(ECustomSetDriveModeIPC, expectData);
    iMockLTSY.CompleteL(ECustomSetDriveModeIPC, KErrNone, 20);
    
    customAPI.SetDriveMode(requestStatus, modeStatus);
                
    
    
    customAPI2.SetDriveMode(requestStatus2, modeStatus2); 	
 	
 	
 	// wait for the first answer
 	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());		
	
	// wait for the second answer
 	User::WaitForRequest(requestStatus2);
 	// Documentation says that KErrServerBusy should be in case of multiple requests
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(8, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSDM-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SetDriveMode with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetDriveMode and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetDriveMode0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	
	TRequestStatus requestStatus;	
	RMmCustomAPI::TSetDriveMode modeStatus(RMmCustomAPI::EActivateDriveMode); 

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::SetDriveMode
 	//-------------------------------------------------------------------------

	// data for ExpectL
	TMockLtsyData1< RMmCustomAPI::TSetDriveMode > ltsyData(modeStatus);	
    ltsyData.SerialiseL(expectData);	    
    iMockLTSY.ExpectL(ECustomSetDriveModeIPC, expectData); 
    
    customAPI.SetDriveMode(requestStatus, modeStatus);
		
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCASL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ActivateSimLock
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ActivateSimLock
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestActivateSimLock0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	
	TRequestStatus requestStatus;	
	 
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	RMmCustomAPI::TSimLockPassword password(_L("changeme"));
	RMmCustomAPI::TLockNumber lockNumber(RMmCustomAPI::EOperator);	
	
	// data for ExpectL
	TMockLtsyData2< RMmCustomAPI::TSimLockPassword, RMmCustomAPI::TLockNumber > 
									ltsyData(password, lockNumber);	    
    ltsyData.SerialiseL(expectData);    	    
    iMockLTSY.ExpectL(ECustomSimLockActivateIPC, expectData, KErrNotSupported);
    
	customAPI.ActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	

	// use othere number for coverage increasing
	lockNumber = RMmCustomAPI::EGid1;
	
	// data for ExpectL
	expectData.Close();
    ltsyData.SerialiseL(expectData);    	    
    iMockLTSY.ExpectL(ECustomSimLockActivateIPC, expectData);
    iMockLTSY.CompleteL(ECustomSimLockActivateIPC, KErrGeneral);
	
	customAPI.ActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::ActivateSimLock when result is not cached.
 	//-------------------------------------------------------------------------
	// use othere number for coverage increasing
	lockNumber = RMmCustomAPI::EGid2;
	
	// data for ExpectL 
	expectData.Close();
    ltsyData.SerialiseL(expectData);    	    
       
    iMockLTSY.ExpectL(ECustomSimLockActivateIPC, expectData);
    iMockLTSY.CompleteL(ECustomSimLockActivateIPC, KErrNone);
	
	customAPI.ActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::ActivateSimLock
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomSimLockActivateIPC, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();
	
	
	//-------------------------------------------------------------------------
	// just to increase coverage
	//-------------------------------------------------------------------------	
	// use othere number for coverage increasing
	lockNumber = RMmCustomAPI::EImsi;
	
	// data for ExpectL	    
	expectData.Close();
    ltsyData.SerialiseL(expectData);    	    
       
    iMockLTSY.ExpectL(ECustomSimLockActivateIPC, expectData);
    iMockLTSY.CompleteL(ECustomSimLockActivateIPC, KErrNone);
	
	customAPI.ActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCASL-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::ActivateSimLock
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::ActivateSimLock
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestActivateSimLock0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	TRequestStatus mockLtsyStatus;
	// to wait CompleteL
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	

	TRequestStatus requestStatus;
 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::ActivateSimLock
 	//-------------------------------------------------------------------------
 	RMmCustomAPI::TSimLockPassword password(_L("changemechangemechangemechange"));
	RMmCustomAPI::TLockNumber lockNumber(RMmCustomAPI::EImsi);	
	
	// data for ExpectL
	TMockLtsyData2< RMmCustomAPI::TSimLockPassword, RMmCustomAPI::TLockNumber > 
									ltsyData(password, lockNumber);	    
    ltsyData.SerialiseL(expectData);    	    
    iMockLTSY.ExpectL(ECustomSimLockActivateIPC, expectData);
	
	customAPI.ActivateSimLock(requestStatus, password, lockNumber);
	// cancel this request
	customAPI.CancelAsyncRequest(ECustomSimLockActivateIPC);
	// try to complete canceled request
	iMockLTSY.CompleteL(ECustomSimLockActivateIPC, KErrNone);		
			
	// wait for request
	User::WaitForRequest(requestStatus);
	// this request can't be canceled 
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	// wait for CompleteL
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCASL-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ActivateSimLock with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ActivateSimLock with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestActivateSimLock0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	TRequestStatus requestStatus;	

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong parameters to
	// RMmCustomAPI::ActivateSimLock
 	//-------------------------------------------------------------------------
	RMmCustomAPI::TSimLockPassword password(_L("changemechangemechangemechange"));
	RMmCustomAPI::TLockNumber lockNumber((RMmCustomAPI::TLockNumber) 0); 	
	  
    customAPI.ActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCASL-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::ActivateSimLock
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::ActivateSimLock
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestActivateSimLock0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);
	
	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::ActivateSimLock
 	//-------------------------------------------------------------------------	
	// data for the first request
	RMmCustomAPI::TSimLockPassword password(_L("changemechangemechangemechange"));
	RMmCustomAPI::TLockNumber lockNumber(RMmCustomAPI::EOperator_Gid1);	
	
	// data for the second request
	RMmCustomAPI::TSimLockPassword password2(_L("changeme2"));
	RMmCustomAPI::TLockNumber lockNumber2(RMmCustomAPI::EOperator_Gid2);	
	
	// data for the first ExpectL
	TMockLtsyData2< RMmCustomAPI::TSimLockPassword, RMmCustomAPI::TLockNumber > ltsyData(password, lockNumber);	    
    ltsyData.SerialiseL(expectData);       
  		
	// send first request
	iMockLTSY.ExpectL(ECustomSimLockActivateIPC, expectData);
    iMockLTSY.CompleteL(ECustomSimLockActivateIPC, KErrNone);	
	customAPI.ActivateSimLock(requestStatus, password, lockNumber);
		
	// send second request
	customAPI2.ActivateSimLock(requestStatus2, password2, lockNumber2);
	
	// wait for the first answer
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	// wait for the second answer
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());		
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(6, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCASL-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ActivateSimLock with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ActivateSimLock and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestActivateSimLock0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::ActivateSimLock
 	//-------------------------------------------------------------------------
 	RMmCustomAPI::TSimLockPassword password(_L("changemechangemechangemechange"));
	RMmCustomAPI::TLockNumber lockNumber(RMmCustomAPI::EOperator_Gid2);	
	
	// data for ExpectL
	TMockLtsyData2< RMmCustomAPI::TSimLockPassword, RMmCustomAPI::TLockNumber > 
									ltsyData(password, lockNumber);	    
    ltsyData.SerialiseL(expectData);    	    
    iMockLTSY.ExpectL(ECustomSimLockActivateIPC, expectData);
    
	customAPI.ActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy(3, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDASL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::DeActivateSimLock
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DeActivateSimLock
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDeActivateSimLock0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	
	TRequestStatus requestStatus;	
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	RMmCustomAPI::TSimLockPassword password(_L("changemechangemechangemechange"));
	RMmCustomAPI::TLockNumber lockNumber(RMmCustomAPI::EOperator);	
	
	// data for ExpectL
	TMockLtsyData2< RMmCustomAPI::TSimLockPassword, RMmCustomAPI::TLockNumber > 
									ltsyData(password, lockNumber);	    
    ltsyData.SerialiseL(expectData);    	    
    iMockLTSY.ExpectL(ECustomSimLockDeActivateIPC, expectData, KErrNotSupported);
    
	customAPI.DeActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	
	// use othere number for coverage increasing
	lockNumber = RMmCustomAPI::EGid1;
	
	// data for ExpectL  
	expectData.Close();
    ltsyData.SerialiseL(expectData);    	    
    iMockLTSY.ExpectL(ECustomSimLockDeActivateIPC, expectData);
    iMockLTSY.CompleteL(ECustomSimLockDeActivateIPC, KErrGeneral);
	
	customAPI.DeActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::DeActivateSimLock when result is not cached.
 	//-------------------------------------------------------------------------
	// use othere number for coverage increasing
	lockNumber = RMmCustomAPI::EGid2;
	
	// data for ExpectL
	expectData.Close();
    ltsyData.SerialiseL(expectData);    	    
       
    iMockLTSY.ExpectL(ECustomSimLockDeActivateIPC, expectData);
    iMockLTSY.CompleteL(ECustomSimLockDeActivateIPC, KErrNone);
	
	customAPI.DeActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::DeActivateSimLock
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomSimLockDeActivateIPC, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// just to increase coverage
	//-------------------------------------------------------------------------	
	// use othere number for coverage increasing
	lockNumber = RMmCustomAPI::EGlobalUnlock;
	
	// data for ExpectL	    
	expectData.Close();
    ltsyData.SerialiseL(expectData);    	    
       
    iMockLTSY.ExpectL(ECustomSimLockDeActivateIPC, expectData);
    iMockLTSY.CompleteL(ECustomSimLockDeActivateIPC, KErrNone);
	
	customAPI.DeActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// just to increase coverage
	//-------------------------------------------------------------------------	
	// use othere number for coverage increasing
	lockNumber = RMmCustomAPI::EImsi;
	
	// data for ExpectL	    
	expectData.Close();
    ltsyData.SerialiseL(expectData);    	    
       
    iMockLTSY.ExpectL(ECustomSimLockDeActivateIPC, expectData);
    iMockLTSY.CompleteL(ECustomSimLockDeActivateIPC, KErrNone);
	
	customAPI.DeActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDASL-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::DeActivateSimLock
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::DeActivateSimLock
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDeActivateSimLock0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	
	TRequestStatus mockLtsyStatus;
	// to wait CompleteL
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	

	TRequestStatus requestStatus;
 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::DeActivateSimLock
 	//-------------------------------------------------------------------------
 	RMmCustomAPI::TSimLockPassword password(_L("changemechangemechangemechange"));
	RMmCustomAPI::TLockNumber lockNumber(RMmCustomAPI::EImsi);	
	
	// data for ExpectL
	TMockLtsyData2< RMmCustomAPI::TSimLockPassword, RMmCustomAPI::TLockNumber > 
									ltsyData(password, lockNumber);	    
    ltsyData.SerialiseL(expectData);    	    
    iMockLTSY.ExpectL(ECustomSimLockDeActivateIPC, expectData);
	
	customAPI.DeActivateSimLock(requestStatus, password, lockNumber);
	// cancel this request
	customAPI.CancelAsyncRequest(ECustomSimLockDeActivateIPC);
	// try to complete canceled request
	iMockLTSY.CompleteL(ECustomSimLockDeActivateIPC, KErrNone);		
			
	// wait for request
	User::WaitForRequest(requestStatus);
	// this request can't be canceled 
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	// wait for CompleteL
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDASL-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::DeActivateSimLock with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DeActivateSimLock with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDeActivateSimLock0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	TRequestStatus requestStatus;	

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMmCustomAPI::DeActivateSimLock
 	//-------------------------------------------------------------------------
	RMmCustomAPI::TSimLockPassword password(_L("changemechangemechangemechange"));
	RMmCustomAPI::TLockNumber lockNumber((RMmCustomAPI::TLockNumber) 0); 	
	  
    customAPI.DeActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDASL-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::DeActivateSimLock
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::DeActivateSimLock
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDeActivateSimLock0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);
	
	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::DeActivateSimLock
 	//-------------------------------------------------------------------------	
	// data for the first request
	RMmCustomAPI::TSimLockPassword password(_L("changemechangemechangemechange"));
	RMmCustomAPI::TLockNumber lockNumber(RMmCustomAPI::EOperator_Gid1);	
	
	// data for the second request
	RMmCustomAPI::TSimLockPassword password2(_L("changemechangemechangemechang2"));
	RMmCustomAPI::TLockNumber lockNumber2(RMmCustomAPI::EOperator_Gid2);	
	
	// data for the first ExpectL
	TMockLtsyData2< RMmCustomAPI::TSimLockPassword, RMmCustomAPI::TLockNumber > ltsyData(password, lockNumber);	    
    ltsyData.SerialiseL(expectData);       
  		
	// send first request
	iMockLTSY.ExpectL(ECustomSimLockDeActivateIPC, expectData);
    iMockLTSY.CompleteL(ECustomSimLockDeActivateIPC, KErrNone);	
	customAPI.DeActivateSimLock(requestStatus, password, lockNumber);
		
	// send second request
	customAPI2.DeActivateSimLock(requestStatus2, password2, lockNumber2);
	
	// wait for the first answer
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	// wait for the second answer
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());		
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(6, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDASL-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::DeActivateSimLock with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DeActivateSimLock and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDeActivateSimLock0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::DeActivateSimLock
 	//-------------------------------------------------------------------------
 	RMmCustomAPI::TSimLockPassword password(_L("changemechangemechangemechange"));
	RMmCustomAPI::TLockNumber lockNumber(RMmCustomAPI::EOperator_Gid2);	
	
	// data for ExpectL
	TMockLtsyData2< RMmCustomAPI::TSimLockPassword, RMmCustomAPI::TLockNumber > 
									ltsyData(password, lockNumber);	    
    ltsyData.SerialiseL(expectData);    	    
    iMockLTSY.ExpectL(ECustomSimLockDeActivateIPC, expectData);
    
	customAPI.DeActivateSimLock(requestStatus, password, lockNumber);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy(3, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNDE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyDtmfEvent
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyDtmfEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyDtmfEvent0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	// data for CompleteL
	RMmCustomAPI::TDtmfInfo completeInfo;
	completeInfo.iType  = RMmCustomAPI::EDtmfSequence;	
	completeInfo.iEvent = RMmCustomAPI::EDtmfStop;
	completeInfo.iTone = 0x10;
	TMockLtsyData1 <RMmCustomAPI::TDtmfInfo> ltsyData(completeInfo);
	ltsyData.SerialiseL(completeData); 	
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyDtmfEvent
	// from LTSY.
	// Test here cause this is a self-reposting request.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifyDtmfEventIPC, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	// data for NotifyDtmfEvent itself
	TRequestStatus requestStatus;
	RMmCustomAPI::TDtmfInfo info;
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	customAPI.NotifyDtmfEvent(requestStatus, info);	
	iMockLTSY.CompleteL(ECustomNotifyDtmfEventIPC, KErrGeneral, completeData);
		
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyDtmfEvent when result is not cached.
 	//-------------------------------------------------------------------------
	customAPI.NotifyDtmfEvent(requestStatus, info);	
	iMockLTSY.CompleteL(ECustomNotifyDtmfEventIPC, KErrNone, completeData);
		
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(completeInfo.iType,   info.iType);
	ASSERT_EQUALS(completeInfo.iEvent,	info.iEvent);
	ASSERT_EQUALS(completeInfo.iTone,	info.iTone); 
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNDE-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifyDtmfEvent
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifyDtmfEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyDtmfEvent0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	// data for CompleteL
	RMmCustomAPI::TDtmfInfo completeInfo;
	completeInfo.iType  = RMmCustomAPI::EDtmfSequence;	
	completeInfo.iEvent = RMmCustomAPI::EDtmfStop;
	completeInfo.iTone = 0x10;
	TMockLtsyData1 <RMmCustomAPI::TDtmfInfo> ltsyData(completeInfo);
	ltsyData.SerialiseL(completeData); 		
	
	// data for NotifyDtmfEvent itself
	TRequestStatus requestStatus;
	RMmCustomAPI::TDtmfInfo info;
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifyDtmfEvent
 	//-------------------------------------------------------------------------
 	
	customAPI.NotifyDtmfEvent(requestStatus, info);
	// cancel request
	customAPI.CancelAsyncRequest(ECustomNotifyDtmfEventIPC);		
	// try to complete request
	iMockLTSY.CompleteL(ECustomNotifyDtmfEventIPC, KErrNone, completeData);
		
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
	
	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNDE-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifyDtmfEvent
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifyDtmfEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyDtmfEvent0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
			
	RBuf8 completeData;
	CleanupClosePushL(completeData);	
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
		
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RMmCustomAPI customAPI2;
	CleanupClosePushL(customAPI2);
	customAPI2.Open(phone2);
		
	// data for CompleteL
	RMmCustomAPI::TDtmfInfo completeInfo;
	completeInfo.iType  = RMmCustomAPI::EDtmfSequence;	
	completeInfo.iEvent = RMmCustomAPI::EDtmfStop;
	completeInfo.iTone = 0x10;
	TMockLtsyData1 <RMmCustomAPI::TDtmfInfo> ltsyData(completeInfo);
	ltsyData.SerialiseL(completeData); 		
	
	// data for the first NotifyDtmfEvent 
	TRequestStatus requestStatus;
	RMmCustomAPI::TDtmfInfo info;

	
	// data for the second NotifyDtmfEvent 
	TRequestStatus requestStatus2;
	RMmCustomAPI::TDtmfInfo info2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifyDtmfEvent
 	//-------------------------------------------------------------------------

	// send first request
	customAPI.NotifyDtmfEvent(requestStatus, info);	
	
	// send second request
	customAPI2.NotifyDtmfEvent(requestStatus2, info2);	
	
	// Complete
	iMockLTSY.CompleteL(ECustomNotifyDtmfEventIPC, KErrNone, completeData);
	
	// wait for the first answer	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	// wait for the second answer	
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	AssertMockLtsyStatusL();
		
	// check first request's data
	ASSERT_EQUALS(completeInfo.iType,   info.iType);
	ASSERT_EQUALS(completeInfo.iEvent,	info.iEvent);
	ASSERT_EQUALS(completeInfo.iTone,	info.iTone);
	
	// check second request's data
	ASSERT_EQUALS(completeInfo.iType,   info2.iType);
	ASSERT_EQUALS(completeInfo.iEvent,	info2.iEvent);
	ASSERT_EQUALS(completeInfo.iTone,	info2.iTone);

	// Done !
	CleanupStack::PopAndDestroy(6, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGDO-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetDiagnosticInfo
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetDiagnosticInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetDiagnosticInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();	
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	
	//-------------------------------------------------------------------------
	// try to get info for non-existing call
 	//-------------------------------------------------------------------------
	TName callName;
	TInt status = customAPI.GetDiagnosticInfo(callName);
	
	ASSERT_EQUALS(KErrNotFound, status);	
	AssertMockLtsyStatusL();	
	

	//-------------------------------------------------------------------------
	// try to get info for existing call,  with initial iDiagnosticOctet ( == NULL)
 	//-------------------------------------------------------------------------
	
	// -----------Open Line and Call ---------------------
	RLine line;
	RCall call;
   	// Open new line
    TInt errorCode = line.Open(iPhone, KMmTsyVoice1LineName);    
	ASSERT_EQUALS(KErrNone, errorCode);	
	CleanupClosePushL(line);
		
	TName incomingCallName;
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
	 	
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, KMmTsyVoice1LineName, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);		
	CleanupClosePushL(call);
		
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
    //-------------------------------------------------------------------------    
    
    status = customAPI.GetDiagnosticInfo(incomingCallName);	
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL();


 	//-------------------------------------------------------------------------
	//  try to get info for existing call, with iDiagnosticOctet = KDiagnosticInfoBarredWithCUG
 	//-------------------------------------------------------------------------
	
	// set iDiagnosticOctet ...
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	TDiagnosticInfoValues diagnostic(KDiagnosticInfoBarredWithCUG);	
	TMockLtsyData2 <TInt, TDiagnosticInfoValues> ltsyData(callId, diagnostic);
	ltsyData.SerialiseL(completeData);	
	
	iMockLTSY.CompleteL(ECustomGetDiagnosticOctetsIPC, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ... and get info
	status = customAPI.GetDiagnosticInfo(incomingCallName);	
	ASSERT_EQUALS(KErrDiagnosticInfoBarredWithCUG, status);
	AssertMockLtsyStatusL();	
	
	
	//-------------------------------------------------------------------------
	//  try to get info for existing call, with iDiagnosticOctet = KDiagnosticInfoBarredNoCUG
 	//-------------------------------------------------------------------------
	
	// set iDiagnosticOctet ...
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	diagnostic = KDiagnosticInfoBarredNoCUG;	
	completeData.Close();
	ltsyData.SerialiseL(completeData);	
	
	iMockLTSY.CompleteL(ECustomGetDiagnosticOctetsIPC, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ... and get info
	status = customAPI.GetDiagnosticInfo(incomingCallName);	
	ASSERT_EQUALS(KErrDiagnosticInfoBarredNoCUG, status);
	AssertMockLtsyStatusL();
	
		
	//-------------------------------------------------------------------------
	//  try to get info for existing call, with iDiagnosticOctet = KDiagnosticInfoBarredUnknownCUG
 	//-------------------------------------------------------------------------
	
	// set iDiagnosticOctet ...
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	diagnostic = KDiagnosticInfoBarredUnknownCUG;
	completeData.Close();
	ltsyData.SerialiseL(completeData);	
	
	iMockLTSY.CompleteL(ECustomGetDiagnosticOctetsIPC, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ... and get info
	status = customAPI.GetDiagnosticInfo(incomingCallName);	
	ASSERT_EQUALS(KErrDiagnosticInfoBarredUnknownCUG, status);
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	//  try to get info for existing call, with iDiagnosticOctet = KDiagnosticInfoBarredIncompatibleCUG
 	//-------------------------------------------------------------------------
	
	// set iDiagnosticOctet ...
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	diagnostic = KDiagnosticInfoBarredIncompatibleCUG;	
	completeData.Close();
	ltsyData.SerialiseL(completeData);	
	
	iMockLTSY.CompleteL(ECustomGetDiagnosticOctetsIPC, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ... and get info
	status = customAPI.GetDiagnosticInfo(incomingCallName);	
	ASSERT_EQUALS(KErrDiagnosticInfoBarredIncompatibleCUG, status);
	AssertMockLtsyStatusL();
	
		
	//-------------------------------------------------------------------------
	//  try to get info for existing call, with iDiagnosticOctet = KDiagnosticInfoBarredFailureCUG
 	//-------------------------------------------------------------------------
	
	// set iDiagnosticOctet ...
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	diagnostic = KDiagnosticInfoBarredFailureCUG;	
	completeData.Close();
	ltsyData.SerialiseL(completeData);	
	
	iMockLTSY.CompleteL(ECustomGetDiagnosticOctetsIPC, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ... and get info
	status = customAPI.GetDiagnosticInfo(incomingCallName);	
	ASSERT_EQUALS(KErrDiagnosticInfoBarredFailureCUG, status);
	AssertMockLtsyStatusL();	
	
	
	//-------------------------------------------------------------------------
	//  try to get info for existing call, with iDiagnosticOctet = KDiagnosticInfoBarredClirNotSubscribed
 	//-------------------------------------------------------------------------
	
	// set iDiagnosticOctet ...
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	diagnostic = KDiagnosticInfoBarredClirNotSubscribed;
	completeData.Close();
	ltsyData.SerialiseL(completeData);	
	
	iMockLTSY.CompleteL(ECustomGetDiagnosticOctetsIPC, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ... and get info
	status = customAPI.GetDiagnosticInfo(incomingCallName);	
	ASSERT_EQUALS(KErrDiagnosticInfoBarredClirNotSubscribed, status);
	AssertMockLtsyStatusL();
	
	
	//-------------------------------------------------------------------------
	//  try to get info for existing call, with iDiagnosticOctet = KDiagnosticInfoBarredCCBSPossible
 	//-------------------------------------------------------------------------
	
	// set iDiagnosticOctet ...
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	diagnostic = KDiagnosticInfoBarredCCBSPossible;		
	completeData.Close();
	ltsyData.SerialiseL(completeData);	
	
	iMockLTSY.CompleteL(ECustomGetDiagnosticOctetsIPC, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ... and get info
	status = customAPI.GetDiagnosticInfo(incomingCallName);	
	ASSERT_EQUALS(KErrDiagnosticInfoBarredCCBSPossible, status);
	AssertMockLtsyStatusL();
	
	
	//-------------------------------------------------------------------------
	//  try to get info for existing call, with iDiagnosticOctet = KDiagnosticInfoBarredCCBSNotPossible
 	//-------------------------------------------------------------------------
	
	// set iDiagnosticOctet ...
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	diagnostic = KDiagnosticInfoBarredCCBSNotPossible;		
	completeData.Close();
	ltsyData.SerialiseL(completeData);	
	
	iMockLTSY.CompleteL(ECustomGetDiagnosticOctetsIPC, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ... and get info
	status = customAPI.GetDiagnosticInfo(incomingCallName);	
	ASSERT_EQUALS(KErrDiagnosticInfoBarredCCBSNotPossible, status);
	AssertMockLtsyStatusL();
	
	
	//-------------------------------------------------------------------------
	//  try to get info for existing call, with iDiagnosticOctet = some arbitrary value
 	//-------------------------------------------------------------------------
	
	// set iDiagnosticOctet ...
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	TInt someDiagnostic = -11; //	some arbitrary value
	TMockLtsyData2 <TInt, TInt> ltsyData2(callId, someDiagnostic);
	completeData.Close();
	ltsyData2.SerialiseL(completeData);	
	
	iMockLTSY.CompleteL(ECustomGetDiagnosticOctetsIPC, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ... and get info
	status = customAPI.GetDiagnosticInfo(incomingCallName);	
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL();
	
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetDiagnosticInfo
	// from LTSY.
 	//-------------------------------------------------------------------------
	//just  set iDiagnosticOctet to non-existing callId
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	callId = 100;
	completeData.Close();
	ltsyData2.SerialiseL(completeData);	
	
	iMockLTSY.CompleteL(ECustomGetDiagnosticOctetsIPC, KErrNone, completeData);
	
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	
	CleanupStack::PopAndDestroy(5, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAB-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SetAlsBlocked
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetAlsBlocked
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetAlsBlocked0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	// data for SetAlsBlocked itself
	TRequestStatus reguestStatus;
	RMmCustomAPI::TSetAlsBlock blockStatus(RMmCustomAPI::EActivateBlock);
	
	// data for ExpectL
	TMockLtsyData1< RMmCustomAPI::TSetAlsBlock > ltsyData(blockStatus);	
    ltsyData.SerialiseL(expectData);	
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomSetAlsBlockedIPC, expectData, KErrNotSupported);	
	
	customAPI.SetAlsBlocked(reguestStatus, blockStatus);
	
	User::WaitForRequest(reguestStatus);	
	ASSERT_EQUALS(KErrNotSupported, reguestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomSetAlsBlockedIPC, expectData);	
	iMockLTSY.CompleteL(ECustomSetAlsBlockedIPC, KErrGeneral);
	
	customAPI.SetAlsBlocked(reguestStatus, blockStatus);
	
	User::WaitForRequest(reguestStatus);	
	ASSERT_EQUALS(KErrGeneral, reguestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::SetAlsBlocked when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomSetAlsBlockedIPC, expectData);	
	iMockLTSY.CompleteL(ECustomSetAlsBlockedIPC, KErrNone);
	
	customAPI.SetAlsBlocked(reguestStatus, blockStatus);
	
	User::WaitForRequest(reguestStatus);	
	ASSERT_EQUALS(KErrNone, reguestStatus.Int());
	AssertMockLtsyStatusL();

 
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::SetAlsBlocked
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomSetAlsBlockedIPC, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAB-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::SetAlsBlocked
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::SetAlsBlocked
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetAlsBlocked0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	// data for SetAlsBlocked itself
	TRequestStatus reguestStatus;
	RMmCustomAPI::TSetAlsBlock blockStatus(RMmCustomAPI::EActivateBlock);
	
	// data for ExpectL
	TMockLtsyData1< RMmCustomAPI::TSetAlsBlock > ltsyData(blockStatus);	
    ltsyData.SerialiseL(expectData);	

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::SetAlsBlocked
 	//------------------------------------------------------------------------- 	
	iMockLTSY.ExpectL(ECustomSetAlsBlockedIPC, expectData);	
	
	customAPI.SetAlsBlocked(reguestStatus, blockStatus);
	// try to cancel request
	customAPI.CancelAsyncRequest(ECustomSetAlsBlockedIPC);
	
	iMockLTSY.CompleteL(ECustomSetAlsBlockedIPC, KErrNone);
	
	User::WaitForRequest(reguestStatus);	
	
	// TSY has started a request and it is not possible to then
    // cancel it. The best thing for the TSY to do in
    // this case is to proceed as though the Cancel never happened.
    // The server's call to the TSY cancel function will return
    // synchronously. The TSY then continues to wait for the methods
    // acknowledgement and when it receives it, the TSY will complete
    // the original request.
	ASSERT_EQUALS(KErrNone, reguestStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAB-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::SetAlsBlocked
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::SetAlsBlocked
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetAlsBlocked0004L()
	{
				
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 expectData2;
	CleanupClosePushL(expectData2);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);
		
	// data for the first SetAlsBlocked 
	TRequestStatus reguestStatus;
	RMmCustomAPI::TSetAlsBlock blockStatus(RMmCustomAPI::EDeactivateBlock);
	
	// data for the first ExpectL
	TMockLtsyData1< RMmCustomAPI::TSetAlsBlock > ltsyData(blockStatus);	
    ltsyData.SerialiseL(expectData);
    
    
    // data for the second SetAlsBlocked 
	TRequestStatus reguestStatus2;
	RMmCustomAPI::TSetAlsBlock blockStatus2(RMmCustomAPI::EActivateBlock);
	
	// data for the second ExpectL
	TMockLtsyData1< RMmCustomAPI::TSetAlsBlock > ltsyData2(blockStatus2);	
    ltsyData2.SerialiseL(expectData2);
    

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::SetAlsBlocked
 	//-------------------------------------------------------------------------
 	
 	
 	// first request
 	iMockLTSY.ExpectL(ECustomSetAlsBlockedIPC, expectData);	
	iMockLTSY.CompleteL(ECustomSetAlsBlockedIPC, KErrNone, 10);	
	customAPI.SetAlsBlocked(reguestStatus, blockStatus);

	// there is no way to check KErrServerBusy return value because second 
	// request is put to a wating queue by ETel and it is resumed only after 
	// first request is completed
		

	customAPI2.SetAlsBlocked(reguestStatus2, blockStatus2);
	
	// wait for the first request
	User::WaitForRequest(reguestStatus);	
	ASSERT_EQUALS(KErrNone, reguestStatus.Int());	
	
	// wait for the second request
	User::WaitForRequest(reguestStatus2);
	// Documentation says that KErrServerBusy should be in case of multiple requests

	ASSERT_EQUALS(KErrServerBusy, reguestStatus2.Int());
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(7, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAB-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SetAlsBlocked with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetAlsBlocked and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetAlsBlocked0005L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	// data for SetAlsBlocked itself
	TRequestStatus reguestStatus;
	RMmCustomAPI::TSetAlsBlock blockStatus(RMmCustomAPI::EActivateBlock);
	
	// data for ExpectL
	TMockLtsyData1< RMmCustomAPI::TSetAlsBlock > ltsyData(blockStatus);	
    ltsyData.SerialiseL(expectData);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::SetAlsBlocked
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomSetAlsBlockedIPC, expectData);		
	customAPI.SetAlsBlocked(reguestStatus, blockStatus);
	
	User::WaitForRequest(reguestStatus);
	ASSERT_EQUALS(KErrTimedOut, reguestStatus.Int());
	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(3, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGAB-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetAlsBlocked
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetAlsBlocked
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetAlsBlocked0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	// data for GetAlsBlocked itself
	TRequestStatus reguestStatus;
	RMmCustomAPI::TGetAlsBlockStatus blockStatus(RMmCustomAPI::EBlockStatusUnknown);
	
	// data for CompleteL
	RMmCustomAPI::TGetAlsBlockStatus completeBlockStatus(RMmCustomAPI::EBlockStatusActive);
	TMockLtsyData1< RMmCustomAPI::TGetAlsBlockStatus > ltsyData(completeBlockStatus);	
    ltsyData.SerialiseL(completeData);


 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomGetAlsBlockedIPC, KErrNotSupported);
		
	customAPI.GetAlsBlocked(reguestStatus, blockStatus);
	
	User::WaitForRequest(reguestStatus);
	ASSERT_EQUALS(KErrNotSupported, reguestStatus.Int());
	AssertMockLtsyStatusL();	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomGetAlsBlockedIPC);
	iMockLTSY.CompleteL(ECustomGetAlsBlockedIPC, KErrGeneral, completeData);		
		
	customAPI.GetAlsBlocked(reguestStatus, blockStatus);
	
	User::WaitForRequest(reguestStatus);
	ASSERT_EQUALS(KErrGeneral, reguestStatus.Int());
	AssertMockLtsyStatusL();	
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetAlsBlocked when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomGetAlsBlockedIPC);
	iMockLTSY.CompleteL(ECustomGetAlsBlockedIPC, KErrNone, completeData);		
		
	customAPI.GetAlsBlocked(reguestStatus, blockStatus);
	
	User::WaitForRequest(reguestStatus);
	ASSERT_EQUALS(KErrNone, reguestStatus.Int());
	ASSERT_EQUALS(completeBlockStatus, blockStatus);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetAlsBlocked
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomGetAlsBlockedIPC, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGAB-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetAlsBlocked
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetAlsBlocked
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetAlsBlocked0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	// data for GetAlsBlocked itself
	TRequestStatus reguestStatus;
	RMmCustomAPI::TGetAlsBlockStatus blockStatus(RMmCustomAPI::EBlockStatusUnknown);
	
	// data for CompleteL
	RMmCustomAPI::TGetAlsBlockStatus completeBlockStatus(RMmCustomAPI::EBlockStatusActive);
	TMockLtsyData1< RMmCustomAPI::TGetAlsBlockStatus > ltsyData(completeBlockStatus);	
    ltsyData.SerialiseL(completeData);
    
 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetAlsBlocked
 	//-------------------------------------------------------------------------    
    iMockLTSY.ExpectL(ECustomGetAlsBlockedIPC);    
    customAPI.GetAlsBlocked(reguestStatus, blockStatus);
    
    // try to cancel request
	customAPI.CancelAsyncRequest(ECustomGetAlsBlockedIPC);	
	
	// try to Complete request
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomGetAlsBlockedIPC, KErrNone, completeData);		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// wait for request completion	
	User::WaitForRequest(reguestStatus);
	ASSERT_EQUALS(KErrCancel, reguestStatus.Int());
	AssertMockLtsyStatusL(); 
	


	CleanupStack::PopAndDestroy(3, this); 
	
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-MCGAB-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetAlsBlocked
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetAlsBlocked
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetAlsBlocked0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);	
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RMmCustomAPI customAPI2;
	CleanupClosePushL(customAPI2);
	customAPI2.Open(phone2);
	
	
	// data for the first GetAlsBlocked
	TRequestStatus reguestStatus;
	RMmCustomAPI::TGetAlsBlockStatus blockStatus(RMmCustomAPI::EBlockStatusUnknown);	
	// data for the first CompleteL
	RMmCustomAPI::TGetAlsBlockStatus completeBlockStatus(RMmCustomAPI::EBlockStatusInactive);
	TMockLtsyData1< RMmCustomAPI::TGetAlsBlockStatus > ltsyData(completeBlockStatus);	
    ltsyData.SerialiseL(completeData);    
    
	// data for the second GetAlsBlocked 
	TRequestStatus reguestStatus2;
	RMmCustomAPI::TGetAlsBlockStatus blockStatus2(RMmCustomAPI::EBlockStatusUnknown);	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetAlsBlocked
 	//------------------------------------------------------------------------- 	
 	// first request
 	iMockLTSY.ExpectL(ECustomGetAlsBlockedIPC);
	iMockLTSY.CompleteL(ECustomGetAlsBlockedIPC, KErrNone, completeData, 10);		
	customAPI.GetAlsBlocked(reguestStatus, blockStatus);
	
	// second request
 	customAPI2.GetAlsBlocked(reguestStatus2, blockStatus2);
	
	// wait for the first answer	
	User::WaitForRequest(reguestStatus);
	ASSERT_EQUALS(KErrNone, reguestStatus.Int());
	ASSERT_EQUALS(completeBlockStatus, blockStatus);

	// wait for the second answer
	User::WaitForRequest(reguestStatus2);
	ASSERT_EQUALS(KErrServerBusy, reguestStatus2.Int());
	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(6, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGAB-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetAlsBlocked with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetAlsBlocked and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetAlsBlocked0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	// data for GetAlsBlocked itself
	TRequestStatus reguestStatus;
	RMmCustomAPI::TGetAlsBlockStatus blockStatus(RMmCustomAPI::EBlockStatusUnknown);
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::GetAlsBlocked
 	//-------------------------------------------------------------------------
    iMockLTSY.ExpectL(ECustomGetAlsBlockedIPC);    
    customAPI.GetAlsBlocked(reguestStatus, blockStatus);
  	
	// wait for request completion	
	User::WaitForRequest(reguestStatus);
	
	ASSERT_EQUALS(KErrTimedOut, reguestStatus.Int());
	AssertMockLtsyStatusL(); 
	
	CleanupStack::PopAndDestroy(2, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCAPS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::CheckAlsPpSupport
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckAlsPpSupport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckAlsPpSupport0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	// data for CompleteL
	RMmCustomAPI::TAlsSupport completeAlsSupport(RMmCustomAPI::EAlsSupportOff);
	TMockLtsyData1< RMmCustomAPI::TAlsSupport > ltsyData(completeAlsSupport);	
    ltsyData.SerialiseL(completeData);   
	
	// data for CheckAlsPpSupport itself
	TRequestStatus reguestStatus;
	RMmCustomAPI::TAlsSupport alsSupport;
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC, KErrNotSupported); 
	
	customAPI.CheckAlsPpSupport(reguestStatus, alsSupport);
	
	User::WaitForRequest(reguestStatus);
	ASSERT_EQUALS(KErrNotSupported, reguestStatus.Int());	
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC); 
	iMockLTSY.CompleteL(ECustomCheckAlsPpSupportIPC, KErrGeneral, completeData);

	customAPI.CheckAlsPpSupport(reguestStatus, alsSupport);
	
	User::WaitForRequest(reguestStatus);	
	ASSERT_EQUALS(KErrGeneral, reguestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::CheckAlsPpSupport when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC); 
	iMockLTSY.CompleteL(ECustomCheckAlsPpSupportIPC, KErrNone, completeData);

	customAPI.CheckAlsPpSupport(reguestStatus, alsSupport);
	
	User::WaitForRequest(reguestStatus);	
	ASSERT_EQUALS(KErrNone, reguestStatus.Int());
	ASSERT_EQUALS(completeAlsSupport, alsSupport);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C1 just to increase coverage
 	//-------------------------------------------------------------------------
	// new data for CompleteL
	RMmCustomAPI::TAlsSupport completeAlsSupport2(RMmCustomAPI::EAlsSupportOn);
	TMockLtsyData1< RMmCustomAPI::TAlsSupport > ltsyData2(completeAlsSupport2);	
	completeData.Close();
    ltsyData2.SerialiseL(completeData); 
    
    iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC); 
	iMockLTSY.CompleteL(ECustomCheckAlsPpSupportIPC, KErrNone, completeData);

	customAPI.CheckAlsPpSupport(reguestStatus, alsSupport);
	
	User::WaitForRequest(reguestStatus);	
	ASSERT_EQUALS(KErrNone, reguestStatus.Int());
	ASSERT_EQUALS(completeAlsSupport2, alsSupport);
	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::CheckAlsPpSupport
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomCheckAlsPpSupportIPC, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCAPS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::CheckAlsPpSupport
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::CheckAlsPpSupport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckAlsPpSupport0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	// data for CompleteL
	RMmCustomAPI::TAlsSupport completeAlsSupport(RMmCustomAPI::EAlsSupportOff);
	TMockLtsyData1< RMmCustomAPI::TAlsSupport > ltsyData(completeAlsSupport);	
    ltsyData.SerialiseL(completeData);   
	
	// data for CheckAlsPpSupport itself
	TRequestStatus reguestStatus;
	RMmCustomAPI::TAlsSupport alsSupport;
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::CheckAlsPpSupport
 	//------------------------------------------------------------------------- 	
	iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC); 
	iMockLTSY.CompleteL(ECustomCheckAlsPpSupportIPC, KErrNone, completeData, 20);
	
	customAPI.CheckAlsPpSupport(reguestStatus, alsSupport);

	customAPI.CancelAsyncRequest(ECustomCheckAlsPpSupportIPC);	
	
	User::WaitForRequest(reguestStatus);	
	ASSERT_EQUALS(KErrCancel, reguestStatus.Int());
	
	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCCAPS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::CheckAlsPpSupport
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::CheckAlsPpSupport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckAlsPpSupport0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RBuf8 completeData2;
	CleanupClosePushL(completeData2);
	
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);
	
	// data for the first CompleteL
	RMmCustomAPI::TAlsSupport completeAlsSupport(RMmCustomAPI::EAlsSupportOff);
	TMockLtsyData1< RMmCustomAPI::TAlsSupport > ltsyData(completeAlsSupport);	
    ltsyData.SerialiseL(completeData);   
	
	// data for the first CheckAlsPpSupport itself
	TRequestStatus reguestStatus;
	RMmCustomAPI::TAlsSupport alsSupport;
	
	// data for the second CheckAlsPpSupport itself
	TRequestStatus reguestStatus2;
	RMmCustomAPI::TAlsSupport alsSupport2;	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::CheckAlsPpSupport
 	//-------------------------------------------------------------------------
	// send first request
	iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC); 
	iMockLTSY.CompleteL(ECustomCheckAlsPpSupportIPC, KErrNone, completeData, 10);
	customAPI.CheckAlsPpSupport(reguestStatus, alsSupport);
	
	// send second request
	customAPI2.CheckAlsPpSupport(reguestStatus2, alsSupport2);
	
	// wait for the first answer
	User::WaitForRequest(reguestStatus);	
	ASSERT_EQUALS(KErrNone, reguestStatus.Int());	
	ASSERT_EQUALS(completeAlsSupport, alsSupport);
		
	// wait for the second answer
	User::WaitForRequest(reguestStatus2);	
	ASSERT_EQUALS(KErrNone, reguestStatus2.Int());	
	ASSERT_EQUALS(completeAlsSupport, alsSupport2);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(7, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCAPS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::CheckAlsPpSupport with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckAlsPpSupport and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckAlsPpSupport0005L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
		
	// data for CheckAlsPpSupport itself
	TRequestStatus reguestStatus;
	RMmCustomAPI::TAlsSupport alsSupport;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::CheckAlsPpSupport
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC); 	
	customAPI.CheckAlsPpSupport(reguestStatus, alsSupport);
	
	// There is no time out for this IPC (ECustomCheckAlsPpSupportIPC)
	User::WaitForRequest(reguestStatus);
	ASSERT_EQUALS(KErrTimedOut, reguestStatus.Int());	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGRATS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetRemoteAlertingToneStatus
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetRemoteAlertingToneStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetRemoteAlertingToneStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	//-------------------------------------------------------------------------
	// TEST Get initial toneStatus
 	//-------------------------------------------------------------------------
	RMmCustomAPI::TRemoteAlertingToneStatus expectedStatus(RMmCustomAPI::EUiNoTone);
	RMmCustomAPI::TRemoteAlertingToneStatus toneStatus;	
	
	customAPI.GetRemoteAlertingToneStatus(toneStatus);
		
	ASSERT_EQUALS(expectedStatus, toneStatus);	
	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// TEST D: RMmCustomAPI::GetRemoteAlertingToneStatus again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
 	 	
 	// --- change tone status --- 	
 	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	// data for changing
	TBool playRemoteAlertToneLocally (ETrue);
	TMockLtsyData1<TBool> ltsyData(playRemoteAlertToneLocally);	
    ltsyData.SerialiseL(completeData);  
		
	iMockLTSY.CompleteL(ECustomGetRemoteAlertingToneStatusIPC, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	// new expected Status
	expectedStatus = RMmCustomAPI::EUiRbtTone;
	// --------------------------
		
	// get changed data
	customAPI.GetRemoteAlertingToneStatus(toneStatus);
		
	ASSERT_EQUALS(expectedStatus, toneStatus);	
	AssertMockLtsyStatusL();	
	
	//-------------------------------------------------------------------------
	// TEST D2: just for coverage increasing
 	//-------------------------------------------------------------------------
 	
 	// --- change tone status --- 	
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	// data for changing
	playRemoteAlertToneLocally = EFalse;
	TMockLtsyData1<TBool> ltsyData2(playRemoteAlertToneLocally);
	completeData.Close();	
    ltsyData2.SerialiseL(completeData);  
		
	iMockLTSY.CompleteL(ECustomGetRemoteAlertingToneStatusIPC, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	// new expected Status
	expectedStatus = RMmCustomAPI::EUiStopTone;
	// --------------------------
		
	// get changed data
	customAPI.GetRemoteAlertingToneStatus(toneStatus);
		
	ASSERT_EQUALS(expectedStatus, toneStatus);	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCO-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::CallOrigin
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CallOrigin
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCallOrigin0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);

	// data for CallOrigin itself
	TName callName;
    RMmCustomAPI::TCallOrigin origin;    
    	
	//-------------------------------------------------------------------------
	// TEST: Get origin for non-existing call
 	//-------------------------------------------------------------------------	
	customAPI.CallOrigin(callName, origin);			
	ASSERT_EQUALS(RMmCustomAPI::EUnknown, origin);
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST: Get origin for call created through ETel
 	//-------------------------------------------------------------------------	
 	RLine line;
	RCall call;
   	// Open Line and Call
    TInt errorCode = line.Open(iPhone, KMmTsyVoice1LineName);    
	ASSERT_EQUALS(KErrNone, errorCode);	
	CleanupClosePushL(line);
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
	
	// create new incoming call
	errorCode = CreateIncomingCallL(line, callId, KMmTsyVoice1LineName,callName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);		
	CleanupClosePushL(call);		
	
    errorCode = call.OpenExistingCall(line, callName);
    ASSERT_EQUALS(KErrNone, errorCode);   
    
    //Get origin for call created through ETel    		
	customAPI.CallOrigin(callName, origin);			
	ASSERT_EQUALS(RMmCustomAPI::EETelCall, origin);
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST: Get origin for Ghost call (NOT created through ETel)
 	//-------------------------------------------------------------------------		
 	TRequestStatus requestStatus;
 	// for getting ghost call name 
	TName ghostCallName;
 	line.NotifyCallAdded(requestStatus, ghostCallName);
 	
 	// ------------ Create ghost call -----------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
	RMobileCall::TMobileCallInfoV1 callInfo;	
	callId = 2;
	callInfo.iCallId = callId;
	callInfo.iStatus = RMobileCall::EStatusDialling;
	callInfo.iService = mobileService;	

	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(callId, mobileService, callInfo);
	callInfoData.SerialiseL(completeData);		
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, completeData);
    
    User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	//-------------------------------------------------------------------------
	
	// wait for NotifyCallAdded (for getting ghost call name)
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
	//Get origin for call NOT created through ETel    		
	customAPI.CallOrigin(ghostCallName, origin);		

	ASSERT_EQUALS(RMmCustomAPI::EOutsider, origin);		
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(5, this); 	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCTC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::TerminateCall
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::TerminateCall
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestTerminateCall0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    TName callName;
	TBool isError(ETrue);
	TInt callId(1);
	RLine line;
	RCall call;

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	OpenLineLC(line, iPhone, KMmTsyVoice1LineName);

	CreateAndOpenIncomingCalLC(line, call, callName, KMmTsyVoice1LineName, 
								callId, RMobileCall::EStatusRinging, RMobilePhone::EVoiceService);

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	TMockLtsyData2<TInt, TBool> mockData2(callId, isError);
	mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(ECustomTerminateCallIPC, expectData, KErrNotSupported);
    customAPI.TerminateCall(requestStatus, callName);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomTerminateCallIPC, expectData);
    iMockLTSY.CompleteL(ECustomTerminateCallIPC, KErrGeneral);
    customAPI.TerminateCall(requestStatus, callName);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::TerminateCall when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomTerminateCallIPC, expectData, KErrNone);
	iMockLTSY.CompleteL(ECustomTerminateCallIPC, KErrNone);
	customAPI.TerminateCall(requestStatus, callName);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::TerminateCall
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomTerminateCallIPC, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

 	//-------------------------------------------------------------------------
	// TerminateCall with KTerminateAllCalls
 	//-------------------------------------------------------------------------

	callName.Copy(KTerminateAllCalls);
	customAPI.TerminateCall(requestStatus, callName);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());


 	//-------------------------------------------------------------------------
	// TerminateCall with KTerminateAllCalls when there is a call with EStatusConnecting
 	//-------------------------------------------------------------------------

	RCall call2;
	callId = 2;

	CreateAndOpenIncomingCalLC(line, call2, callName, KMmTsyVoice1LineName, 
								callId, RMobileCall::EStatusConnecting, RMobilePhone::EVoiceService);

	callName.Copy(KTerminateAllCalls);
	callId = (-callId);
	isError = EFalse;
	TMockLtsyData2<TInt, TBool> mockData2b(callId, isError);
	expectData.Close();
	mockData2b.SerialiseL(expectData);
	iMockLTSY.ExpectL(ECustomTerminateCallIPC, expectData, KErrNone);
	iMockLTSY.CompleteL(ECustomTerminateCallIPC, KErrNone);
	customAPI.TerminateCall(requestStatus, callName);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());


	CleanupStack::PopAndDestroy(6, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCTC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::TerminateCall
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::TerminateCall
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestTerminateCall0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    TName callName;
	TBool isError(ETrue);
	TInt callId(1);
	RLine line;
	RCall call;
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	OpenLineLC(line, iPhone, KMmTsyVoice1LineName);
	
	CreateAndOpenIncomingCalLC(line, call, callName, KMmTsyVoice1LineName, 
								callId, RMobileCall::EStatusRinging, RMobilePhone::EVoiceService);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::TerminateCall
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	TMockLtsyData2<TInt, TBool> mockData2(callId, isError);
	mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(ECustomTerminateCallIPC, expectData);
    iMockLTSY.CompleteL(ECustomTerminateCallIPC, KErrNone, 10);
	
    customAPI.TerminateCall(requestStatus, callName);
    customAPI.CancelAsyncRequest(ECustomTerminateCallIPC);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(5); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCTC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::TerminateCall with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::TerminateCall with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestTerminateCall0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

    TName callName;
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	//-------------------------------------------------------------------------
	// Test passing callName with length equals null
	//-------------------------------------------------------------------------

    customAPI.TerminateCall(requestStatus, callName);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotFound, requestStatus.Int());

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMmCustomAPI::TerminateCall
 	//-------------------------------------------------------------------------

    _LIT(KBadParam, "Bad name");
    callName.Copy(KBadParam);
    customAPI.TerminateCall(requestStatus, callName);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotFound, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCTC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::TerminateCall
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::TerminateCall
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestTerminateCall0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	//-------------------------------------------------------------------------
	// Calls of functions for first client. 
 	//-------------------------------------------------------------------------

    TName callName1;
	TBool isError(ETrue);
	TInt callId1(1);

	RLine line1;
	RCall call1;
	
	RMmCustomAPI customAPI1;
	OpenCustomAPILC(customAPI1, iPhone);

	OpenLineLC(line1, iPhone, KMmTsyVoice1LineName);

	CreateAndOpenIncomingCalLC(line1, call1, callName1, KMmTsyVoice1LineName, 
								callId1, RMobileCall::EStatusRinging, RMobilePhone::EVoiceService);

	//-------------------------------------------------------------------------
	// Calls of functions for second client. 
 	//-------------------------------------------------------------------------

    TName callName2;
	TInt callId2(2);

	RLine line2;
	RCall call2;
	RMmCustomAPI customAPI2;

	OpenCustomAPILC(customAPI2, phone2, EFalse);

	OpenLineLC(line2, phone2, KMmTsyVoice1LineName);

	CreateAndOpenIncomingCalLC(line2, call2, callName2, KMmTsyVoice1LineName, 
								callId2, RMobileCall::EStatusRinging, RMobilePhone::EVoiceService);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::TerminateCall
 	//-------------------------------------------------------------------------

	TMockLtsyData2<TInt, TBool> mockData21(callId1, isError);
	mockData21.SerialiseL(expectData);
	iMockLTSY.ExpectL(ECustomTerminateCallIPC, expectData, KErrNone);
	iMockLTSY.CompleteL(ECustomTerminateCallIPC, KErrNone, 20);
	customAPI1.TerminateCall(requestStatus1, callName1);

	customAPI2.TerminateCall(requestStatus2, callName2);

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(10, this);
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCTC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::TerminateCall with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::TerminateCall and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestTerminateCall0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TRequestStatus requestStatus;

    TName callName;
	TBool isError(ETrue);
	TInt callId(1);
	RLine line;
	RCall call;
	RMmCustomAPI customAPI;
	
	OpenCustomAPILC(customAPI, iPhone);

	OpenLineLC(line, iPhone, KMmTsyVoice1LineName);

	CreateAndOpenIncomingCalLC(line, call, callName, KMmTsyVoice1LineName, 
								callId, RMobileCall::EStatusRinging, RMobilePhone::EVoiceService);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::TerminateCall
 	//-------------------------------------------------------------------------

	TMockLtsyData2<TInt, TBool> mockData2(callId, isError);
	mockData2.SerialiseL(expectData);
	iMockLTSY.ExpectL(ECustomTerminateCallIPC, expectData, KErrNone);
	customAPI.TerminateCall(requestStatus, callName);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(5, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCBABC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyAlsBlockedChanged
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyAlsBlockedChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyAlsBlockedChanged0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	RMmCustomAPI::TGetAlsBlockStatus completeBlockStatus;
	RMmCustomAPI::TSetAlsBlock blockStatus(RMmCustomAPI::EActivateBlock);
	RMmCustomAPI customAPI;

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	OpenCustomAPILC(customAPI, iPhone);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyAlsBlockedChanged
	// from LTSY.
	// Must be first because the request is self-reposting
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifyAlsBlockedChangedIPC, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyAlsBlockedChanged when result is not cached.
 	//-------------------------------------------------------------------------

	customAPI.NotifyAlsBlockedChanged(requestStatus, completeBlockStatus);

	TMockLtsyData1< RMmCustomAPI::TSetAlsBlock > ltsyData(blockStatus);	
	ltsyData.SerialiseL(expectData);
	iMockLTSY.ExpectL(ECustomSetAlsBlockedIPC, expectData);	
	iMockLTSY.CompleteL(ECustomSetAlsBlockedIPC, KErrNone);

	TRequestStatus requestStatus2;
	customAPI.SetAlsBlocked(requestStatus2, blockStatus);

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	AssertMockLtsyStatusL();

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(RMmCustomAPI::EBlockStatusActive, completeBlockStatus);

	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------

	customAPI.NotifyAlsBlockedChanged(requestStatus, completeBlockStatus);

	blockStatus = RMmCustomAPI::EDeactivateBlock;	
	expectData.Close();
	ltsyData.SerialiseL(expectData);
	iMockLTSY.ExpectL(ECustomSetAlsBlockedIPC, expectData);	
	iMockLTSY.CompleteL(ECustomSetAlsBlockedIPC, KErrNone);
	customAPI.SetAlsBlocked(requestStatus2, blockStatus);

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	AssertMockLtsyStatusL();

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(RMmCustomAPI::EBlockStatusInactive, completeBlockStatus);

	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCBABC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifyAlsBlockedChanged
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifyAlsBlockedChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyAlsBlockedChanged0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	RMmCustomAPI::TGetAlsBlockStatus completeBlockStatus;
	RMmCustomAPI::TSetAlsBlock blockStatus(RMmCustomAPI::EActivateBlock);
	RMmCustomAPI customAPI;

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	OpenCustomAPILC(customAPI, iPhone);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifyAlsBlockedChanged
 	//-------------------------------------------------------------------------
 	
	customAPI.NotifyAlsBlockedChanged(requestStatus, completeBlockStatus);
	customAPI.CancelAsyncRequest(ECustomNotifyAlsBlockedChangedIPC);

	TMockLtsyData1< RMmCustomAPI::TSetAlsBlock > ltsyData(blockStatus);	
	ltsyData.SerialiseL(expectData);
	iMockLTSY.ExpectL(ECustomSetAlsBlockedIPC, expectData);	
	iMockLTSY.CompleteL(ECustomSetAlsBlockedIPC, KErrNone);
	customAPI.SetAlsBlocked(requestStatus2, blockStatus);

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

    CleanupStack::PopAndDestroy(3); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCBABC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifyAlsBlockedChanged
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifyAlsBlockedChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyAlsBlockedChanged0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus reqStatus;
	RMmCustomAPI::TGetAlsBlockStatus completeBlockStatus1;
	RMmCustomAPI::TGetAlsBlockStatus completeBlockStatus2;
	RMmCustomAPI::TSetAlsBlock activateBlockStatus(RMmCustomAPI::EActivateBlock);

	RMmCustomAPI customAPI1;
	RMmCustomAPI customAPI2;

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	OpenCustomAPILC(customAPI1, iPhone);
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifyAlsBlockedChanged
 	//-------------------------------------------------------------------------

	customAPI1.NotifyAlsBlockedChanged(requestStatus1, completeBlockStatus1);
	customAPI2.NotifyAlsBlockedChanged(requestStatus2, completeBlockStatus2);

	TMockLtsyData1< RMmCustomAPI::TSetAlsBlock > ltsyData(activateBlockStatus);	
	ltsyData.SerialiseL(expectData);
	iMockLTSY.ExpectL(ECustomSetAlsBlockedIPC, expectData);	
	iMockLTSY.CompleteL(ECustomSetAlsBlockedIPC, KErrNone);
	customAPI1.SetAlsBlocked(reqStatus, activateBlockStatus);

	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(RMmCustomAPI::EBlockStatusActive, completeBlockStatus1);
	ASSERT_EQUALS(RMmCustomAPI::EBlockStatusActive, completeBlockStatus2);

	CleanupStack::PopAndDestroy(6, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetCipheringInfo
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetCipheringInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCipheringInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	RMmCustomAPI::TCipheringInfo info;

	TBool indReq(ETrue);
	TPckg<TBool> pckgIndReq(indReq);

 	//-------------------------------------------------------------------------
	// TEST D: RMmCustomAPI::GetCipheringInfo again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomGetCipheringInfoIPC);
	iMockLTSY.CompleteL(ECustomGetCipheringInfoIPC, KErrNone, pckgIndReq);
	customAPI.GetCipheringInfo(requestStatus, info);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(indReq, info.iIndStatus);
	ASSERT_TRUE(EFalse == info.iCiphStatus);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomGetCipheringInfoIPC, KErrNotSupported);
	customAPI.GetCipheringInfo(requestStatus, info);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomGetCipheringInfoIPC);
	iMockLTSY.CompleteL(ECustomGetCipheringInfoIPC, KErrGeneral, pckgIndReq);
	customAPI.GetCipheringInfo(requestStatus, info);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetCipheringInfo when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	RMmCustomAPI::TCipheringInfo strInfo;
	strInfo.iCiphStatus = ETrue;
	TPckg<RMmCustomAPI::TCipheringInfo> pckg(strInfo);
	iMockLTSY.CompleteL(ECustomNotifyCipheringInfoChangeIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iMockLTSY.ExpectL(ECustomGetCipheringInfoIPC);
	iMockLTSY.CompleteL(ECustomGetCipheringInfoIPC, KErrNone, pckgIndReq);
	customAPI.GetCipheringInfo(requestStatus, info);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(indReq, info.iIndStatus);
	ASSERT_EQUALS(strInfo.iCiphStatus, info.iCiphStatus);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetCipheringInfo
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomGetCipheringInfoIPC, KErrNone, pckgIndReq);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	CleanupStack::PopAndDestroy(2, this); 
	
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetCipheringInfo
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetCipheringInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCipheringInfo0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	RMmCustomAPI::TCipheringInfo info;

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TBool indReq(ETrue);
	TPckg<TBool> pckgIndReq(indReq);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetCipheringInfo
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	iMockLTSY.ExpectL(ECustomGetCipheringInfoIPC);
	iMockLTSY.CompleteL(ECustomGetCipheringInfoIPC, KErrNone, pckgIndReq, 10);
	customAPI.GetCipheringInfo(requestStatus, info);
	customAPI.CancelAsyncRequest(ECustomGetCipheringInfoIPC);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(2); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetCipheringInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetCipheringInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCipheringInfo0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI1;
	RMmCustomAPI customAPI2;

	OpenCustomAPILC(customAPI1, iPhone);
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	RMmCustomAPI::TCipheringInfo info1;
	RMmCustomAPI::TCipheringInfo info2;

	TBool ciphStatus(EFalse);
	TBool indReq(ETrue);
	TPckg<TBool> pckgIndReq(indReq);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetCipheringInfo
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomGetCipheringInfoIPC);
	iMockLTSY.CompleteL(ECustomGetCipheringInfoIPC, KErrNone, pckgIndReq);
	customAPI1.GetCipheringInfo(requestStatus1, info1);

	customAPI2.GetCipheringInfo(requestStatus2, info2);

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	ASSERT_EQUALS(indReq, info1.iIndStatus);
	ASSERT_EQUALS(ciphStatus, info1.iCiphStatus);

	CleanupStack::PopAndDestroy(5, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetCipheringInfo with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetCipheringInfo and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCipheringInfo0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	RMmCustomAPI::TCipheringInfo info;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::GetCipheringInfo
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomGetCipheringInfoIPC);
	customAPI.GetCipheringInfo(requestStatus, info);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNCIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyCipheringInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyCipheringInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyCipheringInfoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RMmCustomAPI::TCipheringInfo info;
	RMmCustomAPI::TCipheringInfo completeInfo;
	completeInfo.iIndStatus = ETrue;
	completeInfo.iCiphStatus = ETrue;
	TPckg<RMmCustomAPI::TCipheringInfo> pckg(completeInfo);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyCipheringInfoChange when result is not cached.
 	//-------------------------------------------------------------------------

	customAPI.NotifyCipheringInfoChange(requestStatus, info);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifyCipheringInfoChangeIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(completeInfo.iIndStatus, info.iIndStatus);
	ASSERT_EQUALS(completeInfo.iCiphStatus, info.iCiphStatus);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyCipheringInfoChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifyCipheringInfoChangeIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNCIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifyCipheringInfoChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifyCipheringInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyCipheringInfoChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RMmCustomAPI::TCipheringInfo info;
	RMmCustomAPI::TCipheringInfo completeInfo;
	completeInfo.iIndStatus = ETrue;
	completeInfo.iCiphStatus = ETrue;
	TPckg<RMmCustomAPI::TCipheringInfo> pckg(completeInfo);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifyCipheringInfoChange
 	//-------------------------------------------------------------------------

	customAPI.NotifyCipheringInfoChange(requestStatus, info);
	customAPI.CancelAsyncRequest(ECustomNotifyCipheringInfoChangeIPC);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifyCipheringInfoChangeIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNCIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifyCipheringInfoChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifyCipheringInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyCipheringInfoChange0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI1;
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI1, iPhone);
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;

	RMmCustomAPI::TCipheringInfo info1;
	RMmCustomAPI::TCipheringInfo info2;
	RMmCustomAPI::TCipheringInfo completeInfo;
	completeInfo.iIndStatus = ETrue;
	completeInfo.iCiphStatus = ETrue;
	TPckg<RMmCustomAPI::TCipheringInfo> pckg(completeInfo);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifyCipheringInfoChange
 	//-------------------------------------------------------------------------

	customAPI1.NotifyCipheringInfoChange(requestStatus1, info1);
	customAPI2.NotifyCipheringInfoChange(requestStatus2, info2);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifyCipheringInfoChangeIPC, KErrNone, pckg);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(completeInfo.iIndStatus, info1.iIndStatus);
	ASSERT_EQUALS(completeInfo.iCiphStatus, info1.iCiphStatus);
	ASSERT_EQUALS(completeInfo.iIndStatus, info2.iIndStatus);
	ASSERT_EQUALS(completeInfo.iCiphStatus, info2.iCiphStatus);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNNS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyNSPSStatus
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyNSPSStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyNSPSStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	TBool nspsIsOn(ETrue);
	TPckg<TBool> pckg(nspsIsOn);
	RMmCustomAPI::TNspsStatus nspsStatus;

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyNSPSStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifyNSPSStatusIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyNSPSStatus when result is not cached.
 	//-------------------------------------------------------------------------

	customAPI.NotifyNSPSStatus(requestStatus, nspsStatus);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifyNSPSStatusIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(RMmCustomAPI::ENspsOn, nspsStatus);

 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------

	customAPI.NotifyNSPSStatus(requestStatus, nspsStatus);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	nspsIsOn = EFalse;
	iMockLTSY.CompleteL(ECustomNotifyNSPSStatusIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(RMmCustomAPI::ENspsOff, nspsStatus);

	CleanupStack::PopAndDestroy(2, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNNS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifyNSPSStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifyNSPSStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyNSPSStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	TBool nspsIsOn(ETrue);
	TPckg<TBool> pckg(nspsIsOn);
	RMmCustomAPI::TNspsStatus nspsStatus;

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifyNSPSStatus
 	//-------------------------------------------------------------------------
 
	customAPI.NotifyNSPSStatus(requestStatus, nspsStatus);
	customAPI.CancelAsyncRequest(ECustomNotifyNSPSStatusIPC);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifyNSPSStatusIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNNS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifyNSPSStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifyNSPSStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyNSPSStatus0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI1;
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI1, iPhone);
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;

	TBool nspsIsOn(ETrue);
	TPckg<TBool> pckg(nspsIsOn);
	RMmCustomAPI::TNspsStatus nspsStatus1;
	RMmCustomAPI::TNspsStatus nspsStatus2;
	RMmCustomAPI::TNspsStatus nspsOn(RMmCustomAPI::ENspsOn);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifyNSPSStatus
 	//-------------------------------------------------------------------------

	customAPI1.NotifyNSPSStatus(requestStatus1, nspsStatus1);
	customAPI2.NotifyNSPSStatus(requestStatus2, nspsStatus2);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifyNSPSStatusIPC, KErrNone, pckg);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(nspsOn, nspsStatus1);
	ASSERT_EQUALS(nspsOn, nspsStatus2);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNW-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NetWakeup
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NetWakeup
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNetWakeup0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomNetWakeupIPC, KErrNotSupported);
	customAPI.NetWakeup(requestStatus);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomNetWakeupIPC);
	iMockLTSY.CompleteL(ECustomNetWakeupIPC, KErrGeneral);
	customAPI.NetWakeup(requestStatus);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NetWakeup when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomNetWakeupIPC);
	iMockLTSY.CompleteL(ECustomNetWakeupIPC, KErrNone);
	customAPI.NetWakeup(requestStatus);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NetWakeup
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNetWakeupIPC, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNW-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NetWakeup
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NetWakeup
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNetWakeup0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NetWakeup
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	iMockLTSY.ExpectL(ECustomNetWakeupIPC);
	iMockLTSY.CompleteL(ECustomNetWakeupIPC, KErrNone, 10);
	customAPI.NetWakeup(requestStatus);
	customAPI.CancelAsyncRequest(ECustomNetWakeupIPC);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNW-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NetWakeup
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NetWakeup
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNetWakeup0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI1;
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI1, iPhone);
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NetWakeup
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomNetWakeupIPC);
	iMockLTSY.CompleteL(ECustomNetWakeupIPC, KErrNone);
	customAPI1.NetWakeup(requestStatus1);

	iMockLTSY.ExpectL(ECustomNetWakeupIPC);
	iMockLTSY.CompleteL(ECustomNetWakeupIPC, KErrNone);
	customAPI2.NetWakeup(requestStatus2);

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	CleanupStack::PopAndDestroy(5, this);
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNW-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NetWakeup with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NetWakeup and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNetWakeup0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::NetWakeup
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomNetWakeupIPC);
	customAPI.NetWakeup(requestStatus);

	/*
	 * Test support in CTSY for RmobilePhone::GetServiceTable with timeout 
	 * IS ABOUT TO ABORT DUE TO TEF TIMEOUT.
	 */

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 401601);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); 

	}


void CCTsyCustomIPCFU::FillWithRandomDataLC( RMmCustomAPI::TViagElements*& aElem, TInt aRnd) const
	{
	
	aElem = new (ELeave) RMmCustomAPI::TViagElements( RMmCustomAPI::KViagElementCount );
	CleanupStack::PushL(aElem);
	
	_LIT(KName, "Name");
	
	RMmCustomAPI::TViagElement element;
	
	for (TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++)
		{
		element.iActiveFlag = ( i%2 == 0 ) ? RMmCustomAPI::ECityZone : RMmCustomAPI::ENotActive;
		element.iCoordinates.iR2 = 2*i + aRnd;
		element.iCoordinates.iX  = i + aRnd;
		element.iCoordinates.iY  = RMmCustomAPI::KViagElementCount - i + aRnd;
		element.iCoordinates.iZoneId = 2*i + 1 + aRnd;
		element.iName = KName;
		TBuf<2> number;
		number.Format(_L("%d"), i + aRnd);
		element.iName.Append(number);
		aElem->AppendL(element);
		}
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadViagHomeZoneParams
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadViagHomeZoneParams
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneParams0001L()
	{
	
	AuxReadViagHomeZoneParamsL();
	
	/*
	 * Test increases CMmSIMTsy::ReadViagHomeZoneParamsL coverage
	 * covers "if ( iMmCustomTsy->Phone()->NosBootState()->iSIMReady )" condition
	 */
	
	AuxReadViagHomeZoneParamsForIncreasingCoverageL();
	
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZP-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::ReadViagHomeZoneParams
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::ReadViagHomeZoneParams
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneParams0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RMmCustomAPI::TViagElement element;
	_LIT(KScp, "Lenin lives!");
	_LIT(KSmsC, "revolution");

	// Variables for call ReadViagHomeZoneParams.
	RMmCustomAPI::TViagParams param;
	RMmCustomAPI::TViagElements* elems;
	
	elems = new (ELeave) RMmCustomAPI::TViagElements( RMmCustomAPI::KViagElementCount );
	CleanupStack::PushL(elems);
	
	for (TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++)
		{			
		elems->AppendL(element);
		}

	// Variables for call CompleteL.
	RMmCustomAPI::TViagParams completeParam;
	completeParam.iScp = KScp;
	completeParam.iSmsC = KSmsC;
	completeParam.iSubscribedZoneAndVersion = 17;

	RMmCustomAPI::TViagElements* completeElems;
	
	FillWithRandomDataLC(completeElems);

	TMockLtsyData2<RMmCustomAPI::TViagParams, RMmCustomAPI::TViagElements*> 
		mockData2(completeParam, completeElems);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::ReadViagHomeZoneParams
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	iMockLTSY.ExpectL(EReadViagHomeZoneParamsIPC);
	
	mockData2.SerialiseL(completeData);
	iMockLTSY.CompleteL(EReadViagHomeZoneParamsIPC, KErrNone, completeData, 10);
	
	customAPI.ReadViagHomeZoneParams(requestStatus, param, *elems);
	
	customAPI.CancelAsyncRequest(EReadViagHomeZoneParamsIPC);

	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	CleanupStack::PopAndDestroy(5, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZP-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadViagHomeZoneParams with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadViagHomeZoneParams with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneParams0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;

	// Variables for call ReadViagHomeZoneParams.
	RMmCustomAPI::TViagParams param;
	RMmCustomAPI::TViagElements* elems = NULL;
	elems = new (ELeave) RMmCustomAPI::TViagElements( RMmCustomAPI::KViagElementCount );
	CleanupStack::PushL(elems);

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RMmCustomAPI::ReadViagHomeZoneParams
 	//-------------------------------------------------------------------------
	
	customAPI.ReadViagHomeZoneParams(requestStatus, param, *elems);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	CleanupStack::PopAndDestroy(3, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZP-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::ReadViagHomeZoneParams
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::ReadViagHomeZoneParams
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneParams0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI1;
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI1, iPhone);
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	RMmCustomAPI::TViagElement element;
	_LIT(KScp, "Lenin lives!");
	_LIT(KSmsC, "revolution");

	// Variables for call ReadViagHomeZoneParams of first client.
	RMmCustomAPI::TViagParams param1;
	RMmCustomAPI::TViagElements* elems1 = NULL;
	
	elems1 = new (ELeave) RMmCustomAPI::TViagElements( RMmCustomAPI::KViagElementCount );
	CleanupStack::PushL(elems1);
	
	for (TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++)
		{			
		elems1->AppendL(element);
		}

	// Variables for call ReadViagHomeZoneParams of second client.
	RMmCustomAPI::TViagParams param2;
	RMmCustomAPI::TViagElements* elems2 = NULL;
	
	elems2 = new (ELeave) RMmCustomAPI::TViagElements( RMmCustomAPI::KViagElementCount );
	CleanupStack::PushL(elems2);
	
	for (TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++)
		{			
		elems2->AppendL(element);
		}

	// Variables for call CompleteL.
	RMmCustomAPI::TViagParams completeParam;
	completeParam.iScp = KScp;
	completeParam.iSmsC = KSmsC;
	completeParam.iSubscribedZoneAndVersion = 17;

	RMmCustomAPI::TViagElements* completeElems;
	
	FillWithRandomDataLC(completeElems);


	TMockLtsyData2<RMmCustomAPI::TViagParams, RMmCustomAPI::TViagElements*> 
		mockData2(completeParam, completeElems);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::ReadViagHomeZoneParams
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EReadViagHomeZoneParamsIPC);
	mockData2.SerialiseL(completeData);
	iMockLTSY.CompleteL(EReadViagHomeZoneParamsIPC, KErrNone, completeData);
	customAPI1.ReadViagHomeZoneParams(requestStatus1, param1, *elems1);

	customAPI2.ReadViagHomeZoneParams(requestStatus2, param2, *elems2);

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	ASSERT_EQUALS(0, completeParam.iScp.Compare(param1.iScp));
	ASSERT_EQUALS(0, completeParam.iSmsC.Compare(param1.iSmsC));
	ASSERT_EQUALS(completeParam.iSubscribedZoneAndVersion, param1.iSubscribedZoneAndVersion);

	for (TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++)
		{
		ASSERT_EQUALS(completeElems->At(i).iActiveFlag         , elems1->At(i).iActiveFlag         );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iR2    , elems1->At(i).iCoordinates.iR2    );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iX     , elems1->At(i).iCoordinates.iX     );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iY     , elems1->At(i).iCoordinates.iY     );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iZoneId, elems1->At(i).iCoordinates.iZoneId);
		ASSERT_EQUALS(0, completeElems->At(i).iName.Compare(elems1->At(i).iName));
		}

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(9, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZP-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadViagHomeZoneParams with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadViagHomeZoneParams and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneParams0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;

	RMmCustomAPI::TViagElement element;

	// Variables for call ReadViagHomeZoneParams.
	RMmCustomAPI::TViagParams param;
	RMmCustomAPI::TViagElements* elems = NULL;
	
	elems = new (ELeave) RMmCustomAPI::TViagElements( RMmCustomAPI::KViagElementCount );
	CleanupStack::PushL(elems);
	
	for (TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++)
		{			
		elems->AppendL(element);
		}

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::ReadViagHomeZoneParams
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EReadViagHomeZoneParamsIPC);
	customAPI.ReadViagHomeZoneParams(requestStatus, param, *elems);

	/*
	 * Test support in CTSY for RMmCustomAPI::ReadViagHomeZoneParams with timeout 
	 * IS ABOUT TO ABORT DUE TO TEF TIMEOUT.
	 */

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 401702);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(3, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadViagHomeZoneCache
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadViagHomeZoneCache
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneCache0001L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));

    RMmCustomAPI customAPI;

    //----------------------------------------------
    // TEST D: CTSY will get results from the cache.
    //----------------------------------------------

    //********************************************************************************
    //* To initialize the VIAG home zone cache properly RMmCustomAPI::OpenL should be
    //* invoked _before_ EMmTsyBootNotifySimStatusReadyIPC is triggered.
    //* That's why OpenPhoneAndCustomAPILC was implemented and is used here 
    //* instead of usual OpenPhoneL/OpenCustomAPILC sequence.
    //********************************************************************************

    OpenPhoneAndCustomAPILC(customAPI);

    for(TInt c = KViagHomeZoneCacheIdMin; c <= KViagHomeZoneCacheIdMin; c++)
        {
        for(TInt r = KViagHomeZoneCacheRecordIdMin; r <= KViagHomeZoneCacheRecordIdMax; r++)
            {           
            RMmCustomAPI::TViagCacheRecordId recId;
            recId.iCacheId  = c; 
            recId.iRecordId = r;
        
            RMmCustomAPI::TViagCacheRecordContent recContent;

            TRequestStatus reqStatus;
            customAPI.ReadViagHomeZoneCache(reqStatus,recId,recContent);
            User::WaitForRequest(reqStatus);

            ASSERT_EQUALS(KErrNone,reqStatus.Int());
            
            const RMmCustomAPI::TViagCacheRecordContent& expectedContent =
                iViagHomeZoneCache[c - KViagHomeZoneCacheIdMin][r - KViagHomeZoneCacheRecordIdMin];
            ASSERT_EQUALS(expectedContent.iLac, recContent.iLac);
            ASSERT_EQUALS(expectedContent.iCellId, recContent.iCellId);
            }
        }   

    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RMmCustomAPI::ReadViagHomeZoneCache
    // from LTSY.
    //-------------------------------------------------------------------------

    // unsolicited EReadViagHomeZoneCacheIPC completion crashes CTSY
    
    RMmCustomAPI::TViagCacheRecordContent unsolicitedData;
    TPckgC<RMmCustomAPI::TViagCacheRecordContent> unsolicitedDataPckg(unsolicitedData);
    unsolicitedData.iCellId = 111;
    unsolicitedData.iLac = 222;
    
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EReadViagHomeZoneCacheIPC,KErrNone,unsolicitedDataPckg);

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(2, this); // customAPI, this
    }

/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZC-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadViagHomeZoneCache when request failed by NTSY
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadViagHomeZoneCache
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneCache0001bL()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));

    RMmCustomAPI customAPI;

    //********************************************************************************
    //* To initialize the VIAG home zone cache properly RMmCustomAPI::OpenL should be
    //* invoked _before_ EMmTsyBootNotifySimStatusReadyIPC is triggered.
    //* That's why OpenPhoneAndCustomAPILC was implemented and is used here 
    //* instead of usual OpenPhoneL/OpenCustomAPILC sequence.
    //********************************************************************************

    OpenPhoneAndCustomAPILC(customAPI, KErrGeneral);
    
    // Complete with an error
    MockPrimeEReadViagHomeZoneCacheIPCL(KViagHomeZoneCacheIdMin, KViagHomeZoneCacheRecordIdMin, ETrue, ETrue, KErrGeneral);

    RMmCustomAPI::TViagCacheRecordContent recContent;
    TRequestStatus reqStatus;
    RMmCustomAPI::TViagCacheRecordId recId;
    recId.iCacheId  = 2; 
    recId.iRecordId = 2;
    customAPI.ReadViagHomeZoneCache(reqStatus, recId, recContent);
    User::WaitForRequest(reqStatus);

    ASSERT_EQUALS(KErrGeneral,reqStatus.Int());

    //complete without an error
    for(TInt c = KViagHomeZoneCacheIdMin; c <= KViagHomeZoneCacheIdMax; c++)
        {
        for(TInt r = KViagHomeZoneCacheRecordIdMin; r <= KViagHomeZoneCacheRecordIdMax; r++)
            {
            MockPrimeEReadViagHomeZoneCacheIPCL(c,r, ETrue, ETrue, KErrNone);
            }
        }

    
    for(TInt c = KViagHomeZoneCacheIdMin; c <= KViagHomeZoneCacheIdMin; c++)
        {
        for(TInt r = KViagHomeZoneCacheRecordIdMin; r <= KViagHomeZoneCacheRecordIdMax; r++)
            {           
            recId.iCacheId  = c; 
            recId.iRecordId = r;
            customAPI.ReadViagHomeZoneCache(reqStatus,recId,recContent);
            User::WaitForRequest(reqStatus);

            ASSERT_EQUALS(KErrNone,reqStatus.Int());
            
            const RMmCustomAPI::TViagCacheRecordContent& expectedContent =
                iViagHomeZoneCache[c - KViagHomeZoneCacheIdMin][r - KViagHomeZoneCacheRecordIdMin];
            ASSERT_EQUALS(expectedContent.iLac, recContent.iLac);
            ASSERT_EQUALS(expectedContent.iCellId, recContent.iCellId);
            }
        }   

    AssertMockLtsyStatusL();


    CleanupStack::PopAndDestroy(2, this); // customAPI, this
    }

/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZC-0001c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadViagHomeZoneCache when request complete when customAPI is dead
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadViagHomeZoneCache
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneCache0001cL()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    RBuf8 data;
    data.CleanupClosePushL();

    RMmCustomAPI customAPI;
    CleanupClosePushL(customAPI);
    TInt err = iPhone.Open(iTelServer,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, err);

    err=iMockLTSY.Connect();
    ASSERT_EQUALS(KErrNone, err);

    RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;
    currentlyRetrievedCache.iCacheId    = 1;
    currentlyRetrievedCache.iRecordId   = 0;        
    TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyData(currentlyRetrievedCache);

    // Only Expect, no complete
    MockPrimeEReadViagHomeZoneCacheIPCL(KViagHomeZoneCacheIdMin, KViagHomeZoneCacheRecordIdMin, EFalse, ETrue );

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    //************************************************************
    //* Invoke RMmCustomAPI::Open prior to triggering any further 
    //* CTSY events including EMmTsyBootNotifySimStatusReadyIPC
    //************************************************************
    err = customAPI.Open(iPhone);
    ASSERT_EQUALS(KErrNone, err);

    // EMmTsyBootNotifyModemStatusReadyIPC
    iMockLTSY.CompleteL(EMmTsyBootNotifyModemStatusReadyIPC,KErrNone);

    // EMobilePhoneGetNetworkRegistrationStatus
    iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);
    iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus,KErrNone,0);

    // EMmTsyBootNotifySimStatusReadyIPC
    iMockLTSY.ExpectL(EMmTsyBootNotifySimStatusReadyIPC);
    iMockLTSY.CompleteL(EMmTsyBootNotifySimStatusReadyIPC,KErrNone,0);

    
    // EMobilePhoneGetHomeNetwork
    RMobilePhone::TMobilePhoneNetworkInfoV5 homeNetwork;
    homeNetwork.iMode = RMobilePhone::ENetworkModeWcdma;
    homeNetwork.iStatus = RMobilePhone::ENetworkStatusCurrent;
    homeNetwork.iBandInfo = RMobilePhone::EBandUnknown;
    homeNetwork.iCountryCode = _L("234");
    homeNetwork.iCdmaSID = _L("");
    homeNetwork.iAnalogSID = _L("");
    homeNetwork.iNetworkId = _L("23499");
    homeNetwork.iDisplayTag = _L("symbian");
    homeNetwork.iShortName = _L("symbian");
    homeNetwork.iLongName = _L("symbian mobile");
    homeNetwork.iAccess = RMobilePhone::ENetworkAccessUtran;
    homeNetwork.iEgprsAvailableIndicator = ETrue;
    homeNetwork.iHsdpaAvailableIndicator = ETrue;
    TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkInfoV5> homeNetworkData(homeNetwork);
    homeNetworkData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetHomeNetwork);
    iMockLTSY.CompleteL(EMobilePhoneGetHomeNetwork,KErrNone,data,0);

    // EMmTsyPhoneGetPin1DisableSupportedIPC
    TBool pin1DisableSupport = ETrue;
    TMockLtsyData1<TBool> pin1DisableSupportData(pin1DisableSupport);
    data.Close();
    pin1DisableSupportData.SerialiseL(data);
    iMockLTSY.ExpectL(EMmTsyPhoneGetPin1DisableSupportedIPC);
    iMockLTSY.CompleteL(EMmTsyPhoneGetPin1DisableSupportedIPC,KErrNone,data,0);

    // EMmTsySimRefreshRegisterIPC
    iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);
    iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrGeneral, 0);       

    // EMobilePhoneGetServiceTable
    RMobilePhone::TMobilePhoneServiceTable serviceTable = RMobilePhone::ESIMServiceTable;
    TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> serviceTableData(serviceTable);
    data.Close();
    serviceTableData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetServiceTable, data);
    RMobilePhone::TMobilePhoneServiceTableV1 serviceTableResult;
    serviceTableResult.iServices1To8  = 0xFF;
    serviceTableResult.iServices9To16 = 0xFF;
    serviceTableResult.iServices17To24= 0xFF;
    serviceTableResult.iServices25To32= 0xFF;
    serviceTableResult.iServices33To40= 0xFF;
    serviceTableResult.iServices41To48= 0xFF;
    serviceTableResult.iServices49To56= 0xFF;
    TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableResultData(serviceTableResult);
    data.Close();
    serviceTableResultData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetServiceTable,KErrNone,data,0);
    
    // EMobilePhoneGetALSLine
    RMobilePhone::TMobilePhoneALSLine alsLine = RMobilePhone::EAlternateLinePrimary;
    TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> alsLineData(alsLine);
    data.Close();
    alsLineData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetALSLine);
    iMockLTSY.CompleteL(EMobilePhoneGetALSLine,KErrNone,data,0);

    // ECustomGetIccCallForwardingStatusIPC
    iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);    

    // EMobilePhoneGetIccMessageWaitingIndicators
    RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
    TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
                                    indicatorsData(expectedMessageIndicators);
    data.Close();
    indicatorsData.SerialiseL(data); 
    iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);
    iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, data);

    iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC);
    iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
    

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // Close the customAPI.
    CleanupStack::PopAndDestroy(&customAPI);
    // Wait for server to clear...
    User::After(KOneSecond);

    // ECustomGetIccCallForwardingStatusIPC
    iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC); 
    
    for(TInt c = KViagHomeZoneCacheIdMin; c <= KViagHomeZoneCacheIdMax; c++)
        {
        for(TInt r = KViagHomeZoneCacheRecordIdMin; r <= KViagHomeZoneCacheRecordIdMax; r++)
            {
            MockPrimeEReadViagHomeZoneCacheIPCL(c,r, ETrue, ETrue, KErrNone);
            }
        }

    // Open another client
    RMmCustomAPI customAPI2;
    CleanupClosePushL(customAPI2);
    ASSERT_EQUALS(KErrNone,customAPI2.Open(iPhone));
    

    
    for(TInt c = KViagHomeZoneCacheIdMin; c <= KViagHomeZoneCacheIdMin; c++)
        {
        for(TInt r = KViagHomeZoneCacheRecordIdMin; r <= KViagHomeZoneCacheRecordIdMax; r++)
            {           
            TRequestStatus reqStatus;
            RMmCustomAPI::TViagCacheRecordId recId;
            recId.iCacheId  = c; 
            recId.iRecordId = r;
            RMmCustomAPI::TViagCacheRecordContent recContent;
            customAPI2.ReadViagHomeZoneCache(reqStatus,recId,recContent);
            User::WaitForRequest(reqStatus);

            ASSERT_EQUALS(KErrNone,reqStatus.Int());
            
            const RMmCustomAPI::TViagCacheRecordContent& expectedContent =
                iViagHomeZoneCache[c - KViagHomeZoneCacheIdMin][r - KViagHomeZoneCacheRecordIdMin];
            ASSERT_EQUALS(expectedContent.iLac, recContent.iLac);
            ASSERT_EQUALS(expectedContent.iCellId, recContent.iCellId);
            }
        }   

    AssertMockLtsyStatusL();


    CleanupStack::PopAndDestroy(3, this); // customAPI2, data, this
    }


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::ReadViagHomeZoneCache
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::ReadViagHomeZoneCache
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneCache0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhoneL();

	RMmCustomAPI customAPI;	
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus reqStatus;

	RMmCustomAPI::TViagCacheRecordId recId;
	recId.iCacheId  = KViagHomeZoneCacheIdMax;
	recId.iRecordId = KViagHomeZoneCacheRecordIdMax;
	RMmCustomAPI::TViagCacheRecordContent recContent;

	customAPI.ReadViagHomeZoneCache(reqStatus,recId,recContent);
	customAPI.CancelAsyncRequest(EReadViagHomeZoneCacheIPC);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(2, this); // customAPI, this	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadViagHomeZoneCache with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadViagHomeZoneCache with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneCache0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;	
	OpenCustomAPILC(customAPI, iPhone);
	
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to RMmCustomAPI::ReadViagHomeZoneCache
 	//-------------------------------------------------------------------------

	RMmCustomAPI::TViagCacheRecordId KBadRecId[] =
	 	{
		{KViagHomeZoneCacheIdMin - 1, KViagHomeZoneCacheRecordIdMin },
		{KViagHomeZoneCacheIdMax + 1, KViagHomeZoneCacheRecordIdMin },
		{KViagHomeZoneCacheIdMin,     KViagHomeZoneCacheRecordIdMin -1 },
		{KViagHomeZoneCacheIdMin,     KViagHomeZoneCacheRecordIdMax + 1}
	 	};

 	for(TInt i = 0; i < sizeof(KBadRecId)/sizeof(KBadRecId[0]); i++)
 		{
 		TRequestStatus reqStatus;

 		RMmCustomAPI::TViagCacheRecordContent recContent;

 		customAPI.ReadViagHomeZoneCache(reqStatus,KBadRecId[i],recContent);

 		User::WaitForRequest(reqStatus);
 		ASSERT_EQUALS(KErrArgument,reqStatus.Int());
 		}

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(2, this); // customAPI, this
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::ReadViagHomeZoneCache
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::ReadViagHomeZoneCache
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneCache0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::ReadViagHomeZoneCache
 	//-------------------------------------------------------------------------
	RMmCustomAPI customAPI2;
	RMmCustomAPI customAPI1;
	
	const RMmCustomAPI::TViagCacheRecordContent& expectedContent2 =
		iViagHomeZoneCache[0][0];

	const RMmCustomAPI::TViagCacheRecordContent& expectedContent1 =
		iViagHomeZoneCache[KViagHomeZoneCacheIdMax - KViagHomeZoneCacheIdMin][KViagHomeZoneCacheRecordIdMax - KViagHomeZoneCacheRecordIdMin];

	RMmCustomAPI::TViagCacheRecordId recId2 = {KViagHomeZoneCacheIdMin, KViagHomeZoneCacheRecordIdMin };
	RMmCustomAPI::TViagCacheRecordId recId1 = {KViagHomeZoneCacheIdMax, KViagHomeZoneCacheRecordIdMax };

	RMmCustomAPI::TViagCacheRecordContent recContent2;
	RMmCustomAPI::TViagCacheRecordContent recContent1;

	TRequestStatus reqStatus2;
	TRequestStatus reqStatus1;

    //OpenPhoneAndCustomAPILC(customAPI1);
	OpenPhoneL();
    OpenCustomAPILC(customAPI1, iPhone, ETrue);
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	// start reading from the 2-nd client. 
	customAPI2.ReadViagHomeZoneCache(reqStatus2,recId2,recContent2);

	// start reading from the 1-st client 
	customAPI1.ReadViagHomeZoneCache(reqStatus1,recId1,recContent1);

	// Complete the cashing
	TBool expect = EFalse;
    for(TInt c = KViagHomeZoneCacheIdMin; c <= KViagHomeZoneCacheIdMax; c++)
        {
        for(TInt r = KViagHomeZoneCacheRecordIdMin; r <= KViagHomeZoneCacheRecordIdMax; r++)
            {
            MockPrimeEReadViagHomeZoneCacheIPCL(c,r, ETrue, expect);
            expect = ETrue;
            }
        }
	
	// wait for results
	User::WaitForRequest(reqStatus1);
	User::WaitForRequest(reqStatus2);

	// check 1-st client results
	ASSERT_EQUALS(KErrNone,reqStatus1.Int());
	ASSERT_EQUALS(expectedContent1.iLac, recContent1.iLac);
	ASSERT_EQUALS(expectedContent1.iCellId, recContent1.iCellId);

	// check 2-nd client results
	ASSERT_EQUALS(KErrNone,reqStatus2.Int());
	ASSERT_EQUALS(expectedContent2.iLac, recContent2.iLac);
	ASSERT_EQUALS(expectedContent2.iCellId, recContent2.iCellId);

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(5, this); // customAPI2,phone2,telServer2,customAPI,this 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRVHZC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadViagHomeZoneCache with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadViagHomeZoneCache and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadViagHomeZoneCache0005L()
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 404902);
	ASSERT_TRUE(EFalse);

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;	
	OpenCustomAPILC(customAPI);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::ReadViagHomeZoneCache
 	//-------------------------------------------------------------------------
	TRequestStatus reqStatus;

 	RMmCustomAPI::TViagCacheRecordId recId = {KViagHomeZoneCacheIdMin, KViagHomeZoneCacheRecordIdMin };

	RMmCustomAPI::TViagCacheRecordContent recContent;

	// the request is never timed out
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 401805);
	ASSERT_TRUE(EFalse);

	customAPI.ReadViagHomeZoneCache(reqStatus,recId,recContent);
		
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut,reqStatus.Int());

	AssertMockLtsyStatusL();	
	
 	CleanupStack::PopAndDestroy(2, this); // customAPI, this
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCWVHZC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::WriteViagHomeZoneCache
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::WriteViagHomeZoneCache
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteViagHomeZoneCache0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RMmCustomAPI customAPI;	

	//********************************************************************************
	//* To initiazlize the VIAG home zone cache properly RMmCustomAPI::OpenL should be
	//* invoked _before_ EMmTsyBootNotifySimStatusReadyIPC is triggered.
	//* That's why OpenPhoneAndCustomAPILC was implemented and is used here 
	//* instead of usual OpenPhoneL/OpenCustomAPILC sequence.
	//********************************************************************************

	OpenPhoneAndCustomAPILC(customAPI);
	
	RBuf8 data;
	data.CleanupClosePushL();

	RMmCustomAPI::TViagCacheRecordId expRecId;
	expRecId.iCacheId  = KViagHomeZoneCacheIdMin;
	expRecId.iRecordId = KViagHomeZoneCacheRecordIdMin;
	
	RMmCustomAPI::TViagCacheRecordContent expContent; 
	expContent.iCellId = 3000;
	expContent.iLac = 4000;

	TMockLtsyData2<RMmCustomAPI::TViagCacheRecordId,
					RMmCustomAPI::TViagCacheRecordContent> expData(expRecId,expContent);
	
	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	expData.SerialiseL(data);
	iMockLTSY.ExpectL(EWriteViagHomeZoneCacheIPC,data,KErrGeneral);
	
	customAPI.WriteViagHomeZoneCache(reqStatus,expRecId,expContent);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral,reqStatus.Int())

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EWriteViagHomeZoneCacheIPC,data);
	iMockLTSY.CompleteL(EWriteViagHomeZoneCacheIPC,KErrGeneral);

	customAPI.WriteViagHomeZoneCache(reqStatus,expRecId,expContent);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral,reqStatus.Int())

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::WriteViagHomeZoneCache when result is not cached.
 	//-------------------------------------------------------------------------
	for(TInt c = KViagHomeZoneCacheIdMin; c <= KViagHomeZoneCacheIdMax; c++)
		{
		for(TInt r = KViagHomeZoneCacheRecordIdMin; r <= KViagHomeZoneCacheRecordIdMax; r++)
			{
			expRecId.iCacheId  = c;
			expRecId.iRecordId = r;

			expContent.iCellId = 3000 + c;
			expContent.iLac = 4000 + r;

			expData.Close();
			expData.SerialiseL(data);

			iMockLTSY.ExpectL(EWriteViagHomeZoneCacheIPC,data);
			iMockLTSY.CompleteL(EWriteViagHomeZoneCacheIPC,KErrNone);
		
			// write new cache content
			customAPI.WriteViagHomeZoneCache(reqStatus,expRecId,expContent);
			User::WaitForRequest(reqStatus);
			ASSERT_EQUALS(KErrNone,reqStatus.Int())
		
			// read it to verify that it was written correctly
			RMmCustomAPI::TViagCacheRecordContent newContent; 
			customAPI.ReadViagHomeZoneCache(reqStatus,expRecId,newContent);
			User::WaitForRequest(reqStatus);
			ASSERT_EQUALS(KErrNone,reqStatus.Int())
			ASSERT_EQUALS(expContent.iCellId,newContent.iCellId);
			ASSERT_EQUALS(expContent.iLac,newContent.iLac);
			}
		}

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::WriteViagHomeZoneCache
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EWriteViagHomeZoneCacheIPC,KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // data, customAPI, this
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCWVHZC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::WriteViagHomeZoneCache
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::WriteViagHomeZoneCache
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteViagHomeZoneCache0002L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;	
	OpenCustomAPILC(customAPI, iPhone);
	
	RBuf8 data;
	data.CleanupClosePushL();

	RMmCustomAPI::TViagCacheRecordId expRecId;
	expRecId.iCacheId  = KViagHomeZoneCacheIdMin;
	expRecId.iRecordId = KViagHomeZoneCacheRecordIdMin;
	
	RMmCustomAPI::TViagCacheRecordContent expContent; 
	expContent.iCellId = 3000;
	expContent.iLac = 4000;

	TMockLtsyData2<RMmCustomAPI::TViagCacheRecordId,
					RMmCustomAPI::TViagCacheRecordContent> expData(expRecId,expContent);
	
	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::WriteViagHomeZoneCache
 	//-------------------------------------------------------------------------
	expData.SerialiseL(data);
	iMockLTSY.ExpectL(EWriteViagHomeZoneCacheIPC,data);
	
	customAPI.WriteViagHomeZoneCache(reqStatus,expRecId,expContent);
	customAPI.CancelAsyncRequest(EWriteViagHomeZoneCacheIPC);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel,reqStatus.Int())

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3,this); // data, customAPI, this	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCWVHZC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::WriteViagHomeZoneCache with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::WriteViagHomeZoneCache with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteViagHomeZoneCache0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RMmCustomAPI::WriteViagHomeZoneCache
 	//-------------------------------------------------------------------------
	const RMmCustomAPI::TViagCacheRecordId KBadRecId[] =
	 	{
		{KViagHomeZoneCacheIdMin - 1, KViagHomeZoneCacheRecordIdMin },
		{KViagHomeZoneCacheIdMax + 1, KViagHomeZoneCacheRecordIdMin },
		{KViagHomeZoneCacheIdMin,     KViagHomeZoneCacheRecordIdMin -1 },
		{KViagHomeZoneCacheIdMin,     KViagHomeZoneCacheRecordIdMax + 1}
	 	};

	const RMmCustomAPI::TViagCacheRecordContent KRecContent = {0,0};

 	for(TInt i = 0; i < sizeof(KBadRecId)/sizeof(KBadRecId[0]); i++)
 		{
 		TRequestStatus reqStatus;

 		customAPI.WriteViagHomeZoneCache(reqStatus,KBadRecId[i],KRecContent);

 		User::WaitForRequest(reqStatus);
 		ASSERT_EQUALS(KErrArgument,reqStatus.Int());
 		}
 	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // customAPI, this
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCWVHZC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::WriteViagHomeZoneCache
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::WriteViagHomeZoneCache
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteViagHomeZoneCache0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	// Open 2-nd phone
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	// now open 1-st phone and two RMmCustomAPI-s 
	RMmCustomAPI customAPI1;
	RMmCustomAPI customAPI2;

	OpenPhoneAndCustomAPILC(customAPI1);
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::WriteViagHomeZoneCache
 	//-------------------------------------------------------------------------
	RMmCustomAPI::TViagCacheRecordId expRecId1;
	expRecId1.iCacheId  = KViagHomeZoneCacheIdMin;
	expRecId1.iRecordId = KViagHomeZoneCacheRecordIdMin;

	RMmCustomAPI::TViagCacheRecordId expRecId2;
	expRecId2.iCacheId  = KViagHomeZoneCacheIdMax;
	expRecId2.iRecordId = KViagHomeZoneCacheRecordIdMax;
	
	RMmCustomAPI::TViagCacheRecordContent expContent1; 
	expContent1.iCellId = 3001;
	expContent1.iLac = 4001;

	RMmCustomAPI::TViagCacheRecordContent expContent2; 
	expContent2.iCellId = 3002;
	expContent2.iLac = 4002;

	TRequestStatus reqStatus1;
	TRequestStatus reqStatus2;

	TMockLtsyData2<RMmCustomAPI::TViagCacheRecordId,
					RMmCustomAPI::TViagCacheRecordContent> expData(expRecId1,expContent1);
	
	RBuf8 data;
	data.CleanupClosePushL();

	// Only one EWriteViagHomeZoneCacheIPC request is passed to the Mock LTSY
	expData.SerialiseL(data);
	iMockLTSY.ExpectL(EWriteViagHomeZoneCacheIPC,data);
	iMockLTSY.CompleteL(EWriteViagHomeZoneCacheIPC,KErrNone);

	// Try to invoke two simultaneous EWriteViagHomeZoneCacheIPC requests.
	// The 2-nd one should fail with KErrServerBusy.
	customAPI1.WriteViagHomeZoneCache(reqStatus1,expRecId1,expContent1);
	customAPI2.WriteViagHomeZoneCache(reqStatus2,expRecId2,expContent2);
	User::WaitForRequest(reqStatus1);
	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone,reqStatus1.Int());
	ASSERT_EQUALS(KErrServerBusy,reqStatus2.Int());

	// verify that the record changed by the 1-st client is changed succesfully 
	RMmCustomAPI::TViagCacheRecordContent newContent;
	customAPI1.ReadViagHomeZoneCache(reqStatus1,expRecId1,newContent);
	User::WaitForRequest(reqStatus1);

	ASSERT_EQUALS(KErrNone,reqStatus1.Int());
	ASSERT_EQUALS(expContent1.iLac, newContent.iLac);
	ASSERT_EQUALS(expContent1.iCellId, newContent.iCellId);
	
	AssertMockLtsyStatusL();

	// data, customAPI2, customAPI1, phone2, telServer2,  this
	CleanupStack::PopAndDestroy(6, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCWVHZC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::WriteViagHomeZoneCache with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::WriteViagHomeZoneCache and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteViagHomeZoneCache0005L()
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 404902);
	ASSERT_TRUE(EFalse);

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI);

	RBuf8 data;
	data.CleanupClosePushL();

	RMmCustomAPI::TViagCacheRecordId expRecId;
	expRecId.iCacheId  = KViagHomeZoneCacheIdMax;
	expRecId.iRecordId = KViagHomeZoneCacheRecordIdMax;
	
	RMmCustomAPI::TViagCacheRecordContent expContent; 
	expContent.iCellId = 6000;
	expContent.iLac = 7000;

	TMockLtsyData2<RMmCustomAPI::TViagCacheRecordId,
					RMmCustomAPI::TViagCacheRecordContent> expData(expRecId,expContent);

	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::WriteViagHomeZoneCache
 	//-------------------------------------------------------------------------
	expData.SerialiseL(data);
	iMockLTSY.ExpectL(EWriteViagHomeZoneCacheIPC,data);
	
	customAPI.WriteViagHomeZoneCache(reqStatus,expRecId,expContent);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut,reqStatus.Int())

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // data, customAPI, this
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCCBL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ClearCallBlackList
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ClearCallBlackList
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestClearCallBlackList0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomClearCallBlackListIPC, KErrNotSupported);
    
    customAPI.ClearCallBlackList(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomClearCallBlackListIPC);

    iMockLTSY.CompleteL( ECustomClearCallBlackListIPC, KErrGeneral);

    customAPI.ClearCallBlackList(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::ClearCallBlackList when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomClearCallBlackListIPC);
    iMockLTSY.CompleteL( ECustomClearCallBlackListIPC, KErrNone);

    customAPI.ClearCallBlackList(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::ClearCallBlackList
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomClearCallBlackListIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCCBL-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::ClearCallBlackList
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::ClearCallBlackList
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestClearCallBlackList0002L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::ClearCallBlackList
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomClearCallBlackListIPC);
    iMockLTSY.CompleteL( ECustomClearCallBlackListIPC, KErrNone, 20);

    customAPI.ClearCallBlackList(requestStatus);
	
	customAPI.CancelAsyncRequest(ECustomClearCallBlackListIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 
	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCCCBL-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::ClearCallBlackList
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::ClearCallBlackList
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestClearCallBlackList0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::ClearCallBlackList
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomClearCallBlackListIPC, KErrNone);
    iMockLTSY.CompleteL( ECustomClearCallBlackListIPC, KErrNone, 20);
    customAPI.ClearCallBlackList(requestStatus);

    customAPI2.ClearCallBlackList(requestStatus2);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCCBL-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ClearCallBlackList with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ClearCallBlackList and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestClearCallBlackList0005L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

    iMockLTSY.ExpectL( ECustomClearCallBlackListIPC);

    customAPI.ClearCallBlackList(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 	

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAIN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SsAdditionalInfoNotification
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SsAdditionalInfoNotification
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSsAdditionalInfoNotification0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	_LIT8(KInfo, "Info");

	RMmCustomAPI::TSsAdditionalInfo info;
	RMmCustomAPI::TSsAdditionalInfo compInfo;
	compInfo.iOperationCode = 0xFF;
	compInfo.iAdditionalInfo.Copy(KInfo);	
	TPckg<RMmCustomAPI::TSsAdditionalInfo> pckg(compInfo);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::SsAdditionalInfoNotification
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomSsAdditionalInfoNotificationIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::SsAdditionalInfoNotification when result is not cached.
 	//-------------------------------------------------------------------------

	customAPI.SsAdditionalInfoNotification(requestStatus, info);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomSsAdditionalInfoNotificationIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(compInfo.iOperationCode == info.iOperationCode);
	ASSERT_TRUE(0 == info.iAdditionalInfo.Compare(compInfo.iAdditionalInfo));

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAIN-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::SsAdditionalInfoNotification
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::SsAdditionalInfoNotification
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSsAdditionalInfoNotification0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	_LIT8(KInfo, "Info");

	RMmCustomAPI::TSsAdditionalInfo info;
	RMmCustomAPI::TSsAdditionalInfo compInfo;
	compInfo.iOperationCode = 0xFF;
	compInfo.iAdditionalInfo.Copy(KInfo);	
	TPckg<RMmCustomAPI::TSsAdditionalInfo> pckg(compInfo);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::SsAdditionalInfoNotification
 	//-------------------------------------------------------------------------

	customAPI.SsAdditionalInfoNotification(requestStatus, info);
	customAPI.CancelAsyncRequest(ECustomSsAdditionalInfoNotificationIPC);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomSsAdditionalInfoNotificationIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAIN-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::SsAdditionalInfoNotification
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::SsAdditionalInfoNotification
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSsAdditionalInfoNotification0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI1;
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI1, iPhone);
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;

	_LIT8(KInfo, "Info");

	RMmCustomAPI::TSsAdditionalInfo info;
	RMmCustomAPI::TSsAdditionalInfo info2;
	RMmCustomAPI::TSsAdditionalInfo compInfo;
	compInfo.iOperationCode = 0xFF;
	compInfo.iAdditionalInfo.Copy(KInfo);	
	TPckg<RMmCustomAPI::TSsAdditionalInfo> pckg(compInfo);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::SsAdditionalInfoNotification
 	//-------------------------------------------------------------------------

	customAPI1.SsAdditionalInfoNotification(requestStatus, info);
	customAPI2.SsAdditionalInfoNotification(requestStatus2, info2);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomSsAdditionalInfoNotificationIPC, KErrNone, pckg);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_TRUE(compInfo.iOperationCode == info.iOperationCode);
	ASSERT_TRUE(0 == info.iAdditionalInfo.Compare(compInfo.iAdditionalInfo));

	ASSERT_TRUE(compInfo.iOperationCode == info2.iOperationCode);
	ASSERT_TRUE(0 == info2.iAdditionalInfo.Compare(compInfo.iAdditionalInfo));

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSRCN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SsRequestCompleteNotification
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SsRequestCompleteNotification
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSsRequestCompleteNotification0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	TInt status;
	const TInt compStatus = 0xFF;
	TPckg<TInt> pckg(compStatus);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::SsRequestCompleteNotification
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifySsRequestCompleteIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::SsRequestCompleteNotification when result is not cached.
 	//-------------------------------------------------------------------------

	customAPI.SsRequestCompleteNotification(requestStatus, status);

	iMockLTSY.CompleteL(ECustomNotifySsRequestCompleteIPC, KErrNone, pckg);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(status, compStatus);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSRCN-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::SsRequestCompleteNotification
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::SsRequestCompleteNotification
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSsRequestCompleteNotification0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	TInt status;
	const TInt compStatus = 0xFF;
	TPckg<TInt> pckg(compStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::SsRequestCompleteNotification
 	//-------------------------------------------------------------------------

	customAPI.SsRequestCompleteNotification(requestStatus, status);
	customAPI.CancelAsyncRequest(ECustomNotifySsRequestCompleteIPC);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECustomNotifySsRequestCompleteIPC, KErrNone, pckg);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 
	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSRCN-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::SsRequestCompleteNotification
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::SsRequestCompleteNotification
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSsRequestCompleteNotification0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI1;
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI1, iPhone);
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	TInt status;
	TInt status2;
	const TInt compStatus = 0xFF;
	TPckg<TInt> pckg(compStatus);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::SsRequestCompleteNotification
 	//-------------------------------------------------------------------------

	customAPI1.SsRequestCompleteNotification(requestStatus, status);
	customAPI2.SsRequestCompleteNotification(requestStatus2, status2);

	iMockLTSY.CompleteL(ECustomNotifySsRequestCompleteIPC, KErrNone, pckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(compStatus, status);
	ASSERT_EQUALS(compStatus, status2);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCIB-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::IsBlocked
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::IsBlocked
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestIsBlocked0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);

    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);
	TBool result;

    TUint8 codeId(KSecCodePin);

	TMockLtsyData1 <TUint8> ltsyData(codeId);
	ltsyData.SerialiseL(data);	
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomIsBlockedIPC, data, KErrNotSupported);

	TInt ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrNotSupported, ret);

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	const TBool compResult(ETrue);
	TPckg<TBool> resPckg(compResult);

    iMockLTSY.ExpectL( ECustomIsBlockedIPC, data);
    iMockLTSY.CompleteL( ECustomIsBlockedIPC, KErrGeneral, resPckg);

	ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrGeneral, ret);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::IsBlocked when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomIsBlockedIPC, data);
    iMockLTSY.CompleteL( ECustomIsBlockedIPC, KErrNone, resPckg);

	ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(compResult, result);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMmCustomAPI::IsBlocked when result is not cached.
 	//-------------------------------------------------------------------------

	type = RMmCustomAPI::ESecurityCodePin2;
    codeId = KSecCodePin2;

	data.Close();
	ltsyData.SerialiseL(data);	
    iMockLTSY.ExpectL( ECustomIsBlockedIPC, data);
    iMockLTSY.CompleteL( ECustomIsBlockedIPC, KErrNone, resPckg);

	ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(compResult, result);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C3: Completion request of
	// RMmCustomAPI::IsBlocked 
 	//-------------------------------------------------------------------------

	type = RMmCustomAPI::ESecurityCodePuk1;

	ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrArgument, ret);

	AssertMockLtsyStatusL();


 	//-------------------------------------------------------------------------
	// TEST C4: Completion request of
	// RMmCustomAPI::IsBlocked 
 	//-------------------------------------------------------------------------

	type = RMmCustomAPI::ESecurityUniversalPin;

	ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrArgument, ret);

	AssertMockLtsyStatusL();



    //- making ICC3G ----------------------------
    
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TICCType iccType(EICCTypeSim3G);

	TMockLtsyData1<TICCType> iccData(iccType);
	data.Close();
	iccData.SerialiseL(data);

	iMockLTSY.CompleteL(EMmTsySimGetICCType, KErrNone, data);    

	iMockLTSY.ExpectL(EMmTsySecurityGetSimActivePinStateIPC);
	iMockLTSY.CompleteL(EMmTsySecurityGetSimActivePinStateIPC, KErrNone);

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C5: Successful completion request of
	// RMmCustomAPI::IsBlocked when result is not cached.
 	//-------------------------------------------------------------------------

	type = RMmCustomAPI::ESecurityUniversalPin;
    codeId = KSecCodeUpin;

	data.Close();
	ltsyData.SerialiseL(data);	
    iMockLTSY.ExpectL( ECustomIsBlockedIPC, data);
    iMockLTSY.CompleteL( ECustomIsBlockedIPC, KErrNone, resPckg);

	ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(compResult, result);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C6: Successful completion request of
	// RMmCustomAPI::IsBlocked when result is not cached.
 	//-------------------------------------------------------------------------

	type = RMmCustomAPI::ESecurityCodePassPhrase;
    codeId = KSecCodeSecurity;

	data.Close();
	ltsyData.SerialiseL(data);	

    iMockLTSY.ExpectL( ECustomIsBlockedIPC, data);
    iMockLTSY.CompleteL( ECustomIsBlockedIPC, KErrNone, resPckg);

	ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(compResult, result);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::IsBlocked
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomIsBlockedIPC, KErrNone, resPckg);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCIB-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::IsBlocked with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::IsBlocked and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestIsBlocked0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);

    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);
	TBool result;

    TUint8 codeId(KSecCodePin);

	TMockLtsyData1 <TUint8> ltsyData(codeId);
	ltsyData.SerialiseL(data);	
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomIsBlockedIPC, data);

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 402401);
	// Request ECustomIsBlockedIPC is not completed by timeout.

	// to void next test blocking
	ASSERT_TRUE(EFalse);

	TInt ret = customAPI.IsBlocked(type, result);
	
	ASSERT_EQUALS(KErrTimedOut, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::CheckSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckSecurityCode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);

    TRequestStatus requestStatus;
    
    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);
    TUint8 codeId(KSecCodePin);
    RMobilePhone::TMobilePassword secCode;
   
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	TMockLtsyData2 <TUint8, RMobilePhone::TMobilePassword> ltsyData(codeId, secCode);
	ltsyData.SerialiseL(data);	

    iMockLTSY.ExpectL( ECustomCheckSecurityCodeIPC, data, KErrNotSupported);
    
    customAPI.CheckSecurityCode(requestStatus, type);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckSecurityCodeIPC, data);    
    iMockLTSY.CompleteL( ECustomCheckSecurityCodeIPC, KErrGeneral);

	TBool flag(EFalse);
	TMockLtsyData2 <TUint8, TBool> ltsyData2(codeId, flag);
	ltsyData2.SerialiseL(data2);	

	// this is because HandleRequest(ECustomCheckSecurityCodeCancelIPC) is called when completing by error
    iMockLTSY.ExpectL( ECustomCheckSecurityCodeCancelIPC, data2);    

    customAPI.CheckSecurityCode(requestStatus, type);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::CheckSecurityCode when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckSecurityCodeIPC, data);    
    iMockLTSY.CompleteL( ECustomCheckSecurityCodeIPC, KErrNone);

    customAPI.CheckSecurityCode(requestStatus, type);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();


 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMmCustomAPI::CheckSecurityCode when result is not cached.
 	//-------------------------------------------------------------------------

    type = RMmCustomAPI::ESecurityCodePin2;
    codeId = KSecCodePin2;
   
    data.Close();
	ltsyData.SerialiseL(data);	
	
    iMockLTSY.ExpectL( ECustomCheckSecurityCodeIPC, data);    
    iMockLTSY.CompleteL( ECustomCheckSecurityCodeIPC, KErrNone);

    customAPI.CheckSecurityCode(requestStatus, type);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C3: Successful completion request of
	// RMmCustomAPI::CheckSecurityCode when result is not cached.
 	//-------------------------------------------------------------------------

    type = RMmCustomAPI::ESecurityCodePuk1;
    codeId = KSecCodePuk;
   
    data.Close();
	ltsyData.SerialiseL(data);	
	
    iMockLTSY.ExpectL( ECustomCheckSecurityCodeIPC, data);    
    iMockLTSY.CompleteL( ECustomCheckSecurityCodeIPC, KErrNone);

    customAPI.CheckSecurityCode(requestStatus, type);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C4: Successful completion request of
	// RMmCustomAPI::CheckSecurityCode when result is not cached.
 	//-------------------------------------------------------------------------

    type = RMmCustomAPI::ESecurityCodePuk2;
    codeId = KSecCodePuk2;
   
    data.Close();
	ltsyData.SerialiseL(data);	
	
    iMockLTSY.ExpectL( ECustomCheckSecurityCodeIPC, data);    
    iMockLTSY.CompleteL( ECustomCheckSecurityCodeIPC, KErrNone);

    customAPI.CheckSecurityCode(requestStatus, type);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();
	

 	//-------------------------------------------------------------------------
	// TEST C5: Successful completion request of
	// RMmCustomAPI::CheckSecurityCode when result is not cached.
 	//-------------------------------------------------------------------------

    type = RMmCustomAPI::ESecurityCodePassPhrase;
    codeId = KSecCodeSecurity;
   
    data.Close();
	ltsyData.SerialiseL(data);	
	
    iMockLTSY.ExpectL( ECustomCheckSecurityCodeIPC, data);    
    iMockLTSY.CompleteL( ECustomCheckSecurityCodeIPC, KErrNone);

    customAPI.CheckSecurityCode(requestStatus, type);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C6: Completion request of
	// RMmCustomAPI::CheckSecurityCode when result type is ESecurityUniversalPin
 	//-------------------------------------------------------------------------

    type = RMmCustomAPI::ESecurityUniversalPin;

    customAPI.CheckSecurityCode(requestStatus, type);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	AssertMockLtsyStatusL();
		

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::CheckSecurityCode
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomCheckSecurityCodeIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 		
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::CheckSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::CheckSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckSecurityCode0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);

    TRequestStatus requestStatus;
    
    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);
    TUint8 codeId(KSecCodePin);
    RMobilePhone::TMobilePassword secCode;
   
	TMockLtsyData2 <TUint8, RMobilePhone::TMobilePassword> ltsyData(codeId, secCode);
	ltsyData.SerialiseL(data);	
   
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test A: Cancelling of RMmCustomAPI::CheckSecurityCode
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckSecurityCodeIPC, data);

    customAPI.CheckSecurityCode(requestStatus, type);

	TBool flag(ETrue);
	TMockLtsyData2 <TUint8, TBool> ltsyData2(codeId, flag);
	ltsyData2.SerialiseL(data2);	

    iMockLTSY.ExpectL( ECustomCheckSecurityCodeCancelIPC, data2);    
    iMockLTSY.CompleteL( ECustomCheckSecurityCodeIPC, KErrCancel);
	
	customAPI.CancelAsyncRequest(ECustomCheckSecurityCodeIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	//-------------------------------------------------------------------------
	// Test B: Cancelling of RMmCustomAPI::CheckSecurityCode
	//		Baseband returns the request successfully before handling cancel function.
	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL( ECustomCheckSecurityCodeIPC, data);
	iMockLTSY.CompleteL( ECustomCheckSecurityCodeIPC, KErrNone, 20);

	customAPI.CheckSecurityCode(requestStatus, type);

	iMockLTSY.ExpectL( ECustomCheckSecurityCodeCancelIPC, data2);

	customAPI.CancelAsyncRequest(ECustomCheckSecurityCodeIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); 				
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::CheckSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::CheckSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckSecurityCode0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

	RBuf8 data;
	CleanupClosePushL(data);

    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);
    RMmCustomAPI::TSecurityCodeType type2(RMmCustomAPI::ESecurityCodePin1);
    TUint8 codeId(KSecCodePin);
    RMobilePhone::TMobilePassword secCode;
   
	TMockLtsyData2 <TUint8, RMobilePhone::TMobilePassword> ltsyData(codeId, secCode);
	ltsyData.SerialiseL(data);	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::CheckSecurityCode
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckSecurityCodeIPC, data);
    iMockLTSY.CompleteL( ECustomCheckSecurityCodeIPC, KErrNone,20);

    customAPI.CheckSecurityCode(requestStatus, type);
    customAPI2.CheckSecurityCode(requestStatus2, type2);		
        
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCSC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::CheckSecurityCode with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckSecurityCode and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckSecurityCode0005L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);

    TRequestStatus requestStatus;
    
    RMmCustomAPI::TSecurityCodeType type(RMmCustomAPI::ESecurityCodePin1);
    TUint8 codeId(KSecCodePin);
    RMobilePhone::TMobilePassword secCode;
   
	TMockLtsyData2 <TUint8, RMobilePhone::TMobilePassword> ltsyData(codeId, secCode);
	ltsyData.SerialiseL(data);	

    iMockLTSY.ExpectL( ECustomCheckSecurityCodeIPC, data);    

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 402502);
	// Request ECustomCheckSecurityCodeIPC is not completed by timeout.
	
	// to avoid next test blocking
	ASSERT_TRUE(EFalse);

    customAPI.CheckSecurityCode(requestStatus, type);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); 			
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGAP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetActivePin
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetActivePin
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetActivePin0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneSecurityCode code;    

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetActivePin when result is not cached.
 	//-------------------------------------------------------------------------

    customAPI.GetActivePin(code);
    
    ASSERT_EQUALS(RMobilePhone::ESecurityCodePin1, code);
	
	AssertMockLtsyStatusL();	

	// now change the pin and get the changed value

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	const RMobilePhone::TMobilePhoneSecurityCode compCode(RMobilePhone::ESecurityCodePin2);
	TPckg<RMobilePhone::TMobilePhoneSecurityCode> compPckg(compCode);

    iMockLTSY.CompleteL(EMmTsySecurityGetSimActivePinStateIPC, KErrNone, compPckg);

	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    customAPI.GetActivePin(code);

    ASSERT_EQUALS(compCode, code);

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(2, this); 			
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGATD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetAirTimeDuration
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetAirTimeDuration
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetAirTimeDuration0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TTimeIntervalSeconds sec;    

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetAirTimeDuration when result is not cached.
 	//-------------------------------------------------------------------------
 	// check when there were no any calls on air

    TInt ret = customAPI.GetAirTimeDuration(sec);
	
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(0, sec.Int());

	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMmCustomAPI::GetAirTimeDuration when result is not cached.
 	//-------------------------------------------------------------------------
 	// check when there was a call on air

	// first make the connected call 

    TName callName;
	TInt callId(1);
	RLine line;
	RMobileCall call;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	OpenLineLC(line, iPhone, KMmTsyVoice1LineName);

	CreateAndOpenIncomingCalLC(line, call, callName, KMmTsyVoice1LineName, 
								callId, RMobileCall::EStatusAnswering, mobileService);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// wait a little 
	User::After(2000000);	//mcsec
	
	ret = customAPI.GetAirTimeDuration(sec);
	
	ASSERT_EQUALS(KErrNone, ret);

	ASSERT_TRUE(0 < sec.Int());

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(5, this); 			
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGNPN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetNetworkProviderName
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetNetworkProviderName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetNetworkProviderName0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TName name;
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetNetworkProviderNameIPC, KErrNotSupported);
    
    customAPI.GetNetworkProviderName(requestStatus, name);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetNetworkProviderNameIPC);

	_LIT(KName, "ProviderName");
	TName compName(KName);
	TPckg<TName> compPckg(compName);

    iMockLTSY.CompleteL( ECustomGetNetworkProviderNameIPC, KErrGeneral, compPckg);

    customAPI.GetNetworkProviderName(requestStatus, name);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetNetworkProviderName when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetNetworkProviderNameIPC);
    iMockLTSY.CompleteL( ECustomGetNetworkProviderNameIPC, KErrNone, compPckg);

    customAPI.GetNetworkProviderName(requestStatus, name);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(0 == name.Compare(compName));
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetNetworkProviderName
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomGetNetworkProviderNameIPC, KErrNone, compPckg);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this);   // customAPI, this		
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGNPN-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetNetworkProviderName
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetNetworkProviderName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetNetworkProviderName0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TName name;

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetNetworkProviderName
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetNetworkProviderNameIPC);
    

	_LIT(KName, "ProviderName");
	TName compName(KName);
	TPckg<TName> compPckg(compName);

    iMockLTSY.CompleteL( ECustomGetNetworkProviderNameIPC, KErrNone, compPckg, 20);

    customAPI.GetNetworkProviderName(requestStatus, name);
	
	customAPI.CancelAsyncRequest(ECustomGetNetworkProviderNameIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // customAPI, this		
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGNPN-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetNetworkProviderName with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetNetworkProviderName with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetNetworkProviderName0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TBuf<1> littleBuf;

    iMockLTSY.ExpectL( ECustomGetNetworkProviderNameIPC);
    
	_LIT(KName, "ProviderName");
	TName compName(KName);
	TPckg<TName> compPckg(compName);

    iMockLTSY.CompleteL( ECustomGetNetworkProviderNameIPC, KErrNone, compPckg);

    customAPI.GetNetworkProviderName(requestStatus, littleBuf);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // customAPI, this	

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGNPN-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetNetworkProviderName
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetNetworkProviderName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetNetworkProviderName0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    TName name;
    TName name2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetNetworkProviderName
 	//-------------------------------------------------------------------------

	_LIT(KName, "ProviderName");
	TName compName(KName);
	TPckg<TName> compPckg(compName);

    iMockLTSY.ExpectL( ECustomGetNetworkProviderNameIPC);
    iMockLTSY.CompleteL( ECustomGetNetworkProviderNameIPC, KErrNone, compPckg, 20);

    customAPI.GetNetworkProviderName(requestStatus, name);

    iMockLTSY.ExpectL( ECustomGetNetworkProviderNameIPC);
    
    _LIT(KName2, "ProviderName2");
	TName compName2(KName2);
	TPckg<TName> compPckg2(compName2);

    iMockLTSY.CompleteL( ECustomGetNetworkProviderNameIPC, KErrNone, compPckg2);

    customAPI2.GetNetworkProviderName(requestStatus2, name2);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	ASSERT_TRUE(0 == name.Compare(compName));
	    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_TRUE(0 == name2.Compare(compName2));
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);   // customAPI2, customAPI, phone2, telServer2, this

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGNPN-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetNetworkProviderName with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetNetworkProviderName and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetNetworkProviderName0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TName name;

    iMockLTSY.ExpectL( ECustomGetNetworkProviderNameIPC);
    
    customAPI.GetNetworkProviderName(requestStatus, name);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // customAPI, this	

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCEN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::CheckEmergencyNumber
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckEmergencyNumber
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckEmergencyNumber0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	_LIT(KEmerNum, "IfThisIsEmer?");

    TRequestStatus requestStatus;
    RMmCustomAPI::TEmerNumberCheckMode mode;
    
    mode.iCheckMode = RMmCustomAPI::EEmerNumberCheckAdvanced;
    mode.iNumber.Copy(KEmerNum);
    
    TBool result;
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 compData;
	CleanupClosePushL(compData);

	TMockLtsyData1< RMmCustomAPI::TEmerNumberCheckMode > ltsyData(mode);	
	ltsyData.SerialiseL(expectData);
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckEmergencyNumberIPC, expectData, KErrNotSupported);
    
    customAPI.CheckEmergencyNumber(requestStatus, mode, result);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckEmergencyNumberIPC, expectData);
    
    _LIT(KNumber, "Number");
    
	RMmCustomAPI::TMobileTelNumber telNumber(KNumber);
	RMmCustomAPI::TMobileTelNumber* telNumberPtr(&telNumber);

	TMockLtsyData1< RMmCustomAPI::TMobileTelNumber* > ltsyData2(telNumberPtr);	
	ltsyData2.SerialiseL(compData);
    
    iMockLTSY.CompleteL( ECustomCheckEmergencyNumberIPC, KErrGeneral, compData);

    customAPI.CheckEmergencyNumber(requestStatus, mode, result);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::CheckEmergencyNumber when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckEmergencyNumberIPC, expectData);
    iMockLTSY.CompleteL( ECustomCheckEmergencyNumberIPC, KErrNone, compData);

    customAPI.CheckEmergencyNumber(requestStatus, mode, result);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(0, mode.iNumber.Compare(telNumber));
	ASSERT_EQUALS(1, result);


 	//-------------------------------------------------------------------------
	// TEST C1: mode.iCheckMode = RMmCustomAPI::EEmerNumberCheckNormal;
 	//-------------------------------------------------------------------------

	mode.iNumber.Copy(KEmerNum);
	mode.iCheckMode = RMmCustomAPI::EEmerNumberCheckNormal;
	ltsyData.SerialiseL(expectData);

    iMockLTSY.ExpectL( ECustomCheckEmergencyNumberIPC, expectData);
    iMockLTSY.CompleteL( ECustomCheckEmergencyNumberIPC, KErrNone, compData);

    customAPI.CheckEmergencyNumber(requestStatus, mode, result);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(0, mode.iNumber.Compare(telNumber));
	ASSERT_EQUALS(1, result);


 	//-------------------------------------------------------------------------
	// TEST C2: now check with telNumberPtr = NULL
 	//-------------------------------------------------------------------------

	telNumberPtr = NULL;
	compData.Close();
	ltsyData2.SerialiseL(compData);

    iMockLTSY.ExpectL( ECustomCheckEmergencyNumberIPC, expectData);
    iMockLTSY.CompleteL( ECustomCheckEmergencyNumberIPC, KErrNone, compData);

    mode.iNumber.Copy(KEmerNum);
    customAPI.CheckEmergencyNumber(requestStatus, mode, result);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(0, mode.iNumber.Compare(KEmerNum));
	ASSERT_TRUE(EFalse == result);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::CheckEmergencyNumber
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomCheckEmergencyNumberIPC, KErrNone, compData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


	// check a branch with iISVDialNumberCheck

    TName callName;
	TInt callId(1);
	RLine line;
	RMobileCall call;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	OpenLineLC(line, iPhone, KMmTsyVoice1LineName);

	CreateAndOpenIncomingCalLC(line, call, callName, KMmTsyVoice1LineName, 
								callId, RMobileCall::EStatusIdle, mobileService);

	_LIT(KPhoneNumber, "135468456456");   	

	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParams;
	callParams.iIdRestrict = RMobileCall::ESendMyId;
	callParams.iAutoRedial = ETrue;
    
    mode.iCheckMode = RMmCustomAPI::EEmerNumberCheckNormal;
    mode.iNumber.Copy(KPhoneNumber);    
    expectData.Close();
	ltsyData.SerialiseL(expectData);
    iMockLTSY.ExpectL( ECustomCheckEmergencyNumberIPC, expectData, KErrNotSupported);

	TPckg<RMobileCall::TEtel3rdPartyMobileCallParamsV1> pckgCallParams(callParams);

	call.DialISV(requestStatus, pckgCallParams, KPhoneNumber);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrServerBusy, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCEN-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::CheckEmergencyNumber
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::CheckEmergencyNumber
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckEmergencyNumber0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 compData;
	CleanupClosePushL(compData);
	
	_LIT(KEmerNum, "IfThisIsEmer?");

    TRequestStatus requestStatus;
    TBool result;
    RMmCustomAPI::TEmerNumberCheckMode mode;
    
    mode.iCheckMode = RMmCustomAPI::EEmerNumberCheckAdvanced;
    mode.iNumber.Copy(KEmerNum);
       
	TMockLtsyData1< RMmCustomAPI::TEmerNumberCheckMode > ltsyData(mode);	
	ltsyData.SerialiseL(expectData);
    	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::CheckEmergencyNumber
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckEmergencyNumberIPC, expectData);

    customAPI.CheckEmergencyNumber(requestStatus, mode, result);
	
	customAPI.CancelAsyncRequest(ECustomCheckEmergencyNumberIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    _LIT(KNumber, "Number");
    
	RMmCustomAPI::TMobileTelNumber telNumber(KNumber);
	RMmCustomAPI::TMobileTelNumber* telNumberPtr(&telNumber);

	TMockLtsyData1< RMmCustomAPI::TMobileTelNumber* > ltsyData2(telNumberPtr);	
	ltsyData2.SerialiseL(compData);
    
    iMockLTSY.CompleteL( ECustomCheckEmergencyNumberIPC, KErrNone, compData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); 	
	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCCEN-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::CheckEmergencyNumber
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::CheckEmergencyNumber
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckEmergencyNumber0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 compData;
	CleanupClosePushL(compData);


	_LIT(KEmerNum, "IfThisIsEmer?");

    TRequestStatus requestStatus;
    TBool result;
    RMmCustomAPI::TEmerNumberCheckMode mode;
    
    mode.iCheckMode = RMmCustomAPI::EEmerNumberCheckAdvanced;
    mode.iNumber.Copy(KEmerNum);
       
	TMockLtsyData1< RMmCustomAPI::TEmerNumberCheckMode > ltsyData(mode);	
	ltsyData.SerialiseL(expectData);
    	
    TRequestStatus requestStatus2;
    TBool result2;
    RMmCustomAPI::TEmerNumberCheckMode mode2;

    mode2.iCheckMode = RMmCustomAPI::EEmerNumberCheckAdvanced;
    mode2.iNumber.Copy(KEmerNum);
   
    iMockLTSY.ExpectL( ECustomCheckEmergencyNumberIPC, expectData);

    _LIT(KNumber, "Number");
    
	RMmCustomAPI::TMobileTelNumber telNumber(KNumber);
	RMmCustomAPI::TMobileTelNumber* telNumberPtr(&telNumber);

	TMockLtsyData1< RMmCustomAPI::TMobileTelNumber* > ltsyData2(telNumberPtr);	
	ltsyData2.SerialiseL(compData);
    
    iMockLTSY.CompleteL( ECustomCheckEmergencyNumberIPC, KErrNone, compData, 10);

    customAPI.CheckEmergencyNumber(requestStatus, mode, result);
    customAPI2.CheckEmergencyNumber(requestStatus2, mode2, result2);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(7, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCEN-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::CheckEmergencyNumber with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckEmergencyNumber and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckEmergencyNumber0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	_LIT(KEmerNum, "IfThisIsEmer?");

    TRequestStatus requestStatus;
    TBool result;
    RMmCustomAPI::TEmerNumberCheckMode mode;
    
    mode.iCheckMode = RMmCustomAPI::EEmerNumberCheckAdvanced;
    mode.iNumber.Copy(KEmerNum);
       
	TMockLtsyData1< RMmCustomAPI::TEmerNumberCheckMode > ltsyData(mode);	
	ltsyData.SerialiseL(expectData);
    	
    iMockLTSY.ExpectL( ECustomCheckEmergencyNumberIPC, expectData);

    customAPI.CheckEmergencyNumber(requestStatus, mode, result);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNPCR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyPndCacheReady
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyPndCacheReady
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyPndCacheReady0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TName name;

	RMobilePhoneBookStore bookStore;
	TName pbName(KETelIccAdnPhoneBook);	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of RMmCustomAPI::NotifyPndCacheReady
 	//-------------------------------------------------------------------------

	customAPI.NotifyPndCacheReady(requestStatus, name);

    OpenPhoneBookStoreL(bookStore, pbName, iPhone);
	CleanupClosePushL(bookStore);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(0, name.Compare(pbName));
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); 
	
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-MCNPCR-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyPndCacheReady when caching has been originated from SIM.
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyPndCacheReady
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyPndCacheReady0001bL()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMmCustomAPI customAPI;
    OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TName name;

    TName pbName(KETelIccAdnPhoneBook); 

    //-------------------------------------------------------------------------
    // TEST A: Successful completion request of RMmCustomAPI::NotifyPndCacheReady
    //              when caching has been originated from SIM.
    //-------------------------------------------------------------------------
    customAPI.NotifyPndCacheReady(requestStatus, name);

    TInt simError = KErrNone;

    RMobilePhoneBookStore bookStore;
    OpenPhoneBookStoreWithSIMRefreshL(bookStore, pbName, iPhone, simError);
    CleanupClosePushL(bookStore);

    User::WaitForRequest(requestStatus);    
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(0, name.Compare(pbName));

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(3, this);    

    //----------------------------------------------------------
    //  Telephony server has to be opened to test this
    //      scenario otherwise the below condition is not
    //      easy to test.
    //----------------------------------------------------------
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RMmCustomAPI customAPI2;
    OpenCustomAPILC(customAPI2, iPhone);

    TRequestStatus requestStatus2;
    TName name2; 

    //-------------------------------------------------------------------------
    // TEST B: Unsuccessful completion request of RMmCustomAPI::NotifyPndCacheReady
    //              when caching has been originated from SIM.
    //-------------------------------------------------------------------------    
    customAPI2.NotifyPndCacheReady(requestStatus2, name2);

    RMobilePhoneBookStore bookStore2;
    simError = KErrGeneral;
    OpenPhoneBookStoreWithSIMRefreshL(bookStore2, pbName, iPhone, simError);
    CleanupClosePushL(bookStore2);

    User::WaitForRequest(requestStatus2);    
    ASSERT_EQUALS(KErrGeneral, requestStatus2.Int());

    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(3, this);    

    }

/**
@SYMTestCaseID BA-CTSY-CIPC-MCNPCR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifyPndCacheReady
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifyPndCacheReady
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyPndCacheReady0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TName name;
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifyPndCacheReady
 	//-------------------------------------------------------------------------

	customAPI.NotifyPndCacheReady(requestStatus, name);

 	_LIT(KName, "Name");
 	
 	TName compName(KName);
	TPckg<TName> compNamePckg(compName);	

    iMockLTSY.CompleteL(ECustomNotifyPndCacheReadyIPC, KErrNone, compNamePckg, 20);
		
	customAPI.CancelAsyncRequest(ECustomNotifyPndCacheReadyIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(2); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNPCR-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyPndCacheReady with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyPndCacheReady with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyPndCacheReady0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TName name;

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);

	RBuf8 data3;
	CleanupClosePushL(data3);
	
	RMobilePhoneBookStore bookStore;
	TName pbName(KETelIccAdnPhoneBook);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
            
    TMockLtsyPhoneBookData0 storeInitData(pbName);
    storeInitData.SerialiseL(data);
    
    CStorageInfoData storageData;

	SetStorageInfoData(storageData);
        
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(pbName, storageData); 
	retStoreInitC.SerialiseL(data2);
	
	//EMmTsyPhoneBookStoreCacheIPC
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);
    
    TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData(pbName, cache);
    storeCacheData.SerialiseL(data3);	
                  
 	//-------------------------------------------------------------------------
	// TEST 1: completion by error value from CMmPhoneBookStoreTsy::CompletePBStoreInitializationL()
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrGeneral, data2);

	customAPI.NotifyPndCacheReady(requestStatus, name);

	TInt ret = bookStore.Open(iPhone, pbName);
	ASSERT_EQUALS(KErrNone, ret);	    	
	CleanupClosePushL(bookStore);

    User::WaitForRequest(mockLtsyStatus);        	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	ASSERT_EQUALS(0, name.Compare(pbName));
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1);

 	//-------------------------------------------------------------------------
	// TEST 2: completion by error value from CMmPhoneBookStoreTsy::CompleteCachingL()
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);	
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2);

	iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, data);
	iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, KErrGeneral, data3);

	customAPI.NotifyPndCacheReady(requestStatus, name);

	ret = bookStore.Open(iPhone, pbName);
	ASSERT_EQUALS(KErrNone, ret);	    	
	CleanupClosePushL(bookStore);

    User::WaitForRequest(mockLtsyStatus);        	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	ASSERT_EQUALS(0, name.Compare(pbName));

	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(7, this); 

	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCNPCR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifyPndCacheReady
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifyPndCacheReady
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyPndCacheReady0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

    TName name;
    TName name2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifyPndCacheReady
 	//-------------------------------------------------------------------------

	customAPI.NotifyPndCacheReady(requestStatus, name);
	customAPI2.NotifyPndCacheReady(requestStatus2, name2);

	RMobilePhoneBookStore bookStore;
	TName pbName(KETelIccAdnPhoneBook);	

    OpenPhoneBookStoreL(bookStore, pbName, iPhone);
	CleanupClosePushL(bookStore);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(0, name.Compare(pbName));

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(0, name2.Compare(pbName));

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this);
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGPCS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetPndCacheStatus
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetPndCacheStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetPndCacheStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TPndCacheStatus status;

	// TEST C: Successful completion request of
	// RMmCustomAPI::GetPndCacheStatus when result is not cached.
	// Check KETelIccAdnPhoneBook 	

	TName name;
	name.Copy(KETelIccAdnPhoneBook);

    customAPI.GetPndCacheStatus(requestStatus, status, name);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_TRUE( RMmCustomAPI::ECacheNotReady == status);

	AssertMockLtsyStatusL();		


	// TEST C2: Successful completion request of
	// RMmCustomAPI::GetPndCacheStatus when result is not cached.
	// Check KETelIccFdnPhoneBook 	

	name.Copy(KETelIccFdnPhoneBook);

    customAPI.GetPndCacheStatus(requestStatus, status, name);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_TRUE( RMmCustomAPI::ECacheNotReady == status);

	AssertMockLtsyStatusL();	

	// TEST C3: Successful completion request of
	// RMmCustomAPI::GetPndCacheStatus when result is not cached.
	// Check KETelIccBdnPhoneBook  	

	name.Copy(KETelIccBdnPhoneBook );

    customAPI.GetPndCacheStatus(requestStatus, status, name);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_TRUE( RMmCustomAPI::ECacheNotUsed == status);

	AssertMockLtsyStatusL();	

	// TEST C4: Successful completion request of
	// RMmCustomAPI::GetPndCacheStatus when result is not cached.
	// Check KETelIccSdnPhoneBook  	

	name.Copy(KETelIccSdnPhoneBook );

    customAPI.GetPndCacheStatus(requestStatus, status, name);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_TRUE( RMmCustomAPI::ECacheNotUsed == status);

	AssertMockLtsyStatusL();	

	// TEST C5: Successful completion request of
	// RMmCustomAPI::GetPndCacheStatus when result is not cached.
	// Check KETelIccVoiceMailBox   	

	name.Copy(KETelIccVoiceMailBox  );

    customAPI.GetPndCacheStatus(requestStatus, status, name);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_TRUE( RMmCustomAPI::ECacheNotUsed == status);

	AssertMockLtsyStatusL();
	
	// TEST C6: Successful completion request of
	// RMmCustomAPI::GetPndCacheStatus.
	// Get cache ready.
	
	name.Copy(KETelIccAdnPhoneBook  );
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);

    customAPI.GetPndCacheStatus(requestStatus, status, name);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_TRUE( RMmCustomAPI::ECacheReady == status);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 	
			
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCGPCS-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetPndCacheStatus with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetPndCacheStatus with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetPndCacheStatus0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TPndCacheStatus status;

	// Check non-existing phoneBookStore

	_LIT(KBadName, "BadName");

	TName name;
	name.Copy(KBadName);

    customAPI.GetPndCacheStatus(requestStatus, status, name);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(2, this); 				

	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCGON-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetOperatorName
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetOperatorName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetOperatorName0001L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TOperatorNameInfo info;
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetOperatorNameIPC, KErrNotSupported);
    
    customAPI.GetOperatorName(requestStatus, info);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetOperatorNameIPC);

	_LIT(KName, "OperName");

    RMmCustomAPI::TOperatorNameInfo compInfo;
    compInfo.iType = RMmCustomAPI::EOperatorNameProgrammableLatin;
    compInfo.iName.Copy(KName);
    TPckg<RMmCustomAPI::TOperatorNameInfo> compInfoPckg(compInfo);

    iMockLTSY.CompleteL( ECustomGetOperatorNameIPC, KErrGeneral, compInfoPckg);

    customAPI.GetOperatorName(requestStatus, info);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetOperatorName when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetOperatorNameIPC);
    iMockLTSY.CompleteL( ECustomGetOperatorNameIPC, KErrNone, compInfoPckg);

    customAPI.GetOperatorName(requestStatus, info);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(compInfo.iType == info.iType);
	ASSERT_TRUE(0 == info.iName.Compare(compInfo.iName));
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetOperatorName
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomGetOperatorNameIPC, KErrNone, compInfoPckg);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 	
		
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGON-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetOperatorName
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetOperatorName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetOperatorName0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TOperatorNameInfo info;

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetOperatorName
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetOperatorNameIPC);
    
	_LIT(KName, "OperName");

    RMmCustomAPI::TOperatorNameInfo compInfo;
    compInfo.iType = RMmCustomAPI::EOperatorNameProgrammableLatin;
    compInfo.iName.Copy(KName);
    TPckg<RMmCustomAPI::TOperatorNameInfo> compInfoPckg(compInfo);
    
    iMockLTSY.CompleteL( ECustomGetOperatorNameIPC, KErrNone, compInfoPckg, 20);

    customAPI.GetOperatorName(requestStatus, info);
	
	customAPI.CancelAsyncRequest(ECustomGetOperatorNameIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 	
	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCGON-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetOperatorName
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetOperatorName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetOperatorName0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    RMmCustomAPI::TOperatorNameInfo info;
    RMmCustomAPI::TOperatorNameInfo info2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetOperatorName
 	//-------------------------------------------------------------------------

	_LIT(KName, "OperName");

    RMmCustomAPI::TOperatorNameInfo compInfo;
    compInfo.iType = RMmCustomAPI::EOperatorNameProgrammableLatin;
    compInfo.iName.Copy(KName);
    TPckg<RMmCustomAPI::TOperatorNameInfo> compInfoPckg(compInfo);

    iMockLTSY.ExpectL( ECustomGetOperatorNameIPC, KErrNone);
    iMockLTSY.CompleteL( ECustomGetOperatorNameIPC, KErrNone, compInfoPckg, 20);

    customAPI.GetOperatorName(requestStatus, info);

	_LIT(KName2, "OperName2");

    RMmCustomAPI::TOperatorNameInfo compInfo2;
    compInfo2.iType = RMmCustomAPI::EOperatorNameNitzShort;
    compInfo2.iName.Copy(KName2);
    TPckg<RMmCustomAPI::TOperatorNameInfo> compInfoPckg2(compInfo2);

    iMockLTSY.ExpectL( ECustomGetOperatorNameIPC, KErrNone);
    iMockLTSY.CompleteL( ECustomGetOperatorNameIPC, KErrNone, compInfoPckg2);

    customAPI2.GetOperatorName(requestStatus2, info2);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	ASSERT_TRUE(compInfo.iType == info.iType);
	ASSERT_TRUE(0 == info.iName.Compare(compInfo.iName));
	    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_TRUE(compInfo2.iType == info2.iType);
	ASSERT_TRUE(0 == info2.iName.Compare(compInfo2.iName));
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGON-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetOperatorName with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetOperatorName and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetOperatorName0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TOperatorNameInfo info;

    iMockLTSY.ExpectL( ECustomGetOperatorNameIPC);

    customAPI.GetOperatorName(requestStatus, info);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGPOL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetProgrammableOperatorLogo
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetProgrammableOperatorLogo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetProgrammableOperatorLogo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    RMmCustomAPI::TOperatorId oper;
    RMmCustomAPI::TOperatorLogo logo;
    
    TBuf8<128> buf;
    logo.iOperatorLogo = &buf;

	oper.iMcc = 0;
	oper.iMnc = 0;    
       
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 compData;
	CleanupClosePushL(compData);
	TRequestStatus requestStatus;

	TMockLtsyData1< RMmCustomAPI::TOperatorId > ltsyData(oper);	
	ltsyData.SerialiseL(expectData);
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetProgrammableOperatorLogoIPC, expectData, KErrNotSupported);
    
    customAPI.GetProgrammableOperatorLogo(requestStatus, oper, logo);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetProgrammableOperatorLogoIPC, expectData);
        
    _LIT8(KBuf, "BufBufBufBufBuf");
    TBuf8<KSerialNumberLength> compBuf(KBuf);
    
    RMmCustomAPI::TOperatorId operComp;
    RMmCustomAPI::TOperatorLogo logoComp;

	operComp.iMcc = 1;
	operComp.iMnc = 1;    
	
	logoComp.iLogoInfo.iLogoWidth = 2;
	logoComp.iLogoInfo.iLogoHeight = 2;
	logoComp.iLogoInfo.iCompression = 2;
	logoComp.iLogoInfo.iDepth = RMmCustomAPI::EBitPerPixel1;
	logoComp.iOperatorLogo = &compBuf;

	TMockLtsyData2< RMmCustomAPI::TOperatorId, RMmCustomAPI::TOperatorLogo> ltsyData2(operComp, logoComp);
    ltsyData2.SerialiseL(compData);       
    
    iMockLTSY.CompleteL( ECustomGetProgrammableOperatorLogoIPC, KErrGeneral, compData);

    customAPI.GetProgrammableOperatorLogo(requestStatus, oper, logo);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetProgrammableOperatorLogo when result is not cached.
 	//-------------------------------------------------------------------------

  
    iMockLTSY.ExpectL( ECustomGetProgrammableOperatorLogoIPC, expectData);
    iMockLTSY.CompleteL( ECustomGetProgrammableOperatorLogoIPC, KErrNone, compData);

    customAPI.GetProgrammableOperatorLogo(requestStatus, oper, logo);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(operComp.iMcc == oper.iMcc);
	ASSERT_TRUE(operComp.iMnc == oper.iMnc);
	ASSERT_TRUE(logoComp.iLogoInfo.iLogoWidth == logo.iLogoInfo.iLogoWidth);
	ASSERT_TRUE(logoComp.iLogoInfo.iLogoHeight == logo.iLogoInfo.iLogoHeight);
	ASSERT_TRUE(logoComp.iLogoInfo.iCompression == logo.iLogoInfo.iCompression);
	ASSERT_TRUE(logoComp.iLogoInfo.iDepth == logo.iLogoInfo.iDepth);
	ASSERT_TRUE(0 == logoComp.iOperatorLogo->Compare(*logo.iOperatorLogo));

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetProgrammableOperatorLogo
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomGetProgrammableOperatorLogoIPC, KErrNone, compData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGPOL-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetProgrammableOperatorLogo
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetProgrammableOperatorLogo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetProgrammableOperatorLogo0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 compData;
	CleanupClosePushL(compData);
	
    RMmCustomAPI::TOperatorId oper;
    RMmCustomAPI::TOperatorLogo logo;

	oper.iMcc = 0;
	oper.iMnc = 0;
       
	TMockLtsyData1< RMmCustomAPI::TOperatorId > ltsyData(oper);	
	ltsyData.SerialiseL(expectData);
    	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetProgrammableOperatorLogo
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetProgrammableOperatorLogoIPC, expectData);

    customAPI.GetProgrammableOperatorLogo(requestStatus, oper, logo);
	
	customAPI.CancelAsyncRequest(ECustomGetProgrammableOperatorLogoIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    RMmCustomAPI::TOperatorId operComp;
    RMmCustomAPI::TOperatorLogo logoComp;
    
    _LIT8(KBuf, "BufBufBufBufBuf");
    TBuf8<KSerialNumberLength> compBuf(KBuf);
	logoComp.iOperatorLogo = &compBuf;
    
    TMockLtsyData2< RMmCustomAPI::TOperatorId, RMmCustomAPI::TOperatorLogo> ltsyData2(operComp, logoComp);
    ltsyData2.SerialiseL(compData);       

    iMockLTSY.CompleteL( ECustomGetProgrammableOperatorLogoIPC, KErrNone, compData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); 	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCGPOL-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetProgrammableOperatorLogo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetProgrammableOperatorLogo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetProgrammableOperatorLogo0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);



	TRequestStatus requestStatus;
    
    RMmCustomAPI::TOperatorId oper;
    RMmCustomAPI::TOperatorLogo logo;

	oper.iMcc = 0;
	oper.iMnc = 0;    

    TBuf8<1> littleBuf;
    logo.iOperatorLogo = &littleBuf;
       
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 compData;
	CleanupClosePushL(compData);
	
	TMockLtsyData1< RMmCustomAPI::TOperatorId > ltsyData(oper);	
	ltsyData.SerialiseL(expectData);

    RMmCustomAPI::TOperatorId operComp;
    RMmCustomAPI::TOperatorLogo logoComp;

    _LIT8(KBuf, "BufBufBufBufBuf");
	TBuf8<KSerialNumberLength> compBuf(KBuf);

	operComp.iMcc = 1;
	operComp.iMnc = 1;    
	
	logoComp.iLogoInfo.iLogoWidth = 2;
	logoComp.iLogoInfo.iLogoHeight = 2;
	logoComp.iLogoInfo.iCompression = 2;
	logoComp.iLogoInfo.iDepth = RMmCustomAPI::EBitPerPixel1;
	logoComp.iOperatorLogo = &compBuf;

	TMockLtsyData2< RMmCustomAPI::TOperatorId, RMmCustomAPI::TOperatorLogo> ltsyData2(operComp, logoComp);
    ltsyData2.SerialiseL(compData);       
    
    iMockLTSY.ExpectL( ECustomGetProgrammableOperatorLogoIPC, expectData);
    iMockLTSY.CompleteL( ECustomGetProgrammableOperatorLogoIPC, KErrNone, compData);

    customAPI.GetProgrammableOperatorLogo(requestStatus, oper, logo);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTooBig, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); 		
	}




/**
@SYMTestCaseID BA-CTSY-CIPC-MCGPOL-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetProgrammableOperatorLogo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetProgrammableOperatorLogo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetProgrammableOperatorLogo0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);
  
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    RMmCustomAPI::TOperatorId oper;
    RMmCustomAPI::TOperatorLogo logo;
   
	oper.iMcc = 0;
	oper.iMnc = 0;
   
    RMmCustomAPI::TOperatorId oper2;
    RMmCustomAPI::TOperatorLogo logo2;

	oper2.iMcc = 0;
	oper2.iMnc = 0;
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 compData;
	CleanupClosePushL(compData);
	
	TMockLtsyData1< RMmCustomAPI::TOperatorId > ltsyData(oper);	
	ltsyData.SerialiseL(expectData);
    	
    iMockLTSY.ExpectL( ECustomGetProgrammableOperatorLogoIPC, expectData);

    RMmCustomAPI::TOperatorId operComp;
    RMmCustomAPI::TOperatorLogo logoComp;

    _LIT8(KBuf, "BufBufBufBufBuf");
	TBuf8<KSerialNumberLength> compBuf(KBuf);

	operComp.iMcc = 1;
	operComp.iMnc = 1;    
	
	logoComp.iLogoInfo.iLogoWidth = 2;
	logoComp.iLogoInfo.iLogoHeight = 2;
	logoComp.iLogoInfo.iCompression = 2;
	logoComp.iLogoInfo.iDepth = RMmCustomAPI::EBitPerPixel1;
	logoComp.iOperatorLogo = &compBuf;

	TMockLtsyData2< RMmCustomAPI::TOperatorId, RMmCustomAPI::TOperatorLogo> ltsyData2(operComp, logoComp);
    ltsyData2.SerialiseL(compData);       
    
    iMockLTSY.CompleteL( ECustomGetProgrammableOperatorLogoIPC, KErrNone, compData, 20);

    
	customAPI.GetProgrammableOperatorLogo(requestStatus, oper, logo);
    customAPI2.GetProgrammableOperatorLogo(requestStatus2, oper2, logo2);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(7, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGPOL-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetProgrammableOperatorLogo with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetProgrammableOperatorLogo and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetProgrammableOperatorLogo0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

    RMmCustomAPI::TOperatorId oper;
    RMmCustomAPI::TOperatorLogo logo;   
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< RMmCustomAPI::TOperatorId > ltsyData(oper);	
	ltsyData.SerialiseL(expectData);
    	
    iMockLTSY.ExpectL( ECustomGetProgrammableOperatorLogoIPC, expectData);

    customAPI.GetProgrammableOperatorLogo(requestStatus, oper, logo);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNPOLC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyProgrammableOperatorLogoChange
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyProgrammableOperatorLogoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyProgrammableOperatorLogoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

	RMmCustomAPI::TOperatorId operatorId;
	operatorId.iMcc = 0;
	operatorId.iMnc = 0;
    
    // tests for RMmCustomAPI::NotifyProgrammableOperatorLogoChange should be written when ECustomNotifyProgrammableOperatorLogoChangeIPC 
    // would be proccessed.

	customAPI.NotifyProgrammableOperatorLogoChange( requestStatus, operatorId);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNSNE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifySsNetworkEvent
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifySsNetworkEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifySsNetworkEvent0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;

    RMmCustomAPI::TSsTypeAndMode ssTypeAndMode;
    RMmCustomAPI::TSsInfo ssInfo;

    RMmCustomAPI::TSsTypeAndMode ssTypeAndModeComp;
    RMmCustomAPI::TSsInfo ssInfoComp;

	// fill with random data
	_LIT(KRemote, "Remote");

	ssTypeAndModeComp.iSsType = RMmCustomAPI::ESsForwNoReply;
	ssTypeAndModeComp.iSsMode = RMmCustomAPI::ESsModeActive;
	
	ssInfoComp.iForwMode = RMmCustomAPI::ESsOutCallForwToC;
	ssInfoComp.iCallWait = ETrue;
	ssInfoComp.iCallHold = RMmCustomAPI::ESsHoldResume;
	ssInfoComp.iConfInd = ETrue;
	ssInfoComp.iCugIndex = 15;
	ssInfoComp.iClirSuppReject = EFalse;
	ssInfoComp.iEctCallState = RMmCustomAPI::ESsEctAlerting;
	ssInfoComp.iChoice = RMmCustomAPI::ESsPresRestrictedAddress;
	ssInfoComp.iRemoteAddress.Copy(KRemote);
           
	TMockLtsyData2< RMmCustomAPI::TSsTypeAndMode, RMmCustomAPI::TSsInfo > 
						ltsyData(ssTypeAndModeComp, ssInfoComp);	
    ltsyData.SerialiseL(data); 


 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifySsNetworkEvent
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomNotifySsNetworkEventIPC, KErrNone, data);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	customAPI.NotifySsNetworkEvent(requestStatus, ssTypeAndMode, ssInfo);

    iMockLTSY.CompleteL(ECustomNotifySsNetworkEventIPC, KErrGeneral, data);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifySsNetworkEvent when result is not cached.
 	//-------------------------------------------------------------------------

	customAPI.NotifySsNetworkEvent(requestStatus, ssTypeAndMode, ssInfo);

    iMockLTSY.CompleteL(ECustomNotifySsNetworkEventIPC, KErrNone, data);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(ssTypeAndModeComp.iSsType == ssTypeAndMode.iSsType);
	ASSERT_TRUE(ssTypeAndModeComp.iSsMode == ssTypeAndMode.iSsMode);
	ASSERT_TRUE(ssInfoComp.iForwMode == ssInfo.iForwMode);
	ASSERT_TRUE(ssInfoComp.iCallWait == ssInfo.iCallWait);
	ASSERT_TRUE(ssInfoComp.iCallHold == ssInfo.iCallHold);
	ASSERT_TRUE(ssInfoComp.iConfInd == ssInfo.iConfInd);
	ASSERT_TRUE(ssInfoComp.iCugIndex == ssInfo.iCugIndex);
	ASSERT_TRUE(ssInfoComp.iClirSuppReject == ssInfo.iClirSuppReject);
	ASSERT_TRUE(ssInfoComp.iEctCallState == ssInfo.iEctCallState);
	ASSERT_TRUE(ssInfoComp.iChoice == ssInfo.iChoice);
	ASSERT_TRUE(0 == ssInfo.iRemoteAddress.Compare(ssInfoComp.iRemoteAddress));
	
	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(3, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNSNE-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifySsNetworkEvent
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifySsNetworkEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifySsNetworkEvent0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;

    RMmCustomAPI::TSsTypeAndMode ssTypeAndMode;
    RMmCustomAPI::TSsInfo ssInfo;

    RMmCustomAPI::TSsTypeAndMode ssTypeAndModeComp;
    RMmCustomAPI::TSsInfo ssInfoComp;
          
	TMockLtsyData2< RMmCustomAPI::TSsTypeAndMode, RMmCustomAPI::TSsInfo > 
						ltsyData(ssTypeAndModeComp, ssInfoComp);	
    ltsyData.SerialiseL(data); 
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifySsNetworkEvent
 	//-------------------------------------------------------------------------
	customAPI.NotifySsNetworkEvent(requestStatus, ssTypeAndMode, ssInfo);
	
	customAPI.CancelAsyncRequest(ECustomNotifySsNetworkEventIPC);

    iMockLTSY.CompleteL(ECustomNotifySsNetworkEventIPC, KErrNone, data);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	
	
	AssertMockLtsyStatusL();	
	
	CleanupStack::PopAndDestroy(3); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNSNE-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifySsNetworkEvent
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifySsNetworkEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifySsNetworkEvent0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

    RMmCustomAPI::TSsTypeAndMode ssTypeAndMode;
    RMmCustomAPI::TSsInfo ssInfo;

    RMmCustomAPI::TSsTypeAndMode ssTypeAndMode2;
    RMmCustomAPI::TSsInfo ssInfo2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifySsNetworkEvent
 	//-------------------------------------------------------------------------
	customAPI.NotifySsNetworkEvent(requestStatus, ssTypeAndMode, ssInfo);
	customAPI2.NotifySsNetworkEvent(requestStatus2, ssTypeAndMode2, ssInfo2);

	// fill with random data
	_LIT(KRemote, "Remote");

    RMmCustomAPI::TSsTypeAndMode ssTypeAndModeComp;
    RMmCustomAPI::TSsInfo ssInfoComp;

	ssTypeAndModeComp.iSsType = RMmCustomAPI::ESsForwNoReply;
	ssTypeAndModeComp.iSsMode = RMmCustomAPI::ESsModeActive;
	
	ssInfoComp.iForwMode = RMmCustomAPI::ESsOutCallForwToC;
	ssInfoComp.iCallWait = ETrue;
	ssInfoComp.iCallHold = RMmCustomAPI::ESsHoldResume;
	ssInfoComp.iConfInd = ETrue;
	ssInfoComp.iCugIndex = 15;
	ssInfoComp.iClirSuppReject = EFalse;
	ssInfoComp.iEctCallState = RMmCustomAPI::ESsEctAlerting;
	ssInfoComp.iChoice = RMmCustomAPI::ESsPresRestrictedAddress;
	ssInfoComp.iRemoteAddress.Copy(KRemote);
           
	TMockLtsyData2< RMmCustomAPI::TSsTypeAndMode, RMmCustomAPI::TSsInfo > 
						ltsyData(ssTypeAndModeComp, ssInfoComp);	
    ltsyData.SerialiseL(data); 

    iMockLTSY.CompleteL(ECustomNotifySsNetworkEventIPC, KErrNone, data);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(ssTypeAndModeComp.iSsType == ssTypeAndMode.iSsType);
	ASSERT_TRUE(ssTypeAndModeComp.iSsMode == ssTypeAndMode.iSsMode);
	ASSERT_TRUE(ssInfoComp.iForwMode == ssInfo.iForwMode);
	ASSERT_TRUE(ssInfoComp.iCallWait == ssInfo.iCallWait);
	ASSERT_TRUE(ssInfoComp.iCallHold == ssInfo.iCallHold);
	ASSERT_TRUE(ssInfoComp.iConfInd == ssInfo.iConfInd);
	ASSERT_TRUE(ssInfoComp.iCugIndex == ssInfo.iCugIndex);
	ASSERT_TRUE(ssInfoComp.iClirSuppReject == ssInfo.iClirSuppReject);
	ASSERT_TRUE(ssInfoComp.iEctCallState == ssInfo.iEctCallState);
	ASSERT_TRUE(ssInfoComp.iChoice == ssInfo.iChoice);
	ASSERT_TRUE(0 == ssInfo.iRemoteAddress.Compare(ssInfoComp.iRemoteAddress));
	
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_TRUE(ssTypeAndModeComp.iSsType == ssTypeAndMode2.iSsType);
	ASSERT_TRUE(ssTypeAndModeComp.iSsMode == ssTypeAndMode2.iSsMode);
	ASSERT_TRUE(ssInfoComp.iForwMode == ssInfo2.iForwMode);
	ASSERT_TRUE(ssInfoComp.iCallWait == ssInfo2.iCallWait);
	ASSERT_TRUE(ssInfoComp.iCallHold == ssInfo2.iCallHold);
	ASSERT_TRUE(ssInfoComp.iConfInd == ssInfo2.iConfInd);
	ASSERT_TRUE(ssInfoComp.iCugIndex == ssInfo2.iCugIndex);
	ASSERT_TRUE(ssInfoComp.iClirSuppReject == ssInfo2.iClirSuppReject);
	ASSERT_TRUE(ssInfoComp.iEctCallState == ssInfo2.iEctCallState);
	ASSERT_TRUE(ssInfoComp.iChoice == ssInfo2.iChoice);
	ASSERT_TRUE(0 == ssInfo2.iRemoteAddress.Compare(ssInfoComp.iRemoteAddress));	
	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(6, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCUS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::CancelUssdSession
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CancelUssdSession
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCancelUssdSession0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    
    // tests for RMmCustomAPI::CancelUssdSession should be written when ECustomCancelUssdSessionIPC 
    // would be proccessed.

	customAPI.CancelUssdSession( requestStatus);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSRCN2-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SatRefreshCompleteNotification
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SatRefreshCompleteNotification
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSatRefreshCompleteNotification0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::SatRefreshCompleteNotification
	// from LTSY.
	// The test comes first because the request is self-reposting
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomSatRefreshCompleteNotificationIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::SatRefreshCompleteNotification when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.CompleteL(ECustomSatRefreshCompleteNotificationIPC, KErrNone);
	
	customAPI.SatRefreshCompleteNotification(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSRCN2-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::SatRefreshCompleteNotification
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::SatRefreshCompleteNotification
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSatRefreshCompleteNotification0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::SatRefreshCompleteNotification
 	//-------------------------------------------------------------------------

	customAPI.SatRefreshCompleteNotification(requestStatus);

    iMockLTSY.CompleteL(ECustomSatRefreshCompleteNotificationIPC, KErrNone, 20);
		
	customAPI.CancelAsyncRequest(ECustomSatRefreshCompleteNotificationIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(2); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSRCN2-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::SatRefreshCompleteNotification
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::SatRefreshCompleteNotification
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSatRefreshCompleteNotification0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifyNetworkConnectionFailure
 	//-------------------------------------------------------------------------

    iMockLTSY.CompleteL(ECustomSatRefreshCompleteNotificationIPC, KErrNone, 20);

	customAPI.SatRefreshCompleteNotification(requestStatus);
	customAPI2.SatRefreshCompleteNotification(requestStatus2);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);
	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCCTDDS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::CheckTwoDigitDialSupport
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckTwoDigitDialSupport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckTwoDigitDialSupport0001L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    
    RMmCustomAPI::TTwoDigitDialSupport  support;
   
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckTwoDigitDialSupportIPC, KErrNotSupported);
    
    customAPI.CheckTwoDigitDialSupport(requestStatus, support);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckTwoDigitDialSupportIPC);
    
    const RMmCustomAPI::TTwoDigitDialSupport compData(RMmCustomAPI::ETwoDigitDialSupportOn);
    TPckg<RMmCustomAPI::TTwoDigitDialSupport> compPckg(compData);
    
    iMockLTSY.CompleteL( ECustomCheckTwoDigitDialSupportIPC, KErrGeneral, compPckg);

    customAPI.CheckTwoDigitDialSupport(requestStatus, support);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::CheckTwoDigitDialSupport when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckTwoDigitDialSupportIPC);
    iMockLTSY.CompleteL( ECustomCheckTwoDigitDialSupportIPC, KErrNone, compPckg);

    customAPI.CheckTwoDigitDialSupport(requestStatus, support);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(compData, support);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::CheckTwoDigitDialSupport
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomCheckTwoDigitDialSupportIPC, KErrNone, compPckg);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCTDDS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::CheckTwoDigitDialSupport
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::CheckTwoDigitDialSupport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckTwoDigitDialSupport0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TTwoDigitDialSupport  support;
   
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);


 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::CheckTwoDigitDialSupport
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckTwoDigitDialSupportIPC);
    
    const RMmCustomAPI::TTwoDigitDialSupport compData(RMmCustomAPI::ETwoDigitDialSupportOn);
    TPckg<RMmCustomAPI::TTwoDigitDialSupport> compPckg(compData);
    
    iMockLTSY.CompleteL( ECustomCheckTwoDigitDialSupportIPC, KErrNone, compPckg, 20);

    customAPI.CheckTwoDigitDialSupport(requestStatus, support);
	
	customAPI.CancelAsyncRequest(ECustomCheckTwoDigitDialSupportIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); 			
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCTDDS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::CheckTwoDigitDialSupport
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::CheckTwoDigitDialSupport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckTwoDigitDialSupport0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    RMmCustomAPI::TTwoDigitDialSupport  support;
    RMmCustomAPI::TTwoDigitDialSupport  support2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::CheckTwoDigitDialSupport
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomCheckTwoDigitDialSupportIPC);

    const RMmCustomAPI::TTwoDigitDialSupport compData(RMmCustomAPI::ETwoDigitDialSupportOn);
    TPckg<RMmCustomAPI::TTwoDigitDialSupport> compPckg(compData);

    iMockLTSY.CompleteL( ECustomCheckTwoDigitDialSupportIPC, KErrNone, compPckg, 20);

    customAPI.CheckTwoDigitDialSupport(requestStatus, support);

	// second client
    iMockLTSY.ExpectL( ECustomCheckTwoDigitDialSupportIPC);

    const RMmCustomAPI::TTwoDigitDialSupport compData2(RMmCustomAPI::ETwoDigitDialSupportOff);
    TPckg<RMmCustomAPI::TTwoDigitDialSupport> compPckg2(compData2);

    iMockLTSY.CompleteL( ECustomCheckTwoDigitDialSupportIPC, KErrNone, compPckg2);

    customAPI2.CheckTwoDigitDialSupport(requestStatus2, support2);		
        
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(compData, support);

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(compData2, support2);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCCTDDS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::CheckTwoDigitDialSupport with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::CheckTwoDigitDialSupport and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestCheckTwoDigitDialSupport0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TLifeTimeData ltData;
    TPckg<RMmCustomAPI::TLifeTimeData> ltPckg(ltData);

    iMockLTSY.ExpectL( ECustomCheckTwoDigitDialSupportIPC);
    
    RMmCustomAPI::TTwoDigitDialSupport  support;

    customAPI.CheckTwoDigitDialSupport(requestStatus, support);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); 			

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRNS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ResetNetServer
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ResetNetServer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestResetNetServer0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomResetNetServerIPC, KErrNotSupported);
    
    customAPI.ResetNetServer(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomResetNetServerIPC);

    iMockLTSY.CompleteL( ECustomResetNetServerIPC, KErrGeneral);

    customAPI.ResetNetServer(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::ResetNetServer when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomResetNetServerIPC);
    iMockLTSY.CompleteL( ECustomResetNetServerIPC, KErrNone);

    customAPI.ResetNetServer(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::ResetNetServer
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomResetNetServerIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRNS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::ResetNetServer
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::ResetNetServer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestResetNetServer0002L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::ResetNetServer
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomResetNetServerIPC);
    iMockLTSY.CompleteL( ECustomResetNetServerIPC, KErrNone, 20);

    customAPI.ResetNetServer(requestStatus);
	
	customAPI.CancelAsyncRequest(ECustomResetNetServerIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 	
	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCRNS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::ResetNetServer
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::ResetNetServer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestResetNetServer0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::ResetNetServer
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomResetNetServerIPC, KErrNone);
    iMockLTSY.CompleteL( ECustomResetNetServerIPC, KErrNone, 20);
    customAPI.ResetNetServer(requestStatus);

    iMockLTSY.ExpectL( ECustomResetNetServerIPC, KErrNone);
    iMockLTSY.CompleteL( ECustomResetNetServerIPC, KErrNone);
    customAPI2.ResetNetServer(requestStatus2);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRNS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ResetNetServer with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ResetNetServer and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestResetNetServer0005L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

    iMockLTSY.ExpectL( ECustomResetNetServerIPC);

    customAPI.ResetNetServer(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 	

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReleaseFile
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReleaseFile
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReleaseFile0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	_LIT(KFile, "FileName");
    TRequestStatus requestStatus;

	customAPI.ReleaseFile( requestStatus, KFile);

    // tests for RMmCustomAPI::ReleaseFile should be written when ECustomReleaseFileIPC 
    // would be proccessed.

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCREF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::RestartFile
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::RestartFile
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestRestartFile0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	_LIT(KFile, "FileName");
    TRequestStatus requestStatus;
    
    // tests for RMmCustomAPI::RestartFile should be written when ECustomRestartFileIPC 
    // would be proccessed.

	customAPI.RestartFile( requestStatus, KFile);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 
	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSCTB-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::StartSimCbTopicBrowsing
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::StartSimCbTopicBrowsing
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestStartSimCbTopicBrowsing0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
    
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	RBuf8 compData;
	CleanupClosePushL(compData);

	CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* simTopicArray =  new ( ELeave ) CArrayFixFlat
                            <RMmCustomAPI::TSimCbTopic>(1);	
	CleanupStack::PushL(simTopicArray);
	
	_LIT(KTopic, "Topic");
	RMmCustomAPI::TSimCbTopic topic;
	topic.iNumber = 0;
	topic.iName.Copy(KTopic);

	simTopicArray->AppendL( topic );
	
	TMockLtsyData1< CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* > ltsyData(simTopicArray);	
    ltsyData.SerialiseL(compData); 

	iMockLTSY.ExpectL( ECustomStartSimCbTopicBrowsingIPC);    
	iMockLTSY.CompleteL( ECustomStartSimCbTopicBrowsingIPC, KErrGeneral, compData);    
	
	TInt ret = customAPI.StartSimCbTopicBrowsing();
	
	ASSERT_EQUALS(KErrGeneral, ret);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::StartSimCbTopicBrowsing when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomStartSimCbTopicBrowsingIPC);
    iMockLTSY.CompleteL( ECustomStartSimCbTopicBrowsingIPC, KErrNone, compData);

    ret = customAPI.StartSimCbTopicBrowsing();
	
	ASSERT_EQUALS(KErrNone, ret);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::StartSimCbTopicBrowsing
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomStartSimCbTopicBrowsingIPC, KErrNone, compData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomStartSimCbTopicBrowsingIPC, KErrNotSupported);
    
    ret = customAPI.StartSimCbTopicBrowsing();
	
	ASSERT_EQUALS(KErrNotSupported, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 			
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSCTB-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::StartSimCbTopicBrowsing with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::StartSimCbTopicBrowsing with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestStartSimCbTopicBrowsing0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

	// Passing zero pointer

    iMockLTSY.ExpectL( ECustomStartSimCbTopicBrowsingIPC);
    
	RBuf8 compData;
	CleanupClosePushL(compData);

	CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* simTopicArray = NULL;	
	
	TMockLtsyData1< CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* > ltsyData(simTopicArray);	
    ltsyData.SerialiseL(compData); 
    
    iMockLTSY.CompleteL( ECustomStartSimCbTopicBrowsingIPC, KErrNone, compData);

    TInt ret = customAPI.StartSimCbTopicBrowsing();
	
	ASSERT_EQUALS(KErrNone, ret);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); 				

	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSCTB-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::StartSimCbTopicBrowsing with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::StartSimCbTopicBrowsing and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestStartSimCbTopicBrowsing0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

    iMockLTSY.ExpectL( ECustomStartSimCbTopicBrowsingIPC);
    
    TInt ret = customAPI.StartSimCbTopicBrowsing();
	
	ASSERT_EQUALS(KErrTimedOut, ret);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); 				

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGNSCT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetNextSimCbTopic
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetNextSimCbTopic
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetNextSimCbTopic0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
   
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetNextSimCbTopic when result is not cached.
 	//-------------------------------------------------------------------------
 	// testing with not initialized topics

	RMmCustomAPI::TSimCbTopic topic;

    customAPI.GetNextSimCbTopic(requestStatus, topic);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotFound, requestStatus.Int());

	AssertMockLtsyStatusL();

	_LIT(KTopic, "Topic");
	TInt num = 0;
 	
 	InitSimCbTopicsL(customAPI, KTopic, num);

 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMmCustomAPI::GetNextSimCbTopic when result is not cached.
 	//-------------------------------------------------------------------------

    customAPI.GetNextSimCbTopic(requestStatus, topic);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(num == topic.iNumber);
	ASSERT_TRUE(0 == topic.iName.Compare(KTopic));

	CleanupStack::PopAndDestroy(2, this); 	
	
	}



void CCTsyCustomIPCFU::InitSimCbTopicsL(RMmCustomAPI &aCustomApi, const TDesC& aTopic, TInt aNum)
	{

	RBuf8 compData;
	CleanupClosePushL(compData);

	CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* simTopicArray =  new ( ELeave ) CArrayFixFlat
                            <RMmCustomAPI::TSimCbTopic>(1);	
	CleanupStack::PushL(simTopicArray);
	
	RMmCustomAPI::TSimCbTopic topic;
	topic.iNumber = aNum;
	topic.iName.Copy(aTopic);

	simTopicArray->AppendL( topic );
	
	TMockLtsyData1< CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* > ltsyData(simTopicArray);	
    ltsyData.SerialiseL(compData); 

	iMockLTSY.ExpectL( ECustomStartSimCbTopicBrowsingIPC);    
	iMockLTSY.CompleteL( ECustomStartSimCbTopicBrowsingIPC, KErrNone, compData);    
	TInt ret = aCustomApi.StartSimCbTopicBrowsing();
	ASSERT_EQUALS(KErrNone, ret);

	CleanupStack::PopAndDestroy(2);	//simTopicArray, compData
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-MCDSCT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::DeleteSimCbTopic
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DeleteSimCbTopic
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDeleteSimCbTopic0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);
 	
 	InitSimCbTopicsL(customAPI, _L("Topic"), 0);
	
	TUint index(0);
	TUint expIndex(index);
	TBool deleteFlag(ETrue);

	TMockLtsyData2< TUint, TBool > ltsyData2(expIndex, deleteFlag);	
	ltsyData2.SerialiseL(expectData);

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomDeleteSimCbTopicIPC, expectData, KErrNotSupported);

    customAPI.DeleteSimCbTopic(requestStatus, index);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomDeleteSimCbTopicIPC, expectData);    
    iMockLTSY.CompleteL( ECustomDeleteSimCbTopicIPC, KErrGeneral);

    customAPI.DeleteSimCbTopic(requestStatus, index);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::DeleteSimCbTopic
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomDeleteSimCbTopicIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();	
	

 	//-------------------------------------------------------------------------
	// TEST C: deletion of not existing entry.
 	//-------------------------------------------------------------------------
 	
 	index = 100;
 	expIndex  = index;
	deleteFlag = EFalse;
	expectData.Close();
	ltsyData2.SerialiseL(expectData);

    iMockLTSY.ExpectL( ECustomDeleteSimCbTopicIPC, expectData);
    iMockLTSY.CompleteL( ECustomDeleteSimCbTopicIPC, KErrArgument);

    customAPI.DeleteSimCbTopic(requestStatus, index);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMmCustomAPI::DeleteSimCbTopic when result is not cached.
 	//-------------------------------------------------------------------------

 	index = 0;
 	expIndex  = index;
	deleteFlag = ETrue;
	expectData.Close();
	ltsyData2.SerialiseL(expectData);

    iMockLTSY.ExpectL( ECustomDeleteSimCbTopicIPC, expectData);
    iMockLTSY.CompleteL( ECustomDeleteSimCbTopicIPC, KErrNone);

    customAPI.DeleteSimCbTopic(requestStatus, index);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDSCT-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::DeleteSimCbTopic
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::DeleteSimCbTopic
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDeleteSimCbTopic0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

 	InitSimCbTopicsL(customAPI, _L("Topic"), 0);
   
    TRequestStatus requestStatus;
   
	TUint index(0);
	TUint expIndex(index);
	TBool deleteFlag(ETrue);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData2< TUint, TBool > ltsyData2(expIndex, deleteFlag);	
	ltsyData2.SerialiseL(expectData);
    		
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::DeleteSimCbTopic
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomDeleteSimCbTopicIPC, expectData);    
    iMockLTSY.CompleteL( ECustomDeleteSimCbTopicIPC, KErrNone, 20);

    customAPI.DeleteSimCbTopic(requestStatus, index);
	
	customAPI.CancelAsyncRequest(ECustomDeleteSimCbTopicIPC);

	// operation (ECustomDeleteSimCbTopicIPC) cannot be cancelled on DOS side as per CTSY.

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); 	
	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDSCT-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::DeleteSimCbTopic with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DeleteSimCbTopic with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDeleteSimCbTopic0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TUint index(0);
	TUint expIndex(index);
	TBool deleteFlag(EFalse);

	TMockLtsyData2< TUint, TBool > ltsyData2(expIndex, deleteFlag);	
	ltsyData2.SerialiseL(expectData);
    	
    iMockLTSY.ExpectL( ECustomDeleteSimCbTopicIPC, expectData);
    iMockLTSY.CompleteL( ECustomDeleteSimCbTopicIPC, KErrNone);

	// Successful completion of ECustomDeleteSimCbTopicIPC request in case of not initialized iSimCbTopics
    customAPI.DeleteSimCbTopic(requestStatus, index);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); 		

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDSCT-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::DeleteSimCbTopic
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::DeleteSimCbTopic
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDeleteSimCbTopic0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

 	InitSimCbTopicsL(customAPI, _L("Topic"), 0);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

	TUint index(0);
	TUint index2(0);
	TUint expIndex(index);
	TBool deleteFlag(ETrue);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData2< TUint, TBool > ltsyData2(expIndex, deleteFlag);	
	ltsyData2.SerialiseL(expectData);
    	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::DeleteSimCbTopic
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomDeleteSimCbTopicIPC, expectData);    
    iMockLTSY.CompleteL( ECustomDeleteSimCbTopicIPC, KErrNone, 20);

    customAPI.DeleteSimCbTopic(requestStatus, index);
    customAPI2.DeleteSimCbTopic(requestStatus2, index2);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this); // telServer2, phone2, customAPI, customAPI2, expectData, this

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDSCT-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::DeleteSimCbTopic with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DeleteSimCbTopic and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDeleteSimCbTopic0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

 	InitSimCbTopicsL(customAPI, _L("Topic"), 0);
   
    TRequestStatus requestStatus;
   
	TUint index(0);
	TUint expIndex(index);
	TBool deleteFlag(ETrue);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData2< TUint, TBool > ltsyData2(expIndex, deleteFlag);	
	ltsyData2.SerialiseL(expectData);
    		
    iMockLTSY.ExpectL( ECustomDeleteSimCbTopicIPC, expectData);    

    customAPI.DeleteSimCbTopic(requestStatus, index);
		
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); 		

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNNCF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyNetworkConnectionFailure
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyNetworkConnectionFailure
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyNetworkConnectionFailure0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyNetworkConnectionFailure
	// from LTSY.
	// The test comes first because the request is self-reposting
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomNotifyNetworkConnectionFailureIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyNetworkConnectionFailure when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.CompleteL(ECustomNotifyNetworkConnectionFailureIPC, KErrNone);

	customAPI.NotifyNetworkConnectionFailure(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNNCF-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifyNetworkConnectionFailure
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifyNetworkConnectionFailure
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyNetworkConnectionFailure0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifyNetworkConnectionFailure
 	//-------------------------------------------------------------------------

	customAPI.NotifyNetworkConnectionFailure(requestStatus);

    iMockLTSY.CompleteL(ECustomNotifyNetworkConnectionFailureIPC, KErrNone, 20);
		
	customAPI.CancelAsyncRequest(ECustomNotifyNetworkConnectionFailureIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(2); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNNCF-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifyNetworkConnectionFailure
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifyNetworkConnectionFailure
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyNetworkConnectionFailure0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifyNetworkConnectionFailure
 	//-------------------------------------------------------------------------

    iMockLTSY.CompleteL(ECustomNotifyNetworkConnectionFailureIPC, KErrNone, 20);

	customAPI.NotifyNetworkConnectionFailure(requestStatus);
	customAPI2.NotifyNetworkConnectionFailure(requestStatus2);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);
	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SendAPDUReq
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SendAPDUReq
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSendAPDUReq0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI   customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	
	TInt           cardReaderId = 1; 
	_LIT8(KCmdData, "1234");
	CmdDataBuf       cmdData(KCmdData);	

    TRequestStatus requestStatus;
	RMmCustomAPI::TApduParameters apdu;
	
	apdu.iCardReaderId = cardReaderId;
	apdu.iCmdData.Copy(cmdData);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 compData;
	CleanupClosePushL(compData);
	
	TMockLtsyData1< RMmCustomAPI::TApduParameters > ltsyData(apdu);	
	ltsyData.SerialiseL(expectData);
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSendAPDUReqV2IPC, expectData, KErrNotSupported);
    customAPI.SendAPDUReq(requestStatus, apdu);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSendAPDUReqV2IPC, expectData);
    
    _LIT8(KRspData,"ApduResponse");
    RspDataBuf rspData(KRspData);    

    TMockLtsyData1< RspDataBuf > ltsyRetData(rspData);
    ltsyRetData.SerialiseL(compData);       
    
    iMockLTSY.CompleteL( ECustomSendAPDUReqV2IPC, KErrGeneral, compData);

    customAPI.SendAPDUReq (requestStatus, apdu);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::SendAPDUReq when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSendAPDUReqV2IPC, expectData);
    iMockLTSY.CompleteL( ECustomSendAPDUReqV2IPC, KErrNone, compData);

    customAPI.SendAPDUReq(requestStatus, apdu);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(0, (apdu.iCardReaderId != cardReaderId)?1:0);
	ASSERT_EQUALS(0, apdu.iCmdData.Compare(cmdData));
	ASSERT_EQUALS(0, apdu.iRspData.Compare(rspData));

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::SendAPDUReq
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomSendAPDUReqV2IPC, KErrNone, compData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::SendAPDUReq
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::SendAPDUReq
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSendAPDUReq0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TInt           cardReaderId = 1; 
    _LIT8(KCmdData, "1234");
    CmdDataBuf      cmdData(KCmdData);   

    TRequestStatus requestStatus;
    RMmCustomAPI::TApduParameters apdu;
    
    apdu.iCardReaderId = cardReaderId;
    apdu.iCmdData.Copy(cmdData);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 compData;
	CleanupClosePushL(compData);
	
	TMockLtsyData1< RMmCustomAPI::TApduParameters > ltsyData(apdu);	
	ltsyData.SerialiseL(expectData);

	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::SendAPDUReq
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSendAPDUReqV2IPC, expectData);

    customAPI.SendAPDUReq(requestStatus, apdu);
	
	customAPI.CancelAsyncRequest(ECustomSendAPDUReqV2IPC);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	
    _LIT8(KRspData,"ApduResponse");
    RspDataBuf      rspData(KRspData);    

    TMockLtsyData1< RspDataBuf > ltsyRetData(rspData);
    ltsyRetData.SerialiseL(compData);  
    
    iMockLTSY.CompleteL( ECustomSendAPDUReqV2IPC, KErrNone, compData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAR-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SendAPDUReq with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SendAPDUReq with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSendAPDUReq0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TInt           cardReaderId = 1; 
    _LIT8(KCmdData, "1234");
    CmdDataBuf      cmdData(KCmdData);   

    TRequestStatus requestStatus;
    RMmCustomAPI::TApduParameters apdu;
    
    apdu.iCardReaderId = cardReaderId;
    apdu.iCmdData.Copy(cmdData);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 compData;
	CleanupClosePushL(compData);
	
	TMockLtsyData1< RMmCustomAPI::TApduParameters > ltsyData(apdu);	
	ltsyData.SerialiseL(expectData);
    	
    iMockLTSY.ExpectL( ECustomSendAPDUReqV2IPC, expectData);
    customAPI.SendAPDUReq(requestStatus, apdu);    

    _LIT8(KRspBig,"bigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbigbi");
    RspBigBuf bigBuf(KRspBig);

	TMockLtsyData1< TBuf8<300> > ltsyDataComp(bigBuf);
    ltsyDataComp.SerialiseL(compData);       
   
    iMockLTSY.CompleteL( ECustomSendAPDUReqV2IPC, KErrNone, compData);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrOverflow, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::SendAPDUReq
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::SendAPDUReq
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSendAPDUReq0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);


    TInt           cardReaderId = 1; 
    _LIT8(KCmdData, "1234");
    CmdDataBuf      cmdData(KCmdData);   

    TRequestStatus requestStatus;
    RMmCustomAPI::TApduParameters apdu;
    
    apdu.iCardReaderId = cardReaderId;
    apdu.iCmdData.Copy(cmdData);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RBuf8 compData;
	CleanupClosePushL(compData);
	
	TMockLtsyData1< RMmCustomAPI::TApduParameters > ltsyData(apdu);	
	ltsyData.SerialiseL(expectData);
    	
	
    iMockLTSY.ExpectL( ECustomSendAPDUReqV2IPC, expectData);

    
    _LIT8(KRspData,"ApduResponse");
    RspDataBuf     rspData(KRspData);    

    TMockLtsyData1< TBuf8<20> > ltsyRetData(rspData);
    ltsyRetData.SerialiseL(compData);        
    
    iMockLTSY.CompleteL(ECustomSendAPDUReqV2IPC, KErrNone, compData, 20);

	//sending request to the first client
    customAPI.SendAPDUReq(requestStatus, apdu);
  
    TRequestStatus requestStatus2;
    RMmCustomAPI::TApduParameters apdu2; // empty request as api cannot be called twice 
 

	//sending request to the second client
    customAPI2.SendAPDUReq(requestStatus2, apdu2);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(7, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAR-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SendAPDUReq with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SendAPDUReq and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSendAPDUReq0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    _LIT8(KCmdData, "1234");
    CmdDataBuf      cmdData(KCmdData);   

    TRequestStatus requestStatus;
    RMmCustomAPI::TApduParameters apdu;
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< RMmCustomAPI::TApduParameters > ltsyData(apdu);	
	ltsyData.SerialiseL(expectData);
	
    iMockLTSY.ExpectL(ECustomSendAPDUReqV2IPC, expectData);

    customAPI.SendAPDUReq(requestStatus, apdu);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDPL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::DisablePhoneLock
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DisablePhoneLock
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDisablePhoneLock0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	_LIT(KPassword, "password");

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePassword pswd(KPassword);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< RMobilePhone::TMobilePassword > ltsyData(pswd);	
	ltsyData.SerialiseL(expectData);
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomDisablePhoneLockIPC, expectData, KErrNotSupported);
    
    customAPI.DisablePhoneLock(requestStatus, pswd);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomDisablePhoneLockIPC, expectData);    
    iMockLTSY.CompleteL( ECustomDisablePhoneLockIPC, KErrGeneral);

    customAPI.DisablePhoneLock(requestStatus, pswd);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::DisablePhoneLock when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomDisablePhoneLockIPC, expectData);
    iMockLTSY.CompleteL( ECustomDisablePhoneLockIPC, KErrNone);

    customAPI.DisablePhoneLock(requestStatus, pswd);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::DisablePhoneLock
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomDisablePhoneLockIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDPL-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::DisablePhoneLock
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::DisablePhoneLock
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDisablePhoneLock0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	_LIT(KPassword, "password");

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePassword pswd(KPassword);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< RMobilePhone::TMobilePassword > ltsyData(pswd);	
	ltsyData.SerialiseL(expectData);
    		
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::DisablePhoneLock
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomDisablePhoneLockIPC, expectData);    
    iMockLTSY.CompleteL( ECustomDisablePhoneLockIPC, KErrNone, 20);

    customAPI.DisablePhoneLock(requestStatus, pswd);
	
	customAPI.CancelAsyncRequest(ECustomDisablePhoneLockIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); 	
	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCDPL-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::DisablePhoneLock
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::DisablePhoneLock
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDisablePhoneLock0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

	_LIT(KPassword, "password");

    RMobilePhone::TMobilePassword pswd(KPassword);
    RMobilePhone::TMobilePassword pswd2(KPassword);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< RMobilePhone::TMobilePassword > ltsyData(pswd);	
	ltsyData.SerialiseL(expectData);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::DisablePhoneLock
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomDisablePhoneLockIPC, expectData);    
    iMockLTSY.CompleteL( ECustomDisablePhoneLockIPC, KErrNone, 20);

    customAPI.DisablePhoneLock(requestStatus, pswd);
    customAPI2.DisablePhoneLock(requestStatus2, pswd2);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCDPL-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::DisablePhoneLock with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DisablePhoneLock and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestDisablePhoneLock0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	_LIT(KPassword, "password");

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePassword pswd(KPassword);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< RMobilePhone::TMobilePassword > ltsyData(pswd);	
	ltsyData.SerialiseL(expectData);
	
    iMockLTSY.ExpectL( ECustomDisablePhoneLockIPC, expectData);

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 404602);
	//Request ECustomDisablePhoneLockIPC is not completed by timeout. 		
	
	//The test is stopped to avoid blocking of next tests.
	ASSERT_TRUE(EFalse);
    
    customAPI.DisablePhoneLock(requestStatus, pswd);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); 	

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNEIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyEGprsInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyEGprsInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyEGprsInfoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RPacketService packetService;		

	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RBuf8 data;
	CleanupClosePushL(data);

    TRequestStatus requestStatus;
    RMmCustomAPI::TGprsInformation info;
    TPckg<RMmCustomAPI::TGprsInformation> infoPckg(info);    

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyEGprsInfoChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	TBool lastEdgeGprsSupport = ETrue;	
	TMockLtsyData1 <TBool> ltsyData(lastEdgeGprsSupport);
	ltsyData.SerialiseL(data);	
		
 	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
	iMockLTSY.CompleteL(EMmTsyNotifyEGprsInfoChangeIPC, KErrNone, data);	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());			
	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyEGprsInfoChange when result is not cached.
 	//-------------------------------------------------------------------------

    customAPI.NotifyEGprsInfoChange(requestStatus, infoPckg);	
	
	iMockLTSY.CompleteL(EMmTsyNotifyEGprsInfoChangeIPC, KErrNone, data);	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_TRUE( RMmCustomAPI::EEdgeGprs == info.iGprsInfo);

	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMmCustomAPI::NotifyEGprsInfoChange
 	//-------------------------------------------------------------------------

	lastEdgeGprsSupport = EFalse;	

	data.Close();
	ltsyData.SerialiseL(data);	

    customAPI.NotifyEGprsInfoChange(requestStatus, infoPckg);	
	
	iMockLTSY.CompleteL(EMmTsyNotifyEGprsInfoChangeIPC, KErrNone, data);	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_TRUE( RMmCustomAPI::EGprs == info.iGprsInfo);

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(4, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNEIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifyEGprsInfoChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifyEGprsInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyEGprsInfoChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RPacketService packetService;		

	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RBuf8 data;
	CleanupClosePushL(data);

    TRequestStatus requestStatus;
    RMmCustomAPI::TGprsInformation info;
    TPckg<RMmCustomAPI::TGprsInformation> infoPckg(info);    

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifyEGprsInfoChange
 	//-------------------------------------------------------------------------
 	
    customAPI.NotifyEGprsInfoChange(requestStatus, infoPckg);	
    
    customAPI.CancelAsyncRequest(ECustomNotifyEGprsInfoChange);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	TBool lastEdgeGprsSupport = ETrue;	
	TMockLtsyData1 <TBool> ltsyData(lastEdgeGprsSupport);
	ltsyData.SerialiseL(data);	

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
		
	iMockLTSY.CompleteL(EMmTsyNotifyEGprsInfoChangeIPC, KErrNone, data);	
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());			

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(4); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNEIC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyEGprsInfoChange with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyEGprsInfoChange with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyEGprsInfoChange0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RPacketService packetService;		

	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RBuf8 data;
	CleanupClosePushL(data);

    TRequestStatus requestStatus;
    TBuf8<1> littleBuf;

 	//-------------------------------------------------------------------------
	// Test on passing too little buffer to NotifyEGprsInfoChange
 	//-------------------------------------------------------------------------
 	
    customAPI.NotifyEGprsInfoChange(requestStatus, littleBuf);	
    
	TBool lastEdgeGprsSupport = ETrue;	
	TMockLtsyData1 <TBool> ltsyData(lastEdgeGprsSupport);
	ltsyData.SerialiseL(data);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(4); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNEIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifyEGprsInfoChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifyEGprsInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyEGprsInfoChange0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	RPacketService packetService;		

	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	RBuf8 data;
	CleanupClosePushL(data);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
	
    RMmCustomAPI::TGprsInformation info;
    TPckg<RMmCustomAPI::TGprsInformation> infoPckg(info);    

    RMmCustomAPI::TGprsInformation info2;
    TPckg<RMmCustomAPI::TGprsInformation> infoPckg2(info2);    

 	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifyEGprsInfoChange
 	//-------------------------------------------------------------------------
    customAPI.NotifyEGprsInfoChange(requestStatus, infoPckg);	
    customAPI2.NotifyEGprsInfoChange(requestStatus2, infoPckg2);	

	TBool lastEdgeGprsSupport = ETrue;	
	TMockLtsyData1 <TBool> ltsyData(lastEdgeGprsSupport);
	ltsyData.SerialiseL(data);	
		
	iMockLTSY.CompleteL(EMmTsyNotifyEGprsInfoChangeIPC, KErrNone, data, 20);	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_TRUE( RMmCustomAPI::EEdgeGprs == info.iGprsInfo);

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

    ASSERT_TRUE( RMmCustomAPI::EEdgeGprs == info2.iGprsInfo);

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(8, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGEI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetEGprsInfo
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetEGprsInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetEGprsInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RPacketService packetService;		

	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

    TRequestStatus requestStatus;
    RMmCustomAPI::TGprsInformation info;
    TPckg<RMmCustomAPI::TGprsInformation> infoPckg(info);
    

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetEGprsInfo when result is not cached.
 	//-------------------------------------------------------------------------

    customAPI.GetEGprsInfo(requestStatus, infoPckg);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_TRUE( RMmCustomAPI::EGprs == info.iGprsInfo);

	AssertMockLtsyStatusL();	

	// now set EdgeGprs
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	TBool lastEdgeGprsSupport = ETrue;	
	TMockLtsyData1 <TBool> ltsyData(lastEdgeGprsSupport);
	ltsyData.SerialiseL(data);	
		
 	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
	iMockLTSY.CompleteL(EMmTsyNotifyEGprsInfoChangeIPC, KErrNone, data);	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());			
	AssertMockLtsyStatusL();	


 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMmCustomAPI::GetEGprsInfo when EdgeGprs
 	//-------------------------------------------------------------------------

    customAPI.GetEGprsInfo(requestStatus, infoPckg);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_TRUE( RMmCustomAPI::EEdgeGprs == info.iGprsInfo);

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(4, this); 	
		
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGEI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetEGprsInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetEGprsInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetEGprsInfo0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TGprsInformation info;
    TPckg<RMmCustomAPI::TGprsInformation> infoPckg(info);
    
	// Calling GetEGprsInfo without opening RPacketService

    customAPI.GetEGprsInfo(requestStatus, infoPckg);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();	
	
	
	// test on passing too little buffer

	RPacketService packetService;		

	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
		
	TBuf8<1> littleBuf;

    customAPI.GetEGprsInfo(requestStatus, littleBuf);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	CleanupStack::PopAndDestroy(3, this); 			

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRSF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadSimFile
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadSimFile
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadSimFile0001L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
	RBuf8 expData;
	CleanupClosePushL(expData);

	TBuf8<32> responce;
	RMmCustomAPI::TSimFileInfo fileInfo;
	fileInfo.iPath.Fill(0x00);
	fileInfo.iOffSet = 0;
	fileInfo.iSize = responce.Size();
	RMmCustomAPI::TSimFileInfo expFileInfo(fileInfo);
	TPckg<RMmCustomAPI::TSimFileInfo> fileInfoPckg(fileInfo);	

	TMockLtsyData1< RMmCustomAPI::TSimFileInfo > ltsyData(fileInfo);	    
    ltsyData.SerialiseL(expData);       
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomReadSimFileIPC, expData, KErrNotSupported);
   
    customAPI.ReadSimFile(requestStatus, fileInfoPckg, responce);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomReadSimFileIPC, expData);

	_LIT8(KResponce, "Responce");

    iMockLTSY.CompleteL( ECustomReadSimFileIPC, KErrGeneral, KResponce);

    customAPI.ReadSimFile(requestStatus, fileInfoPckg, responce);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::ReadSimFile when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomReadSimFileIPC, expData);
    iMockLTSY.CompleteL( ECustomReadSimFileIPC, KErrNone, KResponce);

    customAPI.ReadSimFile(requestStatus, fileInfoPckg, responce);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(0, responce.Compare(KResponce));

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::ReadSimFile
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomReadSimFileIPC, KErrNone, KResponce);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRSF-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::ReadSimFile
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::ReadSimFile
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadSimFile0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TRequestStatus requestStatus;
	
	RBuf8 expData;
	CleanupClosePushL(expData);

	TBuf8<32> responce;
	RMmCustomAPI::TSimFileInfo fileInfo;
	fileInfo.iPath.Fill(0x00);
	fileInfo.iOffSet = 0;
	fileInfo.iSize = responce.Size();
	RMmCustomAPI::TSimFileInfo expFileInfo(fileInfo);
	TPckg<RMmCustomAPI::TSimFileInfo> fileInfoPckg(fileInfo);	

	TMockLtsyData1< RMmCustomAPI::TSimFileInfo > ltsyData(fileInfo);	    
    ltsyData.SerialiseL(expData);       

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::ReadSimFile
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomReadSimFileIPC, expData);
    
	_LIT8(KResponce, "Responce");

    iMockLTSY.CompleteL( ECustomReadSimFileIPC, KErrNone, KResponce, 20);
    
    customAPI.ReadSimFile(requestStatus, fileInfoPckg, responce);
    
    customAPI.CancelAsyncRequest(ECustomReadSimFileIPC);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
 	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(3);	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRSF-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadSimFile with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadSimFile with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadSimFile0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
	RBuf8 expData;
	CleanupClosePushL(expData);

	RMmCustomAPI::TSimFileInfo fileInfo;
	fileInfo.iPath.Fill(0x00);
	fileInfo.iOffSet = 0;
	fileInfo.iSize = 32;
	RMmCustomAPI::TSimFileInfo expFileInfo(fileInfo);
	TPckg<RMmCustomAPI::TSimFileInfo> fileInfoPckg(fileInfo);	
	TBuf8<1> littleBuf;

	TMockLtsyData1< RMmCustomAPI::TSimFileInfo > ltsyData(fileInfo);	    
    ltsyData.SerialiseL(expData);       

    iMockLTSY.ExpectL( ECustomReadSimFileIPC, expData);
    
	_LIT8(KResponce, "Responce");

    iMockLTSY.CompleteL( ECustomReadSimFileIPC, KErrNone, KResponce);
    
    customAPI.ReadSimFile(requestStatus, fileInfoPckg, littleBuf);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrOverflow, requestStatus.Int());

	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(3);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRSF-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::ReadSimFile
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::ReadSimFile
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadSimFile0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
	
	RBuf8 expData;
	CleanupClosePushL(expData);

	TBuf8<32> responce;
	RMmCustomAPI::TSimFileInfo fileInfo;
	fileInfo.iPath.Fill(0x00);
	fileInfo.iOffSet = 0;
	fileInfo.iSize = responce.Size();
	RMmCustomAPI::TSimFileInfo expFileInfo(fileInfo);
	TPckg<RMmCustomAPI::TSimFileInfo> fileInfoPckg(fileInfo);	

	TMockLtsyData1< RMmCustomAPI::TSimFileInfo > ltsyData(fileInfo);	    
    ltsyData.SerialiseL(expData);       

	RBuf8 expData2;
	CleanupClosePushL(expData2);
//	TMockLtsyData1< RMmCustomAPI::TSimFileInfo > ltsyData2(fileInfo3);
//	ltsyData2.SerialiseL(expData3);

	TBuf8<32> responce2;
	RMmCustomAPI::TSimFileInfo fileInfo2;
	fileInfo2.iPath.Fill(0x00);
	fileInfo2.iOffSet = 0;
	fileInfo2.iSize = responce2.Size();
	RMmCustomAPI::TSimFileInfo expFileInfo2(fileInfo2);
	TPckg<RMmCustomAPI::TSimFileInfo> fileInfoPckg2(fileInfo2);	


    RMmCustomAPI::TSimFileInfo fileInfo3;
    fileInfo3.iPath.Fill(0x00);
    fileInfo3.iOffSet = 0x10;
    fileInfo3.iSize = responce2.Size();
    RMmCustomAPI::TSimFileInfo expFileInfo3(fileInfo3);
    TPckg<RMmCustomAPI::TSimFileInfo> fileInfoPckg3(fileInfo3); 	

	//-------------------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::ReadSimFile - different data 
 	//-------------------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomReadSimFileIPC, expData);    
	_LIT8(KResponce, "Responce");
    iMockLTSY.CompleteL( ECustomReadSimFileIPC, KErrNone, KResponce, 20);
    
    customAPI.ReadSimFile(requestStatus, fileInfoPckg, responce);

    //iMockLTSY.ExpectL( ECustomReadSimFileIPC, expData2);    
    //iMockLTSY.CompleteL( ECustomReadSimFileIPC, KErrNone, KResponce);
    customAPI2.ReadSimFile(requestStatus2, fileInfoPckg2, responce2);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(0, responce.Compare(KResponce));
	    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(0, responce2.Compare(KResponce));

	AssertMockLtsyStatusL();
		
    //-----------------------------------------------------------------------------------
    // Test B: Test multiple clients requesting RMmCustomAPI::ReadSimFile with same data
    //-----------------------------------------------------------------------------------
	


    iMockLTSY.ExpectL( ECustomReadSimFileIPC, expData); 
    iMockLTSY.CompleteL( ECustomReadSimFileIPC, KErrNone, KResponce, 20);
    
    customAPI.ReadSimFile(requestStatus, fileInfoPckg, responce);
    customAPI2.ReadSimFile(requestStatus2, fileInfoPckg3, responce);
    
    User::WaitForRequest(requestStatus);    
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(0, responce.Compare(KResponce));
        
    User::WaitForRequest(requestStatus2);   
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

    AssertMockLtsyStatusL();	
	

	CleanupStack::PopAndDestroy(7, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRSF-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadSimFile with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadSimFile and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadSimFile0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
	RBuf8 expData;
	CleanupClosePushL(expData);

	TBuf8<32> responce;
	RMmCustomAPI::TSimFileInfo fileInfo;
	fileInfo.iPath.Fill(0x00);
	fileInfo.iOffSet = 0;
	fileInfo.iSize = responce.Size();
	RMmCustomAPI::TSimFileInfo expFileInfo(fileInfo);
	TPckg<RMmCustomAPI::TSimFileInfo> fileInfoPckg(fileInfo);	

	TMockLtsyData1< RMmCustomAPI::TSimFileInfo > ltsyData(fileInfo);	    
    ltsyData.SerialiseL(expData);       

    iMockLTSY.ExpectL( ECustomReadSimFileIPC, expData);

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 404902);
	// Location capability missing
    
    customAPI.ReadSimFile(requestStatus, fileInfoPckg, responce);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(3);


	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGLT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetLifeTime
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetLifeTime
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetLifeTime0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    
    RMmCustomAPI::TLifeTimeData ltData;
    TPckg<RMmCustomAPI::TLifeTimeData> ltPckg(ltData);
   
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetLifeTimeIPC, KErrNotSupported);
    
    customAPI.GetLifeTime(requestStatus, ltPckg);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetLifeTimeIPC);
    
    RMmCustomAPI::TLifeTimeData compData;
	compData.iCaps = 0x34;		// fill with random data
	compData.iHours = 123;
	compData.iMinutes = 21;
    compData.iManufacturingDate = TDateTime(1,(TMonth)2,3,4,5,6,7);
    TPckg<RMmCustomAPI::TLifeTimeData> compPckg(compData);
    
    iMockLTSY.CompleteL( ECustomGetLifeTimeIPC, KErrGeneral, compPckg);

    customAPI.GetLifeTime(requestStatus, ltPckg);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetLifeTime when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetLifeTimeIPC);
    iMockLTSY.CompleteL( ECustomGetLifeTimeIPC, KErrNone, compPckg);

    customAPI.GetLifeTime(requestStatus, ltPckg);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(compData.iCaps == ltData.iCaps);
	ASSERT_TRUE(compData.iHours == ltData.iHours);
	ASSERT_TRUE(compData.iMinutes == ltData.iMinutes);
	ASSERT_TRUE(compData.iManufacturingDate.Year() == ltData.iManufacturingDate.Year());
	ASSERT_TRUE(compData.iManufacturingDate.Month() == ltData.iManufacturingDate.Month());
	ASSERT_TRUE(compData.iManufacturingDate.Day() == ltData.iManufacturingDate.Day());
	ASSERT_TRUE(compData.iManufacturingDate.Hour() == ltData.iManufacturingDate.Hour());
	ASSERT_TRUE(compData.iManufacturingDate.Minute() == ltData.iManufacturingDate.Minute());
	ASSERT_TRUE(compData.iManufacturingDate.Second() == ltData.iManufacturingDate.Second());
	ASSERT_TRUE(compData.iManufacturingDate.MicroSecond() == ltData.iManufacturingDate.MicroSecond());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetLifeTime
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomGetLifeTimeIPC, KErrNone, compPckg);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGLT-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetLifeTime
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetLifeTime
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetLifeTime0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TLifeTimeData ltData;
    TPckg<RMmCustomAPI::TLifeTimeData> ltPckg(ltData);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);


 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetLifeTime
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetLifeTimeIPC);
    
    RMmCustomAPI::TLifeTimeData compData;
	compData.iCaps = 0xFF;
    TPckg<RMmCustomAPI::TLifeTimeData> compPckg(compData);
    
    iMockLTSY.CompleteL( ECustomGetLifeTimeIPC, KErrNone, compPckg, 20);

    customAPI.GetLifeTime(requestStatus, ltPckg);
	
	customAPI.CancelAsyncRequest(ECustomGetLifeTimeIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); 	
		
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGLT-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetLifeTime with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetLifeTime with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetLifeTime0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TBuf8<1> badPckg;

    customAPI.GetLifeTime(requestStatus, badPckg);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); 	

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGLT-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetLifeTime
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetLifeTime
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetLifeTime0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    RMmCustomAPI::TLifeTimeData ltData;
    TPckg<RMmCustomAPI::TLifeTimeData> ltPckg(ltData);

    RMmCustomAPI::TLifeTimeData ltData2;
    TPckg<RMmCustomAPI::TLifeTimeData> ltPckg2(ltData2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetLifeTime
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetLifeTimeIPC);

    RMmCustomAPI::TLifeTimeData compData;
	compData.iCaps = 0x11;		
	compData.iHours = 111;
	compData.iMinutes = 111;
    compData.iManufacturingDate = TDateTime(1,(TMonth)1,1,1,1,1,1);
    TPckg<RMmCustomAPI::TLifeTimeData> compPckg(compData);

    iMockLTSY.CompleteL( ECustomGetLifeTimeIPC, KErrNone, compPckg, 20);

    customAPI.GetLifeTime(requestStatus, ltPckg);


    customAPI2.GetLifeTime(requestStatus2, ltPckg2);		
        
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(compData.iCaps == ltData.iCaps);
	ASSERT_TRUE(compData.iHours == ltData.iHours);
	ASSERT_TRUE(compData.iMinutes == ltData.iMinutes);
	ASSERT_TRUE(compData.iManufacturingDate.Year() == ltData.iManufacturingDate.Year());
	ASSERT_TRUE(compData.iManufacturingDate.Month() == ltData.iManufacturingDate.Month());
	ASSERT_TRUE(compData.iManufacturingDate.Day() == ltData.iManufacturingDate.Day());
	ASSERT_TRUE(compData.iManufacturingDate.Hour() == ltData.iManufacturingDate.Hour());
	ASSERT_TRUE(compData.iManufacturingDate.Minute() == ltData.iManufacturingDate.Minute());
	ASSERT_TRUE(compData.iManufacturingDate.Second() == ltData.iManufacturingDate.Second());
	ASSERT_TRUE(compData.iManufacturingDate.MicroSecond() == ltData.iManufacturingDate.MicroSecond());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());


	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGB-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::Get3GPBInfo
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::Get3GPBInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGet3GPBInfo0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::T3GPBInfo info;
    
    Mem::FillZ((TAny*)&info, sizeof(info));

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::Get3GPBInfo when result is not cached.
	// Getting data for uninitialized PB
 	//-------------------------------------------------------------------------

    customAPI.Get3GPBInfo(requestStatus, info);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	
	// for a not initializes PB all values are -1.
    ASSERT_TRUE( -1 == info.iMaxLenEmail);
    ASSERT_TRUE( -1 == info.iMaxLenSne);
    ASSERT_TRUE( -1 == info.iMaxLenAnr);
    ASSERT_TRUE( -1 == info.iMaxLenGroupName);
    ASSERT_TRUE( -1 == info.iMaxNumEmail);
    ASSERT_TRUE( -1 == info.iMaxNumSne);
    ASSERT_TRUE( -1 == info.iMaxNumAnr);
    ASSERT_TRUE( -1 == info.iMaxNumGroupName);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RMmCustomAPI::Get3GPBInfo when phoneBookStore is initialized
 	//-------------------------------------------------------------------------

	TName name(KETelIccAdnPhoneBook);
	RMobilePhoneBookStore bookStore;
	
    OpenPhoneBookStoreL(bookStore, name, iPhone);
	CleanupClosePushL(bookStore);

	RBuf8 data;
	CleanupClosePushL(data);
    
    CStorageInfoData storageData;       
    
    storageData.iADNNumOfEntries = 1;
    storageData.iADNTextLengthMax = 2;
    storageData.iADNNumberLengthMax = 3;
    storageData.iSNENumOfEntries = 4;
    storageData.iSNENumOfEntriesPerEntry = 5;
    storageData.iSNETextLengthMax = 6;
    storageData.iEmailNumOfEntries = 7;
    storageData.iEmailNumOfEntriesPerEntry = 8;
    storageData.iEmailTextLengthMax = 9;
    storageData.iFDNNumOfEntries = 10;
    storageData.iFDNTextLengthMax = 11;
    storageData.iFDNNumberLengthMax = 12;
    storageData.iSDNNumOfEntries = 13;
    storageData.iSDNTextLengthMax = 14;
    storageData.iSDNNumberLengthMax = 15;
    storageData.iVMBXNumOfEntries = 16;
    storageData.iVMBXTextLengthMax = 17;
    storageData.iVMBXNumberLengthMax = 18;
    storageData.iMSISDNNumOfEntries = 19;
    storageData.iMSISDNTextLengthMax = 20;
    storageData.iMSISDNNumberLengthMax = 21;
    storageData.iGRPNumOfEntriesPerEntry = 22;
    storageData.iANRNumOfEntries = 23;
    storageData.iANRNumOfEntriesPerEntry = 24;
    storageData.iANRNumLengthMax = 25;
    storageData.iMBDNNumOfEntries = 26;
    storageData.iMBDNTextLengthMax = 27;
    storageData.iMBDNNumberLengthMax = 28;    
    
	TMockLtsyPhoneBookData1< CStorageInfoData > retStoreAndName(name, storageData); 
	retStoreAndName.SerialiseL(data);	
	
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data, 0);
    User::WaitForRequest(mockLtsyStatus);        
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    customAPI.Get3GPBInfo(requestStatus, info);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
    ASSERT_TRUE( storageData.iEmailTextLengthMax == info.iMaxLenEmail);
    ASSERT_TRUE( storageData.iSNETextLengthMax == info.iMaxLenSne);
    ASSERT_TRUE( storageData.iANRNumLengthMax == info.iMaxLenAnr);
    ASSERT_TRUE( -1 == info.iMaxLenGroupName);
    ASSERT_TRUE( storageData.iEmailNumOfEntriesPerEntry == info.iMaxNumEmail);
    ASSERT_TRUE( storageData.iSNENumOfEntriesPerEntry == info.iMaxNumSne);
    ASSERT_TRUE( storageData.iANRNumOfEntriesPerEntry == info.iMaxNumAnr);
    ASSERT_TRUE( storageData.iGRPNumOfEntriesPerEntry == info.iMaxNumGroupName);
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSNM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetSystemNetworkModes
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetSystemNetworkModes
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetSystemNetworkModes0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TUint32 modes;
    TBool useRats(EFalse);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< TBool > ltsyData(useRats);	
	ltsyData.SerialiseL(expectData);
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetSystemNetworkModesIPC, expectData, KErrNotSupported);
    
    customAPI.GetSystemNetworkModes(requestStatus, modes);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetSystemNetworkModesIPC, expectData);
    
    const TUint32 compModes = 0xFFFF;
    TPckg<TUint32> modesPckg(compModes);
    
    iMockLTSY.CompleteL( ECustomGetSystemNetworkModesIPC, KErrGeneral, modesPckg);

    customAPI.GetSystemNetworkModes(requestStatus, modes);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetSystemNetworkModes when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetSystemNetworkModesIPC, expectData);
    iMockLTSY.CompleteL( ECustomGetSystemNetworkModesIPC, KErrNone, modesPckg);

    customAPI.GetSystemNetworkModes(requestStatus, modes);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(compModes, modes);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetSystemNetworkModes
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomGetSystemNetworkModesIPC, KErrNone, modesPckg);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSNM-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetSystemNetworkModes
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetSystemNetworkModes
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetSystemNetworkModes0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TUint32 modes;	
    TBool useRats(EFalse);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< TBool > ltsyData(useRats);	
	ltsyData.SerialiseL(expectData);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetSystemNetworkModes
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetSystemNetworkModesIPC, expectData);
    
    const TUint32 compModes = 0xFFFF;
    TPckg<TUint32> modesPckg(compModes);
    
    iMockLTSY.CompleteL( ECustomGetSystemNetworkModesIPC, KErrNone, modesPckg, 20);

    customAPI.GetSystemNetworkModes(requestStatus, modes);
	
	customAPI.CancelAsyncRequest(ECustomGetSystemNetworkModesIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); 	
		
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSNM-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetSystemNetworkModes
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetSystemNetworkModes
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetSystemNetworkModes0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    TUint32 modes;	
    TUint32 modes2;	

    TBool useRats(EFalse);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< TBool > ltsyData(useRats);	
	ltsyData.SerialiseL(expectData);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetSystemNetworkModes
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetSystemNetworkModesIPC, expectData);

    customAPI.GetSystemNetworkModes(requestStatus, modes);
    customAPI2.GetSystemNetworkModes(requestStatus2, modes2);

    const TUint32 compModes = 0xFFFF;
    TPckg<TUint32> modesPckg(compModes);

    iMockLTSY.CompleteL( ECustomGetSystemNetworkModesIPC, KErrNone, modesPckg);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(compModes, modes);
    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSNM-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetSystemNetworkModes with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetSystemNetworkModes and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetSystemNetworkModes0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TUint32 modes;	
    TBool useRats(EFalse);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< TBool > ltsyData(useRats);	
	ltsyData.SerialiseL(expectData);
	
    iMockLTSY.ExpectL( ECustomGetSystemNetworkModesIPC, expectData);
    
    customAPI.GetSystemNetworkModes(requestStatus, modes);

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 405201);
	//Request ECustomGetSystemNetworkModesIPC is not completed by timeout. 		
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); 	

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSNM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SetSystemNetworkMode
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetSystemNetworkMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetSystemNetworkMode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TNetworkModeCaps caps(RMmCustomAPI::KCapsNetworkModeGsm);
    RMmCustomAPI::TNetworkModeCaps expCaps(caps);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< RMmCustomAPI::TNetworkModeCaps > ltsyData(expCaps);	
	ltsyData.SerialiseL(expectData);
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSetSystemNetworkModeIPC, expectData, KErrNotSupported);
    
    customAPI.SetSystemNetworkMode(requestStatus, caps);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSetSystemNetworkModeIPC, expectData);
    iMockLTSY.CompleteL( ECustomSetSystemNetworkModeIPC, KErrGeneral);

    customAPI.SetSystemNetworkMode(requestStatus, caps);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::SetSystemNetworkMode when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSetSystemNetworkModeIPC, expectData);
    iMockLTSY.CompleteL( ECustomSetSystemNetworkModeIPC, KErrNone);

    customAPI.SetSystemNetworkMode(requestStatus, caps);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::SetSystemNetworkMode
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomSetSystemNetworkModeIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSNM-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::SetSystemNetworkMode
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::SetSystemNetworkMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetSystemNetworkMode0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

    RMmCustomAPI::TNetworkModeCaps caps(RMmCustomAPI::KCapsNetworkModeGsm);
    RMmCustomAPI::TNetworkModeCaps expCaps(caps);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< RMmCustomAPI::TNetworkModeCaps > ltsyData(expCaps);	
	ltsyData.SerialiseL(expectData);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::ECustomSetSystemNetworkModeIPC
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSetSystemNetworkModeIPC, expectData);    
    iMockLTSY.CompleteL( ECustomSetSystemNetworkModeIPC, KErrNone, 20);

    customAPI.SetSystemNetworkMode(requestStatus, caps);
	
	customAPI.CancelAsyncRequest(ECustomSetSystemNetworkModeIPC);


	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); 	
		
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSNM-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::SetSystemNetworkMode
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::SetSystemNetworkMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetSystemNetworkMode0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    RMmCustomAPI::TNetworkModeCaps caps(RMmCustomAPI::KCapsNetworkModeGsm);
    RMmCustomAPI::TNetworkModeCaps caps2(RMmCustomAPI::KCapsNetworkModeUmts);
    RMmCustomAPI::TNetworkModeCaps expCaps(caps);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< RMmCustomAPI::TNetworkModeCaps > ltsyData(expCaps);	
	ltsyData.SerialiseL(expectData);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::SetSystemNetworkMode
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSetSystemNetworkModeIPC, expectData, KErrNone);

    customAPI.SetSystemNetworkMode(requestStatus, caps);
    customAPI2.SetSystemNetworkMode(requestStatus2, caps2);

    iMockLTSY.CompleteL( ECustomSetSystemNetworkModeIPC, KErrNone);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSNM-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SetSystemNetworkMode with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetSystemNetworkMode and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetSystemNetworkMode0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

    RMmCustomAPI::TNetworkModeCaps caps(RMmCustomAPI::KCapsNetworkModeGsm);
    RMmCustomAPI::TNetworkModeCaps expCaps(caps);
   
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TMockLtsyData1< RMmCustomAPI::TNetworkModeCaps > ltsyData(expCaps);	
	ltsyData.SerialiseL(expectData);
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 405302);
	//When testing RMmCustomAPI::SetSystemNetworkMode on timeout request ECustomSetSystemNetworkModeIPC
	// is not completed. Test is aborted by testexecute.
	
	// test is stopped because otherwise it blockes the next test
	ASSERT_TRUE(EFalse);
	
    iMockLTSY.ExpectL( ECustomSetSystemNetworkModeIPC, expectData);

    customAPI.SetSystemNetworkMode(requestStatus, caps);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCSNM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetCurrentSystemNetworkModes
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetCurrentSystemNetworkModes
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCurrentSystemNetworkModes0001L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TUint32 modes;
    	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetCurrentSystemNetworkModesIPC, KErrNotSupported);
    
    customAPI.GetCurrentSystemNetworkModes(requestStatus, modes);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetCurrentSystemNetworkModesIPC);

    const TUint32 compModes = 0xFFFF;
    TPckg<TUint32> modesPckg(compModes);

    iMockLTSY.CompleteL( ECustomGetCurrentSystemNetworkModesIPC, KErrGeneral, modesPckg);

    customAPI.GetCurrentSystemNetworkModes(requestStatus, modes);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetCurrentSystemNetworkModes when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetCurrentSystemNetworkModesIPC);
    iMockLTSY.CompleteL( ECustomGetCurrentSystemNetworkModesIPC, KErrNone, modesPckg);

    customAPI.GetCurrentSystemNetworkModes(requestStatus, modes);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(compModes, modes);
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetCurrentSystemNetworkModes
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomGetCurrentSystemNetworkModesIPC, KErrNone, modesPckg);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCSNM-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetCurrentSystemNetworkModes
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetCurrentSystemNetworkModes
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCurrentSystemNetworkModes0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TUint32 modes;	

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetCurrentSystemNetworkModes
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetCurrentSystemNetworkModesIPC);
    
    const TUint32 compModes = 0xFFFF;
    TPckg<TUint32> modesPckg(compModes);
    
    iMockLTSY.CompleteL( ECustomGetCurrentSystemNetworkModesIPC, KErrNone, modesPckg, 20);

    customAPI.GetCurrentSystemNetworkModes(requestStatus, modes);
	
	customAPI.CancelAsyncRequest(ECustomGetCurrentSystemNetworkModesIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 	
		
	}




/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCSNM-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetCurrentSystemNetworkModes
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetCurrentSystemNetworkModes
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCurrentSystemNetworkModes0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    TUint32 modes;	
    TUint32 modes2;	
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetCurrentSystemNetworkModes
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetCurrentSystemNetworkModesIPC, KErrNone);

    customAPI.GetCurrentSystemNetworkModes(requestStatus, modes);
    customAPI2.GetCurrentSystemNetworkModes(requestStatus2, modes2);

    const TUint32 compModes = 0xFFFF;
    TPckg<TUint32> modesPckg(compModes);

    iMockLTSY.CompleteL( ECustomGetCurrentSystemNetworkModesIPC, KErrNone, modesPckg);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCSNM-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetCurrentSystemNetworkModes with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetCurrentSystemNetworkModes and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCurrentSystemNetworkModes0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    TUint32 modes;	

    iMockLTSY.ExpectL( ECustomGetCurrentSystemNetworkModesIPC);

    customAPI.GetCurrentSystemNetworkModes(requestStatus, modes);
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 405401);
	//Request ECustomGetCurrentSystemNetworkModesIPC is not completed by timeout. 		
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCPSON-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::PowerSimOn
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::PowerSimOn
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestPowerSimOn0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomPowerSimOnIPC, KErrNotSupported);
    
    customAPI.PowerSimOn(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomPowerSimOnIPC);

    iMockLTSY.CompleteL( ECustomPowerSimOnIPC, KErrGeneral);

    customAPI.PowerSimOn(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::PowerSimOn when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomPowerSimOnIPC);
    iMockLTSY.CompleteL( ECustomPowerSimOnIPC, KErrNone);

    customAPI.PowerSimOn(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::PowerSimOn
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomPowerSimOnIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCPSON-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::PowerSimOn
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::PowerSimOn
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestPowerSimOn0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::PowerSimOn
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomPowerSimOnIPC);
    iMockLTSY.CompleteL( ECustomPowerSimOnIPC, KErrNone, 20);

    customAPI.PowerSimOn(requestStatus);
	
	customAPI.CancelAsyncRequest(ECustomPowerSimOnIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCPSON-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::PowerSimOn
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::PowerSimOn
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestPowerSimOn0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::PowerSimOn
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomPowerSimOnIPC, KErrNone);

    customAPI.PowerSimOn(requestStatus);
    customAPI2.PowerSimOn(requestStatus2);

    iMockLTSY.CompleteL( ECustomPowerSimOnIPC, KErrNone);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCPSON-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::PowerSimOn with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::PowerSimOn and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestPowerSimOn0005L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

    iMockLTSY.ExpectL( ECustomPowerSimOnIPC);

    customAPI.PowerSimOn(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCPSOF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::PowerSimOff
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::PowerSimOff
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestPowerSimOff0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomPowerSimOffIPC, KErrNotSupported);
    
    customAPI.PowerSimOff(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomPowerSimOffIPC);

    iMockLTSY.CompleteL( ECustomPowerSimOffIPC, KErrGeneral);

    customAPI.PowerSimOff(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::PowerSimOff when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomPowerSimOffIPC);
    iMockLTSY.CompleteL( ECustomPowerSimOffIPC, KErrNone);

    customAPI.PowerSimOff(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::PowerSimOff
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomPowerSimOffIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCPSOF-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::PowerSimOff
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::PowerSimOff
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestPowerSimOff0002L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::PowerSimOff
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomPowerSimOffIPC);
    iMockLTSY.CompleteL( ECustomPowerSimOffIPC, KErrNone, 20);

    customAPI.PowerSimOff(requestStatus);
	
	customAPI.CancelAsyncRequest(ECustomPowerSimOffIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 	
	}




/**
@SYMTestCaseID BA-CTSY-CIPC-MCPSOF-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::PowerSimOff
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::PowerSimOff
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestPowerSimOff0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::PowerSimOff
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomPowerSimOffIPC, KErrNone);

    customAPI.PowerSimOff(requestStatus);
    customAPI2.PowerSimOff(requestStatus2);

    iMockLTSY.CompleteL( ECustomPowerSimOffIPC, KErrNone);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCPSOF-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::PowerSimOff with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::PowerSimOff and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestPowerSimOff0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

    iMockLTSY.ExpectL( ECustomPowerSimOffIPC);

    customAPI.PowerSimOff(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 	

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSWR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SimWarmReset
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SimWarmReset
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSimWarmReset0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSimWarmResetIPC, KErrNotSupported);
    
    customAPI.SimWarmReset(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSimWarmResetIPC);

    iMockLTSY.CompleteL( ECustomSimWarmResetIPC, KErrGeneral);

    customAPI.SimWarmReset(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::SimWarmReset when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSimWarmResetIPC);
    iMockLTSY.CompleteL( ECustomSimWarmResetIPC, KErrNone);

    customAPI.SimWarmReset(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::SimWarmReset
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomSimWarmResetIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSWR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::SimWarmReset
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::SimWarmReset
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSimWarmReset0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::SimWarmReset
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSimWarmResetIPC);
    iMockLTSY.CompleteL( ECustomSimWarmResetIPC, KErrNone, 20);

    customAPI.SimWarmReset(requestStatus);
	
	customAPI.CancelAsyncRequest(ECustomSimWarmResetIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 
	
	}




/**
@SYMTestCaseID BA-CTSY-CIPC-MCSWR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::SimWarmReset
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::SimWarmReset
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSimWarmReset0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::SimWarmResetTR
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSimWarmResetIPC, KErrNone);

    customAPI.SimWarmReset(requestStatus);
    customAPI2.SimWarmReset(requestStatus2);

    iMockLTSY.CompleteL( ECustomSimWarmResetIPC, KErrNone);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSWR-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SimWarmReset with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SimWarmReset and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSimWarmReset0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

    iMockLTSY.ExpectL( ECustomSimWarmResetIPC);

    customAPI.SimWarmReset(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 
	
	}




/**
@SYMTestCaseID BA-CTSY-CIPC-MCGATR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetATR
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetATR
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetATR0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
	RBuf8 expData;
	CleanupClosePushL(expData);

	TBuf8<32> atr;
	TBuf8<32> expAtr(atr);
	TDesC8* ptr = &expAtr;

	TMockLtsyData1< TDesC8* > ltsyData(ptr);	    
    ltsyData.SerialiseL(expData);       
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetATRIPC, expData, KErrNotSupported);
    
    customAPI.GetATR(requestStatus, atr);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetATRIPC, expData);

	_LIT8(KAtr, "ATRATRATR");

    iMockLTSY.CompleteL( ECustomGetATRIPC, KErrGeneral, KAtr);

    customAPI.GetATR(requestStatus, atr);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetATR when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetATRIPC, expData);
    iMockLTSY.CompleteL( ECustomGetATRIPC, KErrNone, KAtr);

    customAPI.GetATR(requestStatus, atr);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(0, atr.Compare(KAtr));

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetATR
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomGetATRIPC, KErrNone, KAtr);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGATR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetATR
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetATR
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetATR0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TRequestStatus requestStatus;
	
	RBuf8 expData;
	CleanupClosePushL(expData);

	TBuf8<32> atr;
	TBuf8<32> expAtr(atr);
	TDesC8* ptr = &expAtr;

	TMockLtsyData1< TDesC8* > ltsyData(ptr);	    
    ltsyData.SerialiseL(expData);       
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetATR
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetATRIPC, expData, KErrNone);
    
	_LIT8(KAtr, "ATRATRATR");

    iMockLTSY.CompleteL( ECustomGetATRIPC, KErrNone, KAtr, 20);    
    
    customAPI.GetATR(requestStatus, atr);
    
    customAPI.CancelAsyncRequest(ECustomGetATRIPC);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
 	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(3);
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGATR-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetATR with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetATR with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetATR0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
	RBuf8 expData;
	CleanupClosePushL(expData);

	TBuf8<1> atr;
	TBuf8<1> expAtr(atr);
	TDesC8* ptr = &expAtr;

	TMockLtsyData1< TDesC8* > ltsyData(ptr);	    
    ltsyData.SerialiseL(expData);       
	
    iMockLTSY.ExpectL( ECustomGetATRIPC, expData, KErrNone);
    
	_LIT8(KAtr, "ATRATRATR");

    iMockLTSY.CompleteL( ECustomGetATRIPC, KErrNone, KAtr);
    
    customAPI.GetATR(requestStatus, atr);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrOverflow, requestStatus.Int());

	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(3);
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGATR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetATR
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetATR
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetATR0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
	
	RBuf8 expData;
	CleanupClosePushL(expData);

	TBuf8<32> atr;
	TBuf8<32> atr2;
	TBuf8<32> expAtr(atr);
	TDesC8* ptr = &expAtr;

	TMockLtsyData1< TDesC8* > ltsyData(ptr);	    
    ltsyData.SerialiseL(expData);       
	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetATR
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetATRIPC, expData, KErrNone);

    customAPI.GetATR(requestStatus, atr);
    customAPI2.GetATR(requestStatus2, atr2);

	_LIT8(KAtr, "ATRATRATR");

    iMockLTSY.CompleteL( ECustomGetATRIPC, KErrNone, KAtr);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGATR-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetATR with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetATR and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetATR0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
	
	RBuf8 expData;
	CleanupClosePushL(expData);

	TBuf8<32> atr;
	TBuf8<32> expAtr(atr);
	TDesC8* ptr = &expAtr;

	TMockLtsyData1< TDesC8* > ltsyData(ptr);	    
    ltsyData.SerialiseL(expData);       
	
    iMockLTSY.ExpectL( ECustomGetATRIPC, expData, KErrNone);
    
    customAPI.GetATR(requestStatus, atr);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(3);
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSCRS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetSimCardReaderStatus
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetSimCardReaderStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetSimCardReaderStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TSimCardReaderStatus status;
    status.SetStatus(0x33);	//random

	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI::TSimCardReaderStatus expStatus(status);
	RMmCustomAPI::TSimCardReaderStatus* expStatusPtr(&expStatus);

	TMockLtsyData1< RMmCustomAPI::TSimCardReaderStatus* > ltsyData(expStatusPtr);	    
    ltsyData.SerialiseL(data);       
	
	RMmCustomAPI::TSimCardReaderStatus compStatus;
	compStatus.SetStatus(0xFF);	// random
	TPckg<RMmCustomAPI::TSimCardReaderStatus> compStatusPckg(compStatus);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetSimCardReaderStatus when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomGetSimCardReaderStatusIPC, data);
    iMockLTSY.CompleteL(ECustomGetSimCardReaderStatusIPC, KErrNone, compStatusPckg);
	
	customAPI.GetSimCardReaderStatus(requestStatus, status);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(compStatus.GetStatus() == status.GetStatus());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C2: RMmCustomAPI::GetSimCardReaderStatus completion with zero data
 	//-------------------------------------------------------------------------

    status.SetStatus(0x33);	

    iMockLTSY.ExpectL(ECustomGetSimCardReaderStatusIPC, data);
    iMockLTSY.CompleteL(ECustomGetSimCardReaderStatusIPC, KErrNone, KNullDesC8);
	
	customAPI.GetSimCardReaderStatus(requestStatus, status);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(0 == status.GetStatus());

	AssertMockLtsyStatusL();
	

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    expStatus = status;
    data.Close();
    ltsyData.SerialiseL(data);       

    iMockLTSY.ExpectL(ECustomGetSimCardReaderStatusIPC, data, KErrNotSupported);

	customAPI.GetSimCardReaderStatus(requestStatus, status);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    expStatus = status;
    data.Close();
    ltsyData.SerialiseL(data);       
    
    iMockLTSY.ExpectL(ECustomGetSimCardReaderStatusIPC, data);
	
    iMockLTSY.CompleteL(ECustomGetSimCardReaderStatusIPC, KErrGeneral, compStatusPckg);
	
	customAPI.GetSimCardReaderStatus(requestStatus, status);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetSimCardReaderStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomGetSimCardReaderStatusIPC, KErrNone, compStatusPckg);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    status.SetStatus(0x33);	//random
    expStatus = status;
    data.Close();
    ltsyData.SerialiseL(data);       
    
    iMockLTSY.ExpectL(ECustomGetSimCardReaderStatusIPC, data);
	
    iMockLTSY.CompleteL(ECustomGetSimCardReaderStatusIPC, KErrGeneral, compStatusPckg);
	
	customAPI.GetSimCardReaderStatus(requestStatus, status);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	ASSERT_TRUE(expStatus.GetStatus() == status.GetStatus());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSCRS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetSimCardReaderStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetSimCardReaderStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetSimCardReaderStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TSimCardReaderStatus status;

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetSimCardReaderStatus
 	//-------------------------------------------------------------------------
 	
	RMmCustomAPI::TSimCardReaderStatus expStatus(status);
	RMmCustomAPI::TSimCardReaderStatus* expStatusPtr(&expStatus);

	TMockLtsyData1< RMmCustomAPI::TSimCardReaderStatus* > ltsyData(expStatusPtr);	    
    ltsyData.SerialiseL(data);       
	
    iMockLTSY.ExpectL(ECustomGetSimCardReaderStatusIPC, data);

	RMmCustomAPI::TSimCardReaderStatus compStatus;
	compStatus.SetStatus(0xFF);
	TPckg<RMmCustomAPI::TSimCardReaderStatus> compStatusPckg(compStatus);
	
    iMockLTSY.CompleteL(ECustomGetSimCardReaderStatusIPC, KErrNone, compStatusPckg, 20);
	
	customAPI.GetSimCardReaderStatus(requestStatus, status);
	
	customAPI.CancelAsyncRequest(ECustomGetSimCardReaderStatusIPC);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3); 
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSCRS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetSimCardReaderStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetSimCardReaderStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetSimCardReaderStatus0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetSimCardReaderStatus
 	//-------------------------------------------------------------------------

    TRequestStatus requestStatus;
    RMmCustomAPI::TSimCardReaderStatus status;

    TRequestStatus requestStatus2;
    RMmCustomAPI::TSimCardReaderStatus status2;

	RMmCustomAPI::TSimCardReaderStatus expStatus(status);
	RMmCustomAPI::TSimCardReaderStatus* expStatusPtr(&expStatus);

	TMockLtsyData1< RMmCustomAPI::TSimCardReaderStatus* > ltsyData(expStatusPtr);	    
    ltsyData.SerialiseL(data);       
	
    iMockLTSY.ExpectL(ECustomGetSimCardReaderStatusIPC, data);

	customAPI.GetSimCardReaderStatus(requestStatus, status);
	customAPI2.GetSimCardReaderStatus(requestStatus2, status2);

	RMmCustomAPI::TSimCardReaderStatus compStatus;
	compStatus.SetStatus(0xFF);
	TPckg<RMmCustomAPI::TSimCardReaderStatus> compStatusPckg(compStatus);
	
    iMockLTSY.CompleteL(ECustomGetSimCardReaderStatusIPC, KErrNone, compStatusPckg, 20);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	CleanupStack::PopAndDestroy(6, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSCRS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetSimCardReaderStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetSimCardReaderStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetSimCardReaderStatus0005L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TSimCardReaderStatus status;

	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI::TSimCardReaderStatus expStatus(status);
	RMmCustomAPI::TSimCardReaderStatus* expStatusPtr(&expStatus);

	TMockLtsyData1< RMmCustomAPI::TSimCardReaderStatus* > ltsyData(expStatusPtr);	    
    ltsyData.SerialiseL(data);       
	
    iMockLTSY.ExpectL(ECustomGetSimCardReaderStatusIPC, data);

	customAPI.GetSimCardReaderStatus(requestStatus, status);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNSCS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifySimCardStatus
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifySimCardStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifySimCardStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TSIMCardStatus cardStatus;
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifySimCardStatus when result is not cached.
 	//-------------------------------------------------------------------------
 	RMmCustomAPI::TSIMCardStatus compStatus(RMmCustomAPI::SimCardRemoved);
	TPckg<RMmCustomAPI::TSIMCardStatus> compStatusPckg(compStatus);	

    iMockLTSY.CompleteL(ECustomNotifySimCardStatusIPC, KErrNone, compStatusPckg);

	customAPI.NotifySimCardStatus(requestStatus, cardStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compStatus, cardStatus);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifySimCardStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomNotifySimCardStatusIPC, KErrNone, compStatusPckg);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNSCS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifySimCardStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifySimCardStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifySimCardStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TSIMCardStatus cardStatus;
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifySimCardStatus
 	//-------------------------------------------------------------------------

	customAPI.NotifySimCardStatus(requestStatus, cardStatus);

 	RMmCustomAPI::TSIMCardStatus compStatus(RMmCustomAPI::SimCardRemoved);
	TPckg<RMmCustomAPI::TSIMCardStatus> compStatusPckg(compStatus);	
    iMockLTSY.CompleteL(ECustomNotifySimCardStatusIPC, KErrNone, compStatusPckg, 20);
		
	customAPI.CancelAsyncRequest(ECustomNotifySimCardStatusIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(2); 
	
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-MCNSCS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifySimCardStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifySimCardStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifySimCardStatus0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

    RMmCustomAPI::TSIMCardStatus cardStatus;
    RMmCustomAPI::TSIMCardStatus cardStatus2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifySimCardStatus
 	//-------------------------------------------------------------------------

 	RMmCustomAPI::TSIMCardStatus compStatus(RMmCustomAPI::SimCardRemoved);
	TPckg<RMmCustomAPI::TSIMCardStatus> compStatusPckg(compStatus);	
    iMockLTSY.CompleteL(ECustomNotifySimCardStatusIPC, KErrNone, compStatusPckg, 20);

	customAPI.NotifySimCardStatus(requestStatus, cardStatus);
    customAPI2.NotifySimCardStatus(requestStatus2, cardStatus2);


	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compStatus, cardStatus);

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(compStatus, cardStatus2);

	AssertMockLtsyStatusL();
	
    //-------------------------------------------------------------------------
    // Test B: Test multiple clients requesting RMmCustomAPI::NotifySimCardStatus returning SimCardInserted
    //-------------------------------------------------------------------------
    
    RMmCustomAPI::TSIMCardStatus compStatusB(RMmCustomAPI::SimCardInserted);
    TPckg<RMmCustomAPI::TSIMCardStatus> compStatusPckgB(compStatusB); 
    iMockLTSY.CompleteL(ECustomNotifySimCardStatusIPC, KErrNone, compStatusPckgB, 20);
    
    cardStatus  = RMmCustomAPI::SimCardUnknowError;
    cardStatus2 = RMmCustomAPI::SimCardUnknowError;
    customAPI.NotifySimCardStatus(requestStatus, cardStatus);
    customAPI2.NotifySimCardStatus(requestStatus2, cardStatus2);
   
    
    User::WaitForRequest(requestStatus);    
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(compStatusB, cardStatus);
    
    User::WaitForRequest(requestStatus2);   
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(compStatusB, cardStatus2);
    
    AssertMockLtsyStatusL();
	   
	CleanupStack::PopAndDestroy(5, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGWSAD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetWlanSimAuthenticationData
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetWlanSimAuthenticationData
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetWlanSimAuthenticationData0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;	
	
	// --- data for CompleteL ---
	RMmCustomAPI::TSimAuthenticationEapSim compSim;
	// get random value
    FillRandom(compSim.iSRES);
    FillRandom(compSim.iKC);
	TPckg<RMmCustomAPI::TSimAuthenticationEapSim> compSimPckg(compSim);	
 
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	RMmCustomAPI::TSimAuthenticationEapSim sim;
	TPckg<RMmCustomAPI::TSimAuthenticationEapSim> simPckg(sim);
	
	// get random value
    FillRandom(sim.iRandomParameters);
	
    iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, simPckg, KErrNotSupported);

	customAPI.GetWlanSimAuthenticationData(requestStatus, simPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
    FillRandom(sim.iRandomParameters);
	
    iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, simPckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrGeneral, compSimPckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, simPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetWlanSimAuthenticationData when result is not cached.
 	//-------------------------------------------------------------------------
    FillRandom(sim.iRandomParameters);
	
    iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, simPckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrNone, compSimPckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, simPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compSim.iSRES,sim.iSRES);
	ASSERT_EQUALS(compSim.iKC, sim.iKC);

	AssertMockLtsyStatusL();	
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetWlanSimAuthenticationData
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrNone, compSimPckg);

	User::WaitForRequest(mockLtsyStatus);
	
	AssertMockLtsyStatusL();	
	//-------------------------------------------------------------------------
	// Test 1 for coverage increasing	
	//-------------------------------------------------------------------------
	RMmCustomAPI::TSimAuthenticationEapAka simAka;
	TPckg<RMmCustomAPI::TSimAuthenticationEapAka> simAkaPckg(simAka);
	
	// get random value
    FillRandom(simAka.iRandomParameters);
	
	// --- data for CompleteL ---
	RMmCustomAPI::TSimAuthenticationEapAka compSimAka;
	// get random value
    FillRandom(compSimAka.iCK);
    FillRandom(compSimAka.iIK); 
    FillRandom(compSimAka.iAUTS);   
	TPckg<RMmCustomAPI::TSimAuthenticationEapAka> compSimAkaPckg(compSimAka);	
	
    iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, simAkaPckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrNone, compSimAkaPckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, simAkaPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compSimAka.iCK,simAka.iCK);
	ASSERT_EQUALS(compSimAka.iIK, simAka.iIK);
	ASSERT_EQUALS(compSimAka.iRES, simAka.iRES);	
	AssertMockLtsyStatusL();	
	
	
	//-------------------------------------------------------------------------
	// Test 2 for coverage increasing
	// completion of RMmCustomAPI::GetWlanSimAuthenticationData
	// from LTSY with error and RMmCustomAPI::TSimAuthenticationEapAka as data
	//-------------------------------------------------------------------------		
    FillRandom(simAka.iRandomParameters);
	
	iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, simAkaPckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrGeneral, compSimAkaPckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, simAkaPckg);
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	
	ASSERT_EQUALS(compSimAka.iAUTS, simAka.iAUTS);	
 	AssertMockLtsyStatusL();
 	 	
 	//-------------------------------------------------------------------------
	// Test 3 for coverage increasing
	// Unsolicited completion of RMmCustomAPI::GetWlanSimAuthenticationData
	// from LTSY with error
	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrGeneral, compSimPckg);
	User::WaitForRequest(mockLtsyStatus);
	
	AssertMockLtsyStatusL();


 	//-------------------------------------------------------------------------
	// Test using RMmCustomAPI::TSimAuthenticationGbaBootstrap
	//-------------------------------------------------------------------------
 
	// Data for argument
	RMmCustomAPI::TSimAuthenticationGbaBootstrap bootstrap;
	TPckg<RMmCustomAPI::TSimAuthenticationGbaBootstrap> bootstrapPckg(bootstrap);
		
	// get random values and populate the packages
    FillRandom(bootstrap.iRandomParameters);
    FillRandom(bootstrap.iAUTN);
	
	// Return parameters
	bootstrap.iRES.Fill(0, RMmCustomAPI::KMaxParametersLength);	
	bootstrap.iAUTS.Fill(0, RMmCustomAPI::KMaxParametersLength);
	
	// --- data for CompleteL ---
	RMmCustomAPI::TSimAuthenticationGbaBootstrap compBootstrap;
	
	compBootstrap.iRandomParameters = bootstrap.iRandomParameters;
	compBootstrap.iAUTN = bootstrap.iAUTN;

    FillRandom(compBootstrap.iRES);
    FillRandom(compBootstrap.iAUTS);
	
	TPckg<RMmCustomAPI::TSimAuthenticationGbaBootstrap> compBootstrapPckg(compBootstrap);	
		
    iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, bootstrapPckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrNone, compBootstrapPckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, bootstrapPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compBootstrap.iRandomParameters, bootstrap.iRandomParameters);
	ASSERT_EQUALS(compBootstrap.iAUTN, bootstrap.iAUTN);	
	ASSERT_EQUALS(compBootstrap.iAUTS, bootstrap.iAUTS);
	ASSERT_EQUALS(compBootstrap.iRES, bootstrap.iRES);	
	
	AssertMockLtsyStatusL();	

	// completion by error value

	iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, bootstrapPckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrGeneral, compBootstrapPckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, bootstrapPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
	AssertMockLtsyStatusL();
 	 	

 	//-------------------------------------------------------------------------
	// Test using RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate
	//-------------------------------------------------------------------------
 
	RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate bsUpdate;
	TPckg<RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate> bsUpdatePckg(bsUpdate);

	// --- data for ExpectL ---
    FillRandom(bsUpdate.iBTid);
    FillRandom(bsUpdate.iKeyLifeTime);
	bsUpdate.iRandomParameters.Fill(0, RMmCustomAPI::KMaxParametersLength);
	
	// --- data for CompleteL ---
	RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate compBsUpdate;
	compBsUpdate.iBTid = bsUpdate.iBTid;
	compBsUpdate.iKeyLifeTime = bsUpdate.iKeyLifeTime; 
    FillRandom(compBsUpdate.iRandomParameters);
	TPckg<RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate> compBsUpdatePckg(compBsUpdate);	
		
	iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, bsUpdatePckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrNone, compBsUpdatePckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, bsUpdatePckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compBsUpdate.iBTid, bsUpdate.iBTid);
	ASSERT_EQUALS(compBsUpdate.iKeyLifeTime, bsUpdate.iKeyLifeTime);
	ASSERT_EQUALS(compBsUpdate.iRandomParameters, bsUpdate.iRandomParameters);

	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// Test using RMmCustomAPI::TSimAuthenticationGbaNafDerivation
	//-------------------------------------------------------------------------
 
	RMmCustomAPI::TSimAuthenticationGbaNafDerivation naf;
    FillRandom(naf.iNafId);
    FillRandom(naf.iImpi);
	naf.iKsExtNaf.Fill(0, RMmCustomAPI::KMaxParametersLength);
	TPckg<RMmCustomAPI::TSimAuthenticationGbaNafDerivation> nafPckg(naf);
	
	// --- data for CompleteL ---
	RMmCustomAPI::TSimAuthenticationGbaNafDerivation compNaf;
	// get random value
	compNaf.iNafId = naf.iNafId;
	compNaf.iImpi = naf.iImpi;
    FillRandom(compNaf.iKsExtNaf);
	TPckg<RMmCustomAPI::TSimAuthenticationGbaNafDerivation> compNafPckg(compNaf);	
			
    iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, nafPckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrNone, compNafPckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, nafPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compNaf.iNafId, naf.iNafId);
	ASSERT_EQUALS(compNaf.iImpi, naf.iImpi);
	ASSERT_EQUALS(compNaf.iKsExtNaf, naf.iKsExtNaf);
	
	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// Test using RMmCustomAPI::TSimAuthenticationMgvMskUpdate
	//-------------------------------------------------------------------------
 
	//Initialize the data to zero
	RMmCustomAPI::TSimAuthenticationMgvMskUpdate mskUpd;
	mskUpd.iMikey.Fill(0, RMmCustomAPI::KMaxMikeyLength);
	mskUpd.iBCASTManagement.Fill(0,RMmCustomAPI::KMaxParametersLength);
	mskUpd.iParentalRating.Fill(0,RMmCustomAPI::KMaxKeyGroupIdLength);
	mskUpd.iSecurityPolicyExt.Fill(0,RMmCustomAPI::KMaxParametersLength);
	TPckg<RMmCustomAPI::TSimAuthenticationMgvMskUpdate> mskUpdPckg(mskUpd);
	
	// --- data for CompleteL ---
	RMmCustomAPI::TSimAuthenticationMgvMskUpdate compMskUpd;
	// get random value
    FillRandom(compMskUpd.iMikey);
    FillRandom(compMskUpd.iBCASTManagement);
    FillRandom(compMskUpd.iParentalRating);
    FillRandom(compMskUpd.iSecurityPolicyExt);
	TPckg<RMmCustomAPI::TSimAuthenticationMgvMskUpdate> compMskUpdPckg(compMskUpd);	
		
    iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, mskUpdPckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrNone, compMskUpdPckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, mskUpdPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compMskUpd.iMikey, mskUpd.iMikey);
	//Added for MobileTV.
	ASSERT_EQUALS(compMskUpd.iBCASTManagement, mskUpd.iBCASTManagement);
	ASSERT_EQUALS(compMskUpd.iParentalRating, mskUpd.iParentalRating);
	ASSERT_EQUALS(compMskUpd.iSecurityPolicyExt, mskUpd.iSecurityPolicyExt);
			
	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// Test using RMmCustomAPI::TSimAuthenticationMgvMtkGeneration
	//-------------------------------------------------------------------------
 
	RMmCustomAPI::TSimAuthenticationMgvMtkGeneration mtkGen;
    FillRandom(mtkGen.iMikey);
	mtkGen.iMtkSalt.Fill(0, RMmCustomAPI::KMaxMikeyLength);
	mtkGen.iBCASTManagement.Fill(0,RMmCustomAPI::KMaxParametersLength);
	mtkGen.iParentalControl.Fill(0,RMmCustomAPI::KMaxKeyGroupIdLength);
	mtkGen.iTrafficEncryptionKey.Fill(0,RMmCustomAPI::KMaxParametersLength);
	TPckg<RMmCustomAPI::TSimAuthenticationMgvMtkGeneration> mtkGenPckg(mtkGen);
	
	
	// --- data for CompleteL ---
	RMmCustomAPI::TSimAuthenticationMgvMtkGeneration compMtkGen;	
	// get random value
	compMtkGen.iMikey = mtkGen.iMikey;
    FillRandom(compMtkGen.iMtkSalt);
    FillRandom(compMtkGen.iBCASTManagement);
    FillRandom(compMtkGen.iParentalControl);
    FillRandom(compMtkGen.iTrafficEncryptionKey);
	TPckg<RMmCustomAPI::TSimAuthenticationMgvMtkGeneration> compMtkGenPckg(compMtkGen);	
		
    iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, mtkGenPckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrNone, compMtkGenPckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, mtkGenPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compMtkGen.iMikey, mtkGen.iMikey);
	ASSERT_EQUALS(compMtkGen.iMtkSalt, mtkGen.iMtkSalt);
	//Added for Mobile TV
	ASSERT_EQUALS(compMtkGen.iBCASTManagement, mtkGen.iBCASTManagement);
	ASSERT_EQUALS(compMtkGen.iParentalControl, mtkGen.iParentalControl);
	ASSERT_EQUALS(compMtkGen.iTrafficEncryptionKey, mtkGen.iTrafficEncryptionKey);
	
		
	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// Test using RMmCustomAPI::TSimAuthenticationMgvMskDeletion
	//-------------------------------------------------------------------------
 
	RMmCustomAPI::TSimAuthenticationMgvMskDeletion mskDel;
    FillRandom(mskDel.iKeyDomainId);
    FillRandom(mskDel.iKeyGroupIdPart);
	TPckg<RMmCustomAPI::TSimAuthenticationMgvMskDeletion> mskDelPckg(mskDel);
	
	// --- data for CompleteL ---
	RMmCustomAPI::TSimAuthenticationMgvMskDeletion compMskDel;
	compMskDel.iKeyDomainId = mskDel.iKeyDomainId;
	compMskDel.iKeyGroupIdPart = mskDel.iKeyGroupIdPart;
	TPckg<RMmCustomAPI::TSimAuthenticationMgvMskDeletion> compMskDelPckg(compMskDel);	
		
    iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, mskDelPckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrNone, compMskDelPckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, mskDelPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compMskDel.iKeyDomainId, mskDel.iKeyDomainId);
	ASSERT_EQUALS(compMskDel.iKeyGroupIdPart, mskDel.iKeyGroupIdPart);
		
	AssertMockLtsyStatusL();
			 	
	CleanupStack::PopAndDestroy(2, this); // customAPI
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGWSAD-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetWlanSimAuthenticationData
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetWlanSimAuthenticationData
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetWlanSimAuthenticationData0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;

	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetWlanSimAuthenticationData
 	//------------------------------------------------------------------------- 	
 	// data for APi
 	RMmCustomAPI::TSimAuthenticationEapSim sim;
	TPckg<RMmCustomAPI::TSimAuthenticationEapSim> simPckg(sim);	
	// get random value
    FillRandom(sim.iRandomParameters);
	
	// data for ExpectL
	RMmCustomAPI::TSimAuthenticationEapSim expSim(sim);
	TPckg<RMmCustomAPI::TSimAuthenticationEapSim> expSimPckg(expSim);
	
	// data for Complete
	RMmCustomAPI::TSimAuthenticationEapSim compSim;
    FillRandom(compSim.iSRES);
    FillRandom(compSim.iKC);
	TPckg<RMmCustomAPI::TSimAuthenticationEapSim> compSimPckg(compSim);	
	
	iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, expSimPckg);		
	customAPI.GetWlanSimAuthenticationData(requestStatus, simPckg);
	
	customAPI.CancelAsyncRequest(ECustomGetSimAuthenticationDataIPC);	
	
		
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());  	

	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(2, this); //  customAPI	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGWSAD-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetWlanSimAuthenticationData with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetWlanSimAuthenticationData with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetWlanSimAuthenticationData0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	
	TInt  wrongData (-1); // any value, but not 1 or 2
	TPckg<TInt> wrongDataPckg(wrongData);	

	//-------------------------------------------------------------------------
	// Test wrong params, passed from LTSY to CTSY
 	//------------------------------------------------------------------------- 	
	RMmCustomAPI::TSimAuthenticationEapAka simAka;
	TPckg<RMmCustomAPI::TSimAuthenticationEapAka> simAkaPckg(simAka);	
	// get random value
    FillRandom(simAka.iRandomParameters);
	
	iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, simAkaPckg);
    iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrArgument, wrongDataPckg);

	customAPI.GetWlanSimAuthenticationData(requestStatus, simAkaPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	
	
	AssertMockLtsyStatusL();	

	//-------------------------------------------------------------------------
	// Test Try to use the same Rand twice
 	//------------------------------------------------------------------------- 	
	RMmCustomAPI::TSimAuthenticationEapSim sim;
	TPckg<RMmCustomAPI::TSimAuthenticationEapSim> simPckg(sim);		
	sim.iRandomParameters.Fill(0x00, RMmCustomAPI::KMaxParametersLength);
	
	iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, simPckg);
	// use this Rand first time 
    customAPI.GetWlanSimAuthenticationData(requestStatus, simPckg);	
	User::WaitForRequest(requestStatus);
	// don't check requestStatus cause it depends on if  rand_db.cur exists or not
	
	// use this Rand second time 
	// CTSY should complete request with KErrArgument
    customAPI.GetWlanSimAuthenticationData(requestStatus, simPckg);	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());		
    AssertMockLtsyStatusL();	
	//-------------------------------------------------------------------------
	// Test wrong params, passed to API
 	//-------------------------------------------------------------------------  	
 	// CTSY (CustomAPI) checks whether passed parameters are wrong. 
 	// if they are wrong than it completes request
 	// and sends request with wrong parameter to LSTY !!!

		
 	customAPI.GetWlanSimAuthenticationData(requestStatus, wrongDataPckg);
 	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy (2, this); //customAPI
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGWSAD-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetWlanSimAuthenticationData
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetWlanSimAuthenticationData
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetWlanSimAuthenticationData0004L()
	{
						
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	
	// data for the first APi
 	RMmCustomAPI::TSimAuthenticationEapAka simAka;
	TPckg<RMmCustomAPI::TSimAuthenticationEapAka> simPckg(simAka);	
    FillRandom(simAka.iRandomParameters);
	
	// data for the first ExpectL
	RMmCustomAPI::TSimAuthenticationEapAka expSimAka(simAka);
	TPckg<RMmCustomAPI::TSimAuthenticationEapAka> expSimPckg(expSimAka);
	
	// data for the first Complete
	RMmCustomAPI::TSimAuthenticationEapAka compSimAka;
	TPckg<RMmCustomAPI::TSimAuthenticationEapAka> compSimPckg(compSimAka);	
	
	
	// data for second APi
 	RMmCustomAPI::TSimAuthenticationEapSim sim2;
	TPckg<RMmCustomAPI::TSimAuthenticationEapSim> simPckg2(sim2);	
    FillRandom(sim2.iRandomParameters);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetWlanSimAuthenticationData
 	//------------------------------------------------------------------------- 	
 	// first request 
 	iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, expSimPckg);		
	iMockLTSY.CompleteL(ECustomGetSimAuthenticationDataIPC, KErrNone, compSimPckg);
	
	customAPI.GetWlanSimAuthenticationData(requestStatus, simPckg);
	
	// second request  	
	customAPI2.GetWlanSimAuthenticationData(requestStatus2, simPckg2);	
	
	// wait for the first answer
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// wait for the second answer
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(5, this);
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGWSAD-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetWlanSimAuthenticationData with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetWlanSimAuthenticationData and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetWlanSimAuthenticationData0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	
	// data for APi
 	RMmCustomAPI::TSimAuthenticationEapSim sim;
	TPckg<RMmCustomAPI::TSimAuthenticationEapSim> simPckg(sim);	
    FillRandom(sim.iRandomParameters);
	
	// data for t ExpectL
	RMmCustomAPI::TSimAuthenticationEapSim expSim(sim);
	TPckg<RMmCustomAPI::TSimAuthenticationEapSim> expSimPckg(expSim);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMmCustomAPI::GetWlanSimAuthenticationData
 	//-------------------------------------------------------------------------
 	iMockLTSY.ExpectL(ECustomGetSimAuthenticationDataIPC, expSimPckg);			
	customAPI.GetWlanSimAuthenticationData(requestStatus, simPckg);	
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(2, this); // customAPI
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSMSR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SetSimMessageStatusRead
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetSimMessageStatusRead
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetSimMessageStatusRead0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);
	
	TTime time;	
	time.UniversalTime();
	TInt timeZone(1);
	
	TTime expTime(time);	
	TInt expTimeZone(timeZone);
	
	TMockLtsyData2< TTime, TInt > ltsyData(expTime, expTimeZone);	    
    ltsyData.SerialiseL(data);    	    

	
	// Actually the function does not have any opportunity to return error value.
	// Perhaps this is a design error...
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
    
    iMockLTSY.ExpectL(ECustomSetSimMessageStatusReadIPC, data, KErrNotSupported);
	
	customAPI.SetSimMessageStatusRead(time, timeZone);
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::SetSimMessageStatusRead when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomSetSimMessageStatusReadIPC, data, KErrNone);
	
	customAPI.SetSimMessageStatusRead(time, timeZone);
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); 
	
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-MCWVHZUS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteViagHomeZoneUHZIUESettings0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	RMmCustomAPI::TViagUHZIUESettings settings;
	
	settings.iSettings = 0xFF;
	settings.iVersion = 1;
	
	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI::TViagUHZIUESettings expSettings = settings;

	
	TMockLtsyData1< RMmCustomAPI::TViagUHZIUESettings > ltsyData(expSettings);	    
    ltsyData.SerialiseL(data);    	    
    
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
    
    iMockLTSY.ExpectL(EWriteViagHomeZoneUHZIUESettingsIPC, data, KErrNotSupported);
	
	customAPI.WriteViagHomeZoneUHZIUESettings(requestStatus, settings);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EWriteViagHomeZoneUHZIUESettingsIPC, data);
    iMockLTSY.CompleteL(EWriteViagHomeZoneUHZIUESettingsIPC, KErrGeneral);        	

	customAPI.WriteViagHomeZoneUHZIUESettings(requestStatus, settings);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyRauEvent when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EWriteViagHomeZoneUHZIUESettingsIPC, data);
    iMockLTSY.CompleteL(EWriteViagHomeZoneUHZIUESettingsIPC, KErrNone);        	

	customAPI.WriteViagHomeZoneUHZIUESettings(requestStatus, settings);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyRauEvent
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(EWriteViagHomeZoneUHZIUESettingsIPC, KErrNone);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCWVHZUS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteViagHomeZoneUHZIUESettings0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);


	TRequestStatus requestStatus;
	RMmCustomAPI::TViagUHZIUESettings settings;
	
	settings.iSettings = 0xFF;
	settings.iVersion = 1;
	
	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI::TViagUHZIUESettings expSettings = settings;

	
	TMockLtsyData1< RMmCustomAPI::TViagUHZIUESettings > ltsyData(expSettings);	    
    ltsyData.SerialiseL(data);    	    
    
 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EWriteViagHomeZoneUHZIUESettingsIPC, data);
    iMockLTSY.CompleteL(EWriteViagHomeZoneUHZIUESettingsIPC, KErrNone, 20);        	

	customAPI.WriteViagHomeZoneUHZIUESettings(requestStatus, settings);
	
	customAPI.CancelAsyncRequest(EWriteViagHomeZoneUHZIUESettingsIPC);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); 	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCWVHZUS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteViagHomeZoneUHZIUESettings0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	RMmCustomAPI::TViagUHZIUESettings settings;
	RMmCustomAPI::TViagUHZIUESettings settings2;
	
	settings.iSettings = 0xFF;
	settings.iVersion = 1;
	
	RMmCustomAPI::TViagUHZIUESettings expSettings = settings;

	
	TMockLtsyData1< RMmCustomAPI::TViagUHZIUESettings > ltsyData(expSettings);	    
    ltsyData.SerialiseL(data);    	    

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::WriteViagHomeZoneUHZIUESettings
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EWriteViagHomeZoneUHZIUESettingsIPC, data);
    iMockLTSY.CompleteL(EWriteViagHomeZoneUHZIUESettingsIPC, KErrNone, 20);        	

	customAPI.WriteViagHomeZoneUHZIUESettings(requestStatus, settings);
	
	customAPI2.WriteViagHomeZoneUHZIUESettings(requestStatus2, settings2);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCWVHZUS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::WriteViagHomeZoneUHZIUESettings with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::WriteViagHomeZoneUHZIUESettings and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteViagHomeZoneUHZIUESettings0005L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	RMmCustomAPI::TViagUHZIUESettings settings = {0, 0};
	
	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI::TViagUHZIUESettings expSettings = settings;

	
	TMockLtsyData1< RMmCustomAPI::TViagUHZIUESettings > ltsyData(expSettings);	    
    ltsyData.SerialiseL(data);    	    
    
    iMockLTSY.ExpectL(EWriteViagHomeZoneUHZIUESettingsIPC, data);

	customAPI.WriteViagHomeZoneUHZIUESettings(requestStatus, settings);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAO-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SetAlwaysOn
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetAlwaysOn
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetAlwaysOn0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;		

	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TRequestStatus requestStatus; 
	RMmCustomAPI::TSetAlwaysOnMode expMode = RMmCustomAPI::EAlwaysModeBoth; 	

	TMockLtsyData1 <RMmCustomAPI::TSetAlwaysOnMode> expData(expMode);
	expData.SerialiseL(expectData);
		
	RMmCustomAPI::TSetAlwaysOnMode mode = RMmCustomAPI::EAlwaysModeBoth; 	
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomSetAlwaysOnMode, expectData);	

	TInt result = 0;

	TMockLtsyData1 <TInt> ltsyData(result);
	ltsyData.SerialiseL(data);
	
	iMockLTSY.CompleteL(ECustomSetAlwaysOnMode, KErrGeneral, data);

	customAPI.SetAlwaysOn(requestStatus, mode);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::SetAlwaysOn
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.CompleteL(ECustomSetAlwaysOnMode, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomSetAlwaysOnMode, expectData, KErrNotSupported);	

	customAPI.SetAlwaysOn(requestStatus, mode);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::SetAlwaysOn when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomSetAlwaysOnMode, expectData);	
	iMockLTSY.CompleteL(ECustomSetAlwaysOnMode, KErrNone, data);

	customAPI.SetAlwaysOn(requestStatus, mode);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAO-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::SetAlwaysOn
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::SetAlwaysOn
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetAlwaysOn0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;		

	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TRequestStatus requestStatus; 
	RMmCustomAPI::TSetAlwaysOnMode expMode = RMmCustomAPI::EAlwaysModeBoth; 	

	TMockLtsyData1 <RMmCustomAPI::TSetAlwaysOnMode> expData(expMode);
	expData.SerialiseL(expectData);
		
	RMmCustomAPI::TSetAlwaysOnMode mode = RMmCustomAPI::EAlwaysModeBoth; 	
	
	TInt result = 0;

	TMockLtsyData1 <TInt> ltsyData(result);
	ltsyData.SerialiseL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::SetAlwaysOn
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(ECustomSetAlwaysOnMode, expectData);	
	//iMockLTSY.CompleteL(ECustomSetAlwaysOnMode, KErrNone, data, 20);

	customAPI.SetAlwaysOn(requestStatus, mode);
	
	customAPI.CancelAsyncRequest(ECustomSetAlwaysOnMode);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
 	
	AssertMockLtsyStatusL();
 	
	CleanupStack::PopAndDestroy(5); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAO-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SetAlwaysOn with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetAlwaysOn with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetAlwaysOn0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus; 
		
	RMmCustomAPI::TSetAlwaysOnMode mode = RMmCustomAPI::EAlwaysModeBoth; 	
	
	customAPI.SetAlwaysOn(requestStatus, mode);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();
 	
	CleanupStack::PopAndDestroy(2); 
	

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSAO-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::SetAlwaysOn
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::SetAlwaysOn
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetAlwaysOn0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RPacketService packetService;		

	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TRequestStatus requestStatus; 
	RMmCustomAPI::TSetAlwaysOnMode expMode = RMmCustomAPI::EAlwaysModeBoth; 	

	TRequestStatus requestStatus2; 

	TMockLtsyData1 <RMmCustomAPI::TSetAlwaysOnMode> expData(expMode);
	expData.SerialiseL(expectData);
		
	RMmCustomAPI::TSetAlwaysOnMode mode = RMmCustomAPI::EAlwaysModeBoth; 	
	RMmCustomAPI::TSetAlwaysOnMode mode2 = RMmCustomAPI::EAlwaysModeBoth; 	
	
	TInt result = 0;

	TMockLtsyData1 <TInt> ltsyData(result);
	ltsyData.SerialiseL(data);

	iMockLTSY.ExpectL(ECustomSetAlwaysOnMode, expectData);	
	iMockLTSY.CompleteL(ECustomSetAlwaysOnMode, KErrNone, data, 20);

	customAPI.SetAlwaysOn(requestStatus, mode);

	customAPI2.SetAlwaysOn(requestStatus2, mode2);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(8, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNRE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyRauEvent
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyRauEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyRauEvent0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyRauEvent
	// from LTSY.
 	//-------------------------------------------------------------------------

	RMmCustomAPI::TRauEventStatus completeStatus(RMmCustomAPI::ERauEventResumed);
	TPckg<RMmCustomAPI::TRauEventStatus> completePckg(completeStatus);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomNotifyRauEventIPC, KErrNone, completePckg);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	RMmCustomAPI::TRauEventStatus status(RMmCustomAPI::ERauEventActive);

	customAPI.NotifyRauEvent(requestStatus, status);
	
    iMockLTSY.CompleteL(ECustomNotifyRauEventIPC, KErrGeneral, completePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyRauEvent when result is not cached.
 	//-------------------------------------------------------------------------

	customAPI.NotifyRauEvent(requestStatus, status);
	
	completeStatus = RMmCustomAPI::ERauEventResumed;

    iMockLTSY.CompleteL(ECustomNotifyRauEventIPC, KErrNone, completePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeStatus, status);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNRE-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifyRauEvent
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifyRauEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyRauEvent0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifyRauEvent
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	RMmCustomAPI::TRauEventStatus status;

	customAPI.NotifyRauEvent(requestStatus, status);
	
	customAPI.CancelAsyncRequest(ECustomNotifyRauEventIPC);
	
	RMmCustomAPI::TRauEventStatus completeStatus(RMmCustomAPI::ERauEventActive);
	TPckg<RMmCustomAPI::TRauEventStatus> completePckg(completeStatus);
	
    iMockLTSY.CompleteL(ECustomNotifyRauEventIPC, KErrNone, completePckg);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 
	
	}




/**
@SYMTestCaseID BA-CTSY-CIPC-MCNRE-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifyRauEvent
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifyRauEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyRauEvent0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	RMmCustomAPI::TRauEventStatus status(RMmCustomAPI::ERauEventActive);
	RMmCustomAPI::TRauEventStatus status2(RMmCustomAPI::ERauEventActive);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifyRauEvent
 	//-------------------------------------------------------------------------

	customAPI.NotifyRauEvent(requestStatus, status);
	customAPI2.NotifyRauEvent(requestStatus2, status2);

	RMmCustomAPI::TRauEventStatus completeStatus(RMmCustomAPI::ERauEventResumed);
	TPckg<RMmCustomAPI::TRauEventStatus> completePckg(completeStatus);
	
    iMockLTSY.CompleteL(ECustomNotifyRauEventIPC, KErrNone, completePckg);        	
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeStatus, status);
	
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(completeStatus, status2);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); 

	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCRHS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadHSxPAStatus
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadHSxPAStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadHSxPAStatus0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	RMmCustomAPI::THSxPAStatus paStatus(RMmCustomAPI::EHSxPADisabled);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomReadHSxPAStatusIPC, KErrNotSupported);

	customAPI.ReadHSxPAStatus(requestStatus, paStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomReadHSxPAStatusIPC);

	customAPI.ReadHSxPAStatus(requestStatus, paStatus);

	RMmCustomAPI::THSxPAStatus paStatusComplete(RMmCustomAPI::EHSxPAEnabled);
	TPckg<RMmCustomAPI::THSxPAStatus> paStatusCompletePckg(paStatusComplete);
	
    iMockLTSY.CompleteL(ECustomReadHSxPAStatusIPC, KErrGeneral, paStatusCompletePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::ReadHSxPAStatus when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomReadHSxPAStatusIPC);

	customAPI.ReadHSxPAStatus(requestStatus, paStatus);

    iMockLTSY.CompleteL(ECustomReadHSxPAStatusIPC, KErrNone, paStatusCompletePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(paStatusComplete, paStatus);

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C2: read another value
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomReadHSxPAStatusIPC);

	customAPI.ReadHSxPAStatus(requestStatus, paStatus);

	paStatusComplete = RMmCustomAPI::EHSxPADisabled;

    iMockLTSY.CompleteL(ECustomReadHSxPAStatusIPC, KErrNone, paStatusCompletePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(paStatusComplete, paStatus);

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::ReadHSxPAStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomReadHSxPAStatusIPC, KErrNone, paStatusCompletePckg);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	
	
	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRHS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::ReadHSxPAStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::ReadHSxPAStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadHSxPAStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	RMmCustomAPI::THSxPAStatus paStatus;

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::ReadHSxPAStatus
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomReadHSxPAStatusIPC);

	customAPI.ReadHSxPAStatus(requestStatus, paStatus);
	
	customAPI.CancelAsyncRequest(ECustomReadHSxPAStatusIPC);

	RMmCustomAPI::THSxPAStatus paStatusComplete(RMmCustomAPI::EHSxPAEnabled);
	TPckg<RMmCustomAPI::THSxPAStatus> paStatusCompletePckg(paStatusComplete);
	
    iMockLTSY.CompleteL(ECustomReadHSxPAStatusIPC, KErrNone, paStatusCompletePckg);        	
 	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(2);
		
	}




/**
@SYMTestCaseID BA-CTSY-CIPC-MCRHS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::ReadHSxPAStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::ReadHSxPAStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadHSxPAStatus0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	RMmCustomAPI::THSxPAStatus paStatus;
	RMmCustomAPI::THSxPAStatus paStatus2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::ReadHSxPAStatus
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomReadHSxPAStatusIPC);
    
	customAPI.ReadHSxPAStatus(requestStatus, paStatus);
	customAPI2.ReadHSxPAStatus(requestStatus2, paStatus2);

	RMmCustomAPI::THSxPAStatus paStatusComplete(RMmCustomAPI::EHSxPAEnabled);
	TPckg<RMmCustomAPI::THSxPAStatus> paStatusCompletePckg(paStatusComplete);
	
    iMockLTSY.CompleteL(ECustomReadHSxPAStatusIPC, KErrNone, paStatusCompletePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(paStatusComplete, paStatus);

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCRHS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ReadHSxPAStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadHSxPAStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestReadHSxPAStatus0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	RMmCustomAPI::THSxPAStatus paStatus;

    iMockLTSY.ExpectL(ECustomReadHSxPAStatusIPC);

	customAPI.ReadHSxPAStatus(requestStatus, paStatus);
	 	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(2);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCWHS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::WriteHSxPAStatus
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::WriteHSxPAStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteHSxPAStatus0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	RMmCustomAPI::THSxPAStatus paStatus(RMmCustomAPI::EHSxPADisabled);

   	TMockLtsyData1< RMmCustomAPI::THSxPAStatus > tsyData(paStatus);
   	tsyData.SerialiseL(data);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomWriteHSxPAStatusIPC, data, KErrNotSupported);

	customAPI.WriteHSxPAStatus(requestStatus, paStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomWriteHSxPAStatusIPC, data);

	customAPI.WriteHSxPAStatus(requestStatus, paStatus);
	
    iMockLTSY.CompleteL(ECustomWriteHSxPAStatusIPC, KErrGeneral);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::WriteHSxPAStatus when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomWriteHSxPAStatusIPC, data);        	

	customAPI.WriteHSxPAStatus(requestStatus, paStatus);
	
    iMockLTSY.CompleteL(ECustomWriteHSxPAStatusIPC, KErrNone);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::WriteHSxPAStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomWriteHSxPAStatusIPC, KErrNone);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	
	
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCWHS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::WriteHSxPAStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::WriteHSxPAStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteHSxPAStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	RMmCustomAPI::THSxPAStatus paStatus(RMmCustomAPI::EHSxPADisabled);

   	TMockLtsyData1< RMmCustomAPI::THSxPAStatus > tsyData(paStatus);
   	tsyData.SerialiseL(data);


	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::WriteHSxPAStatus
 	//-------------------------------------------------------------------------
    iMockLTSY.ExpectL(ECustomWriteHSxPAStatusIPC, data);        	

	customAPI.WriteHSxPAStatus(requestStatus, paStatus);
	
	customAPI.CancelAsyncRequest(ECustomWriteHSxPAStatusIPC);
	
    iMockLTSY.CompleteL(ECustomWriteHSxPAStatusIPC, KErrNone);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	
	CleanupStack::PopAndDestroy(3);
	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCWHS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::WriteHSxPAStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::WriteHSxPAStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteHSxPAStatus0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RBuf8 data;
	CleanupClosePushL(data);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	CleanupClosePushL(customAPI2);
	customAPI2.Open(phone2);	

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	RMmCustomAPI::THSxPAStatus paStatus(RMmCustomAPI::EHSxPADisabled);

   	TMockLtsyData1< RMmCustomAPI::THSxPAStatus > tsyData(paStatus);
   	tsyData.SerialiseL(data);

	RMmCustomAPI::THSxPAStatus paStatus2(RMmCustomAPI::EHSxPAEnabled);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::WriteHSxPAStatus
 	//-------------------------------------------------------------------------
	
    iMockLTSY.ExpectL(ECustomWriteHSxPAStatusIPC, data);        	

	customAPI.WriteHSxPAStatus(requestStatus, paStatus);
	customAPI2.WriteHSxPAStatus(requestStatus2, paStatus2);
	
    iMockLTSY.CompleteL(ECustomWriteHSxPAStatusIPC, KErrNone);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCWHS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::WriteHSxPAStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::WriteHSxPAStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestWriteHSxPAStatus0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	RMmCustomAPI::THSxPAStatus paStatus(RMmCustomAPI::EHSxPADisabled);

   	TMockLtsyData1< RMmCustomAPI::THSxPAStatus > tsyData(paStatus);
   	tsyData.SerialiseL(data);

    iMockLTSY.ExpectL(ECustomWriteHSxPAStatusIPC, data);        	

	customAPI.WriteHSxPAStatus(requestStatus, paStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();
 	
	CleanupStack::PopAndDestroy(3);
	

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNHS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyHSxPAStatus
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyHSxPAStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyHSxPAStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyHSxPAStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	RMmCustomAPI::THSxPAStatus paCompleteStatus(RMmCustomAPI::EHSxPAEnabled);
	TPckg<RMmCustomAPI::THSxPAStatus> completePckg(paCompleteStatus);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomNotifyHSxPAStatusIPC, KErrNone, completePckg);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	RMmCustomAPI::THSxPAStatus paStatus(RMmCustomAPI::EHSxPADisabled);

	customAPI.NotifyHSxPAStatus(requestStatus, paStatus);
	
    iMockLTSY.CompleteL(ECustomNotifyHSxPAStatusIPC, KErrGeneral, completePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyHSxPAStatus when result is not cached.
 	//-------------------------------------------------------------------------

	customAPI.NotifyHSxPAStatus(requestStatus, paStatus);
	
	paCompleteStatus = RMmCustomAPI::EHSxPAEnabled;

    iMockLTSY.CompleteL(ECustomNotifyHSxPAStatusIPC, KErrNone, completePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(paCompleteStatus, paStatus);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNHS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifyHSxPAStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifyHSxPAStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyHSxPAStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifyHSxPAStatus
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	RMmCustomAPI::THSxPAStatus paStatus;

	customAPI.NotifyHSxPAStatus(requestStatus, paStatus);
	
	customAPI.CancelAsyncRequest(ECustomNotifyHSxPAStatusIPC);
	
	RMmCustomAPI::THSxPAStatus paCompleteStatus(RMmCustomAPI::EHSxPADisabled);
	TPckg<RMmCustomAPI::THSxPAStatus> completePckg(paCompleteStatus);
	
    iMockLTSY.CompleteL(ECustomNotifyHSxPAStatusIPC, KErrNone, completePckg);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); 
	
	}




/**
@SYMTestCaseID BA-CTSY-CIPC-MCNHS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifyHSxPAStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifyHSxPAStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyHSxPAStatus0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	CleanupClosePushL(customAPI2);
	customAPI2.Open(phone2);	

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	RMmCustomAPI::THSxPAStatus paStatus(RMmCustomAPI::EHSxPADisabled);
	RMmCustomAPI::THSxPAStatus paStatus2(RMmCustomAPI::EHSxPADisabled);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifyHSxPAStatus
 	//-------------------------------------------------------------------------

	customAPI.NotifyHSxPAStatus(requestStatus, paStatus);
	customAPI2.NotifyHSxPAStatus(requestStatus2, paStatus2);

	RMmCustomAPI::THSxPAStatus paCompleteStatus(RMmCustomAPI::EHSxPAEnabled);
	TPckg<RMmCustomAPI::THSxPAStatus> completePckg(paCompleteStatus);
	
    iMockLTSY.CompleteL(ECustomNotifyHSxPAStatusIPC, KErrNone, completePckg);        	
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(paCompleteStatus, paStatus);
	
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(paCompleteStatus, paStatus2);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); 

	}

/**
@SYMTestCaseID BA-CTSY-CIPC-MCGICFIS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetIccCallForwardingIndicatorStatus
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetIccCallForwardingIndicatorStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetIccCallForwardingIndicatorStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;

	RMmCustomAPI::TCFIndicators indi;
	RMmCustomAPI::TCFIndicatorsPckg indiPckg(indi);	
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);    

	customAPI.GetIccCallForwardingIndicatorStatus(requestStatus, indiPckg);

	RMmCustomAPI::TCFIndicators indiComplete;
	RMmCustomAPI::TCFIndicatorsPckg indiCompletePckg(indiComplete);	

	_LIT(KNumber, "123456789");
    indiComplete.iIndicator = 0xFF;
    indiComplete.iCFNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;    
    indiComplete.iCFNumber.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    indiComplete.iCFNumber.iTelNumber = KNumber;

    iMockLTSY.CompleteL(ECustomGetIccCallForwardingStatusIPC, KErrGeneral, indiCompletePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetIccCallForwardingIndicatorStatus when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);    

	customAPI.GetIccCallForwardingIndicatorStatus(requestStatus, indiPckg);

    iMockLTSY.CompleteL(ECustomGetIccCallForwardingStatusIPC, KErrNone, indiCompletePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(indiComplete.iIndicator, indi.iIndicator);
	ASSERT_EQUALS(indiComplete.iCFNumber.iTypeOfNumber, indi.iCFNumber.iTypeOfNumber);
	ASSERT_EQUALS(indiComplete.iCFNumber.iNumberPlan, indi.iCFNumber.iNumberPlan);
	ASSERT_EQUALS(0, indi.iCFNumber.iTelNumber.Compare(indiComplete.iCFNumber.iTelNumber));		
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetIccCallForwardingIndicatorStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	// here iGetIccCfStatusBootUp is true and error is set
    iMockLTSY.CompleteL(ECustomGetIccCallForwardingStatusIPC, KErrGeneral, indiCompletePckg);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


	// TEST E2: Unsolicited completion of RMmCustomAPI::GetIccCallForwardingIndicatorStatus
	// from LTSY.
	
	// here iGetIccCfStatusBootUp is true and no error 
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomGetIccCallForwardingStatusIPC, KErrNone, indiCompletePckg);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// TEST E3: Unsolicited completion of RMmCustomAPI::GetIccCallForwardingIndicatorStatus
	// from LTSY.
	
	// here iGetIccCfStatusBootUp is false
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL(ECustomGetIccCallForwardingStatusIPC, KErrNone, indiCompletePckg);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	
    iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC, KErrNotSupported);        	

	customAPI.GetIccCallForwardingIndicatorStatus(requestStatus, indiPckg);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGICFIS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetIccCallForwardingIndicatorStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetIccCallForwardingIndicatorStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetIccCallForwardingIndicatorStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    // This completes request made when customAPI is opened
	RMmCustomAPI::TCFIndicators indiComplete;
	RMmCustomAPI::TCFIndicatorsPckg indiCompletePckg(indiComplete);	
    iMockLTSY.CompleteL(ECustomGetIccCallForwardingStatusIPC, KErrNone, indiCompletePckg); 

	TRequestStatus requestStatus;

	RMmCustomAPI::TCFIndicators indi;
	RMmCustomAPI::TCFIndicatorsPckg indiPckg(indi);	

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetIccCallForwardingIndicatorStatus
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);    

	customAPI.GetIccCallForwardingIndicatorStatus(requestStatus, indiPckg);

	customAPI.CancelAsyncRequest(ECustomGetIccCallForwardingStatusIPC);

    iMockLTSY.CompleteL(ECustomGetIccCallForwardingStatusIPC, KErrNone, indiCompletePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
    
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGICFIS-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetIccCallForwardingIndicatorStatus with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetIccCallForwardingIndicatorStatus with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetIccCallForwardingIndicatorStatus0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);


	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMmCustomAPI::GetIccCallForwardingIndicatorStatus
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;

	TBuf8<1> littleBuf;

	customAPI.GetIccCallForwardingIndicatorStatus(requestStatus, littleBuf);

	RMmCustomAPI::TCFIndicators indiComplete;
	RMmCustomAPI::TCFIndicatorsPckg indiCompletePckg(indiComplete);	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this);

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGICFIS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetIccCallForwardingIndicatorStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetIccCallForwardingIndicatorStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetIccCallForwardingIndicatorStatus0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	CleanupClosePushL(customAPI2);
	customAPI2.Open(phone2);	

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	RMmCustomAPI::TCFIndicators indi;
	RMmCustomAPI::TCFIndicatorsPckg indiPckg(indi);	

	RMmCustomAPI::TCFIndicators indi2;
	RMmCustomAPI::TCFIndicatorsPckg indiPckg2(indi2);	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetIccCallForwardingIndicatorStatus
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);    

	customAPI.GetIccCallForwardingIndicatorStatus(requestStatus, indiPckg);
	customAPI2.GetIccCallForwardingIndicatorStatus(requestStatus2, indiPckg2);

	RMmCustomAPI::TCFIndicators indiComplete;
	RMmCustomAPI::TCFIndicatorsPckg indiCompletePckg(indiComplete);	

    iMockLTSY.CompleteL(ECustomGetIccCallForwardingStatusIPC, KErrNone, indiCompletePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(5, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGICFIS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetIccCallForwardingIndicatorStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetIccCallForwardingIndicatorStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetIccCallForwardingIndicatorStatus0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;

	RMmCustomAPI::TCFIndicators indi;
	RMmCustomAPI::TCFIndicatorsPckg indiPckg(indi);	
	
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);    

	customAPI.GetIccCallForwardingIndicatorStatus(requestStatus, indiPckg);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this);   // customAPI, this

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNICFSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyIccCallForwardingStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyIccCallForwardingStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyIccCallForwardingStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;

	RMmCustomAPI::TCFIndicators indi;
	RMmCustomAPI::TCFIndicatorsPckg indiPckg(indi);	
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	customAPI.NotifyIccCallForwardingStatusChange(requestStatus, indiPckg);

	RMmCustomAPI::TCFIndicators indiComplete;
	RMmCustomAPI::TCFIndicatorsPckg indiCompletePckg(indiComplete);	


    indiComplete.iIndicator = 0xFF;

    iMockLTSY.CompleteL(ECustomNotifyIccCallForwardingStatusChangeIPC, KErrGeneral, indiCompletePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyIccCallForwardingStatusChange when result is not cached.
 	//-------------------------------------------------------------------------

	customAPI.NotifyIccCallForwardingStatusChange(requestStatus, indiPckg);

	_LIT(KNumber, "123456789");
    indiComplete.iIndicator = 0x00;
    indiComplete.iCFNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;    
    indiComplete.iCFNumber.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    indiComplete.iCFNumber.iTelNumber = KNumber;

    iMockLTSY.CompleteL(ECustomNotifyIccCallForwardingStatusChangeIPC, KErrNone, indiCompletePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(indiComplete.iIndicator, indi.iIndicator);
	ASSERT_EQUALS(indiComplete.iCFNumber.iTypeOfNumber, indi.iCFNumber.iTypeOfNumber);
	ASSERT_EQUALS(indiComplete.iCFNumber.iNumberPlan, indi.iCFNumber.iNumberPlan);
	ASSERT_EQUALS(0, indi.iCFNumber.iTelNumber.Compare(indiComplete.iCFNumber.iTelNumber));		
	
	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyIccCallForwardingStatusChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    indiComplete.iIndicator = 0xFF;
    
    iMockLTSY.CompleteL(ECustomNotifyIccCallForwardingStatusChangeIPC, KErrNone, indiCompletePckg);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNICFSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifyIccCallForwardingStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifyIccCallForwardingStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyIccCallForwardingStatusChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	TRequestStatus requestStatus;

	RMmCustomAPI::TCFIndicators indi;
	RMmCustomAPI::TCFIndicatorsPckg indiPckg(indi);	
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifyIccCallForwardingStatusChange
 	//-------------------------------------------------------------------------
	customAPI.NotifyIccCallForwardingStatusChange(requestStatus, indiPckg);
	
	customAPI.CancelAsyncRequest(ECustomNotifyIccCallForwardingStatusChangeIPC);

	RMmCustomAPI::TCFIndicators indiComplete;
	RMmCustomAPI::TCFIndicatorsPckg indiCompletePckg(indiComplete);	

    indiComplete.iIndicator = 0xFF;

    iMockLTSY.CompleteL(ECustomNotifyIccCallForwardingStatusChangeIPC, KErrNone, indiCompletePckg);        	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	
	
	AssertMockLtsyStatusL();	
	
	CleanupStack::PopAndDestroy(2); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNICFSC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyIccCallForwardingStatusChange with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyIccCallForwardingStatusChange with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyIccCallForwardingStatusChange0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMmCustomAPI::NotifyIccCallForwardingStatusChange
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;

	TBuf8<1> littleBuf;
	
	customAPI.NotifyIccCallForwardingStatusChange(requestStatus, littleBuf);

	RMmCustomAPI::TCFIndicators indiComplete;
	RMmCustomAPI::TCFIndicatorsPckg indiCompletePckg(indiComplete);	

    indiComplete.iIndicator = 0xFF;

    iMockLTSY.CompleteL(ECustomNotifyIccCallForwardingStatusChangeIPC, KErrNone, indiCompletePckg);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNICFSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifyIccCallForwardingStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifyIccCallForwardingStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyIccCallForwardingStatusChange0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	RMmCustomAPI::TCFIndicators indi;
	RMmCustomAPI::TCFIndicatorsPckg indiPckg(indi);	

	RMmCustomAPI::TCFIndicators indi2;
	RMmCustomAPI::TCFIndicatorsPckg indiPckg2(indi2);	
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::NotifyIccCallForwardingStatusChange
 	//-------------------------------------------------------------------------
	customAPI.NotifyIccCallForwardingStatusChange(requestStatus, indiPckg);
	customAPI2.NotifyIccCallForwardingStatusChange(requestStatus2, indiPckg2);

	RMmCustomAPI::TCFIndicators indiComplete;
	RMmCustomAPI::TCFIndicatorsPckg indiCompletePckg(indiComplete);	

	_LIT(KNumber, "123456789");

    indiComplete.iIndicator = 0xFF;
    indiComplete.iCFNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;    
    indiComplete.iCFNumber.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    indiComplete.iCFNumber.iTelNumber = KNumber;

    iMockLTSY.CompleteL(ECustomNotifyIccCallForwardingStatusChangeIPC, KErrNone, indiCompletePckg);        	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(indiComplete.iIndicator, indi.iIndicator);
	ASSERT_EQUALS(indiComplete.iCFNumber.iTypeOfNumber, indi.iCFNumber.iTypeOfNumber);
	ASSERT_EQUALS(indiComplete.iCFNumber.iNumberPlan, indi.iCFNumber.iNumberPlan);
	ASSERT_EQUALS(0, indi.iCFNumber.iTelNumber.Compare(indiComplete.iCFNumber.iTelNumber));		

	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(indiComplete.iIndicator, indi2.iIndicator);
	ASSERT_EQUALS(indiComplete.iCFNumber.iTypeOfNumber, indi2.iCFNumber.iTypeOfNumber);
	ASSERT_EQUALS(indiComplete.iCFNumber.iNumberPlan, indi2.iCFNumber.iNumberPlan);
	ASSERT_EQUALS(0, indi2.iCFNumber.iTelNumber.Compare(indiComplete.iCFNumber.iTelNumber));		
	
	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(5, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCI2-0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetCellInfo
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetCellInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/

void CCTsyCustomIPCFU::TestGetCellInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	RMmCustomAPI::TMmCellInfo* cellInfo = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cellInfo);
	
	RMmCustomAPI::TMmCellInfoPckg cellInfoPckg(*cellInfo);

	RMmCustomAPI::TMmCellInfo* cell = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cell);
	RMmCustomAPI::TMmCellInfoPckg cellPckg(*cell);
	
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomGetCellInfoIPC, KErrNotSupported);        	

	customAPI.GetCellInfo(requestStatus, cellInfoPckg);

	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomGetCellInfoIPC);        	

	customAPI.GetCellInfo(requestStatus, cellInfoPckg);

	iMockLTSY.CompleteL(ECustomGetCellInfoIPC, KErrGeneral, cellPckg);

	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetCellInfo when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(ECustomGetCellInfoIPC);        	

	customAPI.GetCellInfo(requestStatus, cellInfoPckg);

	FillWithRandomData(*cell);	
	
	iMockLTSY.CompleteL(ECustomGetCellInfoIPC, KErrNone, cellPckg);

	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( TComparator<RMmCustomAPI::TMmCellInfo>::IsEqual(*cell, *cellInfo) );
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetCellInfo
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.CompleteL(ECustomGetCellInfoIPC, KErrNone, cellPckg);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCI2-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetCellInfo
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetCellInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCellInfo0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetCellInfo
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	RMmCustomAPI::TMmCellInfo* cellInfo = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cellInfo);
	RMmCustomAPI::TMmCellInfoPckg cellInfoPckg(*cellInfo);

	RMmCustomAPI::TMmCellInfo* cell = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cell);
	RMmCustomAPI::TMmCellInfoPckg cellPckg(*cell);	
	
    iMockLTSY.ExpectL(ECustomGetCellInfoIPC);        	

	customAPI.GetCellInfo(requestStatus, cellInfoPckg);

	customAPI.CancelAsyncRequest(ECustomGetCellInfoIPC);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.CompleteL(ECustomGetCellInfoIPC, KErrNone, cellPckg);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
			
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCI2-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetCellInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetCellInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCellInfo0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMmCustomAPI::GetCellInfo
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;
	TBuf8<1> littleBuf;

	customAPI.GetCellInfo(requestStatus, littleBuf);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this);   // customAPI, this

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCI2-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetCellInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetCellInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCellInfo0004L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	RMmCustomAPI::TMmCellInfo* cellInfo = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cellInfo);
	RMmCustomAPI::TMmCellInfoPckg cellInfoPckg(*cellInfo);
	RMmCustomAPI::TMmCellInfo* cellInfo2 = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cellInfo2);
	RMmCustomAPI::TMmCellInfoPckg cellInfoPckg2(*cellInfo2);
	
    iMockLTSY.ExpectL(ECustomGetCellInfoIPC);        		

	customAPI.GetCellInfo(requestStatus, cellInfoPckg);
	customAPI2.GetCellInfo(requestStatus2, cellInfoPckg2);
	
	RMmCustomAPI::TMmCellInfo* cell = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cell);
	RMmCustomAPI::TMmCellInfoPckg cellPckg(*cell);

	iMockLTSY.CompleteL(ECustomGetCellInfoIPC, KErrNone, cellPckg);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(8, this);
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGCI2-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetCellInfo with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetCellInfo and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetCellInfo0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus requestStatus;
	RMmCustomAPI::TMmCellInfo* cellInfo = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cellInfo);
	RMmCustomAPI::TMmCellInfoPckg cellInfoPckg(*cellInfo);
	
    iMockLTSY.ExpectL(ECustomGetCellInfoIPC);        	

	customAPI.GetCellInfo(requestStatus, cellInfoPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	
		
	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(3); 
	}



void CCTsyCustomIPCFU::FillWithRandomData( RMmCustomAPI::TMmCellInfo &aCell, TInt aRnd) const
	{
	
    aCell.iMode = RMmCustomAPI::TMmCellInfo::EGSM;
    aCell.iGsmCellInfo.iTA = 100;
    for( TInt i = 0; i <KMaxNmrAmount; i++ )
        {
        aCell.iGsmCellInfo.iNmr[i].iARFCN = 3 * i + aRnd;
        aCell.iGsmCellInfo.iNmr[i].iBSIC = 3 * i + 1 + aRnd;
        aCell.iGsmCellInfo.iNmr[i].iRxLEV = 3 * i + 2 + aRnd;
        }

    aCell.iWcdmaCellInfo.iFrequencyInfo.iFddUL = 1 + aRnd;
    aCell.iWcdmaCellInfo.iFrequencyInfo.iFddDL = 2 + aRnd;
    aCell.iWcdmaCellInfo.iFrequencyInfo.iTddNt = 3 + aRnd;
    aCell.iWcdmaCellInfo.iPrimaryScrambilingCode = 4 + aRnd;

    for( TInt j = 0; j <KMaxNetworkMeasureReports; j++ )
        {
        aCell.iWcdmaCellInfo.iNwkMeasureReport[j].iCarrierRSSI = 5 + aRnd;

        for( TInt k = 0; k <KMaxCellMeasuredResults; k++ )
            {
            aCell.iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].iCID = k + aRnd;
            aCell.iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].iFddInfo.iPrimaryCPICH = k + aRnd;
            aCell.iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].iFddInfo.iCpichEcN0 = k + aRnd;
            aCell.iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].iFddInfo.iCpichRscp = k + aRnd;
            aCell.iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].iFddInfo.iPathloss = k + aRnd;
            aCell.iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].iTddInfo.iCellParamID = k + aRnd;
            aCell.iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].iTddInfo.iProposedTGSN = k + aRnd;
            aCell.iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].iTddInfo.iPrimaryCcpchRscp = k + aRnd;
            aCell.iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].iTddInfo.iPathloss = k + aRnd;
            for( TInt l = 0; l <KMaxTimeSlotIscpAmount; l++ )
                {
                aCell.iWcdmaCellInfo.iNwkMeasureReport[j].iCellMeasuredResult[k].iTddInfo.iTimeslotISCP[l] = l + aRnd;
                }
            }
        }	
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-MCNCIC2-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyCellInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyCellInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyCellInfoChange0001L()
	{
 
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);


	RMmCustomAPI::TMmCellInfo* cell = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cell);
	RMmCustomAPI::TMmCellInfoPckg cellPckg(*cell);

	// RMmCustomAPI::TMmCellInfo initialization with random data
    FillWithRandomData(*cell);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::NotifyCellInfoChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.CompleteL(ECustomNotifyCellInfoChangeIPC, KErrNone, cellPckg);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	RMmCustomAPI::TMmCellInfo* cellInfo = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cellInfo);
	RMmCustomAPI::TMmCellInfoPckg cellInfoPckg(*cellInfo);
	
	customAPI.NotifyCellInfoChange(requestStatus, cellInfoPckg);

	iMockLTSY.CompleteL(ECustomNotifyCellInfoChangeIPC, KErrGeneral, cellPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::NotifyCellInfoChange when result is not cached.
 	//-------------------------------------------------------------------------

	customAPI.NotifyCellInfoChange(requestStatus, cellInfoPckg);

	iMockLTSY.CompleteL(ECustomNotifyCellInfoChangeIPC, KErrNone, cellPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_TRUE( TComparator<RMmCustomAPI::TMmCellInfo>::IsEqual(*cell, *cellInfo) );

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNCIC2-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::NotifyCellInfoChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::NotifyCellInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyCellInfoChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::NotifyCellInfoChange
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	RMmCustomAPI::TMmCellInfo* cellInfo = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cellInfo);
	RMmCustomAPI::TMmCellInfoPckg cellInfoPckg(*cellInfo);
	
	customAPI.NotifyCellInfoChange(requestStatus, cellInfoPckg);
	
	customAPI.CancelAsyncRequest(ECustomNotifyCellInfoChangeIPC);
	
	RMmCustomAPI::TMmCellInfo* cell = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cell);
	RMmCustomAPI::TMmCellInfoPckg cellPckg(*cell);	

	iMockLTSY.CompleteL(ECustomNotifyCellInfoChangeIPC, KErrNone, cellPckg, 10);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	
 	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNCIC2-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifyCellInfoChange with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifyCellInfoChange with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyCellInfoChange0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMmCustomAPI::NotifyCellInfoChange
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;
	TBuf8<1> littleBuf;
	
	customAPI.NotifyCellInfoChange(requestStatus, littleBuf);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this);    // customAPI, this

	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCNCIC2-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::NotifyCellInfoChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::NotifyCellInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestNotifyCellInfoChange0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	RMmCustomAPI::TMmCellInfo* cellInfo = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cellInfo);
	RMmCustomAPI::TMmCellInfoPckg cellInfoPckg(*cellInfo);
	RMmCustomAPI::TMmCellInfo* cellInfo2 = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cellInfo2);
	RMmCustomAPI::TMmCellInfoPckg cellInfoPckg2(*cellInfo2);
	
	customAPI.NotifyCellInfoChange(requestStatus, cellInfoPckg);
	customAPI2.NotifyCellInfoChange(requestStatus2, cellInfoPckg2);
	
	RMmCustomAPI::TMmCellInfo* cell = new(ELeave) RMmCustomAPI::TMmCellInfo;
    CleanupStack::PushL(cell);
	RMmCustomAPI::TMmCellInfoPckg cellPckg(*cell);

	FillWithRandomData(*cell);

	iMockLTSY.CompleteL(ECustomNotifyCellInfoChangeIPC, KErrNone, cellPckg);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_TRUE( TComparator<RMmCustomAPI::TMmCellInfo>::IsEqual(*cell, *cellInfo) );

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	
	ASSERT_TRUE( TComparator<RMmCustomAPI::TMmCellInfo>::IsEqual(*cell, *cellInfo2) );

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(8, this);

	}

/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSNB-0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetSystemNetworkBand
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetSystemNetworkBand
@SYMTestExpectedResults Pass
@SYMTestType CT
*/

void CCTsyCustomIPCFU::TestGetSystemNetworkBand0001L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TBandSelection band;
    RMmCustomAPI::TNetworkModeCaps mode;
     	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetBandSelectionIPC, KErrNotSupported);
    
    customAPI.GetSystemNetworkBand(requestStatus, band, mode);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetBandSelectionIPC);
		
    RMmCustomAPI::TBandSelection compBand = RMmCustomAPI::ENetworkBandUmts850;
    RMmCustomAPI::TNetworkModeCaps compMode = RMmCustomAPI::KCapsNetworkModeGsm;
	
	TMockLtsyData2 <RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> 
															ltsyData(compBand, compMode);
	ltsyData.SerialiseL(completeData);	
	
	iMockLTSY.CompleteL(ECustomGetBandSelectionIPC, KErrGeneral, completeData);

    customAPI.GetSystemNetworkBand(requestStatus, band, mode);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::GetSystemNetworkBand when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetBandSelectionIPC);
    iMockLTSY.CompleteL( ECustomGetBandSelectionIPC, KErrNone, completeData);

    customAPI.GetSystemNetworkBand(requestStatus, band, mode);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_TRUE(compBand == band);
	ASSERT_TRUE(compMode == mode);
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::GetSystemNetworkBand
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomGetBandSelectionIPC, KErrNone, completeData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // customAPI, completeData, this
		
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSNB-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::GetSystemNetworkBand
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::GetSystemNetworkBand
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetSystemNetworkBand0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TBandSelection band;
    RMmCustomAPI::TNetworkModeCaps mode;


 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::GetSystemNetworkBand
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomGetBandSelectionIPC);
    

    RMmCustomAPI::TBandSelection compBand = RMmCustomAPI::ENetworkBandUmts850;
    RMmCustomAPI::TNetworkModeCaps compMode = RMmCustomAPI::KCapsNetworkModeGsm;
	
	TMockLtsyData2 <RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> 
															ltsyData(compBand, compMode);
	ltsyData.SerialiseL(completeData);	
	
    customAPI.GetSystemNetworkBand(requestStatus, band, mode);
	
	customAPI.CancelAsyncRequest(ECustomGetBandSelectionIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomGetBandSelectionIPC, KErrNone, completeData);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // customAPI, completeData, this
		
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSNB-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::GetSystemNetworkBand
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::GetSystemNetworkBand
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetSystemNetworkBand0004L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    RMmCustomAPI::TBandSelection band;
    RMmCustomAPI::TNetworkModeCaps mode;

    RMmCustomAPI::TBandSelection band2;
    RMmCustomAPI::TNetworkModeCaps mode2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetSystemNetworkBand
 	//-------------------------------------------------------------------------

    RMmCustomAPI::TBandSelection compBand = RMmCustomAPI::ENetworkBandUmts850;
    RMmCustomAPI::TNetworkModeCaps compMode = RMmCustomAPI::KCapsNetworkModeGsm;
	
	TMockLtsyData2 <RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> 
															ltsyData(compBand, compMode);
	ltsyData.SerialiseL(completeData);	

    iMockLTSY.ExpectL( ECustomGetBandSelectionIPC);
    iMockLTSY.CompleteL( ECustomGetBandSelectionIPC, KErrNone, completeData, 20);

    customAPI.GetSystemNetworkBand(requestStatus, band, mode);

    customAPI2.GetSystemNetworkBand(requestStatus2, band2, mode2);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	ASSERT_TRUE(compBand == band);
	ASSERT_TRUE(compMode == mode);
		    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this);
	
	}
	

/**
@SYMTestCaseID BA-CTSY-CIPC-MCGSNB-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetSystemNetworkBand with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetSystemNetworkBand and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetSystemNetworkBand0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TBandSelection band;
    RMmCustomAPI::TNetworkModeCaps mode;

    iMockLTSY.ExpectL( ECustomGetBandSelectionIPC);
    
    customAPI.GetSystemNetworkBand(requestStatus, band, mode);
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // customAPI, this
	
	}




/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSNB-0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SetSystemNetworkBand
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetSystemNetworkBand
@SYMTestExpectedResults Pass
@SYMTestType CT
*/

void CCTsyCustomIPCFU::TestSetSystemNetworkBand0001L()
	{
		
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;

    RMmCustomAPI::TBandSelection band = RMmCustomAPI::ENetworkBandUmts850;
    RMmCustomAPI::TNetworkModeCaps mode = RMmCustomAPI::KCapsNetworkModeGsm;
	
	TMockLtsyData2 <RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> 
															ltsyData(band, mode);
	ltsyData.SerialiseL(data);	
     	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSetBandSelectionIPC, data, KErrNotSupported);
    
    customAPI.SetSystemNetworkBand(requestStatus, band, mode);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSetBandSelectionIPC, data);
		
	iMockLTSY.CompleteL(ECustomSetBandSelectionIPC, KErrGeneral);

    customAPI.SetSystemNetworkBand(requestStatus, band, mode);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::SetSystemNetworkBand when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSetBandSelectionIPC, data);
    iMockLTSY.CompleteL( ECustomSetBandSelectionIPC, KErrNone);

    customAPI.SetSystemNetworkBand(requestStatus, band, mode);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::SetSystemNetworkBand
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.CompleteL( ECustomSetBandSelectionIPC, KErrNone);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 			
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSNB-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMmCustomAPI::SetSystemNetworkBand
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMmCustomAPI::SetSystemNetworkBand
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetSystemNetworkBand0002L()
	{
	//Test works on 9.6 but causes this step and rest of the suite to fail     
	//ASSERT_TRUE(EFalse);
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TBandSelection band = RMmCustomAPI::ENetworkBandUmts850;
    RMmCustomAPI::TNetworkModeCaps mode = RMmCustomAPI::KCapsNetworkModeGsm;
	
	TMockLtsyData2 <RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> 
															ltsyData(band, mode);
	ltsyData.SerialiseL(data);	

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMmCustomAPI::SetSystemNetworkBand
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSetBandSelectionIPC, data);
    iMockLTSY.CompleteL( ECustomSetBandSelectionIPC, KErrNone);
    
    customAPI.SetSystemNetworkBand(requestStatus, band, mode);
	
	customAPI.CancelAsyncRequest(ECustomSetBandSelectionIPC);

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); 		
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSNB-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMmCustomAPI::SetSystemNetworkBand
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMmCustomAPI::SetSystemNetworkBand
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetSystemNetworkBand0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

    RMmCustomAPI::TBandSelection band = RMmCustomAPI::ENetworkBandUmts850;
    RMmCustomAPI::TNetworkModeCaps mode = RMmCustomAPI::KCapsNetworkModeGsm;
	
	TMockLtsyData2 <RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> 
															ltsyData(band, mode);
	ltsyData.SerialiseL(data);	

    RMmCustomAPI::TBandSelection band2 = RMmCustomAPI::ENetworkBandUmts850;
    RMmCustomAPI::TNetworkModeCaps mode2 = RMmCustomAPI::KCapsNetworkModeGsm;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMmCustomAPI::GetSystemNetworkBand
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL( ECustomSetBandSelectionIPC, data);
    iMockLTSY.CompleteL( ECustomSetBandSelectionIPC, KErrNone);

    customAPI.SetSystemNetworkBand(requestStatus, band, mode);

    customAPI2.SetSystemNetworkBand(requestStatus2, band2, mode2);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this);	
	}
	

/**
@SYMTestCaseID BA-CTSY-CIPC-MCSSNB-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SetSystemNetworkBand with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetSystemNetworkBand and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestSetSystemNetworkBand0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

    TRequestStatus requestStatus;
    RMmCustomAPI::TBandSelection band = RMmCustomAPI::ENetworkBandUmts850;
    RMmCustomAPI::TNetworkModeCaps mode = RMmCustomAPI::KCapsNetworkModeGsm;
	
	TMockLtsyData2 <RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> 
															ltsyData(band, mode);
	ltsyData.SerialiseL(data);	

    iMockLTSY.ExpectL( ECustomSetBandSelectionIPC, data);
    
    customAPI.SetSystemNetworkBand(requestStatus, band, mode);

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 407702);
	// RMmCustomAPI::SetSystemNetworkBand is not completed by timeout.
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); 	
		
	}



/**
@SYMTestCaseID BA-CTSY-CIPC-USS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetUSIMServiceSupport
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetUSIMServiceSupport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetUSIMServiceSupport0001L()
	{
	//Setup
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	//End Setup
		
	// Start	
	
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
	//-------------------------------------------------------------------------

	TInt appNum = 68;		// fill expected appNumber
    TPckg<TInt> appNumPckg(appNum);
	iMockLTSY.ExpectL( ECustomGetServiceTableSupportbyApplicationIPC, appNumPckg, KErrNotSupported);

	TRequestStatus requestStatus;
	RMmCustomAPI::TAppSupport usimSupport;
	usimSupport.iAppNum=appNum;
	usimSupport.iSupported=ETrue;
	customAPI.GetUSIMServiceSupport(requestStatus, usimSupport);
		
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();	
	
	// Successful call
	// Expect and Complete Calls

	iMockLTSY.ExpectL( ECustomGetServiceTableSupportbyApplicationIPC, appNumPckg);
	
    RMmCustomAPI::TAppSupport compData;
	compData.iAppNum = 68;		// fill 
	compData.iSupported = EFalse; //app is not supported
    TPckg<RMmCustomAPI::TAppSupport> compPckg(compData);
	iMockLTSY.CompleteL( ECustomGetServiceTableSupportbyApplicationIPC, KErrNone, compPckg);
	
	//Call API
	customAPI.GetUSIMServiceSupport(requestStatus, usimSupport);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(68, usimSupport.iAppNum);
	ASSERT_TRUE(EFalse == usimSupport.iSupported);
	AssertMockLtsyStatusL();		

	//Not Supported
	// Expect and Complete Calls
	iMockLTSY.ExpectL( ECustomGetServiceTableSupportbyApplicationIPC, appNumPckg);
	iMockLTSY.CompleteL( ECustomGetServiceTableSupportbyApplicationIPC, KErrNotSupported, compPckg);

	customAPI.GetUSIMServiceSupport(requestStatus, usimSupport);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	//KErrGeneral
	// Expect and Complete Calls
	iMockLTSY.ExpectL( ECustomGetServiceTableSupportbyApplicationIPC, appNumPckg);
	iMockLTSY.CompleteL( ECustomGetServiceTableSupportbyApplicationIPC, KErrGeneral, compPckg);

	customAPI.GetUSIMServiceSupport(requestStatus, usimSupport);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	//End
	CleanupStack::PopAndDestroy(2); 		//this, customAPI
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-USS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test cancel of RMmCustomAPI::GetUSIMServiceSupport
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetUSIMServiceSupport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetUSIMServiceSupport0002L()
	{
	//Setup
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	//End Setup
		
	// Start	
	// Successful call
	// Expect and Complete Calls
    TInt appNum = 68;		// fill expected appNumber 
    TPckg<TInt> appNumPckg(appNum);
	iMockLTSY.ExpectL( ECustomGetServiceTableSupportbyApplicationIPC, appNumPckg);
	
	//Call API
	TRequestStatus requestStatus;
	RMmCustomAPI::TAppSupport usimSupport;
	usimSupport.iAppNum=appNum;
	usimSupport.iSupported=ETrue;
	customAPI.GetUSIMServiceSupport(requestStatus, usimSupport);

	customAPI.CancelAsyncRequest(ECustomGetServiceTableSupportbyApplicationIPC);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	//End
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); 		//this, customAPI
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-USS-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test cancel of RMmCustomAPI::GetUSIMServiceSupport
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetUSIMServiceSupport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetUSIMServiceSupport0003L()
	{
	//Setup
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	//End Setup

	// Start	
	// Pass negative appNum
	// Expect and Complete Calls
    TInt appNum = -5;		 
    TPckg<TInt> appNumPckg(appNum);
	iMockLTSY.ExpectL( ECustomGetServiceTableSupportbyApplicationIPC, appNumPckg);
	
    RMmCustomAPI::TAppSupport compData;
	compData.iAppNum = -5;		// fill 
	compData.iSupported = EFalse; //app is not supported
    TPckg<RMmCustomAPI::TAppSupport> compPckg(compData);
	iMockLTSY.CompleteL( ECustomGetServiceTableSupportbyApplicationIPC, KErrNotFound, compPckg);

	
	//Call API
	TRequestStatus requestStatus;
	RMmCustomAPI::TAppSupport usimSupport;
	usimSupport.iAppNum=-5;
	usimSupport.iSupported=ETrue;
	customAPI.GetUSIMServiceSupport(requestStatus, usimSupport);
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNotFound, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	//End
	CleanupStack::PopAndDestroy(2); 		//this, customAPI
	}

/**
@SYMTestCaseID BA-CTSY-CIPC-USS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test multiple client requests to RMmCustomAPI::GetUSIMServiceSupport
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetUSIMServiceSupport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetUSIMServiceSupport0004L()
	{
	//Setup
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	RMmCustomAPI customAPI2;
	OpenCustomAPILC(customAPI2, phone2, EFalse);
	//End Setup

	// Start
	// 1st Client Request
	TInt appNum = 68;		// fill expected appNumber
    TPckg<TInt> appNumPckg(appNum);
	iMockLTSY.ExpectL( ECustomGetServiceTableSupportbyApplicationIPC, appNumPckg, KErrNone);

    RMmCustomAPI::TAppSupport compData;
	compData.iAppNum = 68;		// fill 
	compData.iSupported = ETrue; //app is not supported
    TPckg<RMmCustomAPI::TAppSupport> compPckg(compData);
	
	TRequestStatus requestStatus;
	RMmCustomAPI::TAppSupport usimSupport;
	usimSupport.iAppNum=appNum;
	usimSupport.iSupported=EFalse;
	iMockLTSY.CompleteL( ECustomGetServiceTableSupportbyApplicationIPC, KErrNone, compPckg);
	
	// 2nd Client Request
	TInt appNum2 = 67;		// fill expected appNumber
	TRequestStatus requestStatus2;
	RMmCustomAPI::TAppSupport usimSupport2;
	usimSupport2.iAppNum=appNum2;
	usimSupport2.iSupported=EFalse;
	
	customAPI.GetUSIMServiceSupport(requestStatus, usimSupport);
	customAPI2.GetUSIMServiceSupport(requestStatus2, usimSupport2);

	User::WaitForRequest(requestStatus);
	User::WaitForRequest(requestStatus2);
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();	
	
	//End
	CleanupStack::PopAndDestroy(5); 		//this, telserver2, phone2, customAPI, customAPI2
	}


/**
@SYMTestCaseID BA-CTSY-CIPC-USS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test timeout for request RMmCustomAPI::GetUSIMServiceSupport
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetUSIMServiceSupport
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCustomIPCFU::TestGetUSIMServiceSupport0005L()
	{
	//Setup
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);
	//End Setup
		
	// Start	
	TInt appNum = 68;		// fill expected appNumber
    TPckg<TInt> appNumPckg(appNum);
	iMockLTSY.ExpectL( ECustomGetServiceTableSupportbyApplicationIPC, appNumPckg, KErrNone);

	TRequestStatus requestStatus;
	RMmCustomAPI::TAppSupport usimSupport;
	usimSupport.iAppNum=appNum;
	usimSupport.iSupported=ETrue;
	customAPI.GetUSIMServiceSupport(requestStatus, usimSupport);
		
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();	
	
	//End
	CleanupStack::PopAndDestroy(2); 		//this, customAPI
	}

// Helper methods
void CCTsyCustomIPCFU::OpenCustomAPILC(RMmCustomAPI& aCustomAPI, RMobilePhone& aPhone, TBool aExpectNeeded)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	if(aExpectNeeded)
		{			
		iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);
				
		RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;
		currentlyRetrievedCache.iCacheId	= 1;
		currentlyRetrievedCache.iRecordId	= 0;		
		TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyData(currentlyRetrievedCache);
		ltsyData.SerialiseL(data);			
		iMockLTSY.ExpectL(EReadViagHomeZoneCacheIPC, data);		
		}

	TInt errorCode = aCustomAPI.Open(aPhone);
	ASSERT_EQUALS(KErrNone, errorCode);
	CleanupStack::PopAndDestroy (&data);
	CleanupClosePushL(aCustomAPI);

	}

void CCTsyCustomIPCFU::OpenCustomAPILC(RMmCustomAPI& aCustomAPI)
	{
	OpenCustomAPILC(aCustomAPI,iPhone);
	}


void CCTsyCustomIPCFU::OpenLineLC(RLine& aLine, RPhone& aPhone, const TDesC& aName)
	{
	
	TInt res = aLine.Open(aPhone, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(aLine);
	AssertMockLtsyStatusL();
	
	}
	
	
void CCTsyCustomIPCFU::CreateAndOpenIncomingCalLC(RLine &aLine, 
														RCall &aCall,
														TName& aCallName,
														const TDesC& aLineName,
														const TInt aCallId,
														RMobileCall::TMobileCallStatus aMobileCallStatus,
														RMobilePhone::TMobileService aMobileService
														)
	{
	TInt ret = CreateIncomingCallL(aLine, aCallId, aLineName, aCallName, 
	                               aMobileService, aMobileCallStatus);	
	ASSERT_EQUALS(KErrNone, ret);
		
    ret = aCall.OpenExistingCall(aLine, aCallName);
    ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(aCall);
	}

/**
 * The purpose of this function is to invoke RMmCustomAPI::Open
 * _before_ EMmTsyBootNotifySimStatusReadyIPC CTSY event is triggered and thus
 * let the Viag Home Zome Cache to be initialized. 
 */
void CCTsyCustomIPCFU::OpenPhoneAndCustomAPILC(RMmCustomAPI& aCustomAPI, TInt aNtsyError)
	{
    CleanupClosePushL(aCustomAPI);
	TInt err = iPhone.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);

	err=iMockLTSY.Connect();
	ASSERT_EQUALS(KErrNone, err);

    RBuf8 data;
    data.CleanupClosePushL();
    RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;
    currentlyRetrievedCache.iCacheId    = 1;
    currentlyRetrievedCache.iRecordId   = 0;        
    TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyData(currentlyRetrievedCache);

    
    // 2-nd & all further EReadViagHomeZoneCacheIPC
     for(TInt c = KViagHomeZoneCacheIdMin; c <= KViagHomeZoneCacheIdMax; c++)
         {
         for(TInt r = KViagHomeZoneCacheRecordIdMin; r <= KViagHomeZoneCacheRecordIdMax; r++)
             {
             MockPrimeEReadViagHomeZoneCacheIPCL(c,r, ETrue, ETrue, aNtsyError);
             if(aNtsyError != KErrNone)
                 {
                 // got error, send only once
                 c = KViagHomeZoneCacheIdMax + 1;
                 r = KViagHomeZoneCacheRecordIdMax + 1;
                 }
             }
         }

    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	//************************************************************
	//* Invoke RMmCustomAPI::Open prior to triggering any further 
	//* CTSY events including EMmTsyBootNotifySimStatusReadyIPC
	//************************************************************
	err = aCustomAPI.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);

	// EMmTsyBootNotifyModemStatusReadyIPC
	iMockLTSY.CompleteL(EMmTsyBootNotifyModemStatusReadyIPC,KErrNone);

	// EMobilePhoneGetNetworkRegistrationStatus
	iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus,KErrNone,0);

	// EMmTsyBootNotifySimStatusReadyIPC
	iMockLTSY.ExpectL(EMmTsyBootNotifySimStatusReadyIPC);
	iMockLTSY.CompleteL(EMmTsyBootNotifySimStatusReadyIPC,KErrNone,0);

	
	// EMobilePhoneGetHomeNetwork
	RMobilePhone::TMobilePhoneNetworkInfoV5 homeNetwork;
	homeNetwork.iMode = RMobilePhone::ENetworkModeWcdma;
	homeNetwork.iStatus = RMobilePhone::ENetworkStatusCurrent;
	homeNetwork.iBandInfo = RMobilePhone::EBandUnknown;
	homeNetwork.iCountryCode = _L("234");
	homeNetwork.iCdmaSID = _L("");
	homeNetwork.iAnalogSID = _L("");
	homeNetwork.iNetworkId = _L("23499");
	homeNetwork.iDisplayTag = _L("symbian");
	homeNetwork.iShortName = _L("symbian");
	homeNetwork.iLongName = _L("symbian mobile");
	homeNetwork.iAccess = RMobilePhone::ENetworkAccessUtran;
	homeNetwork.iEgprsAvailableIndicator = ETrue;
	homeNetwork.iHsdpaAvailableIndicator = ETrue;
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkInfoV5> homeNetworkData(homeNetwork);
	homeNetworkData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetHomeNetwork);
	iMockLTSY.CompleteL(EMobilePhoneGetHomeNetwork,KErrNone,data,0);

	// EMmTsyPhoneGetPin1DisableSupportedIPC
	TBool pin1DisableSupport = ETrue;
	TMockLtsyData1<TBool> pin1DisableSupportData(pin1DisableSupport);
	data.Close();
	pin1DisableSupportData.SerialiseL(data);
 	iMockLTSY.ExpectL(EMmTsyPhoneGetPin1DisableSupportedIPC);
	iMockLTSY.CompleteL(EMmTsyPhoneGetPin1DisableSupportedIPC,KErrNone,data,0);

	// EMmTsySimRefreshRegisterIPC
	iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);
	iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrGeneral, 0);		

	// EMobilePhoneGetServiceTable
	RMobilePhone::TMobilePhoneServiceTable serviceTable = RMobilePhone::ESIMServiceTable;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> serviceTableData(serviceTable);
	data.Close();
	serviceTableData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetServiceTable, data);
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTableResult;
	serviceTableResult.iServices1To8  = 0xFF;
	serviceTableResult.iServices9To16 = 0xFF;
	serviceTableResult.iServices17To24= 0xFF;
	serviceTableResult.iServices25To32= 0xFF;
	serviceTableResult.iServices33To40= 0xFF;
	serviceTableResult.iServices41To48= 0xFF;
	serviceTableResult.iServices49To56= 0xFF;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableResultData(serviceTableResult);
	data.Close();
	serviceTableResultData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetServiceTable,KErrNone,data,0);
	
	// EMobilePhoneGetALSLine
	RMobilePhone::TMobilePhoneALSLine alsLine = RMobilePhone::EAlternateLinePrimary;
	TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> alsLineData(alsLine);
	data.Close();
	alsLineData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetALSLine);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine,KErrNone,data,0);

	// ECustomGetIccCallForwardingStatusIPC
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);    

	// EMobilePhoneGetIccMessageWaitingIndicators
	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
	data.Close();
	indicatorsData.SerialiseL(data); 
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, data);

    iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC);
    iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
	// 2-nd & all further EReadViagHomeZoneCacheIPC
	for(TInt c = KViagHomeZoneCacheIdMin; c <= KViagHomeZoneCacheIdMax; c++)
		{
		for(TInt r = KViagHomeZoneCacheRecordIdMin; r <= KViagHomeZoneCacheRecordIdMax; r++)
			{
			MockPrimeEReadViagHomeZoneCacheIPCL(c,r, ETrue, ETrue, aNtsyError);
            if(aNtsyError != KErrNone)
                {
                // got error, send only once
                c = KViagHomeZoneCacheIdMax + 1;
                r = KViagHomeZoneCacheRecordIdMax + 1;
                }
			}
		}
	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	CleanupStack::PopAndDestroy(1,&data);
	}

void CCTsyCustomIPCFU::MockPrimeEReadViagHomeZoneCacheIPCL(TInt aCacheId,TInt aRecordId,TBool aDoComplete, TBool aDoExpect, TInt aNtsyError)
	{
	RBuf8 data;
	data.CleanupClosePushL();

	RMmCustomAPI::TViagCacheRecordId id;
	id.iCacheId  = aCacheId;
	id.iRecordId = aRecordId;
	if(aDoExpect)
	    {
        TMockLtsyData1<RMmCustomAPI::TViagCacheRecordId> idData(id);
        idData.SerialiseL(data);
        iMockLTSY.ExpectL(EReadViagHomeZoneCacheIPC,data);
	    }

	if(aDoComplete)
		{
		RMmCustomAPI::TViagCacheRecordContent& content = 
			iViagHomeZoneCache[aCacheId - KViagHomeZoneCacheIdMin][aRecordId - KViagHomeZoneCacheRecordIdMin];

		content.iCellId = 1000 + aCacheId;
		content.iLac =2000 + aRecordId;

		TMockLtsyData1<RMmCustomAPI::TViagCacheRecordContent> contentData(content);
		data.Close();
		contentData.SerialiseL(data);
		iMockLTSY.CompleteL(EReadViagHomeZoneCacheIPC,aNtsyError,data,0);
		}

	CleanupStack::PopAndDestroy(1,&data);
	}

void CCTsyCustomIPCFU::AuxReadViagHomeZoneParamsL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMmCustomAPI customAPI;
	OpenCustomAPILC(customAPI, iPhone);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RMmCustomAPI::TViagElement element;
	_LIT(KScp, "Lenin lives!");
	_LIT(KSmsC, "revolution");

	// Variables for call ReadViagHomeZoneParams.
	RMmCustomAPI::TViagParams param;
	RMmCustomAPI::TViagElements* elems = NULL;
	
	elems = new (ELeave) RMmCustomAPI::TViagElements( RMmCustomAPI::KViagElementCount );
	CleanupStack::PushL(elems);
	
	for (TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++)
		{			
		elems->AppendL(element);
		}

	// Variables for call CompleteL.
	RMmCustomAPI::TViagParams completeParam;
	completeParam.iScp = KScp;
	completeParam.iSmsC = KSmsC;
	completeParam.iSubscribedZoneAndVersion = 17;

	RMmCustomAPI::TViagElements* completeElems;
	
	FillWithRandomDataLC(completeElems);

	TMockLtsyData2<RMmCustomAPI::TViagParams, RMmCustomAPI::TViagElements*> 
		mockData2(completeParam, completeElems);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EReadViagHomeZoneParamsIPC, KErrNotSupported);
	customAPI.ReadViagHomeZoneParams(requestStatus, param, *elems);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EReadViagHomeZoneParamsIPC);
	mockData2.SerialiseL(completeData);
	iMockLTSY.CompleteL(EReadViagHomeZoneParamsIPC, KErrGeneral, completeData);
	customAPI.ReadViagHomeZoneParams(requestStatus, param, *elems);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMmCustomAPI::ReadViagHomeZoneParams when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EReadViagHomeZoneParamsIPC);
	iMockLTSY.CompleteL(EReadViagHomeZoneParamsIPC, KErrNone, completeData);
	customAPI.ReadViagHomeZoneParams(requestStatus, param, *elems);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(0, completeParam.iScp.Compare(param.iScp));
	ASSERT_EQUALS(0, completeParam.iSmsC.Compare(param.iSmsC));
	ASSERT_EQUALS(completeParam.iSubscribedZoneAndVersion, param.iSubscribedZoneAndVersion);

	for (TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++)
		{
		ASSERT_EQUALS(completeElems->At(i).iActiveFlag         , elems->At(i).iActiveFlag         );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iR2    , elems->At(i).iCoordinates.iR2    );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iX     , elems->At(i).iCoordinates.iX     );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iY     , elems->At(i).iCoordinates.iY     );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iZoneId, elems->At(i).iCoordinates.iZoneId);
		ASSERT_EQUALS(0, completeElems->At(i).iName.Compare(elems->At(i).iName));
		}

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMmCustomAPI::ReadViagHomeZoneParams
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EReadViagHomeZoneParamsIPC, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	

	CleanupStack::PopAndDestroy(5, this); 
	
	}

void CCTsyCustomIPCFU::AuxReadViagHomeZoneParamsForIncreasingCoverageL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	TInt err = iPhone.Open(iTelServer, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	
	err=iMockLTSY.Connect();
	ASSERT_EQUALS(KErrNone, err);

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;
	currentlyRetrievedCache.iCacheId	= 1;
	currentlyRetrievedCache.iRecordId	= 0;		
	TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyData(currentlyRetrievedCache);
	ltsyData.SerialiseL(data);			
	iMockLTSY.ExpectL(EReadViagHomeZoneCacheIPC, data);		

	RMmCustomAPI customAPI;
	TInt errorCode = customAPI.Open(iPhone);
	ASSERT_EQUALS(KErrNone, errorCode);
	CleanupStack::PopAndDestroy (&data);
	CleanupClosePushL(customAPI);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	TRequestStatus requestStatus;

	RMmCustomAPI::TViagElement element;
	_LIT(KScp, "Lenin lives!");
	_LIT(KSmsC, "revolution");

	// Variables for call ReadViagHomeZoneParams.
	RMmCustomAPI::TViagParams param;
	RMmCustomAPI::TViagElements* elems = NULL;
	
	elems = new (ELeave) RMmCustomAPI::TViagElements( RMmCustomAPI::KViagElementCount );
	CleanupStack::PushL(elems);
	
	for (TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++)
		{			
		elems->AppendL(element);
		}

	// Variables for call CompleteL.
	RMmCustomAPI::TViagParams completeParam;
	completeParam.iScp = KScp;
	completeParam.iSmsC = KSmsC;
	completeParam.iSubscribedZoneAndVersion = 17;

	RMmCustomAPI::TViagElements* completeElems;
	
	FillWithRandomDataLC(completeElems);

	TMockLtsyData2<RMmCustomAPI::TViagParams, RMmCustomAPI::TViagElements*> 
		mockData2(completeParam, completeElems);

 	//-------------------------------------------------------------------------

	customAPI.ReadViagHomeZoneParams(requestStatus, param, *elems);

 	//-------------------------------------------------------------------------
	// There are preparations for call of function CheckViagHomezoneParamsL(). 
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// EMobilePhoneGetNetworkRegistrationStatus
	iMockLTSY.CompleteL(EMmTsyBootNotifySimStatusReadyIPC,KErrNone,0);
	
	// EMobilePhoneGetHomeNetwork
	iMockLTSY.ExpectL(EMobilePhoneGetHomeNetwork);
	RMobilePhone::TMobilePhoneNetworkInfoV5 homeNetwork;
	homeNetwork.iMode = RMobilePhone::ENetworkModeWcdma;
	homeNetwork.iStatus = RMobilePhone::ENetworkStatusCurrent;
	homeNetwork.iBandInfo = RMobilePhone::EBandUnknown;
	homeNetwork.iCountryCode = _L("234");
	homeNetwork.iCdmaSID = _L("");
	homeNetwork.iAnalogSID = _L("");
	homeNetwork.iNetworkId = _L("23499");
	homeNetwork.iDisplayTag = _L("symbian");
	homeNetwork.iShortName = _L("symbian");
	homeNetwork.iLongName = _L("symbian mobile");
	homeNetwork.iAccess = RMobilePhone::ENetworkAccessUtran;
	homeNetwork.iEgprsAvailableIndicator = ETrue;
	homeNetwork.iHsdpaAvailableIndicator = ETrue;
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkInfoV5> homeNetworkData(homeNetwork);
	completeData.Close();
	homeNetworkData.SerialiseL(completeData);
	iMockLTSY.CompleteL(EMobilePhoneGetHomeNetwork,KErrNone,completeData,0);

	// EMmTsyPhoneGetPin1DisableSupportedIPC
 	iMockLTSY.ExpectL(EMmTsyPhoneGetPin1DisableSupportedIPC);
	TBool pin1DisableSupport = ETrue;
	TMockLtsyData1<TBool> pin1DisableSupportData(pin1DisableSupport);
	completeData.Close();
	pin1DisableSupportData.SerialiseL(completeData);
	iMockLTSY.CompleteL(EMmTsyPhoneGetPin1DisableSupportedIPC,KErrNone,completeData,0);

	// EMmTsySimRefreshRegisterIPC
	iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);
	iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrGeneral, 0);		

	// EMobilePhoneGetServiceTable
	RMobilePhone::TMobilePhoneServiceTable serviceTable = RMobilePhone::ESIMServiceTable;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> serviceTableData(serviceTable);
	expectData.Close();
	serviceTableData.SerialiseL(expectData);
	iMockLTSY.ExpectL(EMobilePhoneGetServiceTable, expectData);
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTableResult;
	serviceTableResult.iServices1To8  = 0xFF;
	serviceTableResult.iServices9To16 = 0xFF;
	serviceTableResult.iServices17To24= 0xFF;
	serviceTableResult.iServices25To32= 0xFF;
	serviceTableResult.iServices33To40= 0xFF;
	serviceTableResult.iServices41To48= 0xFF;
	serviceTableResult.iServices49To56= 0xFF;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableResultData(serviceTableResult);
	completeData.Close();
	serviceTableResultData.SerialiseL(completeData);
	iMockLTSY.CompleteL(EMobilePhoneGetServiceTable,KErrNone,completeData,0);
	
	// EMobilePhoneGetALSLine
	iMockLTSY.ExpectL(EMobilePhoneGetALSLine);
	RMobilePhone::TMobilePhoneALSLine alsLine = RMobilePhone::EAlternateLinePrimary;
	TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> alsLineData(alsLine);
	completeData.Close();
	alsLineData.SerialiseL(completeData);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine,KErrNone,completeData,0);
	
	// ECustomGetIccCallForwardingStatusIPC
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);    
	
	// EMobilePhoneGetIccMessageWaitingIndicators
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);
	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
    completeData.Close();
    indicatorsData.SerialiseL(completeData); 
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, completeData);
	
	// EReadViagHomeZoneParamsIPC
	completeData.Close();
	mockData2.SerialiseL(completeData);
	iMockLTSY.CompleteL(EReadViagHomeZoneParamsIPC, KErrNone, completeData);
	
	iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC);
	iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
 	iMockLTSY.ExpectL(EReadViagHomeZoneParamsIPC);
	
 	// N.B. EReadViagHomeZoneParamsIPC is not expected at this point because there is an ongoing caching started by customAPI.Open() above.
	
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	//-------------------------------------------------------------------------
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(0, completeParam.iScp.Compare(param.iScp));
	ASSERT_EQUALS(0, completeParam.iSmsC.Compare(param.iSmsC));
	ASSERT_EQUALS(completeParam.iSubscribedZoneAndVersion, param.iSubscribedZoneAndVersion);

	for (TInt i = 0; i < RMmCustomAPI::KViagElementCount; i++)
		{
		ASSERT_EQUALS(completeElems->At(i).iActiveFlag         , elems->At(i).iActiveFlag         );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iR2    , elems->At(i).iCoordinates.iR2    );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iX     , elems->At(i).iCoordinates.iX     );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iY     , elems->At(i).iCoordinates.iY     );
		ASSERT_EQUALS(completeElems->At(i).iCoordinates.iZoneId, elems->At(i).iCoordinates.iZoneId);
		ASSERT_EQUALS(0, completeElems->At(i).iName.Compare(elems->At(i).iName));
		}
	
	CleanupStack::PopAndDestroy(6, this); 
	
	}

void CCTsyCustomIPCFU::OpenPhoneBookStoreWithSIMRefreshL(RMobilePhoneBookStore &aStore, TName &aName, RMobilePhone &aPhone, TInt aSimError)
    {

    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 data2;
    CleanupClosePushL(data2);
    

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    //EMmTsyPhoneBookStoreInitIPC
    TMockLtsyPhoneBookData0 storeInitData(aName);
    storeInitData.SerialiseL(data);
    
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreInitIPC, data);   

    _LIT8(KImsi,"01234567890123");
    TUint16 files = 2;
    TPckg<TUint16> filesData(files);
    iMockLTSY.CompleteL(EMmTsySimRefreshNowIPC, aSimError, filesData);
    iMockLTSY.ExpectL(EMobilePhoneGetSubscriberId);
    iMockLTSY.CompleteL(EMobilePhoneGetSubscriberId, KErrNone, KImsi);

    CStorageInfoData storageData;

    SetStorageInfoData(storageData);
        
    TMockLtsyPhoneBookData1< CStorageInfoData > retStoreInitC(aName, storageData); 
    retStoreInitC.SerialiseL(data2);
    
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreInitIPC, KErrNone, data2, 0);

    //EMmTsyPhoneBookStoreCacheIPC
    CArrayPtrSeg<CPhoneBookStoreEntry>* cache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>( 1 );
    CleanupStack::PushL(cache);
    
    TMockLtsyPhoneBookData1<CArrayPtrSeg<CPhoneBookStoreEntry>*> storeCacheData(aName, cache);
    data2.Close();
    storeCacheData.SerialiseL(data2);    
    data.Close();
    storeInitData.SerialiseL(data);
    
                  
    iMockLTSY.ExpectL(EMmTsyPhoneBookStoreCacheIPC, data);
    iMockLTSY.CompleteL(EMmTsyPhoneBookStoreCacheIPC, aSimError, data2, 0);
    
    //Open
    TInt ret = aStore.Open(aPhone, aName);
    ASSERT_EQUALS(KErrNone, ret);           

    User::WaitForRequest(mockLtsyStatus);           
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());  
    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(3); //data, data2, cache

    }


void CCTsyCustomIPCFU::FillRandom(TDes8 &aBuff)
    {
    TUint length = aBuff.MaxLength();
    aBuff.SetLength(0);
    for (TUint i = 0; i < length; ++i)
        {
        TUint8 rand = (TUint8) Math::Random();
        aBuff.Append(rand);
        }
    }





