// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// functional unit of the Common TSY.
// 
//

/**
 @file The TEFUnit header file which tests the CustomIPC
*/

#ifndef CCTSYCUSTOMIPCFU_H
#define CCTSYCUSTOMIPCFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"
#include "CMmSIMTsy.h" // KPhEngMaxViagHomeZones & KPhEngMaxCacheId
#include <ctsy/rmmcustomapi.h>


#define CMDBUFLEN   4
typedef TBuf8<CMDBUFLEN> CmdDataBuf;
#define RSPDATALEN 20
typedef TBuf8<RSPDATALEN> RspDataBuf; 
#define RSPDATALENTOOBIG 300 
typedef TBuf8<RSPDATALENTOOBIG> RspBigBuf;

class CCTsyCustomIPCFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestSetDriveMode0001L();
	void TestSetDriveMode0002L();	
	void TestSetDriveMode0004L();
	void TestSetDriveMode0005L();
	void TestActivateSimLock0001L();
	void TestActivateSimLock0002L();
	void TestActivateSimLock0003L();
	void TestActivateSimLock0004L();
	void TestActivateSimLock0005L();
	void TestDeActivateSimLock0001L();
	void TestDeActivateSimLock0002L();
	void TestDeActivateSimLock0003L();
	void TestDeActivateSimLock0004L();
	void TestDeActivateSimLock0005L();
	void TestNotifyDtmfEvent0001L();
	void TestNotifyDtmfEvent0002L();	
	void TestNotifyDtmfEvent0004L();	
	void TestGetDiagnosticInfo0001L();		
	void TestSetAlsBlocked0001L();
	void TestSetAlsBlocked0002L();
	void TestSetAlsBlocked0004L();
	void TestSetAlsBlocked0005L();	
	void TestGetAlsBlocked0001L();
	void TestGetAlsBlocked0002L();	
	void TestGetAlsBlocked0004L();
	void TestGetAlsBlocked0005L();	
	void TestCheckAlsPpSupport0001L();
	void TestCheckAlsPpSupport0002L();
	void TestCheckAlsPpSupport0004L();
	void TestCheckAlsPpSupport0005L();	
	void TestGetRemoteAlertingToneStatus0001L();
	void TestCallOrigin0001L();
	
	void TestTerminateCall0001L();
	void TestTerminateCall0002L();
	void TestTerminateCall0003L();
	void TestTerminateCall0004L();
	void TestTerminateCall0005L();
	void TestNotifyAlsBlockedChanged0001L();
	void TestNotifyAlsBlockedChanged0002L();
	void TestNotifyAlsBlockedChanged0004L();
	void TestGetCipheringInfo0001L();
	void TestGetCipheringInfo0002L();
	void TestGetCipheringInfo0004L();
	void TestGetCipheringInfo0005L();
	void TestNotifyCipheringInfoChange0001L();
	void TestNotifyCipheringInfoChange0002L();
	void TestNotifyCipheringInfoChange0004L();
	void TestNotifyNSPSStatus0001L();
	void TestNotifyNSPSStatus0002L();
	void TestNotifyNSPSStatus0004L();
	void TestNetWakeup0001L();
	void TestNetWakeup0002L();
	void TestNetWakeup0004L();
	void TestNetWakeup0005L();
	void TestReadViagHomeZoneParams0001L();
	void TestReadViagHomeZoneParams0002L();
	void TestReadViagHomeZoneParams0003L();
	void TestReadViagHomeZoneParams0004L();
	void TestReadViagHomeZoneParams0005L();
    void TestReadViagHomeZoneCache0001L();
    void TestReadViagHomeZoneCache0001bL();
    void TestReadViagHomeZoneCache0001cL();
	void TestReadViagHomeZoneCache0002L();
	void TestReadViagHomeZoneCache0003L();
	void TestReadViagHomeZoneCache0004L();
	void TestReadViagHomeZoneCache0005L();
	void TestWriteViagHomeZoneCache0001L();
	void TestWriteViagHomeZoneCache0002L();
	void TestWriteViagHomeZoneCache0003L();
	void TestWriteViagHomeZoneCache0004L();
	void TestWriteViagHomeZoneCache0005L();
	void TestClearCallBlackList0001L();
	void TestClearCallBlackList0002L();
	void TestClearCallBlackList0004L();
	void TestClearCallBlackList0005L();
	void TestSsAdditionalInfoNotification0001L();
	void TestSsAdditionalInfoNotification0002L();
	void TestSsAdditionalInfoNotification0004L();
	void TestSsRequestCompleteNotification0001L();
	void TestSsRequestCompleteNotification0002L();
	void TestSsRequestCompleteNotification0004L();
	void TestIsBlocked0001L();
	void TestIsBlocked0005L();
	void TestCheckSecurityCode0001L();
	void TestCheckSecurityCode0002L();
	void TestCheckSecurityCode0004L();
	void TestCheckSecurityCode0005L();
	void TestGetActivePin0001L();
	void TestGetAirTimeDuration0001L();
	void TestGetNetworkProviderName0001L();
	void TestGetNetworkProviderName0002L();
	void TestGetNetworkProviderName0003L();
	void TestGetNetworkProviderName0004L();
	void TestGetNetworkProviderName0005L();
	void TestCheckEmergencyNumber0001L();
	void TestCheckEmergencyNumber0002L();
	void TestCheckEmergencyNumber0004L();
	void TestCheckEmergencyNumber0005L();
	void TestNotifyPndCacheReady0001L();
	void TestNotifyPndCacheReady0001bL();
	void TestNotifyPndCacheReady0002L();
	void TestNotifyPndCacheReady0003L();
	void TestNotifyPndCacheReady0004L();
	void TestGetPndCacheStatus0001L();
	void TestGetPndCacheStatus0003L();
	void TestGetOperatorName0001L();
	void TestGetOperatorName0002L();
	void TestGetOperatorName0004L();
	void TestGetOperatorName0005L();
	void TestGetProgrammableOperatorLogo0001L();
	void TestGetProgrammableOperatorLogo0002L();
	void TestGetProgrammableOperatorLogo0003L();
	void TestGetProgrammableOperatorLogo0004L();
	void TestGetProgrammableOperatorLogo0005L();
	void TestNotifyProgrammableOperatorLogoChange0001L();
	void TestNotifySsNetworkEvent0001L();
	void TestNotifySsNetworkEvent0002L();
	void TestNotifySsNetworkEvent0004L();
	void TestCancelUssdSession0001L();
	void TestSatRefreshCompleteNotification0001L();
	void TestSatRefreshCompleteNotification0002L();
	void TestSatRefreshCompleteNotification0004L();
	void TestCheckTwoDigitDialSupport0001L();
	void TestCheckTwoDigitDialSupport0002L();
	void TestCheckTwoDigitDialSupport0004L();
	void TestCheckTwoDigitDialSupport0005L();
	void TestResetNetServer0001L();
	void TestResetNetServer0002L();
	void TestResetNetServer0004L();
	void TestResetNetServer0005L();
	void TestReleaseFile0001L();
	void TestRestartFile0001L();
	void TestStartSimCbTopicBrowsing0001L();
	void TestStartSimCbTopicBrowsing0003L();
	void TestStartSimCbTopicBrowsing0005L();
	void TestGetNextSimCbTopic0001L();
	void TestDeleteSimCbTopic0001L();
	void TestDeleteSimCbTopic0002L();
	void TestDeleteSimCbTopic0003L();
	void TestDeleteSimCbTopic0004L();
	void TestDeleteSimCbTopic0005L();
	void TestNotifyNetworkConnectionFailure0001L();
	void TestNotifyNetworkConnectionFailure0002L();
	void TestNotifyNetworkConnectionFailure0004L();
	void TestSendAPDUReq0001L();
	void TestSendAPDUReq0002L();
	void TestSendAPDUReq0003L();
	void TestSendAPDUReq0004L();
	void TestSendAPDUReq0005L();
	void TestDisablePhoneLock0001L();
	void TestDisablePhoneLock0002L();
	void TestDisablePhoneLock0004L();
	void TestDisablePhoneLock0005L();
	void TestNotifyEGprsInfoChange0001L();
	void TestNotifyEGprsInfoChange0002L();
	void TestNotifyEGprsInfoChange0003L();
	void TestNotifyEGprsInfoChange0004L();
	void TestGetEGprsInfo0001L();
	void TestGetEGprsInfo0003L();
	void TestReadSimFile0001L();
	void TestReadSimFile0002L();
	void TestReadSimFile0003L();
	void TestReadSimFile0004L();
	void TestReadSimFile0005L();
	void TestGetLifeTime0001L();
	void TestGetLifeTime0002L();
	void TestGetLifeTime0003L();
	void TestGetLifeTime0004L();
	void TestGet3GPBInfo0001L();
	void TestGetSystemNetworkModes0001L();
	void TestGetSystemNetworkModes0002L();
	void TestGetSystemNetworkModes0004L();
	void TestGetSystemNetworkModes0005L();
	void TestSetSystemNetworkMode0001L();
	void TestSetSystemNetworkMode0002L();
	void TestSetSystemNetworkMode0004L();
	void TestSetSystemNetworkMode0005L();
	void TestGetCurrentSystemNetworkModes0001L();
	void TestGetCurrentSystemNetworkModes0002L();
	void TestGetCurrentSystemNetworkModes0004L();
	void TestGetCurrentSystemNetworkModes0005L();
	void TestPowerSimOn0001L();
	void TestPowerSimOn0002L();
	void TestPowerSimOn0004L();
	void TestPowerSimOn0005L();
	void TestPowerSimOff0001L();
	void TestPowerSimOff0002L();
	void TestPowerSimOff0004L();
	void TestPowerSimOff0005L();
	void TestSimWarmReset0001L();
	void TestSimWarmReset0002L();
	void TestSimWarmReset0004L();
	void TestSimWarmReset0005L();
	void TestGetATR0001L();
	void TestGetATR0002L();
	void TestGetATR0003L();
	void TestGetATR0004L();
	void TestGetATR0005L();
	void TestGetSimCardReaderStatus0001L();
	void TestGetSimCardReaderStatus0002L();
	void TestGetSimCardReaderStatus0004L();
	void TestGetSimCardReaderStatus0005L();
	void TestNotifySimCardStatus0001L();
	void TestNotifySimCardStatus0002L();
	void TestNotifySimCardStatus0004L();
	void TestGetWlanSimAuthenticationData0001L();
	void TestGetWlanSimAuthenticationData0002L();
	void TestGetWlanSimAuthenticationData0003L();
	void TestGetWlanSimAuthenticationData0004L();
	void TestGetWlanSimAuthenticationData0005L();
	void TestSetSimMessageStatusRead0001L();
	void TestWriteViagHomeZoneUHZIUESettings0001L();
	void TestWriteViagHomeZoneUHZIUESettings0002L();
	void TestWriteViagHomeZoneUHZIUESettings0004L();
	void TestWriteViagHomeZoneUHZIUESettings0005L();
	void TestSetAlwaysOn0001L();
	void TestSetAlwaysOn0002L();
	void TestSetAlwaysOn0003L();
	void TestSetAlwaysOn0004L();
	void TestNotifyRauEvent0001L();
	void TestNotifyRauEvent0002L();
	void TestNotifyRauEvent0004L();
	void TestReadHSxPAStatus0001L();
	void TestReadHSxPAStatus0002L();
	void TestReadHSxPAStatus0004L();
	void TestReadHSxPAStatus0005L();
	void TestWriteHSxPAStatus0001L();
	void TestWriteHSxPAStatus0002L();
	void TestWriteHSxPAStatus0004L();
	void TestWriteHSxPAStatus0005L();
	void TestNotifyHSxPAStatus0001L();
	void TestNotifyHSxPAStatus0002L();
	void TestNotifyHSxPAStatus0004L();
	void TestGetIccCallForwardingIndicatorStatus0001L();
	void TestGetIccCallForwardingIndicatorStatus0002L();
	void TestGetIccCallForwardingIndicatorStatus0003L();
	void TestGetIccCallForwardingIndicatorStatus0004L();
	void TestGetIccCallForwardingIndicatorStatus0005L();
	void TestNotifyIccCallForwardingStatusChange0001L();
	void TestNotifyIccCallForwardingStatusChange0002L();
	void TestNotifyIccCallForwardingStatusChange0003L();
	void TestNotifyIccCallForwardingStatusChange0004L();
	void TestGetCellInfo0001L();
	void TestGetCellInfo0002L();
	void TestGetCellInfo0003L();
	void TestGetCellInfo0004L();
	void TestGetCellInfo0005L();
	void TestNotifyCellInfoChange0001L();
	void TestNotifyCellInfoChange0002L();
	void TestNotifyCellInfoChange0003L();
	void TestNotifyCellInfoChange0004L();
	void TestWriteAlphaString0001L();
	void TestReadAlphaString0001L();
	void TestDeleteAlphaString0001L();
	void TestGetSystemNetworkBand0001L();
	void TestGetSystemNetworkBand0002L();
	void TestGetSystemNetworkBand0004L();
	void TestGetSystemNetworkBand0005L();
	void TestSetSystemNetworkBand0001L();
	void TestSetSystemNetworkBand0002L();
	void TestSetSystemNetworkBand0004L();
	void TestSetSystemNetworkBand0005L();

	void TestGetUSIMServiceSupport0001L();
	void TestGetUSIMServiceSupport0002L();
	void TestGetUSIMServiceSupport0003L();
	void TestGetUSIMServiceSupport0004L();
	void TestGetUSIMServiceSupport0005L();
	
	
private:

	void OpenCustomAPILC(RMmCustomAPI& aCustomAPI, RMobilePhone& aPhone, TBool aExpectNeeded = ETrue);
	void OpenCustomAPILC(RMmCustomAPI& aCustomAPI);
	void OpenLineLC(RLine& aLine, RPhone& aPhone, const TDesC& aName);
	void CreateAndOpenIncomingCalLC(RLine &aLine, 
									RCall &aCall,
									TName &aCallName,
									const TDesC& aLineName,
									const TInt aCallId,
									RMobileCall::TMobileCallStatus aMobileCallStatus,
									RMobilePhone::TMobileService aMobileService
									);
	void OpenPhoneAndCustomAPILC(RMmCustomAPI& aCustomAPI, TInt aNtsyError = KErrNone);
	void MockPrimeEReadViagHomeZoneCacheIPCL(TInt aCacheId,TInt aRecordId, TBool aDoComplete = ETrue, TBool aDoExpect = ETrue, TInt aNtsyError = KErrNone);
	void FillWithRandomData( RMmCustomAPI::TMmCellInfo& cell, TInt aRnd = 0) const;
	void FillWithRandomDataLC( RMmCustomAPI::TViagElements*& cell, TInt aRnd = 0) const;
	void InitSimCbTopicsL(RMmCustomAPI &aCustomApi, const TDesC& aTopic, TInt aNum);

	void AuxReadViagHomeZoneParamsL();
	void AuxReadViagHomeZoneParamsForIncreasingCoverageL();
	void OpenPhoneBookStoreWithSIMRefreshL(RMobilePhoneBookStore &aStore, TName &aName, RMobilePhone &aPhone, TInt aSimError);
    static void FillRandom(TDes8 &aBuff);
	RMmCustomAPI::TViagCacheRecordContent iViagHomeZoneCache[KPhEngMaxViagHomeZones][KPhEngMaxCacheId];
	}; // class CCTsyCustomIPCFU

#endif // CCTSYCUSTOMIPCFU_H

