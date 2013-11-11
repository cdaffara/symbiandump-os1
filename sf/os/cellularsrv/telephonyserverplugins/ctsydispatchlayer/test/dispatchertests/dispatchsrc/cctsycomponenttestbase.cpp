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
 @file base class for CTSY component tests
*/
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include <ctsy/ltsy/mltsydispatchpacketservicesinterface.h>

#include "cctsycomponenttestbase.h"

#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <pcktcs.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/serviceapi/cmmutility.h>

#include <test/tmockltsydata.h>
#include "mockltsyindicatorids.h"
#include "testconstants.h"

_LIT(KMmTsyWithDispatcherModuleName, "phonetsywithdispatcher");

/**
To push a CleanupItem ont the cleanupstack to close the phone
*/
void CCtsyComponentTestBase::Cleanup(TAny* self)
	{
	static_cast<CCtsyComponentTestBase*>(self)->DoCleanup();
	}

/**
Close the Phone a the server connection
*/
void CCtsyComponentTestBase::DoCleanup()
	{
	iPhone.Close();

	TBool hasWaitingExpect;
	TBool hasPendingComplete;
	TBool hasError;
	iMockLTSY.GetStatus(hasWaitingExpect,hasPendingComplete,hasError);

	if (hasWaitingExpect || hasPendingComplete || hasError)
		{
		TBuf<255> log;
		do	{
			iMockLTSY.GetNextLogLine(log);
			INFO_PRINTF1(log);
			}
		while(log.Length() > 0);
		}
	
	ASSERT_FALSE(hasWaitingExpect);
	ASSERT_FALSE(hasPendingComplete);
	ASSERT_FALSE(hasError);
	
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
void CCtsyComponentTestBase::AssertMockLtsyStatusL()
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
 * Waits until the MockLTSY queued events is empty
 * Note that in some cases an outstanding completion may occur after the call to iMockLTSY.NotifyTerminated() 
 * but before the User::WaitForRequest() causing a hung test. In this case the solution is to set up the NotifyTerminated()
 * before the ETel call that generates the expect/context switch which causes the complete, and only then the WaitForRequest().
 */

void CCtsyComponentTestBase::WaitForMockLTSYTerminated()
	{
	TRequestStatus reqStatusTerminated;
	iMockLTSY.NotifyTerminated(reqStatusTerminated); 
	User::WaitForRequest(reqStatusTerminated);
	ASSERT_EQUALS(KErrNone, reqStatusTerminated.Int());
	}
	
	
/**
Open Server session
*/
void CCtsyComponentTestBase::OpenEtelServerL(TExtendedErrorClient aSetExtendedErrorClient)
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

void CCtsyComponentTestBase::OpenPhoneNegativeL(TInt aMode)
	{
	RMobilePhone::TMobilePhoneServiceTableV1 simServiceTable;
	simServiceTable.iServices1To8  = 0xFF;
	simServiceTable.iServices9To16 = 0xFF;
	simServiceTable.iServices17To24= 0xFF;
	simServiceTable.iServices25To32= 0xFF;
	simServiceTable.iServices33To40= 0xFF;
	simServiceTable.iServices41To48= 0xFF;
	simServiceTable.iServices49To56= 0xFF;

	RBuf8 data;
	CleanupClosePushL(data);

	TInt err = iPhone.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);

	//open SMS messaging session
	RMobileSmsMessaging sms;
	err = sms.Open(iPhone);
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
	if (aMode != MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId)
		{
		iMockLTSY.ExpectL(MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId);
		TBool pin1DisableSupport = ETrue;
		TMockLtsyData1<TBool> pin1DisableSupportData(pin1DisableSupport);
		data.Close();
		pin1DisableSupportData.SerialiseL(data);
		iMockLTSY.CompleteL(MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId,KErrNone,data,0);
		}

	//EMmTsySimRefreshRegisterIPC
	if (aMode != MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId)
		{
		TUint16 refreshFileList = KCacheAdn | KCacheFdn | KCacheServiceTable | KCacheALSline;
		TMockLtsyData1<TUint16> refreshFileListData(refreshFileList);
		data.Close();
		refreshFileListData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId,data);
		iMockLTSY.CompleteL(MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId,KErrNone);
		}
	
	//EMobilePhoneGetServiceTable
	if (aMode != MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId)
		{
		RMobilePhone::TMobilePhoneServiceTable serviceTable = RMobilePhone::ESIMServiceTable;
		TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> serviceTableData(serviceTable);
		data.Close();
		serviceTableData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId, data);
	
		TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableResultData(simServiceTable);
		data.Close();
		serviceTableResultData.SerialiseL(data);
		iMockLTSY.CompleteL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId,KErrNone,data);
		}
	
	// EMobilePhoneGetALSLine
	if (aMode != MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId)
		{
		iMockLTSY.ExpectL(MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId);
		RMobilePhone::TMobilePhoneALSLine alsLine = RMobilePhone::EAlternateLinePrimary;
		TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> alsLineData(alsLine);
		data.Close();
		alsLineData.SerialiseL(data);
		iMockLTSY.CompleteL(MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId,KErrNone,data,0);
		}
	
	// ECustomGetIccCallForwardingStatusIPC
	if (aMode != MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId)
		{
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
		}
	
	// EMobilePhoneGetIccMessageWaitingIndicators
	if (aMode != MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId)
		{
		iMockLTSY.ExpectL(MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId);
	
		RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
		TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
		                                indicatorsData(expectedMessageIndicators);
	    data.Close();
	    indicatorsData.SerialiseL(data);
		iMockLTSY.CompleteL(MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId, KErrNone, data);
		}
	
	if (aMode != MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId)
		{
		iMockLTSY.ExpectL(MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId);
	
		RMmCustomAPI::TAlsSupport alsSupport = RMmCustomAPI::EAlsSupportOff;
		TMockLtsyData1<RMmCustomAPI::TAlsSupport> alsLtsyData1(alsSupport);
		data.Close();
		alsLtsyData1.SerialiseL(data);
		iMockLTSY.CompleteL(MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId, KErrNone, data);
		}

	if (aMode != MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId)
		{
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
		}
	
	iMockLTSY.CompleteL(KMockLtsyDispatchSecurityBootNotifySecurityReadyIndId, KErrNone);
	
	User::WaitForRequest(mockLtsyStatus);

	CleanupStack::PopAndDestroy(1); // data
	}

/**
Standard function to open the phone up to the end of the boot sequence
*/
void CCtsyComponentTestBase::OpenPhoneL()
	{
	RMobilePhone::TMobilePhoneServiceTableV1 simServiceTable;
	simServiceTable.iServices1To8  = 0xFF;
	simServiceTable.iServices9To16 = 0xFF;
	simServiceTable.iServices17To24= 0xFF;
	simServiceTable.iServices25To32= 0xFF;
	simServiceTable.iServices33To40= 0xFF;
	simServiceTable.iServices41To48= 0xFF;
	simServiceTable.iServices49To56= 0xFF;
	
	OpenPhoneL(simServiceTable);
	}

void CCtsyComponentTestBase::OpenPhoneL(RMobilePhone::TMobilePhoneServiceTableV1& aSimServiceTable)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TInt err = iPhone.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);

	//open SMS messaging session
	RMobileSmsMessaging sms;
	err = sms.Open(iPhone);
	ASSERT_EQUALS(KErrNone, err);

	//open ADN phonebook
	RMobilePhoneBookStore pbFdn;
//	err = pbFdn.Open(iPhone, KETelIccAdnPhoneBook);
//	ASSERT_EQUALS(KErrNone, err);

	//open FDN phonebook
	RMobilePhoneBookStore pbAdn;
//	err = pbAdn.Open(iPhone, KETelIccFdnPhoneBook);
//	ASSERT_EQUALS(KErrNone, err);

	TRequestStatus modemDetectedStatus;
	RPhone::TModemDetection modemDetection;
	iPhone.NotifyModemDetected(modemDetectedStatus,modemDetection);



	err=iMockLTSY.Connect();
	ASSERT_EQUALS(KErrNone, err);
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);




	// EMmTsyBootNotifyModemStatusReadyIPC
	//iMockLTSY.ExpectL(KLtsyDispatchPhoneBootNotifyModemStatusReadyId);

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

	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableResultData(aSimServiceTable);
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

	iMockLTSY.CompleteL(KMockLtsyDispatchSecurityBootNotifySecurityReadyIndId, KErrNone);

	User::WaitForRequest(mockLtsyStatus);

	CleanupStack::PopAndDestroy(1); // data
	
	}

void CCtsyComponentTestBase::OpenCustomApiLC(RMmCustomAPI& aCustomAPI)
    {
    TRequestStatus reqStatus;

    iMockLTSY.NotifyTerminated(reqStatus);
    
    RBuf8 data;
    CleanupClosePushL(data);
   	
	iMockLTSY.ExpectL(MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId);
	RMobilePhone::TMobilePhoneCFUIndicatorV6 indicator;
	indicator.iMultipleSubscriberProfileID = RMobilePhone::KProfileIdentityTwo;
	indicator.iIndicator = RMobilePhone::KCFUIndicatorVoice;
	indicator.iCFNumber.iNumberPlan = RMobilePhone::ETelexNumberPlan;
	indicator.iCFNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	_LIT(KTelNumber, "012345678");
	indicator.iCFNumber.iTelNumber = KTelNumber; 
	TMockLtsyData1<RMobilePhone::TMobilePhoneCFUIndicatorV6> indicatorData(indicator);
	indicatorData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId,KErrNone,data,0);
	CleanupStack::PopAndDestroy(&data);
	
	CleanupClosePushL(aCustomAPI);
	TInt ret = aCustomAPI.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
    }


void CCtsyComponentTestBase::OpenPacketServiceLC(RPacketService& aPacketService, RPacketService::TStatus aPacketServiceStatus)
	{
	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId);	

	TUint staticCapabilities        
		= RPacketService::KCapsSuspendSupported
		| RPacketService::KCapsNetworkAvailabilitySupported
		| RPacketService::KCapsSetDefaultContextSupported
		| RPacketService::KCapsChangeAttachModeSupported
		| RPacketService::KCapsGetDataTransferredSupported
		| RPacketService::KCapsPreferredBearerSupported
		| RPacketService::KCapsPdpDataCompSupported
		| RPacketService::KCapsPdpHeaderCompSupported
		| RPacketService::KCapsMSClassSupported
		| RPacketService::KCapsNotifyMSClassSupported;
	
	TMockLtsyData1<TUint> packetServiceStaticCapabilities(staticCapabilities);
	packetServiceStaticCapabilities.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId, KErrNone, data);
	data.Close();
	
	TBool resumed = EFalse; // Flag indicating whether packet service has been resumed from suspended state.
	TMockLtsyData2<RPacketService::TStatus, TBool> packetServiceStatusData(aPacketServiceStatus, resumed);
	packetServiceStatusData.SerialiseL(data);
  	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId, KErrNone, data);
  	data.Close();
  	
  	TInt maxNoMonitoredServiceLists = 32;
	TMockLtsyData1<TInt> maxNoMonitoredServiceListsData(maxNoMonitoredServiceLists);
	maxNoMonitoredServiceListsData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId, KErrNone, data);
	data.Close();
	
	TInt maxNoActiveServices = 64;
	TMockLtsyData1<TInt> maxNoActiveServicesData(maxNoActiveServices);
	maxNoActiveServicesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId, KErrNone, data);
   	CleanupStack::PopAndDestroy(&data);

	CleanupClosePushL(aPacketService);
	ASSERT_EQUALS(aPacketService.Open(iPhone),KErrNone);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	AssertMockLtsyStatusL();
	
	
	//check the capabilities
	TUint caps = 0;
	ASSERT_EQUALS(aPacketService.GetStaticCaps(caps,RPacketContext::EPdpTypeIPv4),KErrNone);
	ASSERT_EQUALS(caps,staticCapabilities);
	
	//check the max no monitored service lists
	
	TInt count = 0;
	TInt maxAllowed = 0;
	aPacketService.EnumerateMbmsMonitorServiceList(reqStatus,count,maxAllowed);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),KErrNone);
	ASSERT_EQUALS(maxAllowed,maxNoMonitoredServiceLists);
	ASSERT_EQUALS(count,0);
	}

/**
Context activation
@param aContextName context name
*/

TInt CCtsyComponentTestBase::SetContextActiveL(TInfoName& aContextName)
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
    iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextStatusChangeIndId, KErrNone, data);
    // wait for completion
    User::WaitForRequest(mockLtsyStatus);   
    
    CleanupStack::PopAndDestroy(1); // data
    
    return mockLtsyStatus.Int();
}

/**
Sends ExpectL and CompleteL commands to MockLtsy for getters of various network information.
These getters are internal to the CtsyDispatcher.
This method should be called immediately after the CompleteL for KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId,
since these getters are triggered on the CtsyDispacther receiving a BootNotifyModemStatusReady indication.
*/
void CCtsyComponentTestBase::DriverPrepInternalGettersMockLtsyL()
	{
	RBuf8 data;
	CleanupClosePushL(data);

	//TRequestStatus mockLtsyStatus;

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
	//AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy();  // data
	}

TInt CCtsyComponentTestBase::OpenLineAndCallL(
	RMobileLine& aMobileLine, RMobileCall& aMobileCall, RMobilePhone::TMobileService aMode)
	{
	TInt ret = KErrGeneral;

	if (aMode == RMobilePhone::EVoiceService)
		{
		ret = aMobileLine.Open(iPhone, KMmTsyVoice1LineName);
		}
	else if (aMode == RMobilePhone::EAuxVoiceService)
		{
		ret = aMobileLine.Open(iPhone, KMmTsyVoice2LineName);
		}
	else
		{
		ret = aMobileLine.Open(iPhone, KMmTsyDataLineName);
		}

	if (ret != KErrNone)
		{
		return ret;
		}

	_LIT(KDoubleColon, "::");
	TName name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    if (aMode == RMobilePhone::EVoiceService)
    	{
    	name.Append(KMmTsyVoice1LineName);
    	}
    else if (aMode == RMobilePhone::EAuxVoiceService)
    	{
    	name.Append(KMmTsyVoice2LineName);
    	}
    else
    	{
    	name.Append(KMmTsyDataLineName);
    	}
    name.Append(KDoubleColon);

	ret = aMobileCall.OpenNewCall(aMobileLine, name);

	return ret;
	} // CCtsyComponentTestBase::OpenLineAndCallL


void CCtsyComponentTestBase::DriverExpectDialCallRequestL(
		TInt aCallId, RMobilePhone::TMobileService aMode, TInt aDialError,
		TBool aIsIsvCall, TBool aIsSimOriginated, TBool aPerformFdnCheck)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobileAddress dialledParty;
	dialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
	dialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;
	dialledParty.iTelNumber = KPhoneNumber;

	if (aMode == RMobilePhone::EVoiceService || aMode == RMobilePhone::EAuxVoiceService)
		{
		RMobilePhone::TMobilePhoneALSLine line = RMobilePhone::EAlternateLinePrimary;

		if (aMode == RMobilePhone::EAuxVoiceService)
			{
			line = RMobilePhone::EAlternateLineAuxiliary;
			}

		RMobileCall::TMobileCallParamsV1 callParams;
		RMobileCall::TMobileCallParamsV1Pckg callParamsPckg(callParams);
		callParams.iIdRestrict = RMobileCall::ESendMyId;

		if (aIsIsvCall)
			{
			callParams.iAutoRedial = EFalse;
			RMobileCall::TMobileCallCugV1 cug;
			callParams.iCug = cug;
			}
		else
			{
			callParams.iAutoRedial = ETrue;
			}
		RMobileCall::TCallParamOrigin callOrigin = aIsSimOriginated ? RMobileCall::EOriginatorSIM : RMobileCall::EOriginatorEtelClient; 

		TMockLtsyData6<RMobilePhone::TMobilePhoneALSLine, RMobilePhone::TMobileAddress,
			RMobileCall::TMobileCallParamsV1, TBool, RMobileCall::TCallParamOrigin, TBool>
		    			dialData (line, dialledParty, callParams, aIsIsvCall, callOrigin, aPerformFdnCheck);
		dialData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchCallControlDialVoice::KLtsyDispatchCallControlDialVoiceApiId, data);
		TMockLtsyCallData0 dialData2(aCallId, aMode); // Mode not actually used
		data.Close();
		dialData2.SerialiseL(data);
	    iMockLTSY.CompleteL(MLtsyDispatchCallControlDialVoice::KLtsyDispatchCallControlDialVoiceApiId, aDialError, data);
		}
	else
		{
		RMobileCall::TMobileDataCallParamsV1 callParams;
		callParams.iAckTimer = 3;
		callParams.iAutoRedial = ETrue;
	    callParams.iInterval = 4;
	    callParams.iCug.iCugIndex = 0xFFFF;
	    callParams.iService = RMobileCall::EServiceDataCircuitAsync;

	    TMockLtsyData3<RMobilePhone::TMobileAddress, RMobileCall::TMobileDataCallParamsV1, TBool>
	    				dialData(dialledParty, callParams, aPerformFdnCheck);
		dialData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchCallControlDialData::KLtsyDispatchCallControlDialDataApiId, data);

		TMockLtsyCallData0 dialData2(aCallId, aMode); // Mode not actually used
		data.Close();
		dialData2.SerialiseL(data);
	    iMockLTSY.CompleteL(MLtsyDispatchCallControlDialData::KLtsyDispatchCallControlDialDataApiId, aDialError, data);
		}

	data.Close();
	CleanupStack::PopAndDestroy(1, &data);
	} // CCtsyComponentTestBase::DriverExpectDialCallRequestL


void CCtsyComponentTestBase::DriverCompleteGetMobileCallInfoL(TInt aCallId, RMobilePhone::TMobileService aService)
	{
	_LIT(KPhoneNumber, "12345");

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileCall::TMobileCallInfoV1 callInfo;
	callInfo.iCallId = aCallId;
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = aService;
	callInfo.iEmergency = EFalse;
	callInfo.iRemoteParty.iCallingName = KNullDesC;
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KPhoneNumber;
	callInfo.iRemoteParty.iRemoteNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
	callInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber = RMobilePhone::EUnknownNumber;
	callInfo.iForwarded = EFalse;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallRemoteParty;

	TMockLtsyData1<RMobileCall::TMobileCallInfoV1> callInfoData(callInfo);
	callInfoData.SerialiseL(data);

    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallInfoChangeIndId, KErrNone, data);

    data.Close();
    CleanupStack::PopAndDestroy(&data);
	} // CCtsyComponentTestBase::DriverCompleteGetMobileCallInfoL

/**
 * Queues notifications EStatusDialling -> EStatusConnecting -> EStatusConnected
 * for the specified call.
 */
void CCtsyComponentTestBase::DriverCompleteSuccessfulDialNotificationsL(TInt aCallId)
	{
	RArray<RMobileCall::TMobileCallStatus> arrayCallStatus;
	CleanupClosePushL(arrayCallStatus);

	arrayCallStatus.AppendL(RMobileCall::EStatusDialling);
	arrayCallStatus.AppendL(RMobileCall::EStatusConnecting);
	arrayCallStatus.AppendL(RMobileCall::EStatusConnected);

	DriverCompleteCallStatusChangeNotificationsL(arrayCallStatus, aCallId, KErrNone);

	arrayCallStatus.Close();

	CleanupStack::PopAndDestroy(1, &arrayCallStatus);
	} // CCtsyComponentTestBase::CompleteSuccessfulDialNotifications

/**
 * @param aExtraDelay The delay required to send to CompleteL. 0 means use default.
 */
void CCtsyComponentTestBase::DriverCompleteCallStatusChangeNotificationsL(
		RArray<RMobileCall::TMobileCallStatus>& aStatuses, TInt aCallId, TInt aNotifyError,
		TInt aExtraDelay)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	for (TInt i = 0; i < aStatuses.Count(); i++)
		{
		RMobileCall::TMobileCallStatus callStatus = aStatuses[i];

		// Complete KLtsyDispatchCallControlNotifyCallStatusChangeApiId
		TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(
				aCallId, RMobilePhone::EServiceUnspecified, callStatus);
	    data.Close();
	    mockCallData.SerialiseL(data);
	    if (aExtraDelay == 0)
	    	{
	    	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, aNotifyError, data);
	    	}
	    else
	    	{
	    	iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, aNotifyError, data, aExtraDelay);
	    	}
		}

	data.Close();
	CleanupStack::PopAndDestroy(1, &data);
	} // CCtsyComponentTestBase::DriverCompleteCallStatusChangeNotificationsL

void CCtsyComponentTestBase::DriverDialCallL(TInt aCallId, RMobilePhone::TMobileService aMode, TBool aIsIsvCall, TBool aIsSimOriginated, TBool aPerformFdnCheck)
	{
	DriverExpectDialCallRequestL(aCallId, aMode, KErrNone, aIsIsvCall, aIsSimOriginated, aPerformFdnCheck);
	DriverCompleteGetMobileCallInfoL(aCallId, aMode);
	DriverCompleteSuccessfulDialNotificationsL(aCallId);

	if (aMode == RMobilePhone::ECircuitDataService)
		{
		// In a data call,  complete dial explicitly
		RBuf8 data;
		CleanupClosePushL(data);

		TMockLtsyCallData0 dialData2(aCallId, aMode); // Mode not actually used
		data.Close();
		dialData2.SerialiseL(data);
	    iMockLTSY.CompleteL(MLtsyDispatchCallControlDialData::KLtsyDispatchCallControlDialDataApiId, KErrNone, data);

	    CleanupStack::PopAndDestroy(&data);
		}

	} // CCtsyComponentTestBase::DriverDialCallL

/**
 * Queues notifications EStatusDisconnecting -> EStatusIdle
 * for the specified call for disconnecting a call without inband or
 * EStatusDisconnectingWithInband -> EStatusIdle otherwise.
 *
 * @param aExtraDelay Delay to be passed to CompleteL. 0 means use default MockSY delay.
 * @param aCallHasDuration ETrue if the call has been connected. this implies that a phone life timer update message will be sent down when the call is closed.
 */
void CCtsyComponentTestBase::DriverCompleteSuccessfulHangUpNotificationsL(
	TInt aCallId, TBool aWithInBandInfo, TInt aNotifyError, TBool aCallHasDuration, TInt aExtraDelay)
	{
	RArray<RMobileCall::TMobileCallStatus> arrayCallStatus;
	CleanupClosePushL(arrayCallStatus);

	if (aWithInBandInfo)
		{
		arrayCallStatus.AppendL(RMobileCall::EStatusDisconnectingWithInband);
		}
	else
		{
		arrayCallStatus.AppendL(RMobileCall::EStatusDisconnecting);
		}
	arrayCallStatus.AppendL(RMobileCall::EStatusIdle);

	DriverCompleteCallStatusChangeNotificationsL(arrayCallStatus, aCallId, aNotifyError, aExtraDelay);

	arrayCallStatus.Close();
	
	//if (KErrGsmCCNormalCallClearing == aNotifyError) //the assumption is that the call has only had a duration if its been closed down normally
	if (aCallHasDuration)
		{
		RBuf8 data;
		TUint32 duration = 3; //this is the length of the call and its not possible to be certain of this due to
								//e.g. logging statements so this argument is not actually checked in the mockltsy
		TMockLtsyData1<TUint32> durationLtsyData(duration);
		durationLtsyData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId, data, KErrNone);
		data.Close();
		}
	
	CleanupStack::PopAndDestroy(1, &arrayCallStatus);
	} // CCtsyComponentTestBase::DriverCompleteSuccessfulHangUpNotificationsL

/**
 * @param aCallHasDuration set to ETrue if the call was connected for any amount of time, as the phone life timer will be updated causing an ipc to be sent down to ltsy.
 */

void CCtsyComponentTestBase::DriverHangUpCallL(TInt aCallId, TInt aHangUpCause, TBool aCallHasDuration)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TMockLtsyData2<TInt, TInt> mockData2(aCallId, aHangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);

	DriverCompleteSuccessfulHangUpNotificationsL(aCallId, EFalse, KErrGsmCCNormalCallClearing, aCallHasDuration);

	TMockLtsyData1<TInt> hangUpData(aCallId);
	data.Close();
	hangUpData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, KErrNone, data);

	data.Close();
	
	CleanupStack::PopAndDestroy(1, &data);
	} // CCtsyComponentTestBase::DriverHangUpCall


void CCtsyComponentTestBase::DriverHoldCallL(TInt aCallId, TInt aHoldError)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TMockLtsyData1<TInt> mockData(aCallId);
	data.Close();
	mockData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId, data);

	TMockLtsyData1<TInt> holdData(aCallId);
	data.Close();
	holdData.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId, aHoldError, data);

    if (aHoldError == KErrNone)
    	{
	    RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusHold;
	    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(
	    				aCallId, RMobilePhone::EServiceUnspecified, callStatus);
	    data.Close();
	    mockCallData.SerialiseL(data);
	    iMockLTSY.CompleteL(KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId, KErrNone, data);
    	}

    data.Close();
    CleanupStack::PopAndDestroy(1, &data);
	} // CCtsyComponentTestBase::DriverHoldCallL


void CCtsyComponentTestBase::DriverDialAndHoldFirstCallDialSecondCallL(RMobilePhone::TMobileService aMobileService, TInt aCallId1, TInt aCallId2)
	{
	DriverDialCallL(aCallId1, aMobileService);

	DriverHoldCallL(aCallId1, KErrNone);

	DriverDialCallL(aCallId2, aMobileService);
	} // CCtsyComponentTestBase::DriverDialAndHoldFirstCallDialSecondCallL


/**
 * Opens line and call objects, dials a call and waits until call notifications
 * have been received and the call is connected.
 *
 * @param aMobileLine Line subsession to open.
 * @param aMobileCall Call subsession to open.
 * @param aLineName Name of line to open.
 */
void CCtsyComponentTestBase::ClientDialCallL(
		RMobileLine& aMobileLine, RMobileCall& aMobileCall, RMobilePhone::TMobileService aMode,
		TBool aDialEmergencyCall, TBool aPerformFdnCheck)
	{
	OpenLineAndCallL(aMobileLine, aMobileCall, aMode);

	RMobileCall::TMobileCallStatus callStatus;
	TRequestStatus notifyCallStatus;
	aMobileCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);

	_LIT(KPhoneNumber, "135468456456");
	TRequestStatus dialStatus;

	if (aMode == RMobilePhone::EVoiceService || aMode == RMobilePhone::EAuxVoiceService)
		{
		if (aDialEmergencyCall)
			{
			RMobileENStore::TEmergencyNumber emergencyNumber = _L("911");
			aMobileCall.DialEmergencyCall(dialStatus, emergencyNumber);
			}
		else
			{
			RMobileCall::TMobileCallParamsV1 callParams;
			RMobileCall::TMobileCallParamsV1Pckg callParamsPckg(callParams);
			callParams.iAutoRedial = ETrue;
			callParams.iIdRestrict = RMobileCall::ESendMyId;
			if (aPerformFdnCheck)
				{
				aMobileCall.Dial(dialStatus, callParamsPckg, KPhoneNumber);
				}
			else
				{
				aMobileCall.DialNoFdnCheck(dialStatus, callParamsPckg, KPhoneNumber);
				}
			}
		}
	else
		{
		RMobileCall::TMobileDataCallParamsV1 callParams;
		RMobileCall::TMobileDataCallParamsV1Pckg callParamsPckg(callParams);
		callParams.iAckTimer = 3;
		callParams.iAutoRedial = ETrue;
	    callParams.iInterval = 4;
	    callParams.iCug.iCugIndex = 0xFFFF;
	    callParams.iService = RMobileCall::EServiceDataCircuitAsync;
	    if (aPerformFdnCheck)
	    	{
	    	aMobileCall.Dial(dialStatus, callParamsPckg, KPhoneNumber);
	    	}
	    else
	    	{
	    	aMobileCall.DialNoFdnCheck(dialStatus, callParamsPckg, KPhoneNumber);
	    	}
		}

	RArray<RMobileCall::TMobileCallStatus> expectedCallStatuses;
    CleanupClosePushL(expectedCallStatuses);
    expectedCallStatuses.AppendL(RMobileCall::EStatusDialling);
    expectedCallStatuses.AppendL(RMobileCall::EStatusConnecting);
    expectedCallStatuses.AppendL(RMobileCall::EStatusConnected);
    ClientWaitForCallStatusNotifications(aMobileCall, notifyCallStatus, callStatus, expectedCallStatuses, KErrNone);

	User::WaitForRequest(dialStatus);
	ASSERT_EQUALS(KErrNone, dialStatus.Int());

	aMobileCall.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);

	expectedCallStatuses.Close();
	CleanupStack::PopAndDestroy(1, &expectedCallStatuses);
	} // CCtsyComponentTestBase::ClientDialCallL

void CCtsyComponentTestBase::ClientWaitForCallStatusNotifications(
	RMobileCall& aMobileCall, TRequestStatus& aRequestStatus, RMobileCall::TMobileCallStatus& aCallStatus,
	RArray<RMobileCall::TMobileCallStatus>& aExpectedStatuses, TInt aExpectedError)
	{
	if (aExpectedStatuses.Count() == 0)
		{
		return;
		}

	User::WaitForRequest(aRequestStatus);
	ASSERT_EQUALS(aExpectedError, aRequestStatus.Int());
	ASSERT_EQUALS(aExpectedStatuses[0], aCallStatus);

	for (TInt i = 1; i < aExpectedStatuses.Count(); i++)
		{
		aMobileCall.NotifyMobileCallStatusChange(aRequestStatus, aCallStatus);
		User::WaitForRequest(aRequestStatus);
		ASSERT_EQUALS(aExpectedError, aRequestStatus.Int());
		ASSERT_EQUALS(aExpectedStatuses[i], aCallStatus);
		}

	} // CCtsyComponentTestBase::ClientWaitForCallStatusNotificationsL

void CCtsyComponentTestBase::ClientDialAndHoldFirstCallDialSecondCallL(
	RMobilePhone::TMobileService aMobileService,
	RMobileCall& aMobileCall1, RMobileLine& aMobileLine1,
	RMobileCall& aMobileCall2, RMobileLine& aMobileLine2)
	{
	ClientDialCallL(aMobileLine1, aMobileCall1, aMobileService);

	TRequestStatus notifyStatus;
	RMobileCall::TMobileCallStatus callStatus;
	aMobileCall1.NotifyMobileCallStatusChange(notifyStatus, callStatus);

	TRequestStatus holdStatus;
	aMobileCall1.Hold(holdStatus);
	User::WaitForRequest(holdStatus);
	ASSERT_EQUALS(KErrNone, holdStatus.Int());

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);

	ClientDialCallL(aMobileLine2, aMobileCall2, aMobileService);

	// First call should be on hold, second call should be connected
	ASSERT_EQUALS(KErrNone, aMobileCall1.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusHold, callStatus);
	ASSERT_EQUALS(KErrNone, aMobileCall2.GetMobileCallStatus(callStatus));
	ASSERT_EQUALS(RMobileCall::EStatusConnected, callStatus);
	} // CCtsyComponentTestBase::ClientDialAndHoldFirstCallDialSecondCallL


/**
 * Hangs up call and receives call notifications.
 * @param aMobileCall Call subsession of connected call.
 */
void CCtsyComponentTestBase::ClientHangUpCallL(RMobileCall& aMobileCall)
	{
	TRequestStatus notifyCallStatus;
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusUnknown;
	aMobileCall.NotifyMobileCallStatusChange(notifyCallStatus, callStatus);

	TRequestStatus hangupStatus;
	aMobileCall.HangUp(hangupStatus);

	RArray<RMobileCall::TMobileCallStatus> expectedCallStatuses;
    CleanupClosePushL(expectedCallStatuses);
    expectedCallStatuses.AppendL(RMobileCall::EStatusDisconnecting);
    expectedCallStatuses.AppendL(RMobileCall::EStatusIdle);
    ClientWaitForCallStatusNotifications(aMobileCall, notifyCallStatus, callStatus, expectedCallStatuses, KErrNone);

	User::WaitForRequest(hangupStatus);
	ASSERT_EQUALS(KErrNone, hangupStatus.Int());

	aMobileCall.GetMobileCallStatus(callStatus);
	ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);

	expectedCallStatuses.Close();
	CleanupStack::PopAndDestroy(1, &expectedCallStatuses);
	} // CCtsyComponentTestBase::ClientHangUpCallL
