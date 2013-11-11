// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements all GPRS and Context test cases. The test cases are built
// for using against a Motorola GPRS and Ericsson T39/520 phones.
// All test cases that are in tgprsinter are also in this file.
// 
//

/**
 @file
*/

#include <e32base.h>
#include <e32test.h>
#include <c32comm.h>
#include "f32file.h"
#include <etel.h>
#include <es_sock.h>
#include <in_sock.h>
#include <etelqos.h>
#include <pcktcs.h>
#include "Te_Gprs.h"


CInitGlobals::CInitGlobals()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("InitGlobals"));
	}

enum TVerdict CInitGlobals::doTestStepL()
//LOCAL_C void InitGlobals()
/**
 * Initialise global variables.
 */
    {
#ifdef __WINS__
    User::After(5*KOneSecond);      // allow emulator time to start up
#endif

	const TPtrC phoneBrand = iTestConfigParam->FindAlphaVar(KTestCfgPhoneMake,KNullDesC);
	if(phoneBrand != KTestCfgPhoneMotorola && phoneBrand != KTestCfgPhoneEricsson)
		{
		INFO_PRINTF1(_L("There is no phone specified in ini file or the phone is not supported"));
		User::Panic(_L("There is no phone specified in ini file or the phone is not supported."), KErrNotSupported);
		}
	return TestStepResult();
	}

CTestGprsDefaultContextParams::CTestGprsDefaultContextParams()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsDefaultContextParams"));
	}

enum TVerdict CTestGprsDefaultContextParams::doTestStepL()
//LOCAL_C void  GprsDefaultContextParamsTest()
/**
 * This tests the default context params
 */
    {
    RPacketContext::TContextConfigGPRS     setDefaultParams;
    RPacketContext::TContextConfigGPRS     getDefaultParams;

	// Some default params to set, does not matter what it is.
    _LIT8(KAccesspoint, "www.Symbian.com");  
    _LIT8(KPpdpaddress, "192.168.111.100");

    TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);
    
    setDefaultParams.iPdpType = RPacketContext::EPdpTypeIPv4;
    setDefaultParams.iAccessPointName = KAccesspoint;
    setDefaultParams.iPdpAddress = KPpdpaddress;
    setDefaultParams.iPdpCompression = 1;

    TPckg<RPacketContext::TContextConfigGPRS>    setcontextConfigPckg(setDefaultParams);
    TPckg<RPacketContext::TContextConfigGPRS>    getcontextConfigPckg(getDefaultParams);
    TEST(iPacketService.SetDefaultContextParams(setcontextConfigPckg) == KErrNone);
    TEST(iPacketService.GetDefaultContextParams(getcontextConfigPckg) == KErrNone);
    TestConfigParams(setDefaultParams, getDefaultParams);

    CleanupStack::PopAndDestroy(&iPacketService);
    User::After(KFiveSeconds);

	return TestStepResult();
    }


CTestGprsUnSupported::CTestGprsUnSupported()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsUnSupported"));
	}

enum TVerdict CTestGprsUnSupported::doTestStepL()
//LOCAL_C void  GprsUnSupportedTest()
/**
 * Tests all unsupported GPRS API calls to the TSY.
 */
    {
    TRequestStatus status;
    TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

    iPacketService.RejectActivationRequest(status);
	 
	User::WaitForRequest(status);
	TEST(status.Int()== KErrNotSupported);

    iPacketService.RejectActivationRequest(status);
    iPacketService.CancelAsyncRequest(EPacketRejectActivationRequest);
    User::WaitForRequest(status);
    TEST(status.Int() == KErrNotSupported);

    RPacketService::TMSClass msClass;
    iPacketService.NotifyMSClassChange(status, msClass);
    User::WaitForRequest(status);
    TEST(status.Int() == KErrNotSupported);

    RPacketService::TDynamicCapsFlags dynCaps;
    TEST(iPacketService.GetDynamicCaps(dynCaps) == KErrNotSupported);

    iPacketService.NotifyDynamicCapsChange(status, dynCaps);
    User::WaitForRequest(status);
    TEST(status.Int() == KErrNotSupported);

    RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenPossible;
    TEST(iPacketService.SetAttachMode(attachMode) == KErrNotSupported);
    TEST(iPacketService.GetAttachMode(attachMode) == KErrNotSupported);

    RPacketService::TRegistrationStatus registrationStatus = RPacketService::ERegisteredRoaming;
    iPacketService.GetNtwkRegStatus(status, registrationStatus);

	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone || status.Int() == KErrNotSupported);

    CleanupStack::PopAndDestroy(&iPacketService);
    User::After(KFiveSeconds);

	return TestStepResult();
    }


CTestGprsContextAttach::CTestGprsContextAttach()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsContextAttach"));
	}

enum TVerdict CTestGprsContextAttach::doTestStepL()
/**
 * This tests the context attach and detach
 */
    {
    TRequestStatus                      stat1;
    TRequestStatus						stat2;
	TBuf<30>                            contextName;
    RPacketContext::TContextStatus        contextStatus;
    TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);
    CleanupClosePushL(iPacketContext[0]);
    
    iPacketContext[0].NotifyStatusChange(stat1, contextStatus);
    iPacketContext[0].CancelAsyncRequest(EPacketContextNotifyStatusChange);
    User::WaitForRequest(stat1);
    TEST(stat1.Int() == KErrCancel);

    iPacketContext[0].NotifyStatusChange(stat1, contextStatus);
    
	iPacketContext[0].Activate(stat2);
	User::WaitForRequest(stat2);
	TEST_CHECKL(stat2.Int(), KErrNone, 
		_L("***Failed to activate context. Check that you are using correct SIM!***"));
    
	User::WaitForRequest(stat1);
    TEST(stat1.Int() == KErrNone);
    TEST(contextStatus == RPacketContext::EStatusActivating);
    iPacketContext[0].NotifyStatusChange(stat1, contextStatus);
    User::WaitForRequest(stat1);
    TEST(stat1.Int() == KErrNone);
    TEST(contextStatus == RPacketContext::EStatusActive);
    
    iPacketContext[0].NotifyStatusChange(stat1, contextStatus);
    
	iPacketContext[0].Deactivate(stat2);
	User::WaitForRequest(stat2);
	TEST(stat2.Int() == KErrNone);

    User::WaitForRequest(stat1);
    TEST(stat1.Int() == KErrNone);
    TEST(contextStatus == RPacketContext::EStatusDeactivating);
    iPacketContext[0].NotifyStatusChange(stat1, contextStatus);
    User::WaitForRequest(stat1);
    TEST(stat1.Int() == KErrNone);
    TEST(contextStatus == RPacketContext::EStatusInactive);
    
    CleanupStack::PopAndDestroy(&iPacketContext[0]);
    CleanupStack::PopAndDestroy(&iPacketService);
    User::After(KFiveSeconds);

	return TestStepResult();
    }


CTestGprsContextDelete::CTestGprsContextDelete()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsContextDelete"));
	}

enum TVerdict CTestGprsContextDelete::doTestStepL()
//LOCAL_C void  GprsContextDeleteTest()
/**
 *  This tests the context delete.
 */
    {
    TRequestStatus						stat1;
	TBuf<30>                            contextName;
    RPacketContext::TContextStatus		contextStatus;

    TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);
    CleanupClosePushL(iPacketContext[0]);
    
	iPacketContext[0].Delete(stat1);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNone);
    
	iPacketContext[0].Activate(stat1);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrAccessDenied);

    TEST(iPacketContext[0].GetStatus(contextStatus) == KErrNone);
    TEST(contextStatus == RPacketContext::EStatusDeleted);

    CleanupStack::PopAndDestroy(&iPacketContext[0]);
    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
    }   


CTestGprsContextLoanComport::CTestGprsContextLoanComport()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsContextLoanComport"));
	}

enum TVerdict CTestGprsContextLoanComport::doTestStepL()
//LOCAL_C void  GprsContextLoanComport()
/**
 * This tests the comport loan.
 */
    {
	TRequestStatus						stat1;
    TBuf<30>                            contextName;
	RCall::TCommPort dataPort;

    TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);
    CleanupClosePushL(iPacketContext[0]);

    iPacketContext[0].RecoverCommPort(stat1);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrEtelPortNotLoanedToClient);

    iPacketContext[0].LoanCommPort(stat1, dataPort);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNotReady);

    iPacketContext[0].Activate(stat1);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);

    iPacketContext[0].LoanCommPort(stat1, dataPort);
	User::WaitForRequest(stat1); 
	TEST(stat1.Int() == KErrNone);

    iPacketContext[0].Activate(stat1);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrInUse);

    iPacketContext[0].RecoverCommPort(stat1);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNone);

    iPacketContext[0].Deactivate(stat1);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNone);

    CleanupStack::PopAndDestroy(&iPacketContext[0]);
    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
    }   


CTestGprsContextUnSupported::CTestGprsContextUnSupported()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsContextUnSupported"));
	}

enum TVerdict CTestGprsContextUnSupported::doTestStepL()
//LOCAL_C void  GprsContextUnSupportedTest()
/**
 *  This tests all GPRS context API calls.
 */
    {
    TRequestStatus            stat1;
    TBuf<30>                  contextName;
    TInt                      err = 0;
	RPacketContext::TDataVolume volume;
    TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);
    CleanupClosePushL(iPacketContext[0]);
    
    TEST(iPacketContext[0].GetDataVolumeTransferred(volume) == KErrNotSupported);
    iPacketContext[0].NotifyDataTransferred(stat1, volume);
    User::WaitForRequest(stat1);
    TEST(stat1.Int() == KErrNotSupported);

    TEST(iPacketContext[0].GetLastErrorCause(err) == KErrNotSupported);

    CleanupStack::PopAndDestroy(&iPacketContext[0]);
    CleanupStack::PopAndDestroy(&iPacketService);
    User::After(KFiveSeconds);

	return TestStepResult();
    }

CTestGprsAttach::CTestGprsAttach()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsAttach"));
	}

enum TVerdict CTestGprsAttach::doTestStepL()
//LOCAL_C void  GprsAttachTestL()
/**
 *  This tests the attach and detach.
 */
    {
    TRequestStatus stat1;
    RPacketService::TStatus status;

    User::LeaveIfError(iPacketService.Open(iPhone));
    CleanupClosePushL(iPacketService);
    
	iPacketService.Attach(stat1);
    User::WaitForRequest(stat1);
    TEST_CHECKL(stat1.Int(), KErrNone, 
		_L("***Failed to attach to packet service. Check that you are using correct SIM!***"));
    
	iPacketService.GetStatus(status);
	TEST_CHECKL(status,RPacketService::EStatusAttached,_L("Expected RPacketService::EStatusAttached"));

    iPacketService.Detach(stat1);
    User::WaitForRequest(stat1);
    User::LeaveIfError(stat1.Int());

    iPacketService.GetStatus(status);
	TEST_CHECKL(status,RPacketService::EStatusUnattached,_L("Expected RPacketService::EStatusUnattached"));

    CleanupStack::PopAndDestroy(&iPacketService);
    User::After(KFiveSeconds);

	return TestStepResult();
    }


CTestGprsNotification::CTestGprsNotification()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsNotification"));
	}

enum TVerdict CTestGprsNotification::doTestStepL()
//LOCAL_C void  GprsNotificationTest()
/**
 * Tests Notify Context added, Tests Notify status
 */
    {
    TRequestStatus stat1,stat2,stat3;
    TBuf<20> context= _L("");
    TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

    TBuf<40> contextId;

    iPacketService.NotifyContextAdded(stat1, contextId);
    iPacketService.CancelAsyncRequest(EPacketNotifyContextAdded);
	User::WaitForRequest(stat1);
    TEST(stat1.Int()== KErrCancel);
    iPacketService.NotifyContextAdded(stat1, contextId);

    TESTL(iPacketContext[0].OpenNewContext(iPacketService, context) == KErrNone);
    CleanupClosePushL(iPacketContext[0]);

    User::WaitForRequest(stat1);
    TEST(stat1.Int() == KErrNone);

    RPacketService::TStatus gprsStatus;
    iPacketService.NotifyStatusChange(stat2, gprsStatus);
    iPacketService.CancelAsyncRequest(EPacketNotifyStatusChange);
    User::WaitForRequest(stat2);
    TEST(stat2.Int() == KErrCancel);

    iPacketService.NotifyStatusChange(stat2, gprsStatus);
    iPacketService.Attach(stat1);
	User::WaitForRequest(stat1);
	TEST_CHECKL(stat1.Int(), KErrNone, 
		_L("***Failed to attach to packet service. Check that you are using correct SIM!***"));
    User::WaitForRequest(stat2);
    TEST(stat2.Int() == KErrNone);
    TEST(gprsStatus == RPacketService::EStatusAttached);

	User::After(1000000);	// Delay to allow phone to settle

    iPacketService.NotifyStatusChange(stat3, gprsStatus);
    iPacketService.Detach(stat1);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNone);
    User::WaitForRequest(stat3);
    TEST(stat3.Int() == KErrNone);
    TEST(gprsStatus == RPacketService::EStatusUnattached);

    CleanupStack::PopAndDestroy(&iPacketContext[0]);
    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
    }

CTestGprsMsClass::CTestGprsMsClass()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsMsClass"));
	}

enum TVerdict CTestGprsMsClass::doTestStepL()
//LOCAL_C void  GprsMsClassTest()
/**
 * This tests the MS Class.
 */
    {
    TRequestStatus stat1;

    TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);
    
    RPacketService::TMSClass     currentClass;
    RPacketService::TMSClass     maxClass;
    iPacketService.GetMSClass(stat1, currentClass, maxClass);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNone);
 
	const TPtrC phoneBrand = iTestConfigParam->FindAlphaVar(KTestCfgPhoneMake,KNullDesC);
	if(phoneBrand == KTestCfgPhoneMotorola)
		{
		iPacketService.SetMSClass(stat1, currentClass);
		User::WaitForRequest(stat1);
		TEST(stat1.Int() == KErrNone);
	    TEST(currentClass == RPacketService::EMSClassSuspensionRequired);
		TEST(maxClass == RPacketService::EMSClassSuspensionRequired);
		}
	
    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
    }



CTestGprsContextConfig::CTestGprsContextConfig()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsContextConfig"));
	}

enum TVerdict CTestGprsContextConfig::doTestStepL()
//LOCAL_C void  GprsContextConfigTest()
/**
 * Test context config.
 */
    {
    TRequestStatus stat1;
	TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

    TBuf<30> contextName;
    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);
    CleanupClosePushL(iPacketContext[0]);
    TESTL(iPacketContext[1].OpenNewContext(iPacketService, contextName) == KErrNone);
    CleanupClosePushL(iPacketContext[1]);
    
    RPacketContext::TContextConfigGPRS     Getparams;
    RPacketContext::TContextConfigGPRS     Setparams;
    TPckg<RPacketContext::TContextConfigGPRS>    GetcontextConfigPckg(Getparams);
    iPacketContext[0].GetConfig(stat1, GetcontextConfigPckg);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNone);
    Setparams = Getparams;

    TPckg<RPacketContext::TContextConfigGPRS>    SetcontextConfigPckg(Setparams);
    iPacketContext[0].SetConfig(stat1, SetcontextConfigPckg);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNone);
    iPacketContext[1].SetConfig(stat1, SetcontextConfigPckg);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNone);
    iPacketContext[0].GetConfig(stat1, SetcontextConfigPckg);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNone);
    TestConfigParams(Getparams, Setparams);

	TRequestStatus stat;
	iPacketContext[0].SetConfig(stat, SetcontextConfigPckg);
	iPacketContext[0].CancelAsyncRequest(EPacketContextSetConfig);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrCancel);

	iPacketContext[0].GetConfig(stat, GetcontextConfigPckg);
	iPacketContext[0].CancelAsyncRequest(EPacketContextGetConfig);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrCancel);

	Setparams.iPdpType = RPacketContext::TProtocolType(20); // Set out of range
	TPckg<RPacketContext::TContextConfigGPRS>    errorPackage(Setparams);
	iPacketContext[0].SetConfig(stat1, errorPackage);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNotSupported);

    CleanupStack::PopAndDestroy(&iPacketContext[1]);
    CleanupStack::PopAndDestroy(&iPacketContext[0]);
    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
    }


CTestGprsContextConfigNotifications::CTestGprsContextConfigNotifications()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsContextConfigNotifications"));
	}

enum TVerdict CTestGprsContextConfigNotifications::doTestStepL()
//LOCAL_C void  GprsContextConfigNotificationsTest()
/**
 * Test context notifications.
 */
    {
    TRequestStatus                      stat1, stat2;
    TBuf<30>                            contextName;
    RPacketContext::TContextConfigGPRS   params, notifyParams;
    TPckg<RPacketContext::TContextConfigGPRS>  contextConfigPckg(params);
    TPckg<RPacketContext::TContextConfigGPRS>  notifyContextConfigPckg(notifyParams);
    
    TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);
    CleanupClosePushL(iPacketContext[0]);
    TESTL(iPacketContext[1].OpenNewContext(iPacketService, contextName) == KErrNone);
    CleanupClosePushL(iPacketContext[1]);

    iPacketContext[0].NotifyConfigChanged(stat1, notifyContextConfigPckg);
    iPacketContext[0].CancelAsyncRequest(EPacketContextNotifyConfigChanged);
    User::WaitForRequest(stat1);
    TEST(stat1.Int() == KErrCancel);

    iPacketContext[0].GetConfig(stat1, contextConfigPckg);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrNone);
    iPacketContext[0].NotifyConfigChanged(stat1, notifyContextConfigPckg);
    
	iPacketContext[0].SetConfig(stat2, contextConfigPckg);
	User::WaitForRequest(stat2);
	TEST(stat2.Int() == KErrNone);

    User::WaitForRequest(stat1);
    TEST(stat1.Int() == KErrNone);
    TestConfigParams(params, notifyParams);
	
    CleanupStack::PopAndDestroy(&iPacketContext[1]);
    CleanupStack::PopAndDestroy(&iPacketContext[0]);
    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
    }


CTestOpenGprsMaxContexts::CTestOpenGprsMaxContexts()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestOpenGprsMaxContexts"));
	}

enum TVerdict CTestOpenGprsMaxContexts::doTestStepL()
//LOCAL_C void  OpenGprsMaxContextsTest()
/**
 *  Tests enumerate contexts, open new context, open existing context.
 */
    {
    TRequestStatus	stat1;
	TBuf<20> contextName = _L("");

    TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

    TInt countContexts;
    TInt maxAllowedContexts;
    iPacketService.EnumerateContexts(stat1, countContexts, maxAllowedContexts);
	User::WaitForRequest(stat1);
    TEST(countContexts == 0);

	const TPtrC phoneBrand = iTestConfigParam->FindAlphaVar(KTestCfgPhoneMake,KNullDesC);
	if(phoneBrand == KTestCfgPhoneMotorola)
		TEST(maxAllowedContexts == 3);
	else if(phoneBrand == KTestCfgPhoneEricsson)
		TEST(maxAllowedContexts == 10);
	
    TInt i = 0;
    RPacketService::TContextInfo contextinfo;

    for(i = 0; i < maxAllowedContexts;i++)
        {
        RPacketContext contextPacket;
        TBuf<30> tmpContextName;
        tmpContextName = _L("GPRS_CONTEXT");
        tmpContextName.AppendNum(i+1);
        TESTL(iPacketContext[i].OpenNewContext(iPacketService, contextName) == KErrNone);
        iPacketService.GetContextInfo(stat1, i, contextinfo);
		User::WaitForRequest(stat1);
        TEST(contextinfo.iName == tmpContextName);
        TEST(contextinfo.iStatus == RPacketContext::EStatusUnknown);
        TESTL(contextPacket.OpenExistingContext(iPacketService, contextName) == KErrNone);
        TEST(contextName == tmpContextName); 
        contextPacket.Close();
        }
    TESTL(iPacketContext[i+5].OpenNewContext(iPacketService, contextName) == KErrOverflow);
	
	iPacketService.GetContextInfo(stat1, i+1, contextinfo);
	User::WaitForRequest(stat1);
	TEST(stat1.Int() == KErrArgument);
    for(i = 0; i < maxAllowedContexts;i++)
        {
        iPacketContext[i].Close(); 
        }

    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
    }



CTestGprsCancel::CTestGprsCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsCancel"));
	}

enum TVerdict CTestGprsCancel::doTestStepL()
//LOCAL_C void GprsCancelTest()
/**
 * This test will call all GPRS API calls 
 * that can be canceled with an outstanding request
 * to the phone.
 */ 
	{
	TRequestStatus	stat;
	
	TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

	iPacketService.Attach(stat);
	iPacketService.CancelAsyncRequest(EPacketAttach);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrCancel);

	iPacketService.Detach(stat);
	iPacketService.CancelAsyncRequest(EPacketDetach);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrCancel);

	RPacketService::TMSClass currentClass;
	RPacketService::TMSClass maxClass;

	iPacketService.GetMSClass(stat, currentClass, maxClass);
	iPacketService.CancelAsyncRequest(EPacketGetMSClass);
	User::WaitForRequest(stat);
	TEST((stat.Int() == KErrCancel)||(stat.Int() == KErrNone));

	currentClass = RPacketService::EMSClassSuspensionRequired;
	iPacketService.SetMSClass(stat, currentClass);
	iPacketService.CancelAsyncRequest(EPacketSetMSClass);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrCancel);

    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
	}

CTestGprsfaxCancel::CTestGprsfaxCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsfaxCancel"));
	}

enum TVerdict CTestGprsfaxCancel::doTestStepL()
//LOCAL_C void TestGprsfaxCancel()
/**
 * This function tests the opening of 2 call objects
 * and doing a hang-up on them while there is a 
 * GPRS object in the system.
 */
	{
    TBuf<100>                           Context;
    TRequestStatus stat1, stat2;
    Context = _L("");
    _LIT(KFaxLineName,"Fax");
    _LIT(KVoiceLineName,"Voice");

    TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

    RLine faxLine;
    RCall call;
	RCall call2;
    RLine voiceLine;
    TName callName;

    faxLine.Open(iPhone ,KFaxLineName);
    CleanupClosePushL(faxLine);
    voiceLine.Open(iPhone ,KVoiceLineName);
    CleanupClosePushL(voiceLine);

    TESTL(iPhone.Initialise()==KErrNone);
    faxLine.NotifyIncomingCall(stat1,callName);
	faxLine.NotifyIncomingCallCancel();
	voiceLine.NotifyIncomingCall(stat2,callName);
    voiceLine.NotifyIncomingCallCancel();
    User::WaitForRequest(stat1);
	User::WaitForRequest(stat2);

    TEST(call.OpenNewCall(voiceLine)==KErrNone);
    CleanupClosePushL(call);
    TEST(call.HangUp()==KErrNone);

    TEST(call2.OpenNewCall(faxLine)==KErrNone);
    CleanupClosePushL(call2);
    TEST(call2.HangUp()==KErrNone);

    CleanupStack::PopAndDestroy(&call2);
    CleanupStack::PopAndDestroy(&call);
    CleanupStack::PopAndDestroy(&voiceLine);
    CleanupStack::PopAndDestroy(&faxLine);
    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
	}


CTestOpenContextByName::CTestOpenContextByName()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestOpenContextByName"));
	}

enum TVerdict CTestOpenContextByName::doTestStepL()
//LOCAL_C void  TestOpenContextByNameL()
/**
 * This test should verify that defect SHY-54SNHT is fixed.
 */
	{
	TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

	TBuf<30> contextName;
	TESTL(iPacketContext[0].OpenNewContext(iPacketService,contextName) == KErrNone);
    CleanupClosePushL(iPacketContext[0]);
	TESTL(iPacketContext[1].OpenExistingContext(iPacketService,contextName) == KErrNone);
    CleanupClosePushL(iPacketContext[1]);

    CleanupStack::PopAndDestroy(&iPacketContext[1]);
    CleanupStack::PopAndDestroy(&iPacketContext[0]);
    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
	}


CTestNetworkRegStatus::CTestNetworkRegStatus()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestNetworkRegStatus"));
	}

enum TVerdict CTestNetworkRegStatus::doTestStepL()
//LOCAL_C void  TestNetworkRegStatusL()
/**
 * This test does a basic test on the Get Current Network 
 * Registration Status functionality.
 */
	{
	TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

	RPacketService::TRegistrationStatus regStatus;
	TRequestStatus status;
	iPacketService.GetNtwkRegStatus(status,regStatus);
	User::WaitForRequest(status);
	if(status!=KErrNone && status!=KErrNotSupported)		// Do not consider KErrNotSupported a failure
		{
		iPacketService.Close();
		User::Leave(status.Int());
		}

    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
	}


CTestGetStaticCaps::CTestGetStaticCaps()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetStaticCaps"));
	}

enum TVerdict CTestGetStaticCaps::doTestStepL()
//LOCAL_C void TestGetStaticCapsL()
/**
 * Simple test for RPacketService::GetStaticCaps
 */
 	{
	TESTL(iPacketService.Open(iPhone) == KErrNone);
    CleanupClosePushL(iPacketService);

	//
	// Call RPacketService::GetStaticCaps with EPdpTypeIPv6
	// We would expect this call to fail with KErrNotSupported
	TUint caps(0);		// Create and initialise caps
	TInt ret=iPacketService.GetStaticCaps(caps,RPacketContext::EPdpTypeIPv6);
	TEST_CHECKL(ret,KErrNotSupported,_L("Expected KErrNotSupported for GetStaticCaps with EPdpTypeIPv6"));

	//
	// Call RPacketService::GetStaticCaps
	caps=0;		// Intialise caps
	TESTL(iPacketService.GetStaticCaps(caps, RPacketContext::EPdpTypeIPv4) == KErrNone);
 
	//
	// Validate caps returned by GetStaticCaps.
	// (we know what the values should be as they specified in the MMTSY design document
	//  are harcoded in the MMTSY)
	TEST_CHECKL(caps,RPacketService::KCapsSetDefaultContextSupported|RPacketService::KCapsMSClassSupported,_L("Unexpcted Static Caps"));

    CleanupStack::PopAndDestroy(&iPacketService);
	User::After(KFiveSeconds);

	return TestStepResult();
	}


CTestOpenExistingContext::CTestOpenExistingContext()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestOpenExistingContext"));
	}

enum TVerdict CTestOpenExistingContext::doTestStepL()
//LOCAL_C void TestOpenExistingContextL()
/**
 * Simple test for RPacketService::OpenExistingContext.
 * This test should verify the fix for defect SHY-54SNHT.
 * If SHY-54SNHT has not been fixed an access violation will occur in Etel during the 
 * OPenExistingContext call.
 */
	{
	RPacketService packetService;
	TESTL(packetService.Open(iPhone) == KErrNone);
    CleanupClosePushL(packetService);

	// Open context using an invalid context name (call should fail with KErrNotFound)
	RPacketContext context;
	_LIT(KUnknownContextName,"NonExistantContext");
	TInt ret=context.OpenExistingContext(packetService,KUnknownContextName);
	CHECKPOINT(ret,KErrNotFound,_L("RPacketContext::OpenExistingContext did not fail when it should have"));

    CleanupStack::PopAndDestroy(&packetService);
	User::After(KFiveSeconds);

	return TestStepResult();
	}

CTestGprsDownloadWebPage::CTestGprsDownloadWebPage()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGprsDownloadWebbPage"));
	}

enum TVerdict CTestGprsDownloadWebPage::doTestStepL()
//LOCAL_C void  GprsDownloadWebbPageTestL()

/**
 * This test will open a socket and download a
 * page/file that is specified by KTestCfgHTTPServerIpAddress
 * and KTestCfgHTTPPageAddress
 *
 */

	{
	// Constants used for this test.
	const TInt KWebPort = 80; 	
	const TPtrC testAddr = iTestConfigParam->FindAlphaVar(KTestCfgHTTPServerIpAddress,KNullDesC);
	const TPtrC testPage = iTestConfigParam->FindAlphaVar(KTestCfgHTTPPageAddress,KNullDesC);

	HBufC8* iWebPage = HBufC8::NewMaxL(4096);
	CleanupStack::PushL(iWebPage);
	
	INFO_PRINTF1(_L("Send HTML request"));
	RSocket sock;
	INFO_PRINTF1(_L("Open tcpip socket"));
	TESTL(sock.Open(iSocketServer, KAfInet, KSockStream, KProtocolInetTcp) == KErrNone);
	CleanupClosePushL(sock);

	TInetAddr addr(KWebPort);
	addr.Input(testAddr);
	TRequestStatus stat;

	INFO_PRINTF1(_L("Connecting to socket"));
	sock.Connect(addr, stat);
	WaitWithTimeout(stat);
	TESTL(stat.Int() == KErrNone);

	// List the route to the server.
	INFO_PRINTF1(_L("List the route to the server"));
	ListRoutes(sock);

	TPtr8 des(iWebPage->Des());
	des.Copy(_L("GET"));
	des.Append(testPage);
	des.Append(_L(" \r\n\r\n"));

	INFO_PRINTF1(_L("Writing data to socket"));
	sock.Write(*iWebPage, stat);
	WaitWithTimeout(stat);
	TESTL(stat.Int() == KErrNone);
	INFO_PRINTF1(_L("Sent request OK"));

	TSockXfrLength HTTPHeaderLen;
	TInt rcount = 0;
	TTime startTime;
	startTime.UniversalTime();
	FOREVER
		{
		des.Zero();
		sock.RecvOneOrMore(des,0,stat,HTTPHeaderLen);
		User::WaitForRequest(stat);//, readStat);
		if(stat!=KErrNone)
			break;
		rcount += iWebPage->Length();
//		gTest.Console()->SetPos(0);
		INFO_PRINTF2(_L("\rRecved:%10d Bytes"), rcount);
		}

	if(stat.Int() != KErrEof)
		User::Leave(KErrEof);

	// Calculate the number of bytes per second.
	TTime stopTime;
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds totalTime = stopTime.MicroSecondsFrom(startTime);
	TInt64 byteRate(rcount);
	byteRate *= KOneSecond;
	byteRate /=totalTime.Int64();
	INFO_PRINTF1(_L(""));
	INFO_PRINTF2(_L("Rate: %d B/s"), I64INT(byteRate));
	INFO_PRINTF1(_L("Received HTML page OK"));

	// Close the socket.
	CleanupStack::PopAndDestroy(&sock);	
	// Stop the agent so that the we will be disconnected from the network.
//	gGenericAgent.Stop();
	iConnection.Stop();
	User::After(KFiveSeconds);

	// Delete the webpage buffer.
	CleanupStack::PopAndDestroy(iWebPage);
	User::After(KFiveSeconds * 10);
	return TestStepResult();
	}

TInt CTestGprsDownloadWebPage::ListRoutes(RSocket& aSocket)
/**
 * This function lists all the hops that the packet 
 * needs to make to reach the port on the server.
 */
	{

	TPckgBuf<TSoInetRouteInfo> opt;
	TInt ret;
	ret = aSocket.SetOpt(KSoInetEnumRoutes, KSolInetRtCtrl);
	if (ret!=KErrNone)
		return ret;

	while (ret==KErrNone)
		{
		ret = aSocket.GetOpt(KSoInetNextRoute, KSolInetRtCtrl, opt);
		if (ret!=KErrNone)
			continue;
		TBuf<40> ia, ga, da, nm;
		opt().iDstAddr.Output(da);
		opt().iNetMask.Output(nm);
		opt().iGateway.Output(ga);
		opt().iIfAddr.Output(ia);
		INFO_PRINTF5(_L("Destination=%15S NetMask=%15S Gateway=%15S Interface=%15S"),
					 &da, &nm, &ga, &ia);
		INFO_PRINTF4(_L("State=%d Metric=%d Type=%d"),
					 opt().iState, opt().iMetric, opt().iType);
		}
	return ret==KErrEof ? KErrNone : ret;
	}

void CTestGprsDownloadWebPage::WaitWithTimeout(TRequestStatus& aStatus)
/**	Waits untill end or request or time out
 * @param request itself
 * @param  number Of microseconds
 */
	{
	TRequestStatus timerStatus;
	RTimer timer;
	timer.CreateLocal() ;

	const TInt KOneSecondInMicroSeconds=1000000L;
	timer.After(timerStatus,240*KOneSecondInMicroSeconds);		// x second timer

	User::WaitForRequest(aStatus, timerStatus);

	// Cancel timer if it did not complete
	if (timerStatus == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	else
		{
		aStatus=-999;		// Hopefully -999 reported in the test result log will make it obvious
								    // that the request was cancelled due to it taking to long
		INFO_PRINTF1(_L("User::WaitForRequest cancelled as it did not complete within a reasonable time"));
		}

	timer.Close() ;
	}

/**
Each test step initialises it's own name
*/
CTest2GprsConnection::CTest2GprsConnection()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("Test2GprsConnection"));
	}

/**
 * This test case will open 2 GPRS connection and will check wheteher
 * it is able to open 2 connection or not. This test case is needed
 * because the behavior of opening connection differs in Netcon if
 * Connection preference dialog was set.
 * If the connection preference was set:
 *			Netcon will prompt the user to select the correct IAP.
 *			Sends request to TSY to get MSClass and NetworkRegStatus.
 *
 * Note : In case of MM.TSY, the above scenario might create problem
 *		  due to loaning and recovering commport concept. After 1st 
 * 		  connection establishment, commport was loaned to transfer
 *		  the data so when MM.TSY receives MSClass and NetworkRegStatus
 *		  request it might declined that request which will cause 2nd
 *		  connection to fail.
 *
 *		  In case of other TSY this might not create problem due to different
 *		  commport implementation concept. 
 *
*/
enum TVerdict CTest2GprsConnection::doTestStepL()
	{
	TInt err;
	RConnection conn1;
	TESTL(conn1.Open(iSocketServer)==KErrNone);
	CleanupClosePushL(conn1);
	TInt ret=conn1.Start();
	TESTL(ret==KErrNone);

	RSocket sock1;
	err = sock1.Open(iSocketServer, KAfInet, KSockStream, KProtocolInetTcp, conn1);
	if (err!=KErrNone)
		{
		conn1.Stop();
		}
	TESTL(err==KErrNone);
	CleanupClosePushL(sock1);

	_LIT(KAddr1,"64.233.161.147");
	const TInt KPort = 80; 

	TInetAddr destAddr1;
	destAddr1.Input(KAddr1);
	destAddr1.SetPort(KPort);

	TRequestStatus status1;
	// Request the Socket to connect to the destination
	sock1.Connect(destAddr1, status1);
	User::WaitForRequest(status1);
	if (status1.Int()!=KErrNone)
		{
		conn1.Stop();
		}
	TESTL(status1.Int() == KErrNone);

	RConnection conn2;
	err=conn2.Open(iSocketServer);
	if (err!=KErrNone)
		{
		conn1.Stop();
		}
	TESTL(err==KErrNone);
	CleanupClosePushL(conn2);
	err=conn2.Start();
	if (err!=KErrNone)
		{
		conn1.Stop();
		}
	TESTL(err==KErrNone);

	RSocket sock2;
	err=sock2.Open(iSocketServer, KAfInet, KSockStream, KProtocolInetTcp, conn2);
	if (err!=KErrNone)
		{
		conn2.Stop();
		conn1.Stop();
		}
	TESTL(err==KErrNone);
	CleanupClosePushL(sock2);

	_LIT(KAddr2,"68.142.226.53");

	TInetAddr destAddr2;
	destAddr2.Input(KAddr2);
	destAddr2.SetPort(KPort);

	// Request the Socket to connect to the destination
	sock2.Connect(destAddr2, status1);
	User::WaitForRequest(status1);
	if (status1.Int()!=KErrNone)
		{
		conn2.Stop();
		conn1.Stop();
		}
	TESTL(status1.Int() == KErrNone);

	CleanupStack::PopAndDestroy(&sock2);
	conn2.Stop();
	User::After(KFiveSeconds);
	CleanupStack::PopAndDestroy(&conn2); 
	CleanupStack::PopAndDestroy(&sock1);
	conn1.Stop();
	User::After(KFiveSeconds);
	CleanupStack::PopAndDestroy(&conn1);

	return TestStepResult();
	}
