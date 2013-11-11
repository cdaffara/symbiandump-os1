//  TestTelephonyPktTestStepBase.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestTelephonyPktTestStepBase

#ifndef TESTLTSYPHONESTEPBASE_H_
#define TESTLTSYPHONESTEPBASE_H_


#include <e32base.h>
#include <es_sock.h>
#include <es_enum.h>

// user include
#include "te_integration_stltsysuitestepbase.h"
#include "TestLtsyModel.h"

const TInt	KOneSecond = 1000000;

/**
 * This is the test step base class of phone test 
 * category which provides methods
 */
class CTestLtsyPhoneStepBase : public CTe_integration_stltsySuiteStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	
protected:
	CTestLtsyPhoneStepBase(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPhoneStepBase();
	
	RMobilePhone& SharedPhone();
	
	void TestOpenPhoneL();
	void TestClosePhone();
	void GetFdnStatus();
	void GetNetworkRegistrationStatus();
	void GetPhoneId();
	void GetDetectedNetworksL();
	void GetDetectedNetworksCancelL(); //NO
	void SetNetworkSelectionSetting();
	void SelectNetwork();
	void SetAndSelectNetwork(); //NO
	
	void GetBatteryInfo();
	void GetBatteryCaps();
	void NotifyBatteryInfoChange();
	void GetBatteryStrengthCancel();
	
	void GetSignalStrength();
	void GetSignalCaps();
	void NotifySignalStrengthChange();
	void GetSignalStrengthCancel();
	
	void GetHomeNetwork();
	void GetServiceProviderName();//NO

	
	void GetCurrentNetworkInfo();
//	void GetNetworkMode();
	void GetNitzInfo();//no
	
//	void GetCurrentSystemNetworkModes();  //RMmCustomAPI
//	void GetNetworkProviderName(); //RMmCustomAPI
//	void GetOperatorName();  //RMmCustomAPI
	void RegisterCellInfoChangeNotification();
	void GetCellInfo();
//	void GetPhoneCellInfo(); //RMmCustomAPI
	
	
protected:
	CTestLtsyModel& iModel;
	RMobilePhone& iSharedPhone;
	
private:

	};

#endif // TESTLTSYPHONESTEPBASE_H_
