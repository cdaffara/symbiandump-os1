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
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/
#include <w32std.h>
#include <e32property.h>
#include <e32power.h>
#include <startupdomaindefs.h>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmstatemanager.h>
#include <hal.h>
#include <ssm/ssmpatchableconstants.h>
#include <e32uid.h>

#include "tutilityplugin_step.h"

TUid TestCategory = {0x456};
TUint TestKey = 123;
TInt TestDefaultValue = EPwActive;
const TUint KWaitTimer = 10000000;	// 10 seconds

CShutdownNotifObserverTimer* CShutdownNotifObserverTimer::NewL()
    {
    CShutdownNotifObserverTimer* self = new (ELeave) CShutdownNotifObserverTimer();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CShutdownNotifObserverTimer::~CShutdownNotifObserverTimer()
    {
    Cancel();
    delete iScheduler;
    }

void CShutdownNotifObserverTimer::StartTimer(TInt32 aTimeout)
    {
    After(aTimeout);
    CActiveScheduler::Start();
    }

void CShutdownNotifObserverTimer::RunL()
    {
    User::LeaveIfError(RProperty::Set(TestCategory, TestKey, 0xBAD));
    RDebug::Print(_L("CShutdownNotifObserverTimer::RunL"));
    
    CActiveScheduler::Stop();
    }

TInt CShutdownNotifObserverTimer::RunError(TInt aError)
	{
	RDebug::Print(_L("RProperty::Set for TestCategory: 0x%x, TestKey: %d failed with an error: %d"), TestCategory, TestKey, aError);
	return KErrNone;
	}

void CShutdownNotifObserverTimer::ConstructL()
    {
    iScheduler = new (ELeave) CActiveScheduler();
    CActiveScheduler::Install(iScheduler);
    CActiveScheduler::Add(this);
    CTimer::ConstructL();
    }

CShutdownNotifObserverTimer::CShutdownNotifObserverTimer() : CTimer(EPriorityStandard)
    {
    }

CUtilityPluginTest::CUtilityPluginTest(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}

CUtilityPluginTest::~CUtilityPluginTest()
	{
	}

TVerdict CUtilityPluginTest::doTestStepPreambleL()
	{
	TInt startupMode = GetStartUpModeL();
	if (IsSsmGracefulShutdown()&& (startupMode!=1))
		{
		INFO_PRINTF1(_L("Wait for lafshutdowneventobserveradaptor.dll to load"));
		User::After(5000000);
		//define the test property to check the power mode, this will be set through product code.
		_LIT_SECURITY_POLICY_PASS(KReadPolicy);
		_LIT_SECURITY_POLICY_PASS(KWritePolicy);
		TInt err = RProperty::Define(TestCategory, TestKey, RProperty::EInt, KReadPolicy, KWritePolicy);
		INFO_PRINTF2(_L("Test key defined to store the power state with err : [%d]"), err);
		SetDefaultTestKey();
		}
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CUtilityPluginTest::doTestStepPostambleL()
	{
	TInt startupMode = GetStartUpModeL();
	if (IsSsmGracefulShutdown()&& (startupMode!=1))
		{
		TInt err = RProperty::Delete(TestCategory, TestKey);
		INFO_PRINTF2(_L("Test key deleted to store the power state with err : [%d]"), err);
		}
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CUtilityPluginTest::doTestStepL()
	{
	TInt startupMode = GetStartUpModeL();
	if ((IsSsmGracefulShutdown()) && (startupMode!=1))
		{
		__UHEAP_MARK;
		TRAPD(err, doTest1L());
		TEST(err == KErrNone);
		__UHEAP_MARKEND;

		__UHEAP_MARK;
		TRAP(err, doTest2L());
		TEST(err == KErrNone);
		__UHEAP_MARKEND;

		__UHEAP_MARK;
		TRAP(err, doTest3L());
		TEST(err == KErrNone);
		__UHEAP_MARKEND;

		__UHEAP_MARK;
		TRAP(err, doTest4L());
		TEST(err == KErrNone);
		__UHEAP_MARKEND;

		__UHEAP_MARK;
		TRAP(err, doTest5L());
		TEST(err == KErrNone);
		__UHEAP_MARKEND;

		__UHEAP_MARK;
		TRAP(err, doTest6L());
		TEST(err == KErrNone);
		__UHEAP_MARKEND;

// OOM tests start here

		__UHEAP_MARK;
		TRAP(err, doOOMTestL(&CUtilityPluginTest::doTest1L));
		TEST(err == KErrNone);
		__UHEAP_MARKEND;

		__UHEAP_MARK;
		TRAP(err, doOOMTestL(&CUtilityPluginTest::doTest2L));
		TEST(err == KErrNone);
		__UHEAP_MARKEND;

		__UHEAP_MARK;
		TRAP(err, doOOMTestL(&CUtilityPluginTest::doTest3L));
		TEST(err == KErrNone);
		__UHEAP_MARKEND;

		__UHEAP_MARK;
		TRAP(err, doOOMTestL(&CUtilityPluginTest::doTest5L));
		TEST(err == KErrNone);
		__UHEAP_MARKEND;

		__UHEAP_MARK;
		TRAP(err, doOOMTestL(&CUtilityPluginTest::doTest6L));
		TEST(err == KErrNone);
		__UHEAP_MARKEND;

/**
 Manual test: should be uncommented to test manually.
*/
/*		
		__UHEAP_MARK;
		TRAP(err, doTest7L());
		TEST(err == KErrNone);
		__UHEAP_MARKEND;
*/		}
	else
		{
		INFO_PRINTF2(_L("SSM is not used for shutdown, IsSsmGracefulShutdown() returned: [%d] "), IsSsmGracefulShutdown());
		INFO_PRINTF2(_L("SSM is not used for shutdown, startupMode is [%d] i.e textshellmode"), startupMode);
		}

	return TestStepResult();
	}

/**
Old Test CaseID 		DS-UTILITYPLUGINS-0001
Old Test CaseID 		DS-UTILITYPLUGINS-0008
Old Test CaseID 		DS-UTILITYPLUGINS-0009
New Test CaseID 		DEVSRVS-SSPLUGINS-UTILITYPLUGINS-0001
 */

void CUtilityPluginTest::doTest1L()
	{
	INFO_PRINTF1(_L("**** Test for ESwitchOff event **** "));
	TestNotificationAndPowerModeL(TRawEvent::ESwitchOff, ETrue, EPwStandby);
	SwitchOn();
	SetDefaultTestKey();
	}

/**
Old Test CaseID 		DS-UTILITYPLUGINS-0002
Old Test CaseID 		DS-UTILITYPLUGINS-0008
Old Test CaseID 		DS-UTILITYPLUGINS-0009
New Test CaseID 		DEVSRVS-SSPLUGINS-UTILITYPLUGINS-0001
 */

void CUtilityPluginTest::doTest2L()
	{
	INFO_PRINTF1(_L("**** Test for ERestartSystem event **** "));
	TestNotificationAndPowerModeL(TRawEvent::ERestartSystem, ETrue, EPwRestart);
	SwitchOn();
	SetDefaultTestKey();
	}

/**
Old Test CaseID 		DS-UTILITYPLUGINS-0003
Old Test CaseID 		DS-UTILITYPLUGINS-0008
Old Test CaseID 		DS-UTILITYPLUGINS-0009
New Test CaseID 		DEVSRVS-SSPLUGINS-UTILITYPLUGINS-0001
 */

void CUtilityPluginTest::doTest3L()
	{
	INFO_PRINTF1(_L("**** Test for ESwitchOff event **** "));
	TestNotificationAndPowerModeL(TRawEvent::ESwitchOff, ETrue, EPwStandby);
	SwitchOn();
	SetDefaultTestKey();
	}

/**
Old Test CaseID 		DS-UTILITYPLUGINS-0004
Old Test CaseID 		DS-UTILITYPLUGINS-0005
New Test CaseID 		DEVSRVS-SSPLUGINS-UTILITYPLUGINS-0001
 */

void CUtilityPluginTest::doTest4L()
	{
	INFO_PRINTF1(_L("**** Test for ECaseClose event **** "));
	TestNotificationAndPowerModeL(TRawEvent::ECaseClose, EFalse, EPwActive);
	SwitchOn();
	SetDefaultTestKey();
	}

/**
Old Test CaseID 		DS-UTILITYPLUGINS-0007
New Test CaseID 		DEVSRVS-SSPLUGINS-UTILITYPLUGINS-0001
 */

void CUtilityPluginTest::doTest5L()
	{
	INFO_PRINTF1(_L("**** Test delaying system shutdown **** "));
	TestNotificationAndPowerModeL(TRawEvent::ESwitchOff, ETrue, EPwStandby, ETrue);
	SwitchOn();
	SetDefaultTestKey();
	}

/**
Old Test CaseID 		DS-UTILITYPLUGINS-0006
New Test CaseID 		DEVSRVS-SSPLUGINS-UTILITYPLUGINS-0001
 */

void CUtilityPluginTest::doTest6L()
	{
	INFO_PRINTF1(_L("**** Test Cancel system shutdown transition **** "));
	TestNotificationAndPowerModeL(TRawEvent::ESwitchOff, ETrue, EPwStandby, EFalse, ETrue);
	SwitchOn();
	SetDefaultTestKey();
	}

/**
Old Test CaseID 		DS-UTILITYPLUGINS-0010
New Test CaseID 		DEVSRVS-SSPLUGINS-UTILITYPLUGINS-0001
 */

void CUtilityPluginTest::doTest7L()
	{
	INFO_PRINTF1(_L("**** Test system shutdown transition without acknowledgement **** "));
	TestSystemShutdownWithoutAcknowledgementL();
	SwitchOn();
	SetDefaultTestKey();
	}

/**
Used for Manual test alongwith doTest7L().
Helper function which doesn't acknowledge a shutdown event notification.
*/
void CUtilityPluginTest::TestSystemShutdownWithoutAcknowledgementL()
	{
	//Step1: create state aware session
	RSsmStateAwareSession stateAwareSession;
	CleanupClosePushL(stateAwareSession); // This R-Class might leak memory in event of a leave
	User::LeaveIfError(stateAwareSession.Connect(KUIFrameworkDomain3));
	TRequestStatus status;
	stateAwareSession.RequestStateNotification(status);

	//Step2: connect to wserv 
	RWsSession wsSession;
	User::LeaveIfError(wsSession.Connect());
	//and generate the event
	TRawEvent wsEvent;
	wsEvent.Set(TRawEvent::ESwitchOff);
	wsSession.SimulateRawEvent(wsEvent);
	wsSession.Flush();
	wsSession.Close();
	INFO_PRINTF2(_L("Generated Raw event : [%d]"), TRawEvent::ESwitchOff);

	//Step3: don't acknowledge when state change notification is received
	TSsmState state = stateAwareSession.State();
	for (;;)
		{
		// Wait for next notification of state change
		User::WaitForRequest(status);
		TEST(status.Int() == KErrNone);
		state = stateAwareSession.State();
		TSsmStateName name = state.Name();
		//stateAwareSession.AcknowledgeStateNotification(KErrNone);
		INFO_PRINTF2(_L("Transtition undergoing for the state.substate : [%S]"), &name);
		INFO_PRINTF1(_L("Will remain here till timeout occurs and system moves to Fail State"));
		}

	//Avoid "Warning:  #111-D: statement is unreachable"
	//This statement though required will not be executed as the above for loop will wait forever and a timeout will lead the system to move to Fail state.
	//stateAwareSession.Close();
	//CleanupStack::PopAndDestroy(); //stateAwareSession
	}

/**
Helper function which prints a delay 10 times.
*/
void CUtilityPluginTest::LogDelayAcknowledgement()
	{
	for (TInt i=0; i<10;)
		{
		INFO_PRINTF2(_L("***** Delaying Acknowledgement [%d] time(s)"), ++i);
		}
	}

/**
Helper function which performs following actions
1. Create a CTimer. Set timer to 10 seconds.
2. When timer expires RunL should publish test RProperty to '0xBAD'
3. Get the value and check if its set as expected.
4. Reset the key to default.
5. Cleanup timer.
*/
void CUtilityPluginTest::TestStateChangeNotificationNotReceivedL()
	{
	CShutdownNotifObserverTimer* timer = CShutdownNotifObserverTimer::NewL();
	CleanupStack::PushL(timer);
	timer->StartTimer(KWaitTimer);

	TInt value = 0;
	User::LeaveIfError(RProperty::Get(TestCategory, TestKey, value));
	if (0xBAD == value)
		{
		INFO_PRINTF1(_L("No Notification received as no state change occurred"));
		}
	else
		{
		INFO_PRINTF1(_L(">>>>>>>>>>>>>> Should not be here."));
		TEST(EFalse);
		}
	SetDefaultTestKey();
	CleanupStack::PopAndDestroy(timer);
	}

/**
Helper function which performs following actions
1. Create a state aware session
2. Generate an event i.e. aRawEvent
3. Check for notification depending on the aIsNotificationExpected
4. Check for the power mode set by power off command.
*/
void CUtilityPluginTest::TestNotificationAndPowerModeL(TRawEvent::TType aRawEvent, TBool aIsNotificationExpected, 
														TPowerState aExpectedPowerState, 
														TBool aDelayAcknowledgement, TBool aCancelTransition)
	{
	//Step1: create state aware session
	RSsmStateAwareSession stateAwareSession;
	CleanupClosePushL(stateAwareSession); // This R-Class might leak memory in event of a leave
	User::LeaveIfError(stateAwareSession.Connect(KUIFrameworkDomain3));
	TRequestStatus status;
	stateAwareSession.RequestStateNotification(status);

	//Step2: connect to wserv 
	RWsSession wsSession;
	User::LeaveIfError(wsSession.Connect());
	//and generate the event
	TRawEvent wsEvent;
	wsEvent.Set(aRawEvent);
	wsSession.SimulateRawEvent(wsEvent);
	wsSession.Flush();
	wsSession.Close();
	INFO_PRINTF2(_L("Generated Raw event : [%d]"), aRawEvent);

	//Step3: check for notification depending on the aIsNotificationExpected
	TSsmState state = stateAwareSession.State();
	while (aIsNotificationExpected)
		{
		if (aCancelTransition)
			{
			RSsmStateManager stateMgr;
			User::LeaveIfError(stateMgr.Connect());
			INFO_PRINTF1(_L("Cancelling Shutdown State Transtition"));
			stateMgr.RequestStateTransitionCancel();
			stateMgr.Close();
			}

		// Wait for next notification of state change
		User::WaitForRequest(status);
		TEST(status.Int() == KErrNone);
		state = stateAwareSession.State();
		TSsmStateName name;
		if ((state.MainState() == ESsmShutdown) && ((state.SubState() == ESsmShutdownSubStatePowerOff) || (state.SubState() == ESsmShutdownSubStateReStart)))
			{
			if (aDelayAcknowledgement)
				{
				LogDelayAcknowledgement();
				}
			stateAwareSession.AcknowledgeStateNotification(KErrNone);
			TEST((state.SubState() == ESsmShutdownSubStatePowerOff) || (state.SubState() == ESsmShutdownSubStateReStart));
			name = state.Name();
			INFO_PRINTF2(_L("Transtition undergoing for the state.substate : [%S]"), &name);
			break;	
			}
		else
			{
			if (aDelayAcknowledgement)
				{
				LogDelayAcknowledgement();
				}
			stateAwareSession.AcknowledgeAndRequestStateNotification(KErrNone, status);
			TEST((state.SubState() == ESsmShutdownSubStateCritical) || (state.SubState() == ESsmShutdownSubStateNonCritical));
			name = state.Name();
			INFO_PRINTF2(_L("Transtition undergoing for the state.substate : [%S]"), &name);
			}
		}

	INFO_PRINTF3(_L("Shutdown event received [%d] and expected is [%d]"), state.MainState() == ESsmShutdown, aIsNotificationExpected);

	if (aIsNotificationExpected)
		{
		TEST(state.MainState() == ESsmShutdown);
		TInt value = 0;
		RProperty prop;
		TRequestStatus propertyStatus;
		User::LeaveIfError(prop.Get(TestCategory, TestKey, value));
		if (aExpectedPowerState != (TPowerState)value)
			{
			User::LeaveIfError(prop.Attach(TestCategory, TestKey));
			prop.Subscribe(propertyStatus);
			User::WaitForRequest(propertyStatus);
			User::LeaveIfError(prop.Get(TestCategory, TestKey, value));
			}
		//Step4: Test the power mode
		TEST(aExpectedPowerState = (TPowerState)value);
		INFO_PRINTF3(_L("Power state received is [%d] and expected was [%d]"), value, aExpectedPowerState);		
		}
	else
		{
		// run timer and wait for 10 seconds to see if any notification is received
		TestStateChangeNotificationNotReceivedL();
		stateAwareSession.RequestStateNotificationCancel();
		}

	CleanupStack::PopAndDestroy(); //stateAwareSession
	}

/**
Helper function to switch on the Display, Keyboard and PenState. This will be switched off in case of stand-by
*/
void CUtilityPluginTest::SwitchOn()
	{
	HAL::Set(HAL::EDisplayState, 1);
	HAL::Set(HAL::EKeyboardState, 1);
	HAL::Set(HAL::EPenState, 1);
	}

/**
Helper function to do the OOM testing
*/
void CUtilityPluginTest::doOOMTestL(ClassFuncPtrL aTestFuncL/*, const TDesC& aTestDesc*/)
	{
	INFO_PRINTF1(_L("*** Starting OOM test"));
	TInt failRate = 1;

	for (; ;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;
	
	    TRAPD(ret, (this->*aTestFuncL)());

        TEST((ret==KErrNone || ret==KErrNoMemory));
        __UHEAP_MARKEND;

	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("*** OOM Test finished with #allocs : %d."), failRate-1);
	}

/**
Helper function to set the default test key to TestDefaultValue
*/
void CUtilityPluginTest::SetDefaultTestKey()
	{
	TInt err = RProperty::Set(TestCategory, TestKey, TestDefaultValue);
	INFO_PRINTF3(_L("Default power state set to TPowerState::EPwActive(%d), Published with error(%d)"), TestDefaultValue, err);
	}

 TInt CUtilityPluginTest::GetStartUpModeL()
	{
	TInt startUpMode;
	TInt err;
	TRAP(err,RProperty::Get(KUidSystemCategory, KSystemStartupModeKey, startUpMode));
	TEST(err == KErrNone); 
	return startUpMode;
	}
