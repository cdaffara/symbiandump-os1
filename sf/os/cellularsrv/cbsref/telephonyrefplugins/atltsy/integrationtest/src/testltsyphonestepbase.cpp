// TestTelephonyPktTestStepBase.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonyPktTestStepBase
#include <mmlist.h>
#include <mmretrieve.h>
// 	user include
#include "TestLtsyPhoneStepBase.h"
#include "te_integration_stltsysuitestepbase.h"
#include "te_integration_stltsysuitedefs.h"

// 
_LIT(KTsyModuleName, "PhoneTsy");
/**
 * Constructor
 */
CTestLtsyPhoneStepBase::CTestLtsyPhoneStepBase(CTestLtsyModel& aModel)
	:iModel(aModel),iSharedPhone(aModel.SharedEnv().Phone())
	{
	}

/**
 * Destructor
 * 
 */
CTestLtsyPhoneStepBase::~CTestLtsyPhoneStepBase()
	{

	}
/**
 * Get the shared phone
 * 
 */
RMobilePhone& CTestLtsyPhoneStepBase::SharedPhone() 
	{
	return iSharedPhone;
	}
	
TVerdict CTestLtsyPhoneStepBase::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhoneStepBase::doTestStepPreambleL()"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_integration_stltsySuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}
TVerdict CTestLtsyPhoneStepBase::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhoneStepBase::doTestStepPostambleL()"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_integration_stltsySuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
void CTestLtsyPhoneStepBase::TestOpenPhoneL()
	{
	// reset all shared data to make sure we are going to run on a clean environment
	//Open the shared phone
	iModel.SharedEnv().OpenPhoneL(KTsyModuleName);
	}
void CTestLtsyPhoneStepBase::TestClosePhone()
	{
	iModel.SharedEnv().ClosePhone();	
	}

void CTestLtsyPhoneStepBase::GetFdnStatus()
	{
	INFO_PRINTF1(_L("GetFdnStatus() invoked......\n"));
	TRequestStatus Status;
	RMobilePhone::TMobilePhoneFdnStatus FdnStatus;
	iSharedPhone.GetFdnStatus(Status,FdnStatus);
	User::WaitForRequest(Status);
	INFO_PRINTF2(_L("GetFdnStatus() complete = %d\n"), Status.Int());
	}
void CTestLtsyPhoneStepBase::GetNetworkRegistrationStatus()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("GetNetworkRegistrationStatus() invoked......\n"));
	RMobilePhone::TMobilePhoneRegistrationStatus RegStatus;
	iSharedPhone.GetNetworkRegistrationStatus(Status,RegStatus);
	User::WaitForRequest(Status);
	INFO_PRINTF2(_L("GetNetworkRegistrationStatus() complete = %d\n"), Status.Int());
	}
void CTestLtsyPhoneStepBase::GetHomeNetwork()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("GetHomeNetwork() invoked......\n"));
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;
	iSharedPhone.GetHomeNetwork(Status, aNetworkInfoBuf) ;
	User::WaitForRequest(Status);
	INFO_PRINTF2(_L("GetHomeNetwork() complete = %d\n"), Status.Int());
	}

//void CTestLtsyPhoneStepBase::GetServiceProviderName()
//	{
//	
//	}
void CTestLtsyPhoneStepBase::GetPhoneId()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("GetPhoneId() invoked......\n"));
	RMobilePhone::TMobilePhoneIdentityV1 aPhoneId;
	iSharedPhone.GetPhoneId(Status,aPhoneId);
	User::WaitForRequest(Status);
	INFO_PRINTF2(_L("GetPhoneId() complete = %d\n"), Status.Int());
	
	}
void CTestLtsyPhoneStepBase::GetDetectedNetworksL()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("GetDetectedNetworks() invoked......\n"));
	CRetrieveMobilePhoneDetectedNetworks* Retriever = CRetrieveMobilePhoneDetectedNetworks::NewL(iSharedPhone);
	Retriever->StartV2(Status);
	User::After(50*1000*1000);
	delete Retriever;
	INFO_PRINTF1(_L("GetDetectedNetworks() invoked......OK\n"));
	
//	iList = iRetriever->RetrieveListV2L();
	}
void CTestLtsyPhoneStepBase::GetDetectedNetworksCancelL()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("GetDetectedNetworksCancel() invoked......\n"));
	CRetrieveMobilePhoneDetectedNetworks* Retriever = CRetrieveMobilePhoneDetectedNetworks::NewL(iSharedPhone);
	Retriever->StartV2(Status);
	Retriever->Cancel();
	User::After(50*1000*1000);
	delete Retriever;
	INFO_PRINTF1(_L("GetDetectedNetworksCancel() invoked......OK\n"));
	}
void CTestLtsyPhoneStepBase::SetNetworkSelectionSetting()
/**
*	3.10.12 Set Network Selection Setting
*	void RMobilePhone::SetNetworkSelectionSetting(TRequestStatus& aReqStatus,
*		const TDes8& aSetting) const
*	This method is used by client application to set the way a phone will select a
*	network. The new setting is supplied within the aSetting parameter, which is a
*	packaged version of the TMobilePhoneNetworkSelectionV1V1 class above.
*	In manual network selection, the user is able to select a network to camp on to
*	from a list of detected networks. The selected network has to be accessible by the
*	user, i.e. it is not a forbidden network.
*	When a phone is using manual network selection, at power-up or after signal loss,
*	it will try to register to the network that the user last selected. If this network
*	is not available then it is up to the phone whether it tries to fall-back to
*	automatic selection or leaves the phone in no service.
*
*/
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("GetNetworkSelectionSetting() invoked......\n"));
	RMobilePhone::TMobilePhoneNetworkSelectionV1  localNetworkSel;
	localNetworkSel.iMethod = RMobilePhone::ENetworkSelectionManual;
	
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg localNetworkSelBuf(localNetworkSel) ;
    
	INFO_PRINTF1(_L("SetNetworkSelectionSetting() invoked......\n"));
	iSharedPhone.SetNetworkSelectionSetting(Status, localNetworkSelBuf) ;
	User::WaitForRequest(Status);
	INFO_PRINTF2(_L("SetNetworkSelectionSetting() complete = %d\n"), Status.Int());
	}
void CTestLtsyPhoneStepBase::SelectNetwork()
	{
	TRequestStatus Status;
	RMobilePhone::TMobilePhoneNetworkManualSelection tempsel;
	tempsel.iCountry = _L("460");
	tempsel.iNetwork = _L("00");
	INFO_PRINTF1(_L("SelectNetwork() invoked......\n"));
	iSharedPhone.SelectNetwork(Status,ETrue,tempsel);
//	iSharedPhone.CancelAsyncRequest(Status);
	User::WaitForRequest(Status);
	INFO_PRINTF2(_L("SelectNetwork() complete = %d\n"), Status.Int());
	
	}
void CTestLtsyPhoneStepBase::SetAndSelectNetwork()
	{
	INFO_PRINTF1(_L("SetAndSelectNetwork() invoked......\n"));
	SetNetworkSelectionSetting();
	SelectNetwork();
	}
void CTestLtsyPhoneStepBase::GetCurrentNetworkInfo()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("GetCurrentNetworkInfo() invoked......\n"));	
	RMobilePhone::TMobilePhoneNetworkInfoV5  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV5Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;
	iSharedPhone.GetCurrentNetwork(Status, aNetworkInfoBuf, aArea);
	User::WaitForRequest(Status);

	if (Status==KErrNone)
		{
		INFO_PRINTF1(_L("Get Get Current Network Information OK\n"));
		}
	else
		{
		INFO_PRINTF2(_L("Get Get Current Network Information, Error %d returned.\n"), Status.Int());
		}

	}
//void CTestLtsyPhoneStepBase::GetNetworkMode()
//	{
//	TRequestStatus Status;
//	INFO_PRINTF1(_L("SetAndSelectNetwork() invoked......\n")));
//	User::WaitForRequest(Status);
//	INFO_PRINTF1(_L("SetAndSelectNetwork() complete = %d\n")), Status.Int());
//	}
void CTestLtsyPhoneStepBase::GetNitzInfo()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("SetAndSelectNetwork() invoked......\n"));
	RMobilePhone::TMobilePhoneNITZ NITZInfo;
	iSharedPhone.GetNITZInfo(NITZInfo);
	
    User::After(15*1000*1000);
	INFO_PRINTF1(_L("SetAndSelectNetwork() invoked......OK\n"));
	
	}
void CTestLtsyPhoneStepBase::GetSignalStrength()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("GetSignalStrength() invoked......\n"));
	TInt32 signalStrength;
	TInt8 bars;

	iSharedPhone.GetSignalStrength(Status, signalStrength, bars);
	User::WaitForRequest(Status);
	
	if (Status==KErrNone)
		{
		INFO_PRINTF2(_L("Signal strength (in dBm) : %d\n"), signalStrength);
		INFO_PRINTF2(_L("Signal strength (in display bars) : %d\n"), bars);
		}
	else
		{
		INFO_PRINTF2(_L("Signal strength, Error %d returned.\n"), Status.Int());
		}
		
	User::After(3000000);		// Give user time to see the test results 
	}
void CTestLtsyPhoneStepBase::GetSignalCaps()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("GetSignalCaps() invoked......\n"));
	// Signal Caps
    TUint32 mmSignalCaps;
	TInt retCap;
	retCap = iSharedPhone.GetSignalCaps(mmSignalCaps);
	if (retCap==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Signal Capabilities NOT Supported\n"));		
		}
	else
		{
		INFO_PRINTF2(_L("Phone Signal Strength Caps = 0x%x\n"), mmSignalCaps);
		if (mmSignalCaps & RMobilePhone::KCapsGetSignalStrength)
			{
			INFO_PRINTF1(_L("  Get Signal Strength Supported\n"));
			}
		if (mmSignalCaps & RMobilePhone::KCapsNotifySignalStrengthChange)
			{
			INFO_PRINTF1(_L("  Notify Signal Strength Supported\n"));
			}
		}
	}
void CTestLtsyPhoneStepBase::GetBatteryInfo()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("GetBatteryInfo() invoked......\n"));
	RMobilePhone::TMobilePhoneBatteryInfoV1 battery;
	iSharedPhone.GetBatteryInfo(Status, battery);
	User::WaitForRequest(Status);
	if (Status==KErrNone)
		{
		INFO_PRINTF2(_L("Battery status : %d\n"), battery.iStatus);
		INFO_PRINTF2(_L("Battery charge level : %d\n"), battery.iChargeLevel);
		}
	else
		{
		INFO_PRINTF2(_L("Battery charge, Error %d returned.\n"), Status.Int());
		}
	}

void CTestLtsyPhoneStepBase::GetSignalStrengthCancel()
/** 
 * This method retrieves a phone's Signal and Battery Strength Information.
 */
	{
	TRequestStatus Status;
	// Cancel Requests
	INFO_PRINTF1(_L("GetSignalStrengthCancel() invoked......\n"));
	TInt32 signalStrength;
	TInt8 bars;
	iSharedPhone.GetSignalStrength(Status, signalStrength, bars);
	iSharedPhone.CancelAsyncRequest(EMobilePhoneGetSignalStrength);
	User::WaitForRequest(Status);
	INFO_PRINTF2(_L("GetSignalStrength Cancel status = %d\n\n"), Status.Int());
	}

/**
 * Notify signal strength change
 * 
 */
void CTestLtsyPhoneStepBase::NotifySignalStrengthChange()
	{
	TRequestStatus reqStatus;
	TInt32 signalStrength(0);
	TInt8 bar(0);
	TInt32 completeSignalStrength(70);
	TInt8 completeBar(3);
	SharedPhone().NotifySignalStrengthChange(reqStatus, signalStrength, bar);
	User::WaitForRequest(reqStatus);
	if(bar != completeBar)
		{
		ERR_PRINTF1(_L("bar is not expect."));
		}
	if(signalStrength != completeSignalStrength)
		{
		ERR_PRINTF1(_L("signalStrength is not expect."));
		}
	}
void CTestLtsyPhoneStepBase::GetBatteryCaps()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("GetBatteryCaps() invoked......\n"));
	// Battery Caps
    TUint32 mmBatteryCaps;
	TInt retCap;
	retCap = iSharedPhone.GetBatteryCaps(mmBatteryCaps);

	if (retCap==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Battery Capabilities NOT Supported\n"));		
		}
	else
		{
		INFO_PRINTF2(_L("Phone Battery Caps = 0x%x\n"), mmBatteryCaps);
		}
	}
void CTestLtsyPhoneStepBase::GetBatteryStrengthCancel()
/** 
 * This method retrieves a phone's Signal and Battery Strength Information.
 */
	{
	TRequestStatus Status;
	// Cancel Requests
	INFO_PRINTF1(_L("GetBatteryStrengthCancel() invoked......\n"));
	RMobilePhone::TMobilePhoneBatteryInfoV1 battery;

	iSharedPhone.GetBatteryInfo(Status, battery);
	iSharedPhone.CancelAsyncRequest(EMobilePhoneGetBatteryInfo);
	User::WaitForRequest(Status);
	INFO_PRINTF2(_L("GetBatteryInfo Cancel status = %d\n"), Status.Int());
	}
/**
 * 
 * Notify battery info change or not
 * 
 */
void CTestLtsyPhoneStepBase::NotifyBatteryInfoChange()
	{
	TRequestStatus status;
	RMobilePhone::TMobilePhoneBatteryInfoV1 batInfo;
	TUint twentyCharge = 20;
	iSharedPhone.NotifyBatteryInfoChange(status, batInfo);

	User::WaitForRequest(status);
	if(batInfo.iStatus != RMobilePhone::EBatteryConnectedButExternallyPowered)
		{
		INFO_PRINTF1(_L("Failed to Check Phone battery status."));
		}
	if(batInfo.iChargeLevel != twentyCharge)
		{
		INFO_PRINTF1(_L("Failed to Check Phone battery charge level."));
		}
	}
//void CTestLtsyPhoneStepBase::GetCurrentSystemNetworkModes()
//	{
//	TRequestStatus Status;
//	INFO_PRINTF1(_L("GetCellInfo() invoked......\n")));
//	User::WaitForRequest(Status);
//	INFO_PRINTF1(_L("GetCellInfo() complete = %d\n")), Status.Int());	
//	}
//void CTestLtsyPhoneStepBase::GetNetworkProviderName()
//	{
//	TRequestStatus Status;
//	INFO_PRINTF1(_L("GetCellInfo() invoked......\n")));
//	User::WaitForRequest(Status);
//	INFO_PRINTF1(_L("GetCellInfo() complete = %d\n")), Status.Int());	
//	}
//void CTestLtsyPhoneStepBase::GetOperatorName()
//	{
//	TRequestStatus Status;
//	INFO_PRINTF1(_L("GetCellInfo() invoked......\n")));
//	User::WaitForRequest(Status);
//	INFO_PRINTF1(_L("GetCellInfo() complete = %d\n")), Status.Int());	
//	}
void CTestLtsyPhoneStepBase::RegisterCellInfoChangeNotification()
	{
	TRequestStatus Status;
	INFO_PRINTF1(_L("NotifyCellInfoChange() invoked......\n"));
	RMobilePhone::TMobilePhoneCellInfoV9 phoneCellinfo1;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg Packaged1(phoneCellinfo1);
	iSharedPhone.NotifyCellInfoChange(Status,Packaged1);
	User::WaitForRequest(Status);
	INFO_PRINTF2(_L("NotifyCellInfoChange() complete = %d\n"), Status.Int());
	
	}
void CTestLtsyPhoneStepBase::GetCellInfo()
	{
	TRequestStatus Status;
	RMobilePhone::TMobilePhoneCellInfoV9 phoneCellinfo;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg Packaged(phoneCellinfo);
	
	INFO_PRINTF1(_L("GetCellInfo() invoked......\n"));
	iSharedPhone.GetCellInfo(Status,Packaged);
	User::WaitForRequest(Status);
	INFO_PRINTF2(_L("GetCellInfo() complete = %d\n"), Status.Int());	
	}
//void CTestLtsyPhoneStepBase::GetPhoneCellInfo()
//	{
//	}


// end of file
