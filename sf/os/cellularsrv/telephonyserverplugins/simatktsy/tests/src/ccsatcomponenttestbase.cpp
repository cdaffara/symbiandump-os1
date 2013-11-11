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
//



/**
 @file base class for SAT component tests
*/

#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include <pcktcs.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/serviceapi/cmmutility.h>
#include <test/mockltsyindicatorids.h>
#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h> //for KLtsyDispatchPhonebookSmsStoreWriteEntryApiId
#include <cstktsy/ttlv.h>
#include <charconv.h> 

#include "ccsatcomponenttestbase.h"


_LIT(KDoubleColon, "::");
_LIT(KMmTsyWithDispatcherModuleName, "phonetsywithdispatcher");


/**
To push a CleanupItem ont the cleanupstack to close the phone
*/
void CCSatComponentTestBase::Cleanup(TAny* self)
	{
	static_cast<CCSatComponentTestBase*>(self)->DoCleanup();
	}
	
/**
Close the Phone a the server connection
*/	
void CCSatComponentTestBase::DoCleanup()
	{
	iSms.Close();
	iSat.Close();
	iPhone.Close();
	iMockLTSY.Close();
	
	TInt ret = iTelServer.UnloadPhoneModule(KMmTsyWithDispatcherModuleName);
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to unload phone module"));
		}
	
    iTelServer.Close();
 
	}

/**
Assert MockLtsy Terminated all handlings without errors
*/
void CCSatComponentTestBase::AssertMockLtsyStatusL()
	{
	TBool hasWaitingExpect;
	TBool hasPendingComplete;
	TBool hasError;
	iMockLTSY.GetStatus(hasWaitingExpect,hasPendingComplete,hasError);
	if (hasWaitingExpect || hasPendingComplete)
		{
		ERR_PRINTF1(_L("MockLTsy still have pending/waiting events!"));
		}
	if (hasWaitingExpect || hasPendingComplete || hasError)
		{
		TBuf<255> log;
		do	{
			iMockLTSY.GetNextLogLine(log);
			INFO_PRINTF1(log);
			} 
		while(log.Length() > 0);
		User::Leave(KErrTEFUnitFail); // Fail the test
		}
	}

	
/**
Open Server session
*/
void CCSatComponentTestBase::OpenEtelServerL(TExtendedErrorClient aSetExtendedErrorClient)
	{
	TInt ret = iTelServer.Connect();

	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect phone"));
		User::Leave(ret);
		}

	ret = iTelServer.LoadPhoneModule(KMmTsyWithDispatcherModuleName);

	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}
		
	if(aSetExtendedErrorClient == EUseExtendedError)
		{
		iTelServer.SetExtendedErrorGranularity(	RTelServer::EErrorExtended );
		}
	}

/**
Standard function to open the phone up to the end of the boot sequence
*/
void CCSatComponentTestBase::OpenPhoneL()
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TInt err = iPhone.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);

	//open SMS messaging session
	err = iSms.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);
	
	TRequestStatus modemDetectedStatus;
	RPhone::TModemDetection modemDetection;
	iPhone.NotifyModemDetected(modemDetectedStatus,modemDetection);


	
	err=iMockLTSY.Connect();
	ASSERT_EQUALS(KErrNone, err);
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	 
	TRfStateInfo rfstatus = ERfsStateInfoNormal;
	TMockLtsyData1<TRfStateInfo> rfstatdata(rfstatus);
	rfstatdata.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneBootNotifyModemStatusReady::KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId,KErrNone, data, 1);
	
	// EMobilePhoneGetNetworkRegistrationStatus
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId);
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus = RMobilePhone::ERegisteredOnHomeNetwork;
	TMockLtsyData1<RMobilePhone::TMobilePhoneRegistrationStatus> retNetRegStatusData(regStatus);
	data.Close();
	retNetRegStatusData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId,KErrNone,data, 0);
	
	// EMmTsyBootNotifySimStatusReadyIPC
	iMockLTSY.ExpectL(MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId,KErrNone,0);
	
	// Prepare MockLtsy for the internal getters of the network information
	DriverPrepInternalGettersMockLtsyL();
	
	// EMobilePhoneGetHomeNetwork
	data.Close();
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetHomeNetwork::KLtsyDispatchPhoneGetHomeNetworkApiId);
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
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetHomeNetwork::KLtsyDispatchPhoneGetHomeNetworkApiId,KErrNone,data,0);

	// EMmTsySecurityGetPin1DisableSupportedIPC
 	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId);
	TBool pin1DisableSupport = ETrue;
	TMockLtsyData1<TBool> pin1DisableSupportData(pin1DisableSupport);
	data.Close();
	pin1DisableSupportData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId,KErrNone,data,0);

	//EMmTsySimRefreshRegisterIPC
	TUint16 refreshFileList = KCacheAdn | KCacheFdn | KCacheServiceTable | KCacheALSline;
	TMockLtsyData1<TUint16> refreshFileListData(refreshFileList);
	data.Close();
	refreshFileListData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId,data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId,KErrNone);
		
	//EMobilePhoneGetServiceTable
	RMobilePhone::TMobilePhoneServiceTable serviceTable = RMobilePhone::ESIMServiceTable;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> serviceTableData(serviceTable);
	data.Close();
	serviceTableData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId, data);

	RMobilePhone::TMobilePhoneServiceTableV1 simServiceTable;
	simServiceTable.iServices1To8  = 0xFF;
	simServiceTable.iServices9To16 = 0xFF;
	simServiceTable.iServices17To24= 0xFF;
	simServiceTable.iServices25To32= 0xFF;
	simServiceTable.iServices33To40= 0xFF;
	simServiceTable.iServices41To48= 0xFF;
	simServiceTable.iServices49To56= 0xFF;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableResultData(simServiceTable);
	data.Close();
	serviceTableResultData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId,KErrNone,data);
		
	// EMobilePhoneGetALSLine
	iMockLTSY.ExpectL(MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId);

	RMobilePhone::TMobilePhoneALSLine alsLine = RMobilePhone::EAlternateLinePrimary;
	TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> alsLineData(alsLine);
	data.Close();
	alsLineData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId,KErrNone,data,0);
	
	// ECustomGetIccCallForwardingStatusIPC
	iMockLTSY.ExpectL(MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId);

	RMobilePhone::TMobilePhoneCFUIndicatorV6 indicator;
	indicator.iMultipleSubscriberProfileID = RMobilePhone::KProfileIdentityTwo;
	indicator.iIndicator = RMobilePhone::KCFUIndicatorVoice;
	indicator.iCFNumber.iNumberPlan = RMobilePhone::ETelexNumberPlan;
	indicator.iCFNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KTelNumber, "012345678");
	indicator.iCFNumber.iTelNumber = KTelNumber; 
	TMockLtsyData1<RMobilePhone::TMobilePhoneCFUIndicatorV6> indicatorData(indicator);
	data.Close();
	indicatorData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId,KErrNone,data,0);
	
	// EMobilePhoneGetIccMessageWaitingIndicators
	iMockLTSY.ExpectL(MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId);

	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
    data.Close();
    indicatorsData.SerialiseL(data); 
	iMockLTSY.CompleteL(MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId, KErrNone, data);

	iMockLTSY.ExpectL(MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId);
	 
 	RMmCustomAPI::TAlsSupport alsSupport = RMmCustomAPI::EAlsSupportOff;
 	TMockLtsyData1<RMmCustomAPI::TAlsSupport> alsLtsyData1(alsSupport);
 	data.Close();
 	alsLtsyData1.SerialiseL(data);
 	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId, KErrNone, data);
 	
 	iMockLTSY.ExpectL(MLtsyDispatchSimGetCustomerServiceProfile::KLtsyDispatchSimGetCustomerServiceProfileApiId);
 
 	// Prepare data for the mockLtsy's CompleteL
 	RMobilePhone::TMobilePhoneCspFileV1 cspComp;
 	cspComp.iCallOfferingServices = 0x01;
 	cspComp.iCallRestrictionServices = 0x02;
 	cspComp.iOtherSuppServices = 0x03;
 	cspComp.iCallCompletionServices = 0x04;
 	cspComp.iTeleservices = 0x05;
 	cspComp.iCphsTeleservices = 0x06;
 	cspComp.iCphsFeatures = 0x07;
 	cspComp.iNumberIdentServices = 0x08;
 	cspComp.iPhase2PlusServices = 0x09;
 	cspComp.iValueAddedServices = 0x0A;
 	TMockLtsyData1<RMobilePhone::TMobilePhoneCspFileV1> cspCompData(cspComp);
 	data.Close();
 	cspCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetCustomerServiceProfile::KLtsyDispatchSimGetCustomerServiceProfileApiId, KErrNone, data);
 	

	
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // data
	}

/**
Sends ExpectL and CompleteL commands to MockLtsy for getters of various network information.
These getters are internal to the CtsyDispatcher.
This method should be called immediately after the CompleteL for KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId,
since these getters are triggered on the CtsyDispacther receiving a BootNotifyModemStatusReady indication. 
*/
void CCSatComponentTestBase::DriverPrepInternalGettersMockLtsyL()
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	//
	// Prepare MockLtsy for processing a getter for CurrentNetworkInfo
	//
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId);

	// Prepare the KLtsyDispatchPhoneGetCurrentNetworkInfoApiId data for CompleteL
	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoComplete;
	
	// V5
	networkInfoComplete.iEgprsAvailableIndicator = KEgprsAvailable;
	networkInfoComplete.iHsdpaAvailableIndicator = KHsdpaAvailable;
	// V2
	networkInfoComplete.iAccess = KAccess;
	// V1
	networkInfoComplete.iMode        = KMode;
	networkInfoComplete.iStatus      = KStatus;
	networkInfoComplete.iBandInfo    = KBandInfo;
	networkInfoComplete.iCountryCode = KCountryCode;
	networkInfoComplete.iCdmaSID     = KCdmaSID;
	networkInfoComplete.iAnalogSID   = KAnalogSID;
	networkInfoComplete.iNetworkId   = KNetworkId;
	networkInfoComplete.iDisplayTag  = KDisplayTag;
	networkInfoComplete.iShortName   = KShortName;
	networkInfoComplete.iLongName    = KLongName;
	
	RMobilePhone::TMobilePhoneLocationAreaV1 locationAreaComplete;
	locationAreaComplete.iAreaKnown = KAreaKnown;
	locationAreaComplete.iLocationAreaCode = KLocationAreaCode;
	locationAreaComplete.iCellId = KCellId;
	
    TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV5, 
                   RMobilePhone::TMobilePhoneLocationAreaV1> mockCNCData2(networkInfoComplete, locationAreaComplete);
	
	data.Close();
	mockCNCData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId, KErrNone, data);

	//
	// Prepare MockLtsy for processing a getter for NetworkModeChange
	//
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId);

	// Prepare the KLtsyDispatchPhoneGetNetworkModeApiId data for CompleteL
    RMobilePhone::TMobilePhoneNetworkMode networkModeComplete;
    networkModeComplete = KNetworkMode;

    TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkMode> mockNMCData1(networkModeComplete);
	
	data.Close();
	mockNMCData1.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId, KErrNone, data);

	//
	// Prepare MockLtsy for processing a getter for NitzInfoChange
	//
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId);
	RMobilePhone::TMobilePhoneNITZ nitzComplete(2008, EMay, 12, 11,  59,  0, 0);
	nitzComplete.iShortNetworkId = KShortName;
	nitzComplete.iLongNetworkId = KLongName;
	nitzComplete.iNitzFieldsUsed = KNitzFieldsUsed;
	nitzComplete.iTimeZone = KTimeZone;
	nitzComplete.iDST = KDst;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneNITZ> mockNitzData1(nitzComplete);
	data.Close();
	mockNitzData1.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId, KErrNone, data);
    
	//
	// Prepare MockLtsy for processing a getter for SignalStrengthChange
	//
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId);
	TInt32 signalStrengthComplete(KSignalStrength);
	TInt8 barComplete(KBar);
	TMockLtsyData2<TInt32, TInt8> mockSigSthData2(signalStrengthComplete, barComplete);
	data.Close();
	mockSigSthData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId, KErrNone, data);
	
	//
	// Prepare MockLtsy for processing a getter for BatteryInfoChange
	//
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId);
	RMobilePhone::TMobilePhoneBatteryInfoV1 battInfo;
	battInfo.iChargeLevel = KBatteryChargeLevel;
	battInfo.iStatus = KBatteryStatus;
	                     
	TMockLtsyData1<RMobilePhone::TMobilePhoneBatteryInfoV1> mockBattInfoData1(battInfo);
	data.Close();
	mockBattInfoData1.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId, KErrNone, data);
	
	data.Close();
	CleanupStack::PopAndDestroy();  // data
	}

/**
 * Waits until the MockLTSY queued events is empty
 */

void CCSatComponentTestBase::WaitForMockLTSYTerminated()
	{
	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus); 
	User::WaitForRequest(reqStatus);
	ASSERT_TRUE(reqStatus.Int() == KErrNone);
	}
	

/**
Sends ExpectL and CompleteL commands to MockLtsy for getters of various network information.
These getters are internal to the CtsyDispatcher.
This method should be called immediately after the CompleteL for KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId,
since these getters are triggered on the CtsyDispacther receiving a BootNotifyModemStatusReady indication. 
*/
void CCSatComponentTestBase::PrepInternalGettersMockLtsy()
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	//
	// Prepare MockLtsy for processing a getter for CurrentNetworkInfo
	//
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId);

	// Prepare the KLtsyDispatchPhoneGetCurrentNetworkInfoApiId data for CompleteL
	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoComplete;
	
	// V5
	networkInfoComplete.iEgprsAvailableIndicator = KEgprsAvailable;
	networkInfoComplete.iHsdpaAvailableIndicator = KHsdpaAvailable;
	// V2
	networkInfoComplete.iAccess = KAccess;
	// V1
	networkInfoComplete.iMode        = KMode;
	networkInfoComplete.iStatus      = KStatus;
	networkInfoComplete.iBandInfo    = KBandInfo;
	networkInfoComplete.iCountryCode = KCountryCode;
	networkInfoComplete.iCdmaSID     = KCdmaSID;
	networkInfoComplete.iAnalogSID   = KAnalogSID;
	networkInfoComplete.iNetworkId   = KNetworkId;
	networkInfoComplete.iDisplayTag  = KDisplayTag;
	networkInfoComplete.iShortName   = KShortName;
	networkInfoComplete.iLongName    = KLongName;
	
	RMobilePhone::TMobilePhoneLocationAreaV1 locationAreaComplete;
	locationAreaComplete.iAreaKnown = KAreaKnown;
	locationAreaComplete.iLocationAreaCode = KLocationAreaCode;
	locationAreaComplete.iCellId = KCellId;
	
    TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV5, 
                   RMobilePhone::TMobilePhoneLocationAreaV1> mockCNCData2(networkInfoComplete, locationAreaComplete);
	
	data.Close();
	mockCNCData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId, KErrNone, data);

	//
	// Prepare MockLtsy for processing a getter for NetworkModeChange
	//
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId);

	// Prepare the KLtsyDispatchPhoneGetNetworkModeApiId data for CompleteL
    RMobilePhone::TMobilePhoneNetworkMode networkModeComplete;
    networkModeComplete = KNetworkMode;

    TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkMode> mockNMCData1(networkModeComplete);
	
	data.Close();
	mockNMCData1.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId, KErrNone, data);

	//
	// Prepare MockLtsy for processing a getter for NitzInfoChange
	//
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId);
	RMobilePhone::TMobilePhoneNITZ nitzComplete(2008, EMay, 12, 11,  59,  0, 0);
	nitzComplete.iShortNetworkId = KShortName;
	nitzComplete.iLongNetworkId = KLongName;
	nitzComplete.iNitzFieldsUsed = KNitzFieldsUsed;
	nitzComplete.iTimeZone = KTimeZone;
	nitzComplete.iDST = KDst;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneNITZ> mockNitzData1(nitzComplete);
	data.Close();
	mockNitzData1.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId, KErrNone, data);
    
	//
	// Prepare MockLtsy for processing a getter for SignalStrengthChange
	//
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId);
	TInt32 signalStrengthComplete(KSignalStrength);
	TInt8 barComplete(KBar);
	TMockLtsyData2<TInt32, TInt8> mockSigSthData2(signalStrengthComplete, barComplete);
	data.Close();
	mockSigSthData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId, KErrNone, data);
	
	//
	// Prepare MockLtsy for processing a getter for BatteryInfoChange
	//
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId);
	RMobilePhone::TMobilePhoneBatteryInfoV1 battInfo;
	battInfo.iChargeLevel = KBatteryChargeLevel;
	battInfo.iStatus = KBatteryStatus;
	                     
	TMockLtsyData1<RMobilePhone::TMobilePhoneBatteryInfoV1> mockBattInfoData1(battInfo);
	data.Close();
	mockBattInfoData1.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId, KErrNone, data);
	
	data.Close();
	CleanupStack::PopAndDestroy();  // data
	}
	
/**
Dial a call
@param aCall the call to use to dial
@param aService type of call (must match the line the call what created on)
@param aCallId Call ID the TSY allocates for this call
*/
void CCSatComponentTestBase::DialL(RMobileCall& aCall, TInt aCallId, RMobilePhone::TMobileService aService)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	// EEtelCallDial
	RMobileCall::TMobileCallParamsV1 callParams;
	RMobileCall::TMobileCallParamsV1Pckg callParamsPckg(callParams);
    callParams.iInterval = 0;
    callParams.iCug.iCugIndex = 0xFFFF;
	RMobileCall::TMobileCallInfoV1 callInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(callInfo);
	callInfo.iDialledParty.iTelNumber.Copy(_L("02071541111"));
	callInfo.iService = aService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> 
		dialData(0, aService, callParams, callInfo);
	dialData.SerialiseL(data);
	iMockLTSY.ExpectL(EEtelCallDial,data);

	// EMobileCallGetMobileCallInfo
	callInfo.iService = aService; 
	callInfo.iEmergency = EFalse;
	callInfo.iRemoteParty.iCallingName = _L("");
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityUnknown;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = _L("");
	callInfo.iRemoteParty.iRemoteNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
	callInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber = RMobilePhone::EUnknownNumber;
	callInfo.iForwarded = EFalse;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallRemoteParty;
	
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(aCallId, aService, callInfo);
	data.Close();
	callInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo,KErrNone,data,0);

	// complete Dial
	TMockLtsyCallData0 dialCompleteData(aCallId,aService);
	data.Close();
	dialCompleteData.SerialiseL(data);
	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,data,0);

	TRequestStatus requestStatus ;
	aCall.Dial(requestStatus,callInfo.iDialledParty.iTelNumber);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(1);
	}

/**
Open packet service session.
@param aStartAttachStatus Starting attach status.
@param aStartAttachMode Starting atach mode.  The CTSY does not even cache this value so it does not really matter what it gets set to.
*/
void CCSatComponentTestBase::OpenPacketServiceL(RPacketService& aRPacketService, RPacketService::TStatus aStartAttachStatus, RPacketService::TAttachMode aStartAttachMode)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.ExpectL(EPacketNotifyStatusChange);
	
	TBool resumed = EFalse;
	TMockLtsyData2<RPacketService::TStatus , TBool> comNotifyStatus(aStartAttachStatus, resumed);
	comNotifyStatus.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketNotifyStatusChange,KErrNone, data);
	
	TBool expInitState = ETrue;	 
	TMockLtsyData1<TBool> expGetAttachMode(expInitState);
	data.Close();
	expGetAttachMode.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketGetAttachMode, data);
	
	TMockLtsyData1<RPacketService::TAttachMode> comGetAttachMode(aStartAttachMode);
	data.Close();
	comGetAttachMode.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketGetAttachMode,KErrNone, data);
	
	//this is actually a sync call to LTSY where return represents a context id offset rather than error code.
	iMockLTSY.ExpectL(EPacketInitProxiesIPC, 3);	
	
	//this actually completes before all messages between CSAT<->LTSY have finished.
	TInt err = aRPacketService.Open(iPhone);
		
	// Wait for the end of initialisation
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(); // data
	}
	

/**
Context initiation
@param aPacketContext context object
@param aContextName context name
*/
TInt CCSatComponentTestBase::InitContextL(RPacketContext& aPacketContext, TInfoName& aContextName, TInfoName aHostCidName)
{
	RBuf8 data;
	CleanupClosePushL(data);

	// data for initialisation of the context 
	TRequestStatus initStatus;
	RPacketContext::TDataChannelV2 dataCh;
	TPckg<RPacketContext::TDataChannelV2> pckgInit(dataCh);	
    TMockLtsyData2< TInfoName, TInfoName > ltsyInit(aContextName, aHostCidName);
	ltsyInit.SerialiseL(data);	
	iMockLTSY.ExpectL(EPacketContextInitialiseContext, data);
	
	TMockLtsyData1< TInfoName > ltsyData(aContextName);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketContextInitialiseContext, KErrNone, data);
	//initialisation of the context
	aPacketContext.InitialiseContext(initStatus, pckgInit);
	User::WaitForRequest(initStatus);
	
	CleanupStack::PopAndDestroy(1); // data
	return initStatus.Int();
}



/**
Context activation
@param aContextName context name
*/
TInt CCSatComponentTestBase::SetContextActiveL(TInfoName& aContextName)
{
	RBuf8 data;
	CleanupClosePushL(data);

	//data for CompleteL    
 	TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusActive;
 	TMockLtsyData2 <TInfoName, TContextMisc>
 														 ltsyData1(aContextName, completeMisc);
	ltsyData1.SerialiseL(data);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);	
	
	CleanupStack::PopAndDestroy(1); // data
	
	return mockLtsyStatus.Int();
}


/**
Set context to new status
@param aContextName context name
@param aStatus status to set 
*/
TInt CCSatComponentTestBase::SetContextStatusL(TInfoName& aContextName, RPacketContext::TContextStatus aStatus)
{
	RBuf8 data;
	CleanupClosePushL(data);

	//data for CompleteL    
 	TContextMisc  completeMisc;
    completeMisc.iStatus = aStatus;
 	TMockLtsyData2 <TInfoName, TContextMisc>
 														 ltsyData1(aContextName, completeMisc);
	ltsyData1.SerialiseL(data);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);	
	
	CleanupStack::PopAndDestroy(1); // data
	
	return mockLtsyStatus.Int();
}



/**
Create QoS and set profile params
@param aPacketQoS packet qos object
@param aPacketContext context object
@param aRequested profile
*/

TInt CCSatComponentTestBase::CreateQoSAndSetProfileParamLC(RPacketQoS& aPacketQoS, 
													       RPacketContext& aPacketContext, 
													       TInfoName& aContextName,
														   TPacketDataConfigBase& aRequested,
														   TInfoName aPrimaryContextName)
{
	// ------ create New QoS -------------------------------------------------
	TName qosName;
	aPacketQoS.OpenNewQoS(aPacketContext, qosName);
	CleanupClosePushL(aPacketQoS);	
	
	RBuf8 expectDataSPP;
	CleanupClosePushL(expectDataSPP);
	
	TInt ret(KErrArgument); 	
 	
 	// Get Context current status 
 	// test 1: context not inited
    RPacketContext::TContextStatus contextStatus;    
    TBool statusChanged = EFalse;
    aPacketContext.GetStatus(contextStatus);
    
    //check  Context current status 
    if ( RPacketContext::EStatusActivating != contextStatus &&
             RPacketContext::EStatusActive != contextStatus)
    	{ 	   		
    		// and change it to RPacketContext::EStatusActive     		
    		SetContextActiveL(aContextName);  
    		statusChanged = ETrue;  		
    	}  
 	
 	
	// ------ SetProfileParameters --------------------------------------------
	TRequestStatus requestStatus;	
	
	if (TPacketDataConfigBase::KConfigGPRS == aRequested.ExtensionId())
	{
		RPacketQoS::TQoSGPRSRequested*  req;
		req = reinterpret_cast< RPacketQoS::TQoSGPRSRequested* > ( &aRequested );
		TPckg<RPacketQoS::TQoSGPRSRequested> pckg(*req);
		// prepare data for expectDataSPP ... 
		// ... context status
		RPacketContext::TContextStatus contextStatus;
		aPacketContext.GetStatus(contextStatus);
		AssertMockLtsyStatusL();
		// ... get  contextConfig
		RPacketContext::TContextConfigGPRS contextConfig;
		TPckg< RPacketContext::TContextConfigGPRS > pckgContextConfig = 
		                   ( TPckg< RPacketContext::TContextConfigGPRS > ) contextConfig;	
		aPacketContext.GetConfig(requestStatus, pckgContextConfig);
		User::WaitForRequest(requestStatus);
		AssertMockLtsyStatusL();	
		
		// data for SetProfileParameters's ExpectL		
		TContextParams	contextParams;
		contextParams.iContextName = aContextName;
		contextParams.iContextType = contextConfig.iNWIContext;
		contextParams.iPdpType = contextConfig.iPdpType;
		contextParams.iPrimaryContextName = aPrimaryContextName;
		contextParams.iContextStatus = contextStatus;		
		TMockLtsyData2<TContextParams, RPacketQoS::TQoSGPRSRequested> 
		                    expExpect(contextParams, *req);
	    expExpect.SerialiseL(expectDataSPP);
		
		iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectDataSPP);
		aPacketQoS.SetProfileParameters(requestStatus, pckg);
		User::WaitForRequest(requestStatus);					
		AssertMockLtsyStatusL();
		ret = KErrNone;
	}
	
	if (TPacketDataConfigBase::KConfigRel99Rel4 == aRequested.ExtensionId())
	{
		RPacketQoS::TQoSR99_R4Requested* req;
		req = reinterpret_cast< RPacketQoS::TQoSR99_R4Requested* > ( &aRequested );		
		TPckg<RPacketQoS::TQoSR99_R4Requested> pckg(*req);
		
		// prepare data for expectDataSPP ... 
		// ... context status
		RPacketContext::TContextStatus contextStatus;
		aPacketContext.GetStatus(contextStatus);
		// ... get  contextConfig
		RPacketContext::TContextConfigR99_R4 contextConfig;
		TPckg< RPacketContext::TContextConfigR99_R4 > pckgContextConfig = 
		                   ( TPckg< RPacketContext::TContextConfigR99_R4 > ) contextConfig;	
		aPacketContext.GetConfig(requestStatus, pckgContextConfig);
		User::WaitForRequest(requestStatus);	
		
		// data for SetProfileParameters's ExpectL
		TInfoName primaryContextName;	
		TContextParams	contextParams;
		contextParams.iContextName = aContextName;
		contextParams.iContextType = contextConfig.iNWIContext;
		contextParams.iPdpType = contextConfig.iPdpType;
		contextParams.iPrimaryContextName = primaryContextName;
		contextParams.iContextStatus = contextStatus;		
		TMockLtsyData2<TContextParams, RPacketQoS::TQoSR99_R4Requested> 
		                    expExpect(contextParams, *req);
		expectDataSPP.Close();                    
	    expExpect.SerialiseL(expectDataSPP);
		
		iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectDataSPP);		
		aPacketQoS.SetProfileParameters(requestStatus, pckg);
		User::WaitForRequest(requestStatus);
		AssertMockLtsyStatusL();
		ret = KErrNone;
	}
	
	if (TPacketDataConfigBase::KConfigRel5 == aRequested.ExtensionId())
	{
		RPacketQoS::TQoSR5Requested* req;
		req = reinterpret_cast< RPacketQoS::TQoSR5Requested* > ( &aRequested );		
		TPckg<RPacketQoS::TQoSR5Requested> pckg(*req);
		
		// prepare data for expectDataSPP ... 
		// ... context status
		RPacketContext::TContextStatus contextStatus;
		aPacketContext.GetStatus(contextStatus);
		// ... get  contextConfig
		RPacketContext::TContextConfig_R5 contextConfig;
		TPckg< RPacketContext::TContextConfig_R5 > pckgContextConfig = 
		                   ( TPckg< RPacketContext::TContextConfig_R5 > ) contextConfig;	
		aPacketContext.GetConfig(requestStatus, pckgContextConfig);
		User::WaitForRequest(requestStatus);	
		
		// data for SetProfileParameters's ExpectL
		TInfoName primaryContextName;	
		TContextParams	contextParams;
		contextParams.iContextName = aContextName;
		contextParams.iContextType = contextConfig.iNWIContext;
		contextParams.iPdpType = contextConfig.iPdpType;
		contextParams.iPrimaryContextName = primaryContextName;
		contextParams.iContextStatus = contextStatus;		
		TMockLtsyData2<TContextParams, RPacketQoS::TQoSR5Requested> 
		                    expExpect(contextParams, *req);
	    expectDataSPP.Close();
	    expExpect.SerialiseL(expectDataSPP);
		
		iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectDataSPP);	
		
		aPacketQoS.SetProfileParameters(requestStatus, pckg);
		User::WaitForRequest(requestStatus);
		AssertMockLtsyStatusL();
		ret = KErrNone;
	}
    
    
    // if context status was changed
    if (statusChanged)
    	{
    	// return to previous status
    	SetContextStatusL(aContextName, contextStatus);
    	}
    
    CleanupStack::PopAndDestroy(1);// expectDataSPP    
    // aPacketQoS is still in CleanupStack !!!	
	return ret;
}


/**
Set status to RPacketService
@param aStatus status to set
@param aIsResumed -- is resumed
*/
TInt CCSatComponentTestBase::SetPacketServiceStatusL(RPacketService::TStatus& aStatus, TBool aIsResumed)
	{
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	// Data for CompleteL
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(aStatus, aIsResumed);
	ltsyData.SerialiseL(data);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
		
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	
	CleanupStack::PopAndDestroy(1); // data
	
	return mockLtsyStatus.Int();	
	}
	
/**
Add filter to RContext
@param aContext RPacketContext use to add filter 
@param aPacketFilterInfo -- Reference to a TPacketFilterV2 class packaged inside a TPckg<> class.
*/
TInt CCSatComponentTestBase::AddContextPacketFilterL(RPacketContext& aContext, 
													TInfoName& aContextName, 
													RPacketContext::TPacketFilterV2& aFilter, 
													TBool aIsFailInCSAT)
	{
	
	RBuf8 data;
	CleanupClosePushL(data);
		
	TRequestStatus requestStatus;
	
	RPacketContext::TPacketFilterV2Pckg filterPckg (aFilter);

    // if request reaches LTSY
	if (!aIsFailInCSAT)
		{
		//data for ExpectL
		RPacketContext::TContextConfigGPRS configGPRS;
		CArrayFixFlat< RPacketContext::TPacketFilterV2 >* filterArray = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::TPacketFilterV2>( 1 );
	    CleanupStack::PushL(filterArray);

		filterArray->AppendL( aFilter );
		
		TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >* > 
					packetFilterData( configGPRS, 0, aContextName, filterArray );		
		packetFilterData.SerialiseL(data);		 
		iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);
		
		//data for CompleteL
		TMockLtsyData1<TInfoName > contexName(aContextName);
    	data.Close();
    	contexName.SerialiseL(data); 			
		iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);
		
		CleanupStack::PopAndDestroy(1); // filterArray
		}    
		
	aContext.AddPacketFilter(requestStatus, filterPckg);		
	User::WaitForRequest(requestStatus);	
	
	CleanupStack::PopAndDestroy(1); // data
	
	return requestStatus.Int();
	}

TInt CCSatComponentTestBase::OpenNewCall(RLine& aLine, RCall& aCall, const TDesC& aLineName)
    {
    TName name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(aLineName);
    name.Append(KDoubleColon);

   	return aCall.OpenNewCall(aLine, name);
    }

TInt CCSatComponentTestBase::OpenNewCall(RLine& aLine, RCall& aCall, const TDesC& aLineName, TName& aNewName)
    {
    aNewName = KMmTsyPhoneName;
    aNewName.Append(KDoubleColon);
    aNewName.Append(aLineName);
    aNewName.Append(KDoubleColon);

   	return aCall.OpenNewCall(aLine, aNewName);
    }

TInt CCSatComponentTestBase::CreateIncomingCall(RLine& aLine, 
                                            const TInt aCallId, 
                                            const TDesC& aLineName,
                                            TName& aIncomingCallName,
                                            RMobilePhone::TMobileService aMobileService)
	{
    TInt errorCode = KErrNone;
    _LIT(KDoubleColon, "::");

    TName name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(aLineName);
    name.Append(KDoubleColon);
	
	RCall call;
    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TRequestStatus requestNotify;
    TRequestStatus mockLtsyStatus;

   	errorCode = call.OpenNewCall(aLine, name);
   	CleanupClosePushL(call);

   	if (errorCode != KErrNone)
   		return errorCode;

	aLine.NotifyIncomingCall(requestNotify, aIncomingCallName);

	// Complete NotifyIncomingCall
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = aMobileService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(aCallId, aMobileService, mobileCallInfo);
    mockCallData1.SerialiseL(completeData);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyIncomingCallIndId, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestNotify);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestNotify.Int());
    
    // Complete NotifyCallStatusChange with ringing status
    RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusRinging;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(aCallId, aMobileService, callStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId , KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	call.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // data, completeData 
	
	return errorCode;
	}

void CCSatComponentTestBase::CloseIncomingCall(RCall& aCall, const TInt aCallId,
                                           RMobilePhone::TMobileService aMobileService)
	{
    RBuf8 expectData;
    CleanupClosePushL(expectData);

	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(aCallId, aMobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
    
	aCall.Close();
	AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(1); // expectData 
	}
	
	
void CCSatComponentTestBase::CloseLineL(RLine& aLine, TInt aCallId, RMobilePhone::TMobileService aMobileService)
	{
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	// Get Call Status	
	RCall::TStatus callStatus;
	aLine.GetStatus(callStatus);

	// Hang up call if call is still connecting 
	// so in this case we have to prepare expectData
	if ( !( RMobileCall::EStatusIdle == callStatus ||
            RMobileCall::EStatusDisconnecting == callStatus ||
            RMobileCall::EStatusDisconnectingWithInband == callStatus )
           && aCallId !=0 && aMobileService != RMobilePhone::EServiceUnspecified)
		{
		// prepare data for close line
		TInt symbianHangUpCause( KErrGsmReleaseByUser );
		TBool autoStChangeDisable = EFalse;   
		TMockLtsyCallData2<TInt, TBool> expData(aCallId, aMobileService, symbianHangUpCause, autoStChangeDisable);
		expData.SerialiseL(expectData); 
		iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
		}	

    // close line
   	aLine.Close();   
   	
   	CleanupStack::PopAndDestroy(1); // expectData 	
	}

//  ExpectL and CopmleteL must precede this function.
void CCSatComponentTestBase::CallGetMobileCallInfoL(TInt aCallId, 
                                               RMobilePhone::TMobileService aService, 
                                               const TDesC &aTelNumber)
	{
	_LIT(KNullDesC , "");
	
	RBuf8 data;
	CleanupClosePushL(data);

	RMobileCall::TMobileCallInfoV1 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(aTelNumber);
	callInfo.iService = aService;
	callInfo.iEmergency = EFalse;
	callInfo.iRemoteParty.iCallingName = KNullDesC;
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityUnknown;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNullDesC;
	callInfo.iRemoteParty.iRemoteNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
	callInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber = RMobilePhone::EUnknownNumber;
	callInfo.iForwarded = EFalse;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallRemoteParty;

	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(aCallId, aService, callInfo);
	callInfoData.SerialiseL(data);
		
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    
    CleanupStack::PopAndDestroy(1);
	}

CActiveScheduler* CCSatComponentTestBase::InstallSchedulerLC()
	{
	CActiveScheduler* scheduler=new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);	
	return scheduler;
	}



/**
 * Utility function which handles all the Expects/Completes generated by Opening an RSat object.
 */
void CCSatComponentTestBase::OpenSatL(TUint8 aEnabled)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId);
	
	TBool smsMoControlActivated = aEnabled & KSmsMoControlEnabled;
	TMockLtsyData1<TBool> comLtsyData(smsMoControlActivated);
	comLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId, KErrNone, data, KDefaultDelay);

	TUint8 pcmdCode = KPollInterval;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );

	pcmdCode = KPollingOff;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KMoreTime;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	pcmdCode = KTimerManagement;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
	
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);
	
	_LIT(KSerialNumber, "12345678");
	
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	phoneId.iSerialNumber = KSerialNumber;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdentityV1> phoneIdData(phoneId);
	data.Close();
	phoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId, KErrNone, data,KDefaultDelay);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId);

	TBuf8<KMaxLengthDefaultBearerData> bearerCapabilities(_L8("TEST BEARER!"));
	TDesC8* bearerCapabilitiesPtr = &bearerCapabilities;
	TMockLtsyData1<TDesC8*> bearerCapabilitiesData(bearerCapabilitiesPtr);
	data.Close();
	bearerCapabilitiesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId, KErrNone, data, KDefaultDelay);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId);

	TBool ussdSupported = aEnabled & KUssdEnabled;
	TMockLtsyData1<TBool> ussdSupportedData(ussdSupported);
	data.Close();
	ussdSupportedData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId, KErrNone, data, KDefaultDelay);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatUssdControlSupportedChangeIndId, KErrNone, data, KDefaultDelay); //do a future indicator completion as well as this could happen at some point
	
	iMockLTSY.ExpectL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId,KErrNone, KDefaultDelay);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId);

	TBool smsPpDdlSupported = aEnabled & KSmsPpDownloadEnabled;
	TMockLtsyData1<TBool> smsPpData(smsPpDdlSupported);
	data.Close();
	smsPpData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId, KErrNone, data, KDefaultDelay);
		
	User::LeaveIfError(iSat.Open(iPhone));

	WaitForMockLTSYTerminated();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(&data); 
	}


/**
Prepares MockLtsy with expected IPC data for proactive command notification
@param aPCmdNumber proactive command number
*/
void CCSatComponentTestBase::PrepareMockWithExpDataForPCmdNotifyL( TUint8 aPCmdNumber )
	{
	TPckg<TUint8> pcmdCodePckg(aPCmdNumber);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, pcmdCodePckg);
	}


/**
Prepares MockLtsy with data for completion of proactive command notification
@param aCompleteTlv proactive command Tlv
@param aError proactive command notification result code
*/
void CCSatComponentTestBase::PrepareMockWithCompleteDataForPCmdNotifyL( TPtrC8 aCompleteTlv, TInt aError, TInt aDelay )
	{
	RBuf8 data;
	CleanupClosePushL(data);	

	TDesC8* tlvPtr = &aCompleteTlv;
	
	TMockLtsyData1<TDesC8*> dspTxtDscPack(tlvPtr);
	dspTxtDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aError, data, aDelay);

	CleanupStack::PopAndDestroy(&data);
	}


/**
Fills in a proactive command Tlv with command details and device identities
@param aTlv Tlv to fill in
@param aPCmdNumber proactive command number
@param aTypeOfCommand type of proactive command
@param aCommandQualifier proactive command qualifier
@param aDestDeviceId command destination device Id
*/
void ProactiveCmdTlvBegin( 
		TTlv& aTlv, 
		TUint8 aPCmdNumber,
		TUint8 aTypeOfCommand,
		TUint8 aCommandQualifier,
		TUint8 aDestDeviceId)
	{
	aTlv.Begin(KBerTlvProactiveSimCommandTag);
	aTlv.AddTag(KTlvCommandDetailsTag);
	aTlv.AddByte(aPCmdNumber);//ETLV_CommandNumber
	aTlv.AddByte(aTypeOfCommand); //ETLV_TypeOfCommand
	aTlv.AddByte(aCommandQualifier); //ETLV_CommandQualifier
	
	if ( KDeviceIdNotSet != aDestDeviceId )
		{
		aTlv.AddTag(KTlvDeviceIdentityTag); 
		aTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
		aTlv.AddByte(aDestDeviceId); //ETLV_DestinationDeviceIdentity
		}
	}


/**
Generates a terminal response Tlv
@param aBuf A buffer to fill in Tlv
@param aPCmdNumber proactive command number
@param aTypeOfCommand type of proactive command
@param aCommandQualifier proactive command qualifier
@param aGeneralResult result code of proactive command processing
@param aGeneralResultAddInfo additional command result information
@param aAdditionalInfoTlv additional tlv data
*/
void CCSatComponentTestBase::GenerateTermRspTlv(
		RBuf8& aBuf, 
		TUint8 aPCmdNumber,
		TUint8 aTypeOfCommand, 
		TUint8 aCommandQualifier, 
		RSat::TPCmdResult aGeneralResult, 
		const TDesC& aGeneralResultAddInfo, 
		const TDesC8& aAdditionalInfoTlv)
	{
	TTlv tlv;
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(aPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(aTypeOfCommand); //ETLV_TypeOfCommand
	tlv.AddByte(aCommandQualifier); //ETLV_CommandQualifier

	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KMe);
	tlv.AddByte(KSim);
	
	tlv.AddTag(KTlvResultTag);
	tlv.AddByte(aGeneralResult);
	TUint genResAddInfoLength = aGeneralResultAddInfo.Length();
	for (TUint i = 0; i < genResAddInfoLength; i++)
		{
		tlv.AddByte( static_cast< TUint8 >(aGeneralResultAddInfo[i]) );
		}

	aBuf.Zero();
	aBuf.Append(tlv.GetDataWithoutTopLevelTag());
	aBuf.Append(aAdditionalInfoTlv);
	}


/**
Prepares MockLtsy with expected IPC data for terminal response
@param aPCmdNumber proactive command number
@param aTypeOfCommand type of proactive command
@param aCommandQualifier proactive command qualifier
@param aGeneralResult result code of proactive command processing
@param aGeneralResultAddInfo additional command result information
@param aAdditionalInfoTlv additional tlv data
@param aResultForExpect result value for ExpectL() 
*/
void CCSatComponentTestBase::PrepareTerminalResponseMockDataL(
		TUint8 aPCmdNumber,
		TUint8 aTypeOfCommand,
		TUint8 aCommandQualifier,
		const TDesC& aGeneralResultAddInfo,
		RSat::TPCmdResult aGeneralResult,
		const TDesC8& aAdditionalInfoTlv,
		const TInt aResultForExpect)
	{
	RBuf8 data;
	CleanupClosePushL(data);	
	
	RBuf8 terminalRsp;
	CleanupClosePushL(terminalRsp);
	terminalRsp.CreateL(KTlvMaxSize);
	
	GenerateTermRspTlv(
			terminalRsp,
			aPCmdNumber,
			aTypeOfCommand,
			aCommandQualifier,
			aGeneralResult,
			aGeneralResultAddInfo,
			aAdditionalInfoTlv);
	
	TDesC8* terminalRspPtr = &terminalRsp;
	
	TMockLtsyData1<TDesC8*> termRespData(terminalRspPtr);
	termRespData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data, aResultForExpect);
	
	CleanupStack::PopAndDestroy(&terminalRsp);
	CleanupStack::PopAndDestroy(&data);
	}


/**
Sends a terminal response.
@param aRSatTypeOfCommand proactive command number
@param aRspPckg packaged data for terminal response
@param aExpResult expected result of TerminalRsp() execution
*/
void CCSatComponentTestBase::TerminalResponseL(
		RSat::TPCmd aRSatTypeOfCommand,
		const TDesC8& aRspPckg, 
		const TInt aExpResult)
	{
	TRequestStatus reqStatus;
	iSat.TerminalRsp(reqStatus, aRSatTypeOfCommand, aRspPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(aExpResult, reqStatus.Int());
	}
	
	
/**
Handles sending a terminal response
@param aPCmdNumber proactive command number
@param aTypeOfCommand type of proactive command
@param aCommandQualifier proactive command qualifier
@param aRSatTypeOfCommand proactive command number
@param aRspPckg packaged data for terminal response
@param aGeneralResultAddInfo additional command result information
@param aGeneralResult result code of proactive command processing
@param aAdditionalInfoTlv additional tlv data
@param aExpResult expected result of TerminalRsp() execution
*/
void CCSatComponentTestBase::GenerateTerminalResponseL(
		TUint8 aPCmdNumber,
		TUint8 aTypeOfCommand,
		TUint8 aCommandQualifier,
		RSat::TPCmd aRSatTypeOfCommand,
		const TDesC8& aRspPckg,
		const TDesC& aGeneralResultAddInfo,
		RSat::TPCmdResult aGeneralResult,
		const TDesC8& aAdditionalInfoTlv,
		const TInt aExpResult)
	{
	PrepareTerminalResponseMockDataL(
			aPCmdNumber,
			aTypeOfCommand,
			aCommandQualifier,
			aGeneralResultAddInfo,
			aGeneralResult,
			aAdditionalInfoTlv,
			aExpResult);
	TerminalResponseL( aRSatTypeOfCommand, aRspPckg, aExpResult);
	}


/**
End Proactive Sim Session 
*/    
void CCSatComponentTestBase::EndPSimSessionL()
	{
	//this is as a result of the NotifyProactiveSimSessionEnd call down to the LTSY,
	//this tells the LTSY we are interested in a Sim Session End event
	//CSAT -> LTSY
	PrepareMockWithExpDataForPCmdNotifyL(KEndOfTheProactiveSession);
	
	//the client side call to be notifed when the Proactive Sim Session has ended.
	TRequestStatus reqStatus;
	iSat.NotifyProactiveSimSessionEnd(reqStatus);

	TUint8 statusWord1 = 0x90;
	TUint8 statusWord2 = 0x00;
	TMockLtsyData2<TUint8,TUint8> notifyDataPack(statusWord1,statusWord2);

	RBuf8 data;
	CleanupClosePushL(data);	
	notifyDataPack.SerialiseL(data);
	
	iMockLTSY.CompleteL(KMockLtsyDispatchSatNotifyProactiveSimSessionEndIndId, KErrNone, data);
	
	//wait for the client side to be notified
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(&data);
	}

/**
Test if a proactive command was performed successfully
*/
TBool CCSatComponentTestBase::CommandPerformedSuccessfully
        ( 
        TUint8 aGeneralResult 
        )
    {
    TBool ret( EFalse );
	switch ( aGeneralResult )
	    { 
        case RSat::KSuccess:
        case RSat::KPartialComprehension:
        case RSat::KMissingInformation:
        case RSat::KRefreshAdditionEFRead:
        case RSat::KSuccessRequestedIconNotDisplayed:
        case RSat::KModifiedByCallControl:
        case RSat::KSuccessLimitedService:
        case RSat::KPerformedWithModifications:
        case RSat::KRefreshUSIMNotActive:
        	{
    		ret = ETrue;
            break;
        	}
	    default:
	    	{	
		    ret = EFalse;
		    break;
	    	}
	    }
    return ret;
   }


/**
Compares 16bit descriptor with 8bit descriptor
@param aStringToCompareWith string to compare with
@param aStringToCompare string to compare
@return
Positive, if this descriptor is greater than the specified descriptor.
Negative, if this descriptor is less than the specified descriptor.
Zero, if both descriptors have the same length and the their contents are the same.
*/
TInt Compare16bitDesWith8bitDesL(const TDesC8& aStringToCompareWith, const TDesC16& aStringToCompare)
	{
	RBuf8 buf;

	buf.CreateL(aStringToCompare.Length());
	buf.Copy(aStringToCompare);

	TInt res = aStringToCompareWith.Compare(buf);
	
	buf.Close();
	
	return res;
	}


/**
Converts Unicode to packed 7 bit
@param aInput Input string
@param aOutput Output string
@return KErrNone or the luck space of output descriptor
*/
TInt CCSatComponentTestBase::UnicodeToPacked7L(const TDesC16& aInput, TDes8& aOutput)
	{
	TInt length( aInput.Length() );
	RBuf8 string;
	string.CleanupClosePushL();
	string.CreateL( length );
		
	// convert unicode to 7 bit SMS default alphabet
	CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewLC();

	RFs fileServSes;
	ASSERT_EQUALS(CCnvCharacterSetConverter::EAvailable,
			converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit, fileServSes));
	
	ASSERT_EQUALS(KErrNone,	converter->ConvertFromUnicode(string, aInput));
	
	CleanupStack::PopAndDestroy(converter);

	// pack 7 bit data
	TUint8 shift( 0 );
	TInt packedDataSize( 0 );
	
	for ( TInt i = 0; i < length; i++ )
		{
		// Get first character
		TUint8 char1 = static_cast<TUint8>( string[i] >> shift ); 
		TUint8 char2( 0 );
		
		if ( length > ( i + 1 ) ) // if not last symbol
			{
			// Get next character
			char2 = static_cast<TUint8>( string[i + 1]  << ( 7 - shift ) );
			}
		
		// update packed character
		string[ packedDataSize++ ] = static_cast<TUint8>( char1 | char2 );
	
		// character is 7bit coded, so the shift can not be more then 6
		if ( 6 == shift )
			{
			shift = 0;
			// the next symbol is allready packed (8 bytes packed in 7 bytes)
			// so let increase the counter
			i++;
			}
		else
			{
			shift++;
			}
		}
	
	string.SetLength( packedDataSize );
	
	if ( 0 == shift )
		{
		// 7 last bytes packed in 7 bytes, so there are 7 spare zero bits at
		// the end of the message. To avoid the situation where the receiving
		// entity confuses 7 binary zero pad bits as the @ character,
		// the carriage return (i.e. <CR>) character shall be used for padding
		// in this situation, as defined in TS 23.038 [5]. <CR> = 0x0D
		string[ packedDataSize - 1 ] =
				static_cast<TUint8>( string[ packedDataSize - 1 ] | (0x0D)<<1 );
		}
	
	TInt copySize = Min( packedDataSize, aOutput.MaxLength() );
	
	aOutput.Copy( string.Left( copySize ) );
	CleanupStack::PopAndDestroy(&string);
	
	return (packedDataSize == copySize)?KErrNone:(packedDataSize - copySize);
	}


