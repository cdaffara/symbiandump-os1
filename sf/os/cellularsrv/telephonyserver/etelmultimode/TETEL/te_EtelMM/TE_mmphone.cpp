// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmphone.h"
#include "testdef.h"
#include "TE_mmcall.h"
#include <e32debug.h>
#include <s32file.h>
#include <f32file.h>
#include <etelmmerr.h>


CTestPhone::CTestPhone()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestPhone"));
	}

enum TVerdict CTestPhone::doTestStepL()
    {
    
	iTestCount=0;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile Phone Functionality"));

	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	// local variables used throughout the phone tests
	TRequestStatus reqStatus;

	TestCapabilityL(mmPhone,reqStatus);
	TestSimAccessL(mmPhone,reqStatus);
	TestMobilePowerL(mmPhone,reqStatus);
	TestMobileSignalL(mmPhone,reqStatus);
	TestMobileIndicatorL(mmPhone,reqStatus);
	TestMobileIdentityL(mmPhone,reqStatus);
	TestMobileDTMFL(mmPhone,reqStatus);
	TestMobileNetworkL(mmPhone,reqStatus);
	TestMobileCellInfoL(mmPhone,reqStatus);
	TestHSUPAMobileNetworkL(mmPhone,reqStatus);
	TestTMobilePhoneNetworkInfoV8(reqStatus);
	//TestMobileNetworkISVL(mmPhone,reqStatus);
	TestMobilePrivacyL(mmPhone,reqStatus);
	TestMobileCallServiceCapsL(mmPhone,reqStatus);
	TestMobileUserNetworkL(mmPhone,reqStatus);
	TestMobileCallForwardingL(mmPhone,reqStatus);
	TestMobileIdentityServiceL(mmPhone,reqStatus);
	TestMobileCallBarringL(mmPhone,reqStatus);
	TestMobileCallWaitingL(mmPhone,reqStatus);
	TestMobileCallCompletionL(mmPhone,reqStatus);
	TestMobileAlternatingCallL(mmPhone,reqStatus);
	TestMobileAlternateLineServiceL(mmPhone,reqStatus);
	TestMobileCallCostL(mmPhone,reqStatus);
	TestMobileSecurityL(mmPhone,reqStatus);
	TestMobileMessageWaitingL(mmPhone,reqStatus);
	TestMobileFixedDiallingNumbersL(mmPhone,reqStatus);
	TestMobileMulticallSettings(mmPhone,reqStatus);
	TestMobileIncomingCallTypes(mmPhone,reqStatus);
	TestMobileUUSSetting(mmPhone,reqStatus);
	TestMultimediaCallSettings(mmPhone,reqStatus);
	TestNetworkSecurity(mmPhone,reqStatus);
	TestUSimHandling(mmPhone,reqStatus);
	TestMobileMailboxNumbersFunctionality(mmPhone, reqStatus);
	TestMobileMmsFunctionality(mmPhone, reqStatus);
	TestMobileAPNControlList(mmPhone, reqStatus);
	TestMobileAPNControlListServiceStatus(mmPhone, reqStatus);
	TestMobileAirTime(mmPhone, reqStatus);
	TestTerminateAllCalls(mmPhone, reqStatus);
	TestTerminateActiveCalls(mmPhone, reqStatus);
	TestMobileAutoRedial(mmPhone, reqStatus);
	TestMobilePersonalisation(mmPhone, reqStatus);
	TestSmartCardHandling(mmPhone, reqStatus);
	TestGBAAuthenticationL(mmPhone, reqStatus);
	TestMBMSAuthenticationL(mmPhone, reqStatus);
	TestWlanParametersL(mmPhone, reqStatus);
	
	mmPhone.Close(); 
	INFO_PRINTF1(_L(""));
	return TestStepResult();
}


void CTestPhone::TestCapabilityL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	TUint32 caps;
	TInt    version;

	iTestCount=1;

	// Get Multimode API Version 
	TEST(aPhone.GetMultimodeAPIVersion(version)==KErrNone);
	TEST(version==KEtelExtMultimodeV6);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetMultimodeAPIVersion (sync) passed"), iTestCount++);

	// Get Multimode capabilities
	TEST(aPhone.GetMultimodeCaps(caps)==KErrNone);
	TEST(caps==DMMTSY_PHONE_MULTIMODE_CAPS);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetMultimodeCaps (sync) passed"), iTestCount++);

	// Get Phone Store Info 
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 asyncStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg asyncStoreInfoPckg(asyncStoreInfo);

	TBuf<32> storeName1(DMMTSY_PHONE_STORE_NAME1);
	TBuf<32> iccType1(DMMTSY_PHONEBOOK_TYPE);
	aPhone.GetPhoneStoreInfo(aReqStatus,asyncStoreInfoPckg,storeName1,iccType1);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	ValidatePhoneStoreInfo(asyncStoreInfo,0);

	TEST(asyncStoreInfo.ExtensionId()==RMobilePhoneStore::KETelMobilePhoneStoreV1);

	TBuf<32> storeName2(DMMTSY_PHONE_STORE_NAME2);
	aPhone.GetPhoneStoreInfo(aReqStatus,asyncStoreInfoPckg,storeName2);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	ValidatePhoneStoreInfo(asyncStoreInfo,1);

	aPhone.GetPhoneStoreInfo(aReqStatus, asyncStoreInfoPckg, storeName2);
	aPhone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetPhoneStoreInfo (async & cancel (Cancelled Requests)) passed"), iTestCount++);
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetPhoneStoreInfo (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		ValidatePhoneStoreInfo(asyncStoreInfo,1);
		}

//	test.Next(_L("OK: RMobilePhone's MobilePhoneCapability"));
	}

void CTestPhone::TestSimAccessL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{	
	// Get SIM access capabilities
	
	TUint32 caps;

	TEST(aPhone.GetIccAccessCaps(caps)==KErrNone);
	TEST(caps==DMMTSY_PHONE_SIM_ACCESS_CAPS1);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIccAccessCaps (sync) passed"), iTestCount++);

	// Notify change of SIM access capabilities
	
	// asynchronous

	aPhone.NotifyIccAccessCapsChange(aReqStatus, caps);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(caps==DMMTSY_PHONE_SIM_ACCESS_CAPS2);

	// asynchronous & cancel

	aPhone.NotifyIccAccessCapsChange(aReqStatus, caps);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyIccAccessCapsChange);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);

	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIccAccessCapsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(caps==DMMTSY_PHONE_SIM_ACCESS_CAPS2);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIccAccessCapsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
		
	// Get ICC identity
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIccIdentity."), iTestCount++);
	RMobilePhone::TIccIdentity id;
	aPhone.GetIccIdentity(aReqStatus, id);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(id == DMMTSY_PHONE_ICC_ID);
	
	// Get ICC identity & Cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIccIdentity & Cancel."), iTestCount++);
	aPhone.GetIccIdentity(aReqStatus, id);
	aPhone.CancelAsyncRequest(EMobilePhoneGetIccIdentity);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrCancel);
	
	//

	// Get Customer Service Profile - both synchronously & asynchronously
	
	// asynchronous

	RMobilePhone::TMobilePhoneCspFileV1 asyncCspFile;
	RMobilePhone::TMobilePhoneCspFileV1Pckg asyncCspFilePckg(asyncCspFile);

	aPhone.GetCustomerServiceProfile(aReqStatus, asyncCspFilePckg);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	// extract the returned CSP file from the package
	asyncCspFile=asyncCspFilePckg();

	ValidateCustomerServiceProfile(asyncCspFile);
	TEST(asyncCspFile.ExtensionId()==KETelExtMultimodeV1);

	aPhone.GetCustomerServiceProfile(aReqStatus, asyncCspFilePckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCustomerServiceProfile);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);

	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCustomerServiceProfile (async & cancel (Cancelled Request) passed"), iTestCount++);
	else
		{
		asyncCspFile=asyncCspFilePckg();
		ValidateCustomerServiceProfile(asyncCspFile);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCustomerServiceProfile (async & cancel (Request Not Cancelled) passed"), iTestCount++);
		}

	// Get SIM Service Table 
	
	// asynchronous

	RMobilePhone::TMobilePhoneServiceTableV1 asyncSstFile;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg asyncSstFilePckg(asyncSstFile);
	RMobilePhone::TMobilePhoneServiceTable serviceTableType=DMMTSY_PHONE_SERVICE_TABLE;

	aPhone.GetServiceTable(aReqStatus, serviceTableType, asyncSstFilePckg);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	// extract the returned SST file from the package
	asyncSstFile=asyncSstFilePckg();
	ValidateServiceTable(asyncSstFile);
	TEST(asyncSstFile.ExtensionId()==KETelExtMultimodeV1);

	// asynchronous & cancel

	aPhone.GetServiceTable(aReqStatus, serviceTableType, asyncSstFilePckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetServiceTable);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);

	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetServiceTable (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		// extract the returned SST file from the package
		asyncSstFile=asyncSstFilePckg();
		ValidateServiceTable(asyncSstFile);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetServiceTable (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// asynchronous

	RMobilePhone::TMobilePhoneServiceTableV2 asyncSstV2File;
	RMobilePhone::TMobilePhoneServiceTableV2Pckg asyncSstV2FilePckg(asyncSstV2File);

	aPhone.GetServiceTable(aReqStatus, serviceTableType, asyncSstV2FilePckg);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	// extract the returned SST file from the package
	asyncSstV2File=asyncSstV2FilePckg();
	ValidateServiceTable(asyncSstV2File);
	TEST(asyncSstV2File.ExtensionId()==KETelExtMultimodeV2);

	// asynchronous & cancel

	aPhone.GetServiceTable(aReqStatus, serviceTableType, asyncSstV2FilePckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetServiceTable);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);

	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetServiceTable V2table (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		// extract the returned SST file from the package
		asyncSstFile=asyncSstFilePckg();
		ValidateServiceTable(asyncSstV2File);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetServiceTable V2table (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobilePhone's MobilePhoneSimAccess"));
	}

void CTestPhone::TestMobilePowerL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Get Battery Caps

	TUint32 caps;

	TEST(aPhone.GetBatteryCaps(caps)==KErrNone);
	TEST(caps==DMMTSY_PHONE_BATTERY_CAPS);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetBatteryCaps (sync) passed"), iTestCount++);

	// asynchronous

	RMobilePhone::TMobilePhoneBatteryInfoV1 asyncBatteryInfo;

	aPhone.GetBatteryInfo(aReqStatus, asyncBatteryInfo);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(asyncBatteryInfo.iStatus==DMMTSY_PHONE_BATTERY_STATUS1);
	TEST(asyncBatteryInfo.iChargeLevel==DMMTSY_PHONE_BATTERY_CHARGE1);

	// asynchronous & cancel

	aPhone.GetBatteryInfo(aReqStatus, asyncBatteryInfo);
	aPhone.CancelAsyncRequest(EMobilePhoneGetBatteryInfo);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetBatteryInfo (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncBatteryInfo.iStatus==DMMTSY_PHONE_BATTERY_STATUS1);
		TEST(asyncBatteryInfo.iChargeLevel==DMMTSY_PHONE_BATTERY_CHARGE1);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetBatteryInfo (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	
	// Notify change of battery info
	
	// asynchronous
		
	aPhone.NotifyBatteryInfoChange(aReqStatus, asyncBatteryInfo);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(asyncBatteryInfo.iStatus==DMMTSY_PHONE_BATTERY_STATUS2);
	TEST(asyncBatteryInfo.iChargeLevel==DMMTSY_PHONE_BATTERY_CHARGE2);

	// asynchronous & cancel

	aPhone.NotifyBatteryInfoChange(aReqStatus, asyncBatteryInfo);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyBatteryInfoChange);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyBatteryInfoChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncBatteryInfo.iStatus==DMMTSY_PHONE_BATTERY_STATUS2);
		TEST(asyncBatteryInfo.iChargeLevel==DMMTSY_PHONE_BATTERY_CHARGE2);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyBatteryInfoChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	
//	test.Next(_L("OK: RMobilePhone's MobilePhonePower"));	
	}

void CTestPhone::TestMobileSignalL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Get Signal Caps

	TUint32 caps;

	TEST(aPhone.GetSignalCaps(caps)==KErrNone);
	TEST(caps==DMMTSY_PHONE_SIGNAL_CAPS);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetSignalCaps (sync) passed"), iTestCount++);

	// Get Signal Strength Info 
	
	// asynchronous

	TInt32 asyncSigStrength;
	TInt8 asyncSigBar;

	aPhone.GetSignalStrength(aReqStatus, asyncSigStrength, asyncSigBar);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(asyncSigStrength==DMMTSY_PHONE_SIGNAL_STRENGTH1);
	TEST(asyncSigBar==DMMTSY_PHONE_SIGNAL_BAR1);

	// asynchronous & cancel

	aPhone.GetSignalStrength(aReqStatus, asyncSigStrength, asyncSigBar);
	aPhone.CancelAsyncRequest(EMobilePhoneGetSignalStrength);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetSignalStrength (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		TEST(asyncSigStrength==DMMTSY_PHONE_SIGNAL_STRENGTH1);
		TEST(asyncSigBar==DMMTSY_PHONE_SIGNAL_BAR1);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetSignalStrength (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Notify change of signal strength

	// asynchronous

	aPhone.NotifySignalStrengthChange(aReqStatus, asyncSigStrength, asyncSigBar);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(asyncSigStrength==DMMTSY_PHONE_SIGNAL_STRENGTH2);
	TEST(asyncSigBar==DMMTSY_PHONE_SIGNAL_BAR2);

	// asynchronous & cancel

	aPhone.NotifySignalStrengthChange(aReqStatus, asyncSigStrength, asyncSigBar);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifySignalStrengthChange);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySignalStrengthChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncSigStrength==DMMTSY_PHONE_SIGNAL_STRENGTH2);
		TEST(asyncSigBar==DMMTSY_PHONE_SIGNAL_BAR2);	
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySignalStrengthChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobilePhone's MobilePhoneSignal"));	
	}

void CTestPhone::TestMobileIndicatorL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Get Signal Caps

	TUint32 actionCaps;
	TUint32 indCaps;

	TEST(aPhone.GetIndicatorCaps(actionCaps, indCaps)==KErrNone);
	TEST(actionCaps==DMMTSY_PHONE_ACTION_INDICATOR_CAPS);
	TEST(indCaps==DMMTSY_PHONE_INDICATOR_CAPS);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIndicatorCaps (sync) passed"), iTestCount++);

	// Get Signal Strength Info 
	
	// asynchronous

	TUint32 asyncIndicator;

	aPhone.GetIndicator(aReqStatus, asyncIndicator);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(asyncIndicator==DMMTSY_PHONE_INDICATOR1);

	// asynchronous & cancel

	aPhone.GetIndicator(aReqStatus, asyncIndicator);
	aPhone.CancelAsyncRequest(EMobilePhoneGetIndicator);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIndicator (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		TEST(asyncIndicator==DMMTSY_PHONE_INDICATOR1);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIndicator (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Notify change of signal strength

	// asynchronous

	aPhone.NotifyIndicatorChange(aReqStatus, asyncIndicator);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(asyncIndicator==DMMTSY_PHONE_INDICATOR2);

	// asynchronous & cancel

	aPhone.NotifyIndicatorChange(aReqStatus, asyncIndicator);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyIndicatorChange);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIndicatorChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncIndicator==DMMTSY_PHONE_INDICATOR2);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIndicatorChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobilePhone's MobilePhoneIndicator"));	
	}

void CTestPhone::TestMobileIdentityL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Get Identity Caps

	TUint32 caps;
	
	TEST(aPhone.GetIdentityCaps(caps)==KErrNone);
	TEST(caps==DMMTSY_PHONE_IDENTITY_CAPS);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIdentityCaps (sync) passed"), iTestCount++);

	// Phone ID
	RMobilePhone::TMobilePhoneIdentityV1 asyncPhoneId;

	// asynchronous

	aPhone.GetPhoneId(aReqStatus,asyncPhoneId);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	ValidatePhoneId(asyncPhoneId);
	
	// asynchronous & cancel

	aPhone.GetPhoneId(aReqStatus,asyncPhoneId);
	aPhone.CancelAsyncRequest(EMobilePhoneGetPhoneId);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetPhoneId (sync & async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidatePhoneId(asyncPhoneId);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetPhoneId (sync & async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}


	// Subscriber ID - IMSI retrieval
	RMobilePhone::TMobilePhoneSubscriberId asyncSubId;

	// asynchronous

	aPhone.GetSubscriberId(aReqStatus,asyncSubId);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(asyncSubId.Compare(DMMTSY_PHONE_SUBSCRIBER_ID)==0);

	// asynchronous & cancel

	aPhone.GetSubscriberId(aReqStatus,asyncSubId);
	aPhone.CancelAsyncRequest(EMobilePhoneGetSubscriberId);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetSubscriberId (sync & async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncSubId.Compare(DMMTSY_PHONE_SUBSCRIBER_ID)==0);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetSubscriberId (sync & async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobilePhone's MobilePhoneIdentity"));
	}

void CTestPhone::TestMobileDTMFL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Get DTMF caps
	
	TUint32 caps;

	TEST(aPhone.GetDTMFCaps(caps)==KErrNone);
	TEST(caps==DMMTSY_PHONE_DTMF_CAPS1);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetDTMFCaps (sync) passed"), iTestCount++);

	// notify DTMFCapsChange asynchronous

	aPhone.NotifyDTMFCapsChange(aReqStatus, caps);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(caps==DMMTSY_PHONE_DTMF_CAPS2);

	// asynchronous & cancel

	aPhone.NotifyDTMFCapsChange(aReqStatus, caps);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyDTMFCapsChange);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyDTMFCapsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		TEST(caps==DMMTSY_PHONE_DTMF_CAPS2);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyDTMFCapsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);	
		}

	// Test SendDTMFTones

	// Send buffer of DTMF tones
	TBuf<DMMTSY_PHONE_SEND_TONES_LENGTH> tones(DMMTSY_PHONE_SEND_TONES);

	// asynchronous

	aPhone.SendDTMFTones(aReqStatus, tones);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	// asynchronous & cancel

	aPhone.SendDTMFTones(aReqStatus, tones);
	aPhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SendDTMFTones (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SendDTMFTones (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test StartDTMFTone

	// Send one DTMF tone
	TChar tone(DMMTSY_PHONE_SINGLE_TONE);
	
	// synchronous

	TEST(aPhone.StartDTMFTone(tone)==KErrNone);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::StartDTMFTone (sync) passed"), iTestCount++);

	TEST(aPhone.StopDTMFTone()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::StopDTMFTone (sync) passed"), iTestCount++);

	// Test ReadDTMFTones

	// Read buffer for DTMF tones
	TBuf<DMMTSY_PHONE_READ_TONES_LENGTH> readtones;

	// asynchronous

	aPhone.ReadDTMFTones(aReqStatus, readtones);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(readtones.Compare(DMMTSY_PHONE_READ_TONES)==0);

	INFO_PRINTF2(_L("Test %d - RMobilePhone::ReadDTMFTones (async) passed"), iTestCount++);

	// asynchronous & cancel

	aPhone.ReadDTMFTones(aReqStatus, readtones);
	aPhone.CancelAsyncRequest(EMobilePhoneReadDTMFTones);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::ReadDTMFTones (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::ReadDTMFTones (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test NotifyStopInDTMF String

	// asynchronous

	aPhone.NotifyStopInDTMFString(aReqStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.NotifyStopInDTMFString(aReqStatus);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyStopInDTMFString);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyStopInDTMFString  (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyStopInDTMFString  (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test ContinueDTMFStringSending

	TEST(aPhone.ContinueDTMFStringSending(DMMTSY_PHONE_CONTINUE_DTMF_SENDING) == KErrNone);

	INFO_PRINTF2(_L("Test %d - RMobilePhone::ContinueDTMFStringSending (sync) passed"), iTestCount++);
	
	//Test NotifyDTMFEvent
	
	// asynchronous
	
	RMobilePhone:: TMobilePhoneDTMFEvent dtmfEvent;
	aPhone.NotifyDTMFEvent(aReqStatus, dtmfEvent);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(dtmfEvent == DMMTSY_PHONE_NOTIFY_DTMFEVENT_ON);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyDTMFEvent (async) passed"), iTestCount++);
	
	// asynchronous & cancel
	
	aPhone.NotifyDTMFEvent(aReqStatus, dtmfEvent);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyDTMFEvent);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyDTMFEvent  (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyDTMFEvent  (async & cancel (Request Not Cancelled)) passed"), iTestCount++);	
	
//	test.Next(_L("OK: RMobilePhone's MobilePhoneDTMF"));
	}

void CTestPhone::TestHSUPAMobileNetworkL(RMobilePhone &aPhone, TRequestStatus &aReqStatus)
{

	/* Get Current Network - Start*/
	RMobilePhone::TMobilePhoneNetworkInfoV8 asyncCurrentNetwork8;
	RMobilePhone::TMobilePhoneNetworkInfoV8Pckg asyncCurrentNetworkPckg8(asyncCurrentNetwork8);
	RMobilePhone::TMobilePhoneLocationAreaV1 asyncLocArea;

	aPhone.GetCurrentNetwork(aReqStatus, asyncCurrentNetworkPckg8, asyncLocArea);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	// extract the returned Current network info from the package
	asyncCurrentNetwork8=asyncCurrentNetworkPckg8();
	ValidateCurrentNetworkforHSUPA(asyncCurrentNetwork8, asyncLocArea);

	/* Get Current Network - End*/ 
	
	/* Get Current Network - Cancelling of the request - Start */
	
	aPhone.GetCurrentNetwork(aReqStatus, asyncCurrentNetworkPckg8, asyncLocArea);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetwork);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
		
	if (aReqStatus.Int()==KErrNone)
		{
		INFO_PRINTF1(_L("The Return Status for User::WaitForRequest - KErrNone  "));
		INFO_PRINTF1(_L("GetCurrentNetwork is already completed"));
		INFO_PRINTF1(_L("Cancelling of the GetCurrentNetwork Request is NOT Successfully Completed"));
		}
	else
		{
		INFO_PRINTF1(_L("The Return Status for User::WaitForRequest - KErrCancel "));
		INFO_PRINTF1(_L("Cancelling of the GetCurrentNetwork Request is Successfully Completed"));
		}

	/* Get Current Network - Cancelling of the request - End */	
	
	/* Get Home Network - Start */
	RMobilePhone::TMobilePhoneNetworkInfoV8 asyncHomeNetwork8;
	RMobilePhone::TMobilePhoneNetworkInfoV8Pckg asyncHomeNetworkPckg8(asyncHomeNetwork8);

	aPhone.GetHomeNetwork(aReqStatus, asyncHomeNetworkPckg8);	
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	
	asyncHomeNetwork8=asyncHomeNetworkPckg8();
	ValidateHomeNetworkforHSUPA(asyncHomeNetwork8);
	/* Get Home Network - End*/ 
	
	/* Get Home Network- Cancelling of the request - Start */

	aPhone.GetHomeNetwork(aReqStatus, asyncHomeNetworkPckg8);	
   	aPhone.CancelAsyncRequest(EMobilePhoneGetHomeNetwork);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
		
	if (aReqStatus.Int()==KErrNone)
		{
		INFO_PRINTF1(_L("The Return Status for User::WaitForRequest - KErrNone  "));
		INFO_PRINTF1(_L("GetHomeNetwork is already completed"));
		INFO_PRINTF1(_L("Cancelling of the GetHomeNetwork Request is NOT Successfully Completed"));
		}
	else
		{
		INFO_PRINTF1(_L("The Return Status for User::WaitForRequest - KErrCancel "));
		INFO_PRINTF1(_L("Cancelling of the GetHomeNetwork Request is Successfully Completed"));
		}
	
	/* Get Home Network- Cancelling of the request - End */
	

	/* Get Detected Networks - Start */

	CTestGetDetectedNetworks* getNetworks8=CTestGetDetectedNetworks::NewLC(aPhone, this);

	//Set test version to ETestV1 to test the V1 list class and list retrieval method
	getNetworks8->SetTestVersion(CTestGetDetectedNetworks::ETestV8);
	getNetworks8->TestRetrieve();

	//Delete test object and start afresh
	CleanupStack::PopAndDestroy(); //getNetworks
	
//	getNetworks8=CTestGetDetectedNetworks::NewLC(aPhone,this);

	//Set test version to ETestV8 and test starting and then cancelling retrieve method
//	getNetworks8->SetTestVersion(CTestGetDetectedNetworks::ETestV8);
//	getNetworks8->TestCancel();

//	CleanupStack::PopAndDestroy(); //getNetworks

	/* Get Detected Networks - End */

	/* Notify Current Change - Start */
	RMobilePhone mmPhonev8;
	TInt ret=mmPhonev8.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);
	
	//set up first notification request using V2 package format
	mmPhonev8.NotifyCurrentNetworkChange(aReqStatus, asyncCurrentNetworkPckg8, asyncLocArea);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	asyncCurrentNetwork8=asyncCurrentNetworkPckg8();
	ValidateNotifyCurrentNetworkChangeforHSUPA(asyncCurrentNetwork8,asyncLocArea);

	/* Notify Current Network Change - End */

	/* Notify Current Network Change and Cancel - Start */

	mmPhonev8.NotifyCurrentNetworkChange(aReqStatus, asyncCurrentNetworkPckg8, asyncLocArea);
	mmPhonev8.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
		
	if (aReqStatus.Int()==KErrNone)
		{
		INFO_PRINTF1(_L("The Return Status for User::WaitForRequest - KErrNone  "));
		INFO_PRINTF1(_L("NotifyCurrentNetworkChange is already completed"));
		INFO_PRINTF1(_L("Cancelling of the NotifyCurrentNetworkChange Request is NOT Successfully Completed"));
		}
	else
		{
		INFO_PRINTF1(_L("The Return Status for User::WaitForRequest - KErrCancel "));
		INFO_PRINTF1(_L("Canceling of the Notify Request is Scceussfully Completed"));
		}

	mmPhonev8.Close();
	/* Notify Current Network Change and Cancel - End */

}

void CTestPhone::TestTMobilePhoneNetworkInfoV8(TRequestStatus &aReqStatus)
{
	RMobilePhone::TMobilePhoneNetworkInfoV8 mobilePhoneNetworkInfoV8;
	if((mobilePhoneNetworkInfoV8.iHsupaAvailableIndicator==EFalse) && (mobilePhoneNetworkInfoV8.ExtensionId()==KEtelExtMultimodeV8))
		{
		INFO_PRINTF1(_L("TMobilePhoneNetworkInfoV8 Instance creation successful  "));
		INFO_PRINTF1(_L("Value of iHsupaAvailableIndicator = EFalse"));
		INFO_PRINTF1(_L("Value of iExtensionID = KEtelExtMultimodeV8"));
		aReqStatus= KErrNone;
		}
	else
		{
	   	INFO_PRINTF1(_L("TMobilePhoneNetworkInfoV8 Instance creation NOT successful" ));
		aReqStatus = KErrNotFound;
		}

}

void CTestPhone::TestCRetrievePhoneDetectedNetworksL(RMobilePhone &aPhone,TRequestStatus &aReqStatus)
	{
	
	CRetrieveMobilePhoneDetectedNetworks *pRetrieve = CRetrieveMobilePhoneDetectedNetworks::NewL(aPhone);
	
	CleanupStack::PushL(pRetrieve);
		
	CMobilePhoneNetworkListV8* networkList=NULL;
	networkList=pRetrieve->RetrieveListV8L();
	
	TEST(networkList==NULL);
		
	if(networkList == NULL)
		INFO_PRINTF1(_L("The instance value is NULL"));
	else
		INFO_PRINTF1(_L("The instance value is NOT NULL"));
	
	CleanupStack::PopAndDestroy(); 
	aReqStatus = KErrNone;
			
	}


void CTestPhone::TestMobileNetworkL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{

	TUint32 caps;
 
	 // Get network caps
     
	 TEST(aPhone.GetNetworkCaps(caps)==KErrNone);
	 TEST(caps==DMMTSY_PHONE_NETWORK_CAPS);
	 INFO_PRINTF2(_L("Test %d - RMobilePhone::GetNetworkCaps (sync) passed"), iTestCount++);
 
	 // Get Current Mobile mode
 
	 RMobilePhone::TMobilePhoneNetworkMode syncMode, asyncMode;

	// synchronous

	TEST(aPhone.GetCurrentMode(syncMode) == KErrNone);
 	TEST(syncMode==DMMTSY_PHONE_NETWORK_MODE1);

	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCurrentMode (sync) passed"), iTestCount++);

	// NotifyModeChange

	// asynchronous

	aPhone.NotifyModeChange(aReqStatus, asyncMode);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(asyncMode==DMMTSY_PHONE_NETWORK_MODE2);

	// asynchronous & cancel

	aPhone.NotifyModeChange(aReqStatus, asyncMode);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyModeChange);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyModeChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
	{
		TEST(asyncMode==DMMTSY_PHONE_NETWORK_MODE2);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyModeChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	}
	
	// Get Home Network Info

	// asynchronous

	RMobilePhone::TMobilePhoneNetworkInfoV1 asyncHomeNetwork;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg asyncHomeNetworkPckg(asyncHomeNetwork);

	aPhone.GetHomeNetwork(aReqStatus, asyncHomeNetworkPckg);	
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	ValidateHomeNetwork(asyncHomeNetwork);

	aPhone.GetHomeNetwork(aReqStatus, asyncHomeNetworkPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetHomeNetwork);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetHomeNetwork (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
	{
		asyncHomeNetwork=asyncHomeNetworkPckg();
		ValidateHomeNetwork(asyncHomeNetwork);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetHomeNetwork (async & cancel (Request Not Cancelled)) passed"), iTestCount++);	
	}
	
	// GetHomeNetworkSearchPeriod
	
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetHomeNetworkSearchPeriod async."), iTestCount++);
	TInt searchIntv;
	aPhone.GetHomeNetworkSearchPeriod(aReqStatus, searchIntv);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(searchIntv == DMMTSY_PHONE_HOME_NETWORK_SEARCHINTV);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetHomeNetworkSearchPeriod async & cancel."), iTestCount++);
	aPhone.GetHomeNetworkSearchPeriod(aReqStatus, searchIntv);
	aPhone.CancelAsyncRequest(EMobilePhoneGetHomeNetworkSearchPeriod);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrCancel);
	
	// GetLastUsedAccessTechnology
	
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetLastUsedAccessTechnology async."), iTestCount++);
	RMobilePhone::TMobilePhoneNetworkAccess accessTech;
	aPhone.GetLastUsedAccessTechnology(aReqStatus, accessTech);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(accessTech == DMMTSY_PHONE_LAST_USED_ACCESS_TECH);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetLastUsedAccessTechnology async & cancel."), iTestCount++);
	aPhone.GetLastUsedAccessTechnology(aReqStatus, accessTech);
	aPhone.CancelAsyncRequest(EMobilePhoneGetLastUsedAccessTechnology);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrCancel);
	
	// Get Current Network Info

	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCurrentNetwork async."), iTestCount++);
	RMobilePhone::TMobilePhoneNetworkInfoV1 asyncCurrentNetwork;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg asyncCurrentNetworkPckg(asyncCurrentNetwork);
	RMobilePhone::TMobilePhoneLocationAreaV1 asyncLocArea;

	aPhone.GetCurrentNetwork(aReqStatus, asyncCurrentNetworkPckg, asyncLocArea);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	// extract the returned Current network info from the package
	asyncCurrentNetwork=asyncCurrentNetworkPckg();
	ValidateCurrentNetwork(asyncCurrentNetwork, asyncLocArea);

	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCurrentNetwork async & cancel."), iTestCount++);
	aPhone.GetCurrentNetwork(aReqStatus, asyncCurrentNetworkPckg, asyncLocArea);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetwork);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrCancel || aReqStatus.Int()==KErrNone);
	if (aReqStatus.Int()==KErrNone)
	{	
		asyncCurrentNetwork=asyncCurrentNetworkPckg();
		ValidateCurrentNetwork(asyncCurrentNetwork, asyncLocArea);
	}

	// Notify Current Network Change
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCurrentNetworkChange async."), iTestCount++);
	aPhone.NotifyCurrentNetworkChange(aReqStatus, asyncCurrentNetworkPckg, asyncLocArea);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	asyncCurrentNetwork=asyncCurrentNetworkPckg();
	ValidateNotifyCurrentNetworkChange(asyncCurrentNetwork,asyncLocArea);

	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCurrentNetworkChange async & cancel."), iTestCount++);
	aPhone.NotifyCurrentNetworkChange(aReqStatus, asyncCurrentNetworkPckg, asyncLocArea);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
   	User::WaitForRequest(aReqStatus);
   	
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrNone)
	{
		asyncCurrentNetwork=asyncCurrentNetworkPckg();
		ValidateNotifyCurrentNetworkChange(asyncCurrentNetwork,asyncLocArea);
	}
		
		
	TestMobileNetworkISVL(aPhone, aReqStatus);
		
	//Multiple complete Notify Current Network Change
	//use a second phone client object
	RMobilePhone mmPhone2;
	TInt ret=mmPhone2.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	//set up a second parameter package
	TRequestStatus aReqStatus2;
	RMobilePhone::TMobilePhoneNetworkInfoV2 asyncCurrentNetwork2;
	RMobilePhone::TMobilePhoneNetworkInfoV2Pckg asyncCurrentNetworkPckg2(asyncCurrentNetwork2);
	RMobilePhone::TMobilePhoneLocationAreaV1 asyncLocArea2;

	//set up first notification request using V2 package format
	mmPhone2.NotifyCurrentNetworkChange(aReqStatus, asyncCurrentNetworkPckg2, asyncLocArea2);
	User::WaitForRequest(aReqStatus2);
	TEST(aReqStatus.Int()==KErrNone);

	//set up second notification request using V1 package format
	aPhone.NotifyCurrentNetworkChange(aReqStatus, asyncCurrentNetworkPckg, asyncLocArea);	
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	//validate the version 2 package
	ValidateNotifyCurrentNetworkChange(asyncCurrentNetwork2,asyncLocArea2);
	TEST(asyncCurrentNetwork2.iAccess == DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_GSM);
	mmPhone2.Close();
	
	asyncCurrentNetwork=asyncCurrentNetworkPckg();
	ValidateNotifyCurrentNetworkChange(asyncCurrentNetwork,asyncLocArea);

	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCurrentNetworkChange async & cancel."), iTestCount++);
	aPhone.NotifyCurrentNetworkChange(aReqStatus, asyncCurrentNetworkPckg, asyncLocArea);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
   	User::WaitForRequest(aReqStatus);
   	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel); 
	if (aReqStatus.Int()==KErrNone)
		{
		INFO_PRINTF1(_L("The Return Status for User::WaitForRequest - KErrNone  "));
		INFO_PRINTF1(_L("NotifyCurrentNetworkChange is already completed"));
		INFO_PRINTF1(_L("Cancelling of the NotifyCurrentNetworkChange Request is NOT Successfully Completed"));
		}
	else
		{
		INFO_PRINTF1(_L("The Return Status for User::WaitForRequest - KErrCancel "));
		INFO_PRINTF1(_L("Cancelling of the NotifyCurrentNetworkChange Request is Successfully Completed"));
		}

	// Get network registration status

	RMobilePhone::TMobilePhoneRegistrationStatus asyncRegStatus;

	// asynchronous

	aPhone.GetNetworkRegistrationStatus(aReqStatus, asyncRegStatus);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(asyncRegStatus==DMMTSY_PHONE_REGISTRATION_STATUS1);

	// asynchronous & cancel

	aPhone.GetNetworkRegistrationStatus(aReqStatus, asyncRegStatus);
	aPhone.CancelAsyncRequest(EMobilePhoneGetNetworkRegistrationStatus);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);

	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetNetworkRegistrationStatus (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncRegStatus==DMMTSY_PHONE_REGISTRATION_STATUS1);	
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetNetworkRegistrationStatus (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// NotifyNetworkRegistrationStatusChange

	aPhone.NotifyNetworkRegistrationStatusChange(aReqStatus, asyncRegStatus);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(asyncRegStatus==DMMTSY_PHONE_REGISTRATION_STATUS2);

	aPhone.NotifyNetworkRegistrationStatusChange(aReqStatus, asyncRegStatus);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNetworkRegistrationStatusChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncRegStatus==DMMTSY_PHONE_REGISTRATION_STATUS2);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNetworkRegistrationStatusChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
		
	//Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneDetectedNetworks* retrieve=CRetrieveMobilePhoneDetectedNetworks::NewL(aPhone);
    retrieve->Start(aReqStatus);
    delete retrieve;
 	retrieve = NULL;
 	User::WaitForRequest(aReqStatus);
 	TEST(aReqStatus == KErrCancel);


	//
	// Get Detected Networks
	//
	// Get detected networks using asynchronous AO version
	CTestGetDetectedNetworks* getNetworks=CTestGetDetectedNetworks::NewLC(aPhone, this);

	//Set test version to ETestV1 to test the V1 list class and list retrieval method
	getNetworks->SetTestVersion(CTestGetDetectedNetworks::ETestV1);
	getNetworks->TestRetrieve();
	
	//Delete test object and start afresh
	CleanupStack::PopAndDestroy(); //getNetworks
	getNetworks=CTestGetDetectedNetworks::NewLC(aPhone, this);	
	
	//Set test version to ETestV2 to test the V2 list class and list retrieval method
	getNetworks->SetTestVersion(CTestGetDetectedNetworks::ETestV2);
	getNetworks->TestRetrieve();

	//Delete test object and start afresh
	CleanupStack::PopAndDestroy(); //getNetworks
	getNetworks=CTestGetDetectedNetworks::NewLC(aPhone, this);

	//Set test version to ETestV5 to test the V5 list class and list retrieval method
	getNetworks->SetTestVersion(CTestGetDetectedNetworks::ETestV5);
	getNetworks->TestRetrieve();

	//Delete test object and start afresh
	CleanupStack::PopAndDestroy(); //getNetworks

	getNetworks=CTestGetDetectedNetworks::NewLC(aPhone, this);

	//Set test version to ETestV1 and test starting and then cancelling retrieve method
	getNetworks->SetTestVersion(CTestGetDetectedNetworks::ETestV1);
	getNetworks->TestCancel();

	//Delete test object and start afresh
	CleanupStack::PopAndDestroy(); //getNetworks
	getNetworks=CTestGetDetectedNetworks::NewLC(aPhone, this);
		
	//Set test version to ETestV2 and test starting and then cancelling retrieve method
	getNetworks->SetTestVersion(CTestGetDetectedNetworks::ETestV2);
	getNetworks->TestCancel();
		
	CleanupStack::PopAndDestroy(); //getNetworks
	/* - commenting as the server side is updated without V5  - start */
	/*
	getNetworks=CTestGetDetectedNetworks::NewLC(aPhone, this);

	//Set test version to ETestV5 and test starting and then cancelling retrieve method
	getNetworks->SetTestVersion(CTestGetDetectedNetworks::ETestV5);
	getNetworks->TestCancel();
	
	CleanupStack::PopAndDestroy();//getNetworks*/

	/* - commenting as the server side is updated without V5  - end */

	//

	// Test Network Selection Setting

	// Get Network Selection

	RMobilePhone::TMobilePhoneNetworkSelectionV1 networkSelection;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg networkSelectionPckg(networkSelection);
	
	TEST(aPhone.GetNetworkSelectionSetting(networkSelectionPckg) == KErrNone);
	
	TEST(networkSelection.iMethod==DMMTSY_PHONE_NETWORK_SELECTION_METHOD1);
	TEST(networkSelection.iBandClass==DMMTSY_PHONE_NETWORK_SELECTION_BAND1);
	TEST(networkSelection.iOperationMode==DMMTSY_PHONE_NETWORK_SELECTION_OPERATION1);

	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetNetworkSelectionSetting (sync) passed"), iTestCount++);

	// Set Network Selection
	networkSelection.iMethod=DMMTSY_PHONE_NETWORK_SELECTION_METHOD2;
	networkSelection.iBandClass=DMMTSY_PHONE_NETWORK_SELECTION_BAND2;
	networkSelection.iOperationMode=DMMTSY_PHONE_NETWORK_SELECTION_OPERATION2;

	aPhone.SetNetworkSelectionSetting(aReqStatus,networkSelectionPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	aPhone.SetNetworkSelectionSetting(aReqStatus,networkSelectionPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneSetNetworkSelectionSetting);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetNetworkSelectionSetting (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetNetworkSelectionSetting (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	
	// Test Notify Network Selection Setting Change & Cancel

	aPhone.NotifyNetworkSelectionSettingChange(aReqStatus, networkSelectionPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	TEST(networkSelection.iMethod==DMMTSY_PHONE_NETWORK_SELECTION_METHOD1);
	TEST(networkSelection.iBandClass==DMMTSY_PHONE_NETWORK_SELECTION_BAND1);
	TEST(networkSelection.iOperationMode==DMMTSY_PHONE_NETWORK_SELECTION_OPERATION1);

	aPhone.NotifyNetworkSelectionSettingChange(aReqStatus, networkSelectionPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSelectionSettingChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNetworkSelectionSettingChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNetworkSelectionSettingChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	
	// Test SelecttNetwork

	// asynchronous

	TBool isManual=ETrue;
	RMobilePhone::TMobilePhoneNetworkManualSelection manualSelection;
	manualSelection.iCountry=DMMTSY_PHONE_COUNTRY_ID;
	manualSelection.iNetwork=DMMTSY_PHONE_NETWORK_ID;
	aPhone.SelectNetwork(aReqStatus, isManual, manualSelection);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.SelectNetwork(aReqStatus, isManual, manualSelection);
	aPhone.CancelAsyncRequest(EMobilePhoneSelectNetwork);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SelectNetwork (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SelectNetwork (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	//	test.Next(_L("OK: RMobilePhone's SelectNetwork OK"));
	// Test Network Time Information Methods

	RMobilePhone::TMobilePhoneNITZ networkTimeInfo;
	RMobilePhone::TMobilePhoneNITZ asyncNetworkTimeInfo(DMMTSY_YEAR, DMMTSY_MONTH, DMMTSY_DAY, DMMTSY_HOUR, DMMTSY_MIN, DMMTSY_SEC, DMMTSY_MICROSEC);

	TEST(aPhone.GetNITZInfo(networkTimeInfo) == KErrNone);
	ValidateNITZInfo(networkTimeInfo);

	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetNITZInfo (sync) passed"), iTestCount++);

	// Test NotifyNITZInfoChange async and cancel

	aPhone.NotifyNITZInfoChange(aReqStatus,asyncNetworkTimeInfo);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	ValidateNITZInfo(asyncNetworkTimeInfo);

	aPhone.NotifyNITZInfoChange(aReqStatus,asyncNetworkTimeInfo);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyNITZInfoChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNITZInfoChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateNITZInfo(asyncNetworkTimeInfo);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNITZInfoChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
		
	//	test.Next(_L("OK: RMobilePhone's NITZ Info OK"));
		
	// Test Service Provider Name

	// asynchronous
	RMobilePhone::TMobilePhoneServiceProviderNameV2 serviceProvider;
	RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg serviceProviderPckg(serviceProvider);
	
	aPhone.GetServiceProviderName(aReqStatus,serviceProviderPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	ValidateSPName(serviceProvider);

	// asynchronous & cancel
	aPhone.GetServiceProviderName(aReqStatus,serviceProviderPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetServiceProviderName);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetServiceProviderName (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateSPName(serviceProvider);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetServiceProviderName (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
		
	// GetNetworkInvScanSetting
	
	// synchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetNetworkInvScanSetting sync."), iTestCount++);
	RMobilePhone::TMobilePhoneInvestigationScan setting;
	TInt error = aPhone.GetNetworkInvScanSetting(setting);
	TEST(error==KErrNone);
	TEST(setting == DMMTSY_PHONE_NETWORK_INV_SCAN);
	
	// NotifyNetworkInvScanChange
	
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNetworkInvScanChange async."), iTestCount++);
	aPhone.NotifyNetworkInvScanChange(aReqStatus, setting);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(setting == DMMTSY_PHONE_NETWORK_INV_SCAN);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNetworkInvScanChange async & cancel."), iTestCount++);
	aPhone.NotifyNetworkInvScanChange(aReqStatus, setting);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkInvScanChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrCancel);
	
	
	// NotifyNetworkInvScanEvent
	RMobilePhone::TMobilePhoneInvestigationScanEvent event;
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNetworkInvScanEvent async."), iTestCount++);
	aPhone.NotifyNetworkInvScanEvent(aReqStatus, event);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(event == DMMTSY_PHONE_NETWORK_INV_SCAN_EVENT);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNetworkInvScanEvent async & cancel."), iTestCount++);
	aPhone.NotifyNetworkInvScanEvent(aReqStatus, event);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkInvScanEvent);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrCancel);

	//Test GetCurrentNetworkName
		
	RMobilePhone::TMobilePhoneNetworkNameV3 networkName;
	RMobilePhone::TMobilePhoneNetworkNameV3Pckg networkNamePckg(networkName);
		
	RMobilePhone::TMobilePhoneOPlmnV3 plmn;
	RMobilePhone::TMobilePhoneOPlmnV3Pckg plmnPckg(plmn);	
	
	//asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCurrentNetworkName async."), iTestCount++);
	aPhone.GetCurrentNetworkName(aReqStatus, networkNamePckg, plmnPckg);
	User::WaitForRequest(aReqStatus);
	
	TEST(aReqStatus.Int() == KErrNone);	
	
	TEST(networkName.iLongName == DMMTSY_PHONE_HOME_NETWORK_LONG_NAME);
	TEST(networkName.iShortName == DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME);
	TEST(networkName.iOtherNames == DMMTSY_PHONE_HOME_NETWORK_OTHER_NAMES);

	TEST(plmn.iCountryCode == DMMTSY_PHONE_COUNTRY_ID);
	TEST(plmn.iNetworkId == DMMTSY_PHONE_NETWORK_ID);
	TEST(plmn.iPNNid == DMMTSY_PHONE_HOME_NETWORK_NAME_LOC_ID);		
	TEST(plmn.iFirstLocationAreaCode == DMMTSY_PHONE_HOME_NETWORK_FIRST_ID);
	TEST(plmn.iLastLocationAreaCode == DMMTSY_PHONE_HOME_NETWORK_LAST_ID);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCurrentNetworkName async & cancel."), iTestCount++);
	aPhone.GetCurrentNetworkName(aReqStatus, networkNamePckg, plmnPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetNetworkName);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);	
	
	//Test Get Preferred Networks 
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntry1;
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntry2;
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntry3;
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntry4; //- FOR WLAN
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntryFetch;
	
	preferredNetworkEntry1.iAccess = DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM;
	preferredNetworkEntry1.iUserDefined = DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE;
	preferredNetworkEntry1.iCountryCode = DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID1;
	preferredNetworkEntry1.iNetworkId = DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1;
	
	preferredNetworkEntry2.iAccess = DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSMC;
	preferredNetworkEntry2.iUserDefined = DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE;
	preferredNetworkEntry2.iCountryCode = DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2;
	preferredNetworkEntry2.iNetworkId = DMMTSY_PHONE_NTWK_LIST_NETWORK_ID2;
	
	preferredNetworkEntry3.iAccess = DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_UTRAN;
	preferredNetworkEntry3.iUserDefined = DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE;
	preferredNetworkEntry3.iCountryCode = DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2;
	preferredNetworkEntry3.iNetworkId = DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1;
	
	preferredNetworkEntry4.iAccess = DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_WLAN;  
	preferredNetworkEntry4.iUserDefined = DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE;
	preferredNetworkEntry4.iCountryCode = DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2;
	preferredNetworkEntry4.iNetworkId = DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1;
	CMobilePhoneStoredNetworkList* networklist =  CMobilePhoneStoredNetworkList::NewL();
	CleanupStack::PushL(networklist);
	
	//add an entry
	networklist->AddEntryL(preferredNetworkEntry1);

	preferredNetworkEntryFetch = networklist->GetEntryL(0);
	TEST(preferredNetworkEntryFetch.iAccess == DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM);
	TEST(preferredNetworkEntryFetch.iUserDefined == DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE);
	TEST(preferredNetworkEntryFetch.iCountryCode == DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID1);
	TEST(preferredNetworkEntryFetch.iNetworkId == DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1);	
	
	//try and insert an entry in amongst non-user defined entries
	TInt err = networklist->InsertEntryL(0, preferredNetworkEntry2);
	TEST(err==KErrArgument);
	
	//add at the end
	networklist->AddEntryL(preferredNetworkEntry3);	
	networklist->AddEntryL(preferredNetworkEntry4);	 //- FOR WLAN
	
	err = networklist->InsertEntryL(1, preferredNetworkEntry2);
	TEST(err==KErrNone);
	
	TInt count = networklist->Enumerate();
	TEST(count==DMMTSY_PHONE_NTWK_LIST_ENTRIES);
	
	//try deleting a non-user define entry
	err = networklist->DeleteEntryL(1);
	TEST(err==KErrArgument);
	
	//try deleting a user define entry
	err = networklist->DeleteEntryL(2);
	TEST(err==KErrNone);
	
	count = networklist->Enumerate();
	TEST(count==(DMMTSY_PHONE_NTWK_LIST_ENTRIES-1));
	
	//re-add it
	networklist->AddEntryL(preferredNetworkEntry3);	
	
	//try changing a non-user define entry
	err = networklist->ChangeEntryL(1, preferredNetworkEntry1);
	TEST(err==KErrArgument);
	
	//try changing a user define entry
	err = networklist->ChangeEntryL(2, preferredNetworkEntry1);
	TEST(err==KErrNone);
	
	//check its been updated
	preferredNetworkEntryFetch = networklist->GetEntryL(2);
	TEST(preferredNetworkEntryFetch.iAccess == DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM);
	TEST(preferredNetworkEntryFetch.iUserDefined == DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE);
	TEST(preferredNetworkEntryFetch.iCountryCode == DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID1);
	TEST(preferredNetworkEntryFetch.iNetworkId == DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1);
	
	CleanupStack::PopAndDestroy();	//networklist
	
	//Test retrieval of the list
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetPreferredNetworks async."), iTestCount++);
	
	//Test deletion of list object whilst async request in progress
	CRetrieveMobilePhonePreferredNetworks* retrievepn=CRetrieveMobilePhonePreferredNetworks::NewL(aPhone);
	retrievepn->Start(aReqStatus);
	delete retrievepn;
 	retrievepn = NULL;
 	User::WaitForRequest(aReqStatus);
 	TEST(aReqStatus.Int() == KErrCancel);	
 	
 	//Get prefrred stored networks
 	
 	// Get prefrred stored networks using asynchronous AO version
	CTestGetPreferredNetworks* prefNetworks=CTestGetPreferredNetworks::NewLC(aPhone, this);
	prefNetworks->Start();
	CActiveScheduler::Start();
    
	// ensure that the same AO can be used to retrieve more than one list in succession

	prefNetworks->Start();
	CActiveScheduler::Start();

	// I only need to delete the search object because the destructor
	// of this will also delete the list object.
	CleanupStack::PopAndDestroy();  // prefNetworks

	// Now test the cancelling
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetPreferredNetworks async & cancel."), iTestCount++);

    prefNetworks=CTestGetPreferredNetworks::NewLC(aPhone, this);
    prefNetworks->Start();

	CTestCanceller* cancelPrefNetworks = prefNetworks->RetrieveCancelPtr();
	cancelPrefNetworks->Call();

    CActiveScheduler::Start();
	TEST(prefNetworks->iStatus==KErrCancel);	

	// Ensure that AO can be used after its previous transaction was cancelled

    prefNetworks->Start();
    CActiveScheduler::Start();
    
    CleanupStack::PopAndDestroy();  // prefNetworks	
    
	//Test  RMobilePhone::StorePreferredNetworksListL
	
	INFO_PRINTF2(_L("Test %d - RMobilePhone::StorePreferredNetworksListL async."), iTestCount++);  
	
	networklist =  CMobilePhoneStoredNetworkList::NewL();
	CleanupStack::PushL(networklist);
	
	networklist->AddEntryL(preferredNetworkEntry4); // FOR WLAN
	networklist->AddEntryL(preferredNetworkEntry3);
	networklist->AddEntryL(preferredNetworkEntry2);
	networklist->AddEntryL(preferredNetworkEntry1);
	
	aPhone.StorePreferredNetworksListL(aReqStatus, networklist);
	User::WaitForRequest(aReqStatus);
	
	TEST(aReqStatus==KErrNone);
	
	INFO_PRINTF2(_L("Test %d - RMobilePhone::StorePreferredNetworksListL async & cancel."), iTestCount++);
	
	aPhone.StorePreferredNetworksListL(aReqStatus, networklist);
	aPhone.CancelAsyncRequest(EMobilePhoneStorePreferredNetworksList);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	
	CleanupStack::PopAndDestroy();  // networklist	
	
	//Test RMobilePhone::NotifyPreferredNetworksListChange
	
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyPreferredNetworksListChange async."), iTestCount++);
	aPhone.NotifyPreferredNetworksListChange(aReqStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus==KErrNone);
	
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyPreferredNetworksListChange async & cancel."), iTestCount++);
	
	aPhone.NotifyPreferredNetworksListChange(aReqStatus);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyStorePreferredNetworksListChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	   	   	
	   	   	
	}

void CTestPhone::TestMobileCellInfoL(RMobilePhone& aPhone, TRequestStatus& aReqStatus)
	{
	INFO_PRINTF2(_L("Test %d - RMobilePhone::TestMobileCellInfoL"), iTestCount++);
	
	// test get API
	
	RMobilePhone::TMobilePhoneCellInfoV9 expCellInfo;
	expCellInfo.iCellId = DMMTSY_PHONE_CELL_INFO_CELLID;
	expCellInfo.iMode = DMMTSY_PHONE_CELL_INFO_MODE;
	expCellInfo.iTimingAdvance = DMMTSY_PHONE_CELL_INFO_TIMING_ADVANCE;
	expCellInfo.iStatus = DMMTSY_PHONE_CELL_INFO_STATUS;
	
	RMobilePhone::TMobilePhoneCellInfoV9 actCellInfo;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg actCellInfoPckg(actCellInfo);
	
	aPhone.GetCellInfo(aReqStatus,actCellInfoPckg);
	User::WaitForRequest(aReqStatus);
	
	TEST(aReqStatus.Int()==KErrNone);
	TEST(actCellInfo.iCellId == expCellInfo.iCellId);
	TEST(actCellInfo.iMode == expCellInfo.iMode);	
	TEST(actCellInfo.iTimingAdvance == expCellInfo.iTimingAdvance);
	TEST(actCellInfo.iStatus == expCellInfo.iStatus);
	
	// test cancel get API
	
	aPhone.GetCellInfo(aReqStatus,actCellInfoPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCellInfo);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	
	// test notify API
	
	RMobilePhone::TMobilePhoneCellInfoV9 expCellInfo2;
	expCellInfo2.iCellId = DMMTSY_PHONE_CELL_INFO_CELLID2;
	expCellInfo2.iMode = DMMTSY_PHONE_CELL_INFO_MODE2;
	expCellInfo2.iTimingAdvance = DMMTSY_PHONE_CELL_INFO_TIMING_ADVANCE2;
	expCellInfo2.iStatus = DMMTSY_PHONE_CELL_INFO_STATUS2;
	
	aPhone.NotifyCellInfoChange(aReqStatus,actCellInfoPckg);
	User::WaitForRequest(aReqStatus);
	
	TEST(aReqStatus.Int()==KErrNone);
	TEST(actCellInfo.iCellId == expCellInfo2.iCellId);
	TEST(actCellInfo.iMode == expCellInfo2.iMode);	
	TEST(actCellInfo.iTimingAdvance == expCellInfo2.iTimingAdvance);
	TEST(actCellInfo.iStatus == expCellInfo2.iStatus);
	
	// test cancel notify API
	
	aPhone.NotifyCellInfoChange(aReqStatus,actCellInfoPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCellInfoChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	
	}

void CTestPhone::TestMobileNetworkISVL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Get Current Network 3rdParty Info
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCurrentNetwork Etel3rdParty async."), iTestCount++);
	// asynchronous

	RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1 asyncCurrentNetworkISV;
	RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1Pckg asyncCurrentNetworkISVPckg(asyncCurrentNetworkISV);
	RMobilePhone::TMobilePhoneLocationAreaV1 asyncLocArea;

	aPhone.GetCurrentNetwork(aReqStatus, asyncCurrentNetworkISVPckg, asyncLocArea);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	// extract the returned Current network info from the package
	asyncCurrentNetworkISV=asyncCurrentNetworkISVPckg();
	ValidateCurrentNetworkISV(asyncCurrentNetworkISV, asyncLocArea);

	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCurrentNetwork Etel3rdParty async & cancel."), iTestCount++);
	aPhone.GetCurrentNetwork(aReqStatus, asyncCurrentNetworkISVPckg, asyncLocArea);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetwork);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);

	if (aReqStatus.Int()==KErrNone)
		{
		asyncCurrentNetworkISV=asyncCurrentNetworkISVPckg();
		ValidateCurrentNetworkISV(asyncCurrentNetworkISV, asyncLocArea);
		}
		
	// Notify Current Network Change 3rdParty
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCurrentNetworkChange Etel3rdParty async."), iTestCount++);
	aPhone.NotifyCurrentNetworkChange(aReqStatus, asyncCurrentNetworkISVPckg, asyncLocArea);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	asyncCurrentNetworkISV=asyncCurrentNetworkISVPckg();
	ValidateNotifyCurrentNetworkISVChange(asyncCurrentNetworkISV,asyncLocArea);

	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCurrentNetworkChange Etel3rdParty async & cancel."), iTestCount++);
	aPhone.NotifyCurrentNetworkChange(aReqStatus, asyncCurrentNetworkISVPckg, asyncLocArea);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrNone)
		{
		asyncCurrentNetworkISV=asyncCurrentNetworkISVPckg();
		ValidateNotifyCurrentNetworkISVChange(asyncCurrentNetworkISV,asyncLocArea);
		}


	// Get Current Network Info - No Location

	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCurrentNetwork (No Location) async."), iTestCount++);
	RMobilePhone::TMobilePhoneNetworkInfoV1 asyncCurrentNetworkNoLoc;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg asyncCurrentNetworkPckgNoLoc(asyncCurrentNetworkNoLoc);

	aPhone.GetCurrentNetwork(aReqStatus, asyncCurrentNetworkPckgNoLoc);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	// extract the returned Current network info from the package
	asyncCurrentNetworkNoLoc=asyncCurrentNetworkPckgNoLoc();
	ValidateCurrentNetwork(asyncCurrentNetworkNoLoc, asyncLocArea, EFalse);

	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCurrentNetwork async & cancel."), iTestCount++);
	aPhone.GetCurrentNetwork(aReqStatus, asyncCurrentNetworkPckgNoLoc);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetworkNoLocation);
   	User::WaitForRequest(aReqStatus);

	TEST(aReqStatus.Int()==KErrCancel || aReqStatus.Int()==KErrNone);
	if (aReqStatus.Int()==KErrNone)
		{	
		asyncCurrentNetworkNoLoc=asyncCurrentNetworkPckgNoLoc();
		ValidateCurrentNetwork(asyncCurrentNetworkNoLoc, asyncLocArea, EFalse);
		}


	RMobilePhone::TMobilePhoneNetworkInfoV1 asyncCurrentNetwork;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg asyncCurrentNetworkPckg(asyncCurrentNetwork);

	// Notify Current Network Change - No Location
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCurrentNetworkChange (No Location) async."), iTestCount++);
	aPhone.NotifyCurrentNetworkChange(aReqStatus, asyncCurrentNetworkPckg);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	asyncCurrentNetwork=asyncCurrentNetworkPckg();
	ValidateNotifyCurrentNetworkChange(asyncCurrentNetwork,asyncLocArea, EFalse);

	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCurrentNetworkChange (No Location) async & cancel."), iTestCount++);
	aPhone.NotifyCurrentNetworkChange(aReqStatus, asyncCurrentNetworkPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkNoLocationChange);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrNone)
		{
		asyncCurrentNetwork=asyncCurrentNetworkPckg();
		ValidateNotifyCurrentNetworkChange(asyncCurrentNetwork,asyncLocArea, EFalse);
		}
	}

void CTestPhone::TestMobilePrivacyL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	RMobilePhone::TMobilePhonePrivacy privacySetting;

	TEST(aPhone.GetDefaultPrivacy(privacySetting)==KErrNone);
	TEST(privacySetting==DMMTSY_PHONE_PRIVACY_SETTING1);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetDefaultPrivacy (sync) passed"), iTestCount++);

	aPhone.SetDefaultPrivacy(aReqStatus, privacySetting);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	aPhone.SetDefaultPrivacy(aReqStatus, privacySetting);
	aPhone.CancelAsyncRequest(EMobilePhoneSetDefaultPrivacy);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetDefaultPrivacy (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetDefaultPrivacy (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	aPhone.NotifyDefaultPrivacyChange(aReqStatus, privacySetting);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(privacySetting==DMMTSY_PHONE_PRIVACY_SETTING2);

	aPhone.NotifyDefaultPrivacyChange(aReqStatus, privacySetting);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyDefaultPrivacyChange);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyDefaultPrivacyChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyDefaultPrivacyChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

//	test.Next(_L("OK: RMobilePhone's MobilePhonePrivacy"));
	}

void CTestPhone::TestMobileCallServiceCapsL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	TUint32 caps;

	// Get Call Service caps
	
	TEST(aPhone.GetCallServiceCaps(caps)==KErrNone);
	TEST(caps==DMMTSY_PHONE_CALL_SERVICE_CAPS1);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCallServiceCaps (sync) passed"), iTestCount++);

	// Notify Call Service Caps Change

	// asynchronous

	aPhone.NotifyCallServiceCapsChange(aReqStatus, caps);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(caps==DMMTSY_PHONE_CALL_SERVICE_CAPS2);

	// asynchronous & cancel

	aPhone.NotifyCallServiceCapsChange(aReqStatus, caps);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCallServiceCapsChange);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCallServiceCapsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(caps==DMMTSY_PHONE_CALL_SERVICE_CAPS2);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCallServiceCapsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
//	test.Next(_L("OK: RMobilePhone's MobilePhoneCallServiceCaps"));
	}

void CTestPhone::TestMobileUserNetworkL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Program a feature code
	TBuf<8> featureCode(DMMTSY_PHONE_FEATURE_CODE);
	RMobilePhone::TMobilePhoneNetworkService service=DMMTSY_PHONE_NETWORK_SERVICE;
	RMobilePhone::TMobilePhoneServiceAction serviceAction=DMMTSY_PHONE_NETWORK_SERVICE_ACTION;

	// asynchronous

	aPhone.ProgramFeatureCode(aReqStatus, featureCode, service,serviceAction);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	// asynchronous & cancel

	aPhone.ProgramFeatureCode(aReqStatus, featureCode, service,serviceAction);
	aPhone.CancelAsyncRequest(EMobilePhoneProgramFeatureCode);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::ProgramFeatureCode (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::ProgramFeatureCode (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Get a pre-programmed feature code

	service=DMMTSY_PHONE_NETWORK_SERVICE2;
	serviceAction=DMMTSY_PHONE_NETWORK_SERVICE_ACTION2;
	
	// asynchronous

	TBuf<8> asyncGetFC;
	aPhone.GetFeatureCode(aReqStatus,asyncGetFC,service,serviceAction);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(asyncGetFC.Compare(DMMTSY_PHONE_FEATURE_CODE2) == 0);

	// asynchronous & cancel

	aPhone.GetFeatureCode(aReqStatus,asyncGetFC,service,serviceAction);
	aPhone.CancelAsyncRequest(EMobilePhoneGetFeatureCode);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);

	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetFeatureCode (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncGetFC.Compare(DMMTSY_PHONE_FEATURE_CODE2) == 0);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetFeatureCode (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Send Network Service Request

	TBuf<20> serviceRequest(DMMTSY_PHONE_NETWORK_SERVICE_REQUEST);

	// asynchronous

	aPhone.SendNetworkServiceRequest(aReqStatus, serviceRequest);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	// asynchronous & cancel

	aPhone.SendNetworkServiceRequest(aReqStatus, serviceRequest);
	aPhone.CancelAsyncRequest(EMobilePhoneSendNetworkServiceRequest);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);

	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SendNetworkServiceRequest (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SendNetworkServiceRequest (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

//	test.Next(_L("OK: RMobilePhone's MobilePhoneUserNetworkAccess"));

	// Notify Send Network Service Request
	
	//asynchronous
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 ssRequest;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg ssRequestPckg(ssRequest);
	
	aPhone.NotifySendNetworkServiceRequest(aReqStatus, DMMTSY_PHONE_NOTIFY_SERVICEREQUEST, ssRequestPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(ssRequest.iOpCode == DMMTSY_PHONE_NOTIFY_OPCODE);
	TEST(ssRequest.iAdditionalInfo == DMMTSY_PHONE_NOTIFY_ADDINFO);
	
	//asynchronous & cancel
	
	aPhone.NotifySendNetworkServiceRequest(aReqStatus, DMMTSY_PHONE_NOTIFY_SERVICEREQUEST, ssRequestPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifySendNetworkServiceRequest);
	User::WaitForRequest(aReqStatus);
	
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySendNetworkServiceRequest (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySendNetworkServiceRequest (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	//Test for NotifyAllSendNetworkServiceRequest, which returns type of SS operation notification	
	RMobilePhone::TMobilePhoneNotifySendSSOperation ssOperation = RMobilePhone::ESendSSInvoke;	
	aPhone.NotifyAllSendNetworkServiceRequest(aReqStatus, ssOperation, ssRequestPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(ssOperation == DMMTSY_PHONE_NOTIFY_SERVICEREQUEST_ERROR);
	TEST(ssRequest.iOpCode == DMMTSY_PHONE_NOTIFY_RETURN_ERROR_PROBLEM_CODE);
	TEST(ssRequest.iAdditionalInfo == DMMTSY_PHONE_NOTIFY_ADDINFO);
	
	//asynchronous & cancel	
	aPhone.NotifyAllSendNetworkServiceRequest(aReqStatus, ssOperation, ssRequestPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyAllSendNetworkServiceRequest);
	User::WaitForRequest(aReqStatus);
	
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAllSendNetworkServiceRequest (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAllSendNetworkServiceRequest (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	}

void CTestPhone::TestMobileCallForwardingL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Get Call Forwarding Status

	RMobilePhone::TMobilePhoneCFCondition condition=DMMTSY_PHONE_CF_CONDITION_CFU;
	
	//Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneCFList* retrieve=CRetrieveMobilePhoneCFList::NewL(aPhone);
    retrieve->Start(aReqStatus, DMMTSY_PHONE_CF_CONDITION_CFU, DMMTSY_PHONE_CF_LOCATION);
    delete retrieve;
 	retrieve = NULL;
 	User::WaitForRequest(aReqStatus);
 	TEST(aReqStatus == KErrCancel);

	// Get call forwarding status using asynchronous AO version
	CTestGetCFStatus* getCFStatus=CTestGetCFStatus::NewLC(aPhone, this);
    getCFStatus->Start();
    CActiveScheduler::Start();

	// Ensure that AO can be used to retrieve more than one list.

    getCFStatus->Start();
    CActiveScheduler::Start();
    //Test version of CRetrieveMobilePhoneCFList::Start() which passes a service group, passing the EFaxService service group
    getCFStatus->StartSingleServiceGroup();
    CActiveScheduler::Start();

	// I only need to delete the search object because the destructor
	// of this will also delete the list object.
    CleanupStack::PopAndDestroy();  // getCFStatus

	// Now test the cancelling

	getCFStatus=CTestGetCFStatus::NewLC(aPhone, this);
    getCFStatus->Start();

	CTestCanceller* cancelGetCFStatus = getCFStatus->RetrieveCancelPtr();
	cancelGetCFStatus->Call();

    CActiveScheduler::Start();
	TBool cancelled = (getCFStatus->iStatus == KErrCancel);
	
	// Ensure that AO can be used even after previous request was cancelled.

    getCFStatus->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - CRetrieveMobilePhoneCFList (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - CRetrieveMobilePhoneCFList (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

    CleanupStack::PopAndDestroy();  // getCFStatus

	// Notify Call Forwarding Status

	// asynchronous

	aPhone.NotifyCallForwardingStatusChange(aReqStatus, condition);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(condition==DMMTSY_PHONE_CF_CONDITION_ALL);

	// asynchronous & cancel

	aPhone.NotifyCallForwardingStatusChange(aReqStatus, condition);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCallForwardingStatusChange);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCallForwardingStatusChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(condition==DMMTSY_PHONE_CF_CONDITION_ALL);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCallForwardingStatusChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Set Call Forwarding Status

	condition=DMMTSY_PHONE_CF_CONDITION_CFNRy;

	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=DMMTSY_PHONE_CF_SERVICE_GROUP_VOICE;
	newSetting.iAction=DMMTSY_PHONE_CF_SETTING;
	newSetting.iNumber.iTelNumber=DMMTSY_PHONE_TEL_NUMBER;
	newSetting.iTimeout=DMMTSY_PHONE_CF_TIMEOUT;

	// asynchronous

	aPhone.SetCallForwardingStatus(aReqStatus, condition, newSetting);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);

	// asynchronous & cancel

	aPhone.SetCallForwardingStatus(aReqStatus, condition, newSetting);
	aPhone.CancelAsyncRequest(EMobilePhoneSetCallForwardingStatus);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetCallForwardingStatus (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetCallForwardingStatus (async & cancel (Request Not Cancelled))passed"), iTestCount++);

	// Notify Call Forwarding Active
	RMobilePhone::TMobilePhoneCFActive active;
	RMobilePhone::TMobileService serviceGroup;

	aPhone.NotifyCallForwardingActive(aReqStatus, serviceGroup, active);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(active==DMMTSY_PHONE_CF_ACTIVE_TYPE);
	TEST(serviceGroup==DMMTSY_PHONE_CF_SERVICE_GROUP_FAX);

	// asynchronous & cancel

	aPhone.NotifyCallForwardingActive(aReqStatus, serviceGroup, active);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCallForwardingActive);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCallForwardingActive (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(active==DMMTSY_PHONE_CF_ACTIVE_TYPE);
		TEST(serviceGroup==DMMTSY_PHONE_CF_SERVICE_GROUP_FAX);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCallForwardingActive (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
		
	//Testing GetCallForwardingIndicator
	RMobilePhone::TMobilePhoneCFUIndicatorV3 cfuIndicatorV3;
	RMobilePhone::TMobilePhoneCFUIndicatorV3Pckg cfuIndicatorV3Packg(cfuIndicatorV3);
	cfuIndicatorV3.iIndicator = DMMTSY_PHONE_CFU_INDICATOR;
	
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCallForwardingIndicator async."), iTestCount++);
	aPhone.GetCallForwardingIndicator(aReqStatus, cfuIndicatorV3Packg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(cfuIndicatorV3.iIndicator == DMMTSY_PHONE_CFU_INDICATOR);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCallForwardingIndicator async & cancel."), iTestCount++);
	aPhone.GetCallForwardingIndicator(aReqStatus, cfuIndicatorV3Packg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCallForwardingIndicator);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrCancel);
	//Testing TMobilePhoneCFUIndicatorV6 data structures.
	//Testing GetCallForwardingIndicator 
	RMobilePhone::TMobilePhoneCFUIndicatorV6 cfuIndicatorV6;
	RMobilePhone::TMobilePhoneCFUIndicatorV6Pckg cfuIndicatorV6Packg(cfuIndicatorV6);
	
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCallForwardingIndicatorV6 async."), iTestCount++);
	aPhone.GetCallForwardingIndicator(aReqStatus, cfuIndicatorV6Packg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(cfuIndicatorV6.iIndicator == DMMTSY_PHONE_CFU_INDICATOR1);
	TEST(cfuIndicatorV6.iCFNumber.iTypeOfNumber == DMMTSY_PHONE_CFU_TON);
	TEST(cfuIndicatorV6.iCFNumber.iNumberPlan == DMMTSY_PHONE_CFU_NPI);
	TEST(cfuIndicatorV6.iCFNumber.iTelNumber == DMMTSY_PHONE_CFU_TELNUMBER);
	TEST(cfuIndicatorV6.iMultipleSubscriberProfileID == DMMTSY_PHONE_CFU_MSP);
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCallForwardingIndicatorV6 async & cancel."), iTestCount++);
	aPhone.GetCallForwardingIndicator(aReqStatus, cfuIndicatorV6Packg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCallForwardingIndicator);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrCancel);

//	test.Next(_L("OK: RMobilePhone's MobilePhoneCallForwarding"));
	}


void CTestPhone::TestMobileIdentityServiceL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Test Get Identity Service Status

	// asynchronous

	RMobilePhone::TMobilePhoneIdServiceStatus idServiceStatus;
	aPhone.GetIdentityServiceStatus(aReqStatus,DMMTSY_PHONE_ID_SERVICE,idServiceStatus,DMMTSY_PHONE_ID_LOCATION);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(idServiceStatus == DMMTSY_PHONE_ID_STATUS);

	// asynchronous and cancel

	aPhone.GetIdentityServiceStatus(aReqStatus,DMMTSY_PHONE_ID_SERVICE,idServiceStatus,DMMTSY_PHONE_ID_LOCATION);
	aPhone.CancelAsyncRequest(EMobilePhoneGetIdentityServiceStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIdentityServiceStatus (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(idServiceStatus == DMMTSY_PHONE_ID_STATUS);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIdentityServiceStatus (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
		
	//Test NotifyIdentityServiceStatus
	
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIdentityServiceStatus async."), iTestCount++);
	RMobilePhone::TMobilePhoneIdServiceStatus idNotifyServiceStatus;
	aPhone.NotifyIdentityServiceStatus(aReqStatus,DMMTSY_PHONE_ID_SERVICE,idNotifyServiceStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(idNotifyServiceStatus == DMMTSY_PHONE_ID_STATUS);
	
	// asynchronous and cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIdentityServiceStatus async & cancel."), iTestCount++);
	aPhone.NotifyIdentityServiceStatus(aReqStatus,DMMTSY_PHONE_ID_SERVICE,idNotifyServiceStatus);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyIdentityServiceStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);

	//Test SetIdentityServiceStatus
	
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIdentityServiceStatus async."), iTestCount++);
	aPhone.SetIdentityServiceStatus(aReqStatus,DMMTSY_PHONE_ID_SERVICE,DMMTSY_PHONE_ID_SETTING);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	// asynchronous and cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIdentityServiceStatus async & cancel."), iTestCount++);
	aPhone.SetIdentityServiceStatus(aReqStatus,DMMTSY_PHONE_ID_SERVICE,DMMTSY_PHONE_ID_SETTING);
	aPhone.CancelAsyncRequest(EMobilePhoneSetIdentityServiceStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	
	//Test NotifyIdentitySuppressionRejected
	
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIdentitySuppressionRejected async."), iTestCount++);
	aPhone.NotifyIdentitySuppressionRejected(aReqStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	// asynchronous and cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIdentitySuppressionRejected async & cancel."), iTestCount++);
	aPhone.NotifyIdentitySuppressionRejected(aReqStatus);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyIdentitySuppressionRejected);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
		
	}

void CTestPhone::TestMobileCallBarringL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//
	// Mobile Phone Call Barring Functional Unit
	//

	//Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneCBList* retrieve=CRetrieveMobilePhoneCBList::NewL(aPhone);
    retrieve->Start(aReqStatus, DMMTSY_PHONE_CB_CONDITION, DMMTSY_PHONE_CB_LOCATION);
    delete retrieve;
	retrieve = NULL;
 	User::WaitForRequest(aReqStatus);
 	TEST(aReqStatus == KErrCancel);

	// Get call barring list using asynchronous AO version
	CTestGetCB* getCB=CTestGetCB::NewLC(aPhone, this);
    getCB->Start();
    CActiveScheduler::Start();

	// Ensure that same AO can be used to retrieve more than one list

    getCB->Start();
    CActiveScheduler::Start();


    CleanupStack::PopAndDestroy();  // getCB

	// Now test the cancelling

	getCB=CTestGetCB::NewLC(aPhone, this);
    getCB->Start();

	CTestCanceller* cancelGetCB = getCB->RetrieveCancelPtr();
	cancelGetCB->Call();

    CActiveScheduler::Start();
	TBool cancelled = (getCB->iStatus == KErrCancel);

    getCB->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - CRetrieveMobilePhoneCBList (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - CRetrieveMobilePhoneCBList (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

    CleanupStack::PopAndDestroy();  // getCB
    
 
	// Test Set Call Barring Status

	RMobilePhone::TMobilePhoneCBChangeV1 cbChange;

	// asynchronous

	cbChange.iAction = DMMTSY_PHONE_CB_ACTION;
	cbChange.iPassword = DMMTSY_PHONE_CB_PASSWORD;
	aPhone.SetCallBarringStatus(aReqStatus,DMMTSY_PHONE_CB_CONDITION,cbChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.SetCallBarringStatus(aReqStatus,DMMTSY_PHONE_CB_CONDITION,cbChange);
	aPhone.CancelAsyncRequest(EMobilePhoneSetCallBarringStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetCallBarringStatus (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - CRetrieveMobilePhoneCBList (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		

	// Test NotifyCallBarring Status Change

	RMobilePhone::TMobilePhoneCBCondition cbCondition;

	// asynchronous

	aPhone.NotifyCallBarringStatusChange(aReqStatus,cbCondition);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(cbCondition == DMMTSY_PHONE_CB_CONDITION);

	// asynchronous & cancel

	aPhone.NotifyCallBarringStatusChange(aReqStatus,cbCondition);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCallBarringStatusChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCallBarringStatusChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(cbCondition == DMMTSY_PHONE_CB_CONDITION);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCallBarringStatusChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test Set Call Barring Password V1 & V2

	RMobilePhone::TMobilePhonePasswordChangeV1 cbPasswordChange;

	// asynchronous Version 1
	cbPasswordChange.iOldPassword = DMMTSY_PHONE_CB_PASSWORD;
	cbPasswordChange.iNewPassword = DMMTSY_PHONE_CB_NEW_PASSWORD;
	aPhone.SetCallBarringPassword(aReqStatus,cbPasswordChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel
	aPhone.SetCallBarringPassword(aReqStatus,cbPasswordChange);
	aPhone.CancelAsyncRequest(EMobilePhoneSetCallBarringPassword);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetCallBarringPassword (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetCallBarringPassword (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// asynchronous Version 2
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordPackageV2;
	passwordPackageV2.iOldPassword = DMMTSY_PHONE_CB_PASSWORD;
	passwordPackageV2.iNewPassword = DMMTSY_PHONE_CB_NEW_PASSWORD;
	passwordPackageV2.iVerifiedPassword = DMMTSY_PHONE_CB_VERIFIED_PASSWORD;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg ppPckgV2(passwordPackageV2);
	
	aPhone.SetSSPassword(aReqStatus, ppPckgV2, DMMTSY_PHONE_CB_ALL_SERVICES );
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel Version 2
	aPhone.SetSSPassword(aReqStatus, ppPckgV2, DMMTSY_PHONE_CB_ALL_SERVICES );
	aPhone.CancelAsyncRequest(EMobilePhoneSetSSPassword);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetSSPassword (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetSSPassword (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		
	//Testing GetCompMethodName
	
	//asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCompMethodName async."), iTestCount++);
	TBuf<10> alphatag;
	aPhone.GetCompMethodName(aReqStatus, alphatag, DMMTSY_PHONE_CB_COMP_METHOD_ID);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(alphatag.Compare(DMMTSY_PHONE_CB_ALPHA_TAG)==0);
	
	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCompMethodName async & cancel."), iTestCount++);
	aPhone.GetCompMethodName(aReqStatus, alphatag, DMMTSY_PHONE_CB_COMP_METHOD_ID);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCompMethodName);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);

	}

void CTestPhone::TestMobileCallWaitingL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	
	//Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneCWList* retrieve=CRetrieveMobilePhoneCWList::NewL(aPhone);
    retrieve->Start(aReqStatus, DMMTSY_PHONE_CW_LOCATION);
    delete retrieve;
 	retrieve = NULL;
 	User::WaitForRequest(aReqStatus);
 	TEST(aReqStatus == KErrCancel);

	// Get call waiting list using asynchronous AO version

	CTestGetCW* getCW=CTestGetCW::NewLC(aPhone, this);
    getCW->Start();
    CActiveScheduler::Start();

	// Ensure that AO can be used to retrieve more than one list

    getCW->Start();
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy();  // getCW

	// Now test the cancelling

	getCW=CTestGetCW::NewLC(aPhone, this);
    getCW->Start();

	CTestCanceller* cancelGetCW = getCW->RetrieveCancelPtr();
	cancelGetCW->Call();

    CActiveScheduler::Start();
	TBool cancelled = (getCW->iStatus == KErrCancel);

    getCW->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - CRetrieveMobilePhoneCWList (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - CRetrieveMobilePhoneCWList (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

    CleanupStack::PopAndDestroy();  // getCW

	// Test Set Call Waiting Status

	RMobilePhone::TMobilePhoneServiceAction cwAction = DMMTSY_PHONE_CW_SET_STATUS_ACTION;
	RMobilePhone::TMobileService cwServiceGroup = DMMTSY_PHONE_CW_SG0;

	// asynchronous

	aPhone.SetCallWaitingStatus(aReqStatus,cwServiceGroup,cwAction);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous and cancel

	aPhone.SetCallWaitingStatus(aReqStatus,cwServiceGroup,cwAction);
	aPhone.CancelAsyncRequest(EMobilePhoneSetCallWaitingStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetCallWaitingStatus (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetCallWaitingStatus (async & cancel (Request Not Cancelled)) passed"), iTestCount++);


	// Test NotifyCallWaitingStatusChange

	RMobilePhone::TMobilePhoneCWInfoEntryV1 cwInfoEntry;
	RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg cwInfoEntryPckg(cwInfoEntry);

	// asychronous

	aPhone.NotifyCallWaitingStatusChange(aReqStatus,cwInfoEntryPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(cwInfoEntry.iServiceGroup == DMMTSY_PHONE_CW_SG0);
	TEST(cwInfoEntry.iStatus == DMMTSY_PHONE_CW_STATUS0);

	// asychronous & cancel

	aPhone.NotifyCallWaitingStatusChange(aReqStatus,cwInfoEntryPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCallWaitingStatusChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCallWaitingStatusChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCallWaitingStatusChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		TEST(cwInfoEntry.iServiceGroup == DMMTSY_PHONE_CW_SG0);
		TEST(cwInfoEntry.iStatus == DMMTSY_PHONE_CW_STATUS0);
		}

//	test.Next(_L("OK: RMobilePhone's MobileCallWaiting"));
	}

void CTestPhone::TestMobileCallCompletionL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//
	// Test Mobile Call Completion
	//
	// Test GetCCBSStatus

	RMobilePhone::TMobilePhoneCCBSStatus ccbsStatus, ccbsStatusChange;

	// asynchronous

	aPhone.GetCCBSStatus(aReqStatus,ccbsStatus,DMMTSY_PHONE_CCC_LOCATION);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(ccbsStatus == DMMTSY_PHONE_CCC_STATUS);

	// asynchronous & cancel

	aPhone.GetCCBSStatus(aReqStatus,ccbsStatus,DMMTSY_PHONE_CCC_LOCATION);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCCBSStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCCBSStatus (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(ccbsStatus == DMMTSY_PHONE_CCC_STATUS);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCCBSStatus (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test Notify CcbsStatusChange

	// asynchronous

	aPhone.NotifyCCBSStatusChange(aReqStatus,ccbsStatusChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(ccbsStatusChange == DMMTSY_PHONE_CCC_STATUS);

	// asynchronous & cancel

	aPhone.NotifyCCBSStatusChange(aReqStatus,ccbsStatusChange);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCCBSStatusChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCCBSStatusChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(ccbsStatusChange == DMMTSY_PHONE_CCC_STATUS);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCCBSStatusChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}


 	//NotifyCCBS Recall V1
 
	// asynchronous
 	RMobilePhone::TMobilePhoneCCBSEntryV1 aCCBSEntry;
 	RMobilePhone::TMobilePhoneCCBSEntryV1Pckg aCCBSEntryPckg(aCCBSEntry);

 	aPhone.NotifyCCBSRecall(aReqStatus,aCCBSEntryPckg);
   	User::WaitForRequest(aReqStatus);
   	TEST(aReqStatus.Int() == KErrNone);

 	TEST(aCCBSEntry.iCcbsIndex==DMMTSY_PHONE_CCC_INDEX1);
 	TEST(aCCBSEntry.iServiceGroup==DMMTSY_PHONE_CCC_SERVICE_GROUP0);
 	TEST(aCCBSEntry.iDestination.iNumberPlan==DMMTSY_PHONE_CCC_DESTINATION_NP0);
 	TEST(aCCBSEntry.iDestination.iTelNumber== DMMTSY_PHONE_CCC_DESTINATION_NUM0);
 	TEST(aCCBSEntry.iDestination.iTypeOfNumber == DMMTSY_PHONE_CCC_DESTINATION_TON0);

 	// asynchronous & cancel
 
 	aPhone.NotifyCCBSRecall(aReqStatus,aCCBSEntryPckg);
 	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCCBSRecall);
	User::WaitForRequest(aReqStatus);
   	
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCCBSRecall V1 (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCCBSRecall V1 (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
 	 
 	//NotifyCCBS Recall V2
 
	// asynchronous
 	RMobilePhone::TMobilePhoneCCBSEntryV2 aCCBSEntryV2;
 	RMobilePhone::TMobilePhoneCCBSEntryV2Pckg aCCBSEntryPckgV2(aCCBSEntryV2);

 	aPhone.NotifyCCBSRecall(aReqStatus,aCCBSEntryPckgV2);
   	User::WaitForRequest(aReqStatus);
   	TEST(aReqStatus.Int() == KErrNone);

 	TEST(aCCBSEntryV2.iCcbsIndex==DMMTSY_PHONE_CCC_INDEX1);
 	TEST(aCCBSEntryV2.iServiceGroup==DMMTSY_PHONE_CCC_SERVICE_GROUP0);
 	TEST(aCCBSEntryV2.iDestination.iNumberPlan==DMMTSY_PHONE_CCC_DESTINATION_NP0);
 	TEST(aCCBSEntryV2.iDestination.iTelNumber== DMMTSY_PHONE_CCC_DESTINATION_NUM0);
 	TEST(aCCBSEntryV2.iDestination.iTypeOfNumber == DMMTSY_PHONE_CCC_DESTINATION_TON0);
	TEST(aCCBSEntryV2.iEvent == DMMTSY_PHONE_CCC_DESTINATION_CCBS_RECEIVED);
 
 	// asynchronous & cancel
 
 	aPhone.NotifyCCBSRecall(aReqStatus,aCCBSEntryPckgV2);
 	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCCBSRecall);
	User::WaitForRequest(aReqStatus);
   	
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCCBSRecall V2(async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCCBSRecall V2(async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	//Test multiple completion with different sized parameter packages
	//use a second phone client object
	RMobilePhone mmPhone2;
	TInt ret=mmPhone2.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);
	//set up a second parameter package
	TRequestStatus aReqStatus2;

	//set up first notification request using V2 package format
	aPhone.NotifyCCBSRecall(aReqStatus2,aCCBSEntryPckgV2);
	//set up second notification request using V1 package format
	mmPhone2.NotifyCCBSRecall(aReqStatus,aCCBSEntryPckg);

	User::WaitForRequest(aReqStatus);
	User::WaitForRequest(aReqStatus2);

	TEST(aReqStatus.Int() == KErrNone);
	TEST(aReqStatus2.Int() == KErrNone);

	//test parameters of V2 package
	TEST(aCCBSEntryV2.iCcbsIndex==DMMTSY_PHONE_CCC_INDEX1);
 	TEST(aCCBSEntryV2.iServiceGroup==DMMTSY_PHONE_CCC_SERVICE_GROUP0);
 	TEST(aCCBSEntryV2.iDestination.iNumberPlan==DMMTSY_PHONE_CCC_DESTINATION_NP0);
 	TEST(aCCBSEntryV2.iDestination.iTelNumber== DMMTSY_PHONE_CCC_DESTINATION_NUM0);
 	TEST(aCCBSEntryV2.iDestination.iTypeOfNumber == DMMTSY_PHONE_CCC_DESTINATION_TON0);
	TEST(aCCBSEntryV2.iEvent == DMMTSY_PHONE_CCC_DESTINATION_CCBS_RECEIVED);

	mmPhone2.Close();
 
 	// Test AcceptCCBSRecall
 	// asynchronous
 	TInt index=1;
 	TName aName;
 	aPhone.AcceptCCBSRecall(aReqStatus,index, aName);
    User::WaitForRequest(aReqStatus);
 	TEST(aReqStatus.Int()==KErrNone);
 	TEST(aName==DMMTSY_PHONE_CCC_CALLNAME0);
 
 	// asynchronous & cancel
 
 	aPhone.AcceptCCBSRecall(aReqStatus,index, aName);
 	aPhone.CancelAsyncRequest(EMobilePhoneAcceptCCBSRecall);
    User::WaitForRequest(aReqStatus);
 	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
 	if (aReqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::AcceptCCBSRecall (async & cancel (Cancelled Request)) passed"), iTestCount++);
 	else
 		INFO_PRINTF2(_L("Test %d - RMobilePhone::AcceptCCBSRecall (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
 	
 	// Test RefuseCCBSRecall
 	
 	TEST(aPhone.RefuseCCBSRecall(index)==KErrNone);
 	
 	// Test Decativate CCBS
 
 	// asynchronous
 
 	aPhone.DeactivateCCBS(aReqStatus, index);
    User::WaitForRequest(aReqStatus);
 	TEST(aReqStatus.Int()==KErrNone);
 
 	// asynchronous & cancel
 
 	aPhone.DeactivateCCBS(aReqStatus, index);
 	aPhone.CancelAsyncRequest(EMobilePhoneDeactivateCCBS);
    User::WaitForRequest(aReqStatus);
 	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);
 	if (aReqStatus.Int()==KErrCancel)
 		INFO_PRINTF2(_L("Test %d - RMobileCall::DeactivateCCBS (async & cancel (Cancelled Request)) passed"), iTestCount++);
   	else
 		INFO_PRINTF2(_L("Test %d - RMobileCall::DeactivateCCBS (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
 
 
 	
 	//Deactivate all, here the tsy pretends there is no active ccbs request
 	index=-1;
 	aPhone.DeactivateCCBS(aReqStatus, index);
    User::WaitForRequest(aReqStatus);
 	TEST(aReqStatus.Int()==KErrEtelCallNotActive);
 
	//Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneCcbsList* retrieve=CRetrieveMobilePhoneCcbsList::NewL(aPhone);
    retrieve->Start(aReqStatus);
    delete retrieve;
 	retrieve = NULL;
 	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrCancel);

	// Get call Completion list using asynchronous AO version

	CTestGetCcbs* getCcbs=CTestGetCcbs::NewLC(aPhone, this);
    getCcbs->Start();
    CActiveScheduler::Start();

	// Ensure that the same AO can be used to retrieve more than one list

    getCcbs->Start();
    CActiveScheduler::Start();


	CleanupStack::PopAndDestroy(getCcbs);  // getCcbs

	// Now test the cancelling

	getCcbs=CTestGetCcbs::NewLC(aPhone, this);
    getCcbs->Start();

	CTestCanceller* cancelGetCcbs = getCcbs->RetrieveCancelPtr();
	cancelGetCcbs->Call();

    CActiveScheduler::Start();
	TBool cancelled = (getCcbs->iStatus == KErrCancel);

	// Ensure that an AO can be used to retrieve a list after the previous transaction had been cancelled

    getCcbs->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - CRetrieveMobilePhoneCcbsList (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - CRetrieveMobilePhoneCcbsList (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

 	CleanupStack::PopAndDestroy(getCcbs);  // getCcbs
	}


void CTestPhone::TestMobileAlternatingCallL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//
	// Test Mobile Alternating Call
	// 

	// Test GetAlternatingCallCaps

	TUint32 alternatingCallCaps, asyncAlternatingCallCaps;
	TEST(aPhone.GetAlternatingCallCaps(alternatingCallCaps) == KErrNone);
	TEST(alternatingCallCaps == DMMTSY_PHONE_ALTERNATING_CALL_CAPS);

	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetAlternatingCallCaps (sync) passed"), iTestCount++);

	// Test NotifyAlternatingCallCapsChange

	// asynchronous

	aPhone.NotifyAlternatingCallCapsChange(aReqStatus,asyncAlternatingCallCaps);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(asyncAlternatingCallCaps == DMMTSY_PHONE_ALTERNATING_CALL_CAPS);

	// asynchronous & cancel
	
	aPhone.NotifyAlternatingCallCapsChange(aReqStatus,asyncAlternatingCallCaps);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyAlternatingCallCapsChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAlternatingCallCapsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncAlternatingCallCaps == DMMTSY_PHONE_ALTERNATING_CALL_CAPS);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAlternatingCallCapsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test GetAlternatingCallMode

	RMobilePhone::TMobilePhoneAlternatingCallMode acMode, asyncAcMode;
	RMobilePhone::TMobileService acService, asyncAcService;

	TEST(aPhone.GetAlternatingCallMode(acMode,acService) == KErrNone);
	TEST(acMode == DMMTSY_PHONE_ALTERNATING_CALL_MODE);
	TEST(acService == DMMTSY_PHONE_ALTERNATING_CALL_SERVICE);

	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetAlternatingCallMode (sync) passed"), iTestCount++);

	// Test SetAlternatingCallMode

	aPhone.SetAlternatingCallMode(aReqStatus,DMMTSY_PHONE_ALTERNATING_CALL_MODE,DMMTSY_PHONE_ALTERNATING_CALL_SERVICE);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	aPhone.SetAlternatingCallMode(aReqStatus,DMMTSY_PHONE_ALTERNATING_CALL_MODE,DMMTSY_PHONE_ALTERNATING_CALL_SERVICE);
	aPhone.CancelAsyncRequest(EMobilePhoneSetAlternatingCallMode);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetAlternatingCallMode (async (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetAlternatingCallMode (async (Request Not Cancelled)) passed"), iTestCount++);

	// Test NotifyAlternatingCallModeChange

	// asynchronous

	aPhone.NotifyAlternatingCallModeChange(aReqStatus,asyncAcMode,asyncAcService);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(asyncAcMode == DMMTSY_PHONE_ALTERNATING_CALL_MODE);
	TEST(asyncAcService == DMMTSY_PHONE_ALTERNATING_CALL_SERVICE);

	// asynchronous & cancel

	aPhone.NotifyAlternatingCallModeChange(aReqStatus,asyncAcMode,asyncAcService);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyAlternatingCallModeChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAlternatingCallModeChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncAcMode == DMMTSY_PHONE_ALTERNATING_CALL_MODE);
		TEST(asyncAcService == DMMTSY_PHONE_ALTERNATING_CALL_SERVICE);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAlternatingCallModeChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	
	}


void CTestPhone::TestMobileAlternateLineServiceL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//
	// Test Mobile Alternate Line Service
	//

	RMobilePhone::TMobilePhoneALSLine alsLine;

	// Test Get ALS Line

	// synchronous

	TEST(aPhone.GetALSLine(alsLine) == KErrNone);
	TEST(alsLine == DMMTSY_PHONE_ALS_LINE);

	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetALSLine (sync) passed"), iTestCount++);


	// Test Set ALS Line

	// asynchronous

	aPhone.SetALSLine(aReqStatus,alsLine);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.SetALSLine(aReqStatus,alsLine);
	aPhone.CancelAsyncRequest(EMobilePhoneSetALSLine);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetALSLine (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetALSLine (async & cancel (Request Not Cancelled)) passed"), iTestCount++);


	// Test NotifyALSLineChange

	// asynchronous

	RMobilePhone::TMobilePhoneALSLine notifyAlsLine;

	aPhone.NotifyALSLineChange(aReqStatus,notifyAlsLine);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(notifyAlsLine == DMMTSY_PHONE_ALS_LINE);

	// asynchronous & cancel

	aPhone.NotifyALSLineChange(aReqStatus,notifyAlsLine);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyALSLineChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyALSLineChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(notifyAlsLine == DMMTSY_PHONE_ALS_LINE);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyALSLineChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobilePhone's Mobile Alternate Line Service"));
	}


void CTestPhone::TestMobileCallCostL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//
	// Test Mobile Cost Functional Unit
	//

	TUint32 costCaps, asyncCostCaps;

	// Test Get Cost Caps

	TEST(aPhone.GetCostCaps(costCaps) == KErrNone);
	TEST(costCaps == DMMTSY_PHONE_COST_CAPS);

	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCostCaps (sync) passed"), iTestCount++);

	// Test Notify Cost Caps Change


	// asynchronous

	aPhone.NotifyCostCapsChange(aReqStatus,asyncCostCaps);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(asyncCostCaps == DMMTSY_PHONE_COST_CAPS);

	// asynchronous & cancel

	aPhone.NotifyCostCapsChange(aReqStatus,asyncCostCaps);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCostCapsChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCostCapsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncCostCaps == DMMTSY_PHONE_COST_CAPS);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCostCapsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	

	// Test ClearCostMeter

	// asynchronous
	RMobilePhone::TMobilePhoneCostMeters meter=DMMTSY_PHONE_COST_METER;

	aPhone.ClearCostMeter(aReqStatus, meter);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.ClearCostMeter(aReqStatus, meter);
	aPhone.CancelAsyncRequest(EMobilePhoneClearCostMeter);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::ClearCostMeter (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::ClearCostMeter (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test SetMaxCostMeter

	// asynchronous

	aPhone.SetMaxCostMeter(aReqStatus,DMMTSY_PHONE_COST_UNITS);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.SetMaxCostMeter(aReqStatus,DMMTSY_PHONE_COST_UNITS);
	aPhone.CancelAsyncRequest(EMobilePhoneSetMaxCostMeter);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetMaxCostMeter (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetMaxCostMeter (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test SetPuct

	// asynchronous

	RMobilePhone::TMobilePhonePuctV1 puct;
	RMobilePhone::TMobilePhonePuctV1Pckg puctPckg(puct);	

	puct.iCurrencyName = DMMTSY_PHONE_COST_PUCT_CNAME;
	puct.iPricePerUnit = DMMTSY_PHONE_COST_PUCT_PPU;


	aPhone.SetPuct(aReqStatus,puctPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.SetPuct(aReqStatus,puctPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneSetPuct);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetPuct (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetPuct (async & cancel (Request Not Cancelled)) passed"), iTestCount++);


	// Test GetCostInfo

	RMobilePhone::TMobilePhoneCostInfoV1 asyncCostInfo;
	RMobilePhone::TMobilePhoneCostInfoV1Pckg asyncCostInfoPckg(asyncCostInfo);
	
	// asynchronous

	aPhone.GetCostInfo(aReqStatus,asyncCostInfoPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	ValidateCostInfo(asyncCostInfo);

	// asynchronous & cancel

	aPhone.GetCostInfo(aReqStatus,asyncCostInfoPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCostInfo);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCostInfo (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateCostInfo(asyncCostInfo);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCostInfo (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test Notify CostInfoChange

	// asynchronous

	aPhone.NotifyCostInfoChange(aReqStatus,asyncCostInfoPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	ValidateCostInfo(asyncCostInfo);

	// asynchronous & cancel

	aPhone.NotifyCostInfoChange(aReqStatus,asyncCostInfoPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCostInfoChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCostInfoChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateCostInfo(asyncCostInfo);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCostInfoChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}


//	test.Next(_L("OK: RMobilePhone's Mobile Cost"));
	}


void CTestPhone::TestMobileSecurityL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//
	// Test Mobile Security Functional Unit
	//

	TUint32 secCaps, asyncCaps;

	// Test Security Caps

	TEST(aPhone.GetSecurityCaps(secCaps) == KErrNone);
	TEST(secCaps == DMMTSY_PHONE_SECURITY_CAPS);

	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetSecurityCaps (sync) passed"), iTestCount++);

	// Test Notify Security Caps Change

	// asynchronous

	aPhone.NotifySecurityCapsChange(aReqStatus,asyncCaps);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(asyncCaps == DMMTSY_PHONE_SECURITY_CAPS);

	// asynchronous & cancel

	aPhone.NotifySecurityCapsChange(aReqStatus,asyncCaps);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifySecurityCapsChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySecurityCapsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncCaps == DMMTSY_PHONE_SECURITY_CAPS);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySecurityCapsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test Get Lock Info

	RMobilePhone::TMobilePhoneLockInfoV1 asyncLockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg asyncLockInfoPckg(asyncLockInfo);

	// asynchronous

	aPhone.GetLockInfo(aReqStatus,DMMTSY_PHONE_SECURITY_LOCK,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);	
	TEST(asyncLockInfo.iStatus == DMMTSY_PHONE_SECURITY_STATUS);
	TEST(asyncLockInfo.iSetting == DMMTSY_PHONE_SECURITY_SETTING);

	// asynchronous & cancel

	aPhone.GetLockInfo(aReqStatus,DMMTSY_PHONE_SECURITY_LOCK,asyncLockInfoPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetLockInfo);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetLockInfo (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncLockInfo.iStatus == DMMTSY_PHONE_SECURITY_STATUS);
		TEST(asyncLockInfo.iSetting == DMMTSY_PHONE_SECURITY_SETTING);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetLockInfo (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test Notify Lock Info Change

	RMobilePhone::TMobilePhoneLock phoneLock;

	// asynchronous

	aPhone.NotifyLockInfoChange(aReqStatus,phoneLock,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(phoneLock == DMMTSY_PHONE_SECURITY_LOCK);
	TEST(asyncLockInfo.iStatus == DMMTSY_PHONE_SECURITY_STATUS);
	TEST(asyncLockInfo.iSetting == DMMTSY_PHONE_SECURITY_SETTING);


	// asynchronous & cancel

	aPhone.NotifyLockInfoChange(aReqStatus,phoneLock,asyncLockInfoPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyLockInfoChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyLockInfoChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(phoneLock == DMMTSY_PHONE_SECURITY_LOCK);
		TEST(asyncLockInfo.iStatus == DMMTSY_PHONE_SECURITY_STATUS);
		TEST(asyncLockInfo.iSetting == DMMTSY_PHONE_SECURITY_SETTING);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyLockInfoChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		} 


	// Test Set Lock Setting Change 

	// asynchronous

	aPhone.SetLockSetting(aReqStatus,DMMTSY_PHONE_SECURITY_SC_LOCK,DMMTSY_PHONE_SECURITY_SC_SETTING);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	
	// asynchronous & cancel

	aPhone.SetLockSetting(aReqStatus,DMMTSY_PHONE_SECURITY_SC_LOCK,DMMTSY_PHONE_SECURITY_SC_SETTING);
	aPhone.CancelAsyncRequest(EMobilePhoneSetLockSetting);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetLockSettingChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetLockSettingChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		


	// Test Change Security Code

	// asynchronous

	RMobilePhone::TMobilePhonePasswordChangeV1 passwordChange;

	passwordChange.iOldPassword = DMMTSY_PHONE_SECURITY_OLD_PASS;
	passwordChange.iNewPassword = DMMTSY_PHONE_SECURITY_NEW_PASS;
	aPhone.ChangeSecurityCode(aReqStatus,DMMTSY_PHONE_SECURITY_CODE,passwordChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.ChangeSecurityCode(aReqStatus,DMMTSY_PHONE_SECURITY_CODE,passwordChange);
	aPhone.CancelAsyncRequest(EMobilePhoneChangeSecurityCode);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::ChangeSecurityCode (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::ChangeSecurityCode (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		

	// Test NotifySecurityEvent

	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;

	aPhone.NotifySecurityEvent(aReqStatus,securityEvent);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(securityEvent == DMMTSY_PHONE_SECURITY_EVENT);

	aPhone.NotifySecurityEvent(aReqStatus,securityEvent);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifySecurityEvent);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySecurityEvent (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(securityEvent == DMMTSY_PHONE_SECURITY_EVENT);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySecurityEvent (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test Verify Security Code

	RMobilePhone::TMobilePassword code = DMMTSY_PHONE_SECURITY_OLD_PASS;
	RMobilePhone::TMobilePassword unblockCode = DMMTSY_PHONE_SECURITY_NEW_PASS;

	aPhone.VerifySecurityCode(aReqStatus,DMMTSY_PHONE_SECURITY_CODE,code,unblockCode);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	aPhone.VerifySecurityCode(aReqStatus,DMMTSY_PHONE_SECURITY_CODE,code,unblockCode);
	aPhone.CancelAsyncRequest(EMobilePhoneVerifySecurityCode);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::VerifySecurityCode (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::VerifySecurityCode (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test Abort Security Code

	TEST(aPhone.AbortSecurityCode(DMMTSY_PHONE_SECURITY_CODE) == KErrNone);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::AbortSecurityCode (sync) passed"), iTestCount++);


	// Test Get Security Code Info

	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 asyncSecurityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg asyncSecurityCodePckg(asyncSecurityCodeInfo);

	// asynchronous

	aPhone.GetSecurityCodeInfo(aReqStatus,DMMTSY_PHONE_SECURITY_CODE,asyncSecurityCodePckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);	
	TEST(asyncSecurityCodeInfo.iRemainingEntryAttempts == DMMTSY_PHONE_SECURITY_REMAINING_ATTEMPTS);

	// asynchronous & cancel

	aPhone.GetSecurityCodeInfo(aReqStatus,DMMTSY_PHONE_SECURITY_CODE,asyncSecurityCodePckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetSecurityCodeInfo);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetSecurityCodeInfo (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(asyncSecurityCodeInfo.iRemainingEntryAttempts == DMMTSY_PHONE_SECURITY_REMAINING_ATTEMPTS);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetSecurityCodeInfo (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test Notify Security Code Info Change

	RMobilePhone::TMobilePhoneSecurityCode securityCode;

	// asynchronous

	aPhone.NotifySecurityCodeInfoChange(aReqStatus,securityCode,asyncSecurityCodePckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(securityCode == DMMTSY_PHONE_SECURITY_CODE);
	TEST(asyncSecurityCodeInfo.iRemainingEntryAttempts == DMMTSY_PHONE_SECURITY_REMAINING_ATTEMPTS);


	// asynchronous & cancel

	aPhone.NotifySecurityCodeInfoChange(aReqStatus,securityCode,asyncSecurityCodePckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifySecurityCodeInfoChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySecurityCodeInfoChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(securityCode == DMMTSY_PHONE_SECURITY_CODE);
		TEST(asyncSecurityCodeInfo.iRemainingEntryAttempts == DMMTSY_PHONE_SECURITY_REMAINING_ATTEMPTS);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySecurityCodeInfoChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		} 

//	test.Next(_L("OK: RMobilePhone's Mobile Security"));
	}

void CTestPhone::TestMobileMessageWaitingL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Test NotifyMessageWaiting

	TInt mwCount = 0;

	// asynchronous

	aPhone.NotifyMessageWaiting(aReqStatus,mwCount);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(mwCount == DMMTSY_PHONE_MW_COUNT);

	// asynchronous & cancel

	aPhone.NotifyMessageWaiting(aReqStatus,mwCount);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyMessageWaiting);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::Notify Message Waiting (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(mwCount == DMMTSY_PHONE_MW_COUNT);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMessageWaiting (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	RMobilePhone::TMobilePhoneMessageWaitingV1 msgIndicators;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg msgIndicatorsPckg(msgIndicators);

	aPhone.GetIccMessageWaitingIndicators(aReqStatus, msgIndicatorsPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	TEST(msgIndicators.iDisplayStatus==DMMTSY_PHONE_DISPLAYSTATUS); 
	TEST(msgIndicators.iVoiceMsgs == DMMTSY_PHONE_VOICEMSGS);
	TEST(msgIndicators.iAuxVoiceMsgs == DMMTSY_PHONE_AUXVOICEMSGS);
	TEST(msgIndicators.iDataMsgs == DMMTSY_PHONE_DATAMSGS);
	TEST(msgIndicators.iFaxMsgs == DMMTSY_PHONE_FAXMSGS);
	TEST(msgIndicators.iEmailMsgs == DMMTSY_PHONE_EMAILMSGS);
	TEST(msgIndicators.iOtherMsgs == DMMTSY_PHONE_OTHERMSGS);
	
	aPhone.GetIccMessageWaitingIndicators(aReqStatus, msgIndicatorsPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetIccMessageWaitingIndicators);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIccMessageWaitingIndicators (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIccMessageWaitingIndicators (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	
	msgIndicators.iDisplayStatus=DMMTSY_PHONE_DISPLAYSTATUS; 
	msgIndicators.iVoiceMsgs = DMMTSY_PHONE_VOICEMSGS2;
	msgIndicators.iAuxVoiceMsgs = DMMTSY_PHONE_AUXVOICEMSGS2;
	msgIndicators.iDataMsgs = DMMTSY_PHONE_DATAMSGS2;
	msgIndicators.iFaxMsgs = DMMTSY_PHONE_FAXMSGS2;
	msgIndicators.iEmailMsgs = DMMTSY_PHONE_EMAILMSGS2;
	msgIndicators.iOtherMsgs = DMMTSY_PHONE_OTHERMSGS2;

	aPhone.SetIccMessageWaitingIndicators(aReqStatus, msgIndicatorsPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	aPhone.SetIccMessageWaitingIndicators(aReqStatus, msgIndicatorsPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneSetIccMessageWaitingIndicators);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetIccMessageWaitingIndicators (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetIccMessageWaitingIndicators (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	aPhone.NotifyIccMessageWaitingIndicatorsChange(aReqStatus, msgIndicatorsPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	TEST(msgIndicators.iDisplayStatus==DMMTSY_PHONE_DISPLAYSTATUS); 
	TEST(msgIndicators.iVoiceMsgs == DMMTSY_PHONE_VOICEMSGS2);
	TEST(msgIndicators.iAuxVoiceMsgs == DMMTSY_PHONE_AUXVOICEMSGS2);
	TEST(msgIndicators.iDataMsgs == DMMTSY_PHONE_DATAMSGS2);
	TEST(msgIndicators.iFaxMsgs == DMMTSY_PHONE_FAXMSGS2);
	TEST(msgIndicators.iEmailMsgs == DMMTSY_PHONE_EMAILMSGS2);
	TEST(msgIndicators.iOtherMsgs == DMMTSY_PHONE_OTHERMSGS2);

	aPhone.NotifyIccMessageWaitingIndicatorsChange(aReqStatus, msgIndicatorsPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyIccMessageWaitingIndicatorsChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIccMessageWaitingIndicatorsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIccMessageWaitingIndicatorsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	
	//Test with TMobilePhoneMessageWaitingV8 class
	RMobilePhone::TMobilePhoneMessageWaitingV8 msgIndicatorsV8;
	RMobilePhone::TMobilePhoneMessageWaitingV8Pckg msgIndicatorsV8Pckg(msgIndicatorsV8);

	aPhone.GetIccMessageWaitingIndicators(aReqStatus, msgIndicatorsV8Pckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	TEST(msgIndicatorsV8.iVideoMsgs == DMMTSY_PHONE_VIDEOMSGS1);
	
	aPhone.GetIccMessageWaitingIndicators(aReqStatus, msgIndicatorsV8Pckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetIccMessageWaitingIndicators);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIccMessageWaitingIndicators (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIccMessageWaitingIndicators (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	
	msgIndicatorsV8.iDisplayStatus=DMMTSY_PHONE_DISPLAYSTATUS; 
	msgIndicatorsV8.iVoiceMsgs = DMMTSY_PHONE_VOICEMSGS2;
	msgIndicatorsV8.iAuxVoiceMsgs = DMMTSY_PHONE_AUXVOICEMSGS2;
	msgIndicatorsV8.iDataMsgs = DMMTSY_PHONE_DATAMSGS2;
	msgIndicatorsV8.iFaxMsgs = DMMTSY_PHONE_FAXMSGS2;
	msgIndicatorsV8.iEmailMsgs = DMMTSY_PHONE_EMAILMSGS2;
	msgIndicatorsV8.iOtherMsgs = DMMTSY_PHONE_OTHERMSGS2;
	msgIndicatorsV8.iVideoMsgs = DMMTSY_PHONE_VIDEOMSGS2;

	aPhone.SetIccMessageWaitingIndicators(aReqStatus, msgIndicatorsV8Pckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	aPhone.SetIccMessageWaitingIndicators(aReqStatus, msgIndicatorsV8Pckg);
	aPhone.CancelAsyncRequest(EMobilePhoneSetIccMessageWaitingIndicators);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetIccMessageWaitingIndicators (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetIccMessageWaitingIndicators (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	aPhone.NotifyIccMessageWaitingIndicatorsChange(aReqStatus, msgIndicatorsV8Pckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	TEST(msgIndicatorsV8.iVideoMsgs == DMMTSY_PHONE_VIDEOMSGS2);	
	
	aPhone.NotifyIccMessageWaitingIndicatorsChange(aReqStatus, msgIndicatorsV8Pckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyIccMessageWaitingIndicatorsChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIccMessageWaitingIndicatorsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIccMessageWaitingIndicatorsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	}

void CTestPhone::TestMobileFixedDiallingNumbersL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Test GetFdnStatus

	RMobilePhone::TMobilePhoneFdnStatus fdnStatus,fdnStatus2;

	// synchronous

	TEST(aPhone.GetFdnStatus(fdnStatus) == KErrNone);
	TEST(fdnStatus == DMMTSY_PHONE_FDN_STATUS);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetFdnStatus (sync) passed"), iTestCount++);

	// asynchronous

	aPhone.GetFdnStatus(aReqStatus, fdnStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(fdnStatus == DMMTSY_PHONE_FDN_STATUS);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetFdnStatus (async) passed"), iTestCount++);

	// asynchronous & cancel

	aPhone.GetFdnStatus(aReqStatus, fdnStatus);
	aPhone.CancelAsyncRequest(EMobilePhoneGetFdnStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetFdnStatus (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(fdnStatus == DMMTSY_PHONE_FDN_STATUS);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetFdnStatus (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test SetFdnSetting

	// asynchronous

	aPhone.SetFdnSetting(aReqStatus,DMMTSY_PHONE_FDN_SETTING);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetFdnSetting (async) passed"), iTestCount++);

	// asynchronous & cancel

	aPhone.SetFdnSetting(aReqStatus,DMMTSY_PHONE_FDN_SETTING);
	aPhone.CancelAsyncRequest(EMobilePhoneSetFdnSetting);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetFdnSetting (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetFdnSetting (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		

	// Test Notify Fdn Status Change

	aPhone.NotifyFdnStatusChange(aReqStatus,fdnStatus2);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(fdnStatus2 == DMMTSY_PHONE_FDN_STATUS);

	aPhone.NotifyFdnStatusChange(aReqStatus,fdnStatus2);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyFdnStatusChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	if (aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyFdnStatusChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(fdnStatus2 == DMMTSY_PHONE_FDN_STATUS);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyFdnStatusChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	

	// CleanupStack::PopAndDestroy();  // getCcbs
	}

void CTestPhone::TestMobileMulticallSettings(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Test GetMulticallParams

	RMobilePhone::TMobilePhoneMulticallSettingsV1 bearerSettings;
	RMobilePhone::TMobilePhoneMulticallSettingsV1Pckg bearerSettingsPckg(bearerSettings);

	// asynchronous

	aPhone.GetMulticallParams(aReqStatus, bearerSettingsPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// extract the returned bearerSettings from the package

	bearerSettings=bearerSettingsPckg();
	ValidateMulticallParams(bearerSettings, 0);

	// asynchronous & cancel

	aPhone.GetMulticallParams(aReqStatus, bearerSettingsPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetMulticallParams);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetMulticallParams (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		// extract the returned bearerSettings from the package
		bearerSettings = bearerSettingsPckg();
		ValidateMulticallParams(bearerSettings, 0);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetMulticallParams (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test SetMulticallParams

	// asynchronous

	aPhone.SetMulticallParams(aReqStatus, DMMTSY_PHONE_USER_MAX_BEARERS1);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.SetMulticallParams(aReqStatus, DMMTSY_PHONE_USER_MAX_BEARERS1);
	aPhone.CancelAsyncRequest(EMobilePhoneSetMulticallParams);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetMulticallParams (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else		
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetMulticallParams (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	
	// Test NotifyMulticallParamsChange

	// asynchronous

	aPhone.NotifyMulticallParamsChange(aReqStatus, bearerSettingsPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// extract the returned bearerSettings from the package

	bearerSettings=bearerSettingsPckg();
	ValidateMulticallParams(bearerSettings, 1);

	// asynchronous & cancel

	aPhone.NotifyMulticallParamsChange(aReqStatus, bearerSettingsPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyMulticallParamsChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMulticallParamsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMulticallParamsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	//Test NotifyMulticallIndicatorChange
	RMobilePhone::TMobilePhoneMulticallIndicator indicator;
	
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMulticallIndicatorChange async."), iTestCount++);
	aPhone.NotifyMulticallIndicatorChange(aReqStatus, indicator);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(indicator == DMMTSY_PHONE_MULTICALL_IND);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMulticallIndicatorChange async & cancel."), iTestCount++);
	aPhone.NotifyMulticallIndicatorChange(aReqStatus, indicator);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyMulticallIndicatorChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	
	
	}

void CTestPhone::TestMobileIncomingCallTypes(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	// Test GetIncomingCallType

	RMobileCall::TMobileDataCallParamsV1 dataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg dataParamsPckg(dataParams); 

	RMobilePhone::TMobilePhoneIncomingCallType callType;

	// asynchronous

	aPhone.GetIncomingCallType(aReqStatus, callType, dataParamsPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(callType == DMMTSY_PHONE_INCOMING_CALL_TYPES);

	// extract the returned dataParams from the package
	dataParams=dataParamsPckg();
	ValidateDataCallParams(dataParams, 0);

	// asynchronous & cancel

	aPhone.GetIncomingCallType(aReqStatus, callType, dataParamsPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetIncomingCallType);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIncomingCallType (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		// extract the returned dataParams from the package
		dataParams = dataParamsPckg();
		ValidateDataCallParams(dataParams, 0);
		TEST(callType == DMMTSY_PHONE_INCOMING_CALL_TYPES);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetIncomingCallType (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test SetIncomingCallType

	dataParams.iService = DMMTSY_DATA_CALL_SERVICE;
	dataParams.iSpeed = DMMTSY_DATA_CALL_SPEED;
	dataParams.iProtocol = DMMTSY_DATA_CALL_PROTOCOL;
	dataParams.iQoS = DMMTSY_DATA_CALL_QOS;
	dataParams.iRLPVersion = DMMTSY_DATA_CALL_RLP;
	dataParams.iV42bisReq = DMMTSY_DATA_CALL_V42BIS;

	// asynchronous

	aPhone.SetIncomingCallType(aReqStatus, DMMTSY_PHONE_INCOMING_CALL_TYPES, dataParamsPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.SetIncomingCallType(aReqStatus, DMMTSY_PHONE_INCOMING_CALL_TYPES, dataParamsPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneSetIncomingCallType);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetIncomingCallType (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else		
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetIncomingCallType (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test NotifyIncomingCallTypeChange

	aPhone.NotifyIncomingCallTypeChange(aReqStatus, callType, dataParamsPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// extract the returned dataParams from the package

	dataParams = dataParamsPckg();
	ValidateDataCallParams(dataParams, 1);

	// asynchronous & cancel

	aPhone.NotifyIncomingCallTypeChange(aReqStatus, callType, dataParamsPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyIncomingCallTypeChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIncomingCallTypeChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		// extract the returned dataParams from the package
		dataParams = dataParamsPckg();
		ValidateDataCallParams(dataParams, 1);
		TEST(callType == DMMTSY_PHONE_INCOMING_CALL_TYPES1);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyIncomingCallTypeChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	//test multiple completion with different parameter buffers

	//use a second phone client object
	RMobilePhone mmPhone2;
	TInt ret=mmPhone2.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);
	//set up a second parameter package
	TRequestStatus aReqStatus2;
	RMobileCall::TMobileDataCallParamsV2 dataParams2;
	RMobileCall::TMobileDataCallParamsV2Pckg dataParamsPckg2(dataParams2); 
	RMobilePhone::TMobilePhoneIncomingCallType callType2;

	//set up first notification request using V2 package format
	mmPhone2.NotifyIncomingCallTypeChange(aReqStatus2, callType2, dataParamsPckg2);
	//set up second notification request using V1 package format
	aPhone.NotifyIncomingCallTypeChange(aReqStatus, callType, dataParamsPckg);
	

	User::WaitForRequest(aReqStatus);
	User::WaitForRequest(aReqStatus2);

	//validate the version 2 package
	ValidateDataCallParams(dataParams2, 1);
	dataParams2.iBearerMode = DMMTSY_PHONE_MULTICALLBEARERMODE_NOTSUPPORTED;
	
	mmPhone2.Close();

//	test.Next(_L("OK: RMobilePhone's Incoming Call Types OK"));
	}

void CTestPhone::TestMobileUUSSetting(RMobilePhone& aPhone, TRequestStatus& aReqStatus)
	{
	// Test GetUUSSetting

	RMobilePhone::TMobilePhoneUUSSetting uusSetting;

	// asynchronous

	aPhone.GetUUSSetting(aReqStatus, uusSetting);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(uusSetting == DMMTSY_PHONE_UUS_SETTING1);

	// asynchronous & cancel

	aPhone.GetUUSSetting(aReqStatus, uusSetting);
	aPhone.CancelAsyncRequest(EMobilePhoneGetUUSSetting);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetUUSSetting (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(uusSetting == DMMTSY_PHONE_UUS_SETTING1);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetUUSSetting (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test SetUUSSetting

	// asynchronous

	aPhone.SetUUSSetting(aReqStatus, DMMTSY_PHONE_UUS_SETTING2);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.SetUUSSetting(aReqStatus, DMMTSY_PHONE_UUS_SETTING2);
	aPhone.CancelAsyncRequest(EMobilePhoneSetUUSSetting);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetUUSSetting (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetUUSSetting (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test NotifyUUSSettingChange

	// asynchronous

	aPhone.NotifyUUSSettingChange(aReqStatus, uusSetting);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(uusSetting == DMMTSY_PHONE_UUS_SETTING2);

	// asynchronous & cancel

	aPhone.NotifyUUSSettingChange(aReqStatus, uusSetting);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyUUSSettingChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyUUSSettingChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(uusSetting == DMMTSY_PHONE_UUS_SETTING2);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyUUSSettingChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	}

void CTestPhone::TestMultimediaCallSettings(RMobilePhone& aPhone, TRequestStatus& aReqStatus)
	{
	
	// Test GetMultimediaCallPreference

	RMobilePhone:: TMobilePhoneMultimediaSettings mmSetting;
	
	// synchronous
	aPhone.GetMultimediaCallPreference(mmSetting);
	TEST(mmSetting == DMMTSY_PHONE_MM_SETTING);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetMultimediaCallPreference (sync) passed"), iTestCount++);

	// Test GetMultimediaCallPreference

	// asynchronous
	
	aPhone.SetMultimediaCallPreference(aReqStatus, DMMTSY_PHONE_MM_SETTING);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.SetMultimediaCallPreference(aReqStatus, DMMTSY_PHONE_MM_SETTING);
	aPhone.CancelAsyncRequest(EMobilePhoneSetMultimediaCallPreference);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetMultimediaCallPreference (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetMultimediaCallPreference (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		
	// Test NotifyMultimediaCallPreferenceChange

	// asynchronous
	
	aPhone.NotifyMultimediaCallPreferenceChange(aReqStatus, mmSetting);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(mmSetting == DMMTSY_PHONE_MM_SETTING_2);

	// asynchronous & cancel

	aPhone.NotifyMultimediaCallPreferenceChange(aReqStatus, mmSetting);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyMultimediaCallPreferenceChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMultimediaCallPreferenceChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(mmSetting == DMMTSY_PHONE_MM_SETTING_2);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMultimediaCallPreferenceChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	}

void CTestPhone::TestNetworkSecurity(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	
	// Test GetNetworkSecurityLevel
	RMobilePhone::TMobilePhoneNetworkSecurity netSecurity;

	// asynchronous
	
	aPhone.GetNetworkSecurityLevel(aReqStatus, netSecurity);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(netSecurity == DMMTSY_PHONE_NET_SECURITY);

	// asynchronous & cancel

	aPhone.GetNetworkSecurityLevel(aReqStatus, netSecurity);
	aPhone.CancelAsyncRequest(EMobilePhoneGetNetworkSecurityLevel);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetNetworkSecurityLevel (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(netSecurity == DMMTSY_PHONE_NET_SECURITY);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetNetworkSecurityLevel (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test NotifyNetworkSecurityLevelChange
	
	// asynchronous
	
	aPhone.NotifyNetworkSecurityLevelChange(aReqStatus, netSecurity);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(netSecurity == DMMTSY_PHONE_NET_SECURITY);

	// asynchronous & cancel

	aPhone.NotifyNetworkSecurityLevelChange(aReqStatus, netSecurity);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSecurityLevelChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNetworkSecurityLevelChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(netSecurity == DMMTSY_PHONE_NET_SECURITY);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyNetworkSecurityLevelChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
		
	//Test GetCipheringIndicatorStatus
	RMobilePhone::TMobileCallCipheringIndicator indicator;
	
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCipheringIndicatorStatus async."), iTestCount++);
	
	aPhone.GetCipheringIndicatorStatus(aReqStatus, indicator);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(indicator == DMMTSY_PHONE_NET_CIPHER_IND);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCipheringIndicatorStatus async & cancel."), iTestCount++);
	
	aPhone.GetCipheringIndicatorStatus(aReqStatus, indicator);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCipheringIndicatorStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	
	//Test NotifyCipheringIndicatorStatus
	indicator = RMobilePhone::ECipheringDisplayOff;	//reset
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCipheringIndicatorStatus async."), iTestCount++);
	
	aPhone.NotifyCipheringIndicatorStatus(aReqStatus, indicator);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(indicator == DMMTSY_PHONE_NET_CIPHER_IND);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyCipheringIndicatorStatus async & cancel."), iTestCount++);
	
	aPhone.NotifyCipheringIndicatorStatus(aReqStatus, indicator);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyCipheringIndicatorStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	}

void CTestPhone::TestUSimHandling(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{

	//Test EnumerateUSimApplications
	RMobilePhone::TAID activeAID;
	TInt count;

	// asynchronous
	
	aPhone.EnumerateUSimApplications(aReqStatus, count, activeAID);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(activeAID.Compare(DMMTSY_PHONE_USIM_AID) == 0);
	TEST(count == DMMTSY_PHONE_USIM_COUNT);

	// asynchronous & cancel

	aPhone.EnumerateUSimApplications(aReqStatus, count, activeAID);
	aPhone.CancelAsyncRequest(EMobilePhoneEnumerateUSimApplications);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::EnumerateUSimApplications (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobilePhone::EnumerateUSimApplications (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	//Test GetUSimApplicationInfo
	RMobilePhone::TUSimApplicationInfoV2 usimAppInfo;
	RMobilePhone::TUSimApplicationInfoV2Pckg usimAppInfoPckg(usimAppInfo);

	// asynchronous
	
	aPhone.GetUSimApplicationInfo(aReqStatus, DMMTSY_PHONE_USIM_APPINDEX, usimAppInfoPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(usimAppInfo.iAID.Compare(DMMTSY_PHONE_USIM_AID) == 0);
	TEST(usimAppInfo.iLabel.Compare(DMMTSY_PHONE_USIM_APPLABEL) == 0);

	// asynchronous & cancel

	aPhone.GetUSimApplicationInfo(aReqStatus, DMMTSY_PHONE_USIM_APPINDEX, usimAppInfoPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetUSimApplicationsInfo);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetUSimApplicationInfo (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		//ValidateUSimAppinfo(usimAppInfo);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::GetUSimApplicationInfo (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	//Test NotifyUSimApplicationsInfoChange

	// asynchronous
	
	aPhone.NotifyUSimApplicationsInfoChange(aReqStatus, count, activeAID);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(activeAID.Compare(DMMTSY_PHONE_USIM_AID) == 0);
	TEST(count == DMMTSY_PHONE_USIM_COUNT);
	
	// asynchronous & cancel

	aPhone.NotifyUSimApplicationsInfoChange(aReqStatus, count, activeAID);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyUSimApplicationsInfoChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyUSimApplicationsInfoChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(activeAID.Compare(DMMTSY_PHONE_USIM_AID) == 0);
		TEST(count == DMMTSY_PHONE_USIM_COUNT);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyUSimApplicationsInfoChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	//Test SetUSimApplicationStatus

	// asynchronous
	
	aPhone.SetUSimApplicationStatus(aReqStatus, DMMTSY_PHONE_USIM_AID, DMMTSY_PHONE_USIM_ACTIVATE);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// asynchronous & cancel

	aPhone.SetUSimApplicationStatus(aReqStatus, DMMTSY_PHONE_USIM_AID, DMMTSY_PHONE_USIM_ACTIVATE);
	aPhone.CancelAsyncRequest(EMobilePhoneSetUSimApplicationStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetUSimApplicationStatus (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhone::SetUSimApplicationStatus (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		
	//Test GetUSimAppsSelectionMode
	RMobilePhone::TUSimSelectionMode selectionMode;

	//synchronous
	aPhone.GetUSimAppsSelectionMode(selectionMode);
	TEST(selectionMode == DMMTSY_PHONE_USIM_SELMODE);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetUSimAppsSelectionMode (sync) passed"), iTestCount++);

	//Test SetUSimAppsSelectionMode
	
	//synchronous
	
	aPhone.SetUSimAppsSelectionMode(DMMTSY_PHONE_USIM_SELMODE);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetUSimAppsSelectionMode (sync) passed"), iTestCount++);

	//Test NotifyUSimAppsSelectionModeChange

	// asynchronous
	
	aPhone.NotifyUSimAppsSelectionModeChange(aReqStatus, selectionMode);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(selectionMode == DMMTSY_PHONE_USIM_SELMODE);
	
	// asynchronous & cancel

	aPhone.NotifyUSimAppsSelectionModeChange(aReqStatus, selectionMode);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyUSimAppsSelectionModeChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyUSimAppsSelectionModeChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(selectionMode == DMMTSY_PHONE_USIM_SELMODE);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyUSimAppsSelectionModeChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
		
	//Test GetCurrentActiveUSimApplication
	RMobilePhone::TAID aid;
	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCurrentActiveUSimApplication"), iTestCount++);
	aPhone.GetCurrentActiveUSimApplication(aReqStatus, aid);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(aid == DMMTSY_PHONE_SC_AID);

	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetCurrentActiveUSimApplication Cancel"), iTestCount++);
	aPhone.GetCurrentActiveUSimApplication(aReqStatus, aid);
	aPhone.CancelAsyncRequest(EMobilePhoneGetCurrentActiveUSimApplication);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	

	}
	
void CTestPhone::TestSmartCardHandling(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//Test EnumerateSmartCardApplications
	TUint count;

	// asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::EnumerateSmartCardApplications"), iTestCount++);
	aPhone.EnumerateSmartCardApplications(aReqStatus, count);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(count == DMMTSY_PHONE_SC_COUNT);

	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::EnumerateSmartCardApplications Cancel"), iTestCount++);
	aPhone.EnumerateSmartCardApplications(aReqStatus, count);
	aPhone.CancelAsyncRequest(EMobilePhoneEnumerateSmartCardApplications);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	
	//Test GetSmartCardApplicationInfo
	RMobilePhone::TSmartCardApplicationInfoV5 scAppInfo;
	RMobilePhone::TSmartCardApplicationInfoV5Pckg scAppInfoPckg(scAppInfo);
	
	//asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetSmartCardApplicationInfo"), iTestCount++);
	aPhone.GetSmartCardApplicationInfo(aReqStatus, DMMTSY_PHONE_SC_APP_INDEX1, scAppInfoPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(scAppInfo.iAID == DMMTSY_PHONE_SC_AID);
	TEST(scAppInfo.iLabel == DMMTSY_PHONE_SC_LABEL);
	
	aPhone.GetSmartCardApplicationInfo(aReqStatus, DMMTSY_PHONE_SC_APP_INDEX2, scAppInfoPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == DMMTSY_PHONE_SC_OUTOFRANGE);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetSmartCardApplicationInfo Cancel"), iTestCount++);
	aPhone.GetSmartCardApplicationInfo(aReqStatus, DMMTSY_PHONE_SC_APP_INDEX1, scAppInfoPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetSmartCardApplicationInfo);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);

	//Test NotifySmartCardApplicationInfoChange
	RMobilePhone::TSmartCardApplicationEvent event;
	RMobilePhone::TAID aid;
	
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySmartCardApplicationInfoChange"), iTestCount++);
	aPhone.NotifySmartCardApplicationInfoChange(aReqStatus, aid, event);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(event == DMMTSY_PHONE_SC_EVENT_ACTIVATE);
	TEST(aid == DMMTSY_PHONE_SC_AID);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifySmartCardApplicationInfoChange Cancel"), iTestCount++);
	aPhone.NotifySmartCardApplicationInfoChange(aReqStatus, aid, event);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifySmartCardApplicationInfoChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	
	//Test SetSmartCardApplicationStatus
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetSmartCardApplicationStatus"), iTestCount++);
	aid = DMMTSY_PHONE_SC_AID2;
	RMobilePhone::TSmartCardApplicationAction action = DMMTSY_PHONE_SC_ACTION_TERMINATE;
	aPhone.SetSmartCardApplicationStatus(aReqStatus, aid, action);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetSmartCardApplicationStatus Cancel"), iTestCount++);
	aPhone.SetSmartCardApplicationStatus(aReqStatus, aid, action);
	aPhone.CancelAsyncRequest(EMobilePhoneSetSmartCardApplicationStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	
	//Test GetScFileInfo
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetScFileInfo"), iTestCount++);
	RMobilePhone::TScFilePath filePath;
	RMobilePhone::TScFileInfoV5 scFileInfo;
	RMobilePhone::TScFileInfoV5Pckg scFileInfoPckg(scFileInfo);

	filePath.iAID = DMMTSY_PHONE_SC_AID;
	filePath.iPath = DMMTSY_PHONE_SC_PATH_REF;
	aPhone.GetScFileInfo(aReqStatus, filePath, scFileInfoPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(scFileInfo.iFileID == DMMTSY_PHONE_SC_FILEID);
	TEST(scFileInfo.iNumberRecords == DMMTSY_PHONE_SC_NORECORDS);
	TEST(scFileInfo.iRecordLength == DMMTSY_PHONE_SC_RECORD_SIZE);
	TEST(scFileInfo.iType == DMMTSY_PHONE_SC_TYPE);
	TEST(scFileInfo.iTotalLength == DMMTSY_PHONE_SC_FILESIZE);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetScFileInfo Cancel"), iTestCount++);
	aPhone.GetScFileInfo(aReqStatus, filePath, scFileInfoPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetScFileInfo);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	
	//Test ReadScFile
	INFO_PRINTF2(_L("Test %d - RMobilePhone::ReadScFile"), iTestCount++);
	RMobilePhone::TScFilePathWithAccessOffsets offsets;
	TBuf8<DMMTSY_PHONE_SC_FILESIZE> buffer;
	
	offsets.iAID = DMMTSY_PHONE_SC_AID;
	offsets.iPath = DMMTSY_PHONE_SC_PATH_REF;
	offsets.iType = DMMTSY_PHONE_SC_TYPE;
	offsets.iCount = DMMTSY_PHONE_SC_NORECORDS;
	offsets.iStartOffset = DMMTSY_PHONE_SC_START_OFFSET;
	aPhone.ReadScFile(aReqStatus, offsets, buffer);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	//extract record info and test its OK.
	if(aReqStatus.Int() == KErrNone)
		{
		TInt startoffset = 0;
		TPtr8 record0(buffer.MidTPtr(startoffset,DMMTSY_PHONE_SC_RECORD_SIZE));
		startoffset += DMMTSY_PHONE_SC_RECORD_SIZE;
		TPtr8 record1(buffer.MidTPtr(startoffset,DMMTSY_PHONE_SC_RECORD_SIZE));
		startoffset += DMMTSY_PHONE_SC_RECORD_SIZE;
		TPtr8 record2(buffer.MidTPtr(startoffset,DMMTSY_PHONE_SC_RECORD_SIZE));
		
		TEST(record0 == DMMTSY_PHONE_SC_RECORD0); 
		TEST(record1 == DMMTSY_PHONE_SC_RECORD1);
		TEST(record2 == DMMTSY_PHONE_SC_RECORD2);
		}
		
	buffer = DMMTSY_PHONE_SC_RECORD0;
	buffer.Append(DMMTSY_PHONE_SC_RECORD1);
	buffer.Append(DMMTSY_PHONE_SC_RECORD2);

	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::ReadScFile Cancel"), iTestCount++);
	aPhone.ReadScFile(aReqStatus, offsets, buffer);
	aPhone.CancelAsyncRequest(EMobilePhoneReadScFile);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	
	//Test UpdateScFile
	INFO_PRINTF2(_L("Test %d - RMobilePhone::UpdateScFile"), iTestCount++);
	aPhone.UpdateScFile(aReqStatus, offsets, buffer);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	// asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::UpdateScFile Cancel"), iTestCount++);
	aPhone.UpdateScFile(aReqStatus, offsets, buffer);
	aPhone.CancelAsyncRequest(EMobilePhoneUpdateScFile);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	
	}
	
void CTestPhone::TestMobileMailboxNumbersFunctionality(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//Test	GetMailboxNumbers
	
	RMobilePhone::TMobilePhoneVoicemailIdsV3 voicemailIdsV3;
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg voicemailIdsV3Pckg(voicemailIdsV3);
	
	//asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetMailboxNumbers async."), iTestCount++);
	
	aPhone.GetMailboxNumbers(aReqStatus,voicemailIdsV3Pckg);
	User::WaitForRequest(aReqStatus);	
	TEST(aReqStatus.Int() == KErrNone);
	TEST(voicemailIdsV3.iVoice == DMMTSY_PHONE_MAILBOXNUMBER_VOICE);
	TEST(voicemailIdsV3.iData == DMMTSY_PHONE_MAILBOXNUMBER_DATA);
	TEST(voicemailIdsV3.iFax == DMMTSY_PHONE_MAILBOXNUMBER_FAX);
	TEST(voicemailIdsV3.iOther == DMMTSY_PHONE_MAILBOXNUMBER_OTHER);
	
	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetMailboxNumbers async & cancel."), iTestCount++);
	
	aPhone.GetMailboxNumbers(aReqStatus,voicemailIdsV3Pckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetMailboxNumbers);
	User::WaitForRequest(aReqStatus);		
	TEST(aReqStatus.Int() == KErrCancel);
		
//Test NotifyMailBoxNumbersChange 	

	//asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMailboxNumbersChange async."), iTestCount++);
	
	aPhone.NotifyMailboxNumbersChange(aReqStatus,voicemailIdsV3Pckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(voicemailIdsV3.iVoice == DMMTSY_PHONE_MAILBOXNUMBER_VOICE);
	TEST(voicemailIdsV3.iData == DMMTSY_PHONE_MAILBOXNUMBER_DATA);
	TEST(voicemailIdsV3.iFax == DMMTSY_PHONE_MAILBOXNUMBER_FAX);
	TEST(voicemailIdsV3.iOther == DMMTSY_PHONE_MAILBOXNUMBER_OTHER);

	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMailboxNumbersChange async & cancel."), iTestCount++);
	
	aPhone.NotifyMailboxNumbersChange(aReqStatus,voicemailIdsV3Pckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyMailboxNumbersChange);
	User::WaitForRequest(aReqStatus);	
	TEST(aReqStatus.Int() == KErrCancel);
	
	//Test with TMobilePhoneVoicemailIdsV8 class
	RMobilePhone::TMobilePhoneVoicemailIdsV8 voicemailIdsV8;
	RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg voicemailIdsV8Pckg(voicemailIdsV8);
	
	//asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetMailboxNumbers async."), iTestCount++);
	
	aPhone.GetMailboxNumbers(aReqStatus,voicemailIdsV8Pckg);
	User::WaitForRequest(aReqStatus);	
	TEST(aReqStatus.Int() == KErrNone);
	
	TEST(voicemailIdsV8.iVideo == DMMTSY_PHONE_MAILBOXNUMBER_VIDEO);

	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetMailboxNumbers async & cancel."), iTestCount++);
	
	aPhone.GetMailboxNumbers(aReqStatus,voicemailIdsV8Pckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetMailboxNumbers);
	User::WaitForRequest(aReqStatus);		
	TEST(aReqStatus.Int() == KErrCancel);
		
//Test NotifyMailBoxNumbersChange 	

	//asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMailboxNumbersChange async."), iTestCount++);
	
	aPhone.NotifyMailboxNumbersChange(aReqStatus,voicemailIdsV8Pckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	TEST(voicemailIdsV8.iVideo == DMMTSY_PHONE_MAILBOXNUMBER_VIDEO);

	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMailboxNumbersChange async & cancel."), iTestCount++);
	
	aPhone.NotifyMailboxNumbersChange(aReqStatus,voicemailIdsV8Pckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyMailboxNumbersChange);
	User::WaitForRequest(aReqStatus);	
	TEST(aReqStatus.Int() == KErrCancel);
	}

void CTestPhone::TestMobileMmsFunctionality(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//Testing RMobilePhone::NotifyMmsUpdate
	RMobilePhone::TMmsNotificationV3 mmsNotificationV3;
	RMobilePhone::TMmsNotificationV3Pckg mmsNotificationV3Pckg (mmsNotificationV3);
	
	//asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMmsUpdate async"), iTestCount++);
	aPhone.NotifyMmsUpdate(aReqStatus, mmsNotificationV3Pckg);
	User::WaitForRequest(aReqStatus); 
	TEST(aReqStatus.Int() == KErrNone);
	TEST(mmsNotificationV3.iSpace == DMMTSY_PHONE_MMS_UPDATE_SPACE);
	TEST(mmsNotificationV3.iNotification == DMMTSY_PHONE_MMS_UPDATE_NOTIFICATION);
	TEST(mmsNotificationV3.iMmsStatus == DMMTSY_PHONE_MMS_UPDATE_STATUS);
	TEST(mmsNotificationV3.iImplementation == DMMTSY_PHONE_MMS_UPDATE_IMPLEMENTATION);
	TEST(mmsNotificationV3.iNotify.Compare(DMMTSY_PHONE_MMS_UPDATE_NOTIFY) == 0);

	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMmsUpdate async & cancel"), iTestCount++);
	aPhone.NotifyMmsUpdate(aReqStatus, mmsNotificationV3Pckg);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyMmsUpdate);
	User::WaitForRequest(aReqStatus); 
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);
	

	//Testing RMobilePhone::SetMmsUserConnParams 
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetMmsUserConnParams async"), iTestCount++);
	TBuf8<30> connectivity(DMMTSY_PHONE_MMS_USER_CONN_PARAMS);
	aPhone.SetMmsUserConnParams(aReqStatus, connectivity); 
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetMmsUserConnParams async & cancel"), iTestCount++);
	aPhone.SetMmsUserConnParams(aReqStatus, connectivity); 
	aPhone.CancelAsyncRequest(EMobilePhoneSetMmsUserConnParams);
	User::WaitForRequest(aReqStatus); 
	TEST(aReqStatus.Int() == KErrCancel);

	//Testing RMobilePhone::SetMmsUserPreferences 
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetMmsUserPreferences async"), iTestCount++);
	TBuf8<30> preferences(DMMTSY_PHONE_MMS_USER_PREF_PARAMS);
	aPhone.SetMmsUserPreferences(aReqStatus, preferences);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetMmsUserPreferences async & cancel"), iTestCount++);
	aPhone.SetMmsUserPreferences(aReqStatus, preferences); 
	aPhone.CancelAsyncRequest(EMobilePhoneSetMmsUserPreferences);
	User::WaitForRequest(aReqStatus); 
	TEST(aReqStatus.Int() == KErrCancel);
	
	//Testing RMobilePhone::GetMmsConfig	
	
	//asynchronous	
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetMmsConfig async"), iTestCount++);
	RMobilePhone::TMmsConnParams connectionType = DMMTSY_PHONE_MMS_USER_CONN_PARAMS_TYPE;
	aPhone.GetMmsConfig(aReqStatus, connectionType, connectivity);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);	
	TEST(connectivity.Compare(DMMTSY_PHONE_MMS_USER_CONN_PARAMS) == 0);
	
	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetMmsUserPreferences async & cancel"), iTestCount++);
	aPhone.GetMmsConfig(aReqStatus, connectionType, connectivity);
	aPhone.CancelAsyncRequest(EMobilePhoneGetMmsConfig);
	User::WaitForRequest(aReqStatus); 
	TEST(aReqStatus.Int() == KErrCancel);

	//Testing RMobilePhone::NotifyMmsConfig
	
	//asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMmsConfig async"), iTestCount++);
	aPhone.NotifyMmsConfig(aReqStatus, connectionType, connectivity);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);	
	TEST(connectivity.Compare(DMMTSY_PHONE_MMS_USER_CONN_PARAMS) == 0);
	
	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyMmsConfig async & cancel"), iTestCount++);
	aPhone.NotifyMmsConfig(aReqStatus, connectionType, connectivity);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyMmsConfig);
	User::WaitForRequest(aReqStatus); 
	TEST(aReqStatus.Int() == KErrCancel);	
	}
	
void CTestPhone::TestMobileAirTime(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	TTimeIntervalSeconds aTime;
	
	//Test synchronous GetAirTimeDuration
	
	TEST(KErrNone==aPhone.GetAirTimeDuration(aTime));
	TEST(aTime.Int()==DMMTSY_AIR_TIME_DURATION1);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetAirTimeDuration passed"), iTestCount++);
	
	//Test asynchronous NotifyAirTimeDurationChange
	
	aPhone.NotifyAirTimeDurationChange(aReqStatus,aTime);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	TEST(aTime.Int()==DMMTSY_AIR_TIME_DURATION2);
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAirTimeDurationChange passed"), iTestCount++);
	
	//Test asynchronous & cancel
	
	aPhone.NotifyAirTimeDurationChange(aReqStatus,aTime);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyAirTimeDurationChange);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone || aReqStatus.Int() == KErrCancel);

	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAirTimeDurationChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(aTime.Int()==DMMTSY_AIR_TIME_DURATION2);
		INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAirTimeDurationChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	
	}
	
void CTestPhone::TestMobileAutoRedial(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//Test Blacklist clearing
	
	//asynchronous 
	
	aPhone.ClearBlacklist(aReqStatus);	
	User::WaitForRequest(aReqStatus);
	if(aReqStatus.Int()==KErrNone)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::ClearBlacklist passed"), iTestCount++);
	else
		TEST(aReqStatus.Int()==KErrNone);
	
	//asynchronous & cancel
	
	aPhone.ClearBlacklist(aReqStatus);
	aPhone.CancelAsyncRequest(EMobilePhoneClearBlacklist);
	User::WaitForRequest(aReqStatus);
	if(aReqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhone::ClearBlacklist (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		TEST(aReqStatus.Int()==KErrNone);	
	}
	
void CTestPhone::TestMobilePersonalisation(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//Tests APIs from the MobilePersonalisation functional unit
	
	
	//Test GetPersonalisationCaps
	
	//synchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetPersonalisationCaps async"), iTestCount++);
	TUint32 aScheme;
	TInt ret = aPhone.GetPersonalisationCaps(aScheme);	
	TEST(ret==KErrNone);
	TEST(aScheme == (DMMTSY_PHONE_PERS_CAPS_SERV_PROV | DMMTSY_PHONE_PERS_CAPS_NETWORK));
		
	//Test GetPersonalisationStatus
	
	//asynchronous 
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetPersonalisationStatus async"), iTestCount++);
	aPhone.GetPersonalisationStatus(aReqStatus, aScheme);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrNone);
	TEST(aScheme == (DMMTSY_PHONE_PERS_GET_NETWORK_SUB | DMMTSY_PHONE_PERS_GET_CORPORATE));
	
	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetPersonalisationStatus async & cancel"), iTestCount++);
	aPhone.GetPersonalisationStatus(aReqStatus, aScheme);
	aPhone.CancelAsyncRequest(EMobilePhoneGetPersonalisationStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrCancel);	
	
	//Test SetPersonalisationStatus
	
	//asynchronous 
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetPersonalisationStatus async"), iTestCount++);
	
	TBuf<10> persCode = DMMTSY_PHONE_PERS_SET_CODE;
	aPhone.SetPersonalisationStatus(aReqStatus, DMMTSY_PHONE_PERS_SET_USIM, persCode);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrNone);
		
	//asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetPersonalisationStatus async & cancel"), iTestCount++);
	aPhone.SetPersonalisationStatus(aReqStatus, DMMTSY_PHONE_PERS_SET_USIM, persCode);
	aPhone.CancelAsyncRequest(EMobilePhoneSetPersonalisationStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrCancel);		
	}

	
void CTestPhone::TestMobileAPNControlList(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
/**
Tests ETelmm API APN control list functions.

@param aPhone RMobilePhone object used to call the tested API functions.
@param aReqStatus Stores the request status returned by asynchronous functions.
*/
	{
	// Testing EnumerateAPNEntries
	// Asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::EnumerateAPNEntries async"), iTestCount++);
	TUint32 count;
	aPhone.EnumerateAPNEntries(aReqStatus, count);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);	
	TEST(count == DMMTSY_PHONE_APN_ENUMERATE);

	// Asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::EnumerateAPNEntries async & cancel"), iTestCount++);
	aPhone.EnumerateAPNEntries(aReqStatus, count);
	aPhone.CancelAsyncRequest(EMobilePhoneEnumerateAPNEntries);
	User::WaitForRequest(aReqStatus); 
	TEST(aReqStatus.Int() == KErrCancel); 
	
		
	// Testing GetAPNname
	RMobilePhone::TAPNEntryV3 entry;
	RMobilePhone::TAPNEntryV3Pckg entryPckg(entry);
	
	// Asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetAPNname async"), iTestCount++);
	
	aPhone.GetAPNname(aReqStatus, DMMTSY_PHONE_APN_ENUMERATE-1, entryPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);	
	TEST(entry.iApn.Compare(DMMTSY_PHONE_APN_ENTRY3) == 0);
	
	// Asynchronous & cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetAPNname async & cancel"), iTestCount++);
	aPhone.GetAPNname(aReqStatus, DMMTSY_PHONE_APN_ENUMERATE-1, entryPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneGetAPNname);
	User::WaitForRequest(aReqStatus); 
	TEST(aReqStatus.Int() == KErrCancel); 

	
	// Testing AppendAPNName
	RMobilePhone::TAPNEntryV3 entryToAppend;
	RMobilePhone::TAPNEntryV3Pckg entryToAppendPckg(entryToAppend);
	entryToAppend.iApn = DMMTSY_PHONE_APN_ENTRY_TO_APPEND;

	// Asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::AppendAPNName async"), iTestCount++);
	aPhone.AppendAPNName(aReqStatus, entryToAppendPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// Asynchronous and cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::AppendAPNName async and cancel"), iTestCount++);
	aPhone.AppendAPNName(aReqStatus, entryToAppendPckg);
	aPhone.CancelAsyncRequest(EMobilePhoneAppendAPNName);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);

	
	// Testing DeleteAPNName
	// Asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::DeleteAPNName async"), iTestCount++);
	aPhone.DeleteAPNName(aReqStatus, DMMTSY_PHONE_APN_ENUMERATE-1);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// Asynchronous and cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::DeleteAPNName async and cancel"), iTestCount++);
	aPhone.DeleteAPNName(aReqStatus, DMMTSY_PHONE_APN_ENUMERATE-1);
	aPhone.CancelAsyncRequest(EMobilePhoneDeleteAPNName);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	
	// Testing NotifyAPNListChanged
	TRequestStatus notifyReqStatus;
	
	// Asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAPNListChanged async"), iTestCount++);
	aPhone.NotifyAPNListChanged(notifyReqStatus);
	aPhone.AppendAPNName(aReqStatus, entryToAppendPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	User::WaitForRequest(notifyReqStatus);
	TEST(notifyReqStatus.Int() == KErrNone);
	
	// Asynchronous and cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAPNListChanged async and cancel"), iTestCount++);
	aPhone.NotifyAPNListChanged(aReqStatus);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyAPNListChanged);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	}

void CTestPhone::TestMobileAPNControlListServiceStatus(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
/**
Tests ETelmm API APN control list service status functions.
Tests functionality added for INC086069.

@param aPhone RMobilePhone object used to call the tested API functions.
@param aReqStatus Stores the request status returned by asynchronous functions.
*/
	{
	// Testing SetAPNControlListServiceStatus
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;
	apnControlListServiceStatus = DMMTSY_PHONE_APN_CONTROL_LIST_SERVICE_ENABLED;

	// Asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetAPNControlListServiceStatus async"), iTestCount++);
	aPhone.SetAPNControlListServiceStatus(aReqStatus, apnControlListServiceStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	// Asynchronous and cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::SetAPNControlListServiceStatus async and cancel"), iTestCount++);
	aPhone.SetAPNControlListServiceStatus(aReqStatus, apnControlListServiceStatus);
	aPhone.CancelAsyncRequest(EMobilePhoneSetAPNControlListServiceStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);

	
	// Testing GetAPNControlListServiceStatus
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatusToGet;

	// Asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetAPNControlListServiceStatus async"), iTestCount++);
	aPhone.GetAPNControlListServiceStatus(aReqStatus, apnControlListServiceStatusToGet);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(apnControlListServiceStatusToGet == DMMTSY_PHONE_APN_CONTROL_LIST_SERVICE_DISABLED);

	// Asynchronous and cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::GetAPNControlListServiceStatus async and cancel"), iTestCount++);
	aPhone.GetAPNControlListServiceStatus(aReqStatus, apnControlListServiceStatusToGet);
	aPhone.CancelAsyncRequest(EMobilePhoneGetAPNControlListServiceStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);

	
	// Testing NotifyAPNControlListServiceStatusChange
	RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatusNotify;
	TRequestStatus notifyReqStatus;

	// Asynchronous
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAPNControlListServiceStatusChange async"), iTestCount++);
	aPhone.NotifyAPNControlListServiceStatusChange(notifyReqStatus, apnControlListServiceStatusNotify);
	aPhone.SetAPNControlListServiceStatus(aReqStatus, apnControlListServiceStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	User::WaitForRequest(notifyReqStatus);
	TEST(notifyReqStatus.Int() == KErrNone);
	TEST(apnControlListServiceStatusNotify == DMMTSY_PHONE_APN_CONTROL_LIST_SERVICE_ENABLED);

	// Asynchronous and cancel
	INFO_PRINTF2(_L("Test %d - RMobilePhone::NotifyAPNControlListServiceStatusChange async and cancel"), iTestCount++);
	aPhone.NotifyAPNControlListServiceStatusChange(notifyReqStatus, apnControlListServiceStatusNotify);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyAPNControlListServiceStatusChange);
	User::WaitForRequest(notifyReqStatus);
	TEST(notifyReqStatus == KErrCancel);	
	}
	
void CTestPhone::TestGBAAuthenticationL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{

	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test GBA Functionality"));

	// local variables used throughout the tests
	
	//Verify the support of GBA in USIM Service Table
	RMobilePhone::TMobilePhoneServiceTableV8 asyncSstV8File;
	RMobilePhone::TMobilePhoneServiceTableV8Pckg asyncSstV8FilePckgd(asyncSstV8File);
	RMobilePhone::TMobilePhoneServiceTable serviceTableType=DMMTSY_PHONE_SERVICE_TABLE;
	
	aPhone.GetServiceTable(aReqStatus, serviceTableType, asyncSstV8FilePckgd);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	if(aReqStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for GBA passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for GBA failed"));
		}

	// extract the returned SST file from the package
	asyncSstV8File=asyncSstV8FilePckgd();

	TEST(asyncSstV8File.ExtensionId()==KEtelExtMultimodeV8);
	TEST((asyncSstV8File.iServices57To64) == DMMTSY_PHONE_SST_BYTE8);
	TEST((asyncSstV8File.iServices65To72) == DMMTSY_PHONE_SST_BYTE9);
	
	// asynchronous & cancel
	aPhone.GetServiceTable(aReqStatus, serviceTableType, asyncSstV8FilePckgd);
	aPhone.CancelAsyncRequest(EMobilePhoneGetServiceTable);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);

	if (aReqStatus.Int()==KErrCancel)
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test  - RMobilePhone::GetServiceTable V8table (async & cancel (Request Not Cancelled)) passed"));
		}

	RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8;
	RMobilePhone::TGbaBootstrapAuthDataV8Pckg tGbaBootstrapAuthDataV8Pckgd(tGbaBootstrapAuthDataV8);
	TEST(tGbaBootstrapAuthDataV8.ExtensionId() == RMobilePhone::KETelMobileGbaBootstrapAuthDataV8);
	
	tGbaBootstrapAuthDataV8.iRAND = DMMTSY_PHONE_GBA_RAND_IN; // valid input
	tGbaBootstrapAuthDataV8.iAUTN = DMMTSY_PHONE_GBA_AUTN_IN; // valid input
	
	RMobilePhone::TAID appAID = DMMTSY_PHONE_GBA_AID();
	
	INFO_PRINTF1(_L("Test - RMobilePhone::SmartCardAuthenticate async with AID."));
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tGbaBootstrapAuthDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	 
	TEST(tGbaBootstrapAuthDataV8.iRES == DMMTSY_PHONE_GBA_SUCC_RES_OUT);
	TEST(tGbaBootstrapAuthDataV8.iAUTS == DMMTSY_PHONE_GBA_SUCC_AUTS_OUT);
	
	tGbaBootstrapAuthDataV8.iRAND = DMMTSY_PHONE_GBA_RAND_INV; // invalid input
	tGbaBootstrapAuthDataV8.iAUTN = DMMTSY_PHONE_GBA_AUTN_INV; // invalid input
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tGbaBootstrapAuthDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrMMEtelAuthVerificationFailure);
	
	TEST(tGbaBootstrapAuthDataV8.iRES == DMMTSY_PHONE_GBA_FAIL_RES_OUT);
	TEST(tGbaBootstrapAuthDataV8.iAUTS == DMMTSY_PHONE_GBA_FAIL_AUTS_OUT);
	
	tGbaBootstrapAuthDataV8.iRAND = DMMTSY_PHONE_GBA_RAND_INV; // invalid input
	tGbaBootstrapAuthDataV8.iAUTN = DMMTSY_PHONE_GBA_AUTN_INV; // invalid input
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tGbaBootstrapAuthDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrMMEtelAuthMacInvalid);
	
	TEST(tGbaBootstrapAuthDataV8.iRES == DMMTSY_PHONE_GBA_FAIL_RES_OUT);
	TEST(tGbaBootstrapAuthDataV8.iAUTS == DMMTSY_PHONE_GBA_SUCC_AUTS_OUT );
	
	RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8_1;
	RMobilePhone::TGbaBootstrapAuthDataV8Pckg tGbaBootstrapAuthDataV8Pckgd_1(tGbaBootstrapAuthDataV8_1);
	
	tGbaBootstrapAuthDataV8_1.iRAND = DMMTSY_PHONE_GBA_RAND_INV; // invalid input
	tGbaBootstrapAuthDataV8_1.iAUTN = DMMTSY_PHONE_GBA_AUTN_INV; // invalid input
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tGbaBootstrapAuthDataV8Pckgd_1);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrInUse);
	
	TEST(tGbaBootstrapAuthDataV8.iRES == DMMTSY_PHONE_GBA_FAIL_RES_OUT);
	TEST(tGbaBootstrapAuthDataV8.iAUTS == DMMTSY_PHONE_GBA_SUCC_AUTS_OUT );	
		
	RMobilePhone::TBsfDataV8 tBsfDataV8;
	RMobilePhone::TBsfDataV8Pckg tBsfDataV8Pckgd(tBsfDataV8);
	TEST(tBsfDataV8.ExtensionId() == RMobilePhone::KETelMobileGbaBootstrapAuthDataV8);
		
	tBsfDataV8.iBtid = DMMTSY_PHONE_BSF_VALID_BTID;
	tBsfDataV8.iKeyLifetime = DMMTSY_PHONE_BSF_VALID_KEYLIFETIME;
	
	aPhone.SetGbaBootstrapParams(aReqStatus,appAID,tBsfDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	INFO_PRINTF1(_L("Test - RMobilePhone::SetGbaBootstrapParams Completed."));
	
	tBsfDataV8.iBtid = DMMTSY_PHONE_BSF_INVALID_BTID;
	tBsfDataV8.iKeyLifetime = DMMTSY_PHONE_BSF_INVALID_KEYLIFETIME;
	
	aPhone.SetGbaBootstrapParams(aReqStatus,appAID,tBsfDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCorrupt);
		
	tBsfDataV8.iBtid = DMMTSY_PHONE_BSF_VALID_BTID;
	tBsfDataV8.iKeyLifetime = DMMTSY_PHONE_BSF_VALID_KEYLIFETIME;
	
	aPhone.SetGbaBootstrapParams(aReqStatus,appAID,tBsfDataV8Pckgd);
	aPhone.CancelAsyncRequest(EMobilePhoneSetGbaBootstrapParams);
	User::WaitForRequest(aReqStatus);
	TEST((aReqStatus.Int() == KErrCancel) ||(aReqStatus.Int() == KErrNone));
	INFO_PRINTF1(_L("Test - Cancellation of RMobilePhone::SetGbaBootstrapParams Completed."));
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckgd(tAuthInfoV8);
	aPhone.NotifyAuthenticateDataChange(aReqStatus,tAuthInfoV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	INFO_PRINTF1(_L("Test - RMobilePhone::NotifyAuthenticateDataChange Completed."));
	
	tAuthInfoV8 = tAuthInfoV8Pckgd();
	TEST(tAuthInfoV8.iAID == DMMTSY_PHONE_GBA_AID());
	TEST((tAuthInfoV8.iData == DMMTSY_PHONE_AUTH_DATA_GBA_PARAMS ) ||(tAuthInfoV8.iData == DMMTSY_PHONE_AUTH_DATA_NAF_LIST) 
			|| (tAuthInfoV8.iData == DMMTSY_PHONE_AUTH_DATA_MBMS_MSK) ||(tAuthInfoV8.iData == DMMTSY_PHONE_AUTH_DATA_MBMS_MUK) );
	
	aPhone.NotifyAuthenticateDataChange(aReqStatus,tAuthInfoV8Pckgd);
	aPhone.CancelAsyncRequest(EMobilePhoneNotifyAuthenticateDataChange);
	User::WaitForRequest(aReqStatus);
	TEST((aReqStatus.Int() == KErrNone) || (aReqStatus.Int() == KErrCancel));
	INFO_PRINTF1(_L("Test - Cancellation of RMobilePhone::NotifyAuthenticateDataChange Completed."));
	
	RMobilePhone::TGbaBootstrapParamsV8 tGbaBootstrapParamsV8;
	RMobilePhone::TGbaBootstrapParamsV8Pckg tGbaBootstrapParamsV8Pckgd(tGbaBootstrapParamsV8);
	
	TEST(tGbaBootstrapParamsV8.ExtensionId() == RMobilePhone::KETelMobileGbaBootstrapParamsV8);

	tAuthInfoV8.iAID = DMMTSY_PHONE_GBA_AID();
	tAuthInfoV8.iData = DMMTSY_PHONE_AUTH_DATA_GBA_PARAMS;
	
	aPhone.GetAuthenticationParams(aReqStatus,tAuthInfoV8Pckgd,tGbaBootstrapParamsV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	INFO_PRINTF1(_L("Test - RMobilePhone::GetAuthenticationParams Completed."));

	tGbaBootstrapParamsV8 = tGbaBootstrapParamsV8Pckgd();
	
	TEST(tGbaBootstrapParamsV8.iRand == DMMTSY_PHONE_GBA_RAND_IN);
	TEST(tGbaBootstrapParamsV8.iBtid == DMMTSY_PHONE_BSF_VALID_BTID);
	TEST(tGbaBootstrapParamsV8.iKeyLifetime == DMMTSY_PHONE_BSF_VALID_KEYLIFETIME);
	
	tAuthInfoV8.iAID = DMMTSY_PHONE_GBA_AID();
	tAuthInfoV8.iData = DMMTSY_PHONE_AUTH_DATA_NAF_LIST;
	
	aPhone.GetAuthenticationParams(aReqStatus,tAuthInfoV8Pckgd,tGbaBootstrapParamsV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCorrupt);
	
	tAuthInfoV8.iAID = DMMTSY_PHONE_GBA_AID();
	tAuthInfoV8.iData = DMMTSY_PHONE_AUTH_DATA_GBA_PARAMS;
	
	aPhone.GetAuthenticationParams(aReqStatus,tAuthInfoV8Pckgd,tGbaBootstrapParamsV8Pckgd);
	aPhone.CancelAsyncRequest(EMobilePhoneGetAuthenticationParams);
	User::WaitForRequest(aReqStatus);
	TEST((aReqStatus.Int() == KErrNone) || (aReqStatus.Int() == KErrCancel));
	INFO_PRINTF1(_L("Test - Cancellation of RMobilePhone::GetAuthenticationParams Completed."));
	
	RMobilePhone::TGbaNafAuthDataV8 tGbaNafAuthDataV8;
	RMobilePhone::TGbaNafAuthDataV8Pckg tGbaNafAuthDataV8Pckgd(tGbaNafAuthDataV8);
	
	TEST(tGbaNafAuthDataV8.ExtensionId() == RMobilePhone::KETelMobileGbaNafAuthDataV8);
	
	tGbaNafAuthDataV8.iNafId = DMMTSY_PHONE_GBA_VALID_NAFID;
	tGbaNafAuthDataV8.iImpi = DMMTSY_PHONE_GBA_VALID_IMPI;
	
	aPhone.SmartCardAuthenticate(aReqStatus,tGbaNafAuthDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	tGbaNafAuthDataV8 = tGbaNafAuthDataV8Pckgd();
	TEST(tGbaNafAuthDataV8.iKsExtNaf == DMMTSY_PHONE_GBA_KS_EXTNAF);
	
	tGbaNafAuthDataV8.iNafId = DMMTSY_PHONE_GBA_INVALID_NAFID;
	tGbaNafAuthDataV8.iImpi = DMMTSY_PHONE_GBA_INVALID_IMPI;
	
	aPhone.SmartCardAuthenticate(aReqStatus,tGbaNafAuthDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCorrupt || (aReqStatus.Int() == KErrMMEtelAuthKeyNotFound));
	
	/* Retrieve the list for the GBA NAF authentication parameters */
	
	INFO_PRINTF1(_L("Test - Retrieval of CTestRetrieveMobilePhoneAuthenticationIds..."));
	
	INFO_PRINTF1(_L("Test - Retrieval of CTestRetrieveMobilePhoneAuthenticationIds..."));
	
	CTestRetrieveMobilePhoneAuthenticationIds *pRetrieve = CTestRetrieveMobilePhoneAuthenticationIds::NewLC(aPhone,this);
	TBool startReqd = ETrue;
	
	pRetrieve->SetAuthType(CTestRetrieveMobilePhoneAuthenticationIds::EGba);
	pRetrieve->TestRetrieve(DMMTSY_PHONE_AUTH_DATA_NAF_LIST,startReqd);

	//Delete test object and start afresh
	CleanupStack::PopAndDestroy(); //pRetrieve
	
	/*  Testing the cancellation of the APIs*/
    pRetrieve = CTestRetrieveMobilePhoneAuthenticationIds::NewLC(aPhone,this);
	startReqd = ETrue;
	
	pRetrieve->SetAuthType(CTestRetrieveMobilePhoneAuthenticationIds::EGba);
	pRetrieve->TestCancel(DMMTSY_PHONE_AUTH_DATA_NAF_LIST);
	
	//Delete test object and start afresh
	CleanupStack::PopAndDestroy(); //pRetrieve	
	
	INFO_PRINTF1(_L("Test - CTestRetrieveMobilePhoneAuthenticationIds Completed."));
	}

void CTestPhone::TestMBMSAuthenticationL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test MBMS Functionality"));
	
	//Verify the support of MBMS in USIM Service Table
	RMobilePhone::TMobilePhoneServiceTableV8 asyncSstV8File;
	RMobilePhone::TMobilePhoneServiceTableV8Pckg asyncSstV8FilePckgd(asyncSstV8File);
	RMobilePhone::TMobilePhoneServiceTable serviceTableType=DMMTSY_PHONE_SERVICE_TABLE;
	
	aPhone.GetServiceTable(aReqStatus, serviceTableType, asyncSstV8FilePckgd);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	if(aReqStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for GBA passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for GBA failed"));
		}
		
	// extract the returned SST file from the package
	asyncSstV8File=asyncSstV8FilePckgd();

	TEST(asyncSstV8File.ExtensionId()==KEtelExtMultimodeV8);
	TEST((asyncSstV8File.iServices57To64) == DMMTSY_PHONE_SST_BYTE8);
	TEST((asyncSstV8File.iServices65To72) == DMMTSY_PHONE_SST_BYTE9);
	
	// asynchronous & cancel

	aPhone.GetServiceTable(aReqStatus, serviceTableType, asyncSstV8FilePckgd);
	aPhone.CancelAsyncRequest(EMobilePhoneGetServiceTable);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);

	if (aReqStatus.Int()==KErrCancel)
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test  - RMobilePhone::GetServiceTable V8table (async & cancel (Request Not Cancelled)) passed"));
		}
	
	RMobilePhone::TMbmsMskUpdateDataV8 tMbmsMskUpdateDataV8;
	RMobilePhone::TMbmsMskUpdateDataV8Pckg tTMbmsMskUpdateDataV8Pckgd(tMbmsMskUpdateDataV8);
	RMobilePhone::TAID appAID = DMMTSY_PHONE_MBMS_AID();
	
	TEST(tMbmsMskUpdateDataV8.ExtensionId() == RMobilePhone::KETelMobileMbmsMskUpdateDataV8);
	
	tMbmsMskUpdateDataV8.iInputMikey = DMMTSY_PHONE_MBMS_VALID_IMIKEY_WO_VBIT;
		
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tTMbmsMskUpdateDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	INFO_PRINTF1(_L("Test - RMobilePhone::SmartCardAuthenticate async with AID."));
	
	tMbmsMskUpdateDataV8.iInputMikey = DMMTSY_PHONE_MBMS_VALID_IMIKEY_W_VBIT;
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tTMbmsMskUpdateDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(tMbmsMskUpdateDataV8.iOutputMikey == DMMTSY_PHONE_MBMS_OMIKEY_FOR_W_VBIT);
	
	tMbmsMskUpdateDataV8.iInputMikey = DMMTSY_PHONE_MBMS_INVALID_IMIKEY;
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tTMbmsMskUpdateDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() ==KErrMMEtelAuthKeyNotFound );
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tTMbmsMskUpdateDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() ==KErrMMEtelAuthKeyInvalid );
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tTMbmsMskUpdateDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCorrupt );
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tTMbmsMskUpdateDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrInUse );
		
	RMobilePhone::TMbmsMtkGenerateDataV8 tMbmsMtkGenerateDataV8;
	RMobilePhone::TMbmsMtkGenerateDataV8Pckg tMbmsMtkGenerateDataV8Pckgd(tMbmsMtkGenerateDataV8);

	TEST(tMbmsMtkGenerateDataV8.ExtensionId() == RMobilePhone::KETelMobileMbmsMtkGenerateDataV8);

	tMbmsMtkGenerateDataV8.iMikey = DMMTSY_PHONE_MBMS_VALID_IMIKEY;
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tMbmsMtkGenerateDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);

	TEST(tMbmsMtkGenerateDataV8.iMtk == DMMTSY_PHONE_MBMS_VALID_MTK);
	TEST(tMbmsMtkGenerateDataV8.iSaltKey == DMMTSY_PHONE_MBMS_VALID_SALTKEY);

	RMobilePhone::TMbmsMskDeleteDataV8 tMbmsMskDeleteDataV8;
	RMobilePhone::TMbmsMskDeleteDataV8Pckg tMbmsMskDeleteDataV8Pckgd(tMbmsMskDeleteDataV8);
	
	TEST(tMbmsMskDeleteDataV8.ExtensionId() == RMobilePhone::KETelMobileMbmsMskDeleteDataV8);
	
	tMbmsMskDeleteDataV8.iKeyDomainId = DMMTSY_PHONE_MBMS_VALID_KEYDMN;
	tMbmsMskDeleteDataV8.iMskIdKeyGroup = DMMTSY_PHONE_MBMS_VALID_MSKIDGRP;
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tMbmsMskDeleteDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	tMbmsMskDeleteDataV8.iKeyDomainId = DMMTSY_PHONE_MBMS_INVALID_KEYDMN;
	tMbmsMskDeleteDataV8.iMskIdKeyGroup = DMMTSY_PHONE_MBMS_INVALID_MSKIDGRP;
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tMbmsMskDeleteDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNotFound);
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tMbmsMskDeleteDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrInUse);
	
	RMobilePhone::TMbmsMukDeleteDataV8 tMbmsMukDeleteDataV8;
	RMobilePhone::TMbmsMukDeleteDataV8Pckg tMbmsMukDeleteDataV8Pckgd(tMbmsMukDeleteDataV8);
	
	TEST(tMbmsMukDeleteDataV8.ExtensionId() == RMobilePhone::KETelMobileMbmsMukDeleteDataV8);
	
	tMbmsMukDeleteDataV8.iMukIdTlv = DMMTSY_PHONE_MBMS_VALID_MUKID;
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tMbmsMukDeleteDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	tMbmsMukDeleteDataV8.iMukIdTlv = DMMTSY_PHONE_MBMS_INVALID_MUKID;
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tMbmsMukDeleteDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNotFound);
	
	aPhone.SmartCardAuthenticate(aReqStatus, appAID, tMbmsMukDeleteDataV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrInUse);
	
	RMobilePhone::TMbmsUserKeyV8 tMbmsUserKeyV8;
	RMobilePhone::TMbmsUserKeyV8Pckg tMbmsUserKeyV8Pckgd(tMbmsUserKeyV8);
	
	TEST(tMbmsUserKeyV8.ExtensionId() == RMobilePhone::KETelMobileMbmsUserKeyV8);
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckgd(tAuthInfoV8);
	
	tAuthInfoV8.iData = DMMTSY_PHONE_AUTH_DATA_MBMS_MUK;
	tAuthInfoV8.iAID = DMMTSY_PHONE_MBMS_AID;
	
	aPhone.GetAuthenticationParams(aReqStatus,tAuthInfoV8Pckgd,tMbmsUserKeyV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	
	INFO_PRINTF1(_L("Test - RMobilePhone::GetAuthenticationParams Completed."));
	
	TEST(tMbmsUserKeyV8.iMukIdi == DMMTSY_PHONE_MBMS_VALID_MUK_IDI);
	TEST(tMbmsUserKeyV8.iMukIdr == DMMTSY_PHONE_MBMS_VALID_MUK_IDR);
	TEST(tMbmsUserKeyV8.iTimeStampCounter == DMMTSY_PHONE_MBMS_VALID_MUK_ITIMESTM);
	
	tAuthInfoV8.iData = DMMTSY_PHONE_AUTH_DATA_NAF_LIST;
	tAuthInfoV8.iAID = DMMTSY_PHONE_MBMS_AID;
	
	aPhone.GetAuthenticationParams(aReqStatus,tAuthInfoV8Pckgd,tMbmsUserKeyV8Pckgd);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCorrupt);

	tAuthInfoV8.iData = DMMTSY_PHONE_AUTH_DATA_MBMS_MUK;
	tAuthInfoV8.iAID = DMMTSY_PHONE_MBMS_AID;
	
	aPhone.GetAuthenticationParams(aReqStatus,tAuthInfoV8Pckgd,tMbmsUserKeyV8Pckgd);
	aPhone.CancelAsyncRequest(EMobilePhoneGetAuthenticationParams);
	User::WaitForRequest(aReqStatus);
	TEST((aReqStatus.Int()==KErrNone) || (aReqStatus.Int()==KErrCancel));
	INFO_PRINTF1(_L("Test - Cancellation of RMobilePhone::GetAuthenticationParams Completed."));
	
	/* Retrieve the list for the MBMS authentication parameters */

	INFO_PRINTF1(_L("Test - Retrieval of CTestRetrieveMobilePhoneAuthenticationIds..."));
	
	CTestRetrieveMobilePhoneAuthenticationIds *pRetrieve = CTestRetrieveMobilePhoneAuthenticationIds::NewLC(aPhone,this);
	TBool startReqd = ETrue;
	
	pRetrieve->SetAuthType(CTestRetrieveMobilePhoneAuthenticationIds::EMbms);
	pRetrieve->TestRetrieve(DMMTSY_PHONE_AUTH_DATA_MBMS_MSK,startReqd);

	//Delete test object and start afresh
	
	CleanupStack::PopAndDestroy(); //pRetrieve
		
	pRetrieve = CTestRetrieveMobilePhoneAuthenticationIds::NewLC(aPhone,this);
	startReqd = ETrue;
	
	pRetrieve->SetAuthType(CTestRetrieveMobilePhoneAuthenticationIds::EMbms);
	pRetrieve->TestCancel(DMMTSY_PHONE_AUTH_DATA_MBMS_MSK);
	
	//Delete test object and start afresh
	CleanupStack::PopAndDestroy(); //pRetrieve	
		
	INFO_PRINTF1(_L("Test - CTestRetrieveMobilePhoneAuthenticationIds Completed."));
	}

void CTestPhone::TestWlanParametersL(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("WLAN FUNCTIONALITY"));

	//Verify the support of WLAN in USIM Service Table
	RMobilePhone::TMobilePhoneServiceTableV8 asyncSstV8File;
	RMobilePhone::TMobilePhoneServiceTableV8Pckg asyncSstV8FilePckgd(asyncSstV8File);
	RMobilePhone::TMobilePhoneServiceTable serviceTableType=DMMTSY_PHONE_SERVICE_TABLE;
	
	aPhone.GetServiceTable(aReqStatus, serviceTableType, asyncSstV8FilePckgd);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	if(aReqStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for WLAN passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for WLAN failed"));
		}

	// extract the returned SST file from the package
	asyncSstV8File=asyncSstV8FilePckgd();
	TEST(asyncSstV8File.ExtensionId()==KEtelExtMultimodeV8);
	TEST((asyncSstV8File.iServices57To64) == DMMTSY_PHONE_SST_BYTE8);
	TEST((asyncSstV8File.iServices65To72) == DMMTSY_PHONE_SST_BYTE9);
	
	// asynchronous & cancel
	aPhone.GetServiceTable(aReqStatus, serviceTableType, asyncSstV8FilePckgd);
	aPhone.CancelAsyncRequest(EMobilePhoneGetServiceTable);
   	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone || aReqStatus.Int()==KErrCancel);

	if (aReqStatus.Int()==KErrCancel)
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test  - RMobilePhone::GetServiceTable V8table (async & cancel (Request Not Cancelled)) passed"));
		}
   	
  	RMobilePhone::TUsimWlanDataV8 tUsimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg tUsimWlanDataV8Pckgd(tUsimWlanDataV8);
   
  	tUsimWlanDataV8.iFieldsUsed = DMMTSY_PHONE_WLAN_LIST_DATA_FLAGS;
  	tUsimWlanDataV8.iPseudonym = DMMTSY_PHONE_WLAN_VALID_PSEUDONYM;
  	tUsimWlanDataV8.iReauthenticationId = DMMTSY_PHONE_WLAN_VALID_REAUTHID;
  	tUsimWlanDataV8.iMasterKey = DMMTSY_PHONE_WLAN_VALID_MASTERKEY;
  	tUsimWlanDataV8.iCounter = DMMTSY_PHONE_WLAN_VALID_COUNTER;
  	
  	INFO_PRINTF1(_L("Test - RMobilePhone::SetWlanData async"));

  	aPhone.SetWlanData(aReqStatus,tUsimWlanDataV8Pckgd);
  	User::WaitForRequest(aReqStatus);
  	TEST(aReqStatus.Int() == KErrNone);
  	
  	tUsimWlanDataV8.iFieldsUsed = DMMTSY_PHONE_WLAN_LIST_DATA_FLAGS;
  	tUsimWlanDataV8.iPseudonym = DMMTSY_PHONE_WLAN_INVALID_PSEUDONYM;
  	tUsimWlanDataV8.iReauthenticationId = DMMTSY_PHONE_WLAN_INVALID_REAUTHID;
  	tUsimWlanDataV8.iMasterKey = DMMTSY_PHONE_WLAN_INVALID_MASTERKEY;
  	tUsimWlanDataV8.iCounter = DMMTSY_PHONE_WLAN_INVALID_COUNTER;
  
  	aPhone.SetWlanData(aReqStatus,tUsimWlanDataV8Pckgd);
  	User::WaitForRequest(aReqStatus);
  	TEST(aReqStatus.Int() == KErrCorrupt);
  	
  	tUsimWlanDataV8.iFieldsUsed = DMMTSY_PHONE_WLAN_LIST_DATA_FLAGS;
  	tUsimWlanDataV8.iPseudonym = DMMTSY_PHONE_WLAN_VALID_PSEUDONYM;
  	tUsimWlanDataV8.iReauthenticationId = DMMTSY_PHONE_WLAN_VALID_REAUTHID;
  	tUsimWlanDataV8.iMasterKey = DMMTSY_PHONE_WLAN_VALID_MASTERKEY;
  	tUsimWlanDataV8.iCounter = DMMTSY_PHONE_WLAN_VALID_COUNTER;
 
  	aPhone.SetWlanData(aReqStatus,tUsimWlanDataV8Pckgd);
  	aPhone.CancelAsyncRequest(EMobilePhoneSetWlanData);
  	User::WaitForRequest(aReqStatus);
  	TEST((aReqStatus.Int() == KErrNone) || (aReqStatus.Int() == KErrCancel));
  	
  	if (aReqStatus.Int()==KErrCancel)
  		{
  		INFO_PRINTF1(_L("Test - RMobilePhone::SetWlanData (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test  - RMobilePhone::SetWlanData (async & cancel (Request Not Cancelled)) passed"));
		}
  	
   	INFO_PRINTF1(_L("Test - RMobilePhone::GetWlanData async"));

  	aPhone.GetWlanData(aReqStatus,tUsimWlanDataV8Pckgd);
  	User::WaitForRequest(aReqStatus);
  	TEST(aReqStatus.Int() == KErrNone);
  	
  	TEST(tUsimWlanDataV8.iFieldsUsed == DMMTSY_PHONE_WLAN_LIST_DATA_FLAGS);
  	TEST(tUsimWlanDataV8.iPseudonym == DMMTSY_PHONE_WLAN_VALID_PSEUDONYM);
  	TEST(tUsimWlanDataV8.iReauthenticationId == DMMTSY_PHONE_WLAN_VALID_REAUTHID);
  	TEST(tUsimWlanDataV8.iMasterKey == DMMTSY_PHONE_WLAN_VALID_MASTERKEY);
  	TEST(tUsimWlanDataV8.iCounter == DMMTSY_PHONE_WLAN_VALID_COUNTER);
 
   	aPhone.GetWlanData(aReqStatus,tUsimWlanDataV8Pckgd);
  	aPhone.CancelAsyncRequest(EMobilePhoneGetWlanData);
  	User::WaitForRequest(aReqStatus);
  	TEST((aReqStatus.Int() == KErrNone)|| (aReqStatus.Int() == KErrCancel));
  	
  	if (aReqStatus.Int()==KErrCancel)
  		{
  		INFO_PRINTF1(_L("Test - RMobilePhone::GetWlanData (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test  - RMobilePhone::GetWlanData (async & cancel (Request Not Cancelled)) passed"));
		}
  	
  	INFO_PRINTF1(_L("Test - RMobilePhone::NotifyWlanDataChange async"));

  	aPhone.NotifyWlanDataChange(aReqStatus,tUsimWlanDataV8Pckgd);
  	User::WaitForRequest(aReqStatus);
  	TEST(aReqStatus.Int() == KErrNone);
  	
  	TEST(tUsimWlanDataV8.iFieldsUsed == DMMTSY_PHONE_WLAN_LIST_DATA_FLAGS);
  	TEST(tUsimWlanDataV8.iPseudonym == DMMTSY_PHONE_WLAN_VALID_PSEUDONYM);
  	TEST(tUsimWlanDataV8.iReauthenticationId == DMMTSY_PHONE_WLAN_VALID_REAUTHID);
  	TEST(tUsimWlanDataV8.iMasterKey == DMMTSY_PHONE_WLAN_VALID_MASTERKEY);
  	TEST(tUsimWlanDataV8.iCounter == DMMTSY_PHONE_WLAN_VALID_COUNTER);
  	
   	aPhone.NotifyWlanDataChange(aReqStatus,tUsimWlanDataV8Pckgd);
  	aPhone.CancelAsyncRequest(EMobilePhoneNotifyWlanDataChange);
  	User::WaitForRequest(aReqStatus);
  	TEST((aReqStatus.Int() == KErrNone) || (aReqStatus.Int() == KErrCancel));
  	
  	if (aReqStatus.Int()==KErrCancel)
  		{
  		INFO_PRINTF1(_L("Test - RMobilePhone::NotifyWlanDataChange (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test  - RMobilePhone::NotifyWlanDataChange (async & cancel (Request Not Cancelled)) passed"));
		}
  	
   	CTestRetrieveMobilePhonePreferredWlanSIDs *pRetrieve = CTestRetrieveMobilePhonePreferredWlanSIDs::NewLC(aPhone,this);
  	TBool startReqd = ETrue;
  	/* To test the 2 phase retrieval of CRetrieveMobilePhonePreferredWlanSIDs */
  	pRetrieve->TestRetrieve(startReqd);
   	//Delete test object and start afresh
  	CleanupStack::PopAndDestroy(); //pRetrieve

  	/* To test the cancellation of 2 phase retrieval of CRetrieveMobilePhonePreferredWlanSIDs */
  	pRetrieve = CTestRetrieveMobilePhonePreferredWlanSIDs::NewLC(aPhone,this);
  	pRetrieve->TestRetrieve(startReqd);
  	pRetrieve->TestCancel();
  	//Delete test object and start afresh
  	CleanupStack::PopAndDestroy(); //pRetrieve
  	
  	RMobilePhone::TWlanSIDV8 tWlanSIDV8_1;
  	RMobilePhone::TWlanSIDV8 tWlanSIDV8_2;
  	RMobilePhone::TWlanSIDV8 tWlanSIDV8_3;
  	
  	CMobilePhoneStoredWlanSIDList* wlanSIDlist;
  	wlanSIDlist =  CMobilePhoneStoredWlanSIDList::NewL();
    CleanupStack::PushL(wlanSIDlist);
      
    tWlanSIDV8_1.iWSID = DMMTSY_PHONE_WLAN_WSID_ONE;
  	tWlanSIDV8_2.iWSID = DMMTSY_PHONE_WLAN_WSID_TWO;
  	tWlanSIDV8_3.iWSID = DMMTSY_PHONE_WLAN_WSID_THREE;
  	
  	tWlanSIDV8_1.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_ONE;
  	tWlanSIDV8_2.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_TWO;
  	tWlanSIDV8_3.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_THREE;
  		
    wlanSIDlist->AddEntryL(tWlanSIDV8_1);
    wlanSIDlist->AddEntryL(tWlanSIDV8_2);
    wlanSIDlist->AddEntryL(tWlanSIDV8_3);
      
    aPhone.StorePreferredWlanSIDListL(aReqStatus,wlanSIDlist);
    User::WaitForRequest(aReqStatus);
    TEST(aReqStatus.Int() == KErrNone);
    CleanupStack::PopAndDestroy();
    
    wlanSIDlist =  CMobilePhoneStoredWlanSIDList::NewL();
    CleanupStack::PushL(wlanSIDlist);
 	   	
	tWlanSIDV8_1.iWSID = DMMTSY_PHONE_WLAN_INVALID_WSID_ONE;
  	tWlanSIDV8_2.iWSID = DMMTSY_PHONE_WLAN_INVALID_WSID_TWO;
  	tWlanSIDV8_3.iWSID = DMMTSY_PHONE_WLAN_INVALID_WSID_THREE;
   	
  	tWlanSIDV8_1.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_THREE;
  	tWlanSIDV8_2.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_TWO;
  	tWlanSIDV8_3.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_ONE;
  		
    wlanSIDlist->AddEntryL(tWlanSIDV8_1);
    wlanSIDlist->AddEntryL(tWlanSIDV8_2);
    wlanSIDlist->InsertEntryL(1,tWlanSIDV8_3); // Insert at position=1
    wlanSIDlist->DeleteEntryL(2); // delete the last userdefined entry.
    wlanSIDlist->AddEntryL(tWlanSIDV8_2); // add the deleted entry again
         
    INFO_PRINTF1(_L("Test - RMobilePhone::StorePreferredWlanSIDListL async"));
  
    aPhone.StorePreferredWlanSIDListL(aReqStatus,wlanSIDlist);
    User::WaitForRequest(aReqStatus);
    TEST(aReqStatus.Int() == KErrCorrupt);
    CleanupStack::PopAndDestroy();
    
    wlanSIDlist =  CMobilePhoneStoredWlanSIDList::NewL();
    CleanupStack::PushL(wlanSIDlist);
          
    tWlanSIDV8_1.iWSID = DMMTSY_PHONE_WLAN_WSID_ONE;
  	tWlanSIDV8_2.iWSID = DMMTSY_PHONE_WLAN_WSID_TWO;
  	tWlanSIDV8_3.iWSID = DMMTSY_PHONE_WLAN_WSID_THREE;
   	
  	tWlanSIDV8_1.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_ONE;
  	tWlanSIDV8_2.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_TWO;
  	tWlanSIDV8_3.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_THREE;
  		
    wlanSIDlist->AddEntryL(tWlanSIDV8_1);
    wlanSIDlist->AddEntryL(tWlanSIDV8_2);
    wlanSIDlist->AddEntryL(tWlanSIDV8_3);
      
    aPhone.StorePreferredWlanSIDListL(aReqStatus,wlanSIDlist);
    aPhone.CancelAsyncRequest(EMobilePhoneStorePreferredWlanSIDList );
    User::WaitForRequest(aReqStatus);
    TEST((aReqStatus.Int() == KErrNone)  || (aReqStatus.Int() == KErrCancel));
  	
    CleanupStack::PopAndDestroy();
     
    INFO_PRINTF1(_L("Test - RMobilePhone::NotifyPreferredWlanSIDListChange async"));
      
    aPhone.NotifyPreferredWlanSIDListChange(aReqStatus);
    User::WaitForRequest(aReqStatus);
    TEST(aReqStatus.Int() == KErrNone);
      
    aPhone.NotifyPreferredWlanSIDListChange(aReqStatus);
    aPhone.CancelAsyncRequest(EMobilePhoneNotifyPreferredWlanSIDListChange);
    User::WaitForRequest(aReqStatus);
    TEST((aReqStatus.Int() == KErrNone)  || (aReqStatus.Int() == KErrCancel));
    if (aReqStatus.Int()==KErrCancel)
  		{
  		INFO_PRINTF1(_L("Test - RMobilePhone::NotifyPreferredWlanSIDListChange (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test  - RMobilePhone::NotifyPreferredWlanSIDListChange (async & cancel (Request Not Cancelled)) passed"));
		}
	}
	
void CTestPhone::ValidatePhoneStoreInfo(const RMobilePhoneStore::TMobilePhoneStoreInfoV1 &aSyncStoreInfo, TInt aSlot)
	{
	TEST(aSlot == 0 || aSlot == 1);
	switch(aSlot)
		{
		case 0:
			{
			TEST(aSyncStoreInfo.iName==DMMTSY_PHONE_STORE_NAME1);
			TEST(aSyncStoreInfo.iType==DMMTSY_PHONE_STORE_TYPE1);
			TEST(aSyncStoreInfo.iTotalEntries==DMMTSY_PHONE_STORE_TOTAL_ENTRIES1);
			TEST(aSyncStoreInfo.iUsedEntries==DMMTSY_PHONE_STORE_USED_ENTRIES1);
			TEST(aSyncStoreInfo.iCaps==DMMTSY_PHONE_STORE_CAPS1);
			break;
			}
		case 1:
			{
			TEST(aSyncStoreInfo.iName==DMMTSY_PHONE_STORE_NAME2);
			TEST(aSyncStoreInfo.iType==DMMTSY_PHONE_STORE_TYPE2);
			TEST(aSyncStoreInfo.iTotalEntries==DMMTSY_PHONE_STORE_TOTAL_ENTRIES2);
			TEST(aSyncStoreInfo.iUsedEntries==DMMTSY_PHONE_STORE_USED_ENTRIES2);
			TEST(aSyncStoreInfo.iCaps==DMMTSY_PHONE_STORE_CAPS2);
			break;
			}
		default:
			// Incorrect number of phone stores - flag error
			break;
		}
	}

void CTestPhone::ValidateCustomerServiceProfile(const RMobilePhone::TMobilePhoneCspFileV1 &aSyncCspFile)
	{											
	TEST(aSyncCspFile.iCallOfferingServices==DMMTSY_PHONE_CSP_BYTE1);
	TEST(aSyncCspFile.iCallRestrictionServices==DMMTSY_PHONE_CSP_BYTE2);
	TEST(aSyncCspFile.iOtherSuppServices==DMMTSY_PHONE_CSP_BYTE3);
	TEST(aSyncCspFile.iCallCompletionServices==DMMTSY_PHONE_CSP_BYTE4);
	TEST(aSyncCspFile.iTeleservices==DMMTSY_PHONE_CSP_BYTE5);
	TEST(aSyncCspFile.iCphsTeleservices==DMMTSY_PHONE_CSP_BYTE6);
	TEST(aSyncCspFile.iCphsFeatures==DMMTSY_PHONE_CSP_BYTE7);
	TEST(aSyncCspFile.iNumberIdentServices==DMMTSY_PHONE_CSP_BYTE8);
	TEST(aSyncCspFile.iPhase2PlusServices==DMMTSY_PHONE_CSP_BYTE9);
	TEST(aSyncCspFile.iValueAddedServices==DMMTSY_PHONE_CSP_BYTE10);
	}

void CTestPhone::ValidateServiceTable(const RMobilePhone::TMobilePhoneServiceTableV1 &aSyncServiceTable)
	{
	TEST(aSyncServiceTable.iServices1To8==DMMTSY_PHONE_SST_BYTE1);
	TEST(aSyncServiceTable.iServices9To16==DMMTSY_PHONE_SST_BYTE2);
	TEST(aSyncServiceTable.iServices17To24==DMMTSY_PHONE_SST_BYTE3);
	TEST(aSyncServiceTable.iServices25To32==DMMTSY_PHONE_SST_BYTE4);
	TEST(aSyncServiceTable.iServices33To40==DMMTSY_PHONE_SST_BYTE5);
	TEST(aSyncServiceTable.iServices41To48==DMMTSY_PHONE_SST_BYTE6);
	TEST(aSyncServiceTable.iServices49To56==DMMTSY_PHONE_SST_BYTE7);
	}

void CTestPhone::ValidateServiceTable(const RMobilePhone::TMobilePhoneServiceTableV2 &aSyncServiceTable)
	{
	TEST(aSyncServiceTable.iServices1To8==DMMTSY_PHONE_SST_BYTE1);
	TEST(aSyncServiceTable.iServices9To16==DMMTSY_PHONE_SST_BYTE2);
	TEST(aSyncServiceTable.iServices17To24==DMMTSY_PHONE_SST_BYTE3);
	TEST(aSyncServiceTable.iServices25To32==DMMTSY_PHONE_SST_BYTE4);
	TEST(aSyncServiceTable.iServices33To40==DMMTSY_PHONE_SST_BYTE5);
	TEST(aSyncServiceTable.iServices41To48==DMMTSY_PHONE_SST_BYTE6);
	TEST(aSyncServiceTable.iServices49To56==DMMTSY_PHONE_SST_BYTE7);
	TEST(aSyncServiceTable.iAID==DMMTSY_PHONE_USIM_AID);
	}

void CTestPhone::ValidatePhoneId(const RMobilePhone::TMobilePhoneIdentityV1 &aSyncPhoneId)
	{
	TEST(aSyncPhoneId.iManufacturer.Compare(DMMTSY_PHONE_MANUFACTURER)==0);
	TEST(aSyncPhoneId.iModel.Compare(DMMTSY_PHONE_MODEL)==0);
	TEST(aSyncPhoneId.iRevision.Compare(DMMTSY_PHONE_REVISION)==0);
	TEST(aSyncPhoneId.iSerialNumber.Compare(DMMTSY_PHONE_SERIAL_NUMBER)==0);
	}


void CTestPhone::ValidateHomeNetwork(const RMobilePhone::TMobilePhoneNetworkInfoV1 &aSyncHomeNetwork) 
	{
	TEST(aSyncHomeNetwork.iMode==DMMTSY_PHONE_HOME_NETWORK_MODE);
	TEST(aSyncHomeNetwork.iStatus==DMMTSY_PHONE_HOME_NETWORK_STATUS);
	TEST(aSyncHomeNetwork.iBandInfo==DMMTSY_PHONE_HOME_NETWORK_BAND);
	TEST(aSyncHomeNetwork.iCountryCode.Compare(DMMTSY_PHONE_HOME_NETWORK_MCC) == 0);
	TEST(aSyncHomeNetwork.iNetworkId.Compare(DMMTSY_PHONE_HOME_NETWORK_ID) == 0);
	TEST(aSyncHomeNetwork.iCdmaSID.Compare(DMMTSY_PHONE_HOME_NETWORK_SID) == 0);
	TEST(aSyncHomeNetwork.iAnalogSID.Compare(DMMTSY_PHONE_HOME_NETWORK_SID) == 0);
	TEST(aSyncHomeNetwork.iDisplayTag.Compare(DMMTSY_PHONE_HOME_NETWORK_TAG)==0);
	TEST(aSyncHomeNetwork.iShortName.Compare(DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME)==0);
	TEST(aSyncHomeNetwork.iLongName.Compare(DMMTSY_PHONE_HOME_NETWORK_LONG_NAME)==0);
	}

void CTestPhone::ValidateCurrentNetwork(const RMobilePhone::TMobilePhoneNetworkInfoV1 &aSyncCurrentNetwork,		
		const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea, TBool aValidateArea)
	{
	TEST(aSyncCurrentNetwork.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE1);
	TEST(aSyncCurrentNetwork.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS1);
	TEST(aSyncCurrentNetwork.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND1);
	TEST(aSyncCurrentNetwork.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC1) == 0);
	TEST(aSyncCurrentNetwork.iNetworkId.Compare(DMMTSY_PHONE_CURRENT_NETWORK_ID1)==0);
	TEST(aSyncCurrentNetwork.iCdmaSID.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SID)==0);
	TEST(aSyncCurrentNetwork.iAnalogSID.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SID)==0);
	TEST(aSyncCurrentNetwork.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG1)==0);
	TEST(aSyncCurrentNetwork.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1)==0);
	TEST(aSyncCurrentNetwork.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1)==0);

	if(aValidateArea)
		{
		TEST(aSyncLocArea.iAreaKnown==DMMTSY_PHONE_CURRENT_NETWORK_AREAKNOWN);
		TEST(aSyncLocArea.iLocationAreaCode==DMMTSY_PHONE_CURRENT_NETWORK_LAC1);
		TEST(aSyncLocArea.iCellId==DMMTSY_PHONE_CURRENT_NETWORK_CELLID1);	
		}

	}

void CTestPhone::ValidateCurrentNetworkforHSUPA(const RMobilePhone::TMobilePhoneNetworkInfoV8 &aSyncCurrentNetwork,
		const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea)
	{
	TEST(aSyncCurrentNetwork.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE1);
	TEST(aSyncCurrentNetwork.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS1);
	TEST(aSyncCurrentNetwork.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND1);
	TEST(aSyncCurrentNetwork.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC1) == 0);
	TEST(aSyncCurrentNetwork.iNetworkId.Compare(DMMTSY_PHONE_CURRENT_NETWORK_ID1)==0);
	TEST(aSyncCurrentNetwork.iCdmaSID.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SID)==0);
	TEST(aSyncCurrentNetwork.iAnalogSID.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SID)==0);
	TEST(aSyncCurrentNetwork.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG1)==0);
	TEST(aSyncCurrentNetwork.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1)==0);
	TEST(aSyncCurrentNetwork.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1)==0);
	
	TEST(aSyncCurrentNetwork.iAccess==DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH1);
	TEST(aSyncCurrentNetwork.iHsdpaAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY1);
	TEST(aSyncCurrentNetwork.iEgprsAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY1);
	TEST(aSyncCurrentNetwork.iHsupaAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_HSUPA_AVAILABILITY1);

	(void)aSyncLocArea; // to avoid compilation warnings

	}

void CTestPhone::ValidateHomeNetworkforHSUPA(const RMobilePhone::TMobilePhoneNetworkInfoV8 &aSyncHomeNetwork) 
	{
	TEST(aSyncHomeNetwork.iMode==DMMTSY_PHONE_HOME_NETWORK_MODE);
	TEST(aSyncHomeNetwork.iStatus==DMMTSY_PHONE_HOME_NETWORK_STATUS);
	TEST(aSyncHomeNetwork.iBandInfo==DMMTSY_PHONE_HOME_NETWORK_BAND);
	TEST(aSyncHomeNetwork.iCountryCode.Compare(DMMTSY_PHONE_HOME_NETWORK_MCC) == 0);
	TEST(aSyncHomeNetwork.iNetworkId.Compare(DMMTSY_PHONE_HOME_NETWORK_ID) == 0);
	TEST(aSyncHomeNetwork.iCdmaSID.Compare(DMMTSY_PHONE_HOME_NETWORK_SID) == 0);
	TEST(aSyncHomeNetwork.iAnalogSID.Compare(DMMTSY_PHONE_HOME_NETWORK_SID) == 0);
	TEST(aSyncHomeNetwork.iDisplayTag.Compare(DMMTSY_PHONE_HOME_NETWORK_TAG)==0);
	TEST(aSyncHomeNetwork.iShortName.Compare(DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME)==0);
	TEST(aSyncHomeNetwork.iLongName.Compare(DMMTSY_PHONE_HOME_NETWORK_LONG_NAME)==0);
	
	TEST(aSyncHomeNetwork.iAccess==DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH);
	TEST(aSyncHomeNetwork.iHsdpaAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_HSDPA_AVAILABILITY);
	TEST(aSyncHomeNetwork.iEgprsAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_EGPRS_AVAILABILITY);
	TEST(aSyncHomeNetwork.iHsupaAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_HSUPA_AVAILABILITY);

	}

void CTestPhone::ValidateNotifyCurrentNetworkChangeforHSUPA(const RMobilePhone::TMobilePhoneNetworkInfoV8 &aSyncCurrentNetwork,const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea)

	{
	TEST(aSyncCurrentNetwork.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE2);
	TEST(aSyncCurrentNetwork.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS2);
	TEST(aSyncCurrentNetwork.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND2);
	TEST(aSyncCurrentNetwork.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC2) == 0);
	TEST(aSyncCurrentNetwork.iNetworkId.Compare(DMMTSY_PHONE_CURRENT_NETWORK_ID2) == 0);
	TEST(aSyncCurrentNetwork.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG2)==0);
	TEST(aSyncCurrentNetwork.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2)==0);
	TEST(aSyncCurrentNetwork.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2)==0);

	
	TEST(aSyncCurrentNetwork.iAccess==DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH2);
	TEST(aSyncCurrentNetwork.iHsdpaAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY2);
	TEST(aSyncCurrentNetwork.iEgprsAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY2);
	TEST(aSyncCurrentNetwork.iHsupaAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_HSUPA_AVAILABILITY2);

	(void) aSyncLocArea; // to avoid compilation warnings
	
	}

void CTestPhone::ValidateNotifyCurrentNetworkChange(const RMobilePhone::TMobilePhoneNetworkInfoV1 &aSyncCurrentNetwork,	
				const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea, TBool aValidateArea)
	{
	TEST(aSyncCurrentNetwork.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE2);
	TEST(aSyncCurrentNetwork.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS2);
	TEST(aSyncCurrentNetwork.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND2);
	TEST(aSyncCurrentNetwork.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC2) == 0);
	TEST(aSyncCurrentNetwork.iNetworkId.Compare(DMMTSY_PHONE_CURRENT_NETWORK_ID2) == 0);
	TEST(aSyncCurrentNetwork.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG2)==0);
	TEST(aSyncCurrentNetwork.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2)==0);
	TEST(aSyncCurrentNetwork.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2)==0);

	if(aValidateArea)
		{
		TEST(aSyncLocArea.iAreaKnown==DMMTSY_PHONE_CURRENT_NETWORK_AREAKNOWN);
		TEST(aSyncLocArea.iLocationAreaCode==DMMTSY_PHONE_CURRENT_NETWORK_LAC2);
		TEST(aSyncLocArea.iCellId==DMMTSY_PHONE_CURRENT_NETWORK_CELLID2);
		}
	}
	
void CTestPhone::ValidateCurrentNetworkISV(const RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1 &aSyncCurrentNetworkISV,
									const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea)
	{
	TEST(aSyncCurrentNetworkISV.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE_ISV);
	TEST(aSyncCurrentNetworkISV.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS_ISV);
	TEST(aSyncCurrentNetworkISV.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND_ISV);
	TEST(aSyncCurrentNetworkISV.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC_ISV) == 0);
	TEST(aSyncCurrentNetworkISV.iNetworkId.Compare(DMMTSY_PHONE_CURRENT_NETWORK_ID_ISV)==0);
	TEST(aSyncCurrentNetworkISV.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG_ISV)==0);
	TEST(aSyncCurrentNetworkISV.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME_ISV)==0);
	TEST(aSyncCurrentNetworkISV.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME_ISV)==0);

	TEST(aSyncLocArea.iAreaKnown==DMMTSY_PHONE_CURRENT_NETWORK_AREAKNOWN_ISV);
	TEST(aSyncLocArea.iLocationAreaCode==DMMTSY_PHONE_CURRENT_NETWORK_LAC_ISV);
	TEST(aSyncLocArea.iCellId==DMMTSY_PHONE_CURRENT_NETWORK_CELLID_ISV);
	}

void CTestPhone::ValidateNotifyCurrentNetworkISVChange(const RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1 &aSyncCurrentNetworkISV,
									const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea)
	{
	TEST(aSyncCurrentNetworkISV.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE_ISV2);
	TEST(aSyncCurrentNetworkISV.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS_ISV2);
	TEST(aSyncCurrentNetworkISV.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND_ISV2);
	TEST(aSyncCurrentNetworkISV.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC_ISV2) == 0);
	TEST(aSyncCurrentNetworkISV.iNetworkId.Compare(DMMTSY_PHONE_CURRENT_NETWORK_ID_ISV2)==0);
	TEST(aSyncCurrentNetworkISV.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG_ISV2)==0);
	TEST(aSyncCurrentNetworkISV.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME_ISV2)==0);
	TEST(aSyncCurrentNetworkISV.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME_ISV2)==0);

	TEST(aSyncLocArea.iAreaKnown==DMMTSY_PHONE_CURRENT_NETWORK_AREAKNOWN_ISV2);
	TEST(aSyncLocArea.iLocationAreaCode==DMMTSY_PHONE_CURRENT_NETWORK_LAC_ISV2);
	TEST(aSyncLocArea.iCellId==DMMTSY_PHONE_CURRENT_NETWORK_CELLID_ISV2);
	}


void CTestPhone::ValidateNITZInfo(const RMobilePhone::TMobilePhoneNITZ &aDateTime)
	{
	TEST(aDateTime.iNitzFieldsUsed == DMMTSY_PHONE_NWTINFO_NITZ_USED);
	TEST(aDateTime.iTimeZone == DMMTSY_PHONE_NWTINFO_TIMEZONE);
	TEST(aDateTime.iLongNetworkId.Compare(DMMTSY_PHONE_NWTINFO_LONG_ID)==0);
	TEST(aDateTime.iShortNetworkId.Compare(DMMTSY_PHONE_NWTINFO_SHORT_ID)==0);
	TEST(aDateTime.iDST == DMMTSY_PHONE_NWTINFO_DST);
	TEST(aDateTime.Year() == DMMTSY_PHONE_NWTINFO_YEAR);
	TEST(aDateTime.Month() == DMMTSY_PHONE_NWTINFO_MONTH);
	TEST(aDateTime.Day() == DMMTSY_PHONE_NWTINFO_DAY);
	TEST(aDateTime.Hour() == DMMTSY_PHONE_NWTINFO_HOUR);
	TEST(aDateTime.Minute() == DMMTSY_PHONE_NWTINFO_MINUTE);
	TEST(aDateTime.Second() == DMMTSY_PHONE_NWTINFO_SECOND);
	TEST(aDateTime.MicroSecond() == DMMTSY_PHONE_NWTINFO_MICRO);
	}

void CTestPhone::ValidateSPName(const RMobilePhone::TMobilePhoneServiceProviderNameV2 &aName)
	{
	TEST(aName.iDisplayReq == DMMTSY_PHONE_DISPREQ);
	TEST(aName.iSPName.Compare(DMMTSY_PHONE_SPN_NAME) == 0);
	TEST(aName.iPLMNField.Compare(DMMTSY_PHONE_PLMN_FIELD) == 0);
	}

void CTestPhone::ValidateCostInfo(const RMobilePhone::TMobilePhoneCostInfoV1 &aInfo)
	{
	TEST(aInfo.iService == DMMTSY_PHONE_COST_SERVICE);
	TEST(aInfo.iCCM == DMMTSY_PHONE_COST_CCM);
	TEST(aInfo.iACM == DMMTSY_PHONE_COST_ACM);
	TEST(aInfo.iACMmax == DMMTSY_PHONE_COST_ACMMAX);
	TEST(aInfo.iPuct.iCurrencyName.Compare(DMMTSY_PHONE_COST_PUCT_CNAME) == 0);
	TEST(aInfo.iPuct.iPricePerUnit == DMMTSY_PHONE_COST_PUCT_PPU);
	}

void CTestPhone::ValidateMulticallParams(const RMobilePhone::TMobilePhoneMulticallSettingsV1 &aSettings, TInt aCount)
	{
	TEST(aCount == 0 || aCount == 1);

	switch(aCount)
		{
		case 0:
			{
			TEST(aSettings.iUserMaxBearers             == DMMTSY_PHONE_USER_MAX_BEARERS1);	
			TEST(aSettings.iServiceProviderMaxBearers  == DMMTSY_PHONE_PROVIDER_MAX_BEARERS1);
			TEST(aSettings.iNetworkSupportedMaxBearers == DMMTSY_PHONE_NETWORK_MAX_BEARERS1);
			TEST(aSettings.iUESupportedMaxBearers      == DMMTSY_PHONE_UE_MAX_BEARERS1);
			break;
			}

		case 1:
			{
			TEST(aSettings.iUserMaxBearers             == DMMTSY_PHONE_USER_MAX_BEARERS2);	
			TEST(aSettings.iServiceProviderMaxBearers  == DMMTSY_PHONE_PROVIDER_MAX_BEARERS2);
			TEST(aSettings.iNetworkSupportedMaxBearers == DMMTSY_PHONE_NETWORK_MAX_BEARERS2);
			TEST(aSettings.iUESupportedMaxBearers      == DMMTSY_PHONE_UE_MAX_BEARERS2);
			break;
			}
		}
	}

void CTestPhone::TestTerminateAllCalls(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
	{
	//Test asynchronous TerminateAllCalls - just returns KErrNone
	
	aPhone.TerminateAllCalls(aReqStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int()==KErrNone);
	
	//Test asynchronous TerminateAllCalls & Cancel
	
	aPhone.TerminateAllCalls(aReqStatus);
	aPhone.CancelAsyncRequest(EMobilePhoneTerminateAllCalls);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrCancel);
	}	

void CTestPhone::TestTerminateActiveCalls(RMobilePhone& aPhone, TRequestStatus &aReqStatus)
    {
    //Test asynchronous TerminateActiveCalls - just returns KErrNone
    
    aPhone.TerminateActiveCalls(aReqStatus);
    User::WaitForRequest(aReqStatus);
    TEST(aReqStatus.Int()==KErrNone);
    if (KErrNone == aReqStatus.Int())
        INFO_PRINTF2(_L("Test %d - RMobilePhone::TerminateActiveCalls (async) passed"), iTestCount++);
    else
        ERR_PRINTF2(_L("Test %d - RMobilePhone::TerminateActiveCalls (async) failed"), iTestCount++);
    //Test asynchronous TerminateActiveCalls & Cancel
    
    aPhone.TerminateActiveCalls(aReqStatus);
    aPhone.CancelAsyncRequest(EMobilePhoneTerminateActiveCalls);
    User::WaitForRequest(aReqStatus);
    TEST(aReqStatus.Int() == KErrCancel);
    if (KErrCancel == aReqStatus.Int())
        INFO_PRINTF2(_L("Test %d - Cancelation of  RMobilePhone::TerminateActiveCalls (async) passed"), iTestCount++);
    else
        ERR_PRINTF2(_L("Test %d - Cancelation of  RMobilePhone::TerminateActiveCalls (async) failed"), iTestCount++);
    }   

/**************************************************************/
//
// Testing asynchronous retrieve of detected network list
//
/**************************************************************/

CTestPhone::CTestGetDetectedNetworks* CTestPhone::CTestGetDetectedNetworks::NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone)
	{
    CTestGetDetectedNetworks* r=new(ELeave) CTestGetDetectedNetworks(aPhone, aTestPhone);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestPhone::CTestGetDetectedNetworks::CTestGetDetectedNetworks(RMobilePhone& aPhone, CTestPhone* aTestPhone)
    : CActive(EPriorityNormal), iPhone(aPhone),iTestPhone(aTestPhone), iTestVersion(ETestV1)
    {}
      

void CTestPhone::CTestGetDetectedNetworks::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestPhone::CTestGetDetectedNetworks::~CTestGetDetectedNetworks()
    {
    delete iRetrieve;
	delete iCanceller;
	}

void CTestPhone::CTestGetDetectedNetworks::SetTestVersion(TTestVersion aTestVersion)
	{
	iTestVersion = aTestVersion;
	iTestPhone->INFO_PRINTF2(_L("Value of iTestVersion = %d"),iTestVersion);
	}

void CTestPhone::CTestGetDetectedNetworks::TestRetrieve()
	{
	Start();
	CActiveScheduler::Start();

	// ensure that the same AO can be used to retrieve more than one list in succession
	
//	Start();
//      CActiveScheduler::Start();
	}

void CTestPhone::CTestGetDetectedNetworks::Start()
    {
	//If iTestVersion is set to ETestV1, test calling the V1 list retrieval method
	if(iTestVersion==ETestV1)
		{
		iRetrieve->Start(iStatus);
		TRequestStatus tmpStatus;
		iRetrieve->Start(tmpStatus);
		User::WaitForRequest(tmpStatus);
		iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
	    SetActive();
		}
	//If iTestVersion is set to ETestV2, test calling the V2 list retrieval method
	else if(iTestVersion==ETestV2)
		{
		iRetrieve->StartV2(iStatus);
		TRequestStatus tmpStatus;
		iRetrieve->StartV2(tmpStatus);
		User::WaitForRequest(tmpStatus);
		iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
	    SetActive();
		}
	//If iTestVersion is set to ETestV5, test calling the V5 list retrieval method
	else if(iTestVersion==ETestV5)
		{
		iRetrieve->StartV5(iStatus);
		TRequestStatus tmpStatus;
		iRetrieve->StartV5(tmpStatus);
		User::WaitForRequest(tmpStatus);
		iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
		SetActive();
		}
	else if (iTestVersion==ETestV8)
		{
		iRetrieve->StartV8(iStatus);
		TRequestStatus tmpStatus;
		iRetrieve->StartV8(tmpStatus);
		User::WaitForRequest(tmpStatus);
		iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
		iTestPhone->INFO_PRINTF2(_L("In Start : Value of iTestVersion = %d"),iTestVersion);
		SetActive();
		}
	else
	{}
	}       

void CTestPhone::CTestGetDetectedNetworks::TestCancel()
	{
	Start();

	CTestCanceller* cancelGetNetworks = RetrieveCancelPtr();
	cancelGetNetworks->Call();

	CActiveScheduler::Start();
	TBool cancelled = (iStatus == KErrCancel);

	// Ensure that AO can be used after its previous transaction was cancelled
	Start();
	CActiveScheduler::Start();

	if (cancelled)
		{
		iTestPhone->INFO_PRINTF2(_L("Cancelled version is %d"),iTestVersion);
		iTestPhone->INFO_PRINTF2(_L("Test %d - RMobilePhone::GetDetectedNetworks (async & cancel (Cancelled Request)) passed"), iTestPhone->iTestCount++);
		}
	else
		{
		iTestPhone->INFO_PRINTF2(_L("Test %d - RMobilePhone::GetDetectedNetworks (async & cancel (Request Not Cancelled)) passed"), iTestPhone->iTestCount++);
		}	
	}      

CTestCanceller* CTestPhone::CTestGetDetectedNetworks::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestPhone::CTestGetDetectedNetworks::RunL()
    {
		TInt ret=iStatus.Int();
		User::LeaveIfError(ret);

	iTestPhone->INFO_PRINTF2(_L("In RunL with iTestVersion = %d"),iTestVersion);	
	
		if(iTestVersion == ETestV1)
			{
			iTestPhone->INFO_PRINTF1(_L("In RunL with iTestVersion =ETestV1"));
			ret=CheckResults();
			}
		else if(iTestVersion == ETestV2)
			{
			iTestPhone->INFO_PRINTF1(_L("In RunL with iTestVersion =ETestV2"));
			ret=CheckResultsV2();
			}
		else if(iTestVersion == ETestV5)
			{
			iTestPhone->INFO_PRINTF1(_L("In RunL with iTestVersion =ETestV5"));
			ret=CheckResultsV5();
			}
		else if(iTestVersion == ETestV8)
			{
			iTestPhone->INFO_PRINTF1(_L("In RunL with iTestVersion =ETestV8"));
			ret=CheckResultsV8();
			}
		else
			{
			iTestPhone->INFO_PRINTF1(_L("In RunL with iTestVersion =Error"));
			ret=KErrNotFound;
			}

		User::LeaveIfError(ret);

		CActiveScheduler::Stop();
	}
      

void CTestPhone::CTestGetDetectedNetworks::DoCancel()
    {
	iRetrieve->Cancel();
    }       
      
TInt CTestPhone::CTestGetDetectedNetworks::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneNetworkList* networkList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, networkList=iRetrieve->RetrieveListL(););

	//Check that a list is returned back to the test
	iTestPhone->TEST(networkList!=NULL);

	if (networkList)
	{
	//Check that the number of entries in the list is as expected
	iTestPhone->TEST(networkList->Enumerate()==DMMTSY_NETWORK_COUNT);
	
	// Get each detected network entry
	RMobilePhone::TMobilePhoneNetworkInfoV1 networkEntry;

	for (TInt i=0; i<DMMTSY_NETWORK_COUNT; ++i)
		{
		TRAP(ret,networkEntry=networkList->GetEntryL(i));
		if (ret != KErrNone)
			{
			break;
			}
		switch(i)
			{
			case 0:
				{
				iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE1);
				iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS1);
				iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND1);
				iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC1)==0);
				iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_CURRENT_NETWORK_ID1)==0);
				iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG1)==0);
				iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1)==0);
				iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1)==0);
				break;
				}

			case 1:
				{
				iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE2);
				iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS2);
				iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND2);
				iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC2)==0);
				iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_CURRENT_NETWORK_ID2)==0);
				iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG2)==0);
				iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2)==0);
				iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2)==0);
				break;
				}

			case 2:
				{
				iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_HOME_NETWORK_MODE);
				iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_HOME_NETWORK_STATUS);
				iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_HOME_NETWORK_BAND);
				iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_HOME_NETWORK_MCC)==0);
				iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_HOME_NETWORK_ID)==0);
				iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_HOME_NETWORK_TAG)==0);
				iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME)==0);
				iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_HOME_NETWORK_LONG_NAME)==0);
				break;
				}

			default:
				break;
			}
		}
	delete networkList;
	}
   return ret;
   }

TInt CTestPhone::CTestGetDetectedNetworks::CheckResultsV2()
    {
	TInt ret=KErrNone;

	CMobilePhoneNetworkListV2* networkList=NULL;

	TInt leaveCode=KErrNone;
	TRAP(leaveCode, networkList=iRetrieve->RetrieveListV2L(););

	//Check that a list is returned back to the test
	iTestPhone->TEST(networkList!=NULL);

	if (networkList)
	{
	//Check that the number of entries in the list is as expected
	iTestPhone->TEST(networkList->Enumerate()==DMMTSY_NETWORK_COUNT);

	// Get each detected network entry
	RMobilePhone::TMobilePhoneNetworkInfoV2 networkEntry;

	for (TInt i=0; i<DMMTSY_NETWORK_COUNT; ++i)
		{
		TRAP(ret,networkEntry=networkList->GetEntryL(i));
		if (ret != KErrNone)
			break;
		switch(i)
			{
			case 0:
				{
				iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE1);
				iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS1);
				iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND1);
				iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC1)==0);
				iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_CURRENT_NETWORK_ID1)==0);
				iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG1)==0);
				iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1)==0);
				iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1)==0);
				iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH1);
				break;
				}

			case 1:
				{
				iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE2);
				iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS2);
				iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND2);
				iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC2)==0);
				iTestPhone->TEST(networkEntry.iNetworkId==DMMTSY_PHONE_CURRENT_NETWORK_ID2);
				iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG2)==0);
				iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2)==0);
				iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2)==0);
				iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH2);
				break;
				}

			case 2:
				{
				iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_HOME_NETWORK_MODE);
				iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_HOME_NETWORK_STATUS);
				iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_HOME_NETWORK_BAND);
				iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_HOME_NETWORK_MCC)==0);
				iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_HOME_NETWORK_ID)==0);
				iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_HOME_NETWORK_TAG)==0);
				iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME)==0);
				iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_HOME_NETWORK_LONG_NAME)==0);
				iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_LAST_USED_ACCESS_TECH);
				break;
				}

			case 3:
				{
				iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_HOME_NETWORK_MODE);
				iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_HOME_NETWORK_STATUS);
				iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_HOME_NETWORK_BAND);
				iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_HOME_NETWORK_MCC)==0);
				iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_HOME_NETWORK_ID)==0);
				iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_HOME_NETWORK_TAG)==0);
				iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME)==0);
				iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_HOME_NETWORK_LONG_NAME)==0);
				iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH3);
				break;
				}

			default:
				break;
			}
		}
	delete networkList;
	}
   return ret;
   }

TInt CTestPhone::CTestGetDetectedNetworks::CheckResultsV5()
    {
	TInt ret=KErrNone;

	CMobilePhoneNetworkListV5* networkList=NULL;

	TInt leaveCode=KErrNone;
	TRAP(leaveCode, networkList=iRetrieve->RetrieveListV5L(););

	//Check that a list is returned back to the test
	iTestPhone->TEST(networkList!=NULL);

	if (networkList)
	{
	//Check that the number of entries in the list is as expected
	iTestPhone->TEST(networkList->Enumerate()==DMMTSY_NETWORK_COUNT);

	// Get each detected network entry
	RMobilePhone::TMobilePhoneNetworkInfoV5 networkEntry;

	for (TInt i=0; i<DMMTSY_NETWORK_COUNT; ++i)
		{
		TRAP(ret,networkEntry=networkList->GetEntryL(i));
		if (ret != KErrNone)
			break;
		switch(i)
			{
			case 0:
				{
				iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE1);
				iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS1);
				iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND1);
				iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC1)==0);
				iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_CURRENT_NETWORK_ID1)==0);
				iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG1)==0);
				iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1)==0);
				iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1)==0);
				iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH1);
				iTestPhone->TEST(networkEntry.iHsdpaAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY1);
				iTestPhone->TEST(networkEntry.iEgprsAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY1);
				break;
				}

			case 1:
				{
				iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE2);
				iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS2);
				iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND2);
				iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC2)==0);
				iTestPhone->TEST(networkEntry.iNetworkId==DMMTSY_PHONE_CURRENT_NETWORK_ID2);
				iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG2)==0);
				iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2)==0);
				iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2)==0);
				iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH2);
				iTestPhone->TEST(networkEntry.iHsdpaAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY2);
				iTestPhone->TEST(networkEntry.iEgprsAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY2);
				break;
				}

			case 2:
				{
				iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_HOME_NETWORK_MODE);
				iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_HOME_NETWORK_STATUS);
				iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_HOME_NETWORK_BAND);
				iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_HOME_NETWORK_MCC)==0);
				iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_HOME_NETWORK_ID)==0);
				iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_HOME_NETWORK_TAG)==0);
				iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME)==0);
				iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_HOME_NETWORK_LONG_NAME)==0);
				iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_LAST_USED_ACCESS_TECH);
				iTestPhone->TEST(networkEntry.iHsdpaAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_HSDPA_AVAILABILITY);
				iTestPhone->TEST(networkEntry.iEgprsAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_EGPRS_AVAILABILITY);
				break;
				}

			case 3:
				{
				iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_HOME_NETWORK_MODE);
				iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_HOME_NETWORK_STATUS);
				iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_HOME_NETWORK_BAND);
				iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_HOME_NETWORK_MCC)==0);
				iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_HOME_NETWORK_ID)==0);
				iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_HOME_NETWORK_TAG)==0);
				iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME)==0);
				iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_HOME_NETWORK_LONG_NAME)==0);
				iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH3);
				iTestPhone->TEST(networkEntry.iHsdpaAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_HSDPA_AVAILABILITY);
				iTestPhone->TEST(networkEntry.iEgprsAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_EGPRS_AVAILABILITY);
				break;
				}

			default:
				break;
			}
		}
	delete networkList;
	}
   return ret;
   }
   
TInt CTestPhone::CTestGetDetectedNetworks::CheckResultsV8()
    {
	TInt ret=KErrNone;

	CMobilePhoneNetworkListV8* networkList=NULL;

	TRAPD(leaveCode, networkList=iRetrieve->RetrieveListV8L());
	if(leaveCode != KErrNone)
		{
		return leaveCode;
		}

	//Check that a list is returned back to the test
	iTestPhone->TEST(networkList!=NULL);

	if (networkList)
		{
		//Check that the number of entries in the list is as expected
		iTestPhone->TEST(networkList->Enumerate()==DMMTSY_NETWORK_COUNT);

		// Get each detected network entry
		RMobilePhone::TMobilePhoneNetworkInfoV8 networkEntry;

		for (TInt i=0; i<DMMTSY_NETWORK_COUNT; ++i)
			{
			TRAP(ret,networkEntry=networkList->GetEntryL(i));
			if (ret != KErrNone)
				{
				break;
				}
			switch(i)
				{
				case 0:
					{
					iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE1);
					iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS1);
					iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND1);
					iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC1)==0);
					iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_CURRENT_NETWORK_ID1)==0);
					iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG1)==0);
					iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1)==0);
					iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1)==0);
					iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH1);
					iTestPhone->TEST(networkEntry.iHsdpaAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY1);
					iTestPhone->TEST(networkEntry.iEgprsAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY1);
					iTestPhone->TEST(networkEntry.iHsupaAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_HSUPA_AVAILABILITY1);
					break;
					}

				case 1:
					{
					iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_CURRENT_NETWORK_MODE2);
					iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_CURRENT_NETWORK_STATUS2);
					iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_CURRENT_NETWORK_BAND2);
					iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_CURRENT_NETWORK_MCC2)==0);
					iTestPhone->TEST(networkEntry.iNetworkId==DMMTSY_PHONE_CURRENT_NETWORK_ID2);
					iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_CURRENT_NETWORK_TAG2)==0);
					iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2)==0);
					iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2)==0);
					iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH2);
					iTestPhone->TEST(networkEntry.iHsdpaAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY2);
					iTestPhone->TEST(networkEntry.iEgprsAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY2);
					iTestPhone->TEST(networkEntry.iHsupaAvailableIndicator==DMMTSY_PHONE_CURRENT_NETWORK_HSUPA_AVAILABILITY2);
					break;
					}

				case 2:
					{
					iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_HOME_NETWORK_MODE);
					iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_HOME_NETWORK_STATUS);
					iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_HOME_NETWORK_BAND);
					iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_HOME_NETWORK_MCC)==0);
					iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_HOME_NETWORK_ID)==0);
					iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_HOME_NETWORK_TAG)==0);
					iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME)==0);
					iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_HOME_NETWORK_LONG_NAME)==0);
					iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_LAST_USED_ACCESS_TECH);
					iTestPhone->TEST(networkEntry.iHsdpaAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_HSDPA_AVAILABILITY);
					iTestPhone->TEST(networkEntry.iEgprsAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_EGPRS_AVAILABILITY);
					iTestPhone->TEST(networkEntry.iHsupaAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_HSUPA_AVAILABILITY);
					break;
					}

				case 3:
					{
					iTestPhone->TEST(networkEntry.iMode==DMMTSY_PHONE_HOME_NETWORK_MODE);
					iTestPhone->TEST(networkEntry.iStatus==DMMTSY_PHONE_HOME_NETWORK_STATUS);
					iTestPhone->TEST(networkEntry.iBandInfo==DMMTSY_PHONE_HOME_NETWORK_BAND);
					iTestPhone->TEST(networkEntry.iCountryCode.Compare(DMMTSY_PHONE_HOME_NETWORK_MCC)==0);
					iTestPhone->TEST(networkEntry.iNetworkId.Compare(DMMTSY_PHONE_HOME_NETWORK_ID)==0);
					iTestPhone->TEST(networkEntry.iDisplayTag.Compare(DMMTSY_PHONE_HOME_NETWORK_TAG)==0);
					iTestPhone->TEST(networkEntry.iShortName.Compare(DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME)==0);
					iTestPhone->TEST(networkEntry.iLongName.Compare(DMMTSY_PHONE_HOME_NETWORK_LONG_NAME)==0);
					iTestPhone->TEST(networkEntry.iAccess==DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH3);
					iTestPhone->TEST(networkEntry.iHsdpaAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_HSDPA_AVAILABILITY);
					iTestPhone->TEST(networkEntry.iEgprsAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_EGPRS_AVAILABILITY);
					iTestPhone->TEST(networkEntry.iHsupaAvailableIndicator==DMMTSY_PHONE_HOME_NETWORK_HSUPA_AVAILABILITY);
					break;
					}

				default:
					break;
				}
			}
		delete networkList;
		}
   return ret;
   }

   
/**************************************************************/
//
// Testing asynchronous retrieve of call forwarding status list
//
/**************************************************************/

CTestPhone::CTestGetCFStatus* CTestPhone::CTestGetCFStatus::NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone)
	{
    CTestGetCFStatus* r=new(ELeave) CTestGetCFStatus(aPhone, aTestPhone);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestPhone::CTestGetCFStatus::CTestGetCFStatus(RMobilePhone& aPhone, CTestPhone* aTestPhone)
    : CActive(EPriorityNormal), iPhone(aPhone), iTestPhone(aTestPhone)
    {}
      

void CTestPhone::CTestGetCFStatus::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneCFList::NewL(iPhone);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestPhone::CTestGetCFStatus::~CTestGetCFStatus()
    {
    Cancel();
    delete iRetrieve;
	delete iCanceller;
	}
void CTestPhone::CTestGetCFStatus::StartSingleServiceGroup()
	{
	iRetrieve->Start(iStatus, DMMTSY_PHONE_CF_CONDITION_CFU, DMMTSY_PHONE_CF_SERVICE_GROUP_FAX, DMMTSY_PHONE_CF_CACHELOCATION);
	SetActive();
	iSingleServiceGroupTest = ETrue;
	}
      
void CTestPhone::CTestGetCFStatus::Start()
    {
    iRetrieve->Start(iStatus, DMMTSY_PHONE_CF_CONDITION_CFU, DMMTSY_PHONE_CF_LOCATION);
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus, DMMTSY_PHONE_CF_CONDITION_CFU, DMMTSY_PHONE_CF_LOCATION);
	User::WaitForRequest(tmpStatus);
	iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
	iSingleServiceGroupTest = EFalse;
    }       
      
CTestCanceller* CTestPhone::CTestGetCFStatus::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestPhone::CTestGetCFStatus::RunL()
    {
	TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
    if(iSingleServiceGroupTest)
		ret=CheckSingleServiceGroupResults();
	else
		ret=CheckResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}
      

void CTestPhone::CTestGetCFStatus::DoCancel()
    {
    iRetrieve->Cancel();
    }       
      

TInt CTestPhone::CTestGetCFStatus::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneCFList* callForwardingList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, callForwardingList=iRetrieve->RetrieveListL(););

	if (callForwardingList)
		{
		iTestPhone->TEST(callForwardingList->Enumerate()==DMMTSY_CF_ENTRY_COUNT);

		// Get each detected network entry
		RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;

		for (TInt i=0; i<DMMTSY_CALL_BARRING_COUNT; ++i)
			{
			TRAP(ret,entry=callForwardingList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestPhone->TEST(entry.iCondition==DMMTSY_PHONE_CF_CONDITION_CFU);
					iTestPhone->TEST(entry.iServiceGroup==DMMTSY_PHONE_CF_SERVICE_GROUP_VOICE);
					iTestPhone->TEST(entry.iStatus==DMMTSY_PHONE_CF_NOT_ACTIVE);
					iTestPhone->TEST(entry.iNumber.iTelNumber==DMMTSY_PHONE_TEL_NUMBER);
					iTestPhone->TEST(entry.iTimeout==DMMTSY_PHONE_CF_TIMEOUT);
					break;
					}
				case 1:
					{
					iTestPhone->TEST(entry.iCondition==DMMTSY_PHONE_CF_CONDITION_CFU);
					iTestPhone->TEST(entry.iServiceGroup==DMMTSY_PHONE_CF_SERVICE_GROUP_DATA);
					iTestPhone->TEST(entry.iStatus==DMMTSY_PHONE_CF_ACTIVE);
					iTestPhone->TEST(entry.iNumber.iTelNumber==DMMTSY_PHONE_TEL_NUMBER);
					iTestPhone->TEST(entry.iTimeout==DMMTSY_PHONE_CF_TIMEOUT);
					break;
					}
				case 2:
					{
					iTestPhone->TEST(entry.iCondition==DMMTSY_PHONE_CF_CONDITION_CFU);
					iTestPhone->TEST(entry.iServiceGroup==DMMTSY_PHONE_CF_SERVICE_GROUP_FAX);
					iTestPhone->TEST(entry.iStatus==DMMTSY_PHONE_CF_ACTIVE);
					iTestPhone->TEST(entry.iNumber.iTelNumber==DMMTSY_PHONE_TEL_NUMBER);
					iTestPhone->TEST(entry.iTimeout==DMMTSY_PHONE_CF_TIMEOUT);
					}
					break;
				case 3:
					{
					iTestPhone->TEST(entry.iCondition==DMMTSY_PHONE_CF_CONDITION_CFU);
					iTestPhone->TEST(entry.iServiceGroup==DMMTSY_PHONE_CF_SERVICE_GROUP_AUXVOICE);
					iTestPhone->TEST(entry.iStatus==DMMTSY_PHONE_CF_UNKNOWN);
					iTestPhone->TEST(entry.iTimeout==DMMTSY_NO_CF_TIMEOUT);
					}
					break;
				default:
					break;
				}
			}
		delete callForwardingList;
		}
	return ret;
   }
TInt CTestPhone::CTestGetCFStatus::CheckSingleServiceGroupResults()
	{
	TInt ret=KErrNone;

	CMobilePhoneCFList* callForwardingList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, callForwardingList=iRetrieve->RetrieveListL(););

	if (callForwardingList)
		{		
		iTestPhone->TEST(callForwardingList->Enumerate()==DMMTSY_CF_SINGLE_SERVICE_ENTRY_COUNT);
		
		// Get the detected network entry
		RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;
		TRAP(ret,entry=callForwardingList->GetEntryL(0));

		iTestPhone->TEST(entry.iCondition==DMMTSY_PHONE_CF_CONDITION_CFU);
		iTestPhone->TEST(entry.iServiceGroup==DMMTSY_PHONE_CF_SERVICE_GROUP_FAX);
		iTestPhone->TEST(entry.iStatus==DMMTSY_PHONE_CF_ACTIVE);
		iTestPhone->TEST(entry.iNumber.iTelNumber==DMMTSY_PHONE_TEL_NUMBER);
		iTestPhone->TEST(entry.iTimeout==DMMTSY_PHONE_CF_TIMEOUT);
		
		delete callForwardingList;
		}
	return ret;
	}

/**************************************************************/
//
// Testing asynchronous retrieve of call barring list
//
/**************************************************************/

CTestPhone::CTestGetCB* CTestPhone::CTestGetCB::NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone)
	{
    CTestGetCB* r=new(ELeave) CTestGetCB(aPhone, aTestPhone);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestPhone::CTestGetCB::CTestGetCB(RMobilePhone& aPhone, CTestPhone* aTestPhone)
    : CActive(EPriorityNormal), iPhone(aPhone), iTestPhone(aTestPhone)
    {}
      

void CTestPhone::CTestGetCB::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneCBList::NewL(iPhone);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestPhone::CTestGetCB::~CTestGetCB()
    {
    delete iRetrieve;
	delete iCanceller;
	}
      
void CTestPhone::CTestGetCB::Start()
    {
    iRetrieve->Start(iStatus, DMMTSY_PHONE_CB_CONDITION, DMMTSY_PHONE_CB_LOCATION);
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus, DMMTSY_PHONE_CB_CONDITION, DMMTSY_PHONE_CB_LOCATION);
	User::WaitForRequest(tmpStatus);
	iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
    }       
      
CTestCanceller* CTestPhone::CTestGetCB::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestPhone::CTestGetCB::RunL()
    {
	TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}
      

void CTestPhone::CTestGetCB::DoCancel()
    {
    iRetrieve->Cancel();
    }       
      

TInt CTestPhone::CTestGetCB::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneCBList* callBarringList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, callBarringList=iRetrieve->RetrieveListL(););

	if (callBarringList)
		{
		iTestPhone->TEST(callBarringList->Enumerate()==DMMTSY_CALL_BARRING_COUNT);

		// Get each detected network entry
		RMobilePhone::TMobilePhoneCBInfoEntryV1 entry;

		for (TInt i=0; i<DMMTSY_CALL_BARRING_COUNT; ++i)
			{
			TRAP(ret,entry=callBarringList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestPhone->TEST(entry.iCondition == DMMTSY_PHONE_CB_CONDITION0);
					iTestPhone->TEST(entry.iServiceGroup == DMMTSY_PHONE_CB_SG0);
					iTestPhone->TEST(entry.iStatus == DMMTSY_PHONE_CB_STATUS0);
					break;
					}
				case 1:
					{
					iTestPhone->TEST(entry.iCondition == DMMTSY_PHONE_CB_CONDITION1);
					iTestPhone->TEST(entry.iServiceGroup == DMMTSY_PHONE_CB_SG1);
					iTestPhone->TEST(entry.iStatus == DMMTSY_PHONE_CB_STATUS1);
					break;
					}
				case 2:
					{
					iTestPhone->TEST(entry.iCondition == DMMTSY_PHONE_CB_CONDITION2);
					iTestPhone->TEST(entry.iServiceGroup == DMMTSY_PHONE_CB_SG2);
					iTestPhone->TEST(entry.iStatus == DMMTSY_PHONE_CB_STATUS2);
					}
					break;
				case 3:
					{
					iTestPhone->TEST(entry.iCondition == DMMTSY_PHONE_CB_CONDITION3);
					iTestPhone->TEST(entry.iServiceGroup == DMMTSY_PHONE_CB_SG3);
					iTestPhone->TEST(entry.iStatus == DMMTSY_PHONE_CB_STATUS3);
					}
					break;
				default:
					break;
				}
			}
		delete callBarringList;
		}
	return ret;
   }

/**************************************************************/
//
// Testing asynchronous retrieve of call waiting list
//
/**************************************************************/

CTestPhone::CTestGetCW* CTestPhone::CTestGetCW::NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone)
	{
    CTestGetCW* r=new(ELeave) CTestGetCW(aPhone, aTestPhone);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestPhone::CTestGetCW::CTestGetCW(RMobilePhone& aPhone, CTestPhone* aTestPhone)
    : CActive(EPriorityNormal), iPhone(aPhone), iTestPhone(aTestPhone)
    {}
      

void CTestPhone::CTestGetCW::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneCWList::NewL(iPhone);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestPhone::CTestGetCW::~CTestGetCW()
    {
    delete iRetrieve;
	delete iCanceller;
	}
      
void CTestPhone::CTestGetCW::Start()
    {
    iRetrieve->Start(iStatus, DMMTSY_PHONE_CW_LOCATION);
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus, DMMTSY_PHONE_CW_LOCATION);
	User::WaitForRequest(tmpStatus);
	iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
    }       
      
CTestCanceller* CTestPhone::CTestGetCW::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestPhone::CTestGetCW::RunL()
    {
	TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}
      

void CTestPhone::CTestGetCW::DoCancel()
    {
    iRetrieve->Cancel();
    }       
      

TInt CTestPhone::CTestGetCW::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneCWList* callWaitingList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, callWaitingList=iRetrieve->RetrieveListL(););

	if (callWaitingList)
		{
		iTestPhone->TEST(callWaitingList->Enumerate()==DMMTSY_CW_ENTRY_COUNT);

		// Get each detected network entry
		RMobilePhone::TMobilePhoneCWInfoEntryV1 entry;

		for (TInt i=0; i<DMMTSY_CW_ENTRY_COUNT; ++i)
			{
			TRAP(ret,entry=callWaitingList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestPhone->TEST(entry.iServiceGroup == DMMTSY_PHONE_CW_SG0);
					iTestPhone->TEST(entry.iStatus == DMMTSY_PHONE_CW_STATUS0);
					break;
					}
				case 1:
					{
					iTestPhone->TEST(entry.iServiceGroup == DMMTSY_PHONE_CW_SG1);
					iTestPhone->TEST(entry.iStatus == DMMTSY_PHONE_CW_STATUS1);
					break;
					}
				case 2:
					{
					iTestPhone->TEST(entry.iServiceGroup == DMMTSY_PHONE_CW_SG2);
					iTestPhone->TEST(entry.iStatus == DMMTSY_PHONE_CW_STATUS2);
					}
					break;
				case 3:
					{
					iTestPhone->TEST(entry.iServiceGroup == DMMTSY_PHONE_CW_SG3);
					iTestPhone->TEST(entry.iStatus == DMMTSY_PHONE_CW_STATUS3);
					}
					break;
				default:
					break;
				}
			}
		delete callWaitingList;
		}
	return ret;
   }

/**************************************************************/
//
// Testing asynchronous retrieve of call completion list
//
/**************************************************************/

CTestPhone::CTestGetCcbs* CTestPhone::CTestGetCcbs::NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone)
	{
    CTestGetCcbs* r=new(ELeave) CTestGetCcbs(aPhone, aTestPhone);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestPhone::CTestGetCcbs::CTestGetCcbs(RMobilePhone& aPhone, CTestPhone* aTestPhone)
    : CActive(EPriorityNormal), iPhone(aPhone), iTestPhone(aTestPhone)
    {}
      

void CTestPhone::CTestGetCcbs::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneCcbsList::NewL(iPhone);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestPhone::CTestGetCcbs::~CTestGetCcbs()
    {
    delete iRetrieve;
	delete iCanceller;
	}
      
void CTestPhone::CTestGetCcbs::Start()
    {
    iRetrieve->Start(iStatus);
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus);
	User::WaitForRequest(tmpStatus);
	iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
    }       
      
CTestCanceller* CTestPhone::CTestGetCcbs::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestPhone::CTestGetCcbs::RunL()
    {
	TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}
      

void CTestPhone::CTestGetCcbs::DoCancel()
    {
    iRetrieve->Cancel();
    }       
      

TInt CTestPhone::CTestGetCcbs::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneCcbsList* callCompletionList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, callCompletionList=iRetrieve->RetrieveListL(););

	if (callCompletionList)
		{
		iTestPhone->TEST(callCompletionList->Enumerate()==DMMTSY_PHONE_CCC_ENTRY_COUNT);

		// Get each detected completion request
		RMobilePhone::TMobilePhoneCCBSEntryV1 entry;

		for (TInt i=0; i<DMMTSY_PHONE_CCC_ENTRY_COUNT; ++i)
			{
			TRAP(ret,entry=callCompletionList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestPhone->TEST(entry.iServiceGroup==DMMTSY_PHONE_CCC_SERVICE_GROUP0);
					iTestPhone->TEST(entry.iCcbsIndex == DMMTSY_PHONE_CCC_INDEX0);
					iTestPhone->TEST(entry.iDestination.iTypeOfNumber == DMMTSY_PHONE_CCC_DESTINATION_TON0);
					iTestPhone->TEST(entry.iDestination.iNumberPlan == DMMTSY_PHONE_CCC_DESTINATION_NP0);
					iTestPhone->TEST(entry.iDestination.iTelNumber.Compare(DMMTSY_PHONE_CCC_DESTINATION_NUM0) == 0);
					break;
					}
				case 1:
					{
					iTestPhone->TEST(entry.iServiceGroup==DMMTSY_PHONE_CCC_SERVICE_GROUP1);
					iTestPhone->TEST(entry.iCcbsIndex == DMMTSY_PHONE_CCC_INDEX1);
					iTestPhone->TEST(entry.iDestination.iTypeOfNumber == DMMTSY_PHONE_CCC_DESTINATION_TON1);
					iTestPhone->TEST(entry.iDestination.iNumberPlan == DMMTSY_PHONE_CCC_DESTINATION_NP1);
					iTestPhone->TEST(entry.iDestination.iTelNumber.Compare(DMMTSY_PHONE_CCC_DESTINATION_NUM1) == 0);
					break;
					}
				case 2:
					{
					iTestPhone->TEST(entry.iServiceGroup==DMMTSY_PHONE_CCC_SERVICE_GROUP2);
					iTestPhone->TEST(entry.iCcbsIndex == DMMTSY_PHONE_CCC_INDEX2);
					iTestPhone->TEST(entry.iDestination.iTypeOfNumber == DMMTSY_PHONE_CCC_DESTINATION_TON2);
					iTestPhone->TEST(entry.iDestination.iNumberPlan == DMMTSY_PHONE_CCC_DESTINATION_NP2);
					iTestPhone->TEST(entry.iDestination.iTelNumber.Compare(DMMTSY_PHONE_CCC_DESTINATION_NUM2) == 0);
					}
					break;
				case 3:
					{
					iTestPhone->TEST(entry.iServiceGroup==DMMTSY_PHONE_CCC_SERVICE_GROUP3);
					iTestPhone->TEST(entry.iCcbsIndex == DMMTSY_PHONE_CCC_INDEX3);
					iTestPhone->TEST(entry.iDestination.iTypeOfNumber == DMMTSY_PHONE_CCC_DESTINATION_TON3);
					iTestPhone->TEST(entry.iDestination.iNumberPlan == DMMTSY_PHONE_CCC_DESTINATION_NP3);
					iTestPhone->TEST(entry.iDestination.iTelNumber.Compare(DMMTSY_PHONE_CCC_DESTINATION_NUM3) == 0);
					}
					break;
				default:
					break;
				}
			}
		delete callCompletionList;
		}
	return ret;
   }
   
/**************************************************************/
//
// Testing asynchronous retrieve of preferred stored network list
//
/**************************************************************/


CTestPhone::CTestGetPreferredNetworks* CTestPhone::CTestGetPreferredNetworks::NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone)
	{
    CTestGetPreferredNetworks* r=new(ELeave) CTestGetPreferredNetworks(aPhone, aTestPhone);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestPhone::CTestGetPreferredNetworks::CTestGetPreferredNetworks(RMobilePhone& aPhone, CTestPhone* aTestPhone)
    : CActive(EPriorityNormal), iPhone(aPhone),iTestPhone(aTestPhone)
    {}
      

void CTestPhone::CTestGetPreferredNetworks::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhonePreferredNetworks::NewL(iPhone);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestPhone::CTestGetPreferredNetworks::~CTestGetPreferredNetworks()
    {
    delete iRetrieve;
	delete iCanceller;
	}
      
void CTestPhone::CTestGetPreferredNetworks::Start()
    {
    iRetrieve->Start(iStatus);
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus);
	User::WaitForRequest(tmpStatus);
	iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
    }       
      
CTestCanceller* CTestPhone::CTestGetPreferredNetworks::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestPhone::CTestGetPreferredNetworks::RunL()
    {
	TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResults();
	User::LeaveIfError(ret);
    ret=CheckPrefNetworksErrorResultsL();  
	User::LeaveIfError(ret);
    CActiveScheduler::Stop();
	}
      

void CTestPhone::CTestGetPreferredNetworks::DoCancel()
    {
    iRetrieve->Cancel();
    }       
      
TInt CTestPhone::CTestGetPreferredNetworks::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneStoredNetworkList* networkList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, networkList=iRetrieve->RetrieveListL(););

	if (networkList)
		{
		iTestPhone->TEST(networkList->Enumerate()==DMMTSY_PHONE_NTWK_LIST_ENTRIES);

		// Get each stored network entry
		RMobilePhone::TMobilePreferredNetworkEntryV3 entry;

		for (TInt i=0; i<DMMTSY_PHONE_NTWK_LIST_ENTRIES; ++i)
			{
			TRAP(ret,entry=networkList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestPhone->TEST(entry.iAccess == DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM);
					iTestPhone->TEST(entry.iUserDefined == DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE);
					iTestPhone->TEST(entry.iCountryCode == DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID1);
					iTestPhone->TEST(entry.iNetworkId == DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1);
					}
					break;
				case 1:
					{
					iTestPhone->TEST(entry.iAccess == DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSMC);
					iTestPhone->TEST(entry.iUserDefined == DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE);
					iTestPhone->TEST(entry.iCountryCode == DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2);
					iTestPhone->TEST(entry.iNetworkId == DMMTSY_PHONE_NTWK_LIST_NETWORK_ID2);
					}
					break;
				case 2:
				default:
					{
					iTestPhone->TEST(entry.iAccess == DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_UTRAN);
					iTestPhone->TEST(entry.iUserDefined == DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE);
					iTestPhone->TEST(entry.iCountryCode == DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2);
					iTestPhone->TEST(entry.iNetworkId == DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1);
					}
					break;
				case 3:
					{
					iTestPhone->TEST(entry.iAccess == DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_WLAN);
					iTestPhone->TEST(entry.iUserDefined == DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE);
					iTestPhone->TEST(entry.iCountryCode == DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2);
					iTestPhone->TEST(entry.iNetworkId == DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1);
					break;
					}
				}
			}
		delete networkList;
		}
   return ret;
    }
TInt CTestPhone::CTestGetPreferredNetworks::CheckPrefNetworksErrorResultsL()
{

	CMobilePhoneStoredNetworkList* networkList=NULL;
	TInt leaveCode=KErrNone;
	CRetrieveMobilePhonePreferredNetworks* preferredNetworksList = CRetrieveMobilePhonePreferredNetworks::NewL(iPhone);
	CleanupStack::PushL(preferredNetworksList);
	TRAP(leaveCode, networkList=iRetrieve->RetrieveListL(););
	if(networkList == NULL)
	    {
	    iTestPhone->INFO_PRINTF1(_L("preferredNetworksList->RetrieveListL() returned NULL."));
	    }
    CleanupStack::PopAndDestroy();
	//Check that a list is returned back to the test
	if(leaveCode == KErrNotFound)
		{
		leaveCode = KErrNone;
		}
	return leaveCode;
    }
 
CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds* CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::NewLC(RMobilePhone& aPhone,CTestPhone* aTestPhone)
	{
	CTestRetrieveMobilePhoneAuthenticationIds* r=new(ELeave) CTestRetrieveMobilePhoneAuthenticationIds(aPhone,aTestPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
	return r;
	}
  
CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::CTestRetrieveMobilePhoneAuthenticationIds(RMobilePhone& aPhone,CTestPhone* aTestPhone)
: CActive(EPriorityNormal), iPhone(aPhone), iTestPhone(aTestPhone)
	{}
 
void CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::ConstructL()
	{
	CActiveScheduler::Add(this);

	iRetrieveAuthId=CRetrieveMobilePhoneAuthenticationIds::NewL(iPhone);
	iCanceller=new (ELeave) CTestCanceller(this);
	}
     
CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::~CTestRetrieveMobilePhoneAuthenticationIds()
	{
	delete iRetrieveAuthId;
	delete iCanceller;
	}

void CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::SetAuthType(TTypeOfAuth aAuthType)
	{
	iAuthType = aAuthType;
	if(iAuthType == 0)
		{
		iTestPhone->INFO_PRINTF1(_L("Authentication Type is GBA..."));
		}
	else if (iAuthType == 1)
		{
		iTestPhone->INFO_PRINTF1(_L("Authentication Type is MBMS..."));
		}
	else
		{}
	}

void CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::TestRetrieve(RMobilePhone::TAuthData aAuthData,TBool aStartReqd)
	{
	if(aStartReqd)
		{
		Start(aAuthData);
		}
	CActiveScheduler::Start();
	}

void CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::Start(RMobilePhone::TAuthData aAuthData)
	{
	//If iTestVersion is set to EGba, test calling the Gba retrieval method
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckgd(tAuthInfoV8);
	
	tAuthInfoV8.iAID = DMMTSY_PHONE_GBA_AID();

	if(iAuthType==CTestRetrieveMobilePhoneAuthenticationIds::EGba)
		{
		tAuthInfoV8.iData = aAuthData;
		iRetrieveAuthId->Start(iStatus,tAuthInfoV8Pckgd);
		TRequestStatus tmpStatus;
		iRetrieveAuthId->Start(tmpStatus,tAuthInfoV8Pckgd);
		User::WaitForRequest(tmpStatus);
		iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
		SetActive();
		}
	//If iTestVersion is set to EMbms, test calling the Mbms retrieval method
	else if(iAuthType==CTestRetrieveMobilePhoneAuthenticationIds::EMbms)
		{
		tAuthInfoV8.iData = aAuthData;
		iRetrieveAuthId->Start(iStatus,tAuthInfoV8Pckgd);
		TRequestStatus tmpStatus;
		iRetrieveAuthId->Start(tmpStatus,tAuthInfoV8Pckgd);
		User::WaitForRequest(tmpStatus);
		iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
		SetActive();
		}
	else
		{}
	}       

void CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::RunL()
	{
	TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	if (iAuthType==EGba)
	{
		ret=CheckGBAResults();
  	    User::LeaveIfError(ret);
	    ret=CheckGBAErrorResults();
	}
	else if(iAuthType==EMbms)
	{
		ret=CheckMBMSResults();	
		User::LeaveIfError(ret);
		ret=CheckMBMSErrorResults();
	}
	
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
 	}

void CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::TestCancel(RMobilePhone::TAuthData aAuthData)
	{
	Start(aAuthData);

	CTestCanceller* cancelGetNetworks = RetrieveCancelPtr();
	cancelGetNetworks->Call();

	CActiveScheduler::Start();
	TBool cancelled = (iStatus == KErrCancel);
	
	//Ensure that the same AO can be utilised further.
	Start(aAuthData);
	CActiveScheduler::Start();

	if (cancelled)
		{
		iTestPhone->INFO_PRINTF2(_L("Cancelled Type of Authentication is %d"),iAuthType);
		iTestPhone->INFO_PRINTF1(_L("Test - CRetrieveMobilePhoneAuthenticationIds (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		iTestPhone->INFO_PRINTF1(_L("Test - CRetrieveMobilePhoneAuthenticationIds (async & cancel (Request Not Cancelled)) passed"));
		}	
	}      

CTestCanceller* CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::RetrieveCancelPtr()
	{
	return iCanceller;
	}
  
void CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::DoCancel()
	{
	iRetrieveAuthId->Cancel();
	}       
  
TInt CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::CheckGBAResults()
	{
	TInt ret=KErrNone;

	CMobilePhoneGbaNafIdList* nafList =NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, nafList=iRetrieveAuthId->RetrieveGbaListL());

	//Check that a list is returned back to the test
	iTestPhone->TEST(nafList!=NULL);

	if (nafList)
		{
		//Check that the number of entries in the list is as expected
		iTestPhone->TEST(nafList->Enumerate()==DMMTSY_PHONE_GBA_NAFLIST_COUNT);
		
		// Get each NafList entry
		RMobilePhone::TGbaNafEntryV8 nafEntry;
		
		for (TInt i=0; i<DMMTSY_PHONE_GBA_NAFLIST_COUNT; ++i)
			{
			TRAP(ret,nafEntry=nafList->GetEntryL(i));
			if (ret != KErrNone)
				{
				break;
				}
			switch(i)
				{
			case 0:
				{
				iTestPhone->TEST(nafEntry.iBtid == DMMTSY_PHONE_GBA_NAFLIST_BTID_ONE);
				iTestPhone->TEST(nafEntry.iNafId == DMMTSY_PHONE_GBA_NAFLIST_NAFID_ONE);
				break;
				}
			case 1:
				{
				iTestPhone->TEST(nafEntry.iBtid == DMMTSY_PHONE_GBA_NAFLIST_BTID_TWO);
				iTestPhone->TEST(nafEntry.iNafId == DMMTSY_PHONE_GBA_NAFLIST_NAFID_TWO);
				break;
				}
			case 2:
				{
				iTestPhone->TEST(nafEntry.iBtid == DMMTSY_PHONE_GBA_NAFLIST_BTID_THREE);
				iTestPhone->TEST(nafEntry.iNafId == DMMTSY_PHONE_GBA_NAFLIST_NAFID_THREE);
				break;
				}
			default:
				break;
				}
			}
	delete nafList;
		}
	return ret;
	}

TInt CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::CheckGBAErrorResults()
	{

	CMobilePhoneGbaNafIdList* nafList =NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, nafList=iRetrieveAuthId->RetrieveGbaListL());
    if(nafList == NULL)
        {
        iTestPhone->INFO_PRINTF1(_L("iRetrieveAuthId->RetrieveGbaListL() returned NULL."));
        }
    	
	//Check that a list is returned back to the test
	if(leaveCode == KErrNotFound)
		{
		leaveCode = KErrNone;
		}
	return leaveCode;
	}

TInt CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::CheckMBMSResults()
	{
	TInt ret=KErrNone;

	CMobilePhoneMbmsMskIdList* mbmsList =NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, mbmsList=iRetrieveAuthId->RetrieveMbmsListL());

	//Check that a list is returned back to the test
	iTestPhone->TEST(mbmsList!=NULL);

	if (mbmsList)
	{
	//Check that the number of entries in the list is as expected
	iTestPhone->TEST(mbmsList->Enumerate()==DMMTSY_PHONE_MBMS_LIST_COUNT);

	// Get each MbmsList entry
	RMobilePhone::TMskEntryV8 mbmsEntry;

	for (TInt i=0; i<DMMTSY_PHONE_MBMS_LIST_COUNT; ++i)
		{
		TRAP(ret,mbmsEntry=mbmsList->GetEntryL(i));
		if (ret != KErrNone)
			{
			break;
			}
		switch(i)
			{
		case 0:
			{
			iTestPhone->TEST(mbmsEntry.iKeyDomainId == DMMTSY_PHONE_MBMSLIST_KEYDMN_ONE);
			iTestPhone->TEST(mbmsEntry.iMskId == DMMTSY_PHONE_MBMSLIST_MSKID_ONE);
			iTestPhone->TEST(mbmsEntry.iTimeStampCounter == DMMTSY_PHONE_MBMSLIST_TIMESTM_ONE );
			break;
			}
		case 1:
			{
			iTestPhone->TEST(mbmsEntry.iKeyDomainId == DMMTSY_PHONE_MBMSLIST_KEYDMN_TWO);
			iTestPhone->TEST(mbmsEntry.iMskId == DMMTSY_PHONE_MBMSLIST_MSKID_TWO);
			iTestPhone->TEST(mbmsEntry.iTimeStampCounter == DMMTSY_PHONE_MBMSLIST_TIMESTM_TWO);
			break;
			}
		case 2:
			{
			iTestPhone->TEST(mbmsEntry.iKeyDomainId == DMMTSY_PHONE_MBMSLIST_KEYDMN_THREE);
			iTestPhone->TEST(mbmsEntry.iMskId == DMMTSY_PHONE_MBMSLIST_MSKID_THREE);
			iTestPhone->TEST(mbmsEntry.iTimeStampCounter == DMMTSY_PHONE_MBMSLIST_TIMESTM_THREE);
			break;
			}
		default:
			break;
			}
		}
	delete mbmsList;
	}
	return ret;
	}
	
TInt CTestPhone::CTestRetrieveMobilePhoneAuthenticationIds::CheckMBMSErrorResults()
	{

	CMobilePhoneMbmsMskIdList* mbmsList =NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, mbmsList=iRetrieveAuthId->RetrieveMbmsListL());
    if(mbmsList == NULL)
        {
        iTestPhone->INFO_PRINTF1(_L("iRetrieveAuthId->RetrieveMbmsListL() returned NULL."));
        }
    	
	//Check that a list is returned back to the test
	if(leaveCode == KErrNotFound)
		{
		leaveCode = KErrNone;
		}
	return leaveCode;
	}		
	
CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs* CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::NewLC(RMobilePhone& aPhone,CTestPhone* aTestPhone)
	{
	CTestRetrieveMobilePhonePreferredWlanSIDs* r=new(ELeave) CTestRetrieveMobilePhonePreferredWlanSIDs(aPhone,aTestPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
	return r;
	}
  
CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::CTestRetrieveMobilePhonePreferredWlanSIDs(RMobilePhone& aPhone,CTestPhone* aTestPhone)
: CActive(EPriorityNormal), iPhone(aPhone),iTestPhone(aTestPhone)
{}
 
void CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::ConstructL()
	{
	CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhonePreferredWlanSIDs::NewL(iPhone);
	iCanceller=new (ELeave) CTestCanceller(this);
	}
     
CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::~CTestRetrieveMobilePhonePreferredWlanSIDs()
	{
	delete iRetrieve;
	delete iCanceller;
	}

void CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::TestRetrieve(TBool aStartReqd)
	{
	if(aStartReqd)
		{
		Start();
		}
	CActiveScheduler::Start();
	}

void CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::Start()
	{
	iRetrieve->Start(iStatus);
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus);
	User::WaitForRequest(tmpStatus);
	iTestPhone->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
	}       

void CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::RunL()
	{
	TInt ret=iStatus.Int();
	User::LeaveIfError(ret);
	ret=CheckWLANResults();
	User::LeaveIfError(ret);        
	ret=CheckWLANErrorResultsL();       
	User::LeaveIfError(ret);
	CActiveScheduler::Stop();
	}

void CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::TestCancel()
	{
	Start();
	CTestCanceller* cancelGetNetworks = RetrieveCancelPtr();
	cancelGetNetworks->Call();

	CActiveScheduler::Start();
	TBool cancelled = (iStatus == KErrCancel);

	// Ensure that AO can be used after its previous transaction was cancelled
	Start();
	CActiveScheduler::Start();

	if (cancelled)
		{
		iTestPhone->INFO_PRINTF1(_L("Cancelled WLAN Preferred List"));
		iTestPhone->INFO_PRINTF1(_L("Test - CRetrieveMobilePhoneAuthenticationIds (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		iTestPhone->INFO_PRINTF1(_L("Test - CRetrieveMobilePhoneAuthenticationIds (async & cancel (Request Not Cancelled)) passed"));
		}	
	}      

CTestCanceller* CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::RetrieveCancelPtr()
	{
	return iCanceller;
	}
  
void CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::DoCancel()
	{
	iRetrieve->Cancel();
	}       
  
TInt CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::CheckWLANResults()
	{
	TInt ret=KErrNone;

	CMobilePhoneStoredWlanSIDList* wlanList =NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, wlanList=iRetrieve->RetrieveListL(););

	//Check that a list is returned back to the test
	iTestPhone->TEST(wlanList!=NULL);

	if (wlanList)
		{
		//Check that the number of entries in the list is as expected
		iTestPhone->TEST(wlanList->Enumerate()==DMMTSY_PHONE_WLAN_SIDLIST_COUNT);
		// Get each entry
		RMobilePhone::TWlanSIDV8 tWlanSIDV8;
		for (TInt i=0; i<DMMTSY_PHONE_WLAN_SIDLIST_COUNT; ++i)
			{
			TRAP(ret,tWlanSIDV8=wlanList->GetEntryL(i));
			if (ret != KErrNone)
				{
				break;
				}
			switch(i)
				{
			case 0:
				{
				iTestPhone->TEST(tWlanSIDV8.iWSID == DMMTSY_PHONE_WLAN_WSID_ONE);
				iTestPhone->TEST(tWlanSIDV8.iUserDefined == DMMTSY_PHONE_WLAN_USERDEFD_ONE);
				break;
				}
			case 1:
				{
				iTestPhone->TEST(tWlanSIDV8.iWSID == DMMTSY_PHONE_WLAN_WSID_TWO);
				iTestPhone->TEST(tWlanSIDV8.iUserDefined == DMMTSY_PHONE_WLAN_USERDEFD_TWO);
				break;
				}
			case 2:
				{
				iTestPhone->TEST(tWlanSIDV8.iWSID == DMMTSY_PHONE_WLAN_WSID_THREE);
				iTestPhone->TEST(tWlanSIDV8.iUserDefined == DMMTSY_PHONE_WLAN_USERDEFD_THREE);
				break;
				}
			default:
				break;
				}
			}
		delete wlanList;
		}
	return ret;
	}
TInt CTestPhone::CTestRetrieveMobilePhonePreferredWlanSIDs::CheckWLANErrorResultsL()
	{
	
	CMobilePhoneStoredWlanSIDList* wlanList =NULL;
	TInt leaveCode=KErrNone;
	CRetrieveMobilePhonePreferredWlanSIDs* retrieveMbmsMonitorList = CRetrieveMobilePhonePreferredWlanSIDs::NewL(iPhone);
	CleanupStack::PushL(retrieveMbmsMonitorList);
	TRAP(leaveCode, wlanList=iRetrieve->RetrieveListL(););
    if(wlanList == NULL)
        {
        iTestPhone->INFO_PRINTF1(_L("iRetrieve->RetrieveListL() returned NULL."));
        }
	CleanupStack::PopAndDestroy();
	
	//Check that a list is returned back to the test
	if(leaveCode == KErrNotFound)
		{
		leaveCode = KErrNone;
		}
	
	return leaveCode;
	}		
