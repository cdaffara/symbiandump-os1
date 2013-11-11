// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cmnpanic.h"
#include <ssm/ssmstate.h>
#include <ssm/ssmstatetransition.h>

#include "tcmn_step_state.h"
#include "ssmatest_utils.h"


TInt PanicTestThread(TAny* aOption);

enum TPanicOption
	{
	EFirst,
	EConstructor1,
	EConstructor2,
	EConstructor3,
	ESet1,
	ESet2,
	ESetFromInt1,
	ESetFromInt2,
	ELast
	};


CCmnStateTest::~CCmnStateTest()
	{
	}

CCmnStateTest::CCmnStateTest()
	{
	SetTestStepName(KTCmnStateStep);
	}

TVerdict CCmnStateTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CCmnStateTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CCmnStateTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CCmnStateTest started..."));

	__UHEAP_MARK;
	doTestTSsmState();
	doTestTSsmStateTransition();
	__UHEAP_MARKEND;

	// this test raises panics due to negative testing - close them to clear the screen.
	CloseAllPanicWindowsL();

	INFO_PRINTF1(_L("....CCmnStateTest completed!"));
	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-CMN-0001
New Test CaseID 		DEVSRVS-SSMA-CMN-0001
 */

void CCmnStateTest::doTestTSsmState()
	{
	INFO_PRINTF1(_L("Entering test for TSsmState"));
	doTestTSsmStateConstructors();
	doTestTSsmStateOperators();
	doTestTSsmStateOtherMethods();
	doTestTSsmStatePanic();
	}

void CCmnStateTest::doTestTSsmStateConstructors()
	{
	INFO_PRINTF1(_L("Testing TSsmState default constructor"));
	TSsmState s0;
	TEST(s0.Int() == 0);
	TEST(s0.MainState() == 0);
	TEST(s0.SubState() == 0);
	
	INFO_PRINTF1(_L("Lower bound test for TSsmState constructor with two arguments"));
	TSsmState s1(1,1);
	TEST(s1.Int() == 0x00010001);
	TEST(s1.MainState() == 1);
	TEST(s1.SubState() == 1);
	
	INFO_PRINTF1(_L("Upper bound test for TSsmState constructor with two arguments"));
	TSsmState sF(0x00FF, 0xFFFF);
	TEST(sF.Int() == 0x00FFFFFF);
	TEST(sF.MainState() == 0x00FF);
	TEST(sF.SubState() == 0xFFFF);
	
	INFO_PRINTF1(_L("Testing TSsmState copy constructor"));
	const TSsmState KState(0x00FE, 0xBABE);
	TSsmState copy(KState);
	TEST(copy.Int() == KState.Int());
	TEST(copy.MainState() == KState.MainState());
	TEST(copy.SubState() == KState.SubState());
	
	INFO_PRINTF1(_L("Testing TSsmState enum constructor"));
	TSsmState enum0(ESsmNormal, KSsmAnySubState);
	TEST(enum0.MainState() == 1);
	TEST(enum0.SubState() == 0xFFFF);
	}
	
void CCmnStateTest::doTestTSsmStateOperators()
	{
	INFO_PRINTF1(_L("Testing TSsmState operators, Int() and SetFromInt(TUint)"));
	TSsmState a;
	a.SetFromInt(0x00DEBEEF);
	TSsmState b;
	b.SetFromInt(0x00ADF00D);
	
	TSsmState c = a;
	TEST(c.Int() == a.Int());
	TEST(c == a);
	
	a = b;
	TEST(c.Int() != a.Int());
	TEST(b.Int() == a.Int());
	TEST(b == a);
	
	INFO_PRINTF1(_L("Testing TSsmState operators with 'KSsmAnySubState'"));

	// at this point ...
	//   'c' == 0x00DE, 0xBEEF
	//   'b' == 'a' == 0x00AD, 0xFOOD
	
	TSsmState subAny1(0x00DE, KSsmAnySubState);
	TSsmState subAny2(0x00DE, KSsmAnySubState);
	
	// identical content
	TEST(subAny1 == subAny2);
	TEST(subAny2 == subAny1);

	// identical major, sub is irrelevant in at least one object
	TEST(subAny1 == c);
	TEST(c == subAny1);
	
	// different major, sub is irrelevant in at least one object
	TEST(subAny1 != b);
	TEST(b != subAny1);
	
	// construct and assign
	subAny2 = TSsmState(0x00AD, KSsmAnySubState);
	
	// identical major, sub is irrelevant in at least one object
	TEST(subAny2 == b);
	TEST(b == subAny2);

	// different major, sub is irrelevant in at least one object
	TEST(subAny2 != c);
	TEST(c != subAny2);
	
	//cover protection against self assignment and make sure we are not trapped in a loop
	subAny2 = subAny2;
	}
	
void CCmnStateTest::doTestTSsmStateOtherMethods()
	{
	INFO_PRINTF1(_L("Testing remaining methods in TSsmState"));
	
	TSsmState a(0x00FF, 0xFFFF);
	TSsmStateName name = a.Name();
	TEST(name == _L("00ff.ffff"));
	
	TSsmState b(0x0001, 0x0001);
	name = b.Name();
	TEST(name == _L("0001.0001"));
	
	b.Set(0x00FF, 0xFFFF);
	TEST(a == b);
	}

void CCmnStateTest::doTestTSsmStatePanic()
	{
	INFO_PRINTF1(_L("Testing TSsmState panics in debug mode if State is bigger to 8 bits"));

	for (TInt opt = EFirst + 1; opt < ELast; opt++)
		{
		StartThread(opt);
		}
	}


void CCmnStateTest::StartThread(TInt aOption)
	{
	RThread thread;
	TRequestStatus stat;
	TBuf<32> threadNameBuf;
	// Give each thread a unique name to avoid KErrAlreadyExists error on thread creation
	_LIT(KThreadNameFormat, "CCmnStateTest%d");
	threadNameBuf.Format(KThreadNameFormat, aOption);
	TInt threadCreationVal = thread.Create(threadNameBuf,PanicTestThread,KDefaultStackSize,0x2000,0x20000,(TAny*)aOption);
	TEST(threadCreationVal==KErrNone);	

	TRequestStatus status;
	thread.Logon(status);
	TBool jit =	User::JustInTime();
	User::SetJustInTime(EFalse);
	thread.Resume();
	User::WaitForRequest(status);

	// we are always expecting the same panic category KPanicSsmCmn only in debug (doesn't panic in release -> hw testing)
	TExitCategoryName category = thread.ExitCategory();
#ifdef __WINS__
	TEST(category.Compare(KPanicSsmCmn) == 0);
	INFO_PRINTF4(_L("  *** Case %d: Panic ExitCategory is %S (expected was %S)"), aOption, &category, &KPanicSsmCmn);
#else
	TEST(category.Compare(_L("Kill")) == 0);
	INFO_PRINTF2(_L("  *** category = %S ***"),&category);
#endif	
	
	// we are always expecting the same panic reason in the format ECmnStateMaxValueX (doesn't panic in release -> hw testing)
	const TInt exitReason = thread.ExitReason();
#ifdef __WINS__
	TEST(exitReason >= ECmnStateMaxValue1);
	TEST(exitReason <= ECmnStateMaxValue4);
	INFO_PRINTF5(_L("  *** Case %d: Panic reason is %d (expected was in range [%d, %d]"), aOption, exitReason, ECmnStateMaxValue1, ECmnStateMaxValue4);
#else
	TEST(exitReason == KErrNone);
#endif	
	thread.Close();
	User::SetJustInTime(jit);
	}

TInt PanicTestThread(TAny* aOption)
	{
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	TInt option = (TInt)aOption;
	switch(option)
		{
		case EConstructor1:
			{
			TSsmState state(0x100, 0x0);
			break;
			}
		case EConstructor2:
			{
			TSsmState state(0xFFFF, 0x0);
			break;
			}
		case EConstructor3:
			{
			TSsmState state(static_cast<TSsmMainSystemStates>(0xFFF), 0xFFF);
			break;
			}
		case ESet1:
			{
			TSsmState state;
			state.Set(0x1000, 0xBEEF);
			break;
			}
		case ESet2:
			{
			TSsmState state;
			state.Set(0xFF00, 0xBEEF);
			break;
			}
		case ESetFromInt1:
			{
			TSsmState state;
			state.SetFromInt(0x1000000);
			break;
			}
		case ESetFromInt2:
			{
			TSsmState state;
			state.SetFromInt(0xFFFF0000);
			break;
			}
		default:
			User::Panic(_L("Default panic"), KErrGeneral);
		}			

	delete trapCleanup;
	return KErrNone;
	}



/**
 Old Test CaseID 		APPFWK-CMN-0002
 New Test CaseID 		DEVSRVS-SSMA-CMN-0001
 */
void CCmnStateTest::doTestTSsmStateTransition()
	{
	INFO_PRINTF1(_L("Entering test for TSsmStateTransition"));

	INFO_PRINTF1(_L("Testing TSsmStateTransition default constructor"));
	TSsmStateTransition s0;
	TEST(s0.State().Int() == 0);
	TEST(s0.Reason() == 0);

	INFO_PRINTF1(_L("Testing TSsmStateTransition constructor with 2 arguments"));
	const TSsmState KState(0x00FE, 0xBABE);
	TSsmStateTransition s1(KState, KMaxTInt);
	TEST(s1.State() == KState);
	TEST(s1.Reason() == KMaxTInt);
	
	INFO_PRINTF1(_L("Testing TSsmStateTransition constructor with 3 arguments"));
	TSsmStateTransition s2(0x00FE, 0xBABE, KMaxTInt);
	TEST(s2.State() == KState);
	TEST(s2.Reason() == KMaxTInt);
	
	INFO_PRINTF1(_L("Testing TSsmStateTransition copy constructor and operator =="));
	TSsmStateTransition copy(s1);
	TEST(s1 == copy);
	
	INFO_PRINTF1(_L("Testing TSsmStateTransition operator !="));
	TEST(copy != s0);
	
	INFO_PRINTF1(_L("Testing TSsmStateTransition operator ="));
	s0 = s1;
	s0 = s0;
	TEST(copy == s0);
	}

