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


#include <e32property.h>
#include <s32mem.h>
#include "trtc_adaptationplugin_step.h"

//
// Run the tests
//
_LIT (KExeToDefineTestPS, "\\sys\\bin\\definetestps.exe");
const TUint32 KRtcAdaptationPluginPropertyKey = 0x2000D76C;
const TUid KPropertyCategory={0x2000D75B};

CTestRtcAdaptationPlugin::CTestRtcAdaptationPlugin()
	:CAdaptationTestBase(KTCTestRtcAdaptationPlugin)
	{
	
	}

CTestRtcAdaptationPlugin::~CTestRtcAdaptationPlugin()
	{

	}



void CTestRtcAdaptationPlugin::TestValidateRtc()
	{
	INFO_PRINTF1(_L(" >CTestRtcAdaptationPlugin::TestValidateRtc"));

	TRequestStatus status;
	TPckgBuf <TTime> validityPckg;
	iSsmRtcAdaptation.ValidateRtc(validityPckg, status);
	User::WaitForRequest(status);
	if(iSsmRtcAdaptation.Handle())
		{
		TEST(KErrNotSupported == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestValidateRtc completed with %d error : expected %d>"),status.Int(),KErrNotSupported);	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestValidateRtc completed with %d error : expected %d>"),status.Int(),KErrDisconnected);	
		}
	}

void CTestRtcAdaptationPlugin::TestSetWakeupAlarm()
	{
	INFO_PRINTF1(_L(" >CTestRtcAdaptationPlugin::TestSetWakeupAlarm"));

	TRequestStatus status;
	TPckgBuf <TTime> alarmTimePckg;
	iSsmRtcAdaptation.SetWakeupAlarm(alarmTimePckg, status);
	User::WaitForRequest(status);

	if(iSsmRtcAdaptation.Handle())
		{
		TEST(KErrNotSupported == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestSetWakeupAlarm completed with %d error : expected %d>"),status.Int(),KErrNotSupported);	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestSetWakeupAlarm completed with %d error : expected %d>"),status.Int(),KErrDisconnected);	
		}
	}

void CTestRtcAdaptationPlugin::TestUnsetWakeupAlarm()
	{
	INFO_PRINTF1(_L(" >CTestRtcAdaptationPlugin::TestUnsetWakeupAlarm"));

	TRequestStatus status;
	iSsmRtcAdaptation.UnsetWakeupAlarm(status);
	User::WaitForRequest(status);

	if(iSsmRtcAdaptation.Handle())
		{
		TEST(KErrNotSupported == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestUnsetWakeupAlarm completed with %d error : expected %d>"),status.Int(),KErrNotSupported);	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestUnsetWakeupAlarm completed with %d error : expected %d>"),status.Int(),KErrDisconnected);	
		}
	}

void CTestRtcAdaptationPlugin::TestCancel()
	{
	INFO_PRINTF1(_L(" >CTestRtcAdaptationPlugin::TestCancel"));
	iSsmRtcAdaptation.Cancel();
	INFO_PRINTF1(_L(" CTestRtcAdaptationPlugin::TestCancel>"));	
	}


//from CAdaptationTestBase
TVerdict CTestRtcAdaptationPlugin::doTestStepL()
	{
	__UHEAP_MARK;
	
	RProcess processHandle;
    CleanupClosePushL(processHandle); 
	        
    //Start the test exe which defines startup related property keys 
	            
    TInt err = processHandle.Create(KExeToDefineTestPS, KNullDesC); 
    INFO_PRINTF2(_L("Process creation returned : %d"), err);
    User::LeaveIfError(err);
	processHandle.Resume(); 
	        
	// wait for the newly created process to rendezvous 
	TRequestStatus status; 
	processHandle.Rendezvous(status); 
	User::WaitForRequest(status); 
	TInt retVal = status.Int(); 
	INFO_PRINTF2(_L("iStatus.Int() returned : %d"), retVal); 
	TEST(KErrNone == retVal); 
	CleanupStack::PopAndDestroy();
	
    err = RProperty::Set(KPropertyCategory, KRtcAdaptationPluginPropertyKey, 1);
    TEST(KErrNone == err);
	TRAP(err, TestValidateRtc());
	TEST(err == KErrNone);

	TRAP(err, TestSetWakeupAlarm());
	TEST(err == KErrNone);
	
	TRAP(err, TestUnsetWakeupAlarm());
	TEST(err == KErrNone);
	
	TestCancel();
	
	User::LeaveIfError(iSsmRtcAdaptation.Connect());
	
	TRAP(err, TestValidateRtc());
	TEST(err == KErrNone);

	TRAP(err, TestSetWakeupAlarm());
	TEST(err == KErrNone);
	
	TRAP(err, TestUnsetWakeupAlarm());
	TEST(err == KErrNone);
	
	TestCancel();
	//TestRelease();			// have to test this part too ...

    err = RProperty::Set(KPropertyCategory, KRtcAdaptationPluginPropertyKey, 0);
    TEST(KErrNone == err);
	__UHEAP_MARKEND;

	return TestStepResult();
	}
