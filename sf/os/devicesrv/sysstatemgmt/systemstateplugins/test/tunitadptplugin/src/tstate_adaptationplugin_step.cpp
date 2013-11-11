// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/



#include <s32mem.h>
#include <e32property.h>
#include <ssm/ssmadaptation.h>
#include "tstate_adaptationplugin_step.h"

const TUint32 KStateAdaptationPluginPropertyKey = 0x2000D76A;
const TUid KPropertyCategory={0x2000D75B};

typedef MStateAdaptation* (*TFuncNewL)();

CTestStateAdaptationPlugin::CTestStateAdaptationPlugin()
	:CAdaptationTestBase(KTCTestStateAdaptationPlugin)
	{
	}

CTestStateAdaptationPlugin::~CTestStateAdaptationPlugin()
	{
	
	}

void CTestStateAdaptationPlugin::TestRequestCoopSysStateChangeL()
	{
	INFO_PRINTF1(_L("> CTestStateAdaptationPlugin::TestRequestCoopSysStateChangeL"));

	TRequestStatus requestStatus;
	iStateAdaptation.RequestCoopSysStateChange(TSsmState(ESsmStartup, KSsmAnySubState),  requestStatus);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RequestCoopSysStateChange completed with status : %d"), requestStatus.Int());
	if(iStateAdaptation.Handle())
		{
		TEST(KErrNone == requestStatus.Int());	
		}
	else
		{
		TEST(KErrDisconnected == requestStatus.Int());	
		}
	}

void  CTestStateAdaptationPlugin::TestRequestCoopSysSelfTestL()
	{
	INFO_PRINTF1(_L("> CTestStateAdaptationPlugin::TestRequestCoopSysSelfTestL"));
	
	TRequestStatus requestStatus;
	iStateAdaptation.RequestCoopSysSelfTest(requestStatus);
	User::WaitForRequest(requestStatus);
	INFO_PRINTF2(_L("RequestCoopSysSelfTest completed with status : %d"), requestStatus.Int());
	if(iStateAdaptation.Handle())
		{
		TEST(KErrNone == requestStatus.Int());	
		}
	else
		{
		TEST(KErrDisconnected == requestStatus.Int());	
		}
	}

void CTestStateAdaptationPlugin::TestRequestCoopSysPerformRestartActionsL()
	{
	INFO_PRINTF1(_L("> CTestStateAdaptationPlugin::TestRequestCoopSysPerformRestartActionsL"));

	TRequestStatus status;
	TInt reason = 0;
	iStateAdaptation.RequestCoopSysPerformRestartActions(reason, status);
	User::WaitForRequest(status);
	if(iStateAdaptation.Handle())
		{
		TEST(KErrNone == status.Int());	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		}
	INFO_PRINTF2(_L("TestRequestCoopSysPerformRestartActionsL completed with status : %d"), status.Int());
	}

void CTestStateAdaptationPlugin::TestRequestCoopSysPerformShutdownActionsL()
	{
	INFO_PRINTF1(_L(" > CTestStateAdaptationPlugin::TestRequestCoopSysPerformShutdownActionsL"));

	TRequestStatus status;
	TInt reason = 0;
	iStateAdaptation.RequestCoopSysPerformShutdownActions(reason, status);
	User::WaitForRequest(status);
	if(iStateAdaptation.Handle())
		{
		TEST(KErrNone == status.Int());	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		}
	INFO_PRINTF2(_L("CTestStateAdaptationPlugin::TestRequestCoopSysPerformShutdownActionsL > completed with status : %d"), status.Int());		
	}

void CTestStateAdaptationPlugin::TestRequestCoopSysPerformRfsActionsL()
	{
	INFO_PRINTF1(_L(" > CTestStateAdaptationPlugin::TestRequestCoopSysPerformRfsActionsL"));
	INFO_PRINTF1(_L(" Call RequestCoopSysStateChange for ESsmShallowRfs, ESsmDeepRfs and ESsmRfsTypeExtensionBase"));

	TRequestStatus trs1,trs2,trs3;
	iStateAdaptation.RequestCoopSysPerformRfsActions(ESsmShallowRfs, trs1);
	iStateAdaptation.RequestCoopSysPerformRfsActions(ESsmDeepRfs, trs2);
	iStateAdaptation.RequestCoopSysPerformRfsActions(ESsmRfsTypeExtensionBase, trs3);
	
	User::WaitForRequest(trs1);
	User::WaitForRequest(trs2);
	User::WaitForRequest(trs3);

	INFO_PRINTF1(_L("CTestStateAdaptationPlugin::TestRequestCoopSysPerformRfsActionsL >"));
	}

void CTestStateAdaptationPlugin::TestRequestCancel()
	{
	INFO_PRINTF1(_L(" >CTestStateAdaptationPlugin::TestRequestCancel"));
	INFO_PRINTF1(_L(" Call RequestCancel"));

	iStateAdaptation.RequestCancel();

	INFO_PRINTF1(_L(" CTestStateAdaptationPlugin::TestRequestCancel>"));	
	}


void CTestStateAdaptationPlugin::TestNotifyCoopSysEventL()
	{
	INFO_PRINTF1(_L(" >CTestStateAdaptationPlugin::TestNotifyCoopSysEventL"));

	TRequestStatus status;
	TPckgBuf <TInt> coopSysErr;
	coopSysErr()=ESsmFatalCoopSysError;
	INFO_PRINTF1(_L(" Call NotifyCoopSysFatalEventL for ESsmFatalCoopSysError"));
	iStateAdaptation.NotifyCoopSysEvent(coopSysErr, status);
	User::WaitForRequest(status);

	TPckgBuf <TInt> shutDownDevice;
	shutDownDevice()=ESsmShutdownDevice;
	INFO_PRINTF1(_L(" Call NotifyCoopSysFatalEventL for ESsmShutdownDevice"));	
	iStateAdaptation.NotifyCoopSysEvent(shutDownDevice, status);
	User::WaitForRequest(status);

	TPckgBuf <TInt> restartDevice;
	restartDevice()=ESsmRestartDevice;
	INFO_PRINTF1(_L(" Call NotifyCoopSysFatalEventL for ESsmRestartDevice"));	
	iStateAdaptation.NotifyCoopSysEvent(restartDevice, status);
	User::WaitForRequest(status);

	TPckgBuf <TInt> sysEventType;
	sysEventType()=ESsmCoopSysEventTypeExtensionBase;
	INFO_PRINTF1(_L(" Call NotifyCoopSysFatalEventL for ESsmCoopSysEventTypeExtensionBase"));	
	iStateAdaptation.NotifyCoopSysEvent(sysEventType, status);
	User::WaitForRequest(status);

	INFO_PRINTF1(_L(" CTestStateAdaptationPlugin::TestNotifyCoopSysEventL>"));	
	}

void CTestStateAdaptationPlugin::TestNotifyCancel()
	{
	INFO_PRINTF1(_L(" >CTestStateAdaptationPlugin::TestNotifyCancel"));
	iStateAdaptation.NotifyCancel();
	INFO_PRINTF1(_L(" CTestStateAdaptationPlugin::TestNotifyCancel>"));	
	}

//from CAdaptationTestBase
TVerdict CTestStateAdaptationPlugin::doTestStepL()
	{
	TInt err = RProperty::Define(KPropertyCategory, KStateAdaptationPluginPropertyKey, RProperty::EInt);
	TEST((KErrNone == err) || (KErrAlreadyExists == err));
	err = RProperty::Set(KPropertyCategory, KStateAdaptationPluginPropertyKey, 1);
	TEST(KErrNone == err);
	
	__UHEAP_MARK;
	TRAP(err, TestRequestCoopSysStateChangeL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestRequestCoopSysSelfTestL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestRequestCoopSysPerformRestartActionsL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestRequestCoopSysPerformShutdownActionsL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestRequestCoopSysPerformRfsActionsL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestNotifyCoopSysEventL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	TestRequestCancel();
	TEST(err == KErrNone);

	TestNotifyCancel();
	TEST(err == KErrNone);
	
	User::LeaveIfError(iStateAdaptation.Connect());
	
	__UHEAP_MARK;
	TRAP(err, TestRequestCoopSysStateChangeL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestRequestCoopSysSelfTestL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestRequestCoopSysPerformRestartActionsL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestRequestCoopSysPerformShutdownActionsL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestRequestCoopSysPerformRfsActionsL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, TestNotifyCoopSysEventL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	TestRequestCancel();
	TEST(err == KErrNone);

	TestNotifyCancel();
	TEST(err == KErrNone);
	
	err = RProperty::Delete(KPropertyCategory, KStateAdaptationPluginPropertyKey);
	TEST(KErrNone == err);
	return TestStepResult();
	}
