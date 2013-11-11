// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_SimMiscTest.h"

#define KSimTsyVersionTestOffset	30
_LIT8(KConfigScAuthAUTN, "AUTN_IN");
_LIT8(KConfigScAuthRAND, "RAND_IN");

CSimMiscTest::CSimMiscTest() 
	{ 
	SetTestStepName(_L("MiscTest"));
	}

TVerdict CSimMiscTest::doTestStepL()
	{
	INFO_PRINTF1(_L("Misc Functionality"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	
	TBool result;
	TBuf<10> tsyName;
	tsyName = KSimtsyName;
	INFO_PRINTF1(_L("Tesing Is Supported functionality"));
	for (TInt mixin=KETelExtMultimodeV1; mixin<=KETelFuncMobileUserSignalling;mixin++)
		{	
		iTelServer.IsSupportedByModule(tsyName, mixin, result);
		if (mixin== KETelExtMultimodeV1
			|| mixin==KETelFuncMobileNetwork
			|| mixin==KETelFuncMobilePower
			|| mixin==KETelFuncMobileSignal
			|| mixin==KETelFuncMobileIndicator
			|| mixin==KETelFuncMobileDTMF
			|| mixin==KETelFuncMobileDataCall
			|| mixin==KETelFuncMobileEmergencyCall
			|| mixin==KETelFuncMobileSmsMessaging
			|| mixin==KETelFuncMobilePhonebook
			|| mixin==KETelFuncMobileSmsStore
			|| mixin==KETelFuncMobileOwnNumberStore
			|| mixin==KETelExtPcktV1
			|| mixin==RMobileCall::KETelMobileDataCallParamsV1
			|| mixin==RMobileCall::KETel3rdPartyCallParamsV1			
			)
				TESTL(result!=EFalse);
		else 
				TESTL(result==EFalse);
		
		}
	
	iTelServer.IsSupportedByModule(tsyName, KEtelExtMultimodeV9, result);
	TESTL(result!=EFalse);
	iTelServer.IsSupportedByModule(tsyName, KEtelFuncCellInfo, result);
	TESTL(result!=EFalse);

	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

//------------------------------------------------------------------

CSetVersion::CSetVersion()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SetVersion"));
	}
//------------------------------------------------------------------

TVerdict CSetVersion::doTestStepL( void )
	{
    INFO_PRINTF1(_L("Testing the simulated version."));

	CreateConfigFileL(_L("c:\\config3.txt"));

	for(TInt version = 1; version<=6; version++)
		{
		SetTestNumberL(version + KSimTsyVersionTestOffset);
		TestVersion(version);
		}
	
	return TestStepResult();
	}

void CSetVersion::TestVersion(TInt aVersion)
	{
	// Closing and re-opening the phone (to refresh the version setting)
 	iPhone.Close();
	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TRequestStatus stat0;

	// Testing a V1 data structure.
	TUint32 capabilities;
	RMobilePhone::TMobilePhoneBatteryInfoV1 battery;
	INFO_PRINTF1(_L("Testing a V1 data structure."));
	ret = iPhone.GetBatteryCaps(capabilities);
	TESTL(ret==KErrNone); // must always pass - there is no version number lower than 1;

	// Testing a V2 data structure.
 	RMobilePhone::TUSimApplicationInfoV2 usimAppInfo;
	RMobilePhone::TUSimApplicationInfoV2Pckg usimAppInfoPckg(usimAppInfo);
	INFO_PRINTF1(_L("Testing a V2 data structure."));
	iPhone.GetUSimApplicationInfo(stat0, 0, usimAppInfoPckg);
  	User::WaitForRequest(stat0);
	if(aVersion >=2)
		{
		TESTL(stat0==KErrNone);
		}
	else
		{
		TESTL(stat0==KErrNotSupported);
		}
  	
  	
 	// Testing a V3 data structure.
 	RMobilePhone::TMobilePhoneNetworkNameV3 networkname;
	RMobilePhone::TMobilePhoneNetworkNameV3Pckg networknamePckg(networkname);
	RMobilePhone::TMobilePhoneOPlmnV3 locInfo;
	RMobilePhone::TMobilePhoneOPlmnV3Pckg locInfoPckg(locInfo);
	INFO_PRINTF1(_L("Testing a V3 data structure."));
	iPhone.GetCurrentNetworkName(stat0, networknamePckg, locInfoPckg);
	User::WaitForRequest(stat0);
	if(aVersion >=3)
		{
		TESTL(stat0==KErrNone);
		}
	else
		{
		TESTL(stat0==KErrNotSupported);
		}
  	
 	// Testing a V5 data structure.
 	RMobilePhone::TSmartCardApplicationInfoV5 smartCardAppInfo;
	RMobilePhone::TSmartCardApplicationInfoV5Pckg smartCardAppInfoPckg(smartCardAppInfo);
	INFO_PRINTF1(_L("Testing a V5 data structure."));
	iPhone.GetSmartCardApplicationInfo(stat0, 0, smartCardAppInfoPckg);
	User::WaitForRequest(stat0);
	if(aVersion >=5)
		{
		TESTL(stat0==KErrNone);
		}
	else
		{
		TESTL(stat0==KErrNotSupported);
		}

 	// Testing a V6 data structure.
	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData;
	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckg(authenticateData);
	authenticateData.iAUTN = KConfigScAuthAUTN();
	authenticateData.iRAND = KConfigScAuthRAND();
	INFO_PRINTF1(_L("Testing a V6 data structure."));
	iPhone.SmartCardAuthenticate(stat0, authenticateDataPckg);
	User::WaitForRequest(stat0);
	if(aVersion >=6)
		{
		TESTL(stat0==KErrNone);
		}
	else
		{
		TESTL(stat0==KErrNotSupported);
		}

	
	iPhone.Close();
	}
	
//------------------------------------------------------------------

CCheckVersionSupport::CCheckVersionSupport()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name hat is used by the script file
	SetTestStepName(_L("CCheckVersionSupport"));
	}
//------------------------------------------------------------------

TVerdict CCheckVersionSupport::doTestStepL( void )
	{
    INFO_PRINTF1(_L("Testing the support of the simulated version."));

	CreateConfigFileL(_L("c:\\config3.txt"));

	for(TInt version = 1; version<=6; version++)
		{
		SetTestNumberL(version + KSimTsyVersionTestOffset);
		TestVersionSupport(version);
		}

	return TestStepResult();
	}
	
	
void CCheckVersionSupport::TestVersionSupport(TInt aSimTsyVersion)
	{
	TInt mmBaseClassVersion = KETelExtMultimodeV1;
	TBool isVer;

	const TInt mmVersion[7] = 
		{
		0,
		KETelExtMultimodeV1,
		KETelExtMultimodeV2,
		KETelExtMultimodeV3,
		KETelExtMultimodeV4,
		KEtelExtMultimodeV5,
		KEtelExtMultimodeV6
		};
	
	// Closing and re-opening the phone (to refresh the version setting)
 	iPhone.Close();
	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	for(TInt version = 1; version<=6; version++)
		{
		// setting the version to check
		mmBaseClassVersion = mmVersion[version];
		
		// checking support
		INFO_PRINTF2(_L("Testing KETelExtMultimodeV%d."), version);
		iTelServer.IsSupportedByModule(KSimtsyName,mmBaseClassVersion,isVer);

		if(version <= aSimTsyVersion)
			{
			TEST(isVer);
			}
		else
			{
			TEST(!isVer);
			}
		}
		
	// close the phone (will be opened again on the next iteration)
	iPhone.Close();
	}
