//  TestTelephonyPktTestStepBase.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestTelephonyPktTestStepBase

#ifndef TESTTELEPHONYPKTTESTSTEPBASE_H_
#define TESTTELEPHONYPKTTESTSTEPBASE_H_

// user include
#include "te_integration_stltsysuitestepbase.h"
#include "TestLtsyPktSharedData.h"
#include "TestLtsyModel.h"

/**
 * This is the test step base class of PACKET test 
 * category which provides methods
 */
class CTestLtsyPktTestStepBase : public CTe_integration_stltsySuiteStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
protected:

	CTestLtsyPktTestStepBase(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktTestStepBase();
	
	RMobilePhone& SharedPhone();
	TBool IsOpenedPktService();
	
	void OpenPacketobject();
	void ClosePacketobject();
	
	
	void DisplayPacketServiceStatus();
	void DisplayNetworkRegistrationStatus();
	void DisplayContextInfo();
	void DisplayContextStatus(RPacketContext::TContextStatus aContextStatus);
	void DisplayContextStatus(RPacketContext& aContext, RPacketContext::TContextStatus& aContextStatus);
	
	void GprsContextAttachTest();
	void GprsContextDetachTest();
	
	void GprsNetWorkTest();
	
	void GprsContextSetConfigTest();
	void GprsContextGetConfigTest();
	
	void GprsGetDefaultContextParamsTest();
	void GprsSetDefaultContextParamsTest();
	
	void DisplayStaticCapabilities();
	void DisplayDynamicCapabilities();
	
	void GprsContextActivateTest();
	void GprsContextDeactivateTest();
	
	void GprsSetQoSProfileTest();
	void GprsGetQoSProfileTest();
	
	void NotificationContextAdded();
	void NotificationContextAddedCancel();
	
	void OpenNewContext();
	void CloseExistContext();
	
	void OpenExistingContext();
	void OpenContextUsingUnknownName();
	
	void GprsContextDeleteTest();
	
	void GprsInitialiseContext();
	
	void NotificationContextStatusChange();
	void NotificationContextStatusChangeCancel();
	
	void OpenNewContextQos();
	void CloseExistContextQos();
	

	void GprsContextSetConfigTest1();
	void GprsContextSetConfigTest2();
	void GprsContextSetConfigTest3();
	void GprsContextSetConfigTest4();

	void GprsSetQoSProfileTest1();
	void GprsSetQoSProfileTest2();
	void GprsSetQoSProfileTest3();
	void GprsSetQoSProfileTest4();
	void GprsSetQoSProfileTest5();
	void GprsSetQoSProfileTest6();
	void GprsSetQoSProfileTest7();
	void GprsSetQoSProfileTest8();
	void GprsSetQoSProfileTest9();
	void GprsSetQoSProfileTest10();
	void GprsSetQoSProfileTest11();
	void GprsSetQoSProfileTest12();
	void GprsSetQoSProfileTest13();
	void GprsSetQoSProfileTest14();
	void GprsSetQoSProfileTest15();
	void GprsSetQoSProfileTest16();
	void GprsSetQoSProfileTest17();
	void GprsSetQoSProfileTest18();
	void GprsSetQoSProfileTest19();
	void GprsSetQoSProfileTest20();

protected:
	CTestLtsyModel& iModel;
	RMobilePhone& iSharedPhone;
	};

#endif // TESTTELEPHONYPKTTESTSTEPBASE_H_
