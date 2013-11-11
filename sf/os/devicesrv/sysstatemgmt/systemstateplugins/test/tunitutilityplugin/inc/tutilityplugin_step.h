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
// tutilityplugin_step.cpp
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
#ifndef __TUTILITY_PLUGIN_TEST_H__
#define __TUTILITY_PLUGIN_TEST_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmstateawaresession.h>
#include <e32event.h>

_LIT(KTUtilityPluginStep,"UtilityPluginStep");

class CUtilityPluginTest : public CTestStep
	{
public:
	typedef void (CUtilityPluginTest::*ClassFuncPtrL) ();
public:
	CUtilityPluginTest(const TDesC& aStepName);
	~ CUtilityPluginTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

	void doOOMTestL(ClassFuncPtrL testFuncL);
	
private:
	void doTest1L();
	void doTest2L();
	void doTest3L();
	void doTest4L();
	void doTest5L();
	void doTest6L();
	void doTest7L();	//Manual test
	void TestNotificationAndPowerModeL(TRawEvent::TType aRawEvent, TBool aIsNotificationExpected, TPowerState aExpectedPowerState,
										 TBool aDelayAcknowledgement = EFalse, TBool aCancelTransition = EFalse);
	void SetDefaultTestKey();
	void SwitchOn();
	void TestStateChangeNotificationNotReceivedL();
	void TestSystemShutdownWithoutAcknowledgementL();
	void LogDelayAcknowledgement();
	TInt GetStartUpModeL(); 
	};

/**
Class which will run the timer and expire after stipulated timeout.
*/
class CShutdownNotifObserverTimer : public CTimer
    {
    public:
        static CShutdownNotifObserverTimer* NewL();
        ~CShutdownNotifObserverTimer();
        void StartTimer(TInt32 aTimeout);

    protected:
        // from CActive
        void RunL();
		TInt RunError(TInt aError);

    private:
        void ConstructL();
        CShutdownNotifObserverTimer();
	private:
		CActiveScheduler* iScheduler;
    };

#endif		//__TUTILITY_PLUGIN_TEST_H__

